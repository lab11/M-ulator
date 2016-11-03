//*******************************************************************
//Author: ZhiYoong Foo
//Description: Temperature Sensing System for in FLUKE for Roche
//             PRCv13, SNSv7, PRCv13
//*******************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "SNSv7.h"

// Stack order  PRC->SNS
#define SNS_ADDR 0x5

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;

volatile snsv7_r14_t snsv7_r14 = SNSv7_R14_DEFAULT;
volatile snsv7_r15_t snsv7_r15 = SNSv7_R15_DEFAULT;
volatile snsv7_r18_t snsv7_r18 = SNSv7_R18_DEFAULT;
volatile snsv7_r25_t snsv7_r25 = SNSv7_R25_DEFAULT;
  
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
void handler_ext_int_2(void)  { *NVIC_ICPR = (0x1 << 2);  } // REG0
void handler_ext_int_3(void)  { *NVIC_ICPR = (0x1 << 3);  } // REG1
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4);  } // REG2
void handler_ext_int_5(void)  { *NVIC_ICPR = (0x1 << 5);  } // REG3
void handler_ext_int_6(void)  { *NVIC_ICPR = (0x1 << 6);  } // REG4
void handler_ext_int_7(void)  { *NVIC_ICPR = (0x1 << 7);  } // REG5
void handler_ext_int_8(void)  { *NVIC_ICPR = (0x1 << 8);  } // REG6
void handler_ext_int_9(void)  { *NVIC_ICPR = (0x1 << 9);  } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); } // MEM WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); } // MBUS_FWD
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // MBUS_FWD

//***************************************************
// Temp Sensor Functions (SNSv7)
//***************************************************

static void temp_sensor_enable(){
    snsv7_r14.TEMP_SENSOR_ENABLEb = 0x0;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_release_reset(){
    snsv7_r14.TEMP_SENSOR_RESETn = 1;
    snsv7_r14.TEMP_SENSOR_ISO = 0;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}

static void operation_init(void){
  
    // Set CPU & Mbus Clock Speeds
    prcv13_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv13_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv13_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    *((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv13_r0B.as_int;

  
    //Enumerate & Initialize Registers
    enumerated = 0xDEADBEEF;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    // set_halt_until_mbus_rx();
    
    //Enumeration
    delay(MBUS_DELAY);
    mbus_enumerate(SNS_ADDR);
    
    // Set CPU Halt Option as TX --> Use for register write e.g.
    // set_halt_until_mbus_tx();
    
    // Temp Sensor Settings --------------------------------------
    // SNSv7_R25
    snsv7_r25.TEMP_SENSOR_IRQ_PACKET = 0x001800;
    mbus_remote_register_write(SNS_ADDR,0x19,snsv7_r25.as_int);
    // SNSv7_R14
    snsv7_r14.TEMP_SENSOR_BURST_MODE = 0x1;
    snsv7_r14.TEMP_SENSOR_DELAY_SEL = 0x3;
    snsv7_r14.TEMP_SENSOR_R_tmod = 0xE;
    snsv7_r14.TEMP_SENSOR_R_bmod = 0xE;
    mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
    // snsv7_R15
    snsv7_r15.TEMP_SENSOR_AMP_BIAS = 0x7;
    snsv7_r15.TEMP_SENSOR_CONT_MODEb = 0x0;
    snsv7_r15.TEMP_SENSOR_SEL_CT = 0x9;
    snsv7_r15.TEMP_SENSOR_R_PTAT = 0x8F;
    snsv7_r15.TEMP_SENSOR_R_REF = 0x7F;
    mbus_remote_register_write(SNS_ADDR,0xF,snsv7_r15.as_int);
    
    // snsv7_R18
    snsv7_r18.ADC_LDO_ADC_LDO_ENB      = 0x1;
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;
    snsv7_r18.ADC_LDO_ADC_CURRENT_2X  = 0x1;
    
    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv7_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x2;
    snsv7_r18.ADC_LDO_ADC_VREF_SEL     = 0x10;
    
    mbus_remote_register_write(SNS_ADDR,0x12,snsv7_r18.as_int);
    
    snsv7_r18.ADC_LDO_ADC_LDO_ENB = 0x0;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
    delay(MBUS_DELAY);
<<<<<<< HEAD
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
    // Release Temp Sensor Reset
    temp_sensor_release_reset();
=======

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
		if (radio_tx_option || ((exec_count+1) < TEMP_CYCLE_INIT)){
			radio_power_on();
		}

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
					delay(RADIO_PACKET_DELAY);
					send_radio_data_ppm(1, 0xFAF000);
					set_wakeup_timer(WAKEUP_PERIOD_CONT_INIT, 0x1, 0x1);

				}else{
					if (!radio_tx_option && (exec_count == TEMP_CYCLE_INIT)){
						//set_pmu_sleep_clk_low();
					}
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

>>>>>>> 2b10c84843c4a6b994fcc925687a73878b0a91e1
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
  
    disable_timerwd();
    
    // Initialization sequence
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        operation_init();
    }

    temp_sensor_enable();
    while(1);

    // Should not reach here
    while(1);
}


