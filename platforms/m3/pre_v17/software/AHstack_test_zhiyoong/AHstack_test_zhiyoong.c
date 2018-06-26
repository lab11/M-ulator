//*******************************************************************
//Author: Gyouho Kim
//Description: AM stack with MRRv6 and SNSv10, ADXL362
//			Modified from 'ARstack_ondemand_v1.7.c'
//			v1.7: CL unlimited during tx, shortening CLEN, pulse width
//			v1.8: Incorporate HRV light detection & clean up 
//			v1.9: PMU setting adjustment based on temp
//			v1.10: Use case update; send radio only when activity detected, check in every n wakeups
//                 Header increased to 96b
//				   Different thresholding for low light conditions
//			v1.11: Adding hard reset trigger; adding option to set decap in parallel
//			v1.12: Fixing high current issue when disabling ADXL after false trigger
//				   Making freq hopping configurable, only need the min cap setting now
//			v1.13: Fixing flag initialization in 0x32 trig, battery field in 0x3D packet
//				   Changing some default values
//					v1.13 used for MOD demo & delivery May 2018
//*******************************************************************
#include "PREv17.h"
#include "PREv17_RF.h"
#include "mbus.h"
#include "PMUv7_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// AM stack 
#define PMU_ADDR 0x6

#define WAKEUP_PERIOD_PARKING 60000 // About 2 hours (PRCv17)

// System parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)

#define DATA_STORAGE_SIZE 10 // Need to leave about 500 Bytes for stack --> around 60 words
#define TEMP_NUM_MEAS 1

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

#define GPIO_SDA 5
#define GPIO_SCL 6


//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t irq_history;
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t stack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_sar_conv_ratio_val;
volatile uint32_t read_data_batadc;
volatile uint32_t sleep_time_prev;
volatile uint32_t sleep_time_threshold_factor;
volatile uint32_t wakeup_period_calc_factor;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 


volatile uint32_t pmu_setting_temp_vals[3] = {0};

volatile uint32_t data_storage_count;
volatile uint32_t sns_running;
volatile uint32_t set_sns_exec_count;

volatile prev17_r0B_t prev17_r0B = PREv17_R0B_DEFAULT;
volatile prev17_r0D_t prev17_r0D = PREv17_R0D_DEFAULT;

//***************************************************
// SHT35 Functions
//***************************************************
static void operation_i2c_start(void);
static void operation_i2c_stop(void);
static void operation_i2c_addr(uint8_t addr, uint8_t RWn);
static void operation_i2c_cmd(uint8_t cmd);
static void operation_i2c_rd(uint8_t ACK);
static void operation_gpio_stop(void);

//***************************************************
// Sleep Functions
//***************************************************
static void operation_sleep(void);
static void operation_sleep_noirqreset(void);


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
}


//***************************************************
// ADXL362 Functions
//***************************************************

static void operation_i2c_start(){
  // Enable GPIO OUTPUT
  unfreeze_gpio_out();
  gpio_write_data((1<<GPIO_SDA) | (1<<GPIO_SCL));
  set_gpio_pad((1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_set_dir((1<<GPIO_SDA) | (1<<GPIO_SCL));
  //Start
  gpio_write_data((0<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((0<<GPIO_SDA) | (0<<GPIO_SCL));
}

static void operation_i2c_stop(){
  // Stop
  gpio_write_data((0<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((1<<GPIO_SDA) | (1<<GPIO_SCL));
}

static void operation_i2c_addr(uint8_t addr, uint8_t RWn){
  //Assume started
  //[6]
  gpio_set_dir((1<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((addr>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((addr>>6)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((addr>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[5]
  gpio_write_data((((addr>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((addr>>5)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((addr>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[4]
  gpio_write_data((((addr>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((addr>>4)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((addr>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[3]
  gpio_write_data((((addr>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((addr>>3)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((addr>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[2]
  gpio_write_data((((addr>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((addr>>2)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((addr>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[1]
  gpio_write_data((((addr>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((addr>>1)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((addr>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[0]
  gpio_write_data((((addr>>0)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((addr>>0)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((addr>>0)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  
  //WRITEn/READ
  //Need Hack
  if((RWn&0x1) == 1){ //Need hack
    gpio_set_dir((0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data(0<<GPIO_SCL);
    gpio_write_data(1<<GPIO_SCL);
    gpio_write_data(0<<GPIO_SCL);
  }
  else{
    gpio_write_data((0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_write_data((0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data((0<<GPIO_SDA) | (0<<GPIO_SCL));
  }

  //Wait for ACK
  gpio_set_dir((0<<GPIO_SDA) | (1<<GPIO_SCL));
  while((*GPIO_DATA>>GPIO_SDA)&0x1){
    //mbus_write_message32(0xCE, *GPIO_DATA);
  }
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  gpio_write_data(0<<GPIO_SCL);
}

static void operation_i2c_cmd(uint8_t cmd){
  gpio_set_dir((1<<GPIO_SDA) | (1<<GPIO_SCL));
  //[7]
  gpio_write_data((((cmd>>7)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((cmd>>7)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((cmd>>7)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[6]
  gpio_write_data((((cmd>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((cmd>>6)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((cmd>>6)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[5]
  gpio_write_data((((cmd>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((cmd>>5)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((cmd>>5)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[4]
  gpio_write_data((((cmd>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((cmd>>4)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((cmd>>4)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[3]
  gpio_write_data((((cmd>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((cmd>>3)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((cmd>>3)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[2]
  gpio_write_data((((cmd>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((cmd>>2)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((cmd>>2)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[1]
  gpio_write_data((((cmd>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  gpio_write_data((((cmd>>1)&0x1)<<GPIO_SDA) | (1<<GPIO_SCL));
  gpio_write_data((((cmd>>1)&0x1)<<GPIO_SDA) | (0<<GPIO_SCL));
  //[0]
  if((cmd&0x1) == 1){ //Need hack
    gpio_set_dir((0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data(0<<GPIO_SCL);
    gpio_write_data(1<<GPIO_SCL);
    gpio_write_data(0<<GPIO_SCL);
  }
  else{
    gpio_write_data((0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_write_data((0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data((0<<GPIO_SDA) | (0<<GPIO_SCL));
  }
  
  //Wait for ACK
  gpio_set_dir((0<<GPIO_SDA) | (1<<GPIO_SCL));
  while((*GPIO_DATA>>GPIO_SDA)&0x1){
    //mbus_write_message32(0xCF, *GPIO_DATA);
  }
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  gpio_write_data(0<<GPIO_SCL);
}

static void operation_i2c_rd(uint8_t ACK){

  uint8_t data;
  data = 0;
  gpio_set_dir((0<<GPIO_SDA) | (1<<GPIO_SCL));
  //[7]
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<7);
  //[6]
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<6);
  //[5]
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<5);
  //[4]
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<4);
  //[3]
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<3);
  //[2]
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<2);
  //[1]
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<1);
  //[0]
  gpio_write_data(0<<GPIO_SCL);
  gpio_write_data(1<<GPIO_SCL);
  data = data | ((*GPIO_DATA>>GPIO_SDA&0x1)<<0);
  gpio_write_data(0<<GPIO_SCL);
  if (ACK&0x1){
    gpio_write_data((0<<GPIO_SDA) | (0<<GPIO_SCL));
    gpio_set_dir((1<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data((0<<GPIO_SDA) | (1<<GPIO_SCL));
    gpio_write_data((0<<GPIO_SDA) | (0<<GPIO_SCL));
  }
  else{
  gpio_write_data(1<<GPIO_SCL);
  gpio_write_data(0<<GPIO_SCL);
  }
  mbus_write_message32(0xCF, data);
}


  static void operation_gpio_stop(){
  freeze_gpio_out();
}

//************************************
// PMU Related Functions
//************************************

static void pmu_set_sar_override(uint32_t val){
	// SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY*2);
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (val) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY*2);
}

inline static void pmu_set_adc_period(uint32_t val){
	// PMU_CONTROLLER_DESIRED_STATE Active
	mbus_remote_register_write(PMU_ADDR,0x3C,
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
		| (1 << 19) //state_state_horizon
	));
	delay(MBUS_DELAY*10);

	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,val); 
	delay(MBUS_DELAY*10);

	// Register 0x33: TICK_ADC_RESET
	mbus_remote_register_write(PMU_ADDR,0x33,2);
	delay(MBUS_DELAY);

	// Register 0x34: TICK_ADC_CLK
	mbus_remote_register_write(PMU_ADDR,0x34,2);
	delay(MBUS_DELAY);

	// PMU_CONTROLLER_DESIRED_STATE Active
	mbus_remote_register_write(PMU_ADDR,0x3C,
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
		| (1 << 19) //state_state_horizon
	));
	delay(MBUS_DELAY);
}

inline static void pmu_set_active_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

	// The first register write to PMU needs to be repeated
	// Register 0x16: V1P2 Active
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x18: V3P6 Active 
    mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x1A: V0P6 Active
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);

}

inline static void pmu_set_sleep_clk(uint8_t r, uint8_t l, uint8_t base, uint8_t l_1p2){

	// Register 0x17: V3P6 Sleep
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x15: V1P2 Sleep
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l_1p2 << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x19: V0P6 Sleep
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (0 << 13) // Enable main feedback loop
		| (r << 9)  // Frequency multiplier R
		| (l << 5)  // Frequency multiplier L (actually L+1)
		| (base) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);

}
inline static void pmu_set_clk_init(){
	pmu_set_active_clk(0xA,0x1,0x10,0x2);
	pmu_set_sleep_clk(0xF,0x0,0x1,0x1);
	// SAR_RATIO_OVERRIDE
	// Use the new reset scheme in PMUv3
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (0 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (0 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (0 << 7) // Enable override setting [6:0] (1'h0)
		| (0x45) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);
	pmu_set_sar_override(0x4D);

	pmu_set_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
}


inline static void pmu_adc_reset_setting(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Active
	mbus_remote_register_write(PMU_ADDR,0x3C,
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
		| (1 << 19) //state_state_horizon
	));
	delay(MBUS_DELAY);
}

inline static void pmu_adc_disable(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Sleep
	mbus_remote_register_write(PMU_ADDR,0x3B,
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
		| (1 << 19) //state_state_horizon
	));
	delay(MBUS_DELAY);
}

inline static void pmu_adc_enable(){
	// PMU ADC will be automatically reset when system wakes up
	// PMU_CONTROLLER_DESIRED_STATE Sleep
	mbus_remote_register_write(PMU_ADDR,0x3B,
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
		| (1 << 13) //state_sar_scn_ratio_adjusted
		| (1 << 14) //state_downconverter_on
		| (1 << 15) //state_downconverter_stabilized
		| (1 << 16) //state_vdd_3p6_turned_on
		| (1 << 17) //state_vdd_1p2_turned_on
		| (1 << 18) //state_vdd_0P6_turned_on
		| (1 << 19) //state_state_horizon
	));
	delay(MBUS_DELAY);
}


inline static void pmu_adc_read_latest(){

	// Grab latest PMU ADC readings
	// PMU register read is handled differently
	mbus_remote_register_write(PMU_ADDR,0x00,0x03);
	delay(MBUS_DELAY);
	read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;

}

inline static void pmu_parkinglot_decision_3v_battery(){
	
	// Battery > 3.0V
	if (read_data_batadc < (PMU_ADC_3P0_VAL)){
		pmu_set_sar_override(0x3C);

	// Battery 2.9V - 3.0V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 4){
		pmu_set_sar_override(0x3F);

	// Battery 2.8V - 2.9V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 8){
		pmu_set_sar_override(0x41);

	// Battery 2.7V - 2.8V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 12){
		pmu_set_sar_override(0x43);

	// Battery 2.6V - 2.7V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 17){
		pmu_set_sar_override(0x45);

	// Battery 2.5V - 2.6V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 21){
		pmu_set_sar_override(0x48);

	// Battery 2.4V - 2.5V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 27){
		pmu_set_sar_override(0x4B);

	// Battery 2.3V - 2.4V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 32){
		pmu_set_sar_override(0x4E);

	// Battery 2.2V - 2.3V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 39){
		pmu_set_sar_override(0x51);

	// Battery 2.1V - 2.2V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 46){
		pmu_set_sar_override(0x56);

	// Battery 2.0V - 2.1V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 53){
		pmu_set_sar_override(0x5A);

	// Battery <= 2.0V
	}else if (read_data_batadc < PMU_ADC_3P0_VAL + 53){
		pmu_set_sar_override(0x5F);

	}else{
		// Brown out
		pmu_set_sar_override(0x65);

		// Go to sleep without timer
		operation_sleep();

	}
	
}

inline static void pmu_reset_solar_short(){
    mbus_remote_register_write(PMU_ADDR,0x0E, 
		( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (1 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x0E, 
		( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (0 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (1 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
	delay(MBUS_DELAY);
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

static void operation_sleep_noirqreset(void){

    // Go to Sleep
    mbus_sleep_all();
    while(1);

}

static void operation_init(void){
  
  // Set CPU & Mbus Clock Speeds
  prev17_r0B.CLK_GEN_RING = 0x1; // Default 0x1
  prev17_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
  prev17_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
  prev17_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
  prev17_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
  *REG_CLKGEN_TUNE = prev17_r0B.as_int;

  prev17_r0D.SRAM_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
  prev17_r0D.SRAM_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
  prev17_r0D.SRAM_USE_INVERTER_SA= 1; 
  *REG_SRAM_TUNE = prev17_r0D.as_int;
  
  
  //Enumerate & Initialize Registers
  enumerated = 0xDEADBE13;
  exec_count = 0;
  exec_count_irq = 0;
  PMU_ADC_3P0_VAL = 0x62;
  pmu_parkinglot_mode = 3;
  
  // Set CPU Halt Option as RX --> Use for register read e.g.
  //    set_halt_until_mbus_rx();

  //Enumeration
  mbus_enumerate(PMU_ADDR);
  delay(MBUS_DELAY);

  // Set CPU Halt Option as TX --> Use for register write e.g.
  //    set_halt_until_mbus_tx();

  // PMU Settings ----------------------------------------------
  pmu_set_clk_init();
  pmu_reset_solar_short();

  // Disable PMU ADC measurement in active mode
  // PMU_CONTROLLER_STALL_ACTIVE
  mbus_remote_register_write(PMU_ADDR,0x3A, 
			     ( (1 << 19) // ignore state_horizon; default 1
			       | (1 << 13) // ignore adc_output_ready; default 0
			       | (1 << 12) // ignore adc_output_ready; default 0
			       | (1 << 11) // ignore adc_output_ready; default 0
			       ));
  delay(MBUS_DELAY);
  pmu_adc_reset_setting();
  delay(MBUS_DELAY);
  pmu_adc_enable();
  delay(MBUS_DELAY);

  // Initialize other global variables
  WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
  WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
  data_storage_count = 0;
  wakeup_data = 0;
	
  // Go to sleep without timer
  //operation_sleep();
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
  
  // Initialization sequence
  if (enumerated != 0xDEADBE13){
    operation_init();
  }

  *REG_CPS = 1;
  delay(1000);
  operation_i2c_start();
  operation_i2c_addr(0x44,0);
  operation_i2c_cmd(0x24);
  operation_i2c_cmd(0x0B);
  operation_i2c_stop();
  
  
  delay(1000);
  operation_i2c_start();
  operation_i2c_addr(0x44,0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x0);
  operation_i2c_stop();
  *REG_CPS = 0;


  delay(5000);
  *REG_CPS = 1;
  delay(1000);
  operation_i2c_start();
  operation_i2c_addr(0x44,0);
  operation_i2c_cmd(0x24);
  operation_i2c_cmd(0x0B);
  operation_i2c_stop();
  
  
  delay(1000);
  operation_i2c_start();
  operation_i2c_addr(0x44,0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x1);
  operation_i2c_rd(0x0);
  operation_i2c_stop();
  *REG_CPS = 0;


  //operation_i2c_start();
  //operation_i2c_addr(0x44,0x0);
  //operation_i2c_cmd(0x30);
  //operation_i2c_cmd(0xA2);
  //operation_i2c_stop();

  //operation_i2c_addr(0x44,0x1);
  //operation_i2c_rd();
  //operation_i2c_stop();
  

  while(1);
}
