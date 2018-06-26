//*******************************************************************
//Author: Gyouho Kim
//Description: Temperature Sensing System
//			Modified from 'Pstack_Ondemand_v2.0'
//			v1.1: Changing chip order
//			v1.2: Increasing PMU floors for long term stability
//			v1.3: PMU IRQ off for stability issues
//				  RADv9 settings updated for shifted center freq and higher curr lim
//			v1.4: Decrease PMU sleep floors for reduced sleep power
//			v1.6: PMUv3; Use for Tstack batch 4 onward
//			v1.7: Chaging RF tuning to 0x04, GOC-based triggers reinitializes reg
//			v1.8: Adding battery measurement, PMU battery clamp reset, and battery discharge mode
//			v1.9: Stays awake during data TX, changing temp sensor tuning 
//			v1.10: Records 0x666 if temp sensor times out and moves on  
//				   Incorporating trig2 wakeup time adjustment based on temp measurement 
//				   LDO voltage set to be highest
//			   	  -->Used for AEC France Test in Nov 2016
//			v1.12: Dynamic adjustment of wakeup time based on CPU clock cycle
//				   Changed temp sensor polling to wfi  
//			v1.13: Optimizing PMU sleep/active settings
//				   Lower sleep power, higher sleep power during radio sleep
//				   PMU ADC disabled during radio sleep
//			v1.14: Fixing how PMU ADC is reset
//			v1.15: Turning off VDD_CLK->VBAT during PMU ADC routine
//				   Getting rid of all non-32 bit declarations
//			v2.00: Adding parking lot feature (automatic vbat mornitoring & charging)
//			v2.1:  Usability improvements
//			v2.2:  Increasing radio packet distance; adding header for data TX
//			v2.3:  Add option to start charging when configuring parking
//			v2.4:  During Trig 2, if radio option is on, transmit PMU ADC and count
//				   Radio packet delay is configurable now
//				   Initial trig2 delay removed
//			v2.5:  Sleep power reduced
//				   Chip ID configuration
//				   Reducing LDO sleep time
//			 	   Consolidated trigger radio routine
//			v3.0:  PRCv17, SNSv10, PMUv7 update
//			v3.1: Shallower parking range
//				  Trig4 now reports battery and execution count in the beginning
//			v3.2: Improving wakeup timer measurement; WUPT max value is 0x7FFF
//			      Trig2 with cont TX has headers for data
//			v3.3: Every PMU reg write should be followed by explicit delay to be safe
//			v3.4: Removing parking; charging controlled by pmu solar short vclamp
//			v3.5: Adding trigger to change solar short vclamp; battery logged with temp
//			v3.6: Using SRAM inverter SA
//			v3.7: Using SNTv1 instead of SNSv10; wakeup timer for Trig2 now using SNTv1
//			FIXME: confirm rollover, in case of timeout, wakeup timer needs adjustment
//*******************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "mbus.h"
#include "SNTv1_RF.h"
#include "HRVv5.h"
#include "RADv9.h"
#include "PMUv7_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG_1

// TStack order  PRC->RAD->SNS->HRV->PMU
#define HRV_ADDR 0x3
#define RAD_ADDR 0x4
#define SNT_ADDR 0x5
#define PMU_ADDR 0x6

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define TEMP_CYCLE_INIT 1 

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

#define NUM_TEMP_MEAS 1

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define WAKEUP_PERIOD_RADIO_INIT 10 // About 2 sec (PRCv17)

#define TEMP_STORAGE_SIZE 600 // Need to leave about 500 Bytes for stack --> around 120

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
volatile uint32_t Tstack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t PMU_ADC_4P2_VAL;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

//volatile uint32_t temp_meas_data[NUM_TEMP_MEAS] = {0};
volatile uint32_t temp_storage[TEMP_STORAGE_SIZE] = {0};
volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t temp_storage_count;
volatile uint32_t temp_running;
volatile uint32_t set_temp_exec_count;
volatile uint32_t read_data_temp; // [23:0] Temp Sensor D Out

volatile uint32_t snt_wup_counter_cur;
volatile uint32_t snt_timer_enabled = 0;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_batt;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t RADIO_PACKET_DELAY;

volatile uint32_t read_data_batadc;

volatile sntv1_r00_t sntv1_r00 = SNTv1_R00_DEFAULT;
volatile sntv1_r01_t sntv1_r01 = SNTv1_R01_DEFAULT;
volatile sntv1_r03_t sntv1_r03 = SNTv1_R03_DEFAULT;
volatile sntv1_r08_t sntv1_r08 = SNTv1_R08_DEFAULT;
volatile sntv1_r09_t sntv1_r09 = SNTv1_R09_DEFAULT;
volatile sntv1_r0A_t sntv1_r0A = SNTv1_R0A_DEFAULT;
volatile sntv1_r17_t sntv1_r17 = SNTv1_R17_DEFAULT;

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

volatile hrvv5_r0_t hrvv5_r0 = HRVv5_R0_DEFAULT;

volatile prcv17_r0B_t prcv17_r0B = PRCv17_R0B_DEFAULT;
volatile prcv17_r0D_t prcv17_r0D = PRCv17_R0D_DEFAULT;

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
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
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
		| (1 << 5)  // Frequency multiplier L (actually L+1)
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
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// The first register write to PMU needs to be repeated
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (0 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
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
		| (1 << 5)  // Frequency multiplier L (actually L+1)
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
		| (47) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 13) // Enables override setting [12] (1'b1)
		| (0 << 12) // Let VDD_CLK always connected to vbat
		| (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (47) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);

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

inline static void pmu_reset_solar_short(){
    mbus_remote_register_write(PMU_ADDR,0x0E, 
		( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (1 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (3 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x0E, 
		( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
		| (0 << 9)  // Enables override setting [8]
		| (0 << 8)  // Turn on the harvester-inhibiting switch
		| (3 << 4)  // clamp_tune_bottom (increases clamp thresh)
		| (0) 		// clamp_tune_top (decreases clamp thresh)
	));
	delay(MBUS_DELAY);
}

//***************************************************
// Temp Sensor Functions (SNTv1)
//***************************************************

static void temp_sensor_start(){
	sntv1_r01.TSNS_RESETn = 1;
	mbus_remote_register_write(SNT_ADDR,1,sntv1_r01.as_int);
}
static void temp_sensor_reset(){
	sntv1_r01.TSNS_RESETn = 0;
	mbus_remote_register_write(SNT_ADDR,1,sntv1_r01.as_int);
}
static void temp_sensor_power_on(){
	// Turn on digital block
	sntv1_r01.TSNS_SEL_LDO = 1;
	mbus_remote_register_write(SNT_ADDR,1,sntv1_r01.as_int);
	// Turn on analog block
	sntv1_r01.TSNS_EN_SENSOR_LDO = 1;
	mbus_remote_register_write(SNT_ADDR,1,sntv1_r01.as_int);

	delay(MBUS_DELAY);

	// Release isolation
	sntv1_r01.TSNS_ISOLATE = 0;
	mbus_remote_register_write(SNT_ADDR,1,sntv1_r01.as_int);
}
static void temp_sensor_power_off(){
	sntv1_r01.TSNS_RESETn = 0;
	sntv1_r01.TSNS_SEL_LDO = 0;
	sntv1_r01.TSNS_EN_SENSOR_LDO = 0;
	sntv1_r01.TSNS_ISOLATE = 1;
	mbus_remote_register_write(SNT_ADDR,1,sntv1_r01.as_int);
}
static void sns_ldo_vref_on(){
	sntv1_r00.LDO_EN_VREF 	= 1;
	mbus_remote_register_write(SNT_ADDR,0,sntv1_r00.as_int);
}

static void sns_ldo_power_on(){
	sntv1_r00.LDO_EN_IREF 	= 1;
	sntv1_r00.LDO_EN_LDO	= 1;
	mbus_remote_register_write(SNT_ADDR,0,sntv1_r00.as_int);
}
static void sns_ldo_power_off(){
	sntv1_r00.LDO_EN_VREF 	= 0;
	sntv1_r00.LDO_EN_IREF 	= 0;
	sntv1_r00.LDO_EN_LDO	= 0;
	mbus_remote_register_write(SNT_ADDR,0,sntv1_r00.as_int);
}

static void snt_read_wup_counter(){

	set_halt_until_mbus_rx();
	mbus_remote_register_read(SNT_ADDR,0x1B,1); // CNT_VALUE_EXT
	snt_wup_counter_cur = (*REG1 & 0xFF)<<24;
	mbus_remote_register_read(SNT_ADDR,0x1C,1); // CNT_VALUE
	snt_wup_counter_cur = snt_wup_counter_cur | (*REG1 & 0xFFFFFF);
	set_halt_until_mbus_tx();

}
	
static void snt_start_timer_presleep(){

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
}

static void snt_set_wup_timer(uint32_t sleep_count){
	
	snt_wup_counter_cur = sleep_count + snt_wup_counter_cur; // should handle rollover

	mbus_remote_register_write(SNT_ADDR,0x19,snt_wup_counter_cur>>24);
	mbus_remote_register_write(SNT_ADDR,0x1A,snt_wup_counter_cur & 0xFFFFFF);
	
	sntv1_r17.WUP_ENABLE = 0x1; // Default : 0x
	mbus_remote_register_write(SNT_ADDR,0x17,sntv1_r17.as_int);

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
	config_timer32(TIMER32_VAL, 1, 0, 0);

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
		// In case of timeout, wakeup counter needs to be adjusted 
		snt_read_wup_counter();
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
// End of Program Sleep Operation
//***************************************************
static void operation_sns_sleep_check(void){
	// Make sure LDO is off
	if (temp_running){
		temp_running = 0;
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

	operation_sns_sleep_check();

    // Make sure Radio is off
    if (radio_on){
		radio_power_off();
	}

	// Make sure SNT timer is off
	if (snt_timer_enabled){
		snt_stop_timer();
	}

	// Disable Timer
	set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}


static void operation_tx_stored(void){

	uint32_t radio_tx_count_init = radio_tx_count;

    //Fire off stored data to radio
    while(((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > temp_storage_count))){
		#ifdef DEBUG_MBUS_MSG_1
			mbus_write_message32(0xDD, radio_tx_count);
			mbus_write_message32(0xDD, temp_storage[radio_tx_count]);
		#endif

		// Reset watchdog timer
		config_timerwd(TIMERWD_VAL);

		// Radio out data
		send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & temp_storage[radio_tx_count]));
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		if (radio_tx_batt){
			send_radio_data_ppm(0, 0xBBB000 | (0xFF & (temp_storage[radio_tx_count]>>24)));
			delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		}
		
		radio_tx_count--;
    }

	send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & temp_storage[radio_tx_count]));
	if (radio_tx_batt){
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		send_radio_data_ppm(0, 0xBBB000 | (0xFF & (temp_storage[radio_tx_count]>>24)));
	}

	delay(RADIO_PACKET_DELAY*2); //Set delays between sending subsequent packet
	send_radio_data_ppm(1, 0xFAF000);

	// This is also the end of this IRQ routine
	exec_count_irq = 0;

	// Go to sleep without timer
	radio_tx_count = radio_tx_count_init; // allows data to be sent more than once
	operation_sleep_notimer();
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


static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prcv17_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    prcv17_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prcv17_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	*REG_CLKGEN_TUNE = prcv17_r0B.as_int;

    prcv17_r0D.SRAM_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prcv17_r0D.SRAM_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    prcv17_r0D.SRAM_USE_INVERTER_SA = 1; // Default 0x2, 4 bits
	*REG_SRAM_TUNE = prcv17_r0D.as_int;
  
    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0x3701;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_4P2_VAL = 0x4B;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(RAD_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(SNT_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(HRV_ADDR);
	delay(MBUS_DELAY);
 	mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//set_halt_until_mbus_tx();

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
    // sntv1_r01
	sntv1_r01.TSNS_RESETn = 0;
    sntv1_r01.TSNS_EN_IRQ = 1;
    sntv1_r01.TSNS_BURST_MODE = 0;
    sntv1_r01.TSNS_CONT_MODE = 0;
    mbus_remote_register_write(SNT_ADDR,1,sntv1_r01.as_int);

	// Set temp sensor conversion time
	sntv1_r03.TSNS_SEL_STB_TIME = 0x1; 
	sntv1_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
	mbus_remote_register_write(SNT_ADDR,0x03,sntv1_r03.as_int);

    // Wakeup Timer Settings --------------------------------------
    // Config Register A
	sntv1_r0A.TMR_S = 0x1; // Default: 0x4, use 1 for good TC
	// Tune R for TC
    sntv1_r0A.TMR_DIFF_CON = 0x3FFD; // Default: 0x3FFB
    sntv1_r0A.TMR_POLY_CON = 0x1; // Default: 0x1
    mbus_remote_register_write(SNT_ADDR,0x0A,sntv1_r0A.as_int);

	// TIMER CAP_TUNE  
	// Tune C for freq
	sntv1_r09.TMR_SEL_CAP = 0x80; // Default : 8'h8
	sntv1_r09.TMR_SEL_DCAP = 0x3F; // Default : 6'h4

	sntv1_r09.TMR_EN_TUNE1 = 0x1; // Default : 1'h1
	sntv1_r09.TMR_EN_TUNE2 = 0x1; // Default : 1'h1
	mbus_remote_register_write(SNT_ADDR,0x09,sntv1_r09.as_int);

	// Wakeup Counter setting
	sntv1_r17.WUP_CLK_SEL = 0x0; 
	sntv1_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
	mbus_remote_register_write(SNT_ADDR,0x17,sntv1_r17.as_int);

    // Radio Settings --------------------------------------
	radv9_r0.RADIO_TUNE_CURRENT_LIMITER = 0x2F; //Current Limiter 2F = 30uA, 1F = 3uA
	radv9_r0.RADIO_TUNE_FREQ1 = 0x0; //Tune Freq 1 (MSB)
	radv9_r0.RADIO_TUNE_FREQ2 = 0xA; //Tune Freq 2 (LSB)
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
    WAKEUP_PERIOD_CONT_INIT = 10;   // 
	WAKEUP_PERIOD_CONT_USER = 0x2710;
    temp_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    temp_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_temp_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 800;
	

    // Harvester Settings --------------------------------------
	hrvv5_r0.HRV_TOP_CONV_RATIO = 0x9;
    mbus_remote_register_write(HRV_ADDR,0,hrvv5_r0.as_int);

    delay(MBUS_DELAY);

    // Go to sleep without timer
    operation_sleep_notimer();
}


//***************************************************
// Temperature measurement operation (SNTv1)
//***************************************************
static void operation_temp_run(void){

	if (Tstack_state == TSTK_IDLE){
		Tstack_state = TSTK_LDO;

		wfi_timeout_flag = 0;

		// Turn on SNS LDO VREF; requires settling
		sns_ldo_vref_on();

		// Power on radio
		if (radio_tx_option || ((exec_count+1) < TEMP_CYCLE_INIT)){
			radio_power_on();
			// Put system to sleep
			set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
			operation_sleep();
		}

    }else if (Tstack_state == TSTK_LDO){
		Tstack_state = TSTK_TEMP_START;

		// Power on SNS LDO
		sns_ldo_power_on();

		// Power on temp sensor
		temp_sensor_power_on();
		delay(MBUS_DELAY);

	}else if (Tstack_state == TSTK_TEMP_START){
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
		Tstack_state = TSTK_TEMP_READ;

	}else if (Tstack_state == TSTK_TEMP_READ){

		// Grab Temp Sensor Data
		if (wfi_timeout_flag){
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			// Read register
			set_halt_until_mbus_rx();
			mbus_remote_register_read(SNT_ADDR,0x6,1);
			read_data_temp = *REG1;
			set_halt_until_mbus_tx();
		}
		meas_count++;

		// Last measurement from this wakeup
		if (meas_count == NUM_TEMP_MEAS){
			// No error; see if there was a timeout
			if (wfi_timeout_flag){
				temp_storage_latest = 0x666;
				wfi_timeout_flag = 0;
				// In case of timeout, wakeup counter needs to be adjusted 
				snt_read_wup_counter();
			}else{
				temp_storage_latest = read_data_temp;

			}
		}

		// Option to take multiple measurements per wakeup
		if (meas_count < NUM_TEMP_MEAS){	
			// Repeat measurement while awake
			temp_sensor_reset();
			Tstack_state = TSTK_TEMP_START;
				
		}else{
			meas_count = 0;

			// Read latest PMU ADC measurement
			pmu_adc_read_latest();

			// Assert temp sensor isolation & turn off temp sensor power
			temp_sensor_power_off();
			sns_ldo_power_off();
			Tstack_state = TSTK_IDLE;

			mbus_write_message32(0xCC, exec_count);
			mbus_write_message32(0xC0, temp_storage_latest);

			exec_count++;
			// Store results in memory; unless buffer is full
			if (temp_storage_count < TEMP_STORAGE_SIZE){
				temp_storage[temp_storage_count] = (temp_storage_latest & 0xFFFFFF) | ((read_data_batadc & 0xFF)<<24);
				radio_tx_count = temp_storage_count;
				temp_storage_count++;
			}

			// Optionally transmit the data
			if (radio_tx_option){
				send_radio_data_ppm(0, 0xCC0000+exec_count);
				delay(RADIO_PACKET_DELAY);
				send_radio_data_ppm(0,0xBBB000+read_data_batadc);	
				delay(RADIO_PACKET_DELAY);
		        send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & temp_storage_latest));
				delay(RADIO_PACKET_DELAY);
			}

			// Enter long sleep
			if(exec_count < TEMP_CYCLE_INIT){
				// Send some signal
				send_radio_data_ppm(1, 0xABC000);
			}


			// Make sure Radio is off
			if (radio_on){
				radio_ready = 0;
				radio_power_off();
			}

			if ((set_temp_exec_count != 0) && (exec_count > (50<<set_temp_exec_count))){
				// No more measurement required
				// Make sure temp sensor is off
				temp_running = 0;
				operation_sleep_notimer();
			}else{
				snt_set_wup_timer(WAKEUP_PERIOD_CONT_USER);
				operation_sleep_snt_timer();
			}

		}

    }else{
        //default:  // THIS SHOULD NOT HAPPEN
		// Reset Temp Sensor 
		temp_sensor_power_off();
		sns_ldo_power_off();
		operation_sleep_notimer();
    }

}


static void operation_goc_trigger_init(void){

	// This is critical
	set_halt_until_mbus_tx();
	mbus_write_message32(0xAA,0xABCD1234);
	mbus_write_message32(0xAA,wakeup_data);

	// Initialize variables & registers
	temp_running = 0;
	Tstack_state = TSTK_IDLE;
	
	radio_power_off();
	temp_sensor_power_off();
	sns_ldo_power_off();
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

static void operation_snt_calibration_radio(uint32_t tuning_num){

	uint32_t exec_count_irq_sub;
	exec_count_irq_sub = exec_count_irq >>4; // Divide by 16
	
	if (exec_count_irq_sub < tuning_num){
		// Debug
		mbus_write_message32(0xBB,exec_count_irq);
		
		if ((exec_count_irq & 0xF) == 0xF){ // every 16 iterations
			sntv1_r0A.TMR_DIFF_CON++; // Default: 0x3FFB
			mbus_remote_register_write(SNT_ADDR,0x0A,sntv1_r0A.as_int);
		}

		exec_count_irq++;
		if (exec_count_irq == 1){
			// Tune R for TC
			sntv1_r0A.TMR_DIFF_CON = 0x3FE0; // Default: 0x3FFB
			mbus_remote_register_write(SNT_ADDR,0x0A,sntv1_r0A.as_int);

			// Prepare radio TX
			radio_power_on();
			snt_start_timer_presleep();
			// Go to sleep for SCRO stabilitzation
			// Go to sleep for >3s for timer stabilization
			set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT*3, 0x1, 0x1);
			operation_sleep_noirqreset();
		}else if (exec_count_irq == 2){
			snt_start_timer_postsleep();
			// Read existing counter value; in case not reset to zero
			snt_read_wup_counter();
			snt_set_wup_timer(WAKEUP_PERIOD_CONT_USER);
			operation_sleep_noirqreset();
		}else{
			// radio
			send_radio_data_ppm(0, sntv1_r0A.TMR_DIFF_CON);	
			snt_set_wup_timer(WAKEUP_PERIOD_CONT_USER);
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
    if (enumerated != 0x3701){
        operation_init();
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
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[23:21] specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
        radio_tx_option = wakeup_data_field_2 & 0x10;

        exec_count_irq++;
		if (exec_count_irq == 1){
			snt_start_timer_presleep();
			// Go to sleep for >3s for timer stabilization
			set_wakeup_timer (0x20, 0x1, 0x1);
			operation_sleep_noirqreset();
		}else if (exec_count_irq == 2){
			snt_start_timer_postsleep();
			// Read existing counter value; in case not reset to zero
			snt_read_wup_counter();
		}

		if (!temp_running){
			temp_running = 1;
			set_temp_exec_count = wakeup_data_field_2 >> 5;
		}

		exec_count = 0;
		meas_count = 0;
		temp_storage_count = 0;
		radio_tx_count = 0;

		// Reset GOC_DATA_IRQ
		*GOC_DATA_IRQ = 0;
        exec_count_irq = 0;

		// Run Temp Sensor Program
    	Tstack_state = TSTK_IDLE;
		operation_temp_run();

    }else if(wakeup_data_header == 3){
		// Stop temp sensor program and transmit the battery reading and execution count (alternating n times)
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		operation_sns_sleep_check();

		Tstack_state = TSTK_IDLE;

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
        // Transmit the stored temp sensor data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
		// wakeup_data[16] transmits battery recording too

        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		operation_sns_sleep_check();

		Tstack_state = TSTK_IDLE;

		radio_tx_numdata = wakeup_data_field_0;
		radio_tx_batt = wakeup_data_field_2 & 0x1;

		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= temp_storage_count){
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

		if (exec_count_irq == 0){
			// Read latest PMU ADC measurement
			pmu_adc_read_latest();
		}

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xBBB000, read_data_batadc);


/*
    }else if(wakeup_data_header == 8){
		// Discharge battery by staying active and TX radio
		// wakeup_data[15:0] is the # of transmissions
		// wakeup_data[16] resets PMU solar clamp

		exec_count_irq++;
		if (exec_count_irq == 1){
			// Prepare radio TX
			radio_power_on();
			// Go to sleep for SCRO stabilitzation
			set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
			operation_sleep_noirqreset();
		}else{
			// radio
			uint32_t discharge_count = 0;
			while (discharge_count < (wakeup_data_field_0 + (wakeup_data_field_1<<8))){
				send_radio_data_ppm(0,0xBB0000+discharge_count);
				discharge_count++;
				delay(RADIO_PACKET_DELAY*2);
				// Prevent watchdog kicking in
    			config_timerwd(TIMERWD_VAL);
				delay(RADIO_PACKET_DELAY*2);
				
			}
		}

        if (wakeup_data_field_2 & 0x1){
			// Reset PMU solar clamp
			pmu_reset_solar_short();
		}

		// Finalize
		exec_count_irq = 0;
		// radio
		send_radio_data_ppm(1,0xFAF000);	
		// Go to sleep without timer
		operation_sleep_notimer();
*/
	}else if(wakeup_data_header == 0x13){
		// Change the RF frequency
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the desired RF tuning value (RADv9)

		radv9_r0.RADIO_TUNE_FREQ1 = wakeup_data_field_2>>4; 
		radv9_r0.RADIO_TUNE_FREQ2 = wakeup_data_field_2 & 0xF; 
    	mbus_remote_register_write(RAD_ADDR,0,radv9_r0.as_int);

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xABC000, exec_count_irq);


/*
    }else if(wakeup_data_header == 0x14){
		// Run temp sensor once to update room temperature reference
        radio_tx_option = 1;
		temp_run_single = 1;
		temp_running = 1;

		exec_count = 0;
		meas_count = 0;
		temp_storage_count = 0;
		radio_tx_count = 0;

		// Reset GOC_DATA_IRQ
		*GOC_DATA_IRQ = 0;
        exec_count_irq = 0;

		// Run Temp Sensor Program
		operation_temp_run();

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
			RADIO_PACKET_DELAY = 2000;
		}else{
			RADIO_PACKET_DELAY = user_val;
		}
		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x25){
		// Change the conversion time of the temp sensor

		sntv1_r03.TSNS_SEL_CONV_TIME = wakeup_data & 0xF; // Default: 0x6
		mbus_remote_register_write(SNT_ADDR,0x03,sntv1_r03.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x41){
		// Change PMU solar short clamp value

		mbus_remote_register_write(PMU_ADDR,0x0E, 
			( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
			| (1 << 9)  // Enables override setting [8]
			| (0 << 8)  // Turn on the harvester-inhibiting switch
			| (wakeup_data & 0xF0)  // clamp_tune_bottom (increases clamp thresh)
			| (wakeup_data & 0xF) 		// clamp_tune_top (decreases clamp thresh)
		));
		delay(MBUS_DELAY);
		mbus_remote_register_write(PMU_ADDR,0x0E, 
			( (1 << 10) // When to turn on harvester-inhibiting switch (0: PoR, 1: VBAT high)
			| (0 << 9)  // Enables override setting [8]
			| (0 << 8)  // Turn on the harvester-inhibiting switch
			| (wakeup_data & 0xF0)  // clamp_tune_bottom (increases clamp thresh)
			| (wakeup_data & 0xF) 		// clamp_tune_top (decreases clamp thresh)
		));
		delay(MBUS_DELAY);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x71){
		// Calibration routine for SNT wakeup timer

		operation_snt_calibration_radio(wakeup_data & 0xFFFF);


	}else if(wakeup_data_header == 0xA0){
	// FIXME: Test for SNTv1 wakeup timer
		
		exec_count_irq++;
		if (exec_count_irq == 1){
			snt_start_timer_presleep();
			// Go to sleep for >3s for timer stabilization
			set_wakeup_timer (0x20, 0x1, 0x1);
			operation_sleep_noirqreset();
		}else if (exec_count_irq == 2){
			snt_start_timer_postsleep();
			// Read existing counter value; in case not reset to zero
			snt_read_wup_counter();
		}
		snt_set_wup_timer(wakeup_data & 0xFFFFFF);

		operation_sleep_noirqreset();
		
	}else if(wakeup_data_header == 0xA2){
		WAKEUP_PERIOD_CONT_USER = wakeup_data & 0xFFFFFF;
        operation_sleep_notimer();

	}else if(wakeup_data_header == 0xF0){

		operation_goc_trigger_radio(wakeup_data_field_0, WAKEUP_PERIOD_RADIO_INIT, 0x0, enumerated);

    }else{
		if (wakeup_data_header != 0){
			// Invalid GOC trigger
            // Go to sleep without timer
            operation_sleep_notimer();
		}
	}


	if (temp_running){
		// Proceed to continuous mode
		while(1){
			operation_temp_run();
		}
	}

	
	operation_sleep_notimer();

    while(1);
}


