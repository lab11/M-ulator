//*******************************************************************
//Author: Yoonmyung Lee
//        Gyouho Kim
//Description: SNSv3 Functionality Tests
//             w/ updates from SNSv3_test_gyouho
//*******************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "SNSv3.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG
//#define SNSv3_DEBUG

// MBus Address
#define SNS_ADDR        0x4

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_CDC_RST    0x1
#define PSTK_CDC_MEAS   0x2
#define PSTK_CDC_READ   0x3

// Others
#define NUM_SAMPLES     20         //Number of CDC samples to take
#define WAKEUP_PERIOD   3
#define MBUS_DELAY	1000

//***************************************************
// Global variables
//***************************************************
volatile uint32_t enumerated;
volatile uint32_t Pstack_state;
volatile uint32_t cdc_data[NUM_SAMPLES];
volatile uint32_t MBus_msg_flag;
volatile uint32_t cdc_data_index;
volatile uint8_t reset_timeout_count;
volatile snsv3_r0_t snsv3_r0;
volatile snsv3_r1_t snsv3_r1;
volatile snsv3_r7_t snsv3_r7;

//***************************************************
//Interrupt Handlers
//Must clear pending bit!
//***************************************************
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void) __attribute__ ((interrupt ("IRQ")));
// INT vector 10 and 11 is not real interrupt vectors
// These vector location (0x0068 and 0x006C) are used as on demaned request parameter buffer
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));

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
void handler_ext_int_4(void){
  *((volatile uint32_t *) 0xE000E280) = 0x10;
}
void handler_ext_int_5(void){
  *((volatile uint32_t *) 0xE000E280) = 0x20;
}
void handler_ext_int_6(void){
  *((volatile uint32_t *) 0xE000E280) = 0x40;
}
void handler_ext_int_7(void){
  *((volatile uint32_t *) 0xE000E280) = 0x80;
}
void handler_ext_int_8(void){
  *((volatile uint32_t *) 0xE000E280) = 0x100;
}
void handler_ext_int_9(void){
  *((volatile uint32_t *) 0xE000E280) = 0x200;
}
void handler_ext_int_10(void){
  *((volatile uint32_t *) 0xE000E280) = 0x400;
}
void handler_ext_int_11(void){
  *((volatile uint32_t *) 0xE000E280) = 0x800;
}

//***************************************************
// Subfunctions
//***************************************************
static void assert_cdc_reset(){
    snsv3_r0.CDC_EXT_RESET = 0x1;	// Assert reset
    snsv3_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}
static void release_cdc_reset(){
    snsv3_r0.CDC_EXT_RESET = 0x0;	// Release reset
    snsv3_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}
static void fire_cdc_meas(){
    snsv3_r0.CDC_EXT_RESET = 0x0;
    snsv3_r0.CDC_CLK = 0x1;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}
static void release_cdc_meas(){
    snsv3_r0.CDC_EXT_RESET = 0x0;
    snsv3_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}

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

void initialize(){

    //Mark Init
    enumerated = 0xABCDEF01;

    //Global variable init
    Pstack_state = PSTK_IDLE; 	//0x0;
    MBus_msg_flag = 0;
    cdc_data_index = 0;
//    snsv3_r0 = SNSv3_R0_DEFAULT;

    //Enumeration
    enumerate(SNS_ADDR);
    asm ("wfi;");
    delay(100);

    //CDC init--------------------------------------

    // SNSv3_R7
    snsv3_r7.CDC_LDO_CDC_LDO_ENB      = 0x1;
    snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB  = 0x1;
    snsv3_r7.ADC_LDO_ADC_LDO_ENB      = 0x1;
    snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;

    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv3_r7.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv3_r7.ADC_LDO_ADC_VREF_SEL     = 0x20;

    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv3_r7.CDC_LDO_CDC_VREF_MUX_SEL = 0x0;
    snsv3_r7.CDC_LDO_CDC_VREF_SEL     = 0x20;

    snsv3_r7.LC_CLK_CONF              = 0x9; // default = 0x9
    write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);

    // SNSv3_R1
    snsv3_r1.CDC_S_period = 0x1A;
    snsv3_r1.CDC_R_period = 0xC;
    snsv3_r1.CDC_CR_ext = 0x0;
    write_mbus_register(SNS_ADDR,1,snsv3_r1.as_int);


    // SNSv3_R0 (need to be initialized here)
    snsv3_r0.CDC_ext_select_CR = 0x0;
    snsv3_r0.CDC_max_select = 0x7;
    snsv3_r0.CDC_ext_max = 0x0;
    snsv3_r0.CDC_CR_fixB = 0x1;
    snsv3_r0.CDC_ext_clk = 0x0;
    snsv3_r0.CDC_outck_in = 0x1;
    snsv3_r0.CDC_on_adap = 0x1;		//0x0 (default 0x1)
    snsv3_r0.CDC_s_recycle = 0x1;	//0x1 (default 0x4)
    snsv3_r0.CDC_Td = 0x0;
    snsv3_r0.CDC_OP_on = 0x0;
    snsv3_r0.CDC_Bias_2nd = 0x7;
    snsv3_r0.CDC_Bias_1st = 0x7;
    snsv3_r0.CDC_EXT_RESET = 0x1;
    snsv3_r0.CDC_CLK = 0x0;
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}

static void cdc_run(){
    uint32_t read_data;
    uint32_t count,i; 

	if (Pstack_state == PSTK_IDLE){
		Pstack_state = PSTK_CDC_RST;

		reset_timeout_count = 0;

		snsv3_r7.CDC_LDO_CDC_LDO_ENB = 0x0;
            	snsv3_r7.ADC_LDO_ADC_LDO_ENB = 0x0;
            	write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
            	delay(10000);

		snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
		snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
		write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
		delay(10000);
    
		// Release CDC isolation
		snsv3_r0.CDC_on_adap = 0x0; // This is used for isolation after FIB
    		write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);

	}else if (Pstack_state == PSTK_CDC_RST){
//        case PSTK_CDC_RST:
            // Release reset
            #ifdef DEBUG_MBUS_MSG
                write_mbus_message(0xAA, 0x11111111);
            #endif
            MBus_msg_flag = 0;
            release_cdc_reset();
            for( count=0; count<2000; count++ ){
                if( MBus_msg_flag ){
                    MBus_msg_flag = 0;
                    Pstack_state = PSTK_CDC_MEAS;
                    return;
                }
                else{
                    delay(30);
                }
            }

            // Reset Time out
            #ifdef DEBUG_MBUS_MSG
            	write_mbus_message(0xAA, 0xFAFAFAFA);
            #endif
	    if (reset_timeout_count > 0){
		
            	write_mbus_message(0xAA, 0xFFFFFFFF);
		Pstack_state = PSTK_IDLE;

		// Put system to sleep to reset the layer controller
		set_wakeup_timer (5, 0x1, 0x0);
		operation_sleep();

	    }else{
		// Try one more time
	    	reset_timeout_count++;
            	assert_cdc_reset();
            	delay(10000);
	    }

	}else if (Pstack_state == PSTK_CDC_MEAS){
//        case PSTK_CDC_MEAS:
            // If no time out 
            #ifdef DEBUG_MBUS_MSG
                write_mbus_message(0xAA, 0x22222222);
                write_mbus_message(0xAA, 0x22222222);
                write_mbus_message(0xAA, 0x22222222);
            #endif

		Pstack_state = PSTK_CDC_READ;
		fire_cdc_meas();
        
		// Put system to sleep to reset the layer controller
		//set_wakeup_timer (5, 0x1, 0x0);
		//operation_sleep();

	}else if (Pstack_state == PSTK_CDC_READ){
//	case PSTK_CDC_READ:

            for( count=0; count<1000; count++ ){
                if( MBus_msg_flag ){
		    #ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x33333333);
			write_mbus_message(0xAA, 0x33333333);
			write_mbus_message(0xAA, 0x33333333);
		    #endif
                    MBus_msg_flag = 0;
                    read_data = *((volatile uint32_t *) 0xA0001014);
                    if( read_data & 0x02 ) {
                        // If CDC data is valid
                        // Process Data
                        cdc_data[cdc_data_index] = read_data;
                        ++cdc_data_index;
                        if( cdc_data_index == NUM_SAMPLES ){
                            // Collected all data for radio TX
                            #ifdef DEBUG_MBUS_MSG
                                write_mbus_message(0xAA, 0x44444444);
				delay(MBUS_DELAY);
                            #endif
                            for( i=0; i<NUM_SAMPLES; ++i){
                                delay(MBUS_DELAY);
                                delay(MBUS_DELAY);
                                delay(MBUS_DELAY);
                                delay(MBUS_DELAY);
                                delay(MBUS_DELAY);
				// Modify data to be more readable
				// CDC_DOUT is shifted to start at LSB
				// Valid bit is moved to 28th bit
				// CR data is moved to 24th
				uint32_t cdc_data_modified;
				cdc_data_modified = cdc_data[i]>>5;
				cdc_data_modified = cdc_data_modified | ((0x2 & cdc_data[i])<<27);
				cdc_data_modified = cdc_data_modified | ((0x1C & cdc_data[i])<<22);
                                write_mbus_message(0x77, cdc_data_modified);
                                //write_mbus_message(0x78, cdc_data[i]);
                            }
                            cdc_data_index = 0;
                            assert_cdc_reset();
                            release_cdc_meas();

                            // Finalize CDC operation
                            Pstack_state = PSTK_IDLE;
                            #ifdef DEBUG_MBUS_MSG
                                write_mbus_message(0xAA, 0x55555555);
								delay(5000);
                                read_mbus_register(SNS_ADDR,0,0x10);
                            #endif
			    // Assert CDC isolation
			    // FIXME: somehow if I enable the following two lines,
			    // the CDC code becomes stuck at ~0E11 and CR 6!!!!!
			    snsv3_r0.CDC_on_adap = 0x1; // This is used for isolation after FIB
    			    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);

			    // Turn off LDO's
                            snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
                            snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
                            snsv3_r7.CDC_LDO_CDC_LDO_ENB = 0x1;
                            snsv3_r7.ADC_LDO_ADC_LDO_ENB = 0x1;
                            write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
                            
			    // Enter long sleep
                            set_wakeup_timer (WAKEUP_PERIOD, 0x1, 0x0);
                            operation_sleep();
			    //return;
                        }
                        else{
                            // Need more data for radio TX
                            release_cdc_meas();

			    // Reset CDC and re-measure
			    Pstack_state = PSTK_CDC_RST;
			    release_cdc_meas();
			    assert_cdc_reset();
			    delay(500);
                            return;
                        }
                    }
                    else{
                        // If CDC data is invalid
                        #ifdef DEBUG_MBUS_MSG
                            write_mbus_message(0xAA, 0x3333FFFF);
                            write_mbus_message(0xAA, 0x3333FFFF);
                            write_mbus_message(0xAA, 0x3333FFFF);
                        #endif
			release_cdc_meas();
			Pstack_state = PSTK_CDC_RST;
			release_cdc_meas();
			assert_cdc_reset();
			delay(500);
                        return;
                    }
                }
                else{
                    delay(30);
                }
            }

            // Measurement Time out
            #ifdef DEBUG_MBUS_MSG
                write_mbus_message(0xAA, 0xFFFFBBBB);
            #endif
            Pstack_state = PSTK_CDC_RST;
            release_cdc_meas();
            assert_cdc_reset();
            delay(500);

	}else{
        //default:  // THIS SHOULD NOT HAPPEN
            // Reset CDC
            Pstack_state = PSTK_IDLE;
            assert_cdc_reset();
            set_wakeup_timer (WAKEUP_PERIOD, 0x1, 0x0);
            operation_sleep();
    	}
} // cdc_run


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************

int main() {
  
    //Clear All Pending Interrupts
    *((volatile uint32_t *) 0xE000E280) = 0x3FF;
    //Enable Interrupts
    *((volatile uint32_t *) 0xE000E100) = 0x3FF;

    //Initialize
    if( enumerated != 0xABCDEF01 )
        initialize();  

    while(1){
        cdc_run();
    }

}
