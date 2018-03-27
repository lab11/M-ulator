//*******************************************************************
//Author: Gyouho Kim
//Description: AR stack with MRR and SNSv10
//			Modified from 'Tstack_ondemand_v3.2_BAT3V'
//			v1.0: PREv17, SNSv10, PMUv7, mrrv6
//			v1.2: increasing guard interval, optional freq hopping
//			v1.3: increasing PMU strength for V1P2 and V3P6
//			      ability to switch between short/long pulse
//			v1.5: MRRv6; MRR decap configured to remain charged
//			v1.6: CLEN default changed to 300; skip sleeping for timer settling
//				  radio needs to be turned on/off for every wakeup
//				  optimizing current limiter setting to minimize v drop
//			v1.7: CL unlimited during tx, shortening CLEN, pulse width
//*******************************************************************
#include "PREv17.h"
#include "PREv17_RF.h"
#include "mbus.h"
#include "SNSv10_RF.h"
#include "PMUv7_RF.h"
#include "MRRv6_RF.h"

// uncomment this for debug mbus message
// #define DEBUG_MBUS_MSG
//#define DEBUG_MBUS_MSG_1

// AR stack order  PRE->SNS->MRR->PMU
#define MRR_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// System parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define SNS_CYCLE_INIT 3 

// Pstack states
#define	STK_IDLE		0x0
#define	STK_LDO		0x1
#define	STK_TEMP_START 0x2
#define	STK_TEMP_READ  0x3

// Radio configurations
#define RADIO_DATA_LENGTH 148
#define WAKEUP_PERIOD_RADIO_INIT 10 // About 2 sec (PRCv17)

#define DATA_STORAGE_SIZE 400 // Need to leave about 500 Bytes for stack --> around 60 words
#define TEMP_NUM_MEAS 2

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

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
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t PMU_ADC_3P0_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_harvesting_on;
volatile uint32_t pmu_sar_conv_ratio_val;
volatile uint32_t read_data_batadc;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t temp_storage[DATA_STORAGE_SIZE] = {0};
volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t read_data_temp;

volatile uint32_t data_storage_count;
volatile uint32_t sns_run_single;
volatile uint32_t sns_run_ht_mode;
volatile uint32_t sns_running;
volatile uint32_t set_sns_exec_count;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t mrr_freq_hopping;
volatile uint32_t mrr_cfo_vals[3] = {0};
volatile uint32_t RADIO_PACKET_DELAY;

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
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
    *SREG_WAKEUP_SOURCE = 0;
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

inline static void pmu_set_sleep_radio(){
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (10 << 9)  // Frequency multiplier R
		| (10 << 5)  // Frequency multiplier L (actually L+1)
		| (5) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (10 << 9)  // Frequency multiplier R
		| (10 << 5)  // Frequency multiplier L (actually L+1)
		| (5) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x17: V3P6 Upconverter Sleep Settings
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (2 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (5) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
}

inline static void pmu_set_sleep_low(){
	// Register 0x17: V3P6 Upconverter Sleep Settings
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
}


inline static void pmu_set_clk_init(){
	// Register 0x17: V3P6 Upconverter Sleep Settings
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// The first register write to PMU needs to be repeated
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x18: V3P6 Upconverter Active Settings
    mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (12 << 5)  // Frequency multiplier L (actually L+1)
		| (16) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (0 << 13) // Enable main feedback loop
		| (4 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (16) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x15: V1P2 SAR_TRIM_v3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x16: V1P2 SAR_TRIM_v3_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (4 << 9)  // Frequency multiplier R
		| (12 << 5)  // Frequency multiplier L (actually L+1)
		| (16) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
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
	}else{
		pmu_set_sar_override(0x5F);
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

	// Current Limter set-up 
	mrrv6_r00.MRR_CL_CTRL = 16; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

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

    radio_on = 1;

}

static void radio_power_off(){
	// Need to restore sleep pmu clock
	//pmu_set_sleep_low();

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
	// Sends 148 bit packet, of which 72b is actual data
	// MRR REG_9: reserved for header
	// MRR REG_A: reserved for header
	// MRR REG_B: reserved for header
	// MRR REG_C: DATA[23:0]
	// MRR REG_D: DATA[47:24]
	// MRR REG_E: DATA[71:48]
    mbus_remote_register_write(MRR_ADDR,0xC,radio_data_0);
    mbus_remote_register_write(MRR_ADDR,0xD,radio_data_1);
    mbus_remote_register_write(MRR_ADDR,0xE,radio_data_2);

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
	uint32_t num_packets = 1;
	if (mrr_freq_hopping) num_packets = 3;

	while (count < num_packets){
	
		mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = mrr_cfo_vals[count]; 
		mrrv6_r01.MRR_TRX_CAP_ANTN_TUNE = mrr_cfo_vals[count];
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv6_r01.as_int);
		send_radio_data_mrr_sub1();
		if (count < num_packets){
			delay(RADIO_PACKET_DELAY);
		}
		count++;
	}

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


static void operation_tx_stored(void){

    //Fire off stored data to radio
    while(((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > data_storage_count))){
		#ifdef DEBUG_MBUS_MSG_1
			mbus_write_message32(0xDD, radio_tx_count);
		#endif

		// Reset watchdog timer
		config_timerwd(TIMERWD_VAL);

		
		// Radio out data
		send_radio_data_mrr(0, 0x1D0000 | (*REG_CHIP_ID & 0xFFFF), 0xCC0000 | (radio_tx_count & 0xFFFF), 0xFFFFFF & temp_storage[radio_tx_count]);
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet

		radio_tx_count--;
    }

	send_radio_data_mrr(0, 0x1D0000 | (*REG_CHIP_ID & 0xFFFF), 0xCC0000 | (radio_tx_count & 0xFFFF), 0xFFFFFF & temp_storage[radio_tx_count]);

	delay(RADIO_PACKET_DELAY*2); //Set delays between sending subsequent packet
	send_radio_data_mrr(1,0x1D0000 | (*REG_CHIP_ID & 0xFFFF), 0xFAF000,0);	

	// This is also the end of this IRQ routine
	exec_count_irq = 0;

	// Go to sleep without timer
	radio_tx_count = data_storage_count; // allows data to be sent more than once
	operation_sleep_notimer();
}

uint32_t dumb_divide(uint32_t nu, uint32_t de) {
// Returns quotient of nu/de

    uint32_t temp = 1;
    uint32_t quotient = 0;

		#ifdef DEBUG_MBUS_MSG_1
	mbus_write_message32(0xAA, nu);
	mbus_write_message32(0xBB, de);
		#endif

    while (de <= nu) {
        de <<= 1;
        temp <<= 1;
    }

		#ifdef DEBUG_MBUS_MSG_1
	mbus_write_message32(0xAA, nu);
	mbus_write_message32(0xBB, de);
		#endif

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

	mbus_write_message32(0xE0, 0x0);
	// Prevent watchdogs from kicking in
   	config_timerwd(TIMERWD_VAL*2);
	*REG_MBUS_WD = 1500000*3; // default: 1500000

	uint32_t wakeup_timer_val_0 = *REG_WUPT_VAL;
	wakeup_period_count = 0;

	while( *REG_WUPT_VAL <= wakeup_timer_val_0 + 1){
		wakeup_period_count = 0;
	}
	while( *REG_WUPT_VAL <= wakeup_timer_val_0 + 2){
		wakeup_period_count++;
	}
	mbus_write_message32(0xE1, wakeup_timer_val_0);
	mbus_write_message32(0xE2, wakeup_period_count);

   	config_timerwd(TIMERWD_VAL);
	WAKEUP_PERIOD_CONT = dumb_divide(WAKEUP_PERIOD_CONT_USER*1000*8, wakeup_period_count);
    if (WAKEUP_PERIOD_CONT > 0x7FFF){
        WAKEUP_PERIOD_CONT = 0x7FFF;
    }
	mbus_write_message32(0xED, WAKEUP_PERIOD_CONT); 
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
    enumerated = 0xDEADBEE1;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_3P0_VAL = 0x62;
	pmu_parkinglot_mode = 3;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
//    set_halt_until_mbus_rx();

    //Enumeration
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
	snsv10_r03.TSNS_SEL_CONV_TIME = 0x3; // Default: 0x6
	mbus_remote_register_write(SNS_ADDR,0x03,snsv10_r03.as_int);

    // MRR Settings --------------------------------------

	mrrv6_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
	mrrv6_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
	mbus_remote_register_write(MRR_ADDR,0x1F,mrrv6_r1F.as_int);

	//mrr_configure_pulse_width_short();
	mrr_configure_pulse_width_long();

	mrr_freq_hopping = 1;

	mrr_cfo_vals[0] = 0x0001;
	mrr_cfo_vals[1] = 0x0003;
	mrr_cfo_vals[2] = 0x0000;

	mrr_cfo_vals[0] = 0x01FF;
	mrr_cfo_vals[1] = 0x03FF;
	mrr_cfo_vals[2] = 0x00FF;


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
	mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = mrr_cfo_vals[0];  //ANT CAP 14b unary 830.5 MHz
	//mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = 0x00FF;  //ANT CAP 14b unary 813.8 MHz
	//mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
	mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);
	mrrv6_r01.MRR_TRX_CAP_ANTN_TUNE = mrr_cfo_vals[0]; //ANT CAP 14b unary 830.5 MHz
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
	mbus_remote_register_write(MRR_ADDR,0x0B,0x7AC800);
	mrrv6_r11.MRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
	mrrv6_r11.MRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
	mbus_remote_register_write(MRR_ADDR,0x11,mrrv6_r11.as_int);

	mrrv6_r13.MRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
	mbus_remote_register_write(MRR_ADDR,0x13,mrrv6_r13.as_int);

	// Mbus return address
	mbus_remote_register_write(MRR_ADDR,0x1E,0x1002);


    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    data_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    sns_run_single = 0;
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_sns_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 2500;

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
			#ifdef DEBUG_MBUS_MSG_1
				mbus_write_message32(0xEA, temp_storage_diff);
				delay(MBUS_DELAY);
			#endif
			
			// FIXME: for now, do this every time					
			//measure_wakeup_period();
			
			if ((temp_storage_diff > 10) || (exec_count < (SNS_CYCLE_INIT+5))){ // FIXME: value of 20 correct?
				measure_wakeup_period();
				temp_storage_last_wakeup_adjust = temp_storage_latest;
			}
				

			mbus_write_message32(0xCC, exec_count);
			mbus_write_message32(0xC0, read_data_temp);
				
			exec_count++;

			// Store results in memory; unless buffer is full
			if (data_storage_count < DATA_STORAGE_SIZE){
				temp_storage[data_storage_count] = read_data_temp;
				radio_tx_count = data_storage_count;
				data_storage_count++;
			}

			// Optionally transmit the data
			if (radio_tx_option){
				// Read latest PMU ADC measurement
				pmu_adc_read_latest();

				// Prepare for radio tx
				radio_power_on();
				send_radio_data_mrr(1, 0x1D0000 | (*REG_CHIP_ID & 0xFFFF), 0xBBB000| read_data_batadc, 0xD00000 | (0xFFFFF & read_data_temp));
				delay(RADIO_PACKET_DELAY);
			}

			// Enter long sleep
			if (exec_count < SNS_CYCLE_INIT){
				// Prepare for radio tx
				radio_power_on();
				// Send some signal
				send_radio_data_mrr(1, 0x1D0000 | (*REG_CHIP_ID & 0xFFFF), 0xABC000, 0);
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);

			}else{	
				set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
			}

			// Make sure Radio is off
			if (radio_on){
				radio_power_off();
			}

			if (sns_run_single){
				sns_run_single = 0;
				sns_running = 0;
				operation_sleep_notimer();
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
	mbus_write_message32(0xAA,0xABCD1234);
	mbus_write_message32(0xAA,wakeup_data);

	// Initialize variables & registers
	sns_running = 0;
	stack_state = STK_IDLE;
	
	radio_power_off();
	temp_sensor_power_off();
	sns_ldo_power_off();
}

static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint32_t radio_tx_data0, uint32_t radio_tx_data1){

	// Prepare radio TX
	radio_power_on();

	if (exec_count_irq < radio_tx_num){
		exec_count_irq++;

		// radio
		send_radio_data_mrr(1, 0x1D0000 | (*REG_CHIP_ID & 0xFFFF), radio_tx_data0, radio_tx_data1);
		// set timer
		set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();
		
	}else{
		exec_count_irq = 0;
		// radio
		send_radio_data_mrr(1,0x1D0000 | (*REG_CHIP_ID & 0xFFFF), 0xFAF000,0);	
		// Go to sleep without timer
		operation_sleep_notimer();
	}
}
//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    // Only enable relevant interrupts (PRCv17)
	//enable_reg_irq();
	//enable_all_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

    // Initialization sequence
    if (enumerated != 0xDEADBEE1){
        operation_init();
    }

	// Parking lot feature
	if (pmu_parkinglot_mode > 0){
		//mbus_write_message32(0xAA,0xABCDDCBA);
		pmu_adc_read_latest();
		pmu_parkinglot_decision_3v_battery();
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
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
		operation_goc_trigger_radio(wakeup_data_field_0 + (wakeup_data_field_2<<8), wakeup_data_field_1, 0xABC000, exec_count_irq);

    }else if(wakeup_data_header == 0x51){
		// Debug trigger for MRR testing; repeat trigger 1 for 0xFFFFFFFF times
		operation_goc_trigger_radio(0xFFFFFFFF, wakeup_data_field_1, 0xABC000, exec_count_irq);

    }else if(wakeup_data_header == 0x71){
		// Debug trigger for MRR testing; repeat trigger 1 for 0xFFFFFFFF times
		operation_goc_trigger_radio(0xFFFFFFFF, 0x10, wakeup_data & 0xFFFFFF, exec_count_irq);

    }else if(wakeup_data_header == 2){
		// Slow down PMU sleep osc and run temp sensor code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:17] is the initial user-specified period (LSB assumed to be 0)
		// wakeup_data[16] is HT mode -- only save data above a certain HT
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[23:21] specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
    	WAKEUP_PERIOD_CONT_USER = (wakeup_data_field_0 + (wakeup_data_field_1<<8));
        WAKEUP_PERIOD_CONT_INIT = (wakeup_data_field_2 & 0xE);
        radio_tx_option = wakeup_data_field_2 & 0x10;

		sns_run_single = 0;
		sns_run_ht_mode = (wakeup_data_field_2) & 0x1;
		

		if (!sns_running){
			// Go to sleep for initial settling of temp sensing // FIXME
			//set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
			sns_running = 1;
			set_sns_exec_count = wakeup_data_field_2 >> 5;
            exec_count_irq++;
			//operation_sleep_noirqreset();
		}
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
		
    }else if(wakeup_data_header == 7){
		// Transmit PMU's ADC reading as a battery voltage indicator
		// wakeup_data[7:0] is the # of transmissions
		// wakeup_data[15:8] is the user-specified period 
		// wakeup_data[16] enables parking lot feature (harvesting on/off based on PMU ADC reading)
		// wakeup_data[17] enables parking lot feature in sleep mode (wakes up occasionally) 
		// wakeup_data[18] starts charging 
		pmu_parkinglot_mode = wakeup_data_field_2 & 0x3;

		if (exec_count_irq == 0){
			// Read latest PMU ADC measurement
			pmu_adc_read_latest();

			if (pmu_parkinglot_mode > 0){
				// Solar short based on PMU ADC reading
				pmu_parkinglot_decision_3v_battery();
			}else if (pmu_parkinglot_mode == 0){
				// Start harvesting and let solar short be determined in hardware
				pmu_reset_solar_short();
				pmu_harvesting_on = 1;
			}

		}

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xBBB000, read_data_batadc);


/*
    }else if(wakeup_data_header == 0x14){
		// Run temp sensor once to update room temperature reference
        radio_tx_option = 1;
		sns_run_single = 1;
		sns_running = 1;

		exec_count = 0;
		meas_count = 0;
		data_storage_count = 0;
		radio_tx_count = 0;

		// Reset GOC_DATA_IRQ
		*GOC_DATA_IRQ = 0;
        exec_count_irq = 0;

		// Run Temp Sensor Program
		operation_sns_run();

*/

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

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xABC000, PMU_ADC_3P0_VAL);

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

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xABC000, pmu_sar_conv_ratio_val);

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

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0xCC0000, chip_id_user);


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

		mrr_cfo_vals[0] = wakeup_data & 0x3FFF;
		mrrv6_r00.MRR_TRX_CAP_ANTP_TUNE = mrr_cfo_vals[0]; 
		mrrv6_r01.MRR_TRX_CAP_ANTN_TUNE = mrr_cfo_vals[0];
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);
		mbus_remote_register_write(MRR_ADDR,0x01,mrrv6_r01.as_int);

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

	}else if(wakeup_data_header == 0x77){
	// temporary trigger

		mrrv6_r13.MRR_RAD_FSM_TX_C_LEN = wakeup_data & 0xFFFF; // (PW_LEN+1):C_LEN=1:32
		mbus_remote_register_write(MRR_ADDR,0x13,mrrv6_r13.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x78){
	// temporary trigger

		// Current Limter set-up 
		mrrv6_r00.MRR_CL_CTRL = wakeup_data & 0xFF; //Set CL 1: unlimited, 8: 30uA, 16: 3uA
		mbus_remote_register_write(MRR_ADDR,0x00,mrrv6_r00.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();


	}else if(wakeup_data_header == 0x25){
		// Change the conversion time of the temp sensor

		snsv10_r03.TSNS_SEL_CONV_TIME = wakeup_data & 0xF; // Default: 0x6
		mbus_remote_register_write(SNS_ADDR,0x03,snsv10_r03.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x2A){
		mrr_cfo_vals[1] = wakeup_data & 0x3FFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x2B){
		mrr_cfo_vals[2] = wakeup_data & 0x3FFF;

		// Go to sleep without timer
		operation_sleep_notimer();


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


