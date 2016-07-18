//****************************************************************************************************
//Author:       Gyouho Kim
//Description:  
//              Mouse Implantation & CDC Measurement Code
//				Version 2.1
//				- Adding VBAT measurement through PMUv2
//				- Changing how CDC timeout is handled; 0xFAFAFA will be recorded
//				Version 2.0
//				- PRCv13 / PMUv2 / RADv9 / SNSv7
//				- RADv9 Setting for Y2 Run
//				- RADv9 Current limiter increased for double 8uAh batt
//				Revision 1.14
//				- Adjusting PMU frequencies for TSMC 2015 DEC "Y2" run
//				- RADv9
//				- SNSv7
//				Revision 1.13
//				- GOC frontend setting changed to default
//				- Adjusting PMU frequencies for TSMC 2015 JUL Poly2 run
//				Revision 1.12
//				- Changing CREF measurement to take place during sleep mode
//				Revision 1.11
//				- Changing CDC measurement options for parasitic cancellation
//				Revision 1.10
//				- Changes to correctly implement trigger 7
//				- PMU sleep frequency increased during CDC operation
//				- PMU active/sleep frequency and MBUS frequency adjustment for 2015 JUL Run
//				Revision 1.9
//				- PRCv11
//				- SNSv6
//				- Support for RADv7 & RADv8 (ppm radio)
//              10/27/2015
//              - Changed HRV HRV_TOP_CONV_RATIO from Default (0xE) to 0x6
//				Revision 1.8
//				- For 2015 APR Tapeout: SNSv5 with Wanyeong's CDC (CDCW)
//				- Updated radio configuration: pulse length & bitrate delay
//				Revision 1.7
//				- For 2015 FEB ECO2: CDC interrupt is disabled. Need to poll.
//				Revision 1.6
//				- Adding updates from SNSv3_test_v3.c
//				Revision 1.5
//				- Using SNSv3E instead of SNSv2
//              - Adding ADC functionality
//              - CDC with LDO
//              Revision 1.4
//              - Now pressure measurements are stored and radioed out only when triggered
//				Revision 1.3.2
// 				- Adding feature to change program parameter through GOC IRQ
//				Revision 1.3.1
// 				- Using PRCv9, but SNSv2 not SNSv3
//              Revision 1.3
//              - System stays awake during CDC reset/measurement
//              - Data processing function added  (process_data)
//              Revision 1.2
//              - Rename states / subfunctions for readability
//              - state machine merged to operation_cdc_run();
//              - Take average for measurement result TX (#TX_AVERAGE)
//              - Debug flags (#DEBUG_MBUS_MSG / #DEBUG_RADIO_MSG)
//              Revision 1.1 - optimization for implant
//              - HRV layer related configuration added
//              - Removed unnecessary debug functions
//              - Multiple CDC measurement and averaging
//****************************************************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"
#include "PMUv2_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->RAD->SNS->HRV->PMU
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6
#define PMU_ADDR 0x7

// CDC parameters
#define	MBUS_DELAY 100 //Amount of delay between successive messages; 100: 6-7ms
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 1000
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 2
#define CDC_CYCLE_INIT 4

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_CDC_RST    0x1
#define PSTK_CDC_READ   0x3
#define PSTK_LDO1   	0x4
#define PSTK_LDO2  	 	0x5

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define RADIO_PACKET_DELAY 3000
#define RADIO_TIMEOUT_COUNT 100
#define WAKEUP_PERIOD_RADIO_INIT 2

#define CDC_STORAGE_SIZE 400 // FIXME

//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint8_t Pstack_state;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint8_t mbus_msg_flag;

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
volatile bool cdc_run_single;
volatile bool cdc_running;
volatile uint32_t cdc_reset_timeout_count;
volatile bool cdc_timeout_flag;
volatile uint32_t set_cdc_exec_count;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile bool radio_ready;
volatile bool radio_on;

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

volatile prcv13_r0B_t prcv13_r0B = PRCv13_R0B_DEFAULT;

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

void handler_ext_int_0(void)  { *NVIC_ICPR = (0x1 << 0);  } // TIMER32
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

inline static void set_pmu_sleep_clk_init(){
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// The first register write to PMU needs to be repeated
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (4) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (10) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (1 << 13) // Enable main feedback loop
		| (2 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (2) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (8 << 9)  // Frequency multiplier R
		| (4 << 5)  // Frequency multiplier L (actually L+1)
		| (8) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (2 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (6) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x16: SAR_TRIM_v3_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (8 << 9)  // Frequency multiplier R
		| (8 << 5)  // Frequency multiplier L (actually L+1)
		| (15) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 11) // Enable override setting [10] (1'h0)
		| (0 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (44) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,0x000001);
	delay(MBUS_DELAY);
}

inline static void batadc_reset(){
	// Manually reset ADC
	// PMU_CONTROLLER_DESIRED_STATE
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

inline static void batadc_resetrelease(){
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
		| (0 << 12) //state_adc_adjusted
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
// Radio transmission routines for PPM Radio (RADv9)
//***************************************************

static void radio_power_on(){
	// Need to speed up sleep pmu clock
	//set_pmu_sleep_clk_high();

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
	//set_pmu_sleep_clk_default();

    // Turn off everything
    radio_on = 0;
    radv9_r2.SCRO_ENABLE = 0;
    radv9_r2.SCRO_RESET  = 1;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    radv9_r13.RAD_FSM_SLEEP 	= 1;
    radv9_r13.RAD_FSM_ISOLATE 	= 1;
    radv9_r13.RAD_FSM_RESETn 	= 0;
    radv9_r13.RAD_FSM_ENABLE 	= 0;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
}

static void send_radio_data_ppm(bool last_packet, uint32_t radio_data){
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

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xBB, 0x0);
	#endif

    // Set CPU Halt Option as RX --> Use for register read e.g.
    set_halt_until_mbus_rx();

    // Fire off data
    uint32_t count;
    mbus_msg_flag = 0;
    radv9_r13.RAD_FSM_ENABLE = 1;
    mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);

    for( count=0; count<RADIO_TIMEOUT_COUNT; count++ ){
		if( mbus_msg_flag ){
    		set_halt_until_mbus_tx();
			mbus_msg_flag = 0;
			if (last_packet){
				radio_ready = 0;
				radio_power_off();
			}else{
				radv9_r13.RAD_FSM_ENABLE = 0;
				mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);
			}
			return;
		}else{
			delay(MBUS_DELAY);
		}
    }
	
    // Timeout
    set_halt_until_mbus_tx();
	mbus_write_message32(0xBB, 0xFAFAFAFA);
}


//***************************************************
// CDCW Functions
// snsv7
//***************************************************
static void release_cdc_pg(){
    snsv7_r0.CDCW_PG_V1P2 = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
    snsv7_r0.CDCW_PG_VBAT = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
    snsv7_r0.CDCW_PG_VLDO = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_isolate(){
    snsv7_r0.CDCW_ISO = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void assert_cdc_reset(){
    snsv7_r0.CDCW_RESETn = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_reset(){
    snsv7_r0.CDCW_RESETn = 0x1;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void fire_cdc_meas(){
    snsv7_r0.CDCW_ENABLE = 0x1;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_meas(){
    snsv7_r0.CDCW_ENABLE = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void ldo_power_off(){
    snsv7_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
    snsv7_r18.CDC_LDO_CDC_LDO_ENB = 0x1;
    snsv7_r18.ADC_LDO_ADC_LDO_ENB = 0x1;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
    delay(MBUS_DELAY);
}
static void cdc_power_off(){
    snsv7_r0.CDCW_ISO = 0x1;
    snsv7_r0.CDCW_PG_V1P2 = 0x1;
    snsv7_r0.CDCW_PG_VBAT = 0x1;
    snsv7_r0.CDCW_PG_VLDO = 0x1;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
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

    // Disable Timer
    set_wakeup_timer(0, 0, 0);

    // Go to sleep without timer
    operation_sleep();

}

static void operation_tx_stored(void){

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
    send_radio_data_ppm(0, cdc_storage[radio_tx_count]);
    delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
    send_radio_data_ppm(0, cdc_storage_cref[radio_tx_count]);

    if (((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > cdc_storage_count))){
		radio_tx_count--;
		// set timer
		set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();

    }else{
		delay(RADIO_PACKET_DELAY);
		delay(RADIO_PACKET_DELAY);
		send_radio_data_ppm(1, 0xFAF000);
		// This is also the end of this IRQ routine
		exec_count_irq = 0;
		// Go to sleep without timer
		radio_tx_count = cdc_storage_count; // allows data to be sent more than once
		operation_sleep_notimer();
    }

}

static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;

  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
	delay(MBUS_DELAY);
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

	// Disable PMUv2 IRQ
    mbus_remote_register_write(PMU_ADDR,0x51,0x09);
	delay(MBUS_DELAY);

	set_pmu_sleep_clk_init();

    // CDC Settings --------------------------------------
    // snsv7_r0
    snsv7_r0.CDCW_IRQ_EN	= 1;
    snsv7_r0.CDCW_MODE_PAR	= 1; // Default: 1
    snsv7_r0.CDCW_RESETn 	= 0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
  
    // snsv7_r1
    snsv7_r1.CDCW_N_CYCLE_SINGLE	= 0; // Default: 8; Min: 0
    mbus_remote_register_write(SNS_ADDR,1,snsv7_r1.as_int);
  
    // snsv7_r2
    snsv7_r2.CDCW_N_CYCLE_SET	= 100; // Min: 0
    mbus_remote_register_write(SNS_ADDR,2,snsv7_r2.as_int);
  
    // snsv7_r18
    snsv7_r18.CDC_LDO_CDC_CURRENT_2X  = 0;
  
    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv7_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv7_r18.ADC_LDO_ADC_VREF_SEL     = 0x20;
  
    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv7_r18.CDC_LDO_CDC_VREF_MUX_SEL = 0x2;
    snsv7_r18.CDC_LDO_CDC_VREF_SEL     = 0x20;
  
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
  
	// Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(SNS_ADDR,0x18,0x1800);

    // Radio Settings --------------------------------------
    radv9_r0.RADIO_TUNE_CURRENT_LIMITER = 0x2F; //Current Limiter 2F = 30uA, 1F = 3uA
    radv9_r0.RADIO_TUNE_FREQ1 = 0x0; //Tune Freq 1
    radv9_r0.RADIO_TUNE_FREQ2 = 0x9; //Tune Freq 2
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
    mbus_remote_register_write(RAD_ADDR,0xF,0x1900);
    delay(MBUS_DELAY);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 5;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
    cdc_run_single = 0;
    cdc_running = 0;
	cdc_timeout_flag = 0;
    radio_ready = 0;
    radio_on = 0;
	set_cdc_exec_count = 0;

    // Harvester Settings --------------------------------------
    hrvv2_r0.HRV_TOP_CONV_RATIO = 0x6;
    mbus_remote_register_write(HRV_ADDR,0,hrvv2_r0.as_int);

    delay(MBUS_DELAY);

	// Disable PMU ADC measurement in active mode
	// PMU_CONTROLLER_STALL_ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x3A, 
		( (1 << 19) // ignore state_horizon; default 1
		| (1 << 11) // ignore adc_output_ready; default 0
	));
    delay(MBUS_DELAY);

	// Disable ADC offset measurement
	batadc_resetrelease();

    // Go to sleep without timer
    operation_sleep_notimer();
}

static void operation_cdc_run(){
    uint32_t count; 

    if (Pstack_state == PSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x0);
		#endif
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;
		cdc_timeout_flag = 0;

		// Power on radio
		if (radio_tx_option || ((exec_count+1) < CDC_CYCLE_INIT)){
			radio_power_on();
		}

		snsv7_r18.CDC_LDO_CDC_LDO_ENB = 0x0;
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
		Pstack_state = PSTK_LDO2;
		snsv7_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
		mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
		delay(MBUS_DELAY);
		// Put system to sleep
		set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
		operation_sleep_noirqreset();
		
	}else if (Pstack_state == PSTK_LDO2){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x2);
		#endif
		Pstack_state = PSTK_CDC_RST;

		// Release CDC isolation
		release_cdc_pg();
		delay(MBUS_DELAY*2);
		release_cdc_isolate();

    }else if (Pstack_state == PSTK_CDC_RST){
	// Release reset
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xAA, 0x11111111);
		delay(MBUS_DELAY);
	#endif

		mbus_msg_flag = 0;
		release_cdc_reset();
		delay(MBUS_DELAY*2);
		fire_cdc_meas();

		if (cdc_run_single){
			Pstack_state = PSTK_CDC_READ;
			set_wakeup_timer(WAKEUP_PERIOD_RESET, 0x1, 0x1);
			operation_sleep_noirqreset();
		}

		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
			if( mbus_msg_flag ){
				mbus_msg_flag = 0;
				cdc_reset_timeout_count = 0;
				Pstack_state = PSTK_CDC_READ;

				//set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
				//operation_sleep_noirqreset();
				return;
			}else{
				delay(MBUS_DELAY);
			}
		}

		// Time out
		mbus_write_message32(0xAA, 0xFAFAFAFA);

		release_cdc_meas();


		if (cdc_reset_timeout_count > 0){
			cdc_reset_timeout_count++;
			assert_cdc_reset();

			// CDC timeout repeated; record 0xFAFAFAFA
			cdc_timeout_flag = 1;
			Pstack_state = PSTK_CDC_READ;

		}else{
			// Try one more time
			cdc_reset_timeout_count++;
			assert_cdc_reset();
			delay(MBUS_DELAY*5);
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
    	uint32_t read_data;      // Read data after parasitic cancellation

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
		
		read_data = (read_data_reg4+read_data_reg7-read_data_reg9)/2;

		// Set CPU Halt Option as TX --> Use for register write e.g.
		set_halt_until_mbus_tx();

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0x70, radio_tx_count);
		delay(MBUS_DELAY*20);
		mbus_write_message32(0x70, radio_tx_count);
		delay(MBUS_DELAY*20);
		mbus_write_message32(0x74, read_data_reg7);
		delay(MBUS_DELAY*20);
		mbus_write_message32(0x76, read_data_reg6);
		delay(MBUS_DELAY*10);
		mbus_write_message32(0x70, radio_tx_count);
	#endif

		// Option to take multiple measurements per wakeup

		if (meas_count < 0){	
			meas_count++;

			// Repeat measurement while awake
			release_cdc_meas();
			delay(MBUS_DELAY);
			Pstack_state = PSTK_CDC_RST;
				
		}else{

			meas_count = 0;

			// Finalize CDC operation
			release_cdc_meas();
			assert_cdc_reset();
			Pstack_state = PSTK_IDLE;
			
			// Assert CDC isolation & turn off CDC power
			cdc_power_off();

			#ifdef DEBUG_MBUS_MSG
					mbus_write_message32(0xAA, 0x33333333);
			#endif

			// Check if this is for VBAT measurement
			if (cdc_run_single){
				cdc_run_single = 0;
				#ifdef DEBUG_MBUS_MSG
						mbus_write_message32(0xAA, 0x3333AAAA);
				#endif
				cdc_storage_cref_latest = read_data_reg10;
				return;
			}else{
				exec_count++;
				// Store results in memory; unless buffer is full
				if (cdc_storage_count < CDC_STORAGE_SIZE){
					if (cdc_timeout_flag){
						read_data = 0xFAFAFAFA;
						read_data_reg6 = 0xFAFAFAFA;
					}
					cdc_storage[cdc_storage_count] = read_data;
					cdc_storage_cref[cdc_storage_count] = read_data_reg6;
					cdc_storage_cref_latest = read_data_reg6;
					radio_tx_count = cdc_storage_count;
					cdc_storage_count++;
				}

				// Optionally transmit the data
				if (radio_tx_option){
					send_radio_data_ppm(0, read_data_reg4);
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(0, read_data_reg6);
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(0, read_data_reg7);
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(0, read_data_reg9);
					delay(MBUS_DELAY);
				}

				// Enter long sleep
				if(exec_count < CDC_CYCLE_INIT){
					// Send some signal
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(1, 0xFAF000);
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

//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
    // Reset Wakeup Timer; This is required for PRCv13
    set_wakeup_timer(100, 0, 1);

    // Initialize Interrupts
    // Only enable register-related interrupts
	enable_reg_irq();
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(0xFFFFF); // 0xFFFFF about 13 sec with Y2 run default clock

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
    }

    // Check if wakeup is due to GOC interrupt  
    // 0x78 is reserved for GOC-triggered wakeup (Named IRQ14VEC)
    // 8 MSB bits of the wakeup data are used for function ID
    uint32_t wakeup_data = *((volatile uint32_t *) IRQ14VEC);
    uint8_t wakeup_data_header = wakeup_data>>24;
    uint8_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint8_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint8_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;

    if(wakeup_data_header == 1){
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
        delay(MBUS_DELAY);
        if (exec_count_irq < (wakeup_data_field_0 + (wakeup_data_field_2<<8))){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				// radio
				send_radio_data_ppm(0,0xFAF000+exec_count_irq);	
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

        //set_pmu_sleep_clk_low();
        delay(MBUS_DELAY);

		if (!cdc_running){
			// Go to sleep for initial settling of pressure // FIXME
			set_wakeup_timer(5, 0x1, 0x1); // 150: around 5 min
			cdc_running = 1;
			set_cdc_exec_count = wakeup_data_field_2 >> 5;
			operation_sleep_noirqreset();
		}
		exec_count = 0;
		meas_count = 0;
		cdc_storage_count = 0;
		radio_tx_count = 0;

		// Reset IRQ14VEC
		*((volatile uint32_t *) IRQ14VEC) = 0;

		// Run CDC Program
		cdc_reset_timeout_count = 0;
		operation_cdc_run();

    }else if(wakeup_data_header == 3){
		// Stop CDC program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		cdc_running = 0;
		Pstack_state = PSTK_IDLE;

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				// radio
				send_radio_data_ppm(0,0xFAF000+exec_count);	
    			delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
				send_radio_data_ppm(0,0xFAF000+cdc_reset_timeout_count);	
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
				send_radio_data_ppm(0, 0xFAF000+exec_count_irq);
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
		WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		exec_count = 0;

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Grab latest PMU ADC readings
				// PMUv2 register read is handled differently
				mbus_remote_register_write(PMU_ADDR,0x00,0x03);
				delay(MBUS_DELAY);
				delay(MBUS_DELAY);
				read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;
				batadc_reset();
				delay(MBUS_DELAY);
		
				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				// radio
				send_radio_data_ppm(0,0xFAF000+read_data_batadc);	
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
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
        delay(MBUS_DELAY);

		radv9_r0.RADIO_TUNE_FREQ1 = wakeup_data_field_2>>4; 
		radv9_r0.RADIO_TUNE_FREQ2 = wakeup_data_field_2 & 0xF; 
    	mbus_remote_register_write(RAD_ADDR,0,radv9_r0.as_int);
		delay(MBUS_DELAY);

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
			if (exec_count_irq == 1){
				// Prepare radio TX
				radio_power_on();
				// Go to sleep for SCRO stabilitzation
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1);
				operation_sleep_noirqreset();
			}else{
				// radio
				send_radio_data_ppm(0,0xFAF000+exec_count_irq);	
				// set timer
				set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
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


    // Proceed to continuous mode
    while(1){
        operation_cdc_run();
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

