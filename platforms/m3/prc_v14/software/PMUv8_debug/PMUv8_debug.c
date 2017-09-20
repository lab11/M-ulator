//*******************************************************************
//Author: Gyouho Kim
//Description: Temperature Sensing System
//*******************************************************************
#include "PRCv14.h"
#include "PRCv14_RF.h"
#include "mbus.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"
#include "PMUv3_RF.h"

// uncomment this for debug mbus message
// #define DEBUG_MBUS_MSG
#define DEBUG_MBUS_MSG_1

// TStack order  PRC->RAD->SNS->HRV->PMU
#define HRV_ADDR 0x3
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

#define WAKEUP_PERIOD_PARKING 2000 // 200: ~200sec

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 1
#define TEMP_CYCLE_INIT 3 

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_RSTRL 0x3
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

#define NUM_TEMP_MEAS 5 

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define RADIO_TIMEOUT_COUNT 50
#define WAKEUP_PERIOD_RADIO_INIT 2

#define TEMP_STORAGE_SIZE 600 // Need to leave about 500 Bytes for stack --> around 120

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y2 run default clock

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t Tstack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t mbus_msg_flag;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t PMU_ADC_4P2_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_harvesting_on;

volatile snsv7_r14_t snsv7_r14 = SNSv7_R14_DEFAULT;
volatile snsv7_r15_t snsv7_r15 = SNSv7_R15_DEFAULT;
volatile snsv7_r18_t snsv7_r18 = SNSv7_R18_DEFAULT;
volatile snsv7_r25_t snsv7_r25 = SNSv7_R25_DEFAULT;
  
volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

//volatile uint32_t temp_meas_data[NUM_TEMP_MEAS] = {0};
volatile uint32_t temp_storage[TEMP_STORAGE_SIZE] = {0};
volatile uint32_t temp_storage_latest = 2000;
volatile uint32_t temp_storage_last_wakeup_adjust = 2000;
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t temp_storage_count;
volatile uint32_t temp_run_single;
volatile uint32_t temp_running;
volatile uint32_t set_temp_exec_count;
volatile uint32_t read_data_reg11; // [23:0] Temp Sensor D Out

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;
volatile uint32_t RADIO_PACKET_DELAY;

volatile uint32_t read_data_batadc;

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

volatile hrvv2_r0_t hrvv2_r0 = HRVv2_R0_DEFAULT;

volatile prcv14_r0B_t prcv14_r0B = PRCv14_R0B_DEFAULT;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void)  { *NVIC_ICPR = (0x1 << 0); wfi_timeout_flag = 1;} // TIMER32
void handler_ext_int_1(void)  { *NVIC_ICPR = (0x1 << 1);  } // TIMER16
void handler_ext_int_2(void)  { *NVIC_ICPR = (0x1 << 2); mbus_msg_flag = 0x10; } // REG0
void handler_ext_int_3(void)  { *NVIC_ICPR = (0x1 << 3); mbus_msg_flag = 0x11; } // REG1
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4); mbus_msg_flag = 0x12; } // REG2
void handler_ext_int_5(void)  { *NVIC_ICPR = (0x1 << 5); mbus_msg_flag = 0x13; } // REG3
void handler_ext_int_6(void)  { *NVIC_ICPR = (0x1 << 6); mbus_msg_flag = 0x14; } // REG4
void handler_ext_int_7(void)  { *NVIC_ICPR = (0x1 << 7); mbus_msg_flag = 0x15; } // REG5
void handler_ext_int_8(void)  { *NVIC_ICPR = (0x1 << 8); mbus_msg_flag = 0x16; } // REG6
void handler_ext_int_9(void)  { *NVIC_ICPR = (0x1 << 9); mbus_msg_flag = 0x17; } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); } // MEM WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); } // MBUS_FWD
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // MBUS_FWD


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
		| (44) 		// Binary converter's conversion ratio (7'h00)
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
		| (44) 		// Binary converter's conversion ratio (7'h00)
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


	}else if (read_data_batadc >= PMU_ADC_4P2_VAL + 6){
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
// Radio transmission routines for PPM Radio (RADv9)
//***************************************************

static void radio_power_on(){
	radv9_r2_t  radv9_r2_temp;
	radv9_r13_t radv9_r13_temp;

	// Turn off PMU ADC
	pmu_adc_disable();

	// Need to speed up sleep pmu clock
	set_pmu_sleep_radio();
	
	// This can be safely assumed
	radio_ready = 0;

    // Release FSM Sleep - Requires >2s stabilization time
    radio_on = 1;
	radv9_r13_temp.as_int = radv9_r13.as_int;
    radv9_r13_temp.RAD_FSM_SLEEP = 0;
	radv9_r13.as_int = radv9_r13_temp.as_int;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);
    // Release SCRO Reset
	radv9_r2_temp.as_int = radv9_r2.as_int;
    radv9_r2_temp.SCRO_RESET = 0;
	radv9_r2.as_int = radv9_r2_temp.as_int;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    
    // Additional delay required after SCRO Reset release
    delay(MBUS_DELAY*3); // At least 20ms required
    
    // Enable SCRO
	radv9_r2_temp.as_int = radv9_r2.as_int;
    radv9_r2_temp.SCRO_ENABLE = 1;
	radv9_r2.as_int = radv9_r2_temp.as_int;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    delay(MBUS_DELAY);

	// Release FSM Isolate
	radv9_r13_temp.as_int = radv9_r13.as_int;
	radv9_r13_temp.RAD_FSM_ISOLATE = 0;
	radv9_r13.as_int = radv9_r13_temp.as_int;
	mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
    delay(MBUS_DELAY);

}

static void radio_power_off(){
	radv9_r2_t  radv9_r2_temp;
	radv9_r13_t radv9_r13_temp;

	// Need to restore sleep pmu clock
	set_pmu_sleep_low();
	
	// Enable PMU ADC
	pmu_adc_enable();

    // Turn off everything
    radio_on = 0;
	radio_ready = 0;
	radv9_r2_temp.as_int = radv9_r2.as_int;
    radv9_r2_temp.SCRO_ENABLE = 0;
    radv9_r2_temp.SCRO_RESET  = 1;
	radv9_r2.as_int = radv9_r2_temp.as_int;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
	radv9_r13_temp.as_int = radv9_r13.as_int;
    radv9_r13_temp.RAD_FSM_SLEEP 	= 1;
    radv9_r13_temp.RAD_FSM_ISOLATE 	= 1;
    radv9_r13_temp.RAD_FSM_RESETn 	= 0;
    radv9_r13_temp.RAD_FSM_ENABLE 	= 0;
	radv9_r13.as_int = radv9_r13_temp.as_int;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
}

static void send_radio_data_ppm(uint32_t last_packet, uint32_t radio_data){
	radv9_r3_t  radv9_r3_temp;
	radv9_r13_t radv9_r13_temp;

    // Write Data: Only up to 24bit data for now
	radv9_r3_temp.as_int = radv9_r3.as_int;
	radv9_r3_temp.RAD_FSM_DATA = radio_data;
	radv9_r3.as_int = radv9_r3_temp.as_int;
    mbus_remote_register_write(RAD_ADDR,3,radv9_r3.as_int);

    if (!radio_ready){
		radio_ready = 1;

		// Release FSM Reset
		radv9_r13_temp.as_int = radv9_r13.as_int;
		radv9_r13_temp.RAD_FSM_RESETn = 1;
		radv9_r13.as_int = radv9_r13_temp.as_int;
		mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
		delay(MBUS_DELAY);
    }

	// Use Timer32 as timeout counter
	config_timer32(0x20000, 1, 0, 0); // 1/10 of MBUS watchdog timer default

    // Fire off data
    mbus_msg_flag = 0;
	wfi_timeout_flag = 0;
	radv9_r13_temp.as_int = radv9_r13.as_int;
    radv9_r13_temp.RAD_FSM_ENABLE = 1;
	radv9_r13.as_int = radv9_r13_temp.as_int;
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
		radv9_r13_temp.as_int = radv9_r13.as_int;
		radv9_r13_temp.RAD_FSM_ENABLE = 0;
		radv9_r13.as_int = radv9_r13_temp.as_int;
		mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
	}
}

//***************************************************
// Temp Sensor Functions (SNSv7)
//***************************************************

static void temp_sensor_enable(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_ENABLEb = 0x0;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_disable(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_ENABLEb = 1;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_release_reset(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_RESETn = 1;
	snsv7_r14_temp.TEMP_SENSOR_ISO = 0;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_assert_reset(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_RESETn = 0;
	snsv7_r14_temp.TEMP_SENSOR_ISO = 1;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void ldo_power_off(){
	snsv7_r18_t snsv7_r18_temp;
	snsv7_r18_temp.as_int = snsv7_r18.as_int;
	snsv7_r18_temp.ADC_LDO_ADC_LDO_DLY_ENB = 1;
	snsv7_r18_temp.ADC_LDO_ADC_LDO_ENB = 1;
	snsv7_r18.as_int = snsv7_r18_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
}
static void temp_power_off(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_ENABLEb = 1;
	snsv7_r14_temp.TEMP_SENSOR_RESETn = 0;
	snsv7_r14_temp.TEMP_SENSOR_ISO = 1;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	ldo_power_off();
}


//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

	// Reset IRQ14VEC
	*((volatile uint32_t *) IRQ14VEC) = 0;

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
    
    // Make sure LDO is off
    ldo_power_off();
	
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
    while(((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > temp_storage_count))){
		#ifdef DEBUG_MBUS_MSG_1
			mbus_write_message32(0xDD, radio_tx_count);
			delay(MBUS_DELAY);
			mbus_write_message32(0xDD, temp_storage[radio_tx_count]);
			delay(MBUS_DELAY);
		#endif

		// Reset watchdog timer
		config_timerwd(TIMERWD_VAL);

		// Radio out data
		send_radio_data_ppm(0, 0xBC0000 | temp_storage[radio_tx_count]);
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet

		radio_tx_count--;
    }

	delay(RADIO_PACKET_DELAY*2); //Set delays between sending subsequent packet
	send_radio_data_ppm(1, 0xFAF000);

	// This is also the end of this IRQ routine
	exec_count_irq = 0;

	// Go to sleep without timer
	radio_tx_count = temp_storage_count; // allows data to be sent more than once
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

	mbus_write_message32(0xE0, 0x0);
	// Prevent watchdog kicking in
   	config_timerwd(TIMERWD_VAL);

	uint32_t wakeup_timer_val_0 = *((volatile uint32_t *) REG_WUPT_VAL);
	wakeup_period_count = 0;

	while( *((volatile uint32_t *) REG_WUPT_VAL) == wakeup_timer_val_0){
		wakeup_period_count = 0;
	}
	wakeup_timer_val_0++;
	mbus_write_message32(0xE1, wakeup_timer_val_0);
	while( *((volatile uint32_t *) REG_WUPT_VAL) == wakeup_timer_val_0){
		wakeup_period_count++;
	}
	mbus_write_message32(0xE2, wakeup_period_count);
	delay(MBUS_DELAY);

   	config_timerwd(TIMERWD_VAL);
	WAKEUP_PERIOD_CONT = dumb_divide(WAKEUP_PERIOD_CONT_USER*1000*10, wakeup_period_count);
	mbus_write_message32(0xED, WAKEUP_PERIOD_CONT); 
	delay(MBUS_DELAY);
}


static void operation_init(void){
  
	prcv14_r0B_t prcv14_r0B_temp;
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r15_t snsv7_r15_temp;
	snsv7_r18_t snsv7_r18_temp;
	snsv7_r25_t snsv7_r25_temp;
	
	radv9_r0_t  radv9_r0_temp;
	radv9_r1_t  radv9_r1_temp;
	radv9_r11_t radv9_r11_temp;
	radv9_r12_t radv9_r12_temp;

	hrvv2_r0_t hrvv2_r0_temp;


	// Set CPU & Mbus Clock Speeds
	prcv14_r0B_temp.as_int = prcv14_r0B.as_int;
    prcv14_r0B_temp.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv14_r0B_temp.CLK_GEN_RING = 0x1; // Default 0x1
    prcv14_r0B_temp.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv14_r0B_temp.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    prcv14_r0B_temp.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prcv14_r0B_temp.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	prcv14_r0B.as_int = prcv14_r0B_temp.as_int;
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv14_r0B.as_int;

  
    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEE5;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;
	PMU_ADC_4P2_VAL = 0x4B;
	pmu_parkinglot_mode = 0;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
//    set_halt_until_mbus_rx();

    //Enumeration
 	mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//    set_halt_until_mbus_tx();

	// PMU Settings ----------------------------------------------
	set_pmu_clk_init();
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
	// SNSv7_R25
	snsv7_r25_temp.as_int = snsv7_r25.as_int;
	snsv7_r25_temp.TEMP_SENSOR_IRQ_PACKET = 0x001000;
	snsv7_r25.as_int = snsv7_r25_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0x19,snsv7_r25.as_int);
	// SNSv7_R14
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_BURST_MODE = 0x0;
	snsv7_r14_temp.TEMP_SENSOR_DELAY_SEL = 5;
	snsv7_r14_temp.TEMP_SENSOR_R_tmod = 0x0;
	snsv7_r14_temp.TEMP_SENSOR_R_bmod = 0x0;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	// snsv7_R15
	snsv7_r15_temp.as_int = snsv7_r15.as_int;
	snsv7_r15_temp.TEMP_SENSOR_AMP_BIAS   = 0x7; // Default: 2
	snsv7_r15_temp.TEMP_SENSOR_CONT_MODEb = 0x0;
	snsv7_r15_temp.TEMP_SENSOR_SEL_CT     = 6;
	snsv7_r15.as_int = snsv7_r15_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xF,snsv7_r15.as_int);

	// snsv7_R18
	snsv7_r18_temp.as_int = snsv7_r18.as_int;
	snsv7_r18_temp.ADC_LDO_ADC_LDO_ENB      = 0x1;
	snsv7_r18_temp.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;
	snsv7_r18_temp.ADC_LDO_ADC_CURRENT_2X   = 0x1;
	snsv7_r18_temp.ADC_LDO_ADC_VREF_MUX_SEL = 0x3; // Set ADC LDO to around 1.37V: 0x3//0x20
	snsv7_r18_temp.ADC_LDO_ADC_VREF_SEL     = 0x20; // Set ADC LDO to around 1.37V: 0x3//0x20
	snsv7_r18.as_int = snsv7_r18_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);

	// Temp sensor Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(SNS_ADDR,0x19,0x1800);


    // Radio Settings --------------------------------------
	radv9_r0_temp.as_int = radv9_r0.as_int;
	radv9_r0_temp.RADIO_TUNE_CURRENT_LIMITER = 0x2F; //Current Limiter 2F = 30uA, 1F = 3uA
	radv9_r0_temp.RADIO_TUNE_FREQ1 = 0x0; //Tune Freq 1
	radv9_r0_temp.RADIO_TUNE_FREQ2 = 0x0; //Tune Freq 2
	radv9_r0_temp.RADIO_TUNE_TX_TIME = 0x6; //Tune TX Time
	radv9_r0.as_int = radv9_r0_temp.as_int;
	mbus_remote_register_write(RAD_ADDR,0,radv9_r0.as_int);

	// FSM data length setups
	radv9_r11_temp.as_int = radv9_r11.as_int;
	radv9_r11_temp.RAD_FSM_H_LEN = 16; // N
	radv9_r11_temp.RAD_FSM_D_LEN = RADIO_DATA_LENGTH-1; // N-1
	radv9_r11_temp.RAD_FSM_C_LEN = 10;
	radv9_r11.as_int = radv9_r11_temp.as_int;
	mbus_remote_register_write(RAD_ADDR,11,radv9_r11.as_int);
	
	// Configure SCRO
	radv9_r1_temp.as_int = radv9_r1.as_int;
	radv9_r1_temp.SCRO_FREQ_DIV = 3;
	radv9_r1_temp.SCRO_AMP_I_LEVEL_SEL = 2; // Default 2
	radv9_r1_temp.SCRO_I_LEVEL_SELB = 0x60; // Default 0x6F
	radv9_r1.as_int = radv9_r1_temp.as_int;
	mbus_remote_register_write(RAD_ADDR,1,radv9_r1.as_int);
	
	// LFSR Seed
	radv9_r12_temp.as_int = radv9_r12.as_int;
	radv9_r12_temp.RAD_FSM_SEED = 4;
	radv9_r12.as_int = radv9_r12_temp.as_int;
	mbus_remote_register_write(RAD_ADDR,12,radv9_r12.as_int);
	
	// Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(RAD_ADDR,0xF,0x1900);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    temp_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    temp_run_single = 0;
    temp_running = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_temp_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
	RADIO_PACKET_DELAY = 2000;
	

    // Harvester Settings --------------------------------------
	hrvv2_r0_temp.as_int = hrvv2_r0.as_int;
	hrvv2_r0_temp.HRV_TOP_CONV_RATIO = 0x6;
	hrvv2_r0.as_int = hrvv2_r0_temp.as_int;
    mbus_remote_register_write(HRV_ADDR,0,hrvv2_r0.as_int);

    delay(MBUS_DELAY);

    // Go to sleep without timer
    operation_sleep_notimer();
}




//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    // Reset Wakeup Timer; This is required for PRCv13
    //set_wakeup_timer(200, 0, 1);

    // Initialize Interrupts
    // Only enable register-related interrupts
	//enable_reg_irq();
	enable_all_irq();
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

    // Initialization sequence
    if (enumerated != 0xDEADBEE5){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
    }


    // Check if wakeup is due to GOC interrupt  
    // 0x78 is reserved for GOC-triggered wakeup (Named IRQ14VEC)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *((volatile uint32_t *) IRQ14VEC);
    uint32_t wakeup_data_header = (wakeup_data>>24) & 0xFF;
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint32_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint32_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;


    if(wakeup_data_header == 1){
		// Read arbitrary PMU register	
		// PMU register read is handled differently
		mbus_remote_register_write(PMU_ADDR,0x00,wakeup_data & 0xFF);
		delay(MBUS_DELAY);
        operation_sleep_notimer();

    }else if(wakeup_data_header == 2){

    }else if(wakeup_data_header == 3){

    }else{
		if (wakeup_data_header != 0){
			// Invalid GOC trigger
            // Go to sleep without timer
            operation_sleep_notimer();
		}
	}


	
	operation_sleep_notimer();

    while(1);
}


