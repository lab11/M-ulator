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
//			v3.6a: Adding tunability for SRR drive strength
//*******************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "mbus.h"
#include "SNSv10_RF.h"
#include "HRVv5.h"
#include "PMUv7_RF.h"
#include "SRRv4_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG_1

// TStack order  PRC->RAD->SNS->HRV->PMU
#define HRV_ADDR 0x3
#define SRR_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define TEMP_CYCLE_INIT 3 

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

#define NUM_TEMP_MEAS 1

// Radio configurations
#define RADIO_DATA_LENGTH 168
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
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

//volatile uint32_t temp_meas_data[NUM_TEMP_MEAS] = {0};
volatile uint32_t temp_storage[TEMP_STORAGE_SIZE] = {0};
volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t temp_storage_count;
volatile uint32_t temp_run_single;
volatile uint32_t temp_running;
volatile uint32_t set_temp_exec_count;
volatile uint32_t read_data_temp; // [23:0] Temp Sensor D Out

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_batt;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t RADIO_PACKET_DELAY;
volatile uint32_t radio_packet_count;

volatile uint32_t read_data_batadc;

volatile snsv10_r00_t snsv10_r00 = SNSv10_R00_DEFAULT;
volatile snsv10_r01_t snsv10_r01 = SNSv10_R01_DEFAULT;
volatile snsv10_r03_t snsv10_r03 = SNSv10_R03_DEFAULT;
volatile snsv10_r17_t snsv10_r17 = SNSv10_R17_DEFAULT;

volatile srrv4_r00_t srrv4_r00 = SRRv4_R00_DEFAULT;
volatile srrv4_r01_t srrv4_r01 = SRRv4_R01_DEFAULT;
volatile srrv4_r02_t srrv4_r02 = SRRv4_R02_DEFAULT;
volatile srrv4_r03_t srrv4_r03 = SRRv4_R03_DEFAULT;
volatile srrv4_r04_t srrv4_r04 = SRRv4_R04_DEFAULT;
volatile srrv4_r05_t srrv4_r05 = SRRv4_R05_DEFAULT;
volatile srrv4_r07_t srrv4_r07 = SRRv4_R07_DEFAULT;
volatile srrv4_r10_t srrv4_r10 = SRRv4_R10_DEFAULT;
volatile srrv4_r11_t srrv4_r11 = SRRv4_R11_DEFAULT;
volatile srrv4_r12_t srrv4_r12 = SRRv4_R12_DEFAULT;
volatile srrv4_r13_t srrv4_r13 = SRRv4_R13_DEFAULT;
volatile srrv4_r14_t srrv4_r14 = SRRv4_R14_DEFAULT;
volatile srrv4_r15_t srrv4_r15 = SRRv4_R15_DEFAULT;
volatile srrv4_r16_t srrv4_r16 = SRRv4_R16_DEFAULT;
volatile srrv4_r1E_t srrv4_r1E = SRRv4_R1E_DEFAULT;
volatile srrv4_r1F_t srrv4_r1F = SRRv4_R1F_DEFAULT;

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
	// Report who woke up
	delay(MBUS_DELAY);
	mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
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
// Radio transmission routines for SRR
//***************************************************

static void radio_power_on(){

    radio_on = 1;

    // Turn on Current Limter
    srrv4_r00.SRR_CL_EN = 1;  //Enable CL
    mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);

    // Release timer power-gate
    srrv4_r04.RO_EN_RO_V1P2 = 1;  //Use V1P2 for TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);
    delay(MBUS_DELAY*5);

	// Turn on timer
    srrv4_r04.RO_RESET = 0;  //Release Reset TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);
    delay(MBUS_DELAY*5);

    srrv4_r04.RO_EN_CLK = 1; //Enable CLK TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);
    delay(MBUS_DELAY*5);

    srrv4_r04.RO_ISOLATE_CLK = 0; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);

    // Release FSM Sleep
    srrv4_r11.SRR_RAD_FSM_SLEEP = 0;  // Power on BB
    mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);
	delay(MBUS_DELAY*5); // Freq stab

}

static void radio_power_off(){

    // Turn off everything
    srrv4_r03.SRR_TRX_ISOLATEN = 0;     //set ISOLATEN 0
    mbus_remote_register_write(SRR_ADDR,0x03,srrv4_r03.as_int);

    srrv4_r11.SRR_RAD_FSM_EN = 0;  //Stop BB
    srrv4_r11.SRR_RAD_FSM_RSTN = 0;  //RST BB
    srrv4_r11.SRR_RAD_FSM_SLEEP = 1;
    mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);

    // Turn off Current Limter
    srrv4_r00.SRR_CL_EN = 0;  //Enable CL
    mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);

    srrv4_r04.RO_RESET = 1;  //Release Reset TIMER
    srrv4_r04.RO_EN_CLK = 0; //Enable CLK TIMER
    srrv4_r04.RO_ISOLATE_CLK = 1; //Set Isolate CLK to 0 TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);

    // Enable timer power-gate
    srrv4_r04.RO_EN_RO_V1P2 = 0;  //Use V1P2 for TIMER
    mbus_remote_register_write(SRR_ADDR,0x04,srrv4_r04.as_int);

    radio_on = 0;
	radio_ready = 0;
}

static void srr_configure_pulse_width_short(){

    srrv4_r12.SRR_RAD_FSM_TX_PW_LEN = 0; //4us PW
    srrv4_r13.SRR_RAD_FSM_TX_C_LEN = 32; // (PW_LEN+1):C_LEN=1:32
    srrv4_r12.SRR_RAD_FSM_TX_PS_LEN = 1; // PW=PS guard interval betwen 0 and 1 pulse

    mbus_remote_register_write(SRR_ADDR,0x12,srrv4_r12.as_int);
    mbus_remote_register_write(SRR_ADDR,0x13,srrv4_r13.as_int);
}

static void send_radio_data_mrr_sub1(){

	// Use Timer32 as timeout counter
    wfi_timeout_flag = 0;
	config_timer32(TIMER32_VAL, 1, 0, 0); // 1/10 of MBUS watchdog timer default

    // Fire off data
	srrv4_r11.SRR_RAD_FSM_EN = 1;  //Start BB
	mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);

	// Wait for radio response
	WFI();

	// Turn off Timer32
	*TIMER32_GO = 0;

	if (wfi_timeout_flag){
		mbus_write_message32(0xFA, 0xFAFAFAFA);
	}

	srrv4_r11.SRR_RAD_FSM_EN = 0;
	mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);
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
    mbus_remote_register_write(SRR_ADDR,0xD,radio_data_0);
    mbus_remote_register_write(SRR_ADDR,0xE,radio_data_1);
    mbus_remote_register_write(SRR_ADDR,0xF,radio_data_2);

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		srrv4_r11.SRR_RAD_FSM_RSTN = 1;  //UNRST BB
		mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);
		delay(MBUS_DELAY*5);

    	srrv4_r03.SRR_TRX_ISOLATEN = 1;     //set ISOLATEN 1, let state machine control
    	mbus_remote_register_write(SRR_ADDR,0x03,srrv4_r03.as_int);
		delay(MBUS_DELAY*5);

    }

	send_radio_data_mrr_sub1();

	radio_packet_count++;

	if (last_packet){
		radio_ready = 0;
		radio_power_off();
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
		send_radio_data_mrr(0, 0, 0, 0xD00000 | (0xFFFFF & temp_storage[radio_tx_count]));
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		if (radio_tx_batt){
			send_radio_data_mrr(0, 0, 0, 0xBBB000 | (0xFF & (temp_storage[radio_tx_count]>>24)));
			delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		}
		
		radio_tx_count--;
    }

	send_radio_data_mrr(0, 0, 0, 0xD00000 | (0xFFFFF & temp_storage[radio_tx_count]));
	if (radio_tx_batt){
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		send_radio_data_mrr(0, 0, 0, 0xBBB000 | (0xFF & (temp_storage[radio_tx_count]>>24)));
	}

	delay(RADIO_PACKET_DELAY*2); //Set delays between sending subsequent packet
	send_radio_data_mrr(1, 0, 0, 0xFAF000);

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

    prcv17_r0D.SRAM_TUNE_ASO_DLY = 31; // Default 0x0, 5 bits
    prcv17_r0D.SRAM_TUNE_DECODER_DLY = 15; // Default 0x2, 4 bits
    prcv17_r0D.SRAM_USE_INVERTER_SA = 1; // Default 0x2, 4 bits
	*REG_SRAM_TUNE = prcv17_r0D.as_int;
  
    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0xDEADB36A;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_4P2_VAL = 0x4B;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(SRR_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(SNS_ADDR);
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

    // Radio Settings (SRRv4) -------------------------------------------
    srrv4_r1F.LC_CLK_RING = 0x3;  // ~ 150 kHz
    srrv4_r1F.LC_CLK_DIV = 0x3;  // ~ 150 kHz
    mbus_remote_register_write(SRR_ADDR,0x1F,srrv4_r1F.as_int);

	srr_configure_pulse_width_short();

	// RO setup (SFO)
	// Adjust Diffusion R
	mbus_remote_register_write(SRR_ADDR,0x06,0x1000); // RO_PDIFF

	// Adjust Poly R
	mbus_remote_register_write(SRR_ADDR,0x08,0x400000); // RO_POLY

	// Adjust C
	srrv4_r07.RO_MOM = 0x10;
	srrv4_r07.RO_MIM = 0x10;
	mbus_remote_register_write(SRR_ADDR,0x07,srrv4_r07.as_int);

    // Current Limter set-up 
    srrv4_r00.SRR_CL_CTRL = 16; //Set CL 1-finite 16-20uA

    // TX Setup Carrier Freq
    srrv4_r00.SRR_TRX_CAP_ANTP_TUNE = 0x0000;  //ANT CAP 14b unary 830.5 MHz
	//srrv4_r00.SRR_TRX_CAP_ANTP_TUNE = 0x00FF;  //ANT CAP 14b unary 813.8 MHz
	//srrv4_r00.SRR_TRX_CAP_ANTP_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
    mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);
    srrv4_r01.SRR_TRX_CAP_ANTN_TUNE = 0x0000; //ANT CAP 14b unary 830.5 MHz
	//srrv4_r01.SRR_TRX_CAP_ANTN_TUNE = 0x00FF; //ANT CAP 14b unary 813.8 MHz
	//srrv4_r01.SRR_TRX_CAP_ANTN_TUNE = 0x0FFF;  //ANT CAP 14b unary 805.5 MHz
    mbus_remote_register_write(SRR_ADDR,0x01,srrv4_r01.as_int);
    srrv4_r02.SRR_TX_BIAS_TUNE = 0x0FFF;  //Set TX BIAS TUNE 13b // Set to max
    mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);

    // RX Setup
	srrv4_r03.SRR_RX_BIAS_TUNE    = 0x000F;//  turn on Q_enhancement
    srrv4_r03.SRR_RX_SAMPLE_CAP    = 0x1;  // RX_SAMPLE_CAP
    mbus_remote_register_write(SRR_ADDR,3,srrv4_r03.as_int);

    srrv4_r14.SRR_RAD_FSM_RX_POWERON_LEN = 0x0;  //Set RX Power on length
    //srrv4_r14.SRR_RAD_FSM_RX_SAMPLE_LEN = 0x3;  //Set RX Sample length  16us
    srrv4_r14.SRR_RAD_FSM_RX_SAMPLE_LEN = 0x7;  //Set RX Sample length  4us
    srrv4_r14.SRR_RAD_FSM_GUARD_LEN = 0x001C; //Set TX_RX Guard length, TX_RX guard 32 cycle (28+5)
    mbus_remote_register_write(SRR_ADDR,0x14,srrv4_r14.as_int);

    srrv4_r15.SRR_RAD_FSM_RX_HDR_BITS = 0x00;  //Set RX header
    srrv4_r15.SRR_RAD_FSM_RX_HDR_TH = 0x00;    //Set RX header threshold
    srrv4_r15.SRR_RAD_FSM_RX_DATA_BITS = 0x1; //Set RX data 16b
    mbus_remote_register_write(SRR_ADDR,0x15,srrv4_r15.as_int);

    srrv4_r1E.SRR_IRQ_REPLY_PACKET = 0x061400; //Read RX data Reply
    mbus_remote_register_write(SRR_ADDR,0x1E,srrv4_r1E.as_int);

    mbus_remote_register_write(SRR_ADDR,0x15,srrv4_r15.as_int);
    
	// Use pulse generator
    srrv4_r02.SRR_TX_PULSE_FINE = 0;
    srrv4_r02.SRR_TX_PULSE_FINE_TUNE = 7;
    mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);

    srrv4_r13.SRR_RAD_FSM_SEED = 1; //default
    srrv4_r14.SRR_RAD_FSM_TX_POWERON_LEN = 7; //3bits
    mbus_remote_register_write(SRR_ADDR,0x14,srrv4_r14.as_int);

	// RAD_FSM set-up 
	// Using first 48 bits of data as header
	mbus_remote_register_write(SRR_ADDR,0x09,0x0);
	mbus_remote_register_write(SRR_ADDR,0x0A,0x0);
	mbus_remote_register_write(SRR_ADDR,0x0B,0x0);
	mbus_remote_register_write(SRR_ADDR,0x0C,0x7AC800);
	srrv4_r11.SRR_RAD_FSM_TX_H_LEN = 0; //31-31b header (max)
	srrv4_r11.SRR_RAD_FSM_TX_D_LEN = RADIO_DATA_LENGTH; //0-skip tx data
	mbus_remote_register_write(SRR_ADDR,0x11,srrv4_r11.as_int);

	srrv4_r13.SRR_RAD_FSM_TX_MODE = 3; //code rate 0:4 1:3 2:2 3:1(baseline) 4:1/2 5:1/3 6:1/4
	mbus_remote_register_write(SRR_ADDR,0x13,srrv4_r13.as_int);

	// Mbus return address
	mbus_remote_register_write(SRR_ADDR,0x1E,0x1002);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 10;   // 
    temp_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    temp_run_single = 0;
    temp_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_temp_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 2500;
	radio_packet_count = 0;
	
    // Harvester Settings --------------------------------------
	hrvv5_r0.HRV_TOP_CONV_RATIO = 0x9;
    mbus_remote_register_write(HRV_ADDR,0,hrvv5_r0.as_int);

    delay(MBUS_DELAY);

    // Go to sleep without timer
    operation_sleep_notimer();
}


//***************************************************
// Temperature measurement operation (SNSv10)
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
			mbus_remote_register_read(SNS_ADDR,0x6,1);
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
			}else{
				temp_storage_latest = read_data_temp;

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
				
				if ((temp_storage_diff > 10) || (exec_count < (TEMP_CYCLE_INIT+5))){ // FIXME: value of 20 correct?
					measure_wakeup_period();
					temp_storage_last_wakeup_adjust = temp_storage_latest;
				}
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
				send_radio_data_mrr(0, 0, 0, 0xCC0000+exec_count);
				delay(RADIO_PACKET_DELAY);
				send_radio_data_mrr(0, 0, 0, 0xBBB000+read_data_batadc);	
				delay(RADIO_PACKET_DELAY);
		        send_radio_data_mrr(0, 0, 0, 0xD00000 | (0xFFFFF & temp_storage_latest));
				delay(RADIO_PACKET_DELAY);
			}

			// Enter long sleep
			if(exec_count < TEMP_CYCLE_INIT){
				// Send some signal
				send_radio_data_mrr(1, 0, 0, 0xABC000);
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);

			}else{	
				set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
			}

			// Make sure Radio is off
			if (radio_on){
				radio_ready = 0;
				radio_power_off();
			}

			if (temp_run_single){
				temp_run_single = 0;
				temp_running = 0;
				operation_sleep_notimer();
			}

			if ((set_temp_exec_count != 0) && (exec_count > (50<<set_temp_exec_count))){
				// No more measurement required
				// Make sure temp sensor is off
				temp_running = 0;
				operation_sleep_notimer();
			}else{
				operation_sleep();
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

	// Prepare radio TX
	radio_power_on();

	if (exec_count_irq < radio_tx_num){
		exec_count_irq++;
		// radio
		send_radio_data_mrr(1, 0, 0, radio_tx_prefix | radio_tx_data);	
		// set timer
		set_wakeup_timer (wakeup_timer_val, 0x1, 0x1);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();

	}else{
		exec_count_irq = 0;
		// radio
		send_radio_data_mrr(1, 0, 0, 0xFAF000);	
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
    if (enumerated != 0xDEADB36A){
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
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:16] is the initial user-specified period
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[23:21] specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
    	WAKEUP_PERIOD_CONT_USER = (wakeup_data_field_0 + (wakeup_data_field_1<<8));
        WAKEUP_PERIOD_CONT_INIT = (wakeup_data_field_2 & 0xF);
        radio_tx_option = wakeup_data_field_2 & 0x10;

		temp_run_single = 0;

		if (!temp_running){
			// Go to sleep for initial settling of temp sensing // FIXME
			//set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
			temp_running = 1;
			set_temp_exec_count = wakeup_data_field_2 >> 5;
            exec_count_irq++;
			//operation_sleep_noirqreset();
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

		// Read latest PMU ADC measurement
		pmu_adc_read_latest();

		// Prepare radio TX
		radio_power_on();

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq & 0x1){
				// radio
				send_radio_data_mrr(1, 0, 0, 0xBBB000+read_data_batadc);	
			}else{
				// radio
				send_radio_data_mrr(1, 0, 0, 0xCC0000+exec_count);	
			}
			// set timer
			set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
			// go to sleep and wake up with same condition
			operation_sleep_noirqreset();
        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data_mrr(1, 0, 0, 0xFAF000);	
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
		
		// Read latest PMU ADC measurement
		pmu_adc_read_latest();

		// Prepare radio TX
		radio_power_on();

        if (exec_count_irq < 5){
			exec_count_irq++;
			send_radio_data_mrr(0, 0, 0, 0xBBB000+read_data_batadc);	
			delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
			send_radio_data_mrr(1, 0, 0, 0xCC0000+exec_count);	
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
				send_radio_data_mrr(0,0xBB0000+discharge_count);
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
		send_radio_data_mrr(1,0xFAF000);	
		// Go to sleep without timer
		operation_sleep_notimer();
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

	}else if(wakeup_data_header == 0x22){
		// Change carrier frequency
		srrv4_r00.SRR_TRX_CAP_ANTP_TUNE = (wakeup_data & 0x3FFF);
		mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);
		srrv4_r01.SRR_TRX_CAP_ANTN_TUNE = (wakeup_data & 0x3FFF);
		mbus_remote_register_write(SRR_ADDR,0x01,srrv4_r01.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x25){
		// Change the conversion time of the temp sensor

		snsv10_r03.TSNS_SEL_CONV_TIME = wakeup_data & 0xF; // Default: 0x6
		mbus_remote_register_write(SNS_ADDR,0x03,snsv10_r03.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x2C){
		// Change C_LEN

		srrv4_r13.SRR_RAD_FSM_TX_C_LEN = wakeup_data & 0xFFFF; // (PW_LEN+1):C_LEN=1:32
		mbus_remote_register_write(SRR_ADDR,0x13,srrv4_r13.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x2E){
		// Change SRR TX Amp Current
    	srrv4_r02.SRR_TX_BIAS_TUNE = wakeup_data & 0x1FFF;  //Set TX BIAS TUNE 13b // Set to max
	    mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();
	}else if(wakeup_data_header == 0x2F){
		// Change C_LEN

    	srrv4_r00.SRR_CL_CTRL = wakeup_data & 0xFF; //Set CL 1-finite 16-20uA
		mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);

		// Go to sleep without timer
		operation_sleep_notimer();

	}else if(wakeup_data_header == 0x52){
		// Start TX Continuous mode
    	srrv4_r00.SRR_CL_CTRL = 1; //Set CL 1-finite 16-20uA
		mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);
		srrv4_r00.SRR_CL_EN = 1;  //Enable CL
		mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);

		srrv4_r02.SRR_TX_EN_OW = 1;
		mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);
	
	}else if(wakeup_data_header == 0x53){
		// Stop TX Continuous mode
		srrv4_r02.SRR_TX_EN_OW = 0;
		mbus_remote_register_write(SRR_ADDR,0x02,srrv4_r02.as_int);

		srrv4_r00.SRR_CL_EN = 0;  //Enable CL
		mbus_remote_register_write(SRR_ADDR,0x00,srrv4_r00.as_int);


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


