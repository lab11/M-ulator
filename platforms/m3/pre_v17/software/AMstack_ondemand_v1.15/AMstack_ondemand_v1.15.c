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
//			v1.15: Trying to fix motion bug
//*******************************************************************
#include "PREv17.h"
#include "PREv17_RF.h"
#include "mbus.h"
#include "SNSv10_RF.h"
#include "PMUv7_RF.h"
#include "MRRv6_RF.h"
#include "ADXL362.h"
#include "HRVv5.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// AM stack 
#define HRV_ADDR 0x3
#define MRR_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

#define WAKEUP_PERIOD_PARKING 60000 // About 2 hours (PRCv17)

// System parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define SNS_CYCLE_INIT 3 

// Pstack states
#define	STK_IDLE		0x0
#define	STK_LDO		0x1
#define	STK_TEMP_START 0x2
#define	STK_TEMP_READ  0x3

// ADXL362 Defines
#define SPI_TIME 250

// Radio configurations
#define RADIO_DATA_LENGTH 168
#define WAKEUP_PERIOD_RADIO_INIT 10 // About 2 sec (PRCv17)

#define DATA_STORAGE_SIZE 10 // Need to leave about 500 Bytes for stack --> around 60 words
#define TEMP_NUM_MEAS 1

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

#define GPIO_ADXL_INT 0
#define GPIO_ADXL_EN 4

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

volatile uint32_t temp_storage[DATA_STORAGE_SIZE] = {0};
volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t read_data_temp;
volatile uint32_t pmu_setting_temp_vals[3] = {0};

volatile uint32_t data_storage_count;
volatile uint32_t sns_running;
volatile uint32_t set_sns_exec_count;

volatile uint32_t astack_detection_mode;
volatile uint32_t adxl_enabled;
volatile uint32_t adxl_motion_detected;
volatile uint32_t adxl_user_threshold;
volatile uint32_t adxl_motion_trigger_count;
volatile uint32_t adxl_trigger_mute_count;

volatile uint32_t hrv_light_count;
volatile uint32_t hrv_light_count_enabled;
volatile uint32_t hrv_light_count_prev;
volatile uint32_t hrv_light_diff;
volatile uint32_t hrv_light_threshold_factor;
volatile uint32_t hrv_low_light_threshold_factor;
volatile uint32_t hrv_low_light_threshold = 0x100;
volatile uint32_t hrv_light_detected;
volatile uint32_t hrv_exec_count = 0;
volatile uint32_t hrv_exec_checkin = 12;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_cfo_val_min;
volatile uint32_t mrr_set_decap_parallel;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

volatile snsv10_r00_t snsv10_r00 = SNSv10_R00_DEFAULT;
volatile snsv10_r01_t snsv10_r01 = SNSv10_R01_DEFAULT;
volatile snsv10_r03_t snsv10_r03 = SNSv10_R03_DEFAULT;
volatile snsv10_r17_t snsv10_r17 = SNSv10_R17_DEFAULT;

volatile prev17_r0B_t prev17_r0B = PREv17_R0B_DEFAULT;
volatile prev17_r0D_t prev17_r0D = PREv17_R0D_DEFAULT;

volatile mrrv6_r00_t mrrv6_r00 = MRRv6_R00_DEFAULT;
volatile mrrv6_r01_t mrrv6_r01 = MRRv6_R01_DEFAULT;
volatile mrrv6_r02_t mrrv6_r02 = MRRv6_R02_DEFAULT;
volatile mrrv6_r03_t mrrv6_r03 = MRRv6_R03_DEFAULT;
volatile mrrv6_r04_t mrrv6_r04 = MRRv6_R04_DEFAULT;
volatile mrrv6_r05_t mrrv6_r05 = MRRv6_R05_DEFAULT;
volatile mrrv6_r07_t mrrv6_r07 = MRRv6_R07_DEFAULT;
volatile mrrv6_r10_t mrrv6_r10 = MRRv6_R10_DEFAULT;
volatile mrrv6_r11_t mrrv6_r11 = MRRv6_R11_DEFAULT;
volatile mrrv6_r12_t mrrv6_r12 = MRRv6_R12_DEFAULT;
volatile mrrv6_r13_t mrrv6_r13 = MRRv6_R13_DEFAULT;
volatile mrrv6_r14_t mrrv6_r14 = MRRv6_R14_DEFAULT;
volatile mrrv6_r15_t mrrv6_r15 = MRRv6_R15_DEFAULT;
volatile mrrv6_r16_t mrrv6_r16 = MRRv6_R16_DEFAULT;

volatile mrrv6_r1E_t mrrv6_r1E = MRRv6_R1E_DEFAULT;
volatile mrrv6_r1F_t mrrv6_r1F = MRRv6_R1F_DEFAULT;


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
}


//***************************************************
// ADXL362 Functions
//***************************************************
static void ADXL362_reg_wr (uint8_t reg_id, uint8_t reg_data){
	gpio_write_data((0<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
	// Use Timer32 as timeout counter
	wfi_timeout_flag = 0;
	config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

	*SPI_SSPDR = 0x0A;
	*SPI_SSPDR = reg_id;
	*SPI_SSPDR = reg_data;
	*SPI_SSPCR1 = 0x2;
	while((*SPI_SSPSR>>4)&0x1){}//Spin

	// Turn off Timer32
	*TIMER32_GO = 0;
	if (wfi_timeout_flag){
		mbus_write_message32(0xFA, 0xFAFAFAFA);
	}

	gpio_write_data((1<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
	*SPI_SSPCR1 = 0x0;
}

static void ADXL362_reg_rd (uint8_t reg_id){
	gpio_write_data((0<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
	// Use Timer32 as timeout counter
	wfi_timeout_flag = 0;
	config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

	*SPI_SSPDR = 0x0B;
	*SPI_SSPDR = reg_id;
	*SPI_SSPDR = 0x00;
	*SPI_SSPCR1 = 0x2;
	while((*SPI_SSPSR>>4)&0x1){}//Spin

	// Turn off Timer32
	*TIMER32_GO = 0;
	if (wfi_timeout_flag){
		mbus_write_message32(0xFA, 0xFAFAFAFA);
	}

	gpio_write_data((1<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
	*SPI_SSPCR1 = 0x0;
	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xB1,*SPI_SSPDR);
	#endif
}

static void ADXL362_init(){
	// Soft Reset
	ADXL362_reg_wr(ADXL362_SOFT_RESET,0x52);

	// Check if Alive
	ADXL362_reg_rd(ADXL362_DEVID_AD);
	ADXL362_reg_rd(ADXL362_DEVID_MST);
	ADXL362_reg_rd(ADXL362_PARTID);
	ADXL362_reg_rd(ADXL362_REVID);

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
	*REG_CPS = 1;
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
	set_gpio_pad((1<<GPIO_ADXL_EN) | (1<<GPIO_ADXL_INT));
	gpio_set_dir((1<<GPIO_ADXL_EN) | (1<<GPIO_ADXL_INT));
	gpio_write_data(0);
	freeze_gpio_out();
	adxl_enabled = 0;
}

static void ADXL362_power_off(){
	ADXL362_stop();
	delay(MBUS_DELAY*10);
	*REG_CPS = 0;
	delay(MBUS_DELAY*200);
}

static void operation_spi_init(){
  *SPI_SSPCR0 =  0x0207; // Motorola Mode
  *SPI_SSPCPSR = 0x02;   // Clock Prescale Register

	// Enable SPI Input/Output
  set_spi_pad(1);
  set_gpio_pad((1<<GPIO_ADXL_EN) | (1<<GPIO_ADXL_INT));
  gpio_set_dir((1<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT));
  gpio_write_data((1<<GPIO_ADXL_EN) | (0<<GPIO_ADXL_INT)); // << Crashes here
  unfreeze_gpio_out();
  unfreeze_spi_out();
}

static void operation_spi_stop(){
  freeze_spi_out();
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
		operation_sleep_notimer();

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
// MRR Functions
//***************************************************

static void radio_power_on(){
	// Turn off PMU ADC
	//pmu_adc_disable();

	// Need to speed up sleep pmu clock
	//pmu_set_sleep_radio();

	// Set decap to series
	// FIXME: verify with Li
	mrrv6_r03.MRR_DCP_P_OW = 0;  //RX_Decap P 
	mbus_remote_register_write(MRR_ADDR,3,mrrv6_r03.as_int);
	mrrv6_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)
	mbus_remote_register_write(MRR_ADDR,3,mrrv6_r03.as_int);

	// Current Limter set-up 
	mrrv6_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

    radio_on = 1;

    // Turn on Current Limter
    mrrv6_r00.MRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

    // Release timer power-gate
    mrrv6_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv6_r04.as_int);
    delay(MBUS_DELAY*10);

	// Turn on timer
    mrrv6_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv6_r04.as_int);
    delay(MBUS_DELAY*10);

    mrrv6_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv6_r04.as_int);
    delay(MBUS_DELAY*10);

    mrrv6_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv6_r04.as_int);

    // Release FSM Sleep
    mrrv6_r11.MRR_RAD_FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv6_r11.as_int);
	delay(MBUS_DELAY*50); // Freq stab

}

static void radio_power_off(){
	// Need to restore sleep pmu clock

	// Enable PMU ADC
	//pmu_adc_enable();

	// Current Limter set-up 
	mrrv6_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

    // Turn off everything
    mrrv6_r03.MRR_TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(MRR_ADDR,0x03,mrrv6_r03.as_int);

    mrrv6_r11.MRR_RAD_FSM_EN = 0;  //Stop BB
    mrrv6_r11.MRR_RAD_FSM_RSTN = 0;  //RST BB
    mrrv6_r11.MRR_RAD_FSM_SLEEP = 1;
    mbus_remote_register_write(MRR_ADDR,0x11,mrrv6_r11.as_int);

    // Turn off Current Limter
    mrrv6_r00.MRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

    mrrv6_r04.RO_RESET = 1;  //Release Reset TIMER
    mrrv6_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    mrrv6_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv6_r04.as_int);

    // Enable timer power-gate
    mrrv6_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    mbus_remote_register_write(MRR_ADDR,0x04,mrrv6_r04.as_int);

	// Set decap to parallel
	if (mrr_set_decap_parallel){
		// FIXME: verify with Li
		mrrv6_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
		mbus_remote_register_write(MRR_ADDR,3,mrrv6_r03.as_int);
		mrrv6_r03.MRR_DCP_P_OW = 1;  //RX_Decap P 
		mbus_remote_register_write(MRR_ADDR,3,mrrv6_r03.as_int);
	}

    radio_on = 0;
	radio_ready = 0;

}

static void mrr_configure_pulse_width_long(){

    mrrv6_r12.MRR_RAD_FSM_TX_PW_LEN = 24; //100us PW
    mrrv6_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv6_r12.MRR_RAD_FSM_TX_PS_LEN = 49; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv6_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv6_r13.as_int);
}

static void mrr_configure_pulse_width_long_2(){

    mrrv6_r12.MRR_RAD_FSM_TX_PW_LEN = 19; //80us PW
    mrrv6_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv6_r12.MRR_RAD_FSM_TX_PS_LEN = 39; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv6_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv6_r13.as_int);
}

static void mrr_configure_pulse_width_long_3(){

    mrrv6_r12.MRR_RAD_FSM_TX_PW_LEN = 9; //40us PW
    mrrv6_r13.MRR_RAD_FSM_TX_C_LEN = 100; // (PW_LEN+1):C_LEN=1:32
    mrrv6_r12.MRR_RAD_FSM_TX_PS_LEN = 19; // PW=PS   

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv6_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv6_r13.as_int);
}

static void mrr_configure_pulse_width_short(){

    mrrv6_r12.MRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
    mrrv6_r13.MRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    mrrv6_r12.MRR_RAD_FSM_TX_PS_LEN = 1; // PW=PS guard interval betwen 0 and 1 pulse

    mbus_remote_register_write(MRR_ADDR,0x12,mrrv6_r12.as_int);
    mbus_remote_register_write(MRR_ADDR,0x13,mrrv6_r13.as_int);
}



static void send_radio_data_mrr_sub1(){

	// Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
	config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

    // Turn on Current Limter
    mrrv6_r00.MRR_CL_EN = 1;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

    // Fire off data
	mrrv6_r11.MRR_RAD_FSM_EN = 1;  //Start BB
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv6_r11.as_int);

	// Wait for radio response
	WFI();

	// Turn off Timer32
	*TIMER32_GO = 0;

	if (wfi_timeout_flag){
		mbus_write_message32(0xFA, 0xFAFAFAFA);
	}

    // Turn off Current Limter
    mrrv6_r00.MRR_CL_EN = 0;
    mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

	mrrv6_r11.MRR_RAD_FSM_EN = 0;
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv6_r11.as_int);
}

static void send_radio_data_mrr(uint32_t last_packet, uint32_t radio_data_0, uint32_t radio_data_1, uint32_t radio_data_2){
	// Sends 168 bit packet, of which 72b is actual data
	// MRR REG_9: reserved for header
	// MRR REG_A: reserved for header
	// MRR REG_B: reserved for header
	// MRR REG_C: reserved for header
	// MRR REG_D: DATA[23:0]
	// MRR REG_E: DATA[47:24]
	// MRR REG_F: DATA[71:48]
    mbus_remote_register_write(MRR_ADDR,0xD,radio_data_0);
    mbus_remote_register_write(MRR_ADDR,0xE,radio_data_1);
    mbus_remote_register_write(MRR_ADDR,0xF,radio_data_2);

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		mrrv6_r11.MRR_RAD_FSM_RSTN = 1;  //UNRST BB
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv6_r11.as_int);
		delay(MBUS_DELAY*10);

    	mrrv6_r03.MRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    	mbus_remote_register_write(MRR_ADDR,0x03,mrrv6_r03.as_int);
		delay(MBUS_DELAY*10);

		// Current Limter set-up 
		mrrv6_r00.MRR_CL_CTRL = 2; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

    }

	uint32_t count = 0;
	uint32_t mrr_cfo_val = 0;
	uint32_t num_packets = 1;
	if (mrr_freq_hopping) num_packets = mrr_freq_hopping;
	
	while (count < num_packets){
		mrr_cfo_val = mrr_cfo_val_min;
		uint32_t i;
		for(i=0; i<count;i++){
			mrr_cfo_val = (mrr_cfo_val << 1) | 0x1;
		}
		mrr_cfo_val = mrr_cfo_val & 0x3FFF; // 14 bits

		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCE, mrr_cfo_val);
		#endif

		mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = mrr_cfo_val; 
		mrrv6_r01.MRR_TRX_CAP_ANTN_TUNE = mrr_cfo_val;
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv6_r01.as_int);
		send_radio_data_mrr_sub1();
		if (count < num_packets){
			delay(RADIO_PACKET_DELAY);
		}
		count++;
	}

	radio_packet_count++;

	if (last_packet){
		radio_ready = 0;
		radio_power_off();
	}else{
		mrrv6_r11.MRR_RAD_FSM_EN = 0;
		mbus_remote_register_write(MRR_ADDR,0x11,mrrv6_r11.as_int);
	}
}
//***************************************************
// Temp Sensor Functions (SNSv10)
//***************************************************

static void temp_sensor_start(){
	snsv10_r01.TSNS_RESETn = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
}
static void temp_sensor_reset(){
	snsv10_r01.TSNS_RESETn = 0;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
}
static void temp_sensor_power_on(){
	// Turn on digital block
	snsv10_r01.TSNS_SEL_LDO = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
	// Turn on analog block
	snsv10_r01.TSNS_EN_SENSOR_LDO = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);

	delay(MBUS_DELAY);

	// Release isolation
	snsv10_r01.TSNS_ISOLATE = 0;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
}
static void temp_sensor_power_off(){
	snsv10_r01.TSNS_RESETn = 0;
	snsv10_r01.TSNS_SEL_LDO = 0;
	snsv10_r01.TSNS_EN_SENSOR_LDO = 0;
	snsv10_r01.TSNS_ISOLATE = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
}
static void sns_ldo_vref_on(){
	snsv10_r00.LDO_EN_VREF 	= 1;
	mbus_remote_register_write(SNS_ADDR,0,snsv10_r00.as_int);
}

static void sns_ldo_power_on(){
	snsv10_r00.LDO_EN_IREF 	= 1;
	snsv10_r00.LDO_EN_TSNS_OUT	= 1;
	mbus_remote_register_write(SNS_ADDR,0,snsv10_r00.as_int);
}
static void sns_ldo_power_off(){
	snsv10_r00.LDO_EN_VREF 	= 0;
	snsv10_r00.LDO_EN_IREF 	= 0;
	snsv10_r00.LDO_EN_TSNS_OUT	= 0;
	mbus_remote_register_write(SNS_ADDR,0,snsv10_r00.as_int);
}

//***************************************************
// HRV Functions
//***************************************************
static void hrv_light_reset(void){
  mbus_remote_register_write(HRV_ADDR,0x01,
			     ( (0  << 5)  // HRV_CNT_CNT_IRQ (default: 0)
			       | (0  << 4)  // HRV_CNT_CNT_ENABLE (default: 0)
			       | (2  << 2)  // LC_CLK_DIV (default: 2)
			       | (1)        // LC_CLK_RING (default: 1)
			       ));
	hrv_light_count_enabled = 0;
}
	
static void hrv_light_start(void){
  mbus_remote_register_write(HRV_ADDR,0x01,
			     ( (0  << 5)  // HRV_CNT_CNT_IRQ (default: 0)
			       | (1  << 4)  // HRV_CNT_CNT_ENABLE (default: 0)
			       | (2  << 2)  // LC_CLK_DIV (default: 2)
			       | (1)        // LC_CLK_RING (default: 1)
			       ));
	hrv_light_count = 0;
	hrv_light_count_enabled = 1;
}



//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sns_sleep_check(void){
	// Make sure LDO is off
	if (sns_running){
		sns_running = 0;
		temp_sensor_power_off();
		sns_ldo_power_off();
	}
}

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

static void operation_sleep_notimer(void){

	// Make sure the irq counter is reset    
    exec_count_irq = 0;

	if (hrv_light_count_enabled) hrv_light_reset();
	
	if (adxl_enabled){
		ADXL362_power_off();
	}

	operation_sns_sleep_check();	

    // Make sure Radio is off
    if (radio_on){radio_power_off();}

	// Check if sleep parking lot is on
	if (pmu_parkinglot_mode & 0x2){
		set_wakeup_timer(WAKEUP_PERIOD_PARKING,0x1,0x1);
	}else{
		// Disable Timer
		set_wakeup_timer(0, 0, 0);
	}

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
    stack_state = STK_IDLE; 	//0x0;
    enumerated = 0xDEADBE15;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_3P0_VAL = 0x62;
	pmu_parkinglot_mode = 3;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
//    set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(HRV_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(SNS_ADDR);
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

    // Temp Sensor Settings --------------------------------------

    // snsv10_r01
	snsv10_r01.TSNS_RESETn = 0;
    snsv10_r01.TSNS_EN_IRQ = 1;
    snsv10_r01.TSNS_BURST_MODE = 0;
    snsv10_r01.TSNS_CONT_MODE = 0;
    mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);

	// Set temp sensor conversion time
	snsv10_r03.TSNS_SEL_STB_TIME = 0x1; 
	snsv10_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
	mbus_remote_register_write(SNS_ADDR,0x03,snsv10_r03.as_int);

    // MRR Settings --------------------------------------

	mrr_set_decap_parallel = 0xF;
	mrrv6_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
	mrrv6_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
	mbus_remote_register_write(MRR_ADDR,0x1F,mrrv6_r1F.as_int);

	//mrr_configure_pulse_width_short();
	mrr_configure_pulse_width_long();

	mrr_freq_hopping = 5;

	mrr_cfo_val_min = 0x0000;

	// RO setup (SFO)
	// Adjust Diffusion R
	mbus_remote_register_write(MRR_ADDR,0x06,0x1000); // RO_PDIFF

	// Adjust Poly R
	mbus_remote_register_write(MRR_ADDR,0x08,0x400000); // RO_POLY

	// Adjust C
	mrrv6_r07.RO_MOM = 0x10;
	mrrv6_r07.RO_MIM = 0x10;
	mbus_remote_register_write(MRR_ADDR,0x07,mrrv6_r07.as_int);

	// TX Setup Carrier Freq
	mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = mrr_cfo_val_min;  //ANT CAP 14b unary 830.5 MHz
	//mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = 0x00FF;  //ANT CAP 14b unary 813.8 MHz
	//mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);
	mrrv6_r01.MRR_TRX_CAP_ANTN_TUNE = mrr_cfo_val_min; //ANT CAP 14b unary 830.5 MHz
	//mrrv6_r01.MRR_TRX_CAP_ANTN_TUNE = 0x00FF; //ANT CAP 14b unary 813.8 MHz
	//mrrv6_r01.MRR_TRX_CAP_ANTN_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x01,mrrv6_r01.as_int);
	mrrv6_r02.MRR_TX_BIAS_TUNE = 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
	mbus_remote_register_write(MRR_ADDR,0x02,mrrv6_r02.as_int);

	// Turn off RX mode
    mrrv6_r03.MRR_TRX_MODE_EN = 0; //Set TRX mode

	// Keep decap charged in the background
	mrrv6_r03.MRR_DCP_S_OW = 1;  //TX_Decap S (forced charge decaps)

	// Forces decaps to be parallel
	//mrrv6_r03.MRR_DCP_S_OW = 0;  //TX_Decap S (forced charge decaps)
	//mrrv6_r03.MRR_DCP_P_OW = 1;  //RX_Decap P 
	//mbus_remote_register_write(MRR_ADDR,3,mrrv6_r03.as_int);

	// RX Setup
    mrrv6_r03.MRR_RX_BIAS_TUNE    = 0x02AF;//  turn on Q_enhancement
	//mrrv6_r03.MRR_RX_BIAS_TUNE    = 0x0001;//  turn off Q_enhancement
	mrrv6_r03.MRR_RX_SAMPLE_CAP    = 0x1;  // RX_SAMPLE_CAP
	mbus_remote_register_write(MRR_ADDR,3,mrrv6_r03.as_int);

	mrrv6_r14.MRR_RAD_FSM_RX_POWERON_LEN = 0x0;  //Set RX Power on length
	//mrrv6_r14.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x3;  //Set RX Sample length  16us
	mrrv6_r14.MRR_RAD_FSM_RX_SAMPLE_LEN = 0x0;  //Set RX Sample length  4us
	mrrv6_r14.MRR_RAD_FSM_GUARD_LEN = 0x000F; //Set TX_RX Guard length, TX_RX guard 32 cycle (28+5)
	mbus_remote_register_write(MRR_ADDR,0x14,mrrv6_r14.as_int);

    mrrv6_r14.MRR_RAD_FSM_TX_POWERON_LEN = 2; //3bits
	mrrv6_r15.MRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
	mrrv6_r15.MRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
	mrrv6_r15.MRR_RAD_FSM_RX_DATA_BITS = 0x00; //Set RX data 1b
	mbus_remote_register_write(MRR_ADDR,0x15,mrrv6_r15.as_int);

	// RAD_FSM set-up 
	// Using first 48 bits of data as header
	mbus_remote_register_write(MRR_ADDR,0x09,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0A,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0B,0x0);
	mbus_remote_register_write(MRR_ADDR,0x0C,0x7AC800);
	mrrv6_r11.MRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
	mrrv6_r11.MRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv6_r11.as_int);

	mrrv6_r13.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
	mbus_remote_register_write(MRR_ADDR,0x13,mrrv6_r13.as_int);

	// Mbus return address
	mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);


	// HRV
	hrv_light_reset();
	mbus_remote_register_write(HRV_ADDR,0x00,6); // HRV_TOP_CONV_RATIO(0~15 >> 9x~23x); default: 14
	mbus_remote_register_write(HRV_ADDR,0x03,0xFFFFFF); // HRV_CNT_CNT_THRESHOLD (default: 0xFFFFFF) 
	mbus_remote_register_write(HRV_ADDR,0x04,0x001000); // HRV_CNT_IRQ_PACKET (default: 0x001400) 
	hrv_light_count = 0;

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    data_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_sns_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 2500;
	radio_packet_count = 0;
	
	astack_detection_mode = 0;
	adxl_enabled = 0;
	adxl_motion_detected = 0;
	hrv_light_count = 0;
	hrv_light_count_prev = 0;
	hrv_light_detected = 0;
	hrv_exec_checkin = 12;

	hrv_low_light_threshold_factor = 1;
	hrv_low_light_threshold = 0x100;

	adxl_trigger_mute_count = 2;
	sleep_time_threshold_factor = 1;
	
	hrv_light_threshold_factor = 1;
	adxl_user_threshold = 0x060; // rec. 0x60, max 0x7FF

	wakeup_period_calc_factor = 16;

    // Go to sleep without timer
    operation_sleep_notimer();
}

//***************************************************
// Temperature measurement operation
//***************************************************

static void operation_sns_run(void){

	if (stack_state == STK_IDLE){

		wfi_timeout_flag = 0;
		meas_count = 0;

		stack_state = STK_LDO;

		// Turn on SNS LDO VREF; requires settling
		sns_ldo_vref_on();

    }else if (stack_state == STK_LDO){
		stack_state = STK_TEMP_START;

		// Power on SNS LDO
		sns_ldo_power_on();

		// Power on temp sensor
		temp_sensor_power_on();
		delay(MBUS_DELAY);

	}else if (stack_state == STK_TEMP_START){
		// Start temp measurement

		wfi_timeout_flag = 0;

		// Use Timer32 as timeout counter
		config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Start Temp Sensor
		temp_sensor_start();

		// Wait for temp sensor output
		WFI();

		// Turn off Timer32
		*TIMER32_GO = 0;
		stack_state = STK_TEMP_READ;

	}else if (stack_state == STK_TEMP_READ){

		// Grab Temp Sensor Data
		if (wfi_timeout_flag){
			read_data_temp = 0xFAFA;
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			// Read register
			set_halt_until_mbus_rx();
			mbus_remote_register_read(SNS_ADDR,0x6,1);
			read_data_temp = *REG1;
			set_halt_until_mbus_tx();
			temp_storage_latest = read_data_temp;
		}
		meas_count++;

		// Option to take multiple measurements per wakeup
		if (meas_count < TEMP_NUM_MEAS){	
			// Repeat measurement while awake
			temp_sensor_reset();
			stack_state = STK_TEMP_START;
				
		}else{
			// Last measurement from this wakeup
			meas_count = 0;

			// Assert temp sensor isolation & turn off temp sensor power
			temp_sensor_power_off();
			sns_ldo_power_off();
			stack_state = STK_IDLE;

			// Wakeup time adjustment
			// Record temp difference from last wakeup adjustment
			if (temp_storage_latest > temp_storage_last_wakeup_adjust){
				temp_storage_diff = temp_storage_latest - temp_storage_last_wakeup_adjust;
			}else{
				temp_storage_diff = temp_storage_last_wakeup_adjust - temp_storage_latest;
			}
			#ifdef DEBUG_MBUS_MSG
				mbus_write_message32(0xEA, temp_storage_diff);
				delay(MBUS_DELAY);
			#endif
			
			if ((temp_storage_diff > 200) || (exec_count < (SNS_CYCLE_INIT))){ // FIXME: value of 20 correct?
				measure_wakeup_period();
				temp_storage_last_wakeup_adjust = temp_storage_latest;
			}
				

			#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xCC, exec_count);
			mbus_write_message32(0xC0, read_data_temp);
			#endif
				
			exec_count++;

			// Store results in memory; unless buffer is full
			if (data_storage_count < DATA_STORAGE_SIZE){
				temp_storage[data_storage_count] = read_data_temp;
				radio_tx_count = data_storage_count;
				data_storage_count++;
			}

			uint32_t sleep_time_threshold = WAKEUP_PERIOD_CONT>>sleep_time_threshold_factor; // how fast is too fast?
			if ((WAKEUP_PERIOD_CONT>>5) == 0) sleep_time_threshold = 3;

			// Check if motion is constantly triggering
			if (astack_detection_mode & 0x1){ // motion detection enabled
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
			// Transmit if: Either motion or light change detected OR
			// 				Running in motion only mode (no frequent wakeup) OR
			//				Light detection is running (hence frequent wakeup) and haven't sent out a radio for hrv_exec_checkin times
			if (radio_tx_option && (adxl_motion_detected || hrv_light_detected || (astack_detection_mode == 0x1) || (hrv_exec_count > hrv_exec_checkin))){
				// Read latest PMU ADC measurement

				// Prepare for radio tx
				radio_power_on();
				if (adxl_motion_detected || hrv_light_detected){
					send_radio_data_mrr(1, 0x1D0000 | (*REG_CHIP_ID & 0xFFFF), 0xB00000| (read_data_batadc<<12) | (adxl_enabled<<8) | (hrv_light_count_enabled<<9) | (hrv_light_detected<<4) | adxl_motion_detected, (0xF&radio_packet_count)<<20 | 0xA0000 | (0xFFFF & read_data_temp));
			#ifdef DEBUG_MBUS_MSG
				    radio_power_on();
					send_radio_data_mrr(1, 0xFD0000 | (*REG_CHIP_ID & 0xFFFF), hrv_light_count_prev, (0xF&radio_packet_count)<<20 | hrv_light_count);
			#endif
				}else{
					// Check-in message
					send_radio_data_mrr(1, 0x2D0000 | (*REG_CHIP_ID & 0xFFFF), 0xB00000| (read_data_batadc<<12) | (adxl_enabled<<8) | (hrv_light_count_enabled<<9), (0xF&radio_packet_count)<<20 | 0xA0000 | (0xFFFF & read_data_temp));
				}
                
                // Reset Check-in message count
                hrv_exec_count = 0;
			}

			// Get ready for sleep

			// Make sure Radio is off
			if (radio_on){
				radio_power_off();
			}
	
			// Start HRV Light Counter
			hrv_light_count_prev = hrv_light_count;
			hrv_light_detected = 0;
			if (astack_detection_mode & 0x2) hrv_light_start();
		
			if (astack_detection_mode > 0){
				// Clear any ADXL interrupt
				if (adxl_enabled){
					// Reset ADXL flag
					adxl_motion_detected = 0;
		
					operation_spi_init();
					ADXL362_reg_rd(ADXL362_STATUS);
					operation_spi_stop();
				}
					
				set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);

			}else{
				// Radio Test mode
				// Prepare for radio tx
				radio_power_on();
				// Send debug signal
				send_radio_data_mrr(1, 0x4D0000 | (*REG_CHIP_ID & 0xFFFF), 0xB00000| (read_data_batadc<<12),(0xF&radio_packet_count)<<20 | 0xA0000 | exec_count & 0xFFFF);
				set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
			}

			if ((set_sns_exec_count != 0) && (exec_count > (50<<set_sns_exec_count))){
				// No more measurement required
				// Make sure temp sensor is off
				sns_running = 0;
				operation_sleep_notimer();
			}else{
				operation_sleep();
			}
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
	temp_sensor_power_off();
	sns_ldo_power_off();

	radio_packet_count = 0;
}

static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint32_t radio_tx_data0, uint32_t radio_tx_data1){

	// Prepare radio TX
	radio_power_on();

	if (exec_count_irq < radio_tx_num){
		exec_count_irq++;

		// radio
		send_radio_data_mrr(1, 0xFD0000 | (*REG_CHIP_ID & 0xFFFF), 0xB00000 | (0xFFFFF & radio_tx_data0), (0xF&radio_packet_count)<<20 | 0xA0000 | (0xFFFF & radio_tx_data1));
		// set timer
		set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();
		
	}else{
		exec_count_irq = 0;
		// radio
		send_radio_data_mrr(1,0xFD0000 | (*REG_CHIP_ID & 0xFFFF), 0xB00000, (0xF&radio_packet_count)<<20 | 0xA0000);	
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
	//Woke up via Wakeup Timer
	if(*SREG_WAKEUP_SOURCE & 0x00000002){
	}

	// Record previous sleep time
	sleep_time_prev = *REG_WUPT_VAL;
	// Debug
	#ifdef DEBUG_MBUS_MSG
	mbus_write_message32(0xAB, sleep_time_prev);
	#endif

    // Initialization sequence
    if (enumerated != 0xDEADBE15){
        operation_init();
    }

	// Parking lot feature
	if (pmu_parkinglot_mode > 0){
		//mbus_write_message32(0xAA,0xABCDDCBA);
		pmu_adc_read_latest();
		pmu_parkinglot_decision_3v_battery();
	}

	// Woke up to check light change, not because of motion
	if ((astack_detection_mode & 0x2) && !adxl_motion_detected){
		// Check light count
		set_halt_until_mbus_rx();
		mbus_remote_register_read(HRV_ADDR,0x02,1);
		hrv_light_count = *((volatile uint32_t *) REG1);
		set_halt_until_mbus_tx();
		// First measurement will be off
		if (hrv_exec_count > 0){
			if (hrv_light_count_prev > hrv_light_count){
				hrv_light_diff = hrv_light_count_prev - hrv_light_count;
			}else{
				hrv_light_diff = hrv_light_count - hrv_light_count_prev;
			}
			if (hrv_light_count_prev < hrv_low_light_threshold){
				// Low light condition
				if (hrv_light_diff > (hrv_light_count_prev<<hrv_low_light_threshold_factor)) hrv_light_detected = 1;
		
			}else{
				if (hrv_light_diff > (hrv_light_count_prev>>hrv_light_threshold_factor)) hrv_light_detected = 1;
			}
		}
			
		// Debug
		#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xAC,(hrv_light_detected<<23) | hrv_light_count);
		#endif

		hrv_light_reset();

		hrv_exec_count++;
	}

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
		operation_goc_trigger_radio(wakeup_data_field_0 + (wakeup_data_field_2<<8), wakeup_data_field_1, 0xABC000, exec_count_irq);

    }else if(wakeup_data_header == 0x51){
		// Debug trigger for MRR testing; repeat trigger 1 for 0xFFFFFFFF times
		operation_goc_trigger_radio(0xFFFFFFFF, wakeup_data_field_1, 0xABC000, exec_count_irq);

/*
    }else if(wakeup_data_header == 2){
		// Run temp measurement routine with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:17] is the initial user-specified period (LSB assumed to be 0)
		// wakeup_data[16] is HT mode -- only save data above a certain HT
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[23:21] specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
    	WAKEUP_PERIOD_CONT_USER = (wakeup_data_field_0 + (wakeup_data_field_1<<8));
        WAKEUP_PERIOD_CONT_INIT = (wakeup_data_field_2 & 0xE);
        radio_tx_option = wakeup_data_field_2 & 0x10;

		astack_detection_mode = 0;

		sns_running = 1;
		set_sns_exec_count = wakeup_data_field_2 >> 5;
		exec_count = 0;
		meas_count = 0;
		data_storage_count = 0;
		radio_tx_count = 0;

		// Reset GOC_DATA_IRQ
		*GOC_DATA_IRQ = 0;
        exec_count_irq = 0;

		// Run Temp Sensor Program
    	stack_state = STK_IDLE;
		operation_sns_run();

    }else if(wakeup_data_header == 3){
		// Stop temp sensor program and transmit the battery reading and execution count (alternating n times)
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		operation_sns_sleep_check();
	
		stack_state = STK_IDLE;

		// Prepare radio TX
		radio_power_on();

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Read latest PMU ADC measurement
				pmu_adc_read_latest();
			}
			send_radio_data_mrr(1,0x1D0000 | (*REG_CHIP_ID & 0xFFFF),0xBBB000+read_data_batadc,0xC00000 | (0xFFFFF & exec_count));
			// set timer
			set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
			// go to sleep and wake up with same condition
			operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // radio
			send_radio_data_mrr(1,0x1D0000 | (*REG_CHIP_ID & 0xFFFF), 0xFAF000,0);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }


    }else if(wakeup_data_header == 4){
        // Transmit the stored sns data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		operation_sns_sleep_check();
		
		stack_state = STK_IDLE;

		radio_tx_numdata = wakeup_data_field_0;

		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= data_storage_count){
			radio_tx_numdata = 0;
		}
		
		// Prepare radio TX
		radio_power_on();

        if (exec_count_irq < 3){
			exec_count_irq++;
			if (exec_count_irq == 1){
				// Read latest PMU ADC measurement
				pmu_adc_read_latest();
			}

			send_radio_data_mrr(1,0x1D0000 | (*REG_CHIP_ID & 0xFFFF),0xBBB000+read_data_batadc,0xC00000 | (0xFFFFF & exec_count));
			set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
			// go to sleep and wake up with same condition
			operation_sleep_noirqreset();

		}else{
			operation_tx_stored();
		}
	*/	
    }else if(wakeup_data_header == 7){
		// Status Inquiry - Transmit PMU's ADC reading & Chip ID
		// wakeup_data[7:0] is the # of transmissions
		// wakeup_data[15:8] is the user-specified period 
		// wakeup_data[16] enables parking lot feature (harvesting on/off based on PMU ADC reading)
		// wakeup_data[17] enables parking lot feature in sleep mode (wakes up occasionally) 
		pmu_parkinglot_mode = wakeup_data_field_2 & 0x3;

		if (exec_count_irq == 0){
			// Read latest PMU ADC measurement
			pmu_adc_read_latest();

			if (pmu_parkinglot_mode > 0){
				// Automatic adjustment of PMU ratio based on batt voltage
				pmu_parkinglot_decision_3v_battery();
			}
		}

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xB00000| (read_data_batadc<<12), exec_count_irq);

	}else if(wakeup_data_header == 0x16){
		wakeup_period_calc_factor = wakeup_data & 0xFFFFFF;
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x17){
		// Set parking lot threshold
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the desired parking lot threshold value
		//					-->  if zero: uses the latest PMU ADC measurement
		
		if (wakeup_data_field_2 == 0){
			// Read latest PMU ADC measurement
			pmu_adc_read_latest();
			PMU_ADC_3P0_VAL = read_data_batadc;
		}else{
			PMU_ADC_3P0_VAL = wakeup_data_field_2;
		}

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xB00000| (read_data_batadc<<12), PMU_ADC_3P0_VAL);

	}else if(wakeup_data_header == 0x18){
		// Manually override the SAR ratio
		pmu_set_sar_override(wakeup_data_field_0);
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x19){
		// Report PMU's SAR conversion ratio
		// wakeup_data[7:0] is the # of transmissions
		// wakeup_data[15:8] is the user-specified period 
		if (exec_count_irq == 0){
			// Read PMU register 5
			// PMU register read is handled differently
			mbus_remote_register_write(PMU_ADDR,0x00,0x05);
			delay(MBUS_DELAY);
			delay(MBUS_DELAY);
			pmu_sar_conv_ratio_val = *((volatile uint32_t *) REG0) & 0x7F;
		}

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xB00000| (read_data_batadc<<12), pmu_sar_conv_ratio_val);

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

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xB00000| (read_data_batadc<<12), chip_id_user);


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
		// wakeup_data[23:16]: Turn on/off freq hopping 
		mrr_freq_hopping = wakeup_data_field_2;

		mrr_cfo_val_min = wakeup_data & 0x0FFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x23){
		// Change the baseband frequency of MRR (SFO)
		mrrv6_r07.RO_MOM = wakeup_data & 0x3F;
		mrrv6_r07.RO_MIM = wakeup_data & 0x3F;
		mbus_remote_register_write(MRR_ADDR,0x07,mrrv6_r07.as_int);
		
		// Go to sleep without timer
		operation_sleep_notimer();

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

	}else if(wakeup_data_header == 0x25){
		// Change the conversion time of the temp sensor

		snsv10_r03.TSNS_SEL_CONV_TIME = wakeup_data & 0xF; // Default: 0x6
		mbus_remote_register_write(SNS_ADDR,0x03,snsv10_r03.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x26){
		// Change the temp thresholds for different PMU settings
		// wakeup_data[23:20] == 0: Low temp (<20C)
		// wakeup_data[23:20] == 1: Room temp
		// wakeup_data[23:20] == 2: High temp (>45C)

		pmu_setting_temp_vals[wakeup_data_field_2>>4] = wakeup_data & 0xFFFFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x2C){
		// Change C_LEN

		mrrv6_r13.MRR_RAD_FSM_TX_C_LEN = wakeup_data & 0xFFFF; // (PW_LEN+1):C_LEN=1:32
		mbus_remote_register_write(MRR_ADDR,0x13,mrrv6_r13.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x2D){
		mrr_set_decap_parallel = wakeup_data & 0xF;
		// Go to sleep without timer
		operation_sleep_notimer();

		
    }else if(wakeup_data_header == 0x32){
		// Run temp measurement routine with desired wakeup period and ADXL running in the background
        // wakeup_data[15:0] is the user-specified period in seconds
        // wakeup_data[19:17] is the initial user-specified period (LSB assumed to be 0)
        // wakeup_data[20]: enable motion detection with ADXL
		// wakeup_data[21]: enable light detection with HRV
    	WAKEUP_PERIOD_CONT_USER = (wakeup_data_field_0 + (wakeup_data_field_1<<8));
        WAKEUP_PERIOD_CONT_INIT = (wakeup_data_field_2 & 0xE);
        radio_tx_option = 1;

		astack_detection_mode = (wakeup_data_field_2>>4) & 0x3;

		if (astack_detection_mode & 0x1) ADXL362_enable();

		sns_running = 1;
		set_sns_exec_count = 0;
		exec_count = 0;
		hrv_exec_count = 0;
		meas_count = 0;
		data_storage_count = 0;
		radio_tx_count = 0;
		adxl_motion_trigger_count = 0;

		adxl_motion_detected = 0;
		hrv_light_detected = 0;

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
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		operation_sns_sleep_check();
	
		stack_state = STK_IDLE;
		astack_detection_mode = 0;

		// Stop ADXL
		if (adxl_enabled){
			ADXL362_power_off();
		}

		// Stop light counting
		hrv_light_reset();

		// Prepare radio TX
		radio_power_on();

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Read latest PMU ADC measurement
				pmu_adc_read_latest();
			}
			send_radio_data_mrr(1,0x3D0000 | (*REG_CHIP_ID & 0xFFFF), 0xB00000| (read_data_batadc<<12), (0xF&radio_packet_count)<<20 | 0xA0000 | (0xFFFF & exec_count));
			// set timer
			set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
			// go to sleep and wake up with same condition
			operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // radio
			send_radio_data_mrr(1,0x3D0000 | (*REG_CHIP_ID & 0xFFFF), 0xB00000| (read_data_batadc<<12), (0xF&radio_packet_count)<<20 | 0xA0000 | (0xFFFF & exec_count));
            // Go to sleep without timer
            operation_sleep_notimer();
        }

	}else if(wakeup_data_header == 0x3A){
		// Change ADXL threshold
		adxl_user_threshold = wakeup_data & 0xFFFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x3B){
		// Change Light Thresold
		hrv_light_threshold_factor = wakeup_data & 0xFFFFFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x3C){
		// Change how often to send check-in message
		// Set to zero to TX every wakeup
		hrv_exec_checkin = wakeup_data & 0xFFFFFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x3D){
		// Change ADXL mute settings
		adxl_trigger_mute_count = wakeup_data_field_0;
		sleep_time_threshold_factor = wakeup_data_field_1;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x3E){
		// Change Low Light Thresold
		hrv_low_light_threshold = wakeup_data & 0x00FFFF;
		hrv_low_light_threshold_factor = wakeup_data_field_2;

		// Go to sleep without timer
		operation_sleep_notimer();

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


