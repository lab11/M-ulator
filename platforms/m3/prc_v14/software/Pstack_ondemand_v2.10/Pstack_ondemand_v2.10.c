//****************************************************************************************************
//Author:       Gyouho Kim
//Description:  
//              Mouse Implantation & CDC Measurement Code
//              Revision 1.1 - optimization for implant
//              - HRV layer related configuration added
//              - Removed unnecessary debug functions
//              - Multiple CDC measurement and averaging
//              Revision 1.2
//              - Rename states / subfunctions for readability
//              - state machine merged to operation_cdc_run();
//              - Take average for measurement result TX (#TX_AVERAGE)
//              - Debug flags (#DEBUG_MBUS_MSG / #DEBUG_RADIO_MSG)
//              Revision 1.3
//              - System stays awake during CDC reset/measurement
//              - Data processing function added  (process_data)
//				Revision 1.3.1
// 				- Using PRCv9, but SNSv2 not SNSv3
//				Revision 1.3.2
// 				- Adding feature to change program parameter through GOC IRQ
//              Revision 1.4
//              - Now pressure measurements are stored and radioed out only when triggered
//				Revision 1.5
//				- Using SNSv3E instead of SNSv2
//              - Adding ADC functionality
//              - CDC with LDO
//				Revision 1.6
//				- Adding updates from SNSv3_test_v3.c
//				Revision 1.7
//				- For 2015 FEB ECO2: CDC interrupt is disabled. Need to poll.
//				Revision 1.8
//				- For 2015 APR Tapeout: SNSv5 with Wanyeong's CDC (CDCW)
//				- Updated radio configuration: pulse length & bitrate delay
//              10/27/2015
//              - Changed HRV HRV_TOP_CONV_RATIO from Default (0xE) to 0x6
//				Revision 1.9
//				- PRCv11
//				- SNSv6
//				- Support for RADv7 & RADv8 (ppm radio)
//				Revision 1.10
//				- Changes to correctly implement trigger 7
//				- PMU sleep frequency increased during CDC operation
//				- PMU active/sleep frequency and MBUS frequency adjustment for 2015 JUL Run
//				Revision 1.11
//				- Changing CDC measurement options for parasitic cancellation
//				Revision 1.12
//				- Changing CREF measurement to take place during sleep mode
//				Revision 1.13
//				- GOC frontend setting changed to default
//				- Adjusting PMU frequencies for TSMC 2015 JUL Poly2 run
//				Revision 1.14
//				- Adjusting PMU frequencies for TSMC 2015 DEC "Y2" run
//				- RADv9
//				- SNSv7
//				Version 2.0
//				- PRCv13 / PMUv2 / RADv9 / SNSv7
//				- RADv9 Setting for Y2 Run
//				- RADv9 Current limiter increased for double 8uAh batt
//				Version 2.1
//				- Adding VBAT measurement through PMUv2
//				- Changing how CDC timeout is handled; 0xFAFAFA will be recorded
//				Version 2.2
//				- Optimizing sleep current
//				- Changed radio packet letters
//				- Trig 4 stays awake for faster datarate
//				Version 2.3
//				- Reflecting updates from Tstack_ondemand_v1.15
//				- PRCv14 / PMUv4
//				Version 2.4
//				- Adding parking lot features (automatic vbat mornitoring & charging)
//				Version 2.5
//				- Adding 0xA and 0xB headers for data TX; assumes CDC output < 21b; packet delay increased
//				Version 2.6
//				- Add option to start charging when configuring parking
//				Version 2.7
//				- Packete delay is now configurable
//				- Initial trig 2 delay removed
//				Version 2.8
// 				- Multiple CDC readings per wakeup & only one data is stored (FIXME)
//				- During Trig 2, if radio option is on, transmit PMU ADC and count
//				- Data TX headers changed to 0xC and 0xD
//				Version 2.9
//				- PMU Sleep settings reduced to 1/1/1
//				- Parking high threshold changed to 4P2+1 from 4P2+2
//				Version 2.10
//				- Chip ID configuration
//				- Reducing LDO sleep time
//				- Consolidated trigger radio routine
//****************************************************************************************************
#include "PRCv14.h"
#include "PRCv14_RF.h"
#include "mbus.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"
#include "PMUv3_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// Stack order  PRC->RAD->SNS->HRV->PMU
#define HRV_ADDR 0x3
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

#define WAKEUP_PERIOD_PARKING 2000 // 200: ~200sec

// CDC parameters
#define	MBUS_DELAY 100 //Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 1
#define CDC_CYCLE_INIT 3

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_LDO1   	0x1
#define PSTK_CDC_READ   0x3
#define PSTK_CDC_RUN 	0x5

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define RADIO_TIMEOUT_COUNT 50
#define WAKEUP_PERIOD_RADIO_INIT 2

#define CDC_STORAGE_SIZE 320 // Need to leave about 500 Bytes for stack --> around 60
#define CDC_NUM_MEAS 3 

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y2 run default clock

//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t Pstack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t mbus_msg_flag;
volatile uint32_t PMU_ADC_4P2_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_harvesting_on;

volatile snsv7_r0_t snsv7_r0 = SNSv7_R0_DEFAULT;
volatile snsv7_r1_t snsv7_r1 = SNSv7_R1_DEFAULT;
volatile snsv7_r2_t snsv7_r2 = SNSv7_R2_DEFAULT;
volatile snsv7_r18_t snsv7_r18 = SNSv7_R18_DEFAULT;
  
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t cdc_storage[CDC_STORAGE_SIZE] = {0};
volatile uint32_t cdc_storage_cref[CDC_STORAGE_SIZE] = {0};
volatile uint32_t cdc_storage_cref_latest;
volatile uint32_t cdc_storage_count;
volatile uint32_t cdc_run_single;
volatile uint32_t cdc_running;
volatile uint32_t set_cdc_exec_count;
volatile uint32_t cdc_data_cmeas[CDC_NUM_MEAS] = {0};
volatile uint32_t cdc_data_cref[CDC_NUM_MEAS] = {0};
volatile uint32_t cdc_read_data;
volatile uint32_t cdc_read_data_cref;

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

//***************************************************
//Interrupt Handlers
//***************************************************
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

	// Register 0x34: TICK_ADC_CLK
	mbus_remote_register_write(PMU_ADDR,0x34,2);

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


inline static void set_pmu_sleep_clk_radio(){
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

inline static void set_pmu_sleep_clk_low(){
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
	
	if (read_data_batadc < (PMU_ADC_4P2_VAL + 1)){
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
	set_pmu_sleep_clk_radio();
	
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
	set_pmu_sleep_clk_low();
	
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
	config_timer32(150000, 1, 0, 0); // 1/10 of MBUS watchdog timer default

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
// CDCW Functions (SNSv7)
//***************************************************
static void release_cdc_pg(){
	snsv7_r0_t snsv7_r0_temp;
	snsv7_r0_temp.as_int = snsv7_r0.as_int;
    snsv7_r0_temp.CDCW_PG_V1P2 = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0_temp.as_int);
    delay(MBUS_DELAY);
    snsv7_r0_temp.CDCW_PG_VBAT = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0_temp.as_int);
    delay(MBUS_DELAY);
    snsv7_r0_temp.CDCW_PG_VLDO = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0_temp.as_int);
	snsv7_r0.as_int = snsv7_r0_temp.as_int;
}
static void release_cdc_isolate(){
	snsv7_r0_t snsv7_r0_temp;
	snsv7_r0_temp.as_int = snsv7_r0.as_int;
    snsv7_r0_temp.CDCW_ISO = 0x0;
	snsv7_r0.as_int = snsv7_r0_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
}
static void assert_cdc_reset(){
	snsv7_r0_t snsv7_r0_temp;
	snsv7_r0_temp.as_int = snsv7_r0.as_int;
    snsv7_r0_temp.CDCW_RESETn = 0x0;
	snsv7_r0.as_int = snsv7_r0_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
}
static void release_cdc_reset(){
	snsv7_r0_t snsv7_r0_temp;
	snsv7_r0_temp.as_int = snsv7_r0.as_int;
    snsv7_r0_temp.CDCW_RESETn = 0x1;
	snsv7_r0.as_int = snsv7_r0_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
}
static void fire_cdc_meas(){
	snsv7_r0_t snsv7_r0_temp;
	snsv7_r0_temp.as_int = snsv7_r0.as_int;
    snsv7_r0_temp.CDCW_ENABLE = 0x1;
	snsv7_r0.as_int = snsv7_r0_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
}
static void release_cdc_meas(){
	snsv7_r0_t snsv7_r0_temp;
	snsv7_r0_temp.as_int = snsv7_r0.as_int;
    snsv7_r0_temp.CDCW_ENABLE = 0x0;
	snsv7_r0.as_int = snsv7_r0_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
}
static void ldo_power_off(){
	snsv7_r18_t snsv7_r18_temp;
	snsv7_r18_temp.as_int = snsv7_r18.as_int;
    snsv7_r18_temp.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
    snsv7_r18_temp.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
    snsv7_r18_temp.CDC_LDO_CDC_LDO_ENB = 0x1;
    snsv7_r18_temp.ADC_LDO_ADC_LDO_ENB = 0x1;
	snsv7_r18.as_int = snsv7_r18_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
}
static void cdc_power_off(){
	snsv7_r0_t snsv7_r0_temp;
	snsv7_r0_temp.as_int = snsv7_r0.as_int;
    snsv7_r0_temp.CDCW_ISO = 0x1;
    snsv7_r0_temp.CDCW_PG_V1P2 = 0x1;
    snsv7_r0_temp.CDCW_PG_VBAT = 0x1;
    snsv7_r0_temp.CDCW_PG_VLDO = 0x1;
	snsv7_r0.as_int = snsv7_r0_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
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

    // Go to sleep without timer
    operation_sleep();

}

static void operation_tx_stored(void){

    while(((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > cdc_storage_count))){
		// Config watchdog timer to about 10 sec; default: 0x02FFFFFF
		config_timerwd(0xFFFFF); // 0xFFFFF about 13 sec with Y2 run default clock

		//Fire off stored data to radio
		#ifdef DEBUG_MBUS_MSG
			delay(MBUS_DELAY*10);
			mbus_write_message32(0x70, radio_tx_count);
			delay(MBUS_DELAY*10);
			mbus_write_message32(0x74, cdc_storage[radio_tx_count]);
			delay(MBUS_DELAY*10);
			mbus_write_message32(0x76, cdc_storage_cref[radio_tx_count]);
			delay(MBUS_DELAY*10);
			mbus_write_message32(0x70, radio_tx_count);
			delay(MBUS_DELAY*10);
		#endif
		send_radio_data_ppm(0, 0xC00000 | (0xFFFFF & cdc_storage[radio_tx_count]));
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & cdc_storage_cref[radio_tx_count]));
		delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet

		radio_tx_count--;
	}

	send_radio_data_ppm(0, 0xC00000 | (0xFFFFF & cdc_storage[radio_tx_count]));
	delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
	send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & cdc_storage_cref[radio_tx_count]));

	delay(RADIO_PACKET_DELAY*3);
	send_radio_data_ppm(1, 0xFAF000);
	// This is also the end of this IRQ routine
	exec_count_irq = 0;
	// Go to sleep without timer
	radio_tx_count = cdc_storage_count; // allows data to be sent more than once
	operation_sleep_notimer();

}

static void operation_init(void){
  
	prcv14_r0B_t prcv14_r0B_temp;
	snsv7_r0_t snsv7_r0_temp;
	snsv7_r1_t snsv7_r1_temp;
	snsv7_r2_t snsv7_r2_temp;
	snsv7_r18_t snsv7_r18_temp;
	
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
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEA;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;
	PMU_ADC_4P2_VAL = 0x4B;
	pmu_parkinglot_mode = 0;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(RAD_ADDR);
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

    // CDC Settings --------------------------------------
    // snsv7_r0
	snsv7_r0_temp.as_int = snsv7_r0.as_int;
    snsv7_r0_temp.CDCW_IRQ_EN	= 1;
    snsv7_r0_temp.CDCW_MODE_PAR	= 1; // Default: 1
    snsv7_r0_temp.CDCW_RESETn 	= 0;
	snsv7_r0.as_int = snsv7_r0_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
  
    // snsv7_r1
	snsv7_r1_temp.as_int = snsv7_r1.as_int;
    snsv7_r1_temp.CDCW_N_CYCLE_SINGLE	= 0; // Default: 8; Min: 0
	snsv7_r1.as_int = snsv7_r1_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,1,snsv7_r1.as_int);
  
    // snsv7_r2
	snsv7_r2_temp.as_int = snsv7_r2.as_int;
    snsv7_r2_temp.CDCW_N_CYCLE_SET	= 100; // Min: 0
	snsv7_r2.as_int = snsv7_r2_temp.as_int;
    mbus_remote_register_write(SNS_ADDR,2,snsv7_r2.as_int);
  
    // snsv7_r18
	snsv7_r18_temp.as_int = snsv7_r18.as_int;
    snsv7_r18_temp.CDC_LDO_CDC_CURRENT_2X  = 0;
  
    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv7_r18_temp.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv7_r18_temp.ADC_LDO_ADC_VREF_SEL     = 0x20;
  
    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv7_r18_temp.CDC_LDO_CDC_VREF_MUX_SEL = 0x3;
    snsv7_r18_temp.CDC_LDO_CDC_VREF_SEL     = 0x20;
	snsv7_r18.as_int = snsv7_r18_temp.as_int;

    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
  
	// Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(SNS_ADDR,0x18,0x1800);

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
    WAKEUP_PERIOD_CONT = 5;
    WAKEUP_PERIOD_CONT_INIT = 1;
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
    cdc_run_single = 0;
    cdc_running = 0;
	wfi_timeout_flag = 0;
    radio_ready = 0;
    radio_on = 0;
	wakeup_data = 0;
	set_cdc_exec_count = 0;
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



//***************************************************
// Pressure measurement operation (SNSv7)
//***************************************************

/*
static void cdc_data_median(){

	if (cdc_data_cmeas[0] > cdc_data_cmeas[1]){
		if (cdc_data_cmeas[1] > cdc_data_cmeas[2]){
			cdc_read_data = cdc_data_cmeas[1];
			cdc_read_data_cref = cdc_data_cref[1];
		}else if (cdc_data_cmeas[0] > cdc_data_cmeas[2]){
			cdc_read_data = cdc_data_cmeas[2];
			cdc_read_data_cref = cdc_data_cref[2];
		}else{
			cdc_read_data = cdc_data_cmeas[0];
			cdc_read_data_cref = cdc_data_cref[0];	
		}
	}else{
		if (cdc_data_cmeas[0] > cdc_data_cmeas[2]){
			cdc_read_data = cdc_data_cmeas[0];
			cdc_read_data_cref = cdc_data_cref[0];
		}else if (cdc_data_cmeas[1] > cdc_data_cmeas[2]){
			cdc_read_data = cdc_data_cmeas[2];
			cdc_read_data_cref = cdc_data_cref[2];
		}else{
			cdc_read_data = cdc_data_cmeas[1];
			cdc_read_data_cref = cdc_data_cref[1];
		}
	}

}
*/

static uint32_t cdc_data_median(){

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xD0, cdc_data_cmeas[0]);
		mbus_write_message32(0xD1, cdc_data_cmeas[1]);
		mbus_write_message32(0xD2, cdc_data_cmeas[2]);
	#endif
	

	if (cdc_data_cmeas[0] > cdc_data_cmeas[1]){
		if (cdc_data_cmeas[1] > cdc_data_cmeas[2]){
			return 1;
		}else if (cdc_data_cmeas[0] > cdc_data_cmeas[2]){
			return 2;
		}else{	
			return 0;
		}
	}else{
		if (cdc_data_cmeas[0] > cdc_data_cmeas[2]){
			return 0;
		}else if (cdc_data_cmeas[1] > cdc_data_cmeas[2]){
			return 2;
		}else{
			return 1;
		}
	}

}


static void operation_cdc_run(){
    snsv7_r18_t snsv7_r18_temp;

    if (Pstack_state == PSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x0);
		#endif
		Pstack_state = PSTK_LDO1;

		wfi_timeout_flag = 0;

		// Power on radio
		if (radio_tx_option || ((exec_count+1) < CDC_CYCLE_INIT)){
			radio_power_on();
		}

		snsv7_r18_temp.as_int = snsv7_r18.as_int;
		snsv7_r18_temp.CDC_LDO_CDC_LDO_ENB = 0x0;
		snsv7_r18.as_int = snsv7_r18_temp.as_int;
		mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
		delay(MBUS_DELAY);
		// Long delay required here
		// Put system to sleep
		set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
		operation_sleep_noirqreset();

    }else if (Pstack_state == PSTK_LDO1){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x1);
		#endif
		Pstack_state = PSTK_CDC_RUN;
		snsv7_r18_temp.as_int = snsv7_r18.as_int;
		snsv7_r18_temp.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
		snsv7_r18.as_int = snsv7_r18_temp.as_int;
		mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
		delay(MBUS_DELAY);
		// Put system to sleep
		set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
		operation_sleep_noirqreset();
		
    }else if (Pstack_state == PSTK_CDC_RUN){
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xAA, 0x11111111);
		delay(MBUS_DELAY);
	#endif

		if (meas_count == 0){
			// Release CDC isolation
			release_cdc_pg();
			delay(MBUS_DELAY);
			release_cdc_isolate();
			delay(MBUS_DELAY);

			// Release reset
			release_cdc_reset();
			delay(MBUS_DELAY);
		}
		mbus_msg_flag = 0;
		wfi_timeout_flag = 0;

		// Use Timer32 as timeout counter
		config_timer32(0x249F0, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Start CDC measurement
		fire_cdc_meas();

		// Wait for temp sensor output
		WFI();

		// Turn off Timer32
		*TIMER32_GO = 0;
		Pstack_state = PSTK_CDC_READ;

		// Check if there was a timeout
		if (wfi_timeout_flag){
			mbus_write_message32(0xAA, 0xFAFAFAFA);
		}


    }else if (Pstack_state == PSTK_CDC_READ){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x22222222);
		#endif

		// Grab CDC Data
    	uint32_t read_data_reg4; // CONFIG 0; CMEAS
    	uint32_t read_data_reg6; // CONFIG 1; CREF1
    	uint32_t read_data_reg7; // CONFIG 2; CMEAS reverse
    	uint32_t read_data_reg9; // CONFIG 4; CPAR
    	uint32_t read_data_reg10; // CONFIG 4; CREF5
    	uint32_t cdc_read_data;      // Read data after parasitic cancellation

		// Set CPU Halt Option as RX --> Use for register read e.g.
		set_halt_until_mbus_rx();
		
		mbus_remote_register_read(SNS_ADDR,10,1);
		read_data_reg10 = *((volatile uint32_t *) 0xA0000004);
		mbus_remote_register_read(SNS_ADDR,9,1);
		read_data_reg9 = *((volatile uint32_t *) 0xA0000004);
		mbus_remote_register_read(SNS_ADDR,7,1);
		read_data_reg7 = *((volatile uint32_t *) 0xA0000004);
		mbus_remote_register_read(SNS_ADDR,6,1);
		read_data_reg6 = *((volatile uint32_t *) 0xA0000004);
		mbus_remote_register_read(SNS_ADDR,4,1);
		read_data_reg4 = *((volatile uint32_t *) 0xA0000004);
		
		// Set CPU Halt Option as TX --> Use for register write e.g.
		set_halt_until_mbus_tx();

		cdc_read_data = (read_data_reg4+read_data_reg7-read_data_reg9)/2;
		cdc_read_data_cref = read_data_reg6;

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0x74, cdc_read_data);
		delay(MBUS_DELAY*20);
		mbus_write_message32(0x76, cdc_read_data_cref);
	#endif

		if (wfi_timeout_flag){
			cdc_read_data = 0;
			cdc_read_data_cref = 0;
		}

		cdc_data_cmeas[meas_count] = cdc_read_data;
		cdc_data_cref[meas_count] = cdc_read_data_cref;
		meas_count++;

		// Option to take multiple measurements per wakeup
		if (meas_count < CDC_NUM_MEAS){	
			// Repeat measurement while awake
			release_cdc_meas();
			Pstack_state = PSTK_CDC_RUN;
				
		}else{
			meas_count = 0;

			// Finalize CDC operation
			release_cdc_meas();
			assert_cdc_reset();
			Pstack_state = PSTK_IDLE;
			
			uint32_t median_idx = cdc_data_median();
			mbus_write_message32(0xC1, cdc_data_cmeas[median_idx]);
			mbus_write_message32(0xC2, cdc_data_cref[median_idx]);
				
			// Assert CDC isolation & turn off CDC power
			cdc_power_off();

			#ifdef DEBUG_MBUS_MSG
				mbus_write_message32(0xAA, 0x33333333);
			#endif

			// Check if this is for VBAT measurement
			if (cdc_run_single){
				cdc_run_single = 0;
				cdc_storage_cref_latest = read_data_reg10;
				return;

			}else{
				exec_count++;

			
				// Store results in memory; unless buffer is full
				if (cdc_storage_count < CDC_STORAGE_SIZE){
					cdc_storage[cdc_storage_count] = cdc_data_cmeas[median_idx];
					cdc_storage_cref[cdc_storage_count] = cdc_data_cref[median_idx];
					cdc_storage_cref_latest = cdc_data_cref[median_idx];
					radio_tx_count = cdc_storage_count;
					cdc_storage_count++;
				}

				// Optionally transmit the data
				if (radio_tx_option){
					// Read latest PMU ADC measurement
					pmu_adc_read_latest();
					send_radio_data_ppm(0, exec_count);
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(0,0xBBB000+read_data_batadc);	
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(0, 0xC00000 | (0xFFFFF & cdc_data_cmeas[median_idx]));
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(0, 0xD00000 | (0xFFFFF & cdc_data_cref[median_idx]));
					delay(RADIO_PACKET_DELAY);
				}

				// Enter long sleep
				if(exec_count < CDC_CYCLE_INIT){
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

				if ((set_cdc_exec_count != 0) && (exec_count > (50<<set_cdc_exec_count))){
					// No more measurement required
					// Make sure CDC is off
					cdc_running = 0;
					cdc_power_off();
					operation_sleep_notimer();
				}else{
					operation_sleep_noirqreset();
					
				}

			}
		}

    }else{
        //default:  // THIS SHOULD NOT HAPPEN
		// Reset CDC
		assert_cdc_reset();
		cdc_power_off();
		operation_sleep_notimer();
    }

}


static void operation_goc_trigger_init(void){

	// This is critical
	set_halt_until_mbus_tx();
	mbus_write_message32(0xAA,0xABCD1234);
	mbus_write_message32(0xAA,wakeup_data);

	// Initialize variables & registers
	cdc_running = 0;
	Pstack_state = PSTK_IDLE;
	
	radio_power_off();
	ldo_power_off();
	cdc_power_off();
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

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    // Reset Wakeup Timer; This is required for PRCv13
    //set_wakeup_timer(100, 0, 1);

    // Initialize Interrupts
    // Only enable register-related interrupts
	enable_reg_irq();
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

    // Initialization sequence
    if (enumerated != 0xDEADBEEA){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
    }

	// Parking lot feature
	if ((pmu_parkinglot_mode > 0) && (exec_count_irq == 0)){
		mbus_write_message32(0xAA,0xABCDDCBA);
		pmu_adc_read_latest();
		pmu_parkinglot_decision();
	}

    // Check if wakeup is due to GOC interrupt  
    // 0x78 is reserved for GOC-triggered wakeup (Named IRQ14VEC)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *((volatile uint32_t *) IRQ14VEC);
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
		// Slow down PMU sleep osc and run CDC code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:16] is the initial user-specified period
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[23:21] specifies how many cdc executes; 0: unlimited, n: 50*2^n
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2 & 0xF;
        radio_tx_option = wakeup_data_field_2 & 0x10;

		cdc_run_single = 0;

		if (!cdc_running){
			// Go to sleep for initial settling of pressure // disabled now
			//set_wakeup_timer(5, 0x1, 0x1); // 150: around 5 min
			cdc_running = 1;
			set_cdc_exec_count = wakeup_data_field_2 >> 5;
            exec_count_irq++;
			//operation_sleep_noirqreset();
		}
		exec_count = 0;
		meas_count = 0;
		cdc_storage_count = 0;
		radio_tx_count = 0;

		// Reset IRQ14VEC
		*((volatile uint32_t *) IRQ14VEC) = 0;
        exec_count_irq = 0;

		// Run CDC Program
    	Pstack_state = PSTK_IDLE;
		operation_cdc_run();

    }else if(wakeup_data_header == 3){
		// Stop CDC program and transmit the battery reading and execution count (alternating n times)
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		cdc_running = 0;
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
					send_radio_data_ppm(0,0xBC0000+exec_count);	
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
        // Transmit the stored cdc data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		radio_tx_numdata = wakeup_data_field_0;
		cdc_running = 0;
		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= cdc_storage_count){
			radio_tx_numdata = 0;
		}
		
        if (exec_count_irq < 3){
			exec_count_irq++;
			if (exec_count_irq == 1){
				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				send_radio_data_ppm(0, 0xABC000+exec_count_irq);
				if (exec_count_irq == 3){
					// set timer
					set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				}else{
					// set timer
					set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				}
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
				pmu_parkinglot_decision();
				if (wakeup_data_field_2 & 0x4){
					// Register 0x0E: PMU_VOLTAGE_CLAMP_TRIM
					mbus_remote_register_write(PMU_ADDR,0x0E, 
					   (    ( 0 << 10) // When to turn on Harvester Inhibiting Switch
						  | ( 1 <<  9) // Enables Override Settings [8]
						  | ( 0 <<  8) // Turn On Harvester Inhibiting Switch
						  | ( 1 <<  4) // Clamp Tune Bottom
						  | ( 0 <<  0) // Clamp Tune Top
						  ));
				}
			}else if (pmu_parkinglot_mode == 0){
				// Start harvesting and let solar short be determined in hardware
				pmu_reset_solar_short();
				pmu_harvesting_on = 1;
			}
		}

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xBBB000, read_data_batadc);

	/*
	  }else if(wakeup_data_header == 0x12){
	  // Change the sleep period
	  // wakeup_data[15:0] is the user-specified period
	  // wakeup_data[23:16] determines whether to resume CDC operation or not
	  WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
		
	  if (wakeup_data_field_2){
	  // Resume CDC operation
	  operation_cdc_run();
	  }else{
	  // Go to sleep without timer
	  operation_sleep_notimer();
	  }
	*/

	}else if(wakeup_data_header == 0x13){
		// Change the RF frequency
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the desired RF tuning value (RADv9)

		radv9_r0_t radv9_r0_temp;
		radv9_r0_temp.as_int = radv9_r0.as_int;
		radv9_r0_temp.RADIO_TUNE_FREQ1 = wakeup_data_field_2>>4; 
		radv9_r0_temp.RADIO_TUNE_FREQ2 = wakeup_data_field_2 & 0xF; 
		radv9_r0.as_int = radv9_r0_temp.as_int;
    	mbus_remote_register_write(RAD_ADDR,0,radv9_r0.as_int);

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xABC000, exec_count_irq);

	}else if(wakeup_data_header == 0x17){
		// Set parking lot threshold
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the desired parking lot threshold value
		//					-->  if zero: uses the latest PMU ADC measurement
		
		if (wakeup_data_field_2 == 0){
			// Read latest PMU ADC measurement
			pmu_adc_read_latest();
			PMU_ADC_4P2_VAL = read_data_batadc;
		}else{
			PMU_ADC_4P2_VAL = wakeup_data_field_2;
		}

		operation_goc_trigger_radio(wakeup_data_field_0, wakeup_data_field_1, 0xABC000, PMU_ADC_4P2_VAL);


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

		operation_goc_trigger_radio(wakeup_data_field_0, 2, 0xCC0000, chip_id_user);

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

    }else{
		if (wakeup_data_header != 0){
			// Invalid GOC trigger
            // Go to sleep without timer
            operation_sleep_notimer();
		}
	}

    // Proceed to continuous mode
	if (cdc_running){
		while(1){
			operation_cdc_run();
		}
	}

    operation_sleep_notimer();

    while(1);
}

