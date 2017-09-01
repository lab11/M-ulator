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
	
    // Make sure Radio is off

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
    enumerated = 0xDEADBEE9;
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

	// Change SNS lc clk
    mbus_remote_register_write(SNS_ADDR,0x16,(0x2<<2) | (1));

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


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    // Reset Wakeup Timer; This is required for PRCv13
    //set_wakeup_timer(100, 0, 1);

	mbus_write_message32(0xAA, 0x12341234);
    // Initialize Interrupts
    // Only enable register-related interrupts
	//enable_reg_irq();
	enable_all_irq();
  
	set_halt_until_mbus_tx();

    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

    // Initialization sequence
    if (enumerated != 0xDEADBEE9){
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
		// Register read test
		
		// Set CPU Halt Option as RX --> Use for register read e.g.

		set_halt_until_mbus_rx();
		
		mbus_remote_register_read(SNS_ADDR,10,1);
		mbus_remote_register_read(SNS_ADDR,9,1);
		mbus_remote_register_read(SNS_ADDR,7,1);
		
		set_halt_until_mbus_tx();

		operation_sleep_notimer;



    }else if(wakeup_data_header == 2){
		//	WFI test
		wfi_timeout_flag = 0;

		// Start CDC measurement
		snsv7_r0_t snsv7_r0_temp;
		snsv7_r0_temp.as_int = snsv7_r0.as_int;
		snsv7_r0_temp.CDCW_ENABLE = 0x1;
		snsv7_r0.as_int = snsv7_r0_temp.as_int;
		mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);

		// Use Timer32 as timeout counter
		config_timer32(0x20000, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Wait for temp sensor output
		WFI();

		// Turn off Timer32
		*TIMER32_GO = 0;
		Pstack_state = PSTK_CDC_READ;

		// Check if there was a timeout
		if (wfi_timeout_flag){
			mbus_write_message32(0xAA, 0xFAFAFAFA);
		}

        operation_sleep_notimer();
	

    }else if(wakeup_data_header == 3){
		// CPU clock speed up

		prcv14_r0B_t prcv14_r0B_temp;
		// Set CPU & Mbus Clock Speeds
		prcv14_r0B_temp.as_int = prcv14_r0B.as_int;
		prcv14_r0B_temp.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
		prcv14_r0B_temp.CLK_GEN_RING = 0x1; // Default 0x1
		prcv14_r0B_temp.CLK_GEN_DIV_MBC = 0x0; // Default 0x1
		prcv14_r0B_temp.CLK_GEN_DIV_CORE = 0x2; // Default 0x3
		prcv14_r0B_temp.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
		prcv14_r0B_temp.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
		prcv14_r0B.as_int = prcv14_r0B_temp.as_int;
		*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv14_r0B.as_int;

        operation_sleep_notimer();

    }else if(wakeup_data_header == 4){
		// CPU clock slow down

		prcv14_r0B_t prcv14_r0B_temp;
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

        operation_sleep_notimer();

    }else if(wakeup_data_header == 5){

		// Change SNS lc clk
		mbus_remote_register_write(SNS_ADDR,0x16,wakeup_data_field_0);
        operation_sleep_notimer();


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

