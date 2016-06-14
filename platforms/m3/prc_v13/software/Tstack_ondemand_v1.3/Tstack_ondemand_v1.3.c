//*******************************************************************
//Author: Inhee Lee 
//		  Gyouho Kim
//Description: Temperature Sensing System
//			PRCv13, SNSv7, RADv9, PMUv2
//			Modified from 'Pstack_Ondemand_v2.0'
//			v1.1: Changing chip order
//			v1.2: Increasing PMU floors for long term stability
//			v1.3: PMU IRQ off for stability issues
//				  RADv9 settings updated for shifted center freq and higher curr lim
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
 #define DEBUG_MBUS_MSG_1

// TStack order  PRC->RAD->SNS->HRV->PMU
#define HRV_ADDR 0x3
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define TEMP_TIMEOUT_COUNT 2000
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 2
#define TEMP_CYCLE_INIT 5 

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_RSTRL 0x3
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define RADIO_PACKET_DELAY 3000
#define RADIO_TIMEOUT_COUNT 50
#define WAKEUP_PERIOD_RADIO_INIT 2

#define TEMP_STORAGE_SIZE 1000 // FIXME


//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint8_t Tstack_state;
volatile uint32_t temp_reset_timeout_count;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint8_t mbus_msg_flag;

volatile snsv7_r14_t snsv7_r14 = SNSv7_R14_DEFAULT;
volatile snsv7_r15_t snsv7_r15 = SNSv7_R15_DEFAULT;
volatile snsv7_r18_t snsv7_r18 = SNSv7_R18_DEFAULT;
volatile snsv7_r25_t snsv7_r25 = SNSv7_R25_DEFAULT;
  
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t temp_storage[TEMP_STORAGE_SIZE] = {0};
volatile uint32_t temp_storage_latest;
volatile uint32_t temp_storage_count;
volatile bool temp_run_single;
volatile bool temp_running;
volatile uint32_t set_temp_exec_count;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile bool radio_ready;
volatile bool radio_on;

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

inline static void set_pmu_sleep_clk_default(){
}

inline static void set_pmu_sleep_clk_low(){
}

inline static void set_pmu_sleep_clk_high(){
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
    // Release SCRO Reset
    radv9_r2.SCRO_RESET = 0;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);
    
    // Additional delay required after SCRO Reset release
    delay(MBUS_DELAY*3); // At least 20ms required
    
    // Enable SCRO
    radv9_r2.SCRO_ENABLE = 1;
    mbus_remote_register_write(RAD_ADDR,2,radv9_r2.as_int);

	// Release FSM Isolate
	radv9_r13.RAD_FSM_ISOLATE = 0;
	mbus_remote_register_write(RAD_ADDR,13,radv9_r13.as_int);

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
    }

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
#ifdef DEBUG_MBUS_MSG_1
    mbus_write_message32(0xDD, radio_tx_count);
	delay(MBUS_DELAY);
    mbus_write_message32(0xDD, temp_storage[radio_tx_count]);
	delay(MBUS_DELAY);
#endif
    send_radio_data_ppm(0, temp_storage[radio_tx_count]);
    delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet

    if (((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > temp_storage_count))){
		radio_tx_count--;
		// set timer
		set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();

    }else{
		delay(RADIO_PACKET_DELAY*3); //Set delays between sending subsequent packet
		send_radio_data_ppm(1, 0xFAF000);
		#ifdef DEBUG_MBUS_MSG_1
    		mbus_write_message32(0xDD, radio_tx_count);
			delay(MBUS_DELAY);
    		mbus_write_message32(0xDD, 0xFAFA0000);
			delay(MBUS_DELAY);
		#endif
		// This is also the end of this IRQ routine
		exec_count_irq = 0;
		// Go to sleep without timer
		radio_tx_count = temp_storage_count; // allows data to be sent more than once
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
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;
  
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
    temp_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0; //enables radio tx for each measurement 
    temp_run_single = 0;
    temp_running = 0;
    radio_ready = 0;
    radio_on = 0;
	set_temp_exec_count = 0; // specifies how many temp sensor executes; 0: unlimited, n: 50*2^n

    // Harvester Settings --------------------------------------
    hrvv2_r0.HRV_TOP_CONV_RATIO = 0x6;
    mbus_remote_register_write(HRV_ADDR,0,hrvv2_r0.as_int);

    // Go to sleep without timer
    operation_sleep_notimer();
}



//***************************************************
// Temperature measurement operation (SNSv7)
//***************************************************
static void operation_temp_run(void){
    uint32_t count; 

	if (Tstack_state == TSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG 
			mbus_write_message32(0xBB, 0xFBFB0000);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_LDO;

		temp_reset_timeout_count = 0;

		// Power on radio
//		if (radio_tx_option || ((exec_count+1) < TEMP_CYCLE_INIT)){
//			radio_power_on();
//		}

		snsv7_r18.ADC_LDO_ADC_LDO_ENB = 0x0;
		mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);

		// Put system to sleep
		set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
		operation_sleep_noirqreset();

    }else if (Tstack_state == TSTK_LDO){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xBB, 0xFBFB1111);
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
			mbus_write_message32(0xBB, 0xFBFB2222);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_TEMP_START;

		// Release Temp Sensor Reset
		temp_sensor_release_reset();

	}else if (Tstack_state == TSTK_TEMP_START){
	// Start temp measurement
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xBB, 0xFBFB3333);
		delay(MBUS_DELAY*10);
	#endif

		mbus_msg_flag = 0;
		temp_sensor_enable();

		if (temp_run_single){
			Tstack_state = TSTK_TEMP_READ;
			set_wakeup_timer(WAKEUP_PERIOD_RESET, 0x1, 0x1);
			operation_sleep_noirqreset();
		}

		for(count=0; count<TEMP_TIMEOUT_COUNT; count++){
			if( mbus_msg_flag ){
				mbus_msg_flag = 0;
				temp_reset_timeout_count = 0;
				Tstack_state = TSTK_TEMP_READ;
				return;
			}else{
				delay(MBUS_DELAY);
			}
		}

		// Time out
		mbus_write_message32(0xFA, 0xFAFAFAFA);
		temp_sensor_disable();

		if (temp_reset_timeout_count > 0){
			temp_reset_timeout_count++;
			temp_sensor_assert_reset();

			if (temp_reset_timeout_count > 5){
				// Temp sensor is not resetting for some reason. Go to sleep forever
				Tstack_state = TSTK_IDLE;
				temp_power_off();
				operation_sleep_notimer();
			}else{
				// Put system to sleep to reset the layer controller
				Tstack_state = TSTK_TEMP_RSTRL;
				set_wakeup_timer (WAKEUP_PERIOD_RESET, 0x1, 0x0);
				operation_sleep();
			}

	    }else{
		// Try one more time
	    	temp_reset_timeout_count++;
			temp_sensor_assert_reset();
	    }

	}else if (Tstack_state == TSTK_TEMP_READ){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xBB, 0xFBFB4444);
			delay(MBUS_DELAY*10);
		#endif

		// Grab Temp Sensor Data
		uint32_t read_data_reg10; // [0] Temp Sensor Done
		uint32_t read_data_reg11; // [23:0] Temp Sensor D Out

		// Set CPU Halt Option as RX --> Use for register read e.g.
		set_halt_until_mbus_rx();

		mbus_remote_register_read(SNS_ADDR,0x10,1);
		read_data_reg10 = *((volatile uint32_t *) 0xA0000004);
		delay(MBUS_DELAY);
		mbus_remote_register_read(SNS_ADDR,0x11,1);
		read_data_reg11 = *((volatile uint32_t *) 0xA0000004);
		delay(MBUS_DELAY);

		// Set CPU Halt Option as TX --> Use for register write e.g.
		set_halt_until_mbus_tx();

	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xCC, 0xCCCCCCCC);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, exec_count);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_reg10);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_reg11);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, exec_count);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, 0xCCCCCCCC);
		delay(MBUS_DELAY);
	#endif

	#ifdef DEBUG_MBUS_MSG_1
		mbus_write_message32(0xCC, exec_count);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, set_temp_exec_count);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_reg11);
		delay(MBUS_DELAY);
		mbus_write_message32(0xCC, read_data_reg10);
		delay(MBUS_DELAY);
	#endif

		// Option to take multiple measurements per wakeup

		if (meas_count < 0){	
			meas_count++;

			// Repeat measurement while awake
			temp_sensor_disable();
			Tstack_state = TSTK_TEMP_START;
				
		}else{

			meas_count = 0;

			// Finalize temp sensor operation
			temp_sensor_disable();
			temp_sensor_assert_reset();
			Tstack_state = TSTK_IDLE;
			
			// Assert temp sensor isolation & turn off temp sensor power
			temp_power_off();


			if (temp_run_single){
				temp_run_single = 0;
				temp_storage_latest = read_data_reg11;
				return;
			}else{
				exec_count++;
				// Store results in memory; unless buffer is full
				if (temp_storage_count < TEMP_STORAGE_SIZE){
					temp_storage[temp_storage_count] = read_data_reg11;
					temp_storage_latest = read_data_reg11;
					radio_tx_count = temp_storage_count;
					temp_storage_count++;
				}

				// Optionally transmit the data
				if (radio_tx_option){
					send_radio_data_ppm(0, read_data_reg10);
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(0, read_data_reg11);
				}

				// Enter long sleep
				if(exec_count < TEMP_CYCLE_INIT){
					// Send some signal
			//		delay(RADIO_PACKET_DELAY);
			//		send_radio_data_ppm(1, 0xFAF000);
					set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);

				}else{
					set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
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
					temp_power_off();
					operation_sleep_notimer();
				}else{
					operation_sleep_noirqreset();
					
				}

			}
		}

    }else{
        //default:  // THIS SHOULD NOT HAPPEN
		// Reset Temp Sensor 
		temp_sensor_assert_reset();
		temp_power_off();
		operation_sleep_notimer();
    }

}


//********************************************************************
// MAIN function starts here             
//********************************************************************

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
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA,0xFAFA1111);
			delay(MBUS_DELAY*10);
		#endif
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
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
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA,0xFAFA2222);
			delay(MBUS_DELAY*10);
		#endif
		// Slow down PMU sleep osc and run temp sensor code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:16] is the initial user-specified period
        // wakeup_data[20] enables radio tx for each measurement
        // wakeup_data[23:21] specifies how many temp sensor executes; 0: unlimited, n: 50*2^n
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2 & 0xF;
        radio_tx_option = wakeup_data_field_2 & 0x10;

		temp_run_single = 0;
        set_pmu_sleep_clk_low();

		if (!temp_running){
			// Go to sleep for initial settling of temp sensing // FIXME
			set_wakeup_timer(5, 0x1, 0x1); // 150: around 5 min
			temp_running = 1;
			set_temp_exec_count = wakeup_data_field_2 >> 5;
			operation_sleep_noirqreset();
		}
		exec_count = 0;
		meas_count = 0;
		temp_storage_count = 0;
		radio_tx_count = 0;

		// Reset IRQ14VEC
		*((volatile uint32_t *) IRQ14VEC) = 0;

		// Run Temp Sensor Program
		temp_reset_timeout_count = 0;
		operation_temp_run();

    }else if(wakeup_data_header == 3){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA,0xFAFA3333);
			delay(MBUS_DELAY*10);
		#endif
		// Stop temp sensor program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        // wakeup_data[16] indicates whether or not to speed up PMU sleep clock
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		temp_running = 0;
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
				send_radio_data_ppm(0,0xFAF000+exec_count);	
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
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA,0xFAFA4444);
			delay(MBUS_DELAY*10);
		#endif
        // Transmit the stored temp sensor data
        // wakeup_data[7:0] is the # of data to transmit; if zero, all stored data is sent
        // wakeup_data[15:8] is the user-specified period 
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		radio_tx_numdata = wakeup_data_field_0;
		// Make sure the requested numdata makes sense
		if (radio_tx_numdata >= temp_storage_count){
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
		
    }else if(wakeup_data_header == 0x11){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA,0xFAFA0011);
			delay(MBUS_DELAY*10);
		#endif
		// Slow down PMU sleep osc and go to sleep for further programming
        set_pmu_sleep_clk_low();

        // Go to sleep without timer
        operation_sleep_notimer();

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
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x0);
				operation_sleep_noirqreset();
			}else{
				// radio
				send_radio_data_ppm(0,0xFAF000+exec_count_irq);	
				// set timer
				set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
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
        operation_temp_run();
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}


