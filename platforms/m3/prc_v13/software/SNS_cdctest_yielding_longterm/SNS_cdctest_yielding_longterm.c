//****************************************************************************************************
//Author:       Gyouho Kim
//Description:  Derived from Pstack_ondemand_v1.9_cdctest_yielding_inhee.c
//				- PRCv13 / PMUv2 / RADv9 / SNSv7
//****************************************************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->SNS->HRV->RAD
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6
#define RAD_ADDR 0x4

// CDC parameters
#define	MBUS_DELAY 100 //Amount of delay between successive messages
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 400
#define WAKEUP_PERIOD_RESET 1
#define WAKEUP_PERIOD_LDO 1

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_CDC_RST    0x1
#define PSTK_CDC_READ   0x3
#define PSTK_LDO1   	0x4
#define PSTK_LDO2  	 	0x5

#define CDC_STORAGE_SIZE 0 // FIXME

//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint8_t Pstack_state;
volatile uint32_t cdc_reset_timeout_count;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t mbus_msg_flag;

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
volatile uint32_t set_cdc_exec_count;

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile bool radio_ready;
volatile bool radio_on;

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

	// Reset wakeup timer
	*WUPT_RESET = 0x01;

    // Go to Sleep
    delay(MBUS_DELAY);
    mbus_sleep_all();
    while(1);

}

static void operation_sleep_noirqreset(void){

	// Reset wakeup timer
	*WUPT_RESET = 0x01;

    // Go to Sleep
    delay(MBUS_DELAY);
    mbus_sleep_all();
    while(1);

}

static void operation_sleep_notimer(void){
    
    // Make sure LDO is off
    ldo_power_off();
	
    // Make sure Radio is off
//    if (radio_on){radio_power_off();}

    // Disable Timer
    set_wakeup_timer(0, 0, 0);

    // Go to sleep without timer
    delay(MBUS_DELAY);
    operation_sleep();

}


static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;
  
    delay(1000);
  
    // Disable MBus Watchdog Timer
    //*REG_MBUS_WD = 0;
	*((volatile uint32_t *) 0xA000007C) = 0;

    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;

    //Enumeration
    //mbus_enumerate(RAD_ADDR);
    //delay(MBUS_DELAY*10);
    mbus_enumerate(SNS_ADDR);
    delay(MBUS_DELAY);
    //mbus_enumerate(HRV_ADDR);
    //delay(MBUS_DELAY);

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
    snsv7_r2.CDCW_N_CYCLE_SET	= 200; // Min: 0
    mbus_remote_register_write(SNS_ADDR,2,snsv7_r2.as_int);
  
    // snsv7_r18
    snsv7_r18.CDC_LDO_CDC_CURRENT_2X  = 1;
  
    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv7_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv7_r18.ADC_LDO_ADC_VREF_SEL     = 0x20;
  
    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv7_r18.CDC_LDO_CDC_VREF_MUX_SEL = 0x2;
    snsv7_r18.CDC_LDO_CDC_VREF_SEL     = 0x20;
  
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
  
	// Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(SNS_ADDR,0x18,0x1800);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 1;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
	meas_count = 0;
    
    // Go to sleep without timer
    //operation_sleep_notimer();
}

static void operation_cdc_run(){
    uint32_t count; 

    if (Pstack_state == PSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xAA, 0x0);
		#endif
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;

/*
		// Power on radio
		if (radio_tx_option || ((exec_count+1) < CDC_CYCLE_INIT)){
			radio_power_on();
		}
*/

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

			if (cdc_reset_timeout_count > 5){
				// CDC is not resetting for some reason. Go to sleep forever
				Pstack_state = PSTK_IDLE;
				cdc_power_off();
				operation_sleep_notimer();
			}else{
				// Put system to sleep to reset the layer controller
				set_wakeup_timer(WAKEUP_PERIOD_RESET, 0x1, 0x1);
				operation_sleep_noirqreset();
			}

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

		// FIXME
		mbus_write_message32(0x74, read_data_reg4); // CMEAS
		mbus_write_message32(0x76, read_data_reg6); // CREF
		mbus_write_message32(0x77, read_data_reg7); // CMEAS reverse
		mbus_write_message32(0x79, read_data_reg9); // CPAR

		// FIXME
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
/*
				// Store results in memory; unless buffer is full
				if (cdc_storage_count < CDC_STORAGE_SIZE){
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
*/
				// Enter long sleep
				set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
/*
				// Make sure Radio is off
				if (radio_on){
					radio_ready = 0;
					radio_power_off();
				}
*/
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
    //config_timerwd(0xFFFFF); // 0xFFFFF about 13 sec with Y2 run default clock
	disable_timerwd();

    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();

		radio_tx_option = 0;
		set_cdc_exec_count = 0;	
		cdc_run_single = 0; 
		cdc_reset_timeout_count = 0;
		exec_count = 0;

    }

    // Proceed to continuous mode
    while(1){
      operation_cdc_run();
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

