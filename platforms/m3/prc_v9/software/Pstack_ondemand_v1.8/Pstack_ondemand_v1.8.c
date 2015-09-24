//****************************************************************************************************
//Author:       Gyouho Kim
//              ZhiYoong Foo
//Description:  Derived from zhiyoong_Pblr.c and Tstack_ondemand_temp_radio.c
//              Moving towards Mouse Implantation
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
#include "SNSv5.h"
#include "HRVv1.h"
#include "RADv5.h"

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
#define	MBUS_DELAY 100 //Amount of delay between successive messages
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 1

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_CDC_RST    0x1
#define PSTK_CDC_READ   0x3
#define PSTK_LDO1   	0x4
#define PSTK_LDO2  	 	0x5

// Radio configurations
// FIXME
#define RAD_BIT_DELAY       34     // 14: around 400bps @ USRP // 34: around 200bps
#define RAD_PACKET_DELAY 	2000
#define RAD_PACKET_NUM      1      //How many times identical data will be TXed

// CDC configurations
#define NUM_SAMPLES         3      //Number of CDC samples to take (only 2^n allowed for averaging: 2, 4, 8, 16...)
#define NUM_SAMPLES_TX      1      //Number of CDC samples to be TXed (processed by process_data)
#define NUM_SAMPLES_2PWR    0      //NUM_SAMPLES = 2^NUM_SAMPLES_2PWR - used for averaging

#define CDC_STORAGE_SIZE 20  

//***************************************************
// Global variables
//***************************************************
	//Test Declerations
	// "static" limits the variables to this file, giving compiler more freedom
	// "volatile" should only be used for MMIO
	uint32_t enumerated;
	uint8_t Pstack_state;
	uint32_t cdc_data[NUM_SAMPLES];
	uint32_t cdc_data_tx[NUM_SAMPLES_TX];
	uint32_t cdc_reset_timeout_count;
	uint32_t exec_count;
	uint32_t meas_count;
	uint32_t exec_count_irq;
	uint8_t MBus_msg_flag;
	volatile snsv5_r0_t snsv5_r0 = SNSv5_R0_DEFAULT;
	volatile snsv5_r1_t snsv5_r1 = SNSv5_R1_DEFAULT;
	volatile snsv5_r2_t snsv5_r2 = SNSv5_R2_DEFAULT;
	volatile snsv5_r18_t snsv5_r18 = SNSv5_R18_DEFAULT;
  
	uint32_t WAKEUP_PERIOD_CONT; 
	uint32_t WAKEUP_PERIOD_CONT_INIT; 

	uint32_t cdc_storage[CDC_STORAGE_SIZE] = {0};
	uint32_t cdc_storage_cref[CDC_STORAGE_SIZE] = {0};
	uint32_t cdc_storage_cref_latest;
	uint32_t cdc_storage_count;
	uint8_t cdc_run_single;
	uint8_t cdc_running;
	uint32_t radio_tx_count;
	uint32_t radio_tx_option;
	uint32_t radio_tx_numdata;

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
//Send Radio Data MSB-->LSB
//Two Delays:
//Bit Delay: Delay between each bit (20-bit data, 4-bit header)
//Delays are nop delays (therefore dependent on core speed)
//***************************************************
static void send_radio_data(uint32_t radio_data){
  int32_t i; //loop var
  uint32_t j; //loop var
	for(i=23;i>=0;i--){ //Bit Loop
		if ((radio_data>>i)&1) write_mbus_register(RAD_ADDR,0x27,0x1);
		else                   write_mbus_register(RAD_ADDR,0x27,0x0);
		//Must clear register
		delay(RAD_BIT_DELAY);
		write_mbus_register(RAD_ADDR,0x27,0x0);
		delay(RAD_BIT_DELAY); //Set delay between sending subsequent bit
	}
}

//***************************************************
// CDCW Functions
// SNSv5
//***************************************************
static void release_cdc_pg(){
    snsv5_r0.CDCW_PG_V1P2 = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
    snsv5_r0.CDCW_PG_VBAT = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
    snsv5_r0.CDCW_PG_VLDO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_isolate(){
    snsv5_r0.CDCW_ISO = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void assert_cdc_reset(){
    snsv5_r0.CDCW_RESETn = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_reset(){
    snsv5_r0.CDCW_RESETn = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void fire_cdc_meas(){
    snsv5_r0.CDCW_ENABLE = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_meas(){
    snsv5_r0.CDCW_ENABLE = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
}
static void ldo_power_off(){
	snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
	snsv5_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
	snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x1;
	snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x1;
	write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
    delay(MBUS_DELAY);
}
static void cdc_power_off(){
    snsv5_r0.CDCW_ISO = 0x1;
    snsv5_r0.CDCW_PG_V1P2 = 0x1;
    snsv5_r0.CDCW_PG_VBAT = 0x1;
    snsv5_r0.CDCW_PG_VLDO = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);
    delay(MBUS_DELAY);
	ldo_power_off();
}


inline static void set_pmu_sleep_clk_low(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77003B; // 0x8F77003B: use GOC x0.6-2
}
inline static void set_pmu_sleep_clk_default(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F77004B; // 0x8F77004B: use GOC x10-45
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
	// Disable Timer
	set_wakeup_timer (0, 0x0, 0x0);
	// Go to sleep without timer
	operation_sleep();

}

static void operation_tx_stored(void){

	//Fire off stored data to radio
	uint32_t data1 = 0xF00000 | cdc_storage[radio_tx_count];
	uint32_t data2 = 0xF00000 | cdc_storage_cref[radio_tx_count];
	delay(MBUS_DELAY);

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
	send_radio_data(data1);
	delay(RAD_PACKET_DELAY*3); //Set delays between sending subsequent packet
	send_radio_data(data2);

	if (((!radio_tx_numdata)&&(radio_tx_count > 0)) | ((radio_tx_numdata)&&((radio_tx_numdata+radio_tx_count) > cdc_storage_count))){
		radio_tx_count--;
		// set timer
		set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
		// go to sleep and wake up with same condition
		operation_sleep_noirqreset();

	}else{
		delay(RAD_PACKET_DELAY*3); //Set delays between sending subsequent packet
		send_radio_data(0xFAF000);

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
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
    *((volatile uint32_t *) 0xA200000C) = 0x8F77004B;
  
    // Speed up GOC frontend to match PMU frequency
    // PRCv9 Default: 0x00202903
    *((volatile uint32_t *) 0xA2000008) = 0x00202908;
  
    delay(10000);
    delay(100);
  
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
    // snsv5_r0
    snsv5_r0.CDCW_IRQ_EN	= 1;
    snsv5_r0.CDCW_MODE_PAR	= 0;
    snsv5_r0.CDCW_RESETn 	= 0;
    write_mbus_register(SNS_ADDR,0,snsv5_r0.as_int);

    // snsv5_r1
    snsv5_r1.CDCW_N_CYCLE_SINGLE	= 1; // Default: 8; Min: 1
    write_mbus_register(SNS_ADDR,1,snsv5_r1.as_int);
	
    // snsv5_r2
    snsv5_r2.CDCW_N_CYCLE_SET	= 100; // Min: 0
    write_mbus_register(SNS_ADDR,2,snsv5_r2.as_int);

    // SNSv5_R18
    snsv5_r18.CDC_LDO_CDC_CURRENT_2X  = 0x0;

    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv5_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv5_r18.ADC_LDO_ADC_VREF_SEL     = 0x20;

    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv5_r18.CDC_LDO_CDC_VREF_MUX_SEL = 0x0;
    snsv5_r18.CDC_LDO_CDC_VREF_SEL     = 0x20;

    write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);

    // Radio Settings --------------------------------------
    radv5_r23_t radv5_r23; //Ext Ctrl En
    radv5_r26_t radv5_r26; //Current Limiter 2F = 30uA, 1F = 3uA
    radv5_r20_t radv5_r20; //Tune Power
    radv5_r21_t radv5_r21; //Tune Freq 1
    radv5_r22_t radv5_r22; //Tune Freq 2
    radv5_r25_t radv5_r25; //Tune TX Time
    radv5_r27_t radv5_r27; //Zero the TX register
    //RADv5 R23
    radv5_r23.RADIO_EXT_CTRL_EN_1P2 = 0x0; //Ext Ctrl En
    write_mbus_register(RAD_ADDR,0x23,radv5_r23.as_int);
    delay(MBUS_DELAY);
    //RADv5 R26
    radv5_r26.RADIO_TUNE_CURRENT_LIMITER_1P2 = 0x1F; //Current Limiter 2F = 30uA, 1F = 3uA
    write_mbus_register(RAD_ADDR,0x26,radv5_r26.as_int);
    delay(MBUS_DELAY);
    //RADv5 R20
    radv5_r20.RADIO_TUNE_POWER_1P2 = 0x1F; //Tune Power 0x1F = Full Power
    write_mbus_register(RAD_ADDR,0x20,radv5_r20.as_int);
    delay(MBUS_DELAY);
    //RADv5 R21
    radv5_r21.RADIO_TUNE_FREQ1_1P2 = 0x0; //Tune Freq 1
    write_mbus_register(RAD_ADDR,0x21,radv5_r21.as_int);
    delay(MBUS_DELAY);
    //RADv5 R22
    radv5_r22.RADIO_TUNE_FREQ2_1P2 = 0x0; //Tune Freq 2 //0x0,0x0 = 902MHz on Pblr005
    write_mbus_register(RAD_ADDR,0x22,radv5_r22.as_int);
    delay(MBUS_DELAY);
    //RADv5 R25
    radv5_r25.RADIO_TUNE_TX_TIME_1P2 = 0x5; //Tune TX Time  0x5: no pulse; around 2.4ms // 0x4: 380ns
    write_mbus_register(RAD_ADDR,0x25,radv5_r25.as_int);
    delay(MBUS_DELAY);
    //RADv5 R27
    radv5_r27.RADIO_DATA_1P2 = 0x0; //Zero the TX register
    write_mbus_register(RAD_ADDR,0x27,radv5_r27.as_int);
  
    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
	cdc_run_single = 0;
	cdc_running = 0;
    
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

		snsv5_r18.CDC_LDO_CDC_LDO_ENB = 0x0;
		//snsv5_r18.ADC_LDO_ADC_LDO_ENB = 0x0;
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
		// Long delay required here
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();

	}else if (Pstack_state == PSTK_LDO1){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x1);
		#endif
		Pstack_state = PSTK_LDO2;
		snsv5_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
		write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
		//delay(LDO_DELAY); // This delay is required to avoid current spike
		//snsv5_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
		//write_mbus_register(SNS_ADDR,18,snsv5_r18.as_int);
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
					cdc_storage_count++;
					radio_tx_count = cdc_storage_count;
				}
				// Optionally transmit the data
				if (radio_tx_option){
					send_radio_data(0xF00000 | read_data_reg4);
					delay(MBUS_DELAY*10);
					send_radio_data(0xF00000 | read_data_reg6);
					delay(MBUS_DELAY);
				}

				// Enter long sleep
				if(exec_count < 4){
					// Send some signal
					send_radio_data(0xFAF000);
					set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);

				}else{
					set_wakeup_timer (WAKEUP_PERIOD_CONT, 0x1, 0x0);
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
            // radio
            send_radio_data(0xFAF000+exec_count_irq);	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
            // radio
            send_radio_data(0xFAF000+exec_count_irq);	
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

		if (wakeup_data_field_2 & 0x1){
			// Speed up PMU sleep osc
			set_pmu_sleep_clk_default();
		}

        if (exec_count_irq < wakeup_data_field_0){
            exec_count_irq++;
            // radio
            send_radio_data(0xFAF000+exec_count);	
            // set timer
            set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
            // go to sleep and wake up with same condition
            operation_sleep_noirqreset();

        }else{
            exec_count_irq = 0;
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
			// radio
			send_radio_data(0xFAF000+exec_count_irq);
			if (exec_count_irq == 2){
				// set timer
				set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT*2, 0x1, 0x0);
			}else{
				// set timer
				set_wakeup_timer (WAKEUP_PERIOD_CONT_INIT, 0x1, 0x0);
			}
			// go to sleep and wake up with same condition
			operation_sleep_noirqreset();
    
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
            send_radio_data(0xF00000 | cdc_storage_cref_latest);	
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

