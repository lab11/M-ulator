//****************************************************************************************************
//Author:       Gyouho Kim
//              ZhiYoong Foo
//Description:  
//              Mouse Implantation & CDC Measurement Code
//				Revision 1.9
//				- PRCv11
//				- SNSv6
//				- Support for RADv7 & RADv8 (ppm radio)
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
#include "mbus.h"
#include "PRCv9.h"
#include "SNSv6.h"
#include "HRVv2.h"
#include "RADv8.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->RAD->SNS->HRV
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6

// CDC parameters
#define	MBUS_DELAY 50 //Amount of delay between successive messages; 100: 6-7ms
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 1
#define CDC_CYCLE_INIT 4

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_CDC_RST    0x1
#define PSTK_CDC_READ   0x3
#define PSTK_LDO1   	0x4
#define PSTK_LDO2  	 	0x5

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define RADIO_PACKET_DELAY 2000
#define RADIO_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RADIO_INIT 3

// CDC configurations
#define NUM_SAMPLES         3      //Number of CDC samples to take (only 2^n allowed for averaging: 2, 4, 8, 16...)
#define NUM_SAMPLES_TX      1      //Number of CDC samples to be TXed (processed by process_data)
#define NUM_SAMPLES_2PWR    0      //NUM_SAMPLES = 2^NUM_SAMPLES_2PWR - used for averaging

#define CDC_STORAGE_SIZE 80 

//***************************************************
// Global variables
//***************************************************
	// "static" limits the variables to this file, giving compiler more freedom
	// "volatile" should only be used for MMIO --> ensures memory storage
	volatile uint32_t enumerated;
	volatile uint8_t Pstack_state;
	volatile uint32_t cdc_data[NUM_SAMPLES];
	volatile uint32_t cdc_data_tx[NUM_SAMPLES_TX];
	volatile uint32_t cdc_reset_timeout_count;
	volatile uint32_t exec_count;
	volatile uint32_t meas_count;
	volatile uint32_t exec_count_irq;
	volatile uint8_t MBus_msg_flag;
	volatile snsv6_r0_t snsv6_r0 = SNSv6_R0_DEFAULT;
	volatile snsv6_r1_t snsv6_r1 = SNSv6_R1_DEFAULT;
	volatile snsv6_r2_t snsv6_r2 = SNSv6_R2_DEFAULT;
	volatile snsv6_r17_t snsv6_r17 = SNSv6_R17_DEFAULT;
  
	volatile uint32_t WAKEUP_PERIOD_CONT; 
	volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

	volatile uint32_t cdc_storage[CDC_STORAGE_SIZE] = {0};
	volatile uint32_t cdc_storage_cref[CDC_STORAGE_SIZE] = {0};
	volatile uint32_t cdc_storage_cref_latest;
	volatile uint32_t cdc_storage_count;
	volatile uint8_t cdc_run_single;
	volatile uint8_t cdc_running;

	volatile uint32_t radio_tx_count;
	volatile uint32_t radio_tx_option;
	volatile uint32_t radio_tx_numdata;
	volatile bool radio_ready;
	volatile bool radio_on;

	volatile radv8_r0_t radv8_r0 = RADv8_R0_DEFAULT;
	volatile radv8_r1_t radv8_r1 = RADv8_R1_DEFAULT;
	volatile radv8_r2_t radv8_r2 = RADv8_R2_DEFAULT;
	volatile radv8_r3_t radv8_r3 = RADv8_R3_DEFAULT;
	volatile radv8_r4_t radv8_r4 = RADv8_R4_DEFAULT;
	volatile radv8_r5_t radv8_r5 = RADv8_R5_DEFAULT;
	volatile radv8_r6_t radv8_r6 = RADv8_R6_DEFAULT;
	volatile radv8_r7_t radv8_r7 = RADv8_R7_DEFAULT;
	volatile radv8_r8_t radv8_r8 = RADv8_R8_DEFAULT;
	volatile radv8_r9_t radv8_r9 = RADv8_R9_DEFAULT;

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_0(void){
    *((volatile uint32_t *) 0xE000E280) = 0x1;
    MBus_msg_flag = 0x10;
}
void handler_ext_int_1(void){
    *((volatile uint32_t *) 0xE000E280) = 0x2;
    MBus_msg_flag = 0x11;
}
void handler_ext_int_2(void){
    *((volatile uint32_t *) 0xE000E280) = 0x4;
    MBus_msg_flag = 0x12;
}
void handler_ext_int_3(void){
    *((volatile uint32_t *) 0xE000E280) = 0x8;
    MBus_msg_flag = 0x13;
}


//***************************************************
// Radio transmission routines for PPM Radio (RADv7 & v8)
//***************************************************

static void radio_power_on(){
	// Release FSM Sleep - Requires >2s stabilization time
	radio_on = 1;
	radv8_r8.RAD_FSM_SLEEP = 0;
	write_mbus_register(RAD_ADDR,8,radv8_r8.as_int);
	delay(MBUS_DELAY);
}

static void radio_power_off(){
	// Turn off everything
	radio_on = 0;
	radv8_r2.SCRO_ENABLE = 0;
	radv8_r2.SCRO_RESET  = 1;
	write_mbus_register(RAD_ADDR,2,radv8_r2.as_int);
	delay(MBUS_DELAY);
	radv8_r8.RAD_FSM_SLEEP 		= 1;
	radv8_r8.RAD_FSM_ISOLATE 	= 1;
	radv8_r8.RAD_FSM_RESETn 	= 0;
	radv8_r8.RAD_FSM_ENABLE 	= 0;
	write_mbus_register(RAD_ADDR,8,radv8_r8.as_int);
	delay(MBUS_DELAY);
}

static void send_radio_data_ppm(bool last_packet, uint32_t radio_data){
	// Write Data: Only up to 24bit data for now
	radv8_r3.RAD_FSM_DATA = radio_data;
	write_mbus_register(RAD_ADDR,3,radv8_r3.as_int);
	delay(MBUS_DELAY);

	if (!radio_ready){
		radio_ready = 1;

		// Release SCRO Reset
		radv8_r2.SCRO_RESET = 0;
		write_mbus_register(RAD_ADDR,2,radv8_r2.as_int);
		delay(MBUS_DELAY);

		// Additional delay required after SCRO Reset release
		delay(400); // At least 20ms required

		// Enable SCRO
		radv8_r2.SCRO_ENABLE = 1;
		write_mbus_register(RAD_ADDR,2,radv8_r2.as_int);
		delay(MBUS_DELAY);

		// Release FSM Isolate
		radv8_r8.RAD_FSM_ISOLATE = 0;
		write_mbus_register(RAD_ADDR,8,radv8_r8.as_int);
		delay(MBUS_DELAY);

		// Release FSM Reset
		radv8_r8.RAD_FSM_RESETn = 1;
		write_mbus_register(RAD_ADDR,8,radv8_r8.as_int);
		delay(MBUS_DELAY);

	}

	// Fire off data
	uint32_t count;
	MBus_msg_flag = 0;
	radv8_r8.RAD_FSM_ENABLE = 1;
	write_mbus_register(RAD_ADDR,8,radv8_r8.as_int);
	delay(MBUS_DELAY);

	for( count=0; count<RADIO_TIMEOUT_COUNT; count++ ){
		if( MBus_msg_flag ){
			MBus_msg_flag = 0;
			if (last_packet){
				radio_ready = 0;
				radio_power_off();
			}else{
				radv8_r8.RAD_FSM_ENABLE = 0;
				write_mbus_register(RAD_ADDR,8,radv8_r8.as_int);
				delay(MBUS_DELAY);
			}
			return;
		}else{
			delay(MBUS_DELAY);
		}
	}
	
	// Timeout
	#ifdef DEBUG_MBUS_MSG
		write_mbus_message(0xFF, 0xFAFAFAFA);
	#endif
}


//***************************************************
// CDCW Functions
// snsv6
//***************************************************
static void release_cdc_pg(){
    snsv6_r0.CDCW_PG_V1P2 = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
    snsv6_r0.CDCW_PG_VBAT = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
    snsv6_r0.CDCW_PG_VLDO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_isolate(){
    snsv6_r0.CDCW_ISO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
}
static void assert_cdc_reset(){
    snsv6_r0.CDCW_RESETn = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_reset(){
    snsv6_r0.CDCW_RESETn = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
}
static void fire_cdc_meas(){
    snsv6_r0.CDCW_ENABLE = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_meas(){
    snsv6_r0.CDCW_ENABLE = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
}
static void ldo_power_off(){
	snsv6_r17.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
	snsv6_r17.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
	snsv6_r17.CDC_LDO_CDC_LDO_ENB = 0x1;
	snsv6_r17.ADC_LDO_ADC_LDO_ENB = 0x1;
	write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);
    delay(MBUS_DELAY);
}
static void cdc_power_off(){
    snsv6_r0.CDCW_ISO = 0x1;
    snsv6_r0.CDCW_PG_V1P2 = 0x1;
    snsv6_r0.CDCW_PG_VBAT = 0x1;
    snsv6_r0.CDCW_PG_VLDO = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);
	ldo_power_off();
}


inline static void set_pmu_sleep_clk_low(){
    // PRCv11 Default: 0x8F770049
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77184B; // 0x8F77003B: use GOC x0.6-2, 0x8F77184B: GOC x10-45
    *((volatile uint32_t *) 0xA200000C) = 0x8F77194B; // Sleep current 3nA
    *((volatile uint32_t *) 0xA200000C) = 0x8F7718CB; // Sleep current 5nA
}
inline static void set_pmu_sleep_clk_default(){
    // PRCv11 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77184B; // 0x8F77184B: Sleep current 8nA, use GOC x9-18
}
static void process_data(){
    uint8_t i;
    uint8_t j;
    uint32_t temp;
    // bubble sort
    for( i=0; i<NUM_SAMPLES; ++i ){
        for( j=i+1; j<NUM_SAMPLES; ++j){
            if( cdc_data[j] > cdc_data[i] ){
                temp = cdc_data[i];
                cdc_data[i] = cdc_data[j];
                cdc_data[j] = temp;
            }
        }
    }

    // Assuming NUM_SAMPLES = 3, NUM_SAMPLES_TX = 1
    cdc_data_tx[0] = cdc_data[1];

}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  //set_wakeup_timer (0xFFF, 0x0, 0x1);
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;

  // Go to Sleep
  sleep();
  while(1);

}

static void operation_sleep_noirqreset(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Go to Sleep
  sleep();
  while(1);

}

static void operation_sleep_notimer(void){
    
	// Make sure LDO is off
	ldo_power_off();
	
	// Make sure Radio is off
	if (radio_on){
		radio_power_off();
	}

	// Disable Timer
	set_wakeup_timer (0, 0x0, 0x0);
	// Go to sleep without timer
	operation_sleep();

}

static void operation_tx_stored(void){

	//Fire off stored data to radio
	#ifdef DEBUG_MBUS_MSG
		delay(MBUS_DELAY*10);
		write_mbus_message(0x70, radio_tx_count);
		delay(MBUS_DELAY*10);
		write_mbus_message(0x74, cdc_storage[radio_tx_count]);
		delay(MBUS_DELAY*10);
		write_mbus_message(0x76, cdc_storage_cref[radio_tx_count]);
		delay(MBUS_DELAY*10);
		write_mbus_message(0x70, radio_tx_count);
		delay(MBUS_DELAY*10);
	#endif
	send_radio_data_ppm(0, cdc_storage[radio_tx_count]);
	delay(RADIO_PACKET_DELAY); //Set delays between sending subsequent packet
	send_radio_data_ppm(0, cdc_storage_cref[radio_tx_count]);

	if (((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > cdc_storage_count))){
		radio_tx_count--;
		// set timer
		set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();

	}else{
		delay(RADIO_PACKET_DELAY*3); //Set delays between sending subsequent packet
		send_radio_data_ppm(1, 0xFAF000);

		// This is also the end of this IRQ routine
		exec_count_irq = 0;
		// Go to sleep without timer
		radio_tx_count = cdc_storage_count; // allows data to be sent more than once
		operation_sleep_notimer();
	}

}

static void operation_init(void){
  
    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // PRCv11 Default: 0x8F770049
    // Decrease 5x division switching threshold
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77004B;
	// Increase active pmu clock (for PRCv11)
    *((volatile uint32_t *) 0xA200000C) = 0x8F77184B;
  
    // Speed up GOC frontend to match PMU frequency
    // PRCv11 Default: 0x00202903
    //*((volatile uint32_t *) 0xA2000008) = 0x00202908;
	// Slow down MBUS frequency 
	// Gyouho: This is required for running on the board w/o PMU assist (for PRCv11)
    *((volatile uint32_t *) 0xA2000008) = 0x00202D08;
  
    delay(MBUS_DELAY*20);
  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;

    //Enumeration
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY*10);

	//write_mbus_message(0x70, radio_tx_count);
	//operation_sleep_notimer();

    enumerate(SNS_ADDR);
    delay(MBUS_DELAY*10);
    enumerate(HRV_ADDR);
    delay(MBUS_DELAY*10);

    // CDC Settings --------------------------------------
    // snsv6_r0
    snsv6_r0.CDCW_IRQ_EN	= 1;
    snsv6_r0.CDCW_MODE_PAR	= 0;
    snsv6_r0.CDCW_RESETn 	= 0;
    write_mbus_register(SNS_ADDR,0,snsv6_r0.as_int);
    delay(MBUS_DELAY);

    // snsv6_r1
    snsv6_r1.CDCW_N_CYCLE_SINGLE	= 0; // Default: 8; Min: 0
    write_mbus_register(SNS_ADDR,1,snsv6_r1.as_int);
    delay(MBUS_DELAY);
	
    // snsv6_r2
    snsv6_r2.CDCW_N_CYCLE_SET	= 100; // Min: 0
    write_mbus_register(SNS_ADDR,2,snsv6_r2.as_int);
    delay(MBUS_DELAY);

    // snsv6_r17
    snsv6_r17.CDC_LDO_CDC_CURRENT_2X  = 0x0;

    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv6_r17.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv6_r17.ADC_LDO_ADC_VREF_SEL     = 0x20;

    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv6_r17.CDC_LDO_CDC_VREF_MUX_SEL = 0x0;
    snsv6_r17.CDC_LDO_CDC_VREF_SEL     = 0x20;

    write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);
    delay(MBUS_DELAY);

    // Radio Settings --------------------------------------

    radv8_r0.RADIO_TUNE_CURRENT_LIMITER = 0x1F; //Current Limiter 2F = 30uA, 1F = 3uA
    radv8_r0.RADIO_TUNE_FREQ1 = 0x0; //Tune Freq 1
    radv8_r0.RADIO_TUNE_FREQ2 = 0x0; //Tune Freq 2 //0x0,0x0 = 902MHz on Pblr005
    radv8_r0.RADIO_TUNE_TX_TIME = 0x6; //Tune TX Time

    write_mbus_register(RAD_ADDR,0,radv8_r0.as_int);
    delay(MBUS_DELAY);

	// FSM data length setups
	radv8_r6.RAD_FSM_H_LEN = 16; // N
	radv8_r6.RAD_FSM_D_LEN = RADIO_DATA_LENGTH-1; // N-1
	radv8_r6.RAD_FSM_C_LEN = 10;
    write_mbus_register(RAD_ADDR,6,radv8_r6.as_int);
    delay(MBUS_DELAY);

	// Configure SCRO
	radv8_r1.SCRO_FREQ_DIV = 2;
	radv8_r1.SCRO_AMP_I_LEVEL_SEL = 2; // Default 2
	radv8_r1.SCRO_I_LEVEL_SELB = 0x60; // Default 0x6F
    write_mbus_register(RAD_ADDR,1,radv8_r1.as_int);
    delay(MBUS_DELAY);

	// LFSR Seed
	radv8_r7.RAD_FSM_SEED = 4;
    write_mbus_register(RAD_ADDR,7,radv8_r7.as_int);
    delay(MBUS_DELAY);
  
    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
	cdc_run_single = 0;
	cdc_running = 0;
	radio_ready = 0;
	radio_on = 0;
    
    // Go to sleep without timer
    operation_sleep_notimer();
}

static void operation_cdc_run(){
    uint32_t read_data;
    uint32_t count; 

	if (Pstack_state == PSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x0);
		#endif
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;

		// Power on radio
		if (radio_tx_option || ((exec_count+1) < CDC_CYCLE_INIT)){
			radio_power_on();
		}

		snsv6_r17.CDC_LDO_CDC_LDO_ENB = 0x0;
		write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);
		delay(MBUS_DELAY);
		// Long delay required here
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();

	}else if (Pstack_state == PSTK_LDO1){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x1);
		#endif
		Pstack_state = PSTK_LDO2;
		snsv6_r17.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
		write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);
		delay(MBUS_DELAY);
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();
    
	}else if (Pstack_state == PSTK_LDO2){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x2);
		#endif
		Pstack_state = PSTK_CDC_RST;

		// Release CDC isolation
		release_cdc_pg();
		delay(MBUS_DELAY*2);
		release_cdc_isolate();

	}else if (Pstack_state == PSTK_CDC_RST){
		// Release reset
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x11111111);
			delay(MBUS_DELAY);
		#endif

		MBus_msg_flag = 0;
		release_cdc_reset();
		delay(MBUS_DELAY*2);
		fire_cdc_meas();

		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
			if( MBus_msg_flag ){
				MBus_msg_flag = 0;
				cdc_reset_timeout_count = 0;
				Pstack_state = PSTK_CDC_READ;
				return;
			}else{
				delay(MBUS_DELAY);
			}
		}

		// Time out
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0xFAFAFAFA);
		#endif

		release_cdc_meas();
		if (cdc_reset_timeout_count > 0){
			cdc_reset_timeout_count++;
			assert_cdc_reset();

			if (cdc_reset_timeout_count > 5){
				// CDC is not resetting for some reason. Go to sleep forever
				Pstack_state = PSTK_IDLE;
				cdc_power_off();
				operation_sleep_notimer();
			}else{
				// Put system to sleep to reset the layer controller
				set_wakeup_timer (WAKEUP_PERIOD_RESET, 0x1, 0x0);
				operation_sleep();
			}

	    }else{
		// Try one more time
	    	cdc_reset_timeout_count++;
			assert_cdc_reset();
			delay(MBUS_DELAY*5);
	    }

	}else if (Pstack_state == PSTK_CDC_READ){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x22222222);
		#endif

		// Grab CDC Data
    	uint32_t read_data_reg4;
    	uint32_t read_data_reg6;
		// CDCW_OUT0[23:0]
		read_mbus_register(SNS_ADDR,4,0x15);
		delay(MBUS_DELAY);
		read_data_reg4 = *((volatile uint32_t *) 0xA0001014);
		// CDCW_OUT1[23:0]
		read_mbus_register(SNS_ADDR,6,0x15);
		delay(MBUS_DELAY);
		read_data_reg6 = *((volatile uint32_t *) 0xA0001014);

		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0x70, radio_tx_count);
			delay(MBUS_DELAY*20);
			write_mbus_message(0x70, radio_tx_count);
			delay(MBUS_DELAY*20);
			write_mbus_message(0x74, read_data_reg4);
			delay(MBUS_DELAY*20);
			write_mbus_message(0x76, read_data_reg6);
			delay(MBUS_DELAY*10);
			write_mbus_message(0x70, radio_tx_count);
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
			#ifdef DEBUG_MBUS_MSG
				write_mbus_message(0xAA, 0x33333333);
			#endif
		
			// Assert CDC isolation & turn off CDC power
			cdc_power_off();

			// Check if this is for VBAT measurement
			if (cdc_run_single){
				cdc_run_single = 0;
				cdc_storage_cref_latest = read_data_reg6;
				return;
			}else{
				exec_count++;
				// Store results in memory; unless buffer is full
				if (cdc_storage_count < CDC_STORAGE_SIZE){
					cdc_storage[cdc_storage_count] = read_data_reg4;
					cdc_storage_cref[cdc_storage_count] = read_data_reg6;
					cdc_storage_cref_latest = read_data_reg6;
					radio_tx_count = cdc_storage_count;
					cdc_storage_count++;
				}

				// Optionally transmit the data
				if (radio_tx_option){
					send_radio_data_ppm(0, read_data_reg4);
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(1, read_data_reg6);
					delay(MBUS_DELAY);
				}

				// Enter long sleep
				if(exec_count < CDC_CYCLE_INIT){
					// Send some signal
					send_radio_data_ppm(1, 0xFAF000);
					set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);

				}else{
					set_wakeup_timer (WAKEUP_PERIOD_CONT, 0x1, 0x0);
				}

				// Make sure Radio is off
				if (radio_on){
					radio_power_off();
				}
				operation_sleep();
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
  
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0xF;
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0xF;
  
	//Config watchdog timer to about 10 sec: 1,000,000 with default PRCv9
	//config_timer( timer_id, go, roi, init_val, sat_val )
	config_timer( 0, 1, 0, 0, 1000000 );

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
    }

    // Check if wakeup is due to GOC interrupt  
    // 0x68 is reserved for GOC-triggered wakeup (Named IRQ10VEC)
    // 8 MSB bits of the wakeup data are used for function ID
    uint32_t wakeup_data = *((volatile uint32_t *) IRQ10VEC);
    uint8_t wakeup_data_header = wakeup_data>>24;
    uint8_t wakeup_data_field_0 = wakeup_data & 0xFF;
    uint8_t wakeup_data_field_1 = wakeup_data>>8 & 0xFF;
    uint8_t wakeup_data_field_2 = wakeup_data>>16 & 0xFF;

    if(wakeup_data_header == 1){
        // Debug mode: Transmit something via radio and go to sleep w/o timer
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
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
   
    }else if(wakeup_data_header == 2){
		// Slow down PMU sleep osc and run CDC code with desired wakeup period
        // wakeup_data[15:0] is the user-specified period
        // wakeup_data[19:16] is the initial user-specified period
        // wakeup_data[20] enables radio tx for each measurement
    	WAKEUP_PERIOD_CONT = wakeup_data_field_0 + (wakeup_data_field_1<<8);
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_2 & 0xF;
        radio_tx_option = wakeup_data_field_2 & 0x10;

        set_pmu_sleep_clk_low();
        delay(MBUS_DELAY);

		if (!cdc_running){
			// Go to sleep for initial settling of pressure // FIXME
			set_wakeup_timer (5, 0x1, 0x0); // 150: around 5 min
			cdc_running = 1;
			operation_sleep_noirqreset();
		}
        exec_count = 0;
		meas_count = 0;
        cdc_storage_count = 0;
		radio_tx_count = 0;

		// Reset IRQ10VEC
		*((volatile uint32_t *) IRQ10VEC) = 0;

        // Run CDC Program
		cdc_reset_timeout_count = 0;
        operation_cdc_run();

    }else if(wakeup_data_header == 3){
		// Stop CDC program and transmit the execution count n times
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        // wakeup_data[16] indicates whether or not to speed up PMU sleep clock
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;
		cdc_running = 0;
		Pstack_state = PSTK_IDLE;

		if (wakeup_data_field_2 & 0x1){
			// Speed up PMU sleep osc
			set_pmu_sleep_clk_default();
		}

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
				send_radio_data_ppm(0,0xFAF000+exec_count);	
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
				set_wakeup_timer(WAKEUP_PERIOD_RADIO_INIT, 0x1, 0x0);
				operation_sleep_noirqreset();
			}else{
				send_radio_data_ppm(0, 0xFAF000+exec_count_irq);
				if (exec_count_irq == 3){
					// set timer
					set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
				}else{
					// set timer
					set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
				}
				// go to sleep and wake up with same condition
				operation_sleep_noirqreset();
			}
		}else{
        	operation_tx_stored();
		}

/*
    }else if(wakeup_data_header == 7){
		// Transmit CREF output of the CDC as a battery voltage indicator
		// Optionally runs CREF measurement
        // wakeup_data[7:0] is the # of transmissions
        // wakeup_data[15:8] is the user-specified period 
        // wakeup_data[16] indicates whether or not to speed up PMU sleep clock
        // wakeup_data[17] indicates whether or not to run CDC measurement
        WAKEUP_PERIOD_CONT_INIT = wakeup_data_field_1;

		if (wakeup_data_field_2 & 0x1){
			// Speed up PMU sleep osc
			set_pmu_sleep_clk_default();
		}

		if (wakeup_data_field_2 & 0x2){
			cdc_run_single = 1;
			operation_cdc_run();
		}

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
            // radio
            send_radio_data_ppm(0xF00000 | cdc_storage_cref_latest);	
            // set timer 
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // Go to sleep without timer
            operation_sleep_notimer();
        }
*/

    }else if(wakeup_data_header == 0x11){
		// Slow down PMU sleep osc and go to sleep for further programming
        set_pmu_sleep_clk_low();
        // Go to sleep without timer
        operation_sleep_notimer();

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
    }

    // Proceed to continuous mode
    while(1){
        operation_cdc_run();
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

