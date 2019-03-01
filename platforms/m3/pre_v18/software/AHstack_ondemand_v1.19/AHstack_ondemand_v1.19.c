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
//*******************************************************************
#include "PREv18.h"
#include "PREv18_RF.h"
#include "mbus.h"
#include "SNTv1_RF.h"
#include "PMUv7_RF.h"
#include "ADXL362.h"
#include "MRRv7_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// AM stack 
#define MRR_ADDR 0x4
#define SNT_ADDR 0x5
#define PMU_ADDR 0x6

// System parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define SNS_CYCLE_INIT 3 

// Pstack states
#define	STK_IDLE		0x0
#define	STK_LDO		0x1
#define	STK_TEMP_START 0x2
#define	STK_TEMP_READ  0x3
#define	STK_HUM     0x4

// ADXL362 Defines
#define SPI_TIME 250

// Radio configurations
#define RADIO_DATA_LENGTH 192
#define WAKEUP_PERIOD_RADIO_INIT 10 // About 2 sec (PRCv17)

#define TEMP_NUM_MEAS 1

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
volatile uint32_t sht35_mask = (1<<GPIO_SDA) | (1<<GPIO_SCL);
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

volatile sntv1_r00_t sntv1_r00 = SNTv1_R00_DEFAULT;
volatile sntv1_r01_t sntv1_r01 = SNTv1_R01_DEFAULT;
volatile sntv1_r03_t sntv1_r03 = SNTv1_R03_DEFAULT;
volatile sntv1_r08_t sntv1_r08 = SNTv1_R08_DEFAULT;
volatile sntv1_r09_t sntv1_r09 = SNTv1_R09_DEFAULT;
volatile sntv1_r0A_t sntv1_r0A = SNTv1_R0A_DEFAULT;
volatile sntv1_r17_t sntv1_r17 = SNTv1_R17_DEFAULT;

volatile prev18_r0B_t prev18_r0B = PREv18_R0B_DEFAULT;
volatile prev18_r1C_t prev18_r1C = PREv18_R1C_DEFAULT;

volatile mrrv7_r00_t mrrv7_r00 = MRRv7_R00_DEFAULT;
volatile mrrv7_r01_t mrrv7_r01 = MRRv7_R01_DEFAULT;
volatile mrrv7_r02_t mrrv7_r02 = MRRv7_R02_DEFAULT;
volatile mrrv7_r03_t mrrv7_r03 = MRRv7_R03_DEFAULT;
volatile mrrv7_r04_t mrrv7_r04 = MRRv7_R04_DEFAULT;
volatile mrrv7_r05_t mrrv7_r05 = MRRv7_R05_DEFAULT;
volatile mrrv7_r07_t mrrv7_r07 = MRRv7_R07_DEFAULT;
volatile mrrv7_r10_t mrrv7_r10 = MRRv7_R10_DEFAULT;
volatile mrrv7_r11_t mrrv7_r11 = MRRv7_R11_DEFAULT;
volatile mrrv7_r12_t mrrv7_r12 = MRRv7_R12_DEFAULT;
volatile mrrv7_r13_t mrrv7_r13 = MRRv7_R13_DEFAULT;
volatile mrrv7_r14_t mrrv7_r14 = MRRv7_R14_DEFAULT;
volatile mrrv7_r15_t mrrv7_r15 = MRRv7_R15_DEFAULT;
volatile mrrv7_r16_t mrrv7_r16 = MRRv7_R16_DEFAULT;

volatile mrrv7_r1E_t mrrv7_r1E = MRRv7_R1E_DEFAULT;
volatile mrrv7_r1F_t mrrv7_r1F = MRRv7_R1F_DEFAULT;


//***************************************************
// ADXL362 Functions
//***************************************************
static void ADXL362_reg_wr (uint8_t reg_id, uint8_t);
static void ADXL362_reg_rd (uint8_t reg_id);
static void ADXL362_init(void);
static void ADXL362_stop(void);
static void operation_spi_init(void);
static void operation_spi_stop(void);

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
static void operation_sns_sleep_check(void);
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);
static void operation_sleep_notimer(void);


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
// ADXL362 Functions
//***************************************************
static void ADXL362_reg_wr (uint8_t reg_id, uint8_t reg_data){
  gpio_write_data_with_mask(adxl_mask,(0<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
  *SPI_SSPDR = 0x0A;
  *SPI_SSPDR = reg_id;
  *SPI_SSPDR = reg_data;
  *SPI_SSPCR1 = 0x2;
  while((*SPI_SSPSR>>4)&0x1){}//Spin
  gpio_write_data_with_mask(adxl_mask,(1<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
  *SPI_SSPCR1 = 0x0;
}

static void ADXL362_reg_rd (uint8_t reg_id){
  gpio_write_data_with_mask(adxl_mask,(0<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
  *SPI_SSPDR = 0x0B;
  *SPI_SSPDR = reg_id;
  *SPI_SSPDR = 0x00;
  *SPI_SSPCR1 = 0x2;
  while((*SPI_SSPSR>>4)&0x1){}//Spin
  gpio_write_data_with_mask(adxl_mask,(1<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
  *SPI_SSPCR1 = 0x0;
  mbus_write_message32(0xB1,*SPI_SSPDR);
}

static void ADXL362_init(){
  // Soft Reset
  ADXL362_reg_wr(ADXL362_SOFT_RESET,0x52);
  
  // Check if Aliveo
	/*
  ADXL362_reg_rd(ADXL362_DEVID_AD);
  ADXL362_reg_rd(ADXL362_DEVID_MST);
  ADXL362_reg_rd(ADXL362_PARTID);
  ADXL362_reg_rd(ADXL362_REVID);
	*/
  // Set Interrupt1 Awake Bit [4]
  ADXL362_reg_wr(ADXL362_INTMAP1,0x10);

  // Set Activity Threshold
  ADXL362_reg_wr(ADXL362_THRESH_ACT_L,adxl_user_threshold & 0xFF);
  ADXL362_reg_wr(ADXL362_THRESH_ACT_H,(adxl_user_threshold>>8) & 0xFF);

  // Set Actvity/Inactivity Control
  ADXL362_reg_wr(ADXL362_ACT_INACT_CTL,0x03);

  // Enter Measurement Mode
  ADXL362_reg_wr(ADXL362_POWER_CTL,0x0A);

  // Check Status (Clears first false positive)
  delay(5000);
  ADXL362_reg_rd(ADXL362_STATUS);
}

static void ADXL362_enable(){

	// Initialize Interrupts
	*NVIC_ISER = 1<<IRQ_WAKEUP;

	operation_spi_init();
	delay(MBUS_DELAY);

	// Turn PRE power switch on
	*REG_CPS = *REG_CPS | 0x2;
	delay(MBUS_DELAY*2);

	// Initialize ADXL
	ADXL362_init();
	delay(MBUS_DELAY*2);
	config_gpio_posedge_wirq((0<<GPIO_ADXL_EN) | (1<<GPIO_ADXL_INT));
	operation_spi_stop();

	adxl_enabled = 1;
	adxl_motion_detected = 0;
}

static void ADXL362_stop(){
	operation_spi_init();
	delay(MBUS_DELAY);

	// Put in Standby Mode
	ADXL362_reg_wr(ADXL362_POWER_CTL,0x0);

	// Soft Reset
	ADXL362_reg_wr(ADXL362_SOFT_RESET,0x52);
  
	operation_spi_stop();

	config_gpio_posedge_wirq(0x0);
	*NVIC_ISER = 0<<IRQ_WAKEUP;
	unfreeze_gpio_out();
	set_gpio_pad_with_mask(adxl_mask,(1<<GPIO_ADXL_EN) | (1<<GPIO_ADXL_INT));
	gpio_set_dir_with_mask(adxl_mask,(1<<GPIO_ADXL_EN) | (1<<GPIO_ADXL_INT));
	gpio_write_data_with_mask(adxl_mask,0);
	freeze_gpio_out();
	adxl_enabled = 0;
}

static void ADXL362_power_off(){
	ADXL362_stop();
	delay(MBUS_DELAY*10);
	*REG_CPS = *REG_CPS & 0xFFFFFFFD;
	delay(MBUS_DELAY*200);
}

static void operation_spi_init(){
  *SPI_SSPCR0 =  0x0207; // Motorola Mode
  *SPI_SSPCPSR = 0x02;   // Clock Prescale Register

	// Enable SPI Input/Output
  set_spi_pad(1);
  set_gpio_pad_with_mask(adxl_mask,(1<<GPIO_ADXL_EN) | (1<<GPIO_ADXL_INT));
  gpio_set_dir_with_mask(adxl_mask,(1<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
  gpio_write_data_with_mask(adxl_mask,(1<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT)); // << Crashes here
  unfreeze_gpio_out();
  unfreeze_spi_out();
}

static void operation_spi_stop(){
  freeze_spi_out();
  freeze_gpio_out();
}

//***************************************************
// SHT35 Functions (I2C Bit-Bang)
//***************************************************

static void operation_i2c_start(){
  // Enable GPIO OUTPUT
  gpio_write_data_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  set_gpio_pad_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_set_dir_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  unfreeze_gpio_out();
  //Start
  gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
}

static void operation_i2c_stop(){
  // Stop
  gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
}

static void operation_i2c_addr(uint8_t addr, uint8_t RWn){
  //Assume started
  //[6]
  gpio_set_dir_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>6)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[5]
  gpio_write_data_with_mask(sht35_mask,(((addr>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>5)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[4]
  gpio_write_data_with_mask(sht35_mask,(((addr>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>4)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[3]
  gpio_write_data_with_mask(sht35_mask,(((addr>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>3)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[2]
  gpio_write_data_with_mask(sht35_mask,(((addr>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>2)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[1]
  gpio_write_data_with_mask(sht35_mask,(((addr>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>1)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[0]
  gpio_write_data_with_mask(sht35_mask,(((addr>>0)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>0)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((addr>>0)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  
  //WRITEn/READ
  //Need Hack
  if((RWn&0x1) == 1){ //Need hack
    gpio_set_dir_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
    gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
    gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
  }
  else{
    gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
  }

  //Wait for ACK
  gpio_set_dir_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  while((*GPIO_DATA>>GPIO_SDA)&0x1){
    //mbus_write_message32(0xCE, *GPIO_DATA);
  }
  gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
  gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
}

static void operation_i2c_cmd(uint8_t cmd){
  gpio_set_dir_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
  //[7]
  gpio_write_data_with_mask(sht35_mask,(((cmd>>7)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>7)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>7)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[6]
  gpio_write_data_with_mask(sht35_mask,(((cmd>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>6)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[5]
  gpio_write_data_with_mask(sht35_mask,(((cmd>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>5)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[4]
  gpio_write_data_with_mask(sht35_mask,(((cmd>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>4)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[3]
  gpio_write_data_with_mask(sht35_mask,(((cmd>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>3)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[2]
  gpio_write_data_with_mask(sht35_mask,(((cmd>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>2)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[1]
  gpio_write_data_with_mask(sht35_mask,(((cmd>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>1)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data_with_mask(sht35_mask,(((cmd>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[0]
  if((cmd&0x1) == 1){ //Need hack
    gpio_set_dir_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
    gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
    gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
  }
  else{
    gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
  }
  
  //Wait for ACK
  gpio_set_dir_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
  while((*GPIO_DATA>>GPIO_SDA)&0x1){
    //mbus_write_message32(0xCF, *GPIO_DATA);
  }
  gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
  gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
  gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
}

static uint8_t operation_i2c_rd(uint8_t ACK){

	uint8_t data;
	data = 0;
	gpio_set_dir_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
	//[7]
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<7);
	//[6]
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<6);
	//[5]
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<5);
	//[4]
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<4);
	//[3]
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<3);
	//[2]
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<2);
	//[1]
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<1);
	//[0]
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
	data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<0);
	gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	if (ACK&0x1){
		gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
		gpio_set_dir_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
		gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (1<<GPIO_SCL));
		gpio_write_data_with_mask(sht35_mask,(0<<GPIO_SDA) | (0<<GPIO_SCL));
	}
	else{
		gpio_set_dir_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));
		gpio_write_data_with_mask(sht35_mask,1<<GPIO_SCL);
		gpio_write_data_with_mask(sht35_mask,0<<GPIO_SCL);
	}
	// Debug
	//mbus_write_message32(0xCF, data);
	return data;
}


static void sht35_meas_data(){
	uint8_t i2c_data_rx;
	// FIXME
	// Power on
	*REG_CPS = *REG_CPS | 0x1;
	delay(1000);
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
	*REG_CPS = *REG_CPS & 0xFFFFFFFE;
}

//************************************
// PMU Related Functions
//************************************

static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    set_halt_until_mbus_tx();
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

inline static void pmu_set_clk_init(){
	pmu_set_active_clk(0xF,0x1,0x10,0x2);
	pmu_set_sleep_low();

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
		| (0x45) 		// Binary converter's conversion ratio (7'h00)
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
// MRR Functions
//***************************************************

static void radio_power_on(){
	// Turn off PMU ADC
	//pmu_adc_disable();

	// Need to speed up sleep pmu clock
	//pmu_set_sleep_radio();

    // Turn off Current Limter Briefly
    mrrv7_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Set decap to parallel
	mrrv7_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	mrrv7_r03.MRR_DCP_P_OW = 1;  //RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    delay(MBUS_DELAY);

	// Set decap to series
	mrrv7_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	mrrv7_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
    delay(MBUS_DELAY);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    radio_on = 1;

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Release timer power-gate
    mrrv7_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

	// Turn on timer
    mrrv7_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

    mrrv7_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);
    delay(MBUS_DELAY);

    mrrv7_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    // Release FSM Sleep
    mrrv7_r11.MRR_RAD_FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
	delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off(){
	// Need to restore sleep pmu clock

	// Enable PMU ADC
	//pmu_adc_enable();

    // Turn off Current Limter Briefly
    mrrv7_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn off everything
    mrrv7_r03.MRR_TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);

    mrrv7_r11.MRR_RAD_FSM_EN = 0;  //Stop BB
    mrrv7_r11.MRR_RAD_FSM_RSTN = 0;  //RST BB
    mrrv7_r11.MRR_RAD_FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

    mrrv7_r04.RO_RESET = 1;  //Release Reset TIMER
    mrrv7_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv7_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    // Enable timer power-gate
    mrrv7_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv7_r04.as_int);

    radio_on = 0;
	radio_ready = 0;

}

static void mrr_configure_pulse_width_long(){

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 24; //100us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 49; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}

/*
static void mrr_configure_pulse_width_long_2(){

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 19; //80us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 39; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}

static void mrr_configure_pulse_width_long_3(){

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 9; //40us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 19; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}

static void mrr_configure_pulse_width_short(){

    mrrv7_r12.MRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
    mrrv7_r13.MRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    mrrv7_r12.MRR_RAD_FSM_TX_PS_LEN = 1; // PW=PS guard interval betwen 0 and 1 pulse

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv7_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);
}
*/


static void send_radio_data_mrr_sub1(){

	// Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
	config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Fire off data
	mrrv7_r11.MRR_RAD_FSM_EN = 1;  //Start BB
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

	// Wait for radio response
	WFI();

	// Turn off Timer32
	*TIMER32_GO = 0;

	if (wfi_timeout_flag){
		mbus_write_message32(0xFA, 0xFAFAFAFA);
	}

    // Turn off Current Limter
    mrrv7_r00.MRR_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

	mrrv7_r11.MRR_RAD_FSM_EN = 0;
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
}

static void send_radio_data_mrr(uint32_t last_packet, uint8_t radio_packet_prefix, uint32_t radio_data){
	// Sends 192 bit packet, of which 96b is actual data
	// MRR REG_9: reserved for header
	// MRR REG_A: reserved for header
	// MRR REG_B: reserved for header
	// MRR REG_C: reserved for header
	// MRR REG_D: DATA[23:0]
	// MRR REG_E: DATA[47:24]
	// MRR REG_F: DATA[71:48]
	// MRR REG_10: DATA[95:72]

	// CRC16 Encoding 
    uint32_t* output_data;
    //output_data = crcEnc16(((radio_packet_count & 0xFF)<<8) | radio_packet_prefix, (radio_data_2 <<16) | ((radio_data_1 & 0xFFFFFF) >>8), (radio_data_1 << 24) | (radio_data_0 & 0xFFFFFF));
    output_data = crcEnc16(((read_data_batadc & 0xFF)<<8) | ((radio_packet_prefix & 0xF)<<4) | ((radio_packet_count>>16)&0xF), ((radio_packet_count & 0xFFFF)<<16) | (*REG_CHIP_ID & 0xFFFF), radio_data);

    mbus_remote_register_write(MRR_ADDR,0xD,radio_data & 0xFFFFFF);
    mbus_remote_register_write(MRR_ADDR,0xE,(*REG_CHIP_ID<<8)|(radio_data>>24));
    mbus_remote_register_write(MRR_ADDR,0xF,(radio_packet_prefix<<20)|(radio_packet_count&0xFFFFF));
    mbus_remote_register_write(MRR_ADDR,0x10,((output_data[2] & 0xFFFF)/*CRC16*/<<8)|(read_data_batadc&0xFF));

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		mrrv7_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
		delay(MBUS_DELAY);

    	mrrv7_r03.MRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    	mbus_remote_register_write(MRR_ADDR,0x03,mrrv7_r03.as_int);
		delay(MBUS_DELAY);

		// Current Limter set-up 
		mrrv7_r00.MRR_CL_CTRL = 2; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    }

	uint32_t count = 0;
	uint32_t mrr_cfo_val_fine = 0;
	uint32_t num_packets = 1;
	if (mrr_freq_hopping) num_packets = mrr_freq_hopping;
	
	// New for MRRv7
	mrr_cfo_val_fine = mrr_cfo_val_fine_min;

	while (count < num_packets){
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCE, mrr_cfo_val);
		#endif

		mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine; 
		mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine;
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
		send_radio_data_mrr_sub1();
		count++;
		if (count < num_packets){
			delay(RADIO_PACKET_DELAY);
		}
		mrr_cfo_val_fine = mrr_cfo_val_fine + mrr_freq_hopping_step; // 1: 0.8MHz, 2: 1.6MHz step
	}

	radio_packet_count++;

	if (last_packet){
		radio_ready = 0;
		radio_power_off();
	}else{
		mrrv7_r11.MRR_RAD_FSM_EN = 0;
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);
	}
}

//***************************************************
// Temp Sensor Functions (SNTv1)
//***************************************************

static void snt_read_wup_counter(){

    set_halt_until_mbus_rx();
    mbus_remote_register_read(SNT_ADDR,0x1B,1); // CNT_VALUE_EXT
    snt_wup_counter_cur = (*REG1 & 0xFF)<<24;
    mbus_remote_register_read(SNT_ADDR,0x1C,1); // CNT_VALUE
    snt_wup_counter_cur = snt_wup_counter_cur | (*REG1 & 0xFFFFFF);
    set_halt_until_mbus_tx();

}
    
static void snt_start_timer_presleep(){

    sntv1_r09.TMR_IBIAS_REF = 0x4; // Default : 4'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv1_r09.as_int);

    // TIMER SELF_EN Disable 
    sntv1_r09.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x09,sntv1_r09.as_int);

    // EN_OSC 
    sntv1_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv1_r08.as_int);

    // Release Reset 
    sntv1_r08.TMR_RESETB = 0x1; // Default : 0x0
    sntv1_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
    sntv1_r08.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv1_r08.as_int);

    // TIMER EN_SEL_CLK Reset 
    sntv1_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv1_r08.as_int);

    // TIMER SELF_EN 
    sntv1_r09.TMR_SELF_EN = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x09,sntv1_r09.as_int);
    //delay(100000); 

    snt_timer_enabled = 1;
}

static void snt_start_timer_postsleep(){
    // Turn off sloscillator
    sntv1_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv1_r08.as_int);
}


static void snt_stop_timer(){

    // EN_OSC
    sntv1_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    // RESET
    sntv1_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
    sntv1_r08.TMR_RESETB = 0x0;// Default : 0x0
    sntv1_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
    sntv1_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,sntv1_r08.as_int);
    snt_timer_enabled = 0;

    sntv1_r09.TMR_IBIAS_REF = 0x0; // Default : 4'h4
    mbus_remote_register_write(SNT_ADDR,0x09,sntv1_r09.as_int);

    sntv1_r17.WUP_ENABLE = 0x0; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,sntv1_r17.as_int);

}

static void snt_set_wup_timer(uint32_t sleep_count){
    
    snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover

    mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
    mbus_remote_register_write(SNT_ADDR,0x1A,snt_wup_counter_cur & 0xFFFFFF);
    
    sntv1_r17.WUP_ENABLE = 0x1; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,sntv1_r17.as_int);

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

static void operation_sleep_snt_timer(void){

    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // Disable PRC Timer
    set_wakeup_timer(0, 0, 0);

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}

static void operation_sleep_noirqreset(void){

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}

static void operation_sleep_notimer(void){

	// Make sure the irq counter is reset    
    exec_count_irq = 0;

	if (adxl_enabled){
		ADXL362_power_off();
	}

    // Make sure Radio is off
    if (radio_on){radio_power_off();}

	// Disable Timer
	set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}


uint32_t dumb_divide(uint32_t nu, uint32_t de) {
// Returns quotient of nu/de

    uint32_t temp = 1;
    uint32_t quotient = 0;

    while (de <= nu) {
        de <<= 1;
        temp <<= 1;
    }

    //printf("%d %d\n",de,temp,nu);
    while (temp > 1) {
        de >>= 1;
        temp >>= 1;

        if (nu >= de) {
            nu -= de;
            //printf("%d %d\n",quotient,temp);
            quotient += temp;
        }
    }

    return quotient;
}

static void measure_wakeup_period(void){

    // Reset Wakeup Timer
    *WUPT_RESET = 1;

	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xE0, 0x0);
	#endif
	// Prevent watchdogs from kicking in
   	config_timerwd(TIMERWD_VAL*2);
	*REG_MBUS_WD = 1500000*3; // default: 1500000

	uint32_t wakeup_timer_val_0 = *REG_WUPT_VAL;
	uint32_t wakeup_period_count = 0;

	while( *REG_WUPT_VAL <= wakeup_timer_val_0 + 1){
		wakeup_period_count = 0;
	}
	while( *REG_WUPT_VAL <= wakeup_timer_val_0 + 2){
		wakeup_period_count++;
	}
	mbus_write_message32(0xE1, wakeup_period_count);
	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xE2, *REG_WUPT_VAL);
	#endif

   	config_timerwd(TIMERWD_VAL);

	WAKEUP_PERIOD_CONT = dumb_divide(WAKEUP_PERIOD_CONT_USER*1000*wakeup_period_calc_factor, wakeup_period_count);

	// Limitation of PRCv17
    if (WAKEUP_PERIOD_CONT > 0x7FFF){
        WAKEUP_PERIOD_CONT = 0x7FFF;
    }

	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xED, WAKEUP_PERIOD_CONT); 
	#endif
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
    stack_state = STK_IDLE; 	//0x0;
    enumerated = 0x41481190; // 0x4148 is AH in ascii
    exec_count = 0;
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

    // SNT Wakeup Timer Settings --------------------------------------
    // Config Register A
    sntv1_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
    // Tune R for TC
    sntv1_r0A.TMR_DIFF_CON = 0x3FEF; // Default: 0x3FFB
    sntv1_r0A.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(SNT_ADDR,0x0A,sntv1_r0A.as_int);

    // TIMER CAP_TUNE  
    // Tune C for freq
    sntv1_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
    sntv1_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4

    // to reduce standby current
    sntv1_r09.TMR_IBIAS_REF = 0x0; // Default : 4'h4

    mbus_remote_register_write(SNT_ADDR,0x09,sntv1_r09.as_int);

    // Wakeup Counter
    sntv1_r17.WUP_CLK_SEL = 0x0; 
    sntv1_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
    mbus_remote_register_write(SNT_ADDR,0x17,sntv1_r17.as_int);

    // MRR Settings --------------------------------------

	// Decap in series
	mrrv7_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);
	mrrv7_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

	// Current Limter set-up 
	mrrv7_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);

    // Turn on Current Limter
    mrrv7_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
	delay(MBUS_DELAY*200); // Wait for decap to charge

	mrrv7_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
	mrrv7_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
	mbus_remote_register_write(MRR_ADDR,0x1F,mrrv7_r1F.as_int);

	//mrr_configure_pulse_width_short();
	mrr_configure_pulse_width_long();

	mrr_freq_hopping = 5;
	mrr_freq_hopping_step = 4;

	mrr_cfo_val_fine_min = 0x0000;

	// RO setup (SFO)
	// Adjust Diffusion R
	mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF

	// Adjust Poly R
	mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY

	// Adjust C
	mrrv7_r07.RO_MOM = 0x10;
	mrrv7_r07.RO_MIM = 0x10;
	mbus_remote_register_write(MRR_ADDR,0x07,mrrv7_r07.as_int);

	// TX Setup Carrier Freq
	mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = 0x0;  //ANT CAP 10b unary 830.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
	mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = 0x1F; //ANT CAP 10b unary 830.5 MHz
	mrrv7_r01.MRR_TRX_CAP_ANTP_TUNE_FINE = mrr_cfo_val_fine_min;  //ANT CAP 14b unary 830.5 MHz
	mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_FINE = mrr_cfo_val_fine_min; //ANT CAP 14b unary 830.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);
	mrrv7_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
	mbus_remote_register_write(MRR_ADDR,0x02,mrrv7_r02.as_int);

	// Turn off RX mode
    mrrv7_r03.MRR_TRX_MODE_EN = 0; //Set TRX mode

	// Keep decap charged in the background
	mrrv7_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)

	// Forces decaps to be parallel
	//mrrv7_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
	//mrrv7_r03.MRR_DCP_P_OW = 1;  //RX_Decap P 
	//mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

	// RX Setup
    mrrv7_r03.MRR_RX_BIAS_TUNE    = 0x02AF;//  turn on Q_enhancement
	//mrrv7_r03.MRR_RX_BIAS_TUNE    = 0x0001;//  turn off Q_enhancement
	mrrv7_r03.MRR_RX_SAMPLE_CAP    = 0x1;  // RX_SAMPLE_CAP
	mbus_remote_register_write(MRR_ADDR,3,mrrv7_r03.as_int);

	mrrv7_r14.MRR_RAD_FSM_RX_POWERON_LEN = 0x0;  //Set RX Power on length
	//mrrv7_r14.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x3;  //Set RX Sample length  16us
	mrrv7_r14.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x0;  //Set RX Sample length  4us
	mrrv7_r14.MRR_RAD_FSM_GUARD_LEN = 0x000F; //Set TX_RX Guard length, TX_RX guard 32 cycle (28+5)
	mbus_remote_register_write(MRR_ADDR,0x14,mrrv7_r14.as_int);

    mrrv7_r14.MRR_RAD_FSM_TX_POWERON_LEN = 2; //3bits
	mrrv7_r15.MRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
	mrrv7_r15.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
	mrrv7_r15.MRR_RAD_FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
	mbus_remote_register_write(MRR_ADDR,0x15,mrrv7_r15.as_int);

	// RAD_FSM set-up 
	// Using first 48 bits of data as header
	mbus_remote_register_write(MRR_ADDR,0x09,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0A,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0B,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0C,0x7AC800);
	mrrv7_r11.MRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
	mrrv7_r11.MRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv7_r11.as_int);

	mrrv7_r13.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
	mbus_remote_register_write(MRR_ADDR,0x13,mrrv7_r13.as_int);

	// Mbus return address
	mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_sns_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 2000;
	radio_packet_count = 0;
	
	astack_detection_mode = 0;
	adxl_enabled = 0;
	adxl_motion_detected = 0;

	adxl_trigger_mute_count = 0xFF;
	sleep_time_threshold_factor = 1;
	
	adxl_user_threshold = 0x060; // rec. 0x60, max 0x7FF
	sht35_user_repeatability = 0x0B; // default 0x0B

	wakeup_period_calc_factor = 18;

    SNT_0P5S_VAL = 1000;

    // Go to sleep without timer
    operation_sleep_notimer();
}

//***************************************************
// Temperature measurement operation
//***************************************************

static void operation_sns_run(void){
	if (stack_state == STK_IDLE){

		stack_state = STK_HUM;

		if (adxl_motion_detected || (astack_detection_mode & 0x2) || (astack_detection_mode == 0)){
			// Prepare for radio tx
			// Transmit if: Either motion change detected OR humidity sensing is on OR radio test mode
			if (!radio_on)
				radio_power_on();
			if (adxl_motion_detected){
				adxl_motion_count++;
				send_radio_data_mrr(0,0x2,adxl_motion_count);	
				// Need to grab SNT timer value
				snt_read_wup_counter();
				
			}
		}

    }else if (stack_state == STK_HUM){

		sht35_temp_data = 0;
		sht35_hum_data = 0;
		if (astack_detection_mode & 0x2){
			sht35_meas_data();
		}

		stack_state = STK_TEMP_READ;


	}else if (stack_state == STK_TEMP_READ){

		// Using SHT35's temp data
		// Wakeup time adjustment
		// Record temp difference from last wakeup adjustment
		if (sht35_temp_data > temp_storage_last_wakeup_adjust){
			temp_storage_diff = temp_storage_latest - temp_storage_last_wakeup_adjust;
		}else{
			temp_storage_diff = temp_storage_last_wakeup_adjust - temp_storage_latest;
		}
		
		if (wakeup_timer_option && ((temp_storage_diff > 10) || (exec_count < 1))){ // FIXME: value of 10 correct?
			measure_wakeup_period();
			temp_storage_last_wakeup_adjust = sht35_temp_data;
		}

		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCC, exec_count);
		mbus_write_message32(0xC0, read_data_temp);
		#endif
			
		exec_count++;
		stack_state = STK_IDLE;

		uint32_t sleep_time_threshold = WAKEUP_PERIOD_CONT>>sleep_time_threshold_factor; // how fast is too fast?
		if ((WAKEUP_PERIOD_CONT>>5) == 0) sleep_time_threshold = 3;

		// Check if motion is constantly triggering
		// Only works with PRC wakeup timer
		if (wakeup_timer_option && (astack_detection_mode & 0x1)){ // motion detection enabled
			if (sleep_time_prev < sleep_time_threshold){ // woke up fast
				if (adxl_motion_detected) adxl_motion_trigger_count++;
			}else{
				adxl_motion_trigger_count = 0;
			}

			if (adxl_motion_trigger_count > adxl_trigger_mute_count){
				// Triggering too much; stop ADXL
				if (adxl_enabled){
					// Need to reset interrupt
					ADXL362_power_off();
				}
			}else{
				if (adxl_enabled == 0){
					// Re-enable ADXL
					ADXL362_enable();
				}
			}
		}

		// Optionally transmit the data
		if (radio_on){
			
			uint32_t packet_code;

			if (adxl_motion_detected){
				// Motion Alert message
				if (adxl_enabled){ 
					packet_code = 0x0; // already transmitted
				}else{
					packet_code = 0x3; // Motion muted
				}
			}else{
				if (astack_detection_mode == 0x0){
					// Radio Test mode
					packet_code = 0x4;
				}else{
					// Check-in message
					packet_code = 0x1;
				}
			}

			if (packet_code != 0x0){
				send_radio_data_mrr(1,packet_code,(sht35_hum_data<<16) | (sht35_temp_data&0xFFFF));	
			}
		}
		
		// Make sure SDA and SCL are high
		gpio_write_data_with_mask(sht35_mask,(1<<GPIO_SDA) | (1<<GPIO_SCL));

		// Get ready for sleep
		if (astack_detection_mode & 0x1){
			if (adxl_enabled){
				// Reset ADXL flag
				adxl_motion_detected = 0;
	
				operation_spi_init();
				ADXL362_reg_rd(ADXL362_STATUS);
				//ADXL362_reg_rd(ADXL362_XDATA);
				//ADXL362_reg_rd(ADXL362_YDATA);
				//ADXL362_reg_rd(ADXL362_ZDATA);
				operation_spi_stop();
			}
		}


		// Make sure Radio is off
		if (radio_on){
			radio_power_off();
		}

		if (wakeup_timer_option){
			// Use PRC Timer
			set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
			operation_sleep();
		}else{
			// Use SNT Timer    
			snt_set_wup_timer(WAKEUP_PERIOD_SNT);
			operation_sleep_snt_timer();
		}

    }else{
        //default:  // THIS SHOULD NOT HAPPEN
		operation_sleep_notimer();
    }

}


static void operation_goc_trigger_init(void){

	// This is critical
	set_halt_until_mbus_tx();

	// Debug
	//mbus_write_message32(0xAA,0xABCD1234);
	//mbus_write_message32(0xAA,wakeup_data);

	// Initialize variables & registers
	sns_running = 0;
	stack_state = STK_IDLE;
	
	if (adxl_enabled) {
		ADXL362_power_off();
	}

	radio_power_off();
	//radio_packet_count = 0;
}

static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint8_t radio_tx_prefix, uint32_t radio_tx_data){

	// Prepare radio TX
	radio_power_on();
	exec_count_irq++;
	// radio
	send_radio_data_mrr(1,radio_tx_prefix,radio_tx_data);	

	if (exec_count_irq < radio_tx_num){
		// set timer
		set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();
		
	}else{
		exec_count_irq = 0;
		// Go to sleep without timer
		operation_sleep_notimer();
	}
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){

    // Only enable relevant interrupts (PRCv17)
	//enable_reg_irq();
	//enable_all_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

	// Figure out who triggered wakeup
	if(*SREG_WAKEUP_SOURCE & 0x00000008){
		// Debug
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xAA,0x11331133);
		#endif
		adxl_motion_detected = 1;
	}

	// Record previous sleep time
	sleep_time_prev = *REG_WUPT_VAL;
	// Debug
	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xAB, sleep_time_prev);
	#endif

    // Initialization sequence
    if (enumerated != 0x41481190){
        operation_init();
    }

	// Read latest batt voltage
	pmu_adc_read_latest();

    // Check if wakeup is due to GOC interrupt  
    // 0x8C is reserved for GOC-triggered wakeup (Named GOC_DATA_IRQ)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *GOC_DATA_IRQ;
    uint32_t wakeup_data_header = (wakeup_data>>24) & 0xFF;
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;

	// In case GOC triggered in the middle of routines
	if ((wakeup_data_header != 0) && (exec_count_irq == 0)){
		operation_goc_trigger_init();
	}

    if(wakeup_data_header == 1){
        // Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
		operation_goc_trigger_radio(wakeup_data_field_0 + (wakeup_data_field_2<<8), wakeup_data_field_1, 0x4, exec_count_irq);

    }else if(wakeup_data_header == 0x51){
		// Debug trigger for MRR testing; repeat trigger 1 for 0xFFFFFFFF times
		operation_goc_trigger_radio(0xFFFFFFFF, wakeup_data_field_1, 0x4, exec_count_irq);

    }else if(wakeup_data_header == 0x52){
		// Burst mode for MRR radio scanning
		// Prepare radio TX
		disable_timerwd();
		radio_power_on();
		uint32_t mrr_freq_hopping_saved = mrr_freq_hopping;
		mrr_freq_hopping = 0;
		uint32_t ii = 0;
		// Packet Loop 
		while (ii < (wakeup_data & 0xFFFFFF)){
			send_radio_data_mrr(0,0x4,ii);	
			ii++;
		}
			send_radio_data_mrr(1,0x4,ii);	

		mrr_freq_hopping = mrr_freq_hopping_saved;
		operation_sleep_notimer();

    }else if(wakeup_data_header == 0x14){
        // Update SNT wakeup counter value for 0.5s
        SNT_0P5S_VAL = wakeup_data & 0xFFFF;
        if (SNT_0P5S_VAL == 0){
            SNT_0P5S_VAL = 1000;
        }        

        // Go to sleep without timer
        operation_sleep_notimer();

	}else if(wakeup_data_header == 0x16){
		wakeup_period_calc_factor = wakeup_data & 0xFFFFFF;
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x18){
		// Manually override the SAR ratio
		pmu_set_sar_override(wakeup_data_field_0);
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x20){
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[23:8] is the desired chip id value
		//					-->  if zero: reports current chip id
		
		uint32_t chip_id_user;
		chip_id_user = (wakeup_data>>8) & 0xFFFF;

		if (chip_id_user == 0){
			chip_id_user = *REG_CHIP_ID;
		}else{
			*REG_CHIP_ID = chip_id_user;
		}

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xA, chip_id_user);


	}else if(wakeup_data_header == 0x21){
		// Change the radio tx packet delay
		uint32_t user_val = wakeup_data & 0xFFFFFF;
		if (user_val < 500){
			RADIO_PACKET_DELAY = 4000;
		}else{
			RADIO_PACKET_DELAY = user_val;
		}
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x22){
		// Change the carrier frequency of MRR (CFO)
		// Updated for MRRv7
		// wakeup_data[15:0]: Fine+Coarse setting
		// wakeup_data[23:16]: Turn on/off freq hopping 

		mrr_freq_hopping = wakeup_data_field_2 & 0xF;
		mrr_freq_hopping_step = wakeup_data_field_2 >> 4;

		mrr_cfo_val_fine_min = (wakeup_data >> 10) & 0x3F; // 6 bit
	
		mrrv7_r00.MRR_TRX_CAP_ANTP_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting 
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv7_r00.as_int);
		mrrv7_r01.MRR_TRX_CAP_ANTN_TUNE_COARSE = wakeup_data & 0x3FF; // 10 bit coarse setting
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv7_r01.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x23){
		// Change the baseband frequency of MRR (SFO)
		mrrv7_r07.RO_MOM = wakeup_data & 0x3F;
		mrrv7_r07.RO_MIM = wakeup_data & 0x3F;
		mbus_remote_register_write(MRR_ADDR,0x07,mrrv7_r07.as_int);
		
		// Go to sleep without timer
		operation_sleep_notimer();

/*
	}else if(wakeup_data_header == 0x24){
		// Switch between short / long pulse
		if (wakeup_data_field_0 == 0x1) {
			mrr_configure_pulse_width_long_3();
			
		}else if (wakeup_data_field_0 == 0x2){
			mrr_configure_pulse_width_long_2();
			
		}else if (wakeup_data_field_0 == 0x3){
			mrr_configure_pulse_width_long();

		}else if (wakeup_data_field_0 == 0x0){
			mrr_configure_pulse_width_short();
		}
		// Go to sleep without timer
		operation_sleep_notimer();
*/

    }else if(wakeup_data_header == 0x32){
		// Run temp measurement routine with desired wakeup period and ADXL running in the background
        // wakeup_data[15:0] is the user-specified period in seconds
        // wakeup_data[17] selects which sleep timer to use (1 is PRC, 0 is SNT)
        // wakeup_data[20]: enable motion detection with ADXL
		// wakeup_data[21]: enable humidity and temp measurement using SHT35
    	WAKEUP_PERIOD_CONT_USER = wakeup_data & 0xFFFF;

		wakeup_timer_option = wakeup_data & 0x20000;

		astack_detection_mode = (wakeup_data_field_2>>4) & 0x3;

        if (wakeup_timer_option){
            // Use PRC timer
            //WAKEUP_PERIOD_CONT_USER = wakeup_data & 0xFFFF; // Unit is 1s
        }else{
            // Use SNT timer
            WAKEUP_PERIOD_SNT = (WAKEUP_PERIOD_CONT_USER<<1)*SNT_0P5S_VAL; // Unit is 0.5s
        }

        exec_count_irq++;

        // Skip SNT timer settings if PRC timer is to be used
        if ((exec_count_irq == 1) && wakeup_timer_option)
            exec_count_irq = 3;

        if (exec_count_irq == 1){
            // SNT pulls higher current in the beginning
            pmu_set_sleep_radio();
            snt_start_timer_presleep();
            // Go to sleep for >3s for timer stabilization
            set_wakeup_timer (WAKEUP_PERIOD_RADIO_INIT*2, 0x1, 0x1);
            operation_sleep_noirqreset();
        }else if (exec_count_irq == 2){
            snt_start_timer_postsleep();
            // Read existing counter value; in case not reset to zero
            snt_read_wup_counter();
            // Restore sleep setting to low
            pmu_set_sleep_low();
        }

		// Prepare for Radio TX
		radio_power_on();

		if (astack_detection_mode & 0x1) ADXL362_enable();

		// Starting Operation
		send_radio_data_mrr(0,0x5,0x0);	

		sns_running = 1;
		set_sns_exec_count = 0;
		exec_count = 0;
		adxl_motion_trigger_count = 0;
		adxl_motion_count = 0;

		adxl_motion_detected = 0;

		// Reset GOC_DATA_IRQ
		*GOC_DATA_IRQ = 0;
        exec_count_irq = 0;

		// Run Temp Sensor Program
    	stack_state = STK_IDLE;
		operation_sns_run();

    }else if(wakeup_data_header == 0x33){
		// Stop temp & ADXL program and transmit the battery reading and execution count (alternating n times)
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 

		stack_state = STK_IDLE;
		astack_detection_mode = 0;

		// Stop ADXL
		if (adxl_enabled){
			ADXL362_power_off();
		}

		// Prepare radio TX
		radio_power_on();

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0x6, exec_count); 

	}else if(wakeup_data_header == 0x3A){
		// Change ADXL threshold
		adxl_user_threshold = wakeup_data & 0xFFFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x3C){
		// Change SHT35 repeatability setting
		sht35_user_repeatability = wakeup_data & 0xFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x3D){
		// Change ADXL mute settings
		adxl_trigger_mute_count = wakeup_data_field_0;
		sleep_time_threshold_factor = wakeup_data_field_1;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0xF0){
		// Report firmware version

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xB, enumerated);

	}else if(wakeup_data_header == 0xF1){
		// Report PRC's Chip ID (Serial Number)
		uint32_t puf_chip_id = 0;
		// Power Up PUF
		*REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
		// Wait (~20ms)
		delay(1000);
		// Release Isolation
		*REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x0/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);
		// Store the Chip ID
		puf_chip_id = *REG_PUF_CHIP_ID;
		// Power-Off PUF
		*REG_SYS_CONF = (0x1/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xC, puf_chip_id);

	}else if(wakeup_data_header == 0xFA){

		// Extra protection
		if ((wakeup_data&0xFFFFFF) == 0x89D7E2){
			config_timerwd(0xFF);
			while(1){
				mbus_write_message32(0xE0, 0x0);
				delay(MBUS_DELAY);
			}
		}else{
			operation_sleep_notimer();
		}

    }else{
		if (wakeup_data_header != 0){
			// Invalid GOC trigger
            // Go to sleep without timer
            operation_sleep_notimer();
		}
	}

	if (sns_running){
		// Proceed to continuous mode
		while(1){
			operation_sns_run();
		}
	}

	operation_sleep_notimer();

    while(1);
}


