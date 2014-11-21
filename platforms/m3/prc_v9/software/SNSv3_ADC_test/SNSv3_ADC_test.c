//*******************************************************************
//Author: Dongmin Yoon
//Description: SNSv3 Functionality Tests
//*******************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "SNSv3.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG
#define SNSv3_DEBUG

// MBus Address
#define SNS_ADDR	0x4

// Pstack states
#define	PSTK_IDLE	0x0
#define PSTK_ADC_RST	0x1
#define PSTK_ADC_MEAS	0x2
#define PSTK_ADC_DONE	0x3

// Others
#define NUM_SAMPLES	10         //Number of ADC samples to take
#define WAKEUP_PERIOD	0x3

//***************************************************
// Global variables
//***************************************************
volatile uint32_t enumerated;
volatile uint32_t Pstack_state;
volatile uint32_t adc_data[NUM_SAMPLES];
volatile uint32_t MBus_msg_flag;
volatile uint32_t count_conv;
volatile snsv3_r0_t snsv3_r0;
//volatile snsv3_r1_t snsv3_r1;
volatile snsv3_r7_t snsv3_r7;
volatile snsv3_r8_t snsv3_r8;
volatile snsv3_r9_t snsv3_r9;

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
static void assert_adc_reset(){
	snsv3_r8.SAR_ADC_START_CONV = 0x0;	// Assert reset
	write_mbus_register(SNS_ADDR,8,snsv3_r8.as_int);
	delay(5000);
}
static void release_adc_reset(){
	snsv3_r8.SAR_ADC_START_CONV = 0x1;	// Assert reset
	write_mbus_register(SNS_ADDR,8,snsv3_r8.as_int);
	delay(5000);
}
//static void fire_cdc_meas(){
//	snsv3_r0.CDC_EXT_RESET = 0x0;
//	snsv3_r0.CDC_CLK = 0x1;
//	write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
//}
//static void release_cdc_meas(){
//	snsv3_r0.CDC_EXT_RESET = 0x0;
//	snsv3_r0.CDC_CLK = 0x0;
//	write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
//}

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
//	cdc_data_index = 0;
//	snsv3_r0 = SNSv3_R0_DEFAULT;

	//Enumeration
	enumerate(SNS_ADDR);
	asm ("wfi;");
	delay(100);

	//CDC init--------------------------------------
//	snsv3_r0_t snsv3_r0 = SNSv3_R0_DEFAULT;
	snsv3_r1_t snsv3_r1 = SNSv3_R1_DEFAULT;
//	snsv3_r7_t snsv3_r7 = SNSv3_R7_DEFAULT;

	// SNSv3_R7
	snsv3_r7.as_int = 0x00000000;
	snsv3_r7.CDC_LDO_CDC_LDO_ENB      = 0x1;
	snsv3_r7.CDC_LDO_CDC_LDO_DLY_ENB  = 0x1;
	snsv3_r7.CDC_LDO_CDC_VREF_MUX_SEL = 0x2; // non-default
	snsv3_r7.CDC_LDO_CDC_VREF_SEL     = 0x4; // non-default
	snsv3_r7.ADC_LDO_ADC_LDO_ENB      = 0x1;
	snsv3_r7.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;
	snsv3_r7.ADC_LDO_ADC_VREF_MUX_SEL = 0x0;
	snsv3_r7.ADC_LDO_ADC_VREF_SEL     = 0x1;
	snsv3_r7.LC_CLK_CONF              = 0x9; // default = 0x9
	write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);

	// SNSv3_R8
	snsv3_r8.SAR_ADC_ADC_DOUT_ISO       = 0x1;
	snsv3_r8.SAR_ADC_OSC_ENB            = 0x1;
	snsv3_r8.SAR_ADC_START_CONV         = 0x0;
	snsv3_r8.SAR_ADC_BYPASS_NUMBER      = 0x4;
	snsv3_r8.SAR_ADC_SINGLE_CONV        = 0x1;
	snsv3_r8.SAR_ADC_SIGNAL_SEL         = 0x0;
	snsv3_r8.SAR_ADC_EXT_SIGNAL_SEL     = 0x3;
	snsv3_r8.SAR_ADC_OSC_SEL            = 0x6;
	snsv3_r8.SAR_ADC_OSC_DIV            = 0x1;
	write_mbus_register(SNS_ADDR,8,snsv3_r8.as_int);

	// SNSv3_R9
	snsv3_r9.SAR_ADC_FAST_DIV_EN	= 0x0;
	snsv3_r9.SAR_ADC_COMP_CAP_R	= 0x00;
	snsv3_r9.SAR_ADC_COMP_CAP_L	= 0x00;
	write_mbus_register(SNS_ADDR,9,snsv3_r9.as_int);

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
	snsv3_r0.CDC_on_adap = 0x0;		//0x0 (default 0x1)
	snsv3_r0.CDC_s_recycle = 0x1;	//0x1 (default 0x4)
	snsv3_r0.CDC_Td = 0x0;
	snsv3_r0.CDC_OP_on = 0x0;
	snsv3_r0.CDC_Bias_2nd = 0x7;
	snsv3_r0.CDC_Bias_1st = 0x7;
	snsv3_r0.CDC_EXT_RESET = 0x1;
	snsv3_r0.CDC_CLK = 0x0;
	write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);
}

static void adc_run(){
//	uint32_t read_data;
	uint32_t count,i; 
//	uint32_t count_conv;

	switch( Pstack_state ){
	case PSTK_IDLE:
		Pstack_state = PSTK_ADC_RST;
		count_conv=0;

		write_mbus_register(SNS_ADDR,7,snsv3_r7.as_int);
//		snsv3_r1_t snsv3_r1 = SNSv3_R1_DEFAULT;
//		snsv3_r1.CDC_S_period = 0x1A;
//		snsv3_r1.CDC_R_period = 0xC;
//		snsv3_r1.CDC_CR_ext = 0x0;
//		write_mbus_register(SNS_ADDR,1,snsv3_r1.as_int);
//		write_mbus_register(SNS_ADDR,0,snsv3_r0.as_int);

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

		snsv3_r8.SAR_ADC_OSC_ENB = 0x0;
		snsv3_r8.SAR_ADC_ADC_DOUT_ISO = 0x0;
		write_mbus_register(SNS_ADDR,8,snsv3_r8.as_int);
		delay(5000);
		read_mbus_register(SNS_ADDR,0x0,0x10);
		delay(5000);
		read_mbus_register(SNS_ADDR,0x8,0x10);
		delay(5000);
		break;

	case PSTK_ADC_RST:
		// Release reset
		#ifdef DEBUG_MBUS_MSG
		write_mbus_message(0xAA, 0x11111111);
		delay(5000);
		#endif
		MBus_msg_flag = 0;
		release_adc_reset();
		for( count=0; count<4000; count++ ){
			if( MBus_msg_flag ){
				MBus_msg_flag = 0;
				Pstack_state = PSTK_ADC_MEAS;
				++count_conv;
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
		#endif
		break;

	case PSTK_ADC_MEAS:
		// If no time out 
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x22222222);
			delay(5000);
			write_mbus_message(0x77,count_conv);
			delay(50000);
			read_mbus_register(SNS_ADDR, 0xA, 0x10); 
			delay(5000);
		#endif

		adc_data[count_conv] = *((volatile uint32_t *) 0xA0001018);
		if(count_conv == NUM_SAMPLES) {
			for(i=0; i<NUM_SAMPLES; ++i){
				delay(6000);
				write_mbus_message(0x77,adc_data[i]);
			}
			Pstack_state = PSTK_ADC_DONE;
		}
		else{
			assert_adc_reset();
			Pstack_state = PSTK_ADC_RST;
		}

		// Measurement Time out
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0xBBBBBBBB);
		#endif
		break;

//	case 3:
//		Pstack_state=PSTK_ADC_DONE;
//		delay(50000);
//		break;

	default:  // THIS SHOULD NOT HAPPEN
		delay(50000);
//		Pstack_state = PSTK_IDLE;
//		assert_adc_reset();
//		set_wakeup_timer (WAKEUP_PERIOD, 0x1, 0x0);
//		operation_sleep();
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
    //if( enumerated != 0xABCDEF01 )
        initialize();  

    while(1){
        adc_run();
    }

}
