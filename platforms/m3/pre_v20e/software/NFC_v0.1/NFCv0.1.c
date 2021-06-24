//*******************************************************************
//Author: Gyouho Kim
//Description: AH stack with MRRv7 and SNSv10, ADXL362, SHT35
//			Modified from 'AMstack_ondemand_v1.14.c'
//			AH Stack uses 3V battery
//			v1.14: MRRv7 - larger tuning range, stronger drive
//				   New packet definitions with CRC16
//				   SHT35 uses power switch [0]
//				   ADXL uses power switch [1]
//			v1.14b: fixes a bug in humidty/temp reading
//					fixes a bug in REG_CPS control
//					fixes CRC bug
//			v1.15: fixes ADXL / SHT35 conflict
//			v1.15b: improve latency of radio tx for motion detection
//					adding configurability for SHT35
//			v1.16: improve latency of radio tx further
//					changing MRR decap configuration and shorter SFO settling time
//			v1.17: PMUv9, replacing delays for PMU reg write with set halt
//					adding a burst mode for mrr radio scan (0x52)
//			v1.18: PREv18, SNTv1, no HRVv6
//					Getting rid of motion muting in case of SNT timer
//					Getting rid of temp sensing using SNT
//			v1.18a: Increasing loop count of mrr radio burst mode
//			v1.18c: Don't reset radio packet count every goc; radio packet delay changed to 2000
//				    SNT stabilization time reduced, MRR initial decap charge time increased
//					MRR CFO default changed to 0x0/0x0
//			v1.19: Changing radio packet definitions to increase packet count bits, and include motion count
//					Implemented reading out PUF Chip ID
//			v1.20: Adding pmu adjustment based on temp; adding wakeup counter, CL_CTRL=1 during TX
//					Increasing initial decap charging time for 22uF support
//					Run SHT35 temp sensor even in radio test mode
//					Shortened delay after ADXL power off
//					Removing PRC timer option
//			v1.20a: debugging why checkin period becomes indefinite
//					Read SNT counter val each wakeup, adding timeouts for SHT wait, mbus rx waits
//					Radio error packet when timeout
//			v1.20b: Reset SNT timer every time motion is detected
//			v1.21: PREv20, MRRv10, SNTv4
//			v1.21a: MRR Chirp fix: Use V1P2 for SFO, tune LDO voltage
//			       Make LDO output, MRR bias tunable
//			v1.22: PREv20E, Core clock 2x, Adding GOC clk tuning
//				   Using Macro for GPIO masking
//			v1.22a: Adding PMU_ADC_3P0_VAL configurability
//				   Implement pending GPIO wakeup request
//			v1.22b: Fixing bug where pending wakeup request was not reset
//			v1.22c: Moved resetting location of SREG_WAKEUP_SOURCE register
//			v1.23: Adding new MRR FSM burst mode for radio scanning
//				   Adding SNT Timer R tuning
//			v1.23b: Fixes a problem where running send_radio_data_mrr() more than once and 
//				   running radio_power_on() between them causes CL to be in weak mode
//			v1.24: PMU Optimizations
//*******************************************************************
#include "PREv20.h"
#include "PREv20_RF.h"
#include "mbus.h"
#include "SNTv4_RF.h"
#include "PMUv7_RF.h"
#include "ADXL362.h"
#include "MRRv10_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// AM stack 
#define MRR_ADDR 0x4
#define SNT_ADDR 0x5
#define PMU_ADDR 0x6

// System parameters
#define	MBUS_DELAY 200 // Amount of delay between successive messages; 200: 6-7ms

// Pstack states
#define	STK_IDLE		0x0
#define	STK_LDO		0x1
#define	STK_TEMP_START 0x2
#define	STK_TEMP_READ  0x3
#define	STK_HUM     0x4

#define    PMU_0C 0x0
#define    PMU_10C 0x1
#define    PMU_15C 0x2
#define    PMU_20C 0x3
#define    PMU_25C 0x4
#define    PMU_30C 0x5
#define    PMU_35C 0x6
#define    PMU_40C 0x7
#define    PMU_45C 0x8
#define    PMU_55C 0x9
#define    PMU_65C 0xA
#define    PMU_75C 0xB
#define    PMU_85C 0xC
#define    PMU_95C 0xD


// Radio configurations
#define RADIO_DATA_LENGTH 192
#define WAKEUP_PERIOD_RADIO_INIT 0xA // About 2 sec (PRCv17)

#define TEMP_NUM_MEAS 1

//#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMERWD_VAL 0xFFFFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

#define GPIO_SCL 0
#define GPIO_SDA 1
#define GPIO_GPO 2

#define SHT35_MASK (1<<GPIO_SDA) | (1<<GPIO_SCL)
#define NFC_MASK (1<<GPIO_SDA) | (1<<GPIO_SCL)
#define GPO_MASK (1<<GPIO_GPO)

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t wakeup_source;
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t stack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t error_code;
volatile uint32_t exec_count;
volatile uint32_t wakeup_count;
volatile uint32_t exec_count_irq;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_setting_state;
volatile uint32_t pmu_sar_conv_ratio_val;
volatile uint32_t read_data_batadc;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_SNT; 

volatile uint32_t snt_wup_counter_cur;
volatile uint32_t snt_timer_enabled = 0;
volatile uint32_t SNT_0P5S_VAL;

volatile uint32_t sns_running;

volatile uint32_t astack_detection_mode;
volatile uint32_t adxl_enabled;
volatile uint32_t adxl_motion_detected;
volatile uint32_t adxl_user_threshold;
volatile uint32_t adxl_motion_count;
volatile uint32_t adxl_trigger_mute_count;

volatile uint32_t sht35_temp_data, sht35_hum_data, sht35_cur_temp;
volatile uint32_t sht35_user_repeatability; // Default 0x0B

volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_freq_hopping_step;
volatile uint32_t mrr_cfo_val_fine_min;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

volatile sntv4_r03_t sntv4_r03 = SNTv4_R03_DEFAULT;
volatile sntv4_r08_t sntv4_r08 = SNTv4_R08_DEFAULT;
volatile sntv4_r09_t sntv4_r09 = SNTv4_R09_DEFAULT;
volatile sntv4_r0A_t sntv4_r0A = SNTv4_R0A_DEFAULT;
volatile sntv4_r17_t sntv4_r17 = SNTv4_R17_DEFAULT;

volatile prev20_r0B_t prev20_r0B = PREv20_R0B_DEFAULT;
volatile prev20_r1B_t prev20_r1B = PREv20_R1B_DEFAULT;
volatile prev20_r1C_t prev20_r1C = PREv20_R1C_DEFAULT;

//***************************************************
// SHT35 Functions
//***************************************************
static void operation_i2c_start(void);
static void operation_i2c_stop(void);
static void operation_i2c_addr(uint8_t addr, uint8_t RWn);
static void operation_i2c_cmd(uint8_t cmd);
static uint8_t operation_i2c_rd(uint8_t ACK);

//***************************************************
// Sleep Functions
//***************************************************
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);


//***************************************************
// Timeout Functions
//***************************************************

static void set_timer32_timeout(uint32_t val){
	// Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
	config_timer32(val, 1, 0, 0);
}

static void stop_timer32_timeout_check(uint32_t code){
	// Turn off Timer32
	*TIMER32_GO = 0;
	if (wfi_timeout_flag){
		wfi_timeout_flag = 0;
		error_code = code;
		mbus_write_message32(0xFA, error_code);
	}
}


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
    set_halt_until_mbus_tx();
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
  // Copy Wakeup Source
  wakeup_source = *SREG_WAKEUP_SOURCE;
  // Need to reset SREG_WAKEUP_SOURCE since pending wakeup is used	
  *SCTR_REG_CLR_WUP_SOURCE = 0;
  *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
  
  // Report who woke up
  delay(MBUS_DELAY);
  mbus_write_message32(0xAA,wakeup_source); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
}


//***************************************************
// SHT35 Functions (I2C Bit-Bang)
//***************************************************

static void operation_i2c_start(){
  // Enable GPIO OUTPUT
  gpio_write_data_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  set_gpio_pad_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_set_dir_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  unfreeze_gpio_out();
  //Start
  gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (0<<GPIO_SCL));
}

static void operation_i2c_stop(){
  // Stop
  gpio_set_dir_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
}

static void operation_i2c_addr(uint8_t addr, uint8_t RWn){
  //Assume started
  //[6]
  gpio_set_dir_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>6)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[5]
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>5)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[4]
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>4)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[3]
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>3)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[2]
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>2)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[1]
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>1)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[0]
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>0)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>0)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((addr>>0)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  
  //WRITEn/READ
  //Need Hack
  if((RWn&0x1) == 1){ //Need hack
    gpio_set_dir_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
    gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
    gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
  }
  else{
    gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (0<<GPIO_SCL));
  }

  //Wait for ACK
  gpio_set_dir_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  while((*GPIO_DATA>>GPIO_SDA)&0x1){
    //mbus_write_message32(0xCE, *GPIO_DATA);
  }
  gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
  gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
  gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
}

static void operation_i2c_cmd(uint8_t cmd){
  gpio_set_dir_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  //[7]
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>7)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>7)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>7)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[6]
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>6)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[5]
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>5)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[4]
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>4)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[3]
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>3)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[2]
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>2)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[1]
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>1)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(SHT35_MASK,(((cmd>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[0]
  if((cmd&0x1) == 1){ //Need hack
    gpio_set_dir_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
    gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
    gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
  }
  else{
    gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (0<<GPIO_SCL));
  }
  
  //Wait for ACK
  gpio_set_dir_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  // Use timer32 as timeout counter
  set_timer32_timeout(TIMER32_VAL);
  
  while((*GPIO_DATA>>GPIO_SDA)&0x1){
    //mbus_write_message32(0xCF, *GPIO_DATA);
  }
  stop_timer32_timeout_check(0x5);
  gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
  gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
  gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
}

static uint8_t operation_i2c_rd(uint8_t ACK){

	uint8_t data;
	data = 0;
	gpio_set_dir_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
	//[7]
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<7);
	//[6]
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<6);
	//[5]
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<5);
	//[4]
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<4);
	//[3]
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<3);
	//[2]
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<2);
	//[1]
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<1);
	//[0]
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<0);
	gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	if (ACK&0x1){
		gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (0<<GPIO_SCL));
		gpio_set_dir_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
		gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (1<<GPIO_SCL));
		gpio_write_data_with_mask(SHT35_MASK,(0<<GPIO_SDA) | (0<<GPIO_SCL));
	}
	else{
		gpio_set_dir_with_mask(SHT35_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
		gpio_write_data_with_mask(SHT35_MASK,1<<GPIO_SCL);
		gpio_write_data_with_mask(SHT35_MASK,0<<GPIO_SCL);
	}
	// Debug
	//mbus_write_message32(0xCF, data);
	return data;
}


static void sht35_meas_data(){
	uint8_t i2c_data_rx;
	// Power on
	*REG_CPS = *REG_CPS | 0x2;
	delay(MBUS_DELAY*2);
	// Start measurement
	operation_i2c_start();
	operation_i2c_addr(0x44,0);
	operation_i2c_cmd(0x24);
	operation_i2c_cmd(sht35_user_repeatability); // Default 0x0B
	operation_i2c_stop();

	delay(MBUS_DELAY*2); // about 10ms delay

	// Read
	operation_i2c_start();
	operation_i2c_addr(0x44,0x1);
	i2c_data_rx = operation_i2c_rd(0x1); // Temp MSB
	sht35_temp_data = i2c_data_rx<<8;
	i2c_data_rx = operation_i2c_rd(0x1); // Temp LSB
	sht35_temp_data = sht35_temp_data | i2c_data_rx;
	i2c_data_rx = operation_i2c_rd(0x1); // CRC
	i2c_data_rx = operation_i2c_rd(0x1); // Hum MSB
	sht35_hum_data = i2c_data_rx<<8;
	i2c_data_rx = operation_i2c_rd(0x1); // Hum LSB
	sht35_hum_data = sht35_hum_data | i2c_data_rx;
	i2c_data_rx = operation_i2c_rd(0x0); // CRC
	operation_i2c_stop();
	*REG_CPS = *REG_CPS & (~0x2);
}

//************************************
// PMU Related Functions
//************************************

static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
	
	set_timer32_timeout(TIMER32_VAL);
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    set_halt_until_mbus_tx();
	stop_timer32_timeout_check(0x7);
}


static void pmu_set_sar_override(uint32_t val){
	// SAR_RATIO_OVERRIDE
    pmu_reg_write(0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
    pmu_reg_write(0x05, //default 12'h000
		( (1 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
}

inline static void pmu_set_adc_period(uint32_t val){
	// PMU_CONTROLLER_DESIRED_STATE Active
	// Updated for PMUv9
	pmu_reg_write(0x3C,
		((  1 << 0) //state_sar_scn_on
		| (0 << 1) //state_wait_for_clock_cycles
		| (1 << 2) //state_wait_for_time
		| (1 << 3) //state_sar_scn_reset
		| (1 << 4) //state_sar_scn_stabilized
		| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
		| (1 << 6) //state_clock_supply_switched
		| (1 << 7) //state_control_supply_switched
		| (1 << 8) //state_upconverter_on
		| (1 << 9) //state_upconverter_stabilized
		| (1 << 10) //state_refgen_on
		| (0 << 11) //state_adc_output_ready
		| (0 << 12) //state_adc_adjusted
		| (0 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read
		| (1 << 20) //state_state_horizon
	));

	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    pmu_reg_write(0x36,val); 
	delay(MBUS_DELAY*10);

	// Register 0x33: TICK_ADC_RESET
	pmu_reg_write(0x33,2);

	// Register 0x34: TICK_ADC_CLK
	pmu_reg_write(0x34,2);

	// PMU_CONTROLLER_DESIRED_STATE Active
	pmu_reg_write(0x3C,
		((  1 << 0) //state_sar_scn_on
		| (1 << 1) //state_wait_for_clock_cycles
		| (1 << 2) //state_wait_for_time
		| (1 << 3) //state_sar_scn_reset
		| (1 << 4) //state_sar_scn_stabilized
		| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
		| (1 << 6) //state_clock_supply_switched
		| (1 << 7) //state_control_supply_switched
		| (1 << 8) //state_upconverter_on
		| (1 << 9) //state_upconverter_stabilized
		| (1 << 10) //state_refgen_on
		| (0 << 11) //state_adc_output_ready
		| (0 << 12) //state_adc_adjusted
		| (0 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read
		| (1 << 20) //state_state_horizon
	));
}

inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

	// The first register write to PMU needs to be repeated
	// Register 0x16: V1P2 Active
    pmu_reg_write(0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
    pmu_reg_write(0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x18: V3P6 Active 
    pmu_reg_write(0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x1A: V0P6 Active
    pmu_reg_write(0x1A,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));

}

inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

	// Register 0x17: V3P6 Sleep
    pmu_reg_write(0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x15: V1P2 Sleep
    pmu_reg_write(0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	// Register 0x19: V0P6 Sleep
    pmu_reg_write(0x19,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));

}

inline static void pmu_set_sleep_radio(){
    pmu_set_sleep_clk(0xF,0xA,0x5,0xF/*V1P2*/);
}

inline static void pmu_set_sleep_low(){
    pmu_set_sleep_clk(0xF,0x0,0x1,0x1/*V1P2*/);
}

inline static void pmu_sleep_setting_temp_based(){
    
    if (pmu_setting_state >= PMU_95C){
		pmu_set_sleep_clk(0x2,0x0,0x1,0x1/*V1P2*/);

    }else if (pmu_setting_state >= PMU_75C){
		pmu_set_sleep_clk(0x4,0x0,0x1,0x1/*V1P2*/);

    }else if (pmu_setting_state >= PMU_55C){
		pmu_set_sleep_clk(0x6,0x0,0x1,0x1/*V1P2*/);

    }else if (pmu_setting_state >= PMU_35C){
		pmu_set_sleep_low();

    }else if (pmu_setting_state < PMU_10C){
		pmu_set_sleep_clk(0xF,0x2,0x1,0x4/*V1P2*/);

    }else if (pmu_setting_state < PMU_20C){
		pmu_set_sleep_clk(0xF,0x1,0x1,0x2/*V1P2*/);

    }else{ // 25C, default
		pmu_set_sleep_low();
    }

}
inline static void pmu_active_setting_temp_based(){
    
    if (pmu_setting_state >= PMU_95C){
		pmu_set_active_clk(0x1,0x0,0x10,0x1/*V1P2*/);

    }else if (pmu_setting_state == PMU_85C){
		pmu_set_active_clk(0x2,0x1,0x10,0x2/*V1P2*/);

    }else if (pmu_setting_state == PMU_75C){
		pmu_set_active_clk(0x3,0x1,0x10,0x2/*V1P2*/);

    }else if (pmu_setting_state == PMU_65C){
		pmu_set_active_clk(0x4,0x1,0x10,0x2/*V1P2*/);

    }else if (pmu_setting_state == PMU_55C){
		pmu_set_active_clk(0x6,0x1,0x10,0x2/*V1P2*/);

    }else if (pmu_setting_state == PMU_45C){
	    pmu_set_active_clk(0x5,0x2,0x10,0x4/*V1P2*/);

    }else if (pmu_setting_state == PMU_40C){
	    pmu_set_active_clk(0x6,0x3,0x10,0x6/*V1P2*/);

    }else if (pmu_setting_state == PMU_35C){
	    pmu_set_active_clk(0x8,0x3,0x10,0x7/*V1P2*/);

    }else if (pmu_setting_state == PMU_30C){
	    pmu_set_active_clk(0xC,0x3,0x10,0x7/*V1P2*/);

    }else if (pmu_setting_state == PMU_25C){
		pmu_set_active_clk(0xF,0x3,0x10,0x7/*V1P2*/);

    }else if (pmu_setting_state == PMU_20C){
		pmu_set_active_clk(0xF,0x5,0x10,0xB/*V1P2*/);

    }else if (pmu_setting_state == PMU_15C){
		pmu_set_active_clk(0xF,0xA,0x10,0xF/*V1P2*/);

    }else if (pmu_setting_state == PMU_10C){
		pmu_set_active_clk(0xF,0xF,0x10,0xF/*V1P2*/);

    }else{ 
		pmu_set_active_clk(0xF,0xF,0x1F,0xF/*V1P2*/);
    }
}

inline static void pmu_setting_temp_based(){
    
	uint32_t pmu_setting_prev = pmu_setting_state;
	// Change PMU based on temp
	if (sht35_cur_temp > 52428){
		pmu_setting_state = PMU_95C;
	}else if (sht35_cur_temp > 48683){
		pmu_setting_state = PMU_85C;
	}else if (sht35_cur_temp > 44938){
		pmu_setting_state = PMU_75C;
	}else if (sht35_cur_temp > 41193){
		pmu_setting_state = PMU_65C;
	}else if (sht35_cur_temp > 37449){
		pmu_setting_state = PMU_55C;
	}else if (sht35_cur_temp > 33704){
		pmu_setting_state = PMU_45C;
	}else if (sht35_cur_temp > 31831){
		pmu_setting_state = PMU_40C;
	}else if (sht35_cur_temp > 29959){
		pmu_setting_state = PMU_35C;
	}else if (sht35_cur_temp > 28086){
		pmu_setting_state = PMU_30C;
	}else if (sht35_cur_temp > 26214){
		pmu_setting_state = PMU_25C;
	}else if (sht35_cur_temp > 24342){
		pmu_setting_state = PMU_20C;
	}else if (sht35_cur_temp > 22469){
		pmu_setting_state = PMU_15C;
	}else if (sht35_cur_temp > 20597){
		pmu_setting_state = PMU_10C;
	}else{
		pmu_setting_state = PMU_0C;
	}

	if (pmu_setting_prev != pmu_setting_state){
		pmu_active_setting_temp_based();
		pmu_sleep_setting_temp_based();
	}

}


inline static void pmu_set_clk_init(){
    pmu_setting_state = PMU_25C;
    pmu_active_setting_temp_based();
    pmu_sleep_setting_temp_based();

	// SAR_RATIO_OVERRIDE
	// Use the new reset scheme in PMUv3
    pmu_reg_write(0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (0 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (0 << 7) // Enable override setting [6:0] (1'h0)
		| (0x3C) 		// Binary converter's conversion ratio (7'h00)
	));
	pmu_set_sar_override(0x3C);

	pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
}


inline static void pmu_adc_reset_setting(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Active
	// Updated for PMUv9
	pmu_reg_write(0x3C,
		((  1 << 0) //state_sar_scn_on
		| (1 << 1) //state_wait_for_clock_cycles
		| (1 << 2) //state_wait_for_time
		| (1 << 3) //state_sar_scn_reset
		| (1 << 4) //state_sar_scn_stabilized
		| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
		| (1 << 6) //state_clock_supply_switched
		| (1 << 7) //state_control_supply_switched
		| (1 << 8) //state_upconverter_on
		| (1 << 9) //state_upconverter_stabilized
		| (1 << 10) //state_refgen_on
		| (0 << 11) //state_adc_output_ready
		| (0 << 12) //state_adc_adjusted
		| (0 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read
		| (1 << 20) //state_state_horizon
	));
}

inline static void pmu_adc_disable(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Sleep
	// Updated for PMUv9
	pmu_reg_write(0x3B,
		((  1 << 0) //state_sar_scn_on
		| (1 << 1) //state_wait_for_clock_cycles
		| (1 << 2) //state_wait_for_time
		| (1 << 3) //state_sar_scn_reset
		| (1 << 4) //state_sar_scn_stabilized
		| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
		| (1 << 6) //state_clock_supply_switched
		| (1 << 7) //state_control_supply_switched
		| (1 << 8) //state_upconverter_on
		| (1 << 9) //state_upconverter_stabilized
		| (1 << 10) //state_refgen_on
		| (0 << 11) //state_adc_output_ready
		| (0 << 12) //state_adc_adjusted
		| (1 << 13) //state_sar_scn_ratio_adjusted // Turn on for old adc, off for new adc
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read // Turn off for old adc
		| (1 << 20) //state_state_horizon
	));
}

inline static void pmu_adc_enable(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Sleep
	// Updated for PMUv9
	pmu_reg_write(0x3B,
		((  1 << 0) //state_sar_scn_on
		| (1 << 1) //state_wait_for_clock_cycles
		| (1 << 2) //state_wait_for_time
		| (1 << 3) //state_sar_scn_reset
		| (1 << 4) //state_sar_scn_stabilized
		| (1 << 5) //state_sar_scn_ratio_roughly_adjusted
		| (1 << 6) //state_clock_supply_switched
		| (1 << 7) //state_control_supply_switched
		| (1 << 8) //state_upconverter_on
		| (1 << 9) //state_upconverter_stabilized
		| (1 << 10) //state_refgen_on
		| (1 << 11) //state_adc_output_ready
		| (0 << 12) //state_adc_adjusted // Turning off offset cancellation
		| (1 << 13) //state_sar_scn_ratio_adjusted // Turn on for old adc, off for new adc
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (0 << 19) //state_vbat_read // Turn off for old adc
		| (1 << 20) //state_state_horizon
	));
}


inline static void pmu_adc_read_latest(){
	// Grab latest PMU ADC readings
	// PMU register read is handled differently
	pmu_reg_write(0x00,0x03);
	// Updated for PMUv9
	read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;

}

inline static void pmu_reset_solar_short(){
	// Updated for PMUv9
    pmu_reg_write(0x0E, 
		( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
		| (1 << 11) // Reset of vbat_high latch for [12]=1
		| (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (1 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
    pmu_reg_write(0x0E, 
		( (0 << 12) // 1: solar short by latched vbat_high (new); 0: follow [10] setting
		| (1 << 11) // Reset of vbat_high latch for [12]=1
		| (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (0 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
}

//***************************************************
// CRC16 Encoding
//***************************************************
//uint16_t* CRC_A(uint8_t data[]){
//  //ISO14443
//  //Initial Register Value = 0xC6C6
//  //Polynomial = x^16+x^12+x^5+1
//  //Polynomial = 0x1021
//  //Reflected Input (byte): Only Reflect BYTES. Not the whole data field. 
//  //Reflected Input (byte) Example:
//  //Input = 0x1234 = 0b 0001 0010 0011 0100. Reflect(0x1234) = Reflect(0b 0001 0010 0011 0100) = 0b 0100 1000 0010 1100 = 0x4823
//  //Reflected Output (whole): Reflect the WHOLE data field
//  //Reflected Output (whole) Example:
//  //Input = 0x1234 = 0b 0001 0010 0011 0100. Reflect(0x1234) = Reflect(0b 0001 0010 0011 0100) = 0b 0010 1100 0100 1000 = 0x2C48
//  //Example can be found: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
//  //Documentation can be found: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
//
//   uint16_t poly = 0x1021;
//   uint16_t crc = 0xC6C6;
//
//   //Reflect Input (byte)
//   uint16_t data_reflect = 0;
//   uint16_t data_tmp = data[1]<<8|data[0];
//   uint16_t j = 0;
//   for (int i=7; i>=0; i--)
//     {
//       data_reflect = data_reflect | ((data_tmp>>j)&0x101)<<i;
//       j++;
//     }
//   crc = crc^data_reflect;
//
//   mbus_write_message32(0xDC, sizeof(data));
//   mbus_write_message32(0xDF, (sizeof(data)/sizeof(data[0])));
//
//  for (j = 2; j<sizeof data; j++)
//     {
//       for (int i=0; i<8; i++)
//	 {
//	   if ((crc & 0x8000) != 0)
//	     {
//	       crc = ((crc<<1) | ((data[j] >> (8-i))&0x1)) ^poly;
//	     }
//	   else
//	     { /* most significant bit not set, go to next bit */
//	       crc = ((crc << 1) | ((data[j] >> (8-i))&0x1)) ;
//	     }
//	 }
//     }
//
//   //Reflect Output (whole)
//   uint16_t crc_reflect = 0;
//   j = 0;
//   for (int i=15; i>=0; i--)
//     {
//       crc_reflect = crc_reflect | ((crc>>j)&1)<<i;
//       j++;
//     }
//   return crc_reflect;
//}

#define DATA_LEN 96
#define CRC_LEN 16

uint32_t* crcEnc16(uint32_t data2, uint32_t data1, uint32_t data0)
{
    // intialization
    uint32_t i;
   
    uint16_t poly = 0xc002;
    uint16_t poly_not = ~poly;
    uint16_t remainder = 0x0000;
    uint16_t remainder_shift = 0x0000;
    data2 = (data2 << CRC_LEN) + (data1 >> CRC_LEN);
    data1 = (data1 << CRC_LEN) + (data0 >> CRC_LEN);
    data0 = data0 << CRC_LEN;
    
    // LFSR
    uint16_t input_bit;
    for (i = 0; i < DATA_LEN; i++)
    {
        uint16_t MSB;
        if (remainder > 0x7fff)
            MSB = 0xffff;
        else
            MSB = 0x0000;
        
        if (i < 32)
            input_bit = ((data2 << i) > 0x7fffffff);
        else if (i < 64)
            input_bit = (data1 << (i-32)) > 0x7fffffff;
        else
            input_bit = (data0 << (i-64)) > 0x7fffffff;

        remainder_shift = remainder << 1;
        remainder = (poly&((remainder_shift)^MSB))|((poly_not)&(remainder_shift))
                         + (input_bit^(remainder > 0x7fff));
    }

    data0 = data0 + remainder;

    static uint32_t msg_out[3];
    msg_out[0] = data2;
    msg_out[1] = data1;
    msg_out[2] = data0;

    return msg_out;    
}



//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){
  // Reset GOC_DATA_IRQ
  *GOC_DATA_IRQ = 0;
  
  // Go to Sleep
  mbus_sleep_all();
  while(1);
 }

static void operation_sleep_notimer(void){

  // Make sure the irq counter is reset    
  exec_count_irq = 0;
  
  // Disable Timer
  set_wakeup_timer(0, 0, 0);
  
  // Go to sleep
  operation_sleep();
}

static void operation_init(void){
  
  // Set CPU & Mbus Clock Speeds
  prev20_r0B.CLK_GEN_RING = 0x1; // Default 0x1
  prev20_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
  prev20_r0B.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
  prev20_r0B.GOC_CLK_GEN_SEL_FREQ = 0x5; // Default 0x6
  prev20_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
  prev20_r0B.GOC_SEL = 0xF; // Default 0x8
  *REG_CLKGEN_TUNE = prev20_r0B.as_int;
  
  prev20_r1C.SRAM0_TUNE_DECODER_DLY = 0x10; // Default 0x10, 5 bits
  prev20_r1C.SRAM0_USE_INVERTER_SA = 0;  // Default 0
  *REG_SRAM0_TUNE = prev20_r1C.as_int;
  
  
  //Enumerate & Initialize Registers
  stack_state = STK_IDLE; 	//0x0;
  enumerated = 0x4148124A; // 0x4148 is AH in ascii
  exec_count = 0;
  wakeup_count = 0;
  exec_count_irq = 0;
  PMU_ADC_3P0_VAL = 0x62;
  
  // Set CPU Halt Option as RX --> Use for register read e.g.
  //    set_halt_until_mbus_rx();
  
  //Enumeration
  mbus_enumerate(SNT_ADDR);
  delay(MBUS_DELAY);
  mbus_enumerate(MRR_ADDR);
  delay(MBUS_DELAY);
  mbus_enumerate(PMU_ADDR);
  delay(MBUS_DELAY);
  
  // Set CPU Halt Option as TX --> Use for register write e.g.
  //    set_halt_until_mbus_tx();
  
  // PMU Settings ----------------------------------------------
  pmu_set_clk_init();
  pmu_reset_solar_short();
  
  // New for PMUv9
  // VBAT_READ_TRIM Register
  pmu_reg_write(0x45,
		( (0x0 << 9) // 0x0: no mon; 0x1: sar conv mon; 0x2: up conv mon; 0x3: down conv mon
		  | (0x0 << 8) // 1: vbat_read_mode enable; 0: vbat_read_mode disable
		  | (0x48 << 0) //sampling multiplication factor N; vbat_read out = vbat/1p2*N
		  ));
  
  // Disable PMU ADC measurement in active mode
  // PMU_CONTROLLER_STALL_ACTIVE
  // Updated for PMUv9
  pmu_reg_write(0x3A, 
		( (1 << 20) // ignore state_horizon; default 1
		  | (0 << 19) // state_vbat_read
		  | (1 << 13) // ignore adc_output_ready; default 0
		  | (1 << 12) // ignore adc_output_ready; default 0
		  | (1 << 11) // ignore adc_output_ready; default 0
		  ));
  
  pmu_adc_reset_setting();
  pmu_adc_enable();
  
  
  // Initialize other global variables
  WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
  sns_running = 0;
  radio_ready = 0;
  radio_on = 0;
  wakeup_data = 0;
  RADIO_PACKET_DELAY = 4000;
  radio_packet_count = 0;
  error_code = 0;
  
  astack_detection_mode = 0;
  adxl_enabled = 0;
  adxl_motion_detected = 0;
  
  adxl_user_threshold = 0x060; // rec. 0x60, max 0x7FF
  sht35_user_repeatability = 0x0B; // default 0x0B
  sht35_cur_temp = 26215; // 25C
  
  SNT_0P5S_VAL = 1000;
  
  // Go to sleep without timer
  //operation_sleep_notimer();
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){
  
  // Only enable relevant interrupts (PRCv17)
  //enable_reg_irq();
  //enable_all_irq();
  *NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3) | (1 << IRQ_GPIO);
  
  // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
  config_timerwd(TIMERWD_VAL);
  
  wakeup_count++;
  
  // Figure out who triggered wakeup
  if(wakeup_source & 0x00000008){
    // Debug
#ifdef DEBUG_MBUS_MSG
    mbus_write_message32(0xAA,0x11331133);
#endif
    adxl_motion_detected = 1;
  }
  
#ifdef DEBUG_MBUS_MSG
  mbus_write_message32(0xAB, sleep_time_prev);
#endif
  
  // Initialization sequence
  if (enumerated != 0x4148124A){
    operation_init();
  }

  //mbus_write_message32(0xDD, 0x0);
  //set_gpio_pad_with_mask(GPO_MASK,(1<<GPIO_GPO));
  //mbus_write_message32(0xDD, 0x1);
  //gpio_set_dir_with_mask(GPO_MASK,(0<<GPIO_GPO));
  //unfreeze_gpio_out();

  //uint8_t array_test[5] = {0x01, 0x00, 0x00, 0xFF, 0xCC};
  //uint16_t test;
  //test = CRC_A(array_test);
  //mbus_write_message32(0xDE, test);
  //mbus_write_message32(0xDE, test);
  //mbus_write_message32(0xDE, test);
  //mbus_write_message32(0xDE, test);

//
//  ////////////////////////////////////////////////////////////////////////////////
//  //Start|M24SR Device Select Code|PCB Field|Payload                        |EOD
//  //                                        |CLA  INS  P1   P2   L    Data  |CRC --> Computed on SOD (Device select + PCB Field)
//  //Start|0xAC                    |0x02     |0x00 0xA4 0x00 0x0C 0x02 0xE101|0x35 0xC0
//  
  mbus_write_message32(0xDD, 0x2);
//
  //Enable CPS for NFC
  *REG_CPS = *REG_CPS | 0x1;
  // Enable GPIO OUTPUT
  gpio_write_data_with_mask(NFC_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  set_gpio_pad_with_mask(NFC_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_set_dir_with_mask(NFC_MASK,(1<<GPIO_SDA) | (1<<GPIO_SCL));

  set_gpio_pad_with_mask(GPO_MASK,(1<<GPIO_GPO));
  gpio_set_dir_with_mask(GPO_MASK,(0<<GPIO_GPO));
  unfreeze_gpio_out();

  //Get Token
  operation_i2c_start();
  operation_i2c_cmd(0xAC);
  operation_i2c_cmd(0x26);
  operation_i2c_stop();
  //Check if got token
  while( (*GPIO_DATA>>GPIO_GPO&0x1) == 1)
    {
      mbus_write_message32(0xDD, 0x0);
    }

  //NDEF Tag Application Select
  operation_i2c_start();
  operation_i2c_cmd(0xAC);
  operation_i2c_cmd(0x02);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0xA4);
  operation_i2c_cmd(0x04);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x07);
  operation_i2c_cmd(0xD2);
  operation_i2c_cmd(0x76);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x85);
  operation_i2c_cmd(0x01);
  operation_i2c_cmd(0x01);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x35);
  operation_i2c_cmd(0xC0);
  operation_i2c_stop();

  operation_i2c_start();
  operation_i2c_cmd(0xAD);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_stop();

  //NDEF Select
  operation_i2c_start();
  operation_i2c_cmd(0xAC);
  operation_i2c_cmd(0x02);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0xA4);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x0C);
  operation_i2c_cmd(0x02);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x01);
  operation_i2c_cmd(0x3E);
  operation_i2c_cmd(0xFD);
  operation_i2c_stop();

  operation_i2c_start();
  operation_i2c_cmd(0xAD);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_stop();

  //Read Binary
  operation_i2c_start();
  operation_i2c_cmd(0xAC);
  operation_i2c_cmd(0x02);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0xB0);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x04);
  operation_i2c_cmd(0x04);
  operation_i2c_cmd(0x3D);
  operation_i2c_cmd(0x7F);
  operation_i2c_stop();

  operation_i2c_start();
  operation_i2c_cmd(0xAD);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_stop();

  //Update Binary (DEADBEEF)
  operation_i2c_start();
  operation_i2c_cmd(0xAC);
  operation_i2c_cmd(0x02);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0xD6);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x04);
  operation_i2c_cmd(0x04);
  operation_i2c_cmd(0xDE);
  operation_i2c_cmd(0xAD);
  operation_i2c_cmd(0xBE);
  operation_i2c_cmd(0xEF);
  operation_i2c_cmd(0xC7);
  operation_i2c_cmd(0xDA);
  operation_i2c_stop();


  delay(1000);
  
  operation_i2c_start();
  operation_i2c_cmd(0xAD);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_stop();

  //Read Binary
  operation_i2c_start();
  operation_i2c_cmd(0xAC);
  operation_i2c_cmd(0x02);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0xB0);
  operation_i2c_cmd(0x00);
  operation_i2c_cmd(0x04);
  operation_i2c_cmd(0x08);
  operation_i2c_cmd(0x51);
  operation_i2c_cmd(0xB5);
  operation_i2c_stop();

  operation_i2c_start();
  operation_i2c_cmd(0xAD);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_stop();

  //  //CC File Select
//  operation_i2c_start();
//  operation_i2c_cmd(0xAC);
//  operation_i2c_cmd(0x02);
//  operation_i2c_cmd(0x00);
//  operation_i2c_cmd(0xA4);
//  operation_i2c_cmd(0x00);
//  operation_i2c_cmd(0x0C);
//  operation_i2c_cmd(0x02);
//  operation_i2c_cmd(0xE1);
//  operation_i2c_cmd(0x03);
//  operation_i2c_cmd(0x6D);
//  operation_i2c_cmd(0x2E);
//  operation_i2c_stop();
//
//  
//  operation_i2c_start();
//  operation_i2c_cmd(0xAD);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_stop();

//  //System File Select
//  operation_i2c_start();
//  operation_i2c_cmd(0xAC);
//  operation_i2c_cmd(0x02);
//  operation_i2c_cmd(0x00);
//  operation_i2c_cmd(0xA4);
//  operation_i2c_cmd(0x00);
//  operation_i2c_cmd(0x0C);
//  operation_i2c_cmd(0x02);
//  operation_i2c_cmd(0xE1);
//  operation_i2c_cmd(0x01);
//  operation_i2c_cmd(0x7F);
//  operation_i2c_cmd(0x0D);
//  operation_i2c_stop();
//
//  operation_i2c_start();
//  operation_i2c_cmd(0xAD);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_stop();
//
//  //Read Binary
//  operation_i2c_start();
//  operation_i2c_cmd(0xAC);
//  operation_i2c_cmd(0x02);
//  operation_i2c_cmd(0x00);
//  operation_i2c_cmd(0xB0);
//  operation_i2c_cmd(0x00);
//  operation_i2c_cmd(0x00);
//  operation_i2c_cmd(0x12);
//  operation_i2c_cmd(0xEA);
//  operation_i2c_cmd(0x6D);
//  operation_i2c_stop();
//
//  operation_i2c_start();
//  operation_i2c_cmd(0xAD);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_rd(0x1);
//  operation_i2c_stop();

  //Token Release
  operation_i2c_start();
  operation_i2c_cmd(0xAC);
  while( (*GPIO_DATA>>GPIO_GPO&0x1) == 0)
  {
      mbus_write_message32(0xDD, *GPIO_DATA);
    }
  //  delay(1200); //Min 20ms, Max 40ms
  operation_i2c_stop();
  *REG_CPS = *REG_CPS & 0x0;
//
//  delay(1000);
//  mbus_write_message32(0xDD, 0x1);
//  delay(1000);
//
//  config_gpio_posedge_wirq(0x0);
//  config_gpio_negedge_wirq(0x4);
//  freeze_gpio_out();
  
  operation_sleep_notimer();
  
  while(1);
}
