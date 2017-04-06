//****************************************************************************************************
//Author:       Gyouho Kim
//Description:  Derived from SNS_cdctest_yielding_longterm_double.c
//				and Tstack_ondemand_v1.15.c
//				- PRCv14 / PMUv2 / RADv9 / SNSv7
//				2/1/2017: PRCv14, cdc timeout handling, cdc polling changed to wfi
//****************************************************************************************************
#include "PRCv14.h"
#include "PRCv14_RF.h"
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
#define SNS2_ADDR 0x6

// CDC parameters
#define	MBUS_DELAY 100 //Amount of delay between successive messages
#define	LDO_DELAY 500 // 1000: 150msec
#define CDC_TIMEOUT_COUNT 400
#define WAKEUP_PERIOD_RESET 1
#define WAKEUP_PERIOD_LDO 1

// Pstack states
#define	PSTK_IDLE       0x0
#define PSTK_CDC_RST    0x1
#define PSTK_CDC2_RST    0x2
#define PSTK_CDC_READ   0x3
#define PSTK_LDO1   	0x4
#define PSTK_LDO2  	 	0x5

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_RSTRL 0x3
#define TSTK_TEMP2_RSTRL 0x13
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP2_START 0x12
#define TSTK_TEMP_READ  0x6
#define TSTK_TEMP2_READ  0x16

#define NUM_TEMP_MEAS 5 

#define CDC_STORAGE_SIZE 0 // FIXME

//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t Tstack_state;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t mbus_msg_flag;
volatile uint32_t wfi_timeout_flag;

volatile snsv7_r0_t snsv7_r0 = SNSv7_R0_DEFAULT;
volatile snsv7_r1_t snsv7_r1 = SNSv7_R1_DEFAULT;
volatile snsv7_r2_t snsv7_r2 = SNSv7_R2_DEFAULT;
volatile snsv7_r18_t snsv7_r18 = SNSv7_R18_DEFAULT;

volatile snsv7_r14_t snsv7_r14 = SNSv7_R14_DEFAULT;
volatile snsv7_r15_t snsv7_r15 = SNSv7_R15_DEFAULT;
volatile snsv7_r25_t snsv7_r25 = SNSv7_R25_DEFAULT;
  
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile uint32_t cdc_storage_cref_latest;
volatile uint32_t cdc_storage_count;
volatile uint32_t cdc_run_single;
volatile uint32_t cdc_running;
volatile uint32_t set_cdc_exec_count;

volatile uint32_t temp_storage_latest = 2000;
volatile uint32_t temp_storage_last_wakeup_adjust = 2000;
volatile uint32_t temp_storage_diff = 0;
volatile uint32_t temp_storage_count;
volatile uint32_t temp_run_single;
volatile uint32_t temp_running;
volatile uint32_t set_temp_exec_count;
volatile uint32_t read_data_reg11; // [23:0] Temp Sensor D Out

volatile uint32_t radio_tx_count;
volatile uint32_t radio_tx_option;
volatile uint32_t radio_tx_numdata;
volatile uint32_t radio_ready;
volatile uint32_t radio_on;

volatile prcv14_r0B_t prcv14_r0B = PRCv14_R0B_DEFAULT;

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

void handler_ext_int_0(void)  { *NVIC_ICPR = (0x1 << 0); wfi_timeout_flag = 1;} // TIMER32
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
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
    snsv7_r0.CDCW_PG_VBAT = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
    snsv7_r0.CDCW_PG_VLDO = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_isolate(){
    snsv7_r0.CDCW_ISO = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void assert_cdc_reset(){
    snsv7_r0.CDCW_RESETn = 0x0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc_reset(){
    snsv7_r0.CDCW_RESETn = 0x1;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
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
static void fire_cdc2_meas(){
    snsv7_r0.CDCW_ENABLE = 0x1;
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void release_cdc2_meas(){
    snsv7_r0.CDCW_ENABLE = 0x0;
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
}
static void ldo_power_off(){
    snsv7_r18.CDC_LDO_CDC_LDO_DLY_ENB = 0x1;
    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x1;
    snsv7_r18.CDC_LDO_CDC_LDO_ENB = 0x1;
    snsv7_r18.ADC_LDO_ADC_LDO_ENB = 0x1;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
    mbus_remote_register_write(SNS2_ADDR,18,snsv7_r18.as_int);
    delay(MBUS_DELAY);
}
static void cdc_power_off(){
    snsv7_r0.CDCW_ISO = 0x1;
    snsv7_r0.CDCW_PG_V1P2 = 0x1;
    snsv7_r0.CDCW_PG_VBAT = 0x1;
    snsv7_r0.CDCW_PG_VLDO = 0x1;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
    delay(MBUS_DELAY);
    ldo_power_off();
}

static void temp_sensor_enable(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_ENABLEb = 0x0;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor2_enable(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_ENABLEb = 0x0;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS2_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_disable(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_ENABLEb = 1;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor2_disable(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_ENABLEb = 1;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS2_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_release_reset(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_RESETn = 1;
	snsv7_r14_temp.TEMP_SENSOR_ISO = 0;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	mbus_remote_register_write(SNS2_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_sensor_assert_reset(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_RESETn = 0;
	snsv7_r14_temp.TEMP_SENSOR_ISO = 1;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	mbus_remote_register_write(SNS2_ADDR,0xE,snsv7_r14.as_int);
}
static void temp_power_off(){
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_ENABLEb = 1;
	snsv7_r14_temp.TEMP_SENSOR_RESETn = 0;
	snsv7_r14_temp.TEMP_SENSOR_ISO = 1;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	mbus_remote_register_write(SNS2_ADDR,0xE,snsv7_r14.as_int);
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
  
	snsv7_r14_t snsv7_r14_temp;
	snsv7_r15_t snsv7_r15_temp;
	snsv7_r18_t snsv7_r18_temp;
	snsv7_r25_t snsv7_r25_temp;

	// Set CPU & Mbus Clock Speeds
    prcv14_r0B.DSLP_CLK_GEN_FAST_MODE = 0x1; // Default 0x0
    prcv14_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv14_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv14_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
	*((volatile uint32_t *) REG_CLKGEN_TUNE ) = prcv14_r0B.as_int;
  
    delay(1000);
  
    // Disable MBus Watchdog Timer
    //*REG_MBUS_WD = 0;
	*((volatile uint32_t *) 0xA000007C) = 0;

    //Enumerate & Initialize Registers
    Tstack_state = PSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEEF;
    exec_count = 0;
    exec_count_irq = 0;
    mbus_msg_flag = 0;

    //Enumeration
    //mbus_enumerate(RAD_ADDR);
    //delay(MBUS_DELAY*10);
    mbus_enumerate(SNS_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(SNS2_ADDR);
    delay(MBUS_DELAY);

    // Temp Sensor Settings --------------------------------------
	// SNSv7_R25
	snsv7_r25_temp.as_int = snsv7_r25.as_int;
	snsv7_r25_temp.TEMP_SENSOR_IRQ_PACKET = 0x001000;
	snsv7_r25.as_int = snsv7_r25_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0x19,snsv7_r25.as_int);
	mbus_remote_register_write(SNS2_ADDR,0x19,snsv7_r25.as_int);
	// SNSv7_R14
	snsv7_r14_temp.as_int = snsv7_r14.as_int;
	snsv7_r14_temp.TEMP_SENSOR_BURST_MODE = 0x0;
	snsv7_r14_temp.TEMP_SENSOR_DELAY_SEL = 5;
	snsv7_r14_temp.TEMP_SENSOR_R_tmod = 0x0;
	snsv7_r14_temp.TEMP_SENSOR_R_bmod = 0x0;
	snsv7_r14.as_int = snsv7_r14_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xE,snsv7_r14.as_int);
	mbus_remote_register_write(SNS2_ADDR,0xE,snsv7_r14.as_int);
	// snsv7_R15
	snsv7_r15_temp.as_int = snsv7_r15.as_int;
	snsv7_r15_temp.TEMP_SENSOR_AMP_BIAS   = 0x7; // Default: 2
	snsv7_r15_temp.TEMP_SENSOR_CONT_MODEb = 0x0;
	snsv7_r15_temp.TEMP_SENSOR_SEL_CT     = 6;
	snsv7_r15.as_int = snsv7_r15_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,0xF,snsv7_r15.as_int);
	mbus_remote_register_write(SNS2_ADDR,0xF,snsv7_r15.as_int);

	// snsv7_R18
	snsv7_r18_temp.as_int = snsv7_r18.as_int;
	snsv7_r18_temp.ADC_LDO_ADC_LDO_ENB      = 0x1;
	snsv7_r18_temp.ADC_LDO_ADC_LDO_DLY_ENB  = 0x1;
	snsv7_r18_temp.ADC_LDO_ADC_CURRENT_2X   = 0x1;
	snsv7_r18_temp.ADC_LDO_ADC_VREF_MUX_SEL = 0x3; // Set ADC LDO to around 1.37V: 0x3//0x20
	snsv7_r18_temp.ADC_LDO_ADC_VREF_SEL     = 0x20; // Set ADC LDO to around 1.37V: 0x3//0x20
	snsv7_r18.as_int = snsv7_r18_temp.as_int;
	mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
	mbus_remote_register_write(SNS2_ADDR,18,snsv7_r18.as_int);

    // CDC Settings --------------------------------------
    // snsv7_r0
    snsv7_r0.CDCW_IRQ_EN	= 1;
    snsv7_r0.CDCW_MODE_PAR	= 1; // Default: 1
    snsv7_r0.CDCW_RESETn 	= 0;
    mbus_remote_register_write(SNS_ADDR,0,snsv7_r0.as_int);
    mbus_remote_register_write(SNS2_ADDR,0,snsv7_r0.as_int);
  
    // snsv7_r1
    snsv7_r1.CDCW_N_CYCLE_SINGLE	= 0; // Default: 8; Min: 0
    mbus_remote_register_write(SNS_ADDR,1,snsv7_r1.as_int);
    mbus_remote_register_write(SNS2_ADDR,1,snsv7_r1.as_int);
  
    // snsv7_r2
    snsv7_r2.CDCW_N_CYCLE_SET	= 200; // Min: 0
    mbus_remote_register_write(SNS_ADDR,2,snsv7_r2.as_int);
    mbus_remote_register_write(SNS2_ADDR,2,snsv7_r2.as_int);
  
    // snsv7_r18
    snsv7_r18.CDC_LDO_CDC_CURRENT_2X  = 1;
  
    // Set ADC LDO to around 1.37V: 0x3//0x20
    snsv7_r18.ADC_LDO_ADC_VREF_MUX_SEL = 0x3;
    snsv7_r18.ADC_LDO_ADC_VREF_SEL     = 0x20;
  
    // Set CDC LDO to around 1.03V: 0x0//0x20
    snsv7_r18.CDC_LDO_CDC_VREF_MUX_SEL = 0x2;
    snsv7_r18.CDC_LDO_CDC_VREF_SEL     = 0x20;
  
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
    mbus_remote_register_write(SNS2_ADDR,18,snsv7_r18.as_int);
  
	// Mbus return address; Needs to be between 0x18-0x1F
    mbus_remote_register_write(SNS_ADDR,0x18,0x1800);
    mbus_remote_register_write(SNS2_ADDR,0x18,0x1800);

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

static void operation_temp_run(void){
    snsv7_r18_t snsv7_r18_temp;

	if (Tstack_state == TSTK_IDLE){
		#ifdef DEBUG_MBUS_MSG 
			mbus_write_message32(0xBB, 0xFBFB0000);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_LDO;

		meas_count = 0;
		wfi_timeout_flag = 0;

		snsv7_r18_temp.as_int = snsv7_r18.as_int;
		snsv7_r18_temp.ADC_LDO_ADC_LDO_ENB = 0x0;
		snsv7_r18.as_int = snsv7_r18_temp.as_int;
		mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
		mbus_remote_register_write(SNS2_ADDR,18,snsv7_r18.as_int);

		// Put system to sleep
		set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
		operation_sleep_noirqreset();

    }else if (Tstack_state == TSTK_LDO){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xBB, 0xFBFB1111);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_TEMP_RSTRL;
		snsv7_r18_temp.as_int = snsv7_r18.as_int;
		snsv7_r18_temp.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
		snsv7_r18.as_int = snsv7_r18_temp.as_int;
		mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
		mbus_remote_register_write(SNS2_ADDR,18,snsv7_r18.as_int);
		// Put system to sleep
		set_wakeup_timer(WAKEUP_PERIOD_LDO, 0x1, 0x1);
		operation_sleep_noirqreset();

	}else if (Tstack_state == TSTK_TEMP_RSTRL){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xBB, 0xFBFB2222);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_TEMP_READ;

		// Release Temp Sensor Reset
		temp_sensor_release_reset();
		delay(MBUS_DELAY);
			
		// Start Temp Sensor
		temp_sensor_enable();

		// Put system to sleep
		set_wakeup_timer(20, 0x1, 0x1); // FIXME timeout value should be set
		operation_sleep_noirqreset();

	}else if (Tstack_state == TSTK_TEMP2_RSTRL){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xBB, 0xFBFB2222);
			delay(MBUS_DELAY*10);
		#endif
		Tstack_state = TSTK_TEMP2_READ;

		// Start Temp Sensor
		temp_sensor2_enable();

		// Put system to sleep
		set_wakeup_timer(20, 0x1, 0x1); // FIXME timeout value should be set
		operation_sleep_noirqreset();

	}else if (Tstack_state == TSTK_TEMP_START){
	// Start temp measurement
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xBB, 0xFBFB3333);
		delay(MBUS_DELAY*10);
	#endif

		mbus_msg_flag = 0;

		// Start Temp Sensor
		temp_sensor_enable();

		// Use Timer32 as timeout counter
		config_timer32(0x249F0, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Wait for temp sensor output
		WFI();

		// Turn off Timer32
		*TIMER32_GO = 0;
		Tstack_state = TSTK_TEMP_READ;

	}else if (Tstack_state == TSTK_TEMP2_START){
	// Start temp measurement
	#ifdef DEBUG_MBUS_MSG
		mbus_write_message32(0xBB, 0xFBFB3333);
		delay(MBUS_DELAY*10);
	#endif

		mbus_msg_flag = 0;

		// Start Temp Sensor
		temp_sensor2_enable();

		// Use Timer32 as timeout counter
		config_timer32(0x249F0, 1, 0, 0); // 1/10 of MBUS watchdog timer default

		// Wait for temp sensor output
		WFI();

		// Turn off Timer32
		*TIMER32_GO = 0;
		Tstack_state = TSTK_TEMP2_READ;

	}else if (Tstack_state == TSTK_TEMP_READ){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xBB, 0xFBFB4444);
			delay(MBUS_DELAY*10);
		#endif

		// Grab Temp Sensor Data
		if (wfi_timeout_flag){
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			read_data_reg11 = *((volatile uint32_t *) 0xA0000000);
		}
		meas_count++;

		// Last measurement from this wakeup
		if (meas_count == NUM_TEMP_MEAS){
			// No error; see if there was a timeout
			if (wfi_timeout_flag){
				temp_storage_latest = 0x666;
				wfi_timeout_flag = 0;
			}else{
				temp_storage_latest = read_data_reg11;
				mbus_write_message32(0x74, temp_storage_latest);
			}
		}

		// Option to take multiple measurements per wakeup
		if (meas_count < NUM_TEMP_MEAS){	
			// Repeat measurement while awake
			temp_sensor_disable();
			Tstack_state = TSTK_TEMP_START;
				
		}else{
			meas_count = 0;

			temp_sensor_disable();
			Tstack_state = TSTK_TEMP2_RSTRL;
		}

	}else if (Tstack_state == TSTK_TEMP2_READ){
		#ifdef DEBUG_MBUS_MSG
			mbus_write_message32(0xBB, 0xFBFB4444);
			delay(MBUS_DELAY*10);
		#endif

		// Grab Temp Sensor Data
		if (wfi_timeout_flag){
			mbus_write_message32(0xFA, 0xFAFAFAFA);
		}else{
			read_data_reg11 = *((volatile uint32_t *) 0xA0000000);
		}
		meas_count++;

		// Last measurement from this wakeup
		if (meas_count == NUM_TEMP_MEAS){
			// No error; see if there was a timeout
			if (wfi_timeout_flag){
				temp_storage_latest = 0x666;
				wfi_timeout_flag = 0;
			}else{
				temp_storage_latest = read_data_reg11;
				mbus_write_message32(0x7D, temp_storage_latest);

			}
		}

		// Option to take multiple measurements per wakeup
		if (meas_count < NUM_TEMP_MEAS){	
			// Repeat measurement while awake
			temp_sensor2_disable();
			Tstack_state = TSTK_TEMP2_START;
				
		}else{
			meas_count = 0;

			temp_sensor2_disable();

			// Assert temp sensor isolation & turn off temp sensor power
			temp_power_off();
			Tstack_state = TSTK_IDLE;

			exec_count++;

			set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
			operation_sleep_noirqreset();

		}


    }else{
        //default:  // THIS SHOULD NOT HAPPEN
		// Reset Temp Sensor 
		temp_sensor_assert_reset();
		temp_power_off();
		operation_sleep_notimer();
    }

}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
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
		exec_count = 0;

    }

    // Proceed to continuous mode
    while(1){
      operation_temp_run();
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

