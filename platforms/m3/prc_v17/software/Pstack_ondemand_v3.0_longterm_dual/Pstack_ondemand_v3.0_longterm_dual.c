//*******************************************************************
//Author: Gyouho Kim
//Description: Pressure Sensing System with RDC and new temp sensor
//			Modified from 'Tstack_Ondemand_v3.0'
//			v3.0:  PRCv17, SNSv10, PMUv7, RDCv1 
//*******************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "mbus.h"
#include "RDCv1_RF.h"
#include "SNSv10_RF.h"
#include "HRVv5.h"
#include "RADv9.h"
#include "PMUv7_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG_1

// Pstack order  PRC->RAD->SNS->RDC->HRV->PMU
#define PMU_ADDR 0x6
volatile uint32_t RDC_ADDR;

#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// Temp Sensor parameters
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
#define PSTK_RDC2_RUN 	0x6
#define PSTK_RDC2_READ  	0x7

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
volatile uint32_t PMU_ADC_4P2_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_harvesting_on;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t temp_storage[DATA_STORAGE_SIZE] = {0};
volatile uint32_t temp_storage_latest = 150; // SNSv10
volatile uint32_t temp_storage_last_wakeup_adjust = 150; // SNSv10
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t read_data_temp; // [23:0] Temp Sensor D Out

volatile uint32_t rdc_storage[DATA_STORAGE_SIZE] = {0};
volatile uint32_t rdc_data[RDC_NUM_MEAS] = {0};
volatile uint32_t data_storage_count;
volatile uint32_t sns_run_single;
volatile uint32_t sns_running;
volatile uint32_t set_sns_exec_count;
volatile uint32_t read_data_rdc;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t RADIO_PACKET_DELAY;

volatile uint32_t read_data_batadc;

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

volatile hrvv5_r0_t hrvv5_r0 = HRVv5_R0_DEFAULT;

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

inline static void set_pmu_adc_period(uint32_t val){
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

inline static void set_pmu_sleep_radio(){
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

inline static void set_pmu_sleep_low(){
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


inline static void set_pmu_clk_init(){
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

	set_pmu_adc_period(1); // 0x100 about 1 min for 1/2/1 1P2 setting
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

inline static void pmu_parkinglot_decision(){
	
	if (read_data_batadc < (PMU_ADC_4P2_VAL + 2)){
		// Stop Harvesting (4.1V)
		pmu_harvesting_on = 0;
		// Register 0x0E: PMU_VOLTAGE_CLAMP_TRIM
		mbus_remote_register_write(PMU_ADDR,0x0E, 
		   (    ( 0 << 10) // When to turn on Harvester Inhibiting Switch
			  | ( 1 <<  9) // Enables Override Settings [8]
			  | ( 1 <<  8) // Turn On Harvester Inhibiting Switch
			  | ( 1 <<  4) // Clamp Tune Bottom
			  | ( 0 <<  0) // Clamp Tune Top
			  ));


	}else if (read_data_batadc >= PMU_ADC_4P2_VAL + 3){
		//Start Harvesting (3.9V)
		pmu_harvesting_on = 1;
		// Register 0x0E: PMU_VOLTAGE_CLAMP_TRIM
		mbus_remote_register_write(PMU_ADDR,0x0E, 
		   (    ( 0 << 10) // When to turn on Harvester Inhibiting Switch
			  | ( 1 <<  9) // Enables Override Settings [8]
			  | ( 0 <<  8) // Turn On Harvester Inhibiting Switch
			  | ( 1 <<  4) // Clamp Tune Bottom
			  | ( 0 <<  0) // Clamp Tune Top
			  ));
	}else{

	}
	
	if (pmu_harvesting_on == 0){
		mbus_write_message32(0xAA, 0x000FF0FF);
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
    
	operation_sns_sleep_check();

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
    enumerated = 0xDEADBEE0;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_4P2_VAL = 0x4B;
	pmu_parkinglot_mode = 0;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(0x6); // RDC1
	delay(MBUS_DELAY);
    mbus_enumerate(0x7); // RDC2
	delay(MBUS_DELAY);

	RDC_ADDR = 6;
    // Set CPU Halt Option as TX --> Use for register write e.g.
	//set_halt_until_mbus_tx();

	// RDC Settings ----------------------------------------
	rdcv1_r1A.BRDC_OSR= 32;
	rdcv1_r1A.BRDC_SAMPLE_TIME_SEL = 15;
	rdcv1_r1A.BRDC_TICK_SPL_TO_SPL = 0;
	rdcv1_r1A.BRDC_TICK_VREF_SPL_REDIS_WAIT = 2;

	rdcv1_r14.BRDC_MODES= 1; //0: single, 1: oversample, 2: always

	mbus_remote_register_write(RDC_ADDR,0x1A,rdcv1_r1A.as_int);
	mbus_remote_register_write(RDC_ADDR,0x14,rdcv1_r14.as_int);
    mbus_remote_register_write(RDC_ADDR,0x15,0x0000);
    mbus_remote_register_write(RDC_ADDR,0x16,0x0FFF);
    mbus_remote_register_write(RDC_ADDR,0x1B,0x0000);
    mbus_remote_register_write(RDC_ADDR,0x1C,0x0FFF);
    //mbus_remote_register_write(RDC_ADDR,0x1D,0x0000);

	// RDC Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(RDC_ADDR,0x11,0x0018);

	RDC_ADDR = 7;

	mbus_remote_register_write(RDC_ADDR,0x1A,rdcv1_r1A.as_int);
	mbus_remote_register_write(RDC_ADDR,0x14,rdcv1_r14.as_int);
    mbus_remote_register_write(RDC_ADDR,0x15,0x0000);
    mbus_remote_register_write(RDC_ADDR,0x16,0x0FFF);
    mbus_remote_register_write(RDC_ADDR,0x1B,0x0000);
    mbus_remote_register_write(RDC_ADDR,0x1C,0x0FFF);
    //mbus_remote_register_write(RDC_ADDR,0x1D,0x0000);

	// RDC Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(RDC_ADDR,0x11,0x0018);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 0x10;
    WAKEUP_PERIOD_CONT_INIT = 10;   // 
    data_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    sns_run_single = 0;
    sns_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_sns_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 2000;
	

    delay(MBUS_DELAY);

    // Go to sleep without timer
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

		Pstack_state = PSTK_RDC_RUN;
		RDC_ADDR = 6;

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

			RDC_ADDR = 7;
			Pstack_state = PSTK_RDC2_RUN;

			mbus_write_message32(0xCC, exec_count);
			mbus_write_message32(0xC1, read_data_rdc);
				
		}

	}else if (Pstack_state == PSTK_RDC2_RUN){
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
		Pstack_state = PSTK_RDC2_READ;

	}else if (Pstack_state == PSTK_RDC2_READ){

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
			Pstack_state = PSTK_RDC2_RUN;
				
		}else{
			meas_count = 0;

			// Assert RDC isolation & turn off RDC power
			rdc_disable();
			rdc_osc_disable();
			rdc_assert_pg();
			Pstack_state = PSTK_IDLE;

			mbus_write_message32(0xC2, read_data_rdc);
				
			exec_count++;


			// Enter long sleep
			set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
			operation_sleep();

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
    if (enumerated != 0xDEADBEE0){
        operation_init();
    }

	while(1){
		operation_sns_run();
	}

	
	operation_sleep_notimer();

    while(1);
}


