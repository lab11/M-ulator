//*******************************************************************
//Author: ZhiYoong Foo
//Description: Si7210 Initial Testing
//*******************************************************************
#include "PREv18.h"
#include "PREv18_RF.h"
#include "mbus.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// System parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

#define GPIO_ADXL_INT 2
#define GPIO_ADXL_EN 4
#define GPIO_SDA 1
#define GPIO_SCL 0

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t stack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t exec_count_irq;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_sar_conv_ratio_val;
volatile uint32_t read_data_batadc;
volatile uint32_t sleep_time_prev;
volatile uint32_t sleep_time_threshold_factor;
volatile uint32_t wakeup_period_calc_factor;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_SNT; 

volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t read_data_temp;

volatile uint32_t wakeup_timer_option;
volatile uint32_t snt_wup_counter_cur;
volatile uint32_t snt_timer_enabled = 0;
volatile uint32_t SNT_0P5S_VAL;

volatile uint32_t sns_running;
volatile uint32_t set_sns_exec_count;

volatile uint32_t astack_detection_mode;
volatile uint32_t adxl_enabled;
volatile uint32_t adxl_motion_detected;
volatile uint32_t adxl_user_threshold;
volatile uint32_t adxl_motion_trigger_count;
volatile uint32_t adxl_motion_count;
volatile uint32_t adxl_trigger_mute_count;
volatile uint32_t adxl_mask = (1<<GPIO_ADXL_INT) | (1<<GPIO_ADXL_EN);

volatile uint32_t sht35_temp_data, sht35_hum_data;
volatile uint32_t si7210_mask = (1<<GPIO_SDA) | (1<<GPIO_SCL);
volatile uint32_t sht35_user_repeatability; // Default 0x0B


volatile uint32_t temp_alert;

volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_freq_hopping_step;
volatile uint32_t mrr_cfo_val_fine_min;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
volatile prev18_r1C_t prev18_r1C = PREv18_R1C_DEFAULT;

//***************************************************
// Si7210 Functions
//***************************************************
static void operation_i2c_strt(void);
static void operation_i2c_stop(void);
static void operation_i2c_addr(uint8_t RWn);
static void operation_i2c_cmd(uint8_t cmd);
static uint8_t operation_i2c_read();

//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17)
//*******************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
	wfi_timeout_flag = 1;
    }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
//[ 0] = GOC/EP
//[ 1] = Wakeuptimer
//[ 2] = XO timer
//[ 3] = gpio_pad
//[ 4] = mbus message
//[ 8] = gpio[0]
//[ 9] = gpio[1]
//[10] = gpio[2]
//[11] = gpio[3]
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
	// Report who woke up
	delay(MBUS_DELAY);
	mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
}



//***************************************************
// Si7210 Functions (I2C Bit-Bang)
//***************************************************

//Write Register
//START --> ADDR --> !W --> !ACK --> REGISTER --> !ACK --> DATA --> !ACK --> STOP
//
//Read Register
//START --> ADDR --> !W --> !ACK --> REGISTER --> !ACK --> SR --> ADDR --> R --> DATA --> NACK --> STOP

static void operation_i2c_strt(){
  // Assume SCL: X
  // Assume SDA: X
  //
  //     X+---+---+
  // SCL X        |
  //     X        +-
  //
  //     X+---+
  // SDA X    |
  //     X    +---+-
  //
  
  // Enable GPIO OUTPUT
  *GPIO_DATA = 3;
  set_gpio_pad_with_mask   (si7210_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_set_dir_with_mask   (si7210_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  unfreeze_gpio_out();
  //Start
  *GPIO_DATA = 1;
  *GPIO_DATA = 0;
}

static void operation_i2c_stop(){
  // Assume SCL: 0
  // Assume SDA: X
  //
  //     X    +---+---+-
  // SCL X    |   
  //     X+---+   
  //
  //     X        +---+-   
  // SDA X        |
  //     X+---+---+  
  //
  
  // Stop
  gpio_set_dir_with_mask   (si7210_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  *GPIO_DATA = 0;
  *GPIO_DATA = 1;
  *GPIO_DATA = 3;
}

static void operation_i2c_addr(uint8_t RWn){
  // Assume ADDR: 0x30 for Si7210
  // Assume operation_i2c_strt was ran before this
  // Assume SCL: 0
  // Assume SDA: 0
  //
  //          +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+
  // SCL      |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
  //     -+---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +-
  //
  //      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+-
  // SDA  |ADDR[6]|ADDR[5]|ADDR[4]|ADDR[3]|ADDR[2]|ADDR[1]|ADDR[0]| !W/R  | !ACK  |
  //     -+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  //
  // SDA: 0x30
  //              +---+---+---+---+                               +---+---+---+---+-
  // SDA          |               |                               | !W/R  | !ACK  |
  //     -+---+---+               +---+---+---+---+---+---+---+---+---+---+---+---+
  //

  // ADDR[6]
  *GPIO_DATA = 1;
  // ADDR[5]
  *GPIO_DATA = 2;
  *GPIO_DATA = 3;
  // ADDR[4]
  *GPIO_DATA = 2;
  *GPIO_DATA = 3;
  // ADDR[3]
  *GPIO_DATA = 0;
  *GPIO_DATA = 1;
  // ADDR[2]
  *GPIO_DATA = 0;
  *GPIO_DATA = 1;
  // ADDR[1]
  *GPIO_DATA = 0;
  *GPIO_DATA = 1;
  // ADDR[0]
  *GPIO_DATA = 0;
  *GPIO_DATA = 1;

  // !W/R
  if((RWn&0x1) == 1){ //Need hack
    gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
    gpio_set_dir_with_mask(si7210_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
    gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  }
  else{
    gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
  }

  // Wait for !ACK
  // Change SDA Direction to input (PCB Pull-up Resistor will pull high)
  // Si7210 should pull to 0 if !ACK
  gpio_set_dir_with_mask(si7210_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  while((*GPIO_DATA>>GPIO_SDA)&0x1){
    //mbus_write_message32(0xCE, *GPIO_DATA);
  }
  gpio_write_data_with_mask(si7210_mask, 0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask, 1<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask, 0<<GPIO_SCL);
}

static void operation_i2c_cmd(uint8_t cmd){
  // Assume SCL: X
  // Assume SDA: X
  //
  //     X    +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+
  // SCL X    |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
  //     X+---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +-
  //
  //     X+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  // SDA X| CMD[7]| CMD[6]| CMD[5]| CMD[4]| CMD[3]| CMD[2]| CMD[1]| CMD[0]| !ACK  |
  //     X+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+-
  //

  gpio_set_dir_with_mask(si7210_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));

  // CMD[7]
  gpio_write_data_with_mask(si7210_mask,(((cmd>>7)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(((cmd>>7)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  // CMD[6]
  gpio_write_data_with_mask(si7210_mask,(((cmd>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(((cmd>>6)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  // CMD[5]
  gpio_write_data_with_mask(si7210_mask,(((cmd>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(((cmd>>5)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  // CMD[4]
  gpio_write_data_with_mask(si7210_mask,(((cmd>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(((cmd>>4)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  // CMD[3]
  gpio_write_data_with_mask(si7210_mask,(((cmd>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(((cmd>>3)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  // CMD[2]
  gpio_write_data_with_mask(si7210_mask,(((cmd>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(((cmd>>2)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  // CMD[1]
  gpio_write_data_with_mask(si7210_mask,(((cmd>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(((cmd>>1)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(((cmd>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  // CMD[0]
  if((cmd&0x1) == 1){ //Need hack
    gpio_set_dir_with_mask(si7210_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
    gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
    gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  }
  else{
    gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
  }
  // Wait for !ACK
  // Change SDA Direction to input (PCB Pull-up Resistor will pull high)
  // Si7210 should pull to 0 if !ACK
  gpio_set_dir_with_mask(si7210_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  while((*GPIO_DATA>>GPIO_SDA)&0x1){
    //mbus_write_message32(0xCE, *GPIO_DATA);
  }
  gpio_write_data_with_mask(si7210_mask, 0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask, 1<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask, 0<<GPIO_SCL);
}

static uint8_t operation_i2c_read(){
  // Assume SCL: 0
  // Assume SDA: 0
  //
  //          +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+
  // SCL      |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
  //     -+---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +-
  //
  //      +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  // SDA  |DATA[7]|DATA[6]|DATA[5]|DATA[4]|DATA[3]|DATA[2]|DATA[1]|DATA[0]| NACK  |
  //     -+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+-
  //

  uint8_t data;
  data = 0;
  
  // DATA[7]
  gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<7);
  // DATA[6]
  gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<6);
  // DATA[5]
  gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<5);
  // DATA[4]
  gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<4);
  // DATA[3]
  gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<3);
  // DATA[2]
  gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<2);
  // DATA[1]
  gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<1);
  // DATA[0]
  gpio_write_data_with_mask(si7210_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(si7210_mask,1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<0);

  // NACK
  gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_set_dir_with_mask(si7210_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(si7210_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
  
  return data;
}



static void operation_init(void){
  
  // Set CPU & Mbus Clock Speeds
  prev18_r0B.CLK_GEN_RING = 0x1; // Default 0x1
  prev18_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
  prev18_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
  prev18_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
  prev18_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
  *REG_CLKGEN_TUNE = prev18_r0B.as_int;
  
  prev18_r1C.SRAM0_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
  prev18_r1C.SRAM0_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
  prev18_r1C.SRAM0_USE_INVERTER_SA= 0; 
  *REG_SRAM0_TUNE = prev18_r1C.as_int;
  
  
  //Enumerate & Initialize Registers
  enumerated = 0x41481190; // 0x4148 is AH in ascii
  exec_count = 0;
  exec_count_irq = 0;
  
  // Set CPU Halt Option as RX --> Use for register read e.g.
  //set_halt_until_mbus_rx();
  
  // Initialize other global variables
  WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
  // Go to sleep without timer
  //operation_sleep_notimer();
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){
  uint8_t test_rx; //Received I2C Read Data
  
  // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
  config_timerwd(TIMERWD_VAL);

  //DBG
  mbus_write_message32(0xCF, 0x00);

  // Initialization sequence
  if (enumerated != 0x41481190){
    operation_init();
  }

  // Turn on CPS
  *REG_CPS = *REG_CPS | 0x1;
  delay(1000);

  //DBG
  mbus_write_message32(0xCF, 0x01);

  //Wake device from Power Up
  operation_i2c_strt();
  operation_i2c_addr(0x0);
  operation_i2c_stop();
  mbus_write_message32(0xCF, 0x02);
  
  //Check chipid
  //ChipID on Register 0xC0
  //Should return 0x14
  operation_i2c_strt();
  operation_i2c_addr(0x0);
  operation_i2c_cmd (0xC0);
  operation_i2c_strt();
  operation_i2c_addr(0x1);
  test_rx = operation_i2c_read();
  operation_i2c_stop();
  mbus_write_message32(0xCF, test_rx);

  //Read sleep time
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xC8);
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,1);
  //operation_i2c_rd(0x0);
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xCF, 0x03);
  //
  ////Change sleep time
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xC8);
  //operation_i2c_cmd(0xFF);
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xCF, 0x04);
  //
  ////Check sleep time
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xC8);
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,1);
  //operation_i2c_rd(0x0);
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xCF, 0x03);
  //
  ////Check sleep time
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xC9);
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,1);
  //operation_i2c_rd(0x0);
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xCF, 0x03);
  //
  //
  ////while(1){
  //  
  ////operation_i2c_strt();
  ////operation_i2c_addr(0x30,0);
  ////operation_i2c_cmd(0xC4); // Temp MSB
  ////operation_i2c_cmd(0x04); // Temp MSB
  ////operation_i2c_stop();
  ////delay(100);
  ////mbus_write_message32(0xCF, 0x04);
  //
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xC1); // Temp MSB
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,1);
  //test_rx = operation_i2c_rd(0x0);
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xC1, test_rx);
  //
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xC2); // Temp MSB
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,1);
  //test_rx = operation_i2c_rd(0x0);
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xC2, test_rx);
  //
  //operation_i2c_strt();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xC4); // Temp MSB
  //operation_i2c_cmd(0x00); // Temp MSB
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xCF, 0x04);
  //
  //freeze_gpio_out();

  //}
    
  //GRAB OTP DATA
  //operation_i2c_start();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xE1); // Temp MSB
  //operation_i2c_cmd(0x1D); // Temp MSB
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xCF, 0x04);
  //
  //operation_i2c_start();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xE3); // Temp MSB
  //operation_i2c_cmd(0x02); // Temp MSB
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xCF, 0x05);
  //
  //operation_i2c_start();
  //operation_i2c_addr(0x30,0);
  //operation_i2c_cmd(0xE2); // Temp MSB
  //operation_i2c_start();
  //operation_i2c_addr(0x30,1);
  //operation_i2c_rd(0x0);
  //operation_i2c_stop();
  //delay(100);
  //mbus_write_message32(0xCF, 0x06);
    
  // Reset GOC_DATA_IRQ
  *GOC_DATA_IRQ = 0;
    
  // Go to Sleep
  mbus_sleep_all();
  while(1);
}
