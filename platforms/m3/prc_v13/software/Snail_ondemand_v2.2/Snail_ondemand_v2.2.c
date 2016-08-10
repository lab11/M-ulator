//*******************************************************************
//Author: Inhee Lee 
//		  
//Description: Sensing System for Snail Project
//
//			Version 2.1
//			- 20nA sleep mode setting for PMU
//			- Can change power for radio
//			- Add Trigger #7 for battery voltage monitor
//
//			Version 2.0
//			- PRCv13, SNSv7, RADv9, PMUv2, HRVv4
//			- Reset HRV counter 
//
//*******************************************************************

#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"
#include "PMUv2_RF.h"

// uncomment this for debug mbus message
// #define DEBUG_MBUS_MSG
// #define DEBUG_MBUS_MSG_1

// TStack order  PRC->RAD->SNS->HRV->PMU
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6
#define PMU_ADDR 0x7

// Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define TEMP_TIMEOUT_COUNT 1000
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 2
#define SENSING_CYCLE_INIT 5 

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_TEMP_LDO   0x1
#define TSTK_TEMP_RSTRL 0x2
#define TSTK_TEMP_START 0x3
#define TSTK_DATA_READ  0x4

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define RADIO_PACKET_DELAY 4000
#define RADIO_TIMEOUT_COUNT 100
#define WAKEUP_PERIOD_RADIO_INIT 2

#define DATA_STORAGE_SIZE 895 // FIXME


//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage

volatile snsv7_r14_t snsv7_r14 = SNSv7_R14_DEFAULT;
volatile snsv7_r15_t snsv7_r15 = SNSv7_R15_DEFAULT;
volatile snsv7_r18_t snsv7_r18 = SNSv7_R18_DEFAULT;
volatile snsv7_r25_t snsv7_r25 = SNSv7_R25_DEFAULT;
  
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

volatile uint32_t enumerated;
volatile uint8_t  Tstack_state;
volatile uint32_t temp_reset_timeout_count;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint8_t  mbus_msg_flag;
volatile uint32_t read_data_lux;
volatile uint32_t read_data_batadc;

volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t data_storage[DATA_STORAGE_SIZE] = {0};
volatile uint32_t data_storage_latest;
volatile uint32_t data_storage_count;
volatile bool	  sensor_running;
volatile uint32_t set_max_exec_count;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile bool radio_ready;
volatile bool radio_on;


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
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (3) 		// Floor frequency base (0-63)
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
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (3) 		// Floor frequency base (0-63)
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
		| (48) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,0x000001);
	delay(MBUS_DELAY);
}


inline static void set_pmu_sleep_clk_high(){
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// The first register write to PMU needs to be repeated
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (1 << 13) // Enable main feedback loop
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
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (6) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY*10);
}

inline static void set_pmu_sleep_clk_default(){
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (3) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// The first register write to PMU needs to be repeated
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (3) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (3) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY*10);
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
	set_pmu_sleep_clk_high();

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
	set_pmu_sleep_clk_default();

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
// Temp Sensor Functions (SNSv7)
//***************************************************

static void temp_sensor_enable(){
    snsv7_r14.TEMP_SENSOR_ENABLEb = 0x0;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_disable(){
    snsv7_r14.TEMP_SENSOR_ENABLEb = 1;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_release_reset(){
    snsv7_r14.TEMP_SENSOR_RESETn = 1;
    snsv7_r14.TEMP_SENSOR_ISO = 0;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_assert_reset(){
    snsv7_r14.TEMP_SENSOR_RESETn = 0;
    snsv7_r14.TEMP_SENSOR_ISO = 1;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void ldo_power_off(){
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB = 1;
    snsv7_r18.ADC_LDO_ADC_LDO_ENB = 1;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
}
static void temp_power_off(){
    snsv7_r14.TEMP_SENSOR_ENABLEb = 1;
    snsv7_r14.TEMP_SENSOR_RESETn = 0;
    snsv7_r14.TEMP_SENSOR_ISO = 1;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
    ldo_power_off();
}

//***************************************************************************************
// HRV function starts here             
//***************************************************************************************
static void snail_cnt_reset(void){
    	mbus_remote_register_write(HRV_ADDR,0x01,
			( (0  << 5)  // HRV_CNT_CNT_IRQ (default: 0)
			| (0  << 4)  // HRV_CNT_CNT_ENABLE (default: 0)
			| (2  << 2)  // LC_CLK_DIV (default: 2)
			| (1)        // LC_CLK_RING (default: 1)
		));
}
	
static void snail_cnt_start(void){
    	mbus_remote_register_write(HRV_ADDR,0x01,
			( (0  << 5)  // HRV_CNT_CNT_IRQ (default: 0)
			| (1  << 4)  // HRV_CNT_CNT_ENABLE (default: 0)
			| (2  << 2)  // LC_CLK_DIV (default: 2)
			| (1)        // LC_CLK_RING (default: 1)
		));
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

//***************************************************
// operation_tx_stored
//***************************************************

static void operation_tx_stored(void){

	while (((!radio_tx_numdata)&&(radio_tx_count>0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > data_storage_count))){

		// Config watchdog timer to about 10 sec; default: 0x02FFFFFF
		config_timerwd(0xFFFFF); // 0xFFFFF about 13 sec with Y2 run default clock
	
    //Fire off stored data to radio
		#ifdef DEBUG_MBUS_MSG_1
			mbus_write_message32(0xDD, radio_tx_count);
			delay(MBUS_DELAY);
			mbus_write_message32(0xDD, data_storage[radio_tx_count]);
			delay(MBUS_DELAY);
		#endif
    	send_radio_data_ppm(0, data_storage[radio_tx_count]);
    	delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
		radio_tx_count--;
	}

    send_radio_data_ppm(0, data_storage[radio_tx_count]);
    delay(RADIO_PACKET_DELAY*3); //Set delays between sending subsequent packet
	send_radio_data_ppm(1, 0xFAF000);
	// This is also the end of this IRQ routine
	exec_count_irq = 0;
	// Go to sleep without timer
	radio_tx_count = data_storage_count; // allows data to be sent more than once
	operation_sleep_notimer();
}

//***************************************************
// initialization
//***************************************************

static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;

  
    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;
	read_data_lux = 0;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
//    set_halt_until_mbus_rx();

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
	//    set_halt_until_mbus_tx();

	// Disable PMUv2 IRQ
    mbus_remote_register_write(PMU_ADDR,0x51,0x09);
	delay(MBUS_DELAY);

	set_pmu_sleep_clk_init();
	

    // Harvester Settings --------------------------------------
    mbus_remote_register_write(HRV_ADDR,0x00, 
		( (15) // HRV_TOP_CONV_RATIO(0~15 >> 9x~23x); default: 14
	));

    mbus_remote_register_write(HRV_ADDR,0x01,
		( (0  << 5)  // HRV_CNT_CNT_IRQ (default: 0)
		| (0  << 4)  // HRV_CNT_CNT_ENABLE (default: 0)
		| (2  << 2)  // LC_CLK_DIV (default: 2)
		| (1)        // LC_CLK_RING (default: 1)
	));

    mbus_remote_register_write(HRV_ADDR,0x03, 
		( (0xFFFFFF) // HRV_CNT_CNT_THRESHOLD (default: 0xFFFFFF) 
	));

    mbus_remote_register_write(HRV_ADDR,0x04,
		( (7  << 6)  // VOLTAGE_CLAMP_I_TUNE (default: 7)
		| (19)       // VOLTAGE_CLAMP_V_TUNE (default: 19)
	));

    mbus_remote_register_write(HRV_ADDR,0x05, 
		( (0x001000) // HRV_CNT_IRQ_PACKET (default: 0x001400) 
	));


	snail_cnt_reset();

    // Temp Sensor Settings --------------------------------------
	// SNSv7_R25
	snsv7_r25.TEMP_SENSOR_IRQ_PACKET = 0x001000;
    mbus_remote_register_write(SNS_ADDR,0x19,snsv7_r25.as_int);
    // SNSv7_R14
	snsv7_r14.TEMP_SENSOR_DELAY_SEL = 3;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
    // snsv7_R15
	snsv7_r15.TEMP_SENSOR_SEL_CT = 7;
    mbus_remote_register_write(SNS_ADDR,0xF,snsv7_r15.as_int);

    // snsv7_R18
    snsv7_r18.ADC_LDO_ADC_LDO_ENB      = 0x1;
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;
    snsv7_r18.ADC_LDO_ADC_CURRENT_2X  = 0x1;

    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv7_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x2;
    snsv7_r18.ADC_LDO_ADC_VREF_SEL     = 0x10;

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

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 33750;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 3;   // 0x1E (30): ~1 min with PRCv9
    data_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    sensor_running = 0;
    radio_ready = 0;
    radio_on = 0;
	set_max_exec_count = 0; // specifies how many sensor executes; 0: unlimited, n: 50*2^n


	// Disable ADC measurement in active mode
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



//*****************************************************************************
// Temperature, battery voltage, light intensity measurement operation (SNSv7)
//*****************************************************************************
static void operation_run(void){
    uint32_t count; 

	if (Tstack_state == TSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG 
			mbus_write_message32(0xAA, 0x0);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_TEMP_LDO;

		mbus_remote_register_read(HRV_ADDR,0x02,1);
		delay(MBUS_DELAY);
		read_data_lux = *((volatile uint32_t *) REG1);
		delay(MBUS_DELAY);
		snail_cnt_reset();

		temp_reset_timeout_count = 0;

		// Power on radio
		if (radio_tx_option || ((exec_count+1) < SENSING_CYCLE_INIT)){
			radio_power_on();
		}

		snsv7_r18.ADC_LDO_ADC_LDO_ENB = 0x0;
		mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);

		// Put system to sleep
		set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
		operation_sleep_noirqreset();

    }else if (Tstack_state == TSTK_TEMP_LDO){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x1);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_TEMP_RSTRL;
		snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
		mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
		// Put system to sleep
		set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
		operation_sleep_noirqreset();

	}else if (Tstack_state == TSTK_TEMP_RSTRL){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x2);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_TEMP_START;

		// Release Temp Sensor Reset
		temp_sensor_release_reset();


	}else if (Tstack_state == TSTK_TEMP_START){
	// Start temp measurement
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xAA, 0x3);
		delay(MBUS_DELAY*10);
	#endif

		mbus_msg_flag = 0;
		temp_sensor_enable();

		for(count=0; count<TEMP_TIMEOUT_COUNT; count++){
			if( mbus_msg_flag ){
				mbus_msg_flag = 0;
				temp_reset_timeout_count = 0;
				Tstack_state = TSTK_DATA_READ;
				return;
			}else{
				delay(MBUS_DELAY);
			}
		}

		// Time out
		mbus_write_message32(0xAA, 0xFAFAFAFA);
		temp_sensor_disable();

		temp_reset_timeout_count++;
		Tstack_state = TSTK_DATA_READ;


	}else if (Tstack_state == TSTK_DATA_READ){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x4);
			delay(MBUS_DELAY*10);
		#endif


		//***************************************************
		// Grab Data
		//***************************************************

		uint32_t read_data_temp_done; // [0] Temp Sensor Done
		uint32_t read_data_temp_data; // [23:0] Temp Sensor D Out

		// PMUv2 register read is handled differently
		mbus_remote_register_write(PMU_ADDR,0x00,0x03);
		delay(MBUS_DELAY);
		read_data_batadc = *((volatile uint32_t *) REG0) & 0xFF;
		delay(MBUS_DELAY);
		batadc_reset();
		delay(MBUS_DELAY);
		
		// Set CPU Halt Option as RX --> Use for register read e.g.
		set_halt_until_mbus_rx();

		mbus_remote_register_read(SNS_ADDR,0x10,1);
		read_data_temp_done = *((volatile uint32_t *) REG1);
		delay(MBUS_DELAY);
		if (temp_reset_timeout_count == 0){
			mbus_remote_register_read(SNS_ADDR,0x11,1);
			read_data_temp_data = *((volatile uint32_t *) REG1);
		}else{
			read_data_temp_data = 0;
			temp_reset_timeout_count = 0;
		}
		delay(MBUS_DELAY);

		// Set CPU Halt Option as TX --> Use for register write e.g.
		set_halt_until_mbus_tx();
		delay(MBUS_DELAY);
		batadc_resetrelease();

		//***************************************************
		// Finalize Sensing Operation 
		//***************************************************
		temp_sensor_disable();
		temp_sensor_assert_reset();	
		temp_power_off();
		Tstack_state = TSTK_IDLE;	


	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCC, 0xCCCCCCCC);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, exec_count);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_temp_done);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_temp_data);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_lux);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_batadc);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, 0xCCCCCCCC);
		delay(MBUS_DELAY);
	#endif

	#ifdef DEBUG_MBUS_MSG_1
		mbus_write_message32(0xCC, 0xCCCCCCCC);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, exec_count);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_temp_done);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_temp_data);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_lux);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_batadc);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, 0xCCCCCCCC);
		delay(MBUS_DELAY);
	#endif

		exec_count++;
		// Store results in memory; unless buffer is full
		if (data_storage_count < DATA_STORAGE_SIZE){
			data_storage[data_storage_count] = read_data_temp_data;
			data_storage_count++;
			data_storage[data_storage_count] = read_data_lux;
			data_storage_count++;
			data_storage[data_storage_count] = read_data_batadc;
			radio_tx_count = data_storage_count;
			data_storage_count++;
			
			data_storage_latest = read_data_lux;
			}

		// Optionally transmit the data
		if (radio_tx_option){
			send_radio_data_ppm(0, read_data_temp_done);
			delay(RADIO_PACKET_DELAY);
			send_radio_data_ppm(0, read_data_temp_data);
			delay(RADIO_PACKET_DELAY);
			send_radio_data_ppm(0, read_data_lux);
			delay(RADIO_PACKET_DELAY);
			send_radio_data_ppm(0, read_data_batadc);
		}

		// Enter long sleep
		if(exec_count < SENSING_CYCLE_INIT){
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

		// Restart HRV counter 
		snail_cnt_start();

		if ((set_max_exec_count != 0) && (exec_count > (50<<set_max_exec_count))){
			// No more measurement required
			// Make sure temp sensor is off
			sensor_running = 0;
			temp_power_off();
			operation_sleep_notimer();
		}else{
			operation_sleep_noirqreset();
		}

    }else{
        // default: // THIS SHOULD NOT HAPPEN
		// Reset Temp Sensor 
		temp_sensor_assert_reset();
		temp_power_off();
		operation_sleep_notimer();
    }

}


//********************************************************************
// MAIN function          
//********************************************************************

int main() {
    
    set_wakeup_timer(100, 0, 1); // Reset Wakeup Timer; 
	enable_reg_irq(); // Initialize Interrupts, Only enable register-related interrupts
    config_timerwd(0x3FFFFF); // Config watchdog timer to about 10 sec (default: 0x02FFFFFF), 0xFFFFF about 13 sec with Y2 run default clock
//	disable_timerwd();


    if (enumerated != 0xDEADBEEF){
        operation_init(); // Initialization
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
        // wakeup_data[7:0] is the LSB of # of transmissions
        // wakeup_data[15:8] is the user-specified period
        // wakeup_data[23:16] is the MSB of # of transmissions
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
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
				send_radio_data_ppm(0,0xABC000+exec_count_irq);	
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
		// Slow down PMU sleep osc and run temp sensor code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:16] is the initial user-specified period
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[23:21] specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2 & 0xF;
        radio_tx_option = wakeup_data_field_2 & 0x10;

        //set_pmu_sleep_clk_low();

		if (!sensor_running){
			// Go to sleep for initial settling of sensing // FIXME
			set_wakeup_timer(5, 0x1, 0x1); // 150: around 5 min
			sensor_running = 1;
			set_max_exec_count = wakeup_data_field_2 >> 5;
			operation_sleep_noirqreset();
		}
		exec_count = 0;
		meas_count = 0;
		data_storage_count = 0;
		radio_tx_count = 0;

		// Reset IRQ14VEC
		*((volatile uint32_t *) IRQ14VEC) = 0;

		// Run Temp Sensor Program
		temp_reset_timeout_count = 0;
		operation_run();

    }else if(wakeup_data_header == 3){
		// Stop temp sensor program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		sensor_running = 0;
		Tstack_state = TSTK_IDLE;

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
				send_radio_data_ppm(0,0xC10000+exec_count);	
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
		disable_timerwd();
        // Transmit the stored temp sensor data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		radio_tx_numdata = wakeup_data_field_0;
		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= data_storage_count){
			radio_tx_numdata = 0;
		}
		
        if (exec_count_irq < 5){
			exec_count_irq++;
			if (exec_count_irq == 1){
				radio_power_on(); // Prepare radio TX
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x1); // Go to sleep for SCRO stabilitzation
				operation_sleep_noirqreset();
			}else{
				send_radio_data_ppm(0, 0xABC000+exec_count_irq);
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1); // set timer	
				operation_sleep_noirqreset(); // go to sleep and wake up with same condition
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
				send_radio_data_ppm(0,0xBBB000+read_data_batadc);	
				// set timer
				set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
				// go to sleep and wake up with same condition
				operation_sleep_noirqreset();
			}
        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data_ppm(1,0xFAF000);	
			// Release reset of PMU ADC
			batadc_resetrelease();
            // Go to sleep without timer
            operation_sleep_notimer();
        }


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
				send_radio_data_ppm(0,0xABC000+exec_count_irq);	
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
        operation_run();
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}


