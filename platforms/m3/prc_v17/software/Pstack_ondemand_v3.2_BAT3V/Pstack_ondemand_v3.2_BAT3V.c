//*******************************************************************
//Author: Gyouho Kim
//Description: Pressure Sensing system with RDC and new temp sensor
//			Modified from 'Tstack_ondemand_v2.5_PMU_3V' and 'Pstack_ondemand_rdc_v1.0'
//			v3.0: PRCv17, SNSv10, PMUv7, RDCv1
//			v3.1: Adding trig X that only records if temp > threshold
//				  RDCv1 offset configuration
//				  Trig4 now reports battery and execution count in the beginning
//			v3.2: Improving wakeup timer measurement; WUPT max value is 0x7FFF
//			      Trig2 with cont TX has headers for data
//*******************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "mbus.h"
#include "RDCv1_RF.h"
#include "SNSv10_RF.h"
#include "RADv9.h"
#include "PMUv7_RF.h"

// uncomment this for debug mbus message
// #define DEBUG_MBUS_MSG
//#define DEBUG_MBUS_MSG_1

// TStack order  PRC->RAD->SNS->RDC->PMU
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6
#define RDC_ADDR 0x7

#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// System parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define SNS_CYCLE_INIT 3 

// Pstack states
#define	PSTK_IDLE		0x0
#define	PSTK_LDO		0x1
#define	PSTK_TEMP_START 0x2
#define	PSTK_TEMP_READ  0x3
#define PSTK_RDC_RUN 	0x4
#define PSTK_RDC_READ  	0x5

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define WAKEUP_PERIOD_RADIO_INIT 10 // About 2 sec (PRCv17)

#define DATA_STORAGE_SIZE 200 // Need to leave about 500 Bytes for stack --> around 60 words
#define RDC_NUM_MEAS 2 
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
volatile uint32_t Pstack_state;
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

volatile uint32_t rdc_storage[DATA_STORAGE_SIZE] = {0};
volatile uint32_t rdc_data[RDC_NUM_MEAS] = {0};
volatile uint32_t data_storage_count;
volatile uint32_t sns_run_single;
volatile uint32_t sns_run_ht_mode;
volatile uint32_t sns_run_ht_threshold;
volatile uint32_t sns_running;
volatile uint32_t set_sns_exec_count;
volatile uint32_t read_data_rdc;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t RADIO_PACKET_DELAY;

volatile snsv10_r00_t snsv10_r00 = SNSv10_R00_DEFAULT;
volatile snsv10_r01_t snsv10_r01 = SNSv10_R01_DEFAULT;
volatile snsv10_r03_t snsv10_r03 = SNSv10_R03_DEFAULT;
volatile snsv10_r17_t snsv10_r17 = SNSv10_R17_DEFAULT;

volatile rdcv1_r11_t rdcv1_r11 = RDCv1_R11_DEFAULT;
volatile rdcv1_r12_t rdcv1_r12 = RDCv1_R12_DEFAULT;
volatile rdcv1_r13_t rdcv1_r13 = RDCv1_R13_DEFAULT;
volatile rdcv1_r14_t rdcv1_r14 = RDCv1_R14_DEFAULT;
volatile rdcv1_r15_t rdcv1_r15 = RDCv1_R15_DEFAULT;
volatile rdcv1_r16_t rdcv1_r16 = RDCv1_R16_DEFAULT;
volatile rdcv1_r1A_t rdcv1_r1A = RDCv1_R1A_DEFAULT;
volatile rdcv1_r1B_t rdcv1_r1B = RDCv1_R1B_DEFAULT;
volatile rdcv1_r1C_t rdcv1_r1C = RDCv1_R1C_DEFAULT;
volatile rdcv1_r1D_t rdcv1_r1D = RDCv1_R1D_DEFAULT;
  
volatile radv9_r0_t radv9_r0 = RADv9_R0_DEFAULT;
volatile radv9_r1_t radv9_r1 = RADv9_R1_DEFAULT;
volatile radv9_r2_t radv9_r2 = RADv9_R2_DEFAULT;
volatile radv9_r3_t radv9_r3 = RADv9_R3_DEFAULT;
volatile radv9_r4_t radv9_r4 = RADv9_R4_DEFAULT;
volatile radv9_r5_t radv9_r5 = RADv9_R5_DEFAULT;
volatile radv9_r11_t radv9_r11 = RADv9_R11_DEFAULT;
volatile radv9_r12_t radv9_r12 = RADv9_R12_DEFAULT;
volatile radv9_r13_t radv9_r13 = RADv9_R13_DEFAULT;
volatile radv9_r14_t radv9_r14 = RADv9_R14_DEFAULT;

volatile prcv17_r0B_t prcv17_r0B = PRCv17_R0B_DEFAULT;

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
		| (14 << 9)  // Frequency multiplier R
		| (14 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (0 << 13) // Enable main feedback loop
		| (14 << 9)  // Frequency multiplier R
		| (14 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x17: V3P6 Upconverter Sleep Settings
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (2 << 9)  // Frequency multiplier R
		| (3 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
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
		| (2 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
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
		| (0 << 5)  // Frequency multiplier L (actually L+1)
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
// Radio transmission routines for PPM Radio (RADv9)
//***************************************************

static void radio_power_on(){

	// Turn off PMU ADC
	pmu_adc_disable();

	// Need to speed up sleep pmu clock
	pmu_set_sleep_radio();
	
	// This can be safely assumed
	radio_ready = 0;

    // Release FSM Sleep - Requires >2s stabilization time
    radio_on = 1;
    radv9_r13.RAD_FSM_SLEEP = 0;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);
    // Release SCRO Reset
    radv9_r2.SCRO_RESET = 0;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    
    // Additional delay required after SCRO Reset release
    delay(MBUS_DELAY*3); // At least 20ms required
    
    // Enable SCRO
    radv9_r2.SCRO_ENABLE = 1;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    delay(MBUS_DELAY);

	// Release FSM Isolate
	radv9_r13.RAD_FSM_ISOLATE = 0;
	mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);

}

static void radio_power_off(){

	// Need to restore sleep pmu clock
	pmu_set_sleep_low();
	
	// Enable PMU ADC
	pmu_adc_enable();

    // Turn off everything
    radio_on = 0;
	radio_ready = 0;
    radv9_r2.SCRO_ENABLE = 0;
    radv9_r2.SCRO_RESET  = 1;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    radv9_r13.RAD_FSM_SLEEP 	= 1;
    radv9_r13.RAD_FSM_ISOLATE 	= 1;
    radv9_r13.RAD_FSM_RESETn 	= 0;
    radv9_r13.RAD_FSM_ENABLE 	= 0;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
}

static void send_radio_data_ppm(uint32_t last_packet, uint32_t radio_data){

    // Write Data: Only up to 24bit data for now
	radv9_r3.RAD_FSM_DATA = radio_data;
    mbus_remote_register_write(RAD_ADDR,3,radv9_r3.as_int);

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		radv9_r13.RAD_FSM_RESETn = 1;
		mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
		delay(MBUS_DELAY);
    }

	// Use Timer32 as timeout counter
	config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

    // Fire off data
	wfi_timeout_flag = 0;
    radv9_r13.RAD_FSM_ENABLE = 1;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);

	// Wait for radio response
	WFI();

	// Turn off Timer32
	*TIMER32_GO = 0;

	if (wfi_timeout_flag){
		mbus_write_message32(0xBB, 0xFAFAFAFA);
	}

	if (last_packet){
		radio_ready = 0;
		radio_power_off();
	}else{
		radv9_r13.RAD_FSM_ENABLE = 0;
		mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
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
// RDC Functions (RDCv1)
//***************************************************

static void rdc_enable(){
	rdcv1_r12.BRDC_RSTB = 1;
	mbus_remote_register_write(RDC_ADDR,0x12,rdcv1_r12.as_int);
}
static void rdc_disable(){
	rdcv1_r12.BRDC_RSTB = 0;
	mbus_remote_register_write(RDC_ADDR,0x12,rdcv1_r12.as_int);
}
static void rdc_osc_enable(){
	rdcv1_r12.BRDC_OSC_RESET = 0;
	rdcv1_r12.BRDC_IB_ENB = 0;
	mbus_remote_register_write(RDC_ADDR,0x12,rdcv1_r12.as_int);
}
static void rdc_osc_disable(){
	rdcv1_r12.BRDC_OSC_RESET = 1;
	rdcv1_r12.BRDC_IB_ENB = 1;
	mbus_remote_register_write(RDC_ADDR,0x12,rdcv1_r12.as_int);
}
static void rdc_release_v1p2_pg(){
	rdcv1_r13.BRDC_V1P2_PG_EN = 0;
	mbus_remote_register_write(RDC_ADDR,0x13,rdcv1_r13.as_int);
}
static void rdc_release_pg(){
	rdcv1_r13.BRDC_V3P6_PG_EN = 0;
	rdcv1_r13.BRDC_OSC_V1P2_PG_EN = 0;
	mbus_remote_register_write(RDC_ADDR,0x13,rdcv1_r13.as_int);
}
static void rdc_release_isolate(){
	rdcv1_r13.BRDC_V3P6_ISOLB = 1;
	rdcv1_r13.BRDC_OSC_ISOLATE = 0;
	mbus_remote_register_write(RDC_ADDR,0x13,rdcv1_r13.as_int);
}
static void rdc_assert_pg(){ 
	rdcv1_r13.BRDC_V1P2_PG_EN = 1;
	rdcv1_r13.BRDC_V3P6_PG_EN = 1;
	rdcv1_r13.BRDC_OSC_V1P2_PG_EN = 1;
	rdcv1_r13.BRDC_V3P6_ISOLB = 0;
	rdcv1_r13.BRDC_OSC_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,0x13,rdcv1_r13.as_int);
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
		rdc_assert_pg();
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
			mbus_write_message32(0xDD, rdc_storage[radio_tx_count]);
		#endif

		// Reset watchdog timer
		config_timerwd(TIMERWD_VAL);

		// Radio out data
		send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & temp_storage[radio_tx_count]));
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		send_radio_data_ppm(0, 0xE00000 | (0xFFFFF & rdc_storage[radio_tx_count]));
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet

		radio_tx_count--;
    }

	send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & temp_storage[radio_tx_count]));
	delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
	send_radio_data_ppm(0, 0xE00000 | (0xFFFFF & rdc_storage[radio_tx_count]));

	delay(RADIO_PACKET_DELAY*2); //Set delays between sending subsequent packet
	send_radio_data_ppm(1, 0xFAF000);

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
    prcv17_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    prcv17_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prcv17_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	*REG_CLKGEN_TUNE = prcv17_r0B.as_int;

  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEE2;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_3P0_VAL = 0x62;
	pmu_parkinglot_mode = 3;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
//    set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(RAD_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(SNS_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(RDC_ADDR);
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

	// RDC Settings ----------------------------------------
	rdcv1_r1A.BRDC_OSR= 32;
	rdcv1_r1A.BRDC_SAMPLE_TIME_SEL = 15;
	rdcv1_r1A.BRDC_TICK_SPL_TO_SPL = 0;
	rdcv1_r1A.BRDC_TICK_VREF_SPL_REDIS_WAIT = 2;
	mbus_remote_register_write(RDC_ADDR,0x1A,rdcv1_r1A.as_int);

	rdcv1_r14.BRDC_MODES= 1; //0: single, 1: oversample, 2: always
	mbus_remote_register_write(RDC_ADDR,0x14,rdcv1_r14.as_int);

	rdcv1_r1D.BRDC_OFSBOT_IN = 0x01F;
    mbus_remote_register_write(RDC_ADDR,0x1D,rdcv1_r1D.as_int);

    mbus_remote_register_write(RDC_ADDR,0x15,0x0000);
    mbus_remote_register_write(RDC_ADDR,0x16,0x0FFF);
    mbus_remote_register_write(RDC_ADDR,0x1B,0x0000);
    mbus_remote_register_write(RDC_ADDR,0x1C,0x0FFF);

	// RDC Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(RDC_ADDR,0x11,0x0018);


    // Radio Settings --------------------------------------
	radv9_r0.RADIO_TUNE_CURRENT_LIMITER = 0x2F; //Current Limiter 2F = 30uA, 1F = 3uA
	radv9_r0.RADIO_TUNE_FREQ1 = 0x0; //Tune Freq 1
	radv9_r0.RADIO_TUNE_FREQ2 = 0x0; //Tune Freq 2
	radv9_r0.RADIO_TUNE_TX_TIME = 0x6; //Tune TX Time
	mbus_remote_register_write(RAD_ADDR,0,radv9_r0.as_int);

	// FSM data length setups
	radv9_r11.RAD_FSM_H_LEN = 16; // N
	radv9_r11.RAD_FSM_D_LEN = RADIO_DATA_LENGTH-1; // N-1
	radv9_r11.RAD_FSM_C_LEN = 10;
	mbus_remote_register_write(RAD_ADDR,11,radv9_r11.as_int);
	
	// Configure SCRO
	radv9_r1.SCRO_FREQ_DIV = 3;
	radv9_r1.SCRO_AMP_I_LEVEL_SEL = 2; // Default 2
	radv9_r1.SCRO_I_LEVEL_SELB = 0x60; // Default 0x6F
	mbus_remote_register_write(RAD_ADDR,1,radv9_r1.as_int);
	
	// LFSR Seed
	radv9_r12.RAD_FSM_SEED = 4;
	mbus_remote_register_write(RAD_ADDR,12,radv9_r12.as_int);
	
	// Mbus return address; Needs to be between 0x18-0x1F
	// New to PRCv17
    mbus_remote_register_write(RAD_ADDR,0xF,0x1002);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    data_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    sns_run_single = 0;
    sns_run_ht_threshold = 380;
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_sns_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 2000;

    // Go to sleep without timer
    operation_sleep_notimer();
}

//***************************************************
// Pressure measurement operation
//***************************************************

static uint32_t rdc_data_median(){

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xD0, rdc_data[0]);
		mbus_write_message32(0xD1, rdc_data[1]);
		mbus_write_message32(0xD2, rdc_data[2]);
	#endif
	

	if (rdc_data[0] > rdc_data[1]){
		if (rdc_data[1] > rdc_data[2]){
			return 1;
		}else if (rdc_data[0] > rdc_data[2]){
			return 2;
		}else{	
			return 0;
		}
	}else{
		if (rdc_data[0] > rdc_data[2]){
			return 0;
		}else if (rdc_data[1] > rdc_data[2]){
			return 2;
		}else{
			return 1;
		}
	}

}

static void operation_sns_run(void){

	if (Pstack_state == PSTK_IDLE){

		wfi_timeout_flag = 0;
		meas_count = 0;

		Pstack_state = PSTK_LDO;

		// Turn on SNS LDO VREF; requires settling
		sns_ldo_vref_on();

		// Power on radio
		if (radio_tx_option || ((exec_count+1) < SNS_CYCLE_INIT)){
			radio_power_on();
			// Put system to sleep
			set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
			operation_sleep();
		}

    }else if (Pstack_state == PSTK_LDO){
		Pstack_state = PSTK_TEMP_START;

		// Power on SNS LDO
		sns_ldo_power_on();

		// Power on temp sensor
		temp_sensor_power_on();
		delay(MBUS_DELAY);

	}else if (Pstack_state == PSTK_TEMP_START){
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
		Pstack_state = PSTK_TEMP_READ;

	}else if (Pstack_state == PSTK_TEMP_READ){

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
			Pstack_state = PSTK_TEMP_START;
				
		}else{
			// Last measurement from this wakeup
			meas_count = 0;

			// Assert temp sensor isolation & turn off temp sensor power
			temp_sensor_power_off();
			sns_ldo_power_off();
			Pstack_state = PSTK_RDC_RUN;

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
				
		}

	}else if (Pstack_state == PSTK_RDC_RUN){
	// Start RDC measurement

		wfi_timeout_flag = 0;

		if (meas_count == 0){
			// Release RDC power gates / isolation
			rdc_release_v1p2_pg();
			rdc_release_pg();
			delay(MBUS_DELAY);
			rdc_release_isolate();

			// Need delay for osc to stabilize
			rdc_osc_enable();
			delay(MBUS_DELAY*2);
		}

		// Use Timer32 as timeout counter
		config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Start RDC
		rdc_enable();

		// Wait for output
		WFI();

		// Turn off Timer32
		*TIMER32_GO = 0;
		Pstack_state = PSTK_RDC_READ;

	}else if (Pstack_state == PSTK_RDC_READ){

		// Grab RDC Data
		if (wfi_timeout_flag){
			read_data_rdc = 0xFAFA;
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			// Read register
			set_halt_until_mbus_rx();
			mbus_remote_register_read(RDC_ADDR,0x20,1);
			read_data_rdc = *REG1;
			set_halt_until_mbus_tx();
		}
		rdc_data[meas_count] = read_data_rdc;
		meas_count++;

		// Option to take multiple measurements per wakeup
		if (meas_count < RDC_NUM_MEAS){	
			// Repeat measurement while awake
			rdc_disable();
			Pstack_state = PSTK_RDC_RUN;
				
		}else{
			meas_count = 0;

			// Assert RDC isolation & turn off RDC power
			rdc_disable();
			rdc_osc_disable();
			rdc_assert_pg();
			Pstack_state = PSTK_IDLE;

			// Don't use median for now
			//uint32_t median_idx = rdc_data_median();

			mbus_write_message32(0xCC, exec_count);
			mbus_write_message32(0xC0, read_data_temp);
			mbus_write_message32(0xC1, read_data_rdc);
				
			exec_count++;

			// If in HT mode, only store values above HT threshold
			if (!(sns_run_ht_mode && (read_data_temp < sns_run_ht_threshold))){
				// Store results in memory; unless buffer is full
				if (data_storage_count < DATA_STORAGE_SIZE){
					rdc_storage[data_storage_count] = read_data_rdc;
					temp_storage[data_storage_count] = read_data_temp;
					radio_tx_count = data_storage_count;
					data_storage_count++;
				}
			}

			// Optionally transmit the data
			if (radio_tx_option){
				// Read latest PMU ADC measurement
				pmu_adc_read_latest();
				send_radio_data_ppm(0, 0xCC0000+exec_count);
				delay(RADIO_PACKET_DELAY);
				send_radio_data_ppm(0,0xBBB000+read_data_batadc);	
				delay(RADIO_PACKET_DELAY);
				send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & read_data_temp));
				delay(RADIO_PACKET_DELAY);
				send_radio_data_ppm(0, 0xE00000 | (0xFFFFF & read_data_rdc));
				delay(RADIO_PACKET_DELAY);
			}

			// Enter long sleep
			if (exec_count < SNS_CYCLE_INIT){
				// Send some signal
				send_radio_data_ppm(1, 0xABC000);
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);

			}else{	
				set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
			}

			// Make sure Radio is off
			if (radio_on){
				radio_ready = 0;
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
		// Reset RDC 
		rdc_assert_pg();
		rdc_disable();
		rdc_osc_disable();
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
	Pstack_state = PSTK_IDLE;
	
	radio_power_off();
	temp_sensor_power_off();
	sns_ldo_power_off();
	rdc_assert_pg();
}

static void operation_goc_trigger_radio(uint32_t radio_tx_num, uint32_t wakeup_timer_val, uint32_t radio_tx_prefix, uint32_t radio_tx_data){

	if (exec_count_irq < radio_tx_num){
		exec_count_irq++;
		if (exec_count_irq == 1){
			// Prepare radio TX
			radio_power_on();
			// Go to sleep for SCRO stabilitzation
			set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
			operation_sleep_noirqreset();
		}else{
			// radio
			send_radio_data_ppm(0, radio_tx_prefix | radio_tx_data);	
			// set timer
			set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
			// go to sleep and wake up with same condition
			operation_sleep_noirqreset();
		}
	}else{
		exec_count_irq = 0;
		// radio
		send_radio_data_ppm(1,0xFAF000);	
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
    if (enumerated != 0xDEADBEE2){
        operation_init();
    }

	// Parking lot feature
	if ((pmu_parkinglot_mode > 0) && (exec_count_irq == 0)){
		mbus_write_message32(0xAA,0xABCDDCBA);
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
    	Pstack_state = PSTK_IDLE;
		operation_sns_run();

    }else if(wakeup_data_header == 3){
		// Stop temp sensor program and transmit the battery reading and execution count (alternating n times)
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		operation_sns_sleep_check();
	
		Pstack_state = PSTK_IDLE;

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Read latest PMU ADC measurement
				pmu_adc_read_latest();

				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				if (exec_count_irq & 0x1){
					// radio
					send_radio_data_ppm(0,0xBBB000+read_data_batadc);	
				}else{
					// radio
					send_radio_data_ppm(0,0xCC0000+exec_count);	
				}
				// set timer
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				// go to sleep and wake up with same condition
				operation_sleep_noirqreset();
			}
        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data_ppm(1,0xFAF000);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }


    }else if(wakeup_data_header == 4){
        // Transmit the stored sns data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		operation_sns_sleep_check();
		
		Pstack_state = PSTK_IDLE;

		radio_tx_numdata = wakeup_data_field_0;

		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= data_storage_count){
			radio_tx_numdata = 0;
		}
		
        if (exec_count_irq < 5){
			exec_count_irq++;
			if (exec_count_irq == 1){
				// Read latest PMU ADC measurement
				pmu_adc_read_latest();

				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				send_radio_data_ppm(0,0xBBB000+read_data_batadc);	
				delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
				send_radio_data_ppm(0,0xCC0000+exec_count);	
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				// go to sleep and wake up with same condition
				operation_sleep_noirqreset();
			}
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

	}else if(wakeup_data_header == 0x13){
		// Change the RF frequency
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the desired RF tuning value (RADv9)
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
        delay(MBUS_DELAY);

		radv9_r0.RADIO_TUNE_FREQ1 = wakeup_data_field_2>>4; 
		radv9_r0.RADIO_TUNE_FREQ2 = wakeup_data_field_2 & 0xF; 
    	mbus_remote_register_write(RAD_ADDR,0,radv9_r0.as_int);
		delay(MBUS_DELAY);

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xABC000, exec_count_irq);

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

    }else if(wakeup_data_header == 0x15){
		// Transmit wakeup period as counted by (roughly) CPU clock
		// wakeup_data[7:0] is the # of transmissions
		// wakeup_data[15:8] is the user-specified period 
		WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Measure wakeup period
				measure_wakeup_period();

				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				// radio
				send_radio_data_ppm(0,0xC00000+wakeup_period_count);	
				// set timer
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				// go to sleep and wake up with same condition
				operation_sleep_noirqreset();
			}
        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data_ppm(1,0xFAF000);	
            // Go to sleep without timer
            operation_sleep_notimer();
        }
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

	}else if(wakeup_data_header == 0x25){
		// Change the conversion time of the temp sensor

		snsv10_r03.TSNS_SEL_CONV_TIME = wakeup_data & 0xF; // Default: 0x6
		mbus_remote_register_write(SNS_ADDR,0x03,snsv10_r03.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x26){
		// Change the threshold for HT operation

		sns_run_ht_threshold = wakeup_data & 0xFFFFFF;

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x30){
		// Change RDC OSR value (7bit)

		rdcv1_r1A.BRDC_OSR= wakeup_data_field_0;
		mbus_remote_register_write(RDC_ADDR,0x1A,rdcv1_r1A.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x31){
		// Change RDC Offset value (10bit)

		rdcv1_r1D.BRDC_OFSBOT_IN = wakeup_data & 0x3FF;
		mbus_remote_register_write(RDC_ADDR,0x1D,rdcv1_r1D.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x35){
	    mbus_remote_register_write(RDC_ADDR,0x15, wakeup_data & 0xFFFF);
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x36){

		mbus_remote_register_write(RDC_ADDR,0x16, wakeup_data & 0xFFFF);
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x3B){

		mbus_remote_register_write(RDC_ADDR,0x1B, wakeup_data & 0xFFFF);
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x3C){

		mbus_remote_register_write(RDC_ADDR,0x1C, wakeup_data & 0xFFFF);
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


