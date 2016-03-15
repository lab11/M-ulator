//****************************************************************************************************
//Author:       Gyouho Kim
//              ZhiYoong Foo
//Description:  Derived from zhiyoong_Pblr.c and Tstack_ondemand_temp_radio.c
//              Moving towards Mouse Implantation
//                              Revision 1.9
//                              - Using SNSv6
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9.h"
#include "SNSv6.h"

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
#define	MBUS_DELAY 50 //Amount of delay between successive messages
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 500
#define WAKEUP_PERIOD_RESET 2
#define WAKEUP_PERIOD_LDO 1

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_ADC_RST    0x1
#define PSTK_ADC_READ   0x3
#define PSTK_LDO1   	0x4
#define PSTK_LDO2  	 	0x5

// Radio configurations
#define RAD_BIT_DELAY       13     //40      //0x54    //Radio tuning: Delay between bits sent (16 bits / packet)
#define RAD_PACKET_DELAY 	2000      //1000    //Radio tuning: Delay between packets sent (3 packets / sample)
#define RAD_PACKET_NUM      1      //How many times identical data will be TXed

// CDC configurations
#define NUM_SAMPLES         3      //Number of CDC samples to take (only 2^n allowed for averaging: 2, 4, 8, 16...)
#define NUM_SAMPLES_TX      1      //Number of CDC samples to be TXed (processed by process_data)
#define NUM_SAMPLES_2PWR    0      //NUM_SAMPLES = 2^NUM_SAMPLES_2PWR - used for averaging

#define CDC_STORAGE_SIZE 0  

//***************************************************
// Global variables
//***************************************************
	//Test Declerations
	// "static" limits the variables to this file, giving compiler more freedom
	// "volatile" should only be used for MMIO
	static uint32_t enumerated;
	static uint8_t Pstack_state;
	static uint32_t cdc_data[NUM_SAMPLES];
	static uint32_t cdc_data_tx[NUM_SAMPLES_TX];
	static uint32_t cdc_data_index;
	static uint32_t cdc_reset_timeout_count;
	static uint32_t exec_count;
	static uint32_t meas_count;
	static uint32_t exec_count_irq;
	static uint32_t MBus_msg_flag;
	volatile snsv6_r0_t snsv6_r0 = SNSv6_R0_DEFAULT;
	volatile snsv6_r1_t snsv6_r1 = SNSv6_R1_DEFAULT;
	volatile snsv6_r2_t snsv6_r2 = SNSv6_R2_DEFAULT;
	volatile snsv6_r17_t snsv6_r17 = SNSv6_R17_DEFAULT; // For LDO's
	volatile snsv6_r18_t snsv6_r18 = SNSv6_R18_DEFAULT;
	volatile snsv6_r19_t snsv6_r19 = SNSv6_R19_DEFAULT;
  
	static uint32_t WAKEUP_PERIOD_CONT; 
	static uint32_t WAKEUP_PERIOD_CONT_INIT; 

	static uint32_t cdc_storage[CDC_STORAGE_SIZE] = {0};
	static uint32_t cdc_storage_count;
	static uint32_t radio_tx_count;
	static uint32_t radio_tx_option;
	static uint32_t radio_tx_numdata;

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

static void assert_adc_reset(){
	snsv6_r18.SAR_ADC_START_CONV = 0x0;	// Assert reset
	write_mbus_register(SNS_ADDR,18,snsv6_r18.as_int);
	delay(5000);
}
static void release_adc_reset(){
	snsv6_r18.SAR_ADC_START_CONV = 0x1;	// Release reset
	write_mbus_register(SNS_ADDR,18,snsv6_r18.as_int);
	delay(5000);
}

static void ldo_power_off(){
    snsv6_r17.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
    snsv6_r17.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
    snsv6_r17.CDC_LDO_CDC_LDO_ENB = 0x1;
    snsv6_r17.ADC_LDO_ADC_LDO_ENB = 0x1;
    write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);
    delay(MBUS_DELAY);
}
static void adc_power_off(){
	// FIXME
	snsv6_r18.SAR_ADC_ADC_DOUT_ISO       = 0x1;
	snsv6_r18.SAR_ADC_OSC_ENB            = 0x1;
    write_mbus_register(SNS_ADDR,18,snsv6_r18.as_int);
    delay(MBUS_DELAY);
    ldo_power_off();
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
    
  // Disable Timer
  set_wakeup_timer (0, 0x0, 0x0);
  // Go to sleep without timer
  operation_sleep();

}


static void operation_init(void){
  
    // Set PMU Strength & division threshold
    // Change PMU_CTRL Register
    // PRCv9 Default: 0x8F770049
    // Decrease 5x division switching threshold
    //*((volatile uint32_t *) 0xA200000C) = 0x8F77004B;
	// Increase active pmu clock (for PRCv11)
    *((volatile uint32_t *) 0xA200000C) = 0x8F77184B;
  
    // Speed up GOC frontend to match PMU frequency
    // PRCv9 Default: 0x00202903
    //*((volatile uint32_t *) 0xA2000008) = 0x00202908;
	// Slow down MBUS frequency 
	// Gyouho: This is required for running on the board w/o PMU assist
    *((volatile uint32_t *) 0xA2000008) = 0x00202D08;

  
    delay(1000);
  
    //Enumerate & Initialize Registers
    Pstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    cdc_data_index = 0;
    exec_count = 0;
    exec_count_irq = 0;
    MBus_msg_flag = 0;
    //Enumeration
    enumerate(RAD_ADDR);
    delay(MBUS_DELAY*10);
    enumerate(SNS_ADDR);
    delay(MBUS_DELAY*10);
    enumerate(HRV_ADDR);
    delay(MBUS_DELAY*10);

    // ADC Settings --------------------------------------
	// FIXME
	// SNSv6_R18
	snsv6_r18.SAR_ADC_BYPASS_NUMBER      = 0x4;
	snsv6_r18.SAR_ADC_SINGLE_CONV        = 0x1;
	snsv6_r18.SAR_ADC_SIGNAL_SEL         = 0x3;	//0 for EXT, 3 for 1/8 VBAT
	snsv6_r18.SAR_ADC_EXT_SIGNAL_SEL     = 0x3;
	snsv6_r18.SAR_ADC_OSC_SEL            = 0x3;	//0x05	//0x03	//0=slow, 7=fast
	snsv6_r18.SAR_ADC_OSC_DIV            = 0x1;	//0x01	//0x02	//0=fast, 3=1/8
	write_mbus_register(SNS_ADDR,18,snsv6_r18.as_int);

	// SNSv6_R19
	snsv6_r19.SAR_ADC_FAST_DIV_EN	= 0x1;
	snsv6_r19.SAR_ADC_COMP_CAP_R	= 0x00;
	snsv6_r19.SAR_ADC_COMP_CAP_L	= 0x00;
	write_mbus_register(SNS_ADDR,19,snsv6_r19.as_int);


    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv6_r17.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv6_r17.ADC_LDO_ADC_VREF_SEL     = 0x20;

    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv6_r17.CDC_LDO_CDC_VREF_MUX_SEL = 0x0;
    snsv6_r17.CDC_LDO_CDC_VREF_SEL     = 0x20;

    write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);

    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 3;   // 1: 2-4 sec with PRCv9
    WAKEUP_PERIOD_CONT_INIT = 1;   // 0x1E (30): ~1 min with PRCv9
    cdc_storage_count = 0;
    radio_tx_count = 0;
    radio_tx_option = 0;
    
    // Go to sleep without timer
    //operation_sleep_notimer();
}

static void operation_adc_run(){
    uint32_t read_data;
    uint32_t count; 

	if (Pstack_state == PSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x0);
		#endif
		Pstack_state = PSTK_LDO1;

		cdc_reset_timeout_count = 0;

		//snsv6_r17.CDC_LDO_CDC_LDO_ENB = 0x0;
		snsv6_r17.ADC_LDO_ADC_LDO_ENB = 0x0;
		write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);
		// Long delay required here
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();


	}else if (Pstack_state == PSTK_LDO1){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x1);
		#endif
		Pstack_state = PSTK_ADC_RST;
		snsv6_r17.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
		delay(MBUS_DELAY);
		write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);
		delay(MBUS_DELAY);
		write_mbus_register(SNS_ADDR,17,snsv6_r17.as_int);
		delay(LDO_DELAY); // This delay is required to avoid current spike
		// Put system to sleep
		set_wakeup_timer (WAKEUP_PERIOD_LDO, 0x1, 0x0);
		operation_sleep();
    
	}else if (Pstack_state == PSTK_ADC_RST){
		// Release reset
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x11111111);
			delay(MBUS_DELAY);
		#endif

		// fIXME
		snsv6_r18.SAR_ADC_OSC_ENB = 0;
		snsv6_r18.SAR_ADC_ADC_DOUT_ISO = 0;
		write_mbus_register(SNS_ADDR,18,snsv6_r18.as_int);
		delay(MBUS_DELAY*2);

		release_adc_reset();

		for( count=0; count<CDC_TIMEOUT_COUNT; count++ ){
			if( MBus_msg_flag ){
				MBus_msg_flag = 0;
				cdc_reset_timeout_count = 0;
				Pstack_state = PSTK_ADC_READ;
				return;
			}else{
				delay(MBUS_DELAY);
			}
		}

		// Time out
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0xFAFAFAFA);
		#endif
		// Put system to sleep to reset the layer controller
		set_wakeup_timer (WAKEUP_PERIOD_RESET, 0x1, 0x0);
		operation_sleep();

	}else if (Pstack_state == PSTK_ADC_READ){
		#ifdef DEBUG_MBUS_MSG
			write_mbus_message(0xAA, 0x22222222);
		#endif

		// FIXME
    	uint32_t read_data_adc; 

		read_mbus_register(SNS_ADDR,0x14,0x15);
		delay(MBUS_DELAY);
		read_data_adc = *((volatile uint32_t *) 0xA0001014);

		write_mbus_message(0x74, read_data_adc); // CMEAS

		// FIXME

		if (meas_count < 31000){	
			meas_count++;

			// Repeat measurement while awake
			assert_adc_reset();
			delay(MBUS_DELAY);
			Pstack_state = PSTK_ADC_RST;

		}else{
			meas_count = 0;
			// Finalize CDC operation
			exec_count++;
			assert_adc_reset();
			Pstack_state = PSTK_IDLE;
			#ifdef DEBUG_MBUS_MSG
				write_mbus_message(0xAA, 0x33333333);
			#endif
		
			// Assert CDC isolation & turn off CDC power
			adc_power_off();

			// Enter long sleep
			set_wakeup_timer (WAKEUP_PERIOD_CONT, 0x1, 0x0);
			operation_sleep();
		}

	}else{
        //default:  // THIS SHOULD NOT HAPPEN
		// Reset CDC
		assert_adc_reset();
		adc_power_off();
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
	// FIXME
	config_timer( 0, 0, 0, 0, 1000000 );

    // Initialization sequence
	//FIXME
    if (enumerated != 0xDEADBEEF){
        // Set up PMU/GOC register in PRC layer (every time)
        // Enumeration & RAD/SNS layer register configuration
        exec_count = 0;
		meas_count = 0;
        operation_init();
    }

    // Proceed to continuous mode
    while(1){
      operation_adc_run();
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

