//*******************************************************************
//Author: Yoonmyung Lee
//Description: SNSv3 Functionality Tests
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

// Others
#define NUM_SAMPLES       5         //Number of CDC samples to take
#define WAKEUP_PERIOD   0x5

//***************************************************
// Global variables
//***************************************************
volatile uint32_t enumerated;
volatile uint32_t Pstack_state;
volatile uint32_t cdc_data[NUM_SAMPLES];
volatile uint32_t MBus_msg_flag;
volatile uint32_t cdc_data_index;
volatile snsv3_r0_t snsv3_r0;
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
//  snsv3_r0_t snsv3_r0 = SNSv3_R0_DEFAULT;
    snsv3_r1_t snsv3_r1 = SNSv3_R1_DEFAULT;
//  snsv3_r7_t snsv3_r7 = SNSv3_R7_DEFAULT;

    // SNSv3_R7
    snsv3_r7.as_int = 0x00000000;
    snsv3_r7.CDC_LDO_CDC_LDO_ENB      = 0x1;
    snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB  = 0x1;
    snsv3_r7.CDC_LDO_CDC_VREF_MUX_SEL = 0x2; // non-default
    snsv3_r7.CDC_LDO_CDC_VREF_SEL     = 0x4; // non-default
    snsv3_r7.ADC_LDO_ADC_LDO_ENB      = 0x1;
    snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;
    snsv3_r7.ADC_LDO_ADC_VREF_MUX_SEL = 0x2;
    snsv3_r7.ADC_LDO_ADC_VREF_SEL     = 0x4;
    snsv3_r7.LC_CLK_CONF              = 0x9; // default = 0x9
    write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);

    // SNSv3_R1
    snsv3_r1.CDC_S_period = 0x1A;
    snsv3_r1.CDC_R_period = 0xC;
    snsv3_r1.CDC_CR_ext = 0x0;
    write_mbus_register(SNS_ADDR,1,snsv3_r1.as_int);


    // SNSv3_R0 (need to be initialized here)
    snsv3_r0.as_int = 0x00000000; //SNSv3_R0_DEFAULT;
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

    switch( Pstack_state ){

        case PSTK_IDLE:
            Pstack_state = PSTK_CDC_RST;

    write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
    snsv3_r1_t snsv3_r1 = SNSv3_R1_DEFAULT;
    snsv3_r1.CDC_S_period = 0x1A;
    snsv3_r1.CDC_R_period = 0xC;
    snsv3_r1.CDC_CR_ext = 0x0;
    write_mbus_register(SNS_ADDR,1,snsv3_r1.as_int);
    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);

#ifdef SNSv3_DEBUG
        write_mbus_message(0xBB, 0x11111111);
			delay(5000);
            read_mbus_register(SNS_ADDR,0x0,0x10);
			delay(5000);
            read_mbus_register(SNS_ADDR,0x7,0x10);
			delay(5000);
#endif
            snsv3_r7.ADC_LDO_ADC_LDO_ENB = 0x0;
            write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
            delay(50000);
            read_mbus_register(SNS_ADDR,0x7,0x10);
			delay(5000);

#ifdef SNSv3_DEBUG
        write_mbus_message(0xBB, 0x22222222);
			delay(5000);
            read_mbus_register(SNS_ADDR,0x0,0x10);
			delay(5000);
            read_mbus_register(SNS_ADDR,0x7,0x10);
			delay(5000);
#endif
            snsv3_r7.CDC_LDO_CDC_LDO_ENB = 0x0;
            write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
            delay(50000);
            read_mbus_register(SNS_ADDR,0x7,0x10);
			delay(5000);

#ifdef SNSv3_DEBUG
        write_mbus_message(0xBB, 0x33333333);
  			delay(5000);
            read_mbus_register(SNS_ADDR,0x0,0x10);
			delay(5000);
            read_mbus_register(SNS_ADDR,0x7,0x10);
			delay(5000);
#endif
            snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB = 0x0;
            write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
            delay(50000);
            read_mbus_register(SNS_ADDR,0x7,0x10);
			delay(5000);

#ifdef SNSv3_DEBUG
        write_mbus_message(0xBB, 0x44444444);
			delay(5000);
            read_mbus_register(SNS_ADDR,0x0,0x10);
			delay(5000);
            read_mbus_register(SNS_ADDR,0x7,0x10);
			delay(5000);
#endif
            snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
            write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
            delay(5000);
            read_mbus_register(SNS_ADDR,0x0,0x10);
			delay(5000);
            read_mbus_register(SNS_ADDR,0x7,0x10);
			delay(5000);
    

		// Release CDC isolation
		snsv3_r0.CDC_on_adap = 0x0; // This is used for isolation after FIB
    		write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
        break;

        case PSTK_CDC_RST:
            // Release reset
            #ifdef DEBUG_MBUS_MSG
                write_mbus_message(0xAA, 0x11111111);
            #endif
            MBus_msg_flag = 0;
            release_cdc_reset();
            for( count=0; count<4000; count++ ){
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
                write_mbus_message(0xAA, 0xAAAAAAAA);
				delay(10000);
    	        read_mbus_register(SNS_ADDR,0x0,0x10);
				delay(10000);
    	        read_mbus_register(SNS_ADDR,0x1,0x10);
				delay(10000);
    	        read_mbus_register(SNS_ADDR,0x7,0x10);
				delay(10000);
            #endif
            assert_cdc_reset();
            delay(50000);
            break;

        case PSTK_CDC_MEAS:
            // If no time out 
            #ifdef DEBUG_MBUS_MSG
                write_mbus_message(0xAA, 0x22222222);
            #endif
            fire_cdc_meas();
            for( count=0; count<1000; count++ ){
                if( MBus_msg_flag ){
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
                            #endif
                            for( i=0; i<NUM_SAMPLES; ++i){
                                delay(6000);
                                write_mbus_message(0x77, cdc_data[i]);
                            }
                            cdc_data_index = 0;
                            assert_cdc_reset();

                            // Finalize CDC operation
                            Pstack_state = PSTK_IDLE;
                            #ifdef DEBUG_MBUS_MSG
                                write_mbus_message(0xAA, 0x55555555);
								delay(5000);
                                read_mbus_register(SNS_ADDR,0,0x10);
                            #endif

			    // Assert CDC isolation
			    snsv3_r0.CDC_on_adap = 0x1; // This is used for isolation after FIB
    			    write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
				
				
                            snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
                            snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
                            write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
                            snsv3_r7.CDC_LDO_CDC_LDO_ENB = 0x1;
                            snsv3_r7.ADC_LDO_ADC_LDO_ENB = 0x1;
                            write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
                            // Enter long sleep
                            set_wakeup_timer (WAKEUP_PERIOD, 0x1, 0x0);
                            operation_sleep();
                        }
                        else{
                            // Need more data for radio TX
                            #ifdef DEBUG_MBUS_MSG
                                write_mbus_message(0xAA, 0x33333333);
                                read_mbus_register(SNS_ADDR,0,0x10);
                            #endif
                            release_cdc_meas();
                            return;
                        }
                    }
                    else{
                        // If CDC data is invalid
                        release_cdc_meas();
                        return;
                    }
                }
                else{
                    delay(30);
                }
            }

            // Measurement Time out
            #ifdef DEBUG_MBUS_MSG
                write_mbus_message(0xAA, 0xBBBBBBBB);
            #endif
            Pstack_state = PSTK_CDC_RST;
            assert_cdc_reset();
            delay(500);
            break;

        default:  // THIS SHOULD NOT HAPPEN
            // Reset CDC
            Pstack_state = PSTK_IDLE;
            assert_cdc_reset();
            set_wakeup_timer (WAKEUP_PERIOD, 0x1, 0x0);
            operation_sleep();
            break;
    }
}


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
