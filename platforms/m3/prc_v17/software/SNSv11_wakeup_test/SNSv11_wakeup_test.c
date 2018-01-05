//*******************************************************************
//Author: Gyouho Kim
//Description: Simple code that repeats sleep/wakeup
//*******************************************************************
#include "PRCv17.h"
#include "PRCv17_RF.h"
#include "mbus.h"
#include "SNSv11_RF.h"
#include "HRVv5.h"
#include "RADv9.h"
#include "PMUv7_RF.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG_1

// TStack order  PRC->RAD->SNS->HRV->PMU
#define HRV_ADDR 0x3
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define PMU_ADDR 0x6

#define WAKEUP_PERIOD_PARKING 30000 // About 2 hours (PRCv17)

// Temp Sensor parameters
#define	MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms
#define WAKEUP_PERIOD_LDO 5 // About 1 sec (PRCv17)
#define TEMP_CYCLE_INIT 3 

// Tstack states
#define	TSTK_IDLE       0x0
#define	TSTK_LDO        0x1
#define TSTK_TEMP_START 0x2
#define TSTK_TEMP_READ  0x6

#define NUM_TEMP_MEAS 2

// Radio configurations
#define RADIO_DATA_LENGTH 24
#define WAKEUP_PERIOD_RADIO_INIT 10 // About 2 sec (PRCv17)

#define TEMP_STORAGE_SIZE 600 // Need to leave about 500 Bytes for stack --> around 120

#define TIMERWD_VAL 0xFFFFF // 0xFFFFF about 13 sec with Y5 run default clock (PRCv17)
#define TIMER32_VAL 0x50000 // 0x20000 about 1 sec with Y5 run default clock (PRCv17)

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t Tstack_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t meas_count;
volatile uint32_t exec_count_irq;
volatile uint32_t wakeup_period_count;
volatile uint32_t wakeup_timer_multiplier;
volatile uint32_t PMU_ADC_4P2_VAL;
volatile uint32_t pmu_parkinglot_mode;
volatile uint32_t pmu_harvesting_on;

volatile uint32_t WAKEUP_PERIOD_CONT_USER; 
volatile uint32_t WAKEUP_PERIOD_CONT; 
volatile uint32_t WAKEUP_PERIOD_CONT_INIT; 

volatile prcv17_r0B_t prcv17_r0B = PRCv17_R0B_DEFAULT;

volatile snsv11_r00_t snsv11_r00 = SNSv11_R00_DEFAULT;
volatile snsv11_r01_t snsv11_r01 = SNSv11_R01_DEFAULT;
volatile snsv11_r03_t snsv11_r03 = SNSv11_R03_DEFAULT;
volatile snsv11_r17_t snsv11_r17 = SNSv11_R17_DEFAULT;

//*******************************************************************
// INTERRUPT HANDLERS (Updated for PRCv17)
//*******************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
	wfi_timeout_flag = 1;
    }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); 
    *SREG_WAKEUP_SOURCE = 0;
}


//***************************************************
// End of Program Sleep Operation
//***************************************************

static void operation_sleep(void){

	// Reset GOC_DATA_IRQ
	*GOC_DATA_IRQ = 0;

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
    
	// Disable Timer
	set_wakeup_timer(0, 0, 0);

    // Go to sleep
    operation_sleep();

}


static void operation_init(void){
  
	// Set CPU & Mbus Clock Speeds
    prcv17_r0B.CLK_GEN_RING = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_MBC = 0x1; // Default 0x1
    prcv17_r0B.CLK_GEN_DIV_CORE = 0x3; // Default 0x3
    prcv17_r0B.GOC_CLK_GEN_SEL_DIV = 0x0; // Default 0x0
    prcv17_r0B.GOC_CLK_GEN_SEL_FREQ = 0x6; // Default 0x6
	*REG_CLKGEN_TUNE = prcv17_r0B.as_int;

  
    //Enumerate & Initialize Registers
    Tstack_state = TSTK_IDLE; 	//0x0;
    enumerated = 0xDEADBEE0;
    exec_count = 0;
    exec_count_irq = 0;
	PMU_ADC_4P2_VAL = 0x4B;
	pmu_parkinglot_mode = 0;
	pmu_harvesting_on = 1;
  
    // Set CPU Halt Option as RX --> Use for register read e.g.
    //set_halt_until_mbus_rx();

    //Enumeration
    //mbus_enumerate(RAD_ADDR);
	//delay(MBUS_DELAY);
    mbus_enumerate(SNS_ADDR);
	delay(MBUS_DELAY);
    //mbus_enumerate(HRV_ADDR);
	//delay(MBUS_DELAY);
 	//mbus_enumerate(PMU_ADDR);
	//delay(MBUS_DELAY);

    // Set CPU Halt Option as TX --> Use for register write e.g.
	//set_halt_until_mbus_tx();


    // Temp Sensor Settings --------------------------------------

    // snsv11_r01
	snsv11_r01.TSNS_RESETn = 0;
    snsv11_r01.TSNS_EN_IRQ = 1;
    snsv11_r01.TSNS_BURST_MODE = 0;
    snsv11_r01.TSNS_CONT_MODE = 0;
    mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	// Set temp sensor conversion time
	snsv11_r03.TSNS_SEL_STB_TIME = 0x1; 
	snsv11_r03.TSNS_SEL_CONV_TIME = 0x3; // Default: 0x6
	mbus_remote_register_write(SNS_ADDR,0x03,snsv11_r03.as_int);



    // Initialize other global variables
    WAKEUP_PERIOD_CONT = 100;   // 10: 2-4 sec with PRCv17
	wakeup_data = 0;

    delay(MBUS_DELAY);

    // Go to sleep
	//set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
    //operation_sleep();
}

static void sns_temp_test(void){

	// Read temp result
	set_halt_until_mbus_rx();
	mbus_remote_register_read(SNS_ADDR,0x6,1);
	set_halt_until_mbus_tx();
	mbus_write_message32(0xC0,*REG1);

	// Reset the counter and temp sensor
	snsv11_r01.TSNS_RESETn = 0;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	// Set temp sensor conversion time 
	snsv11_r03.TSNS_SEL_CONV_TIME = 0x6;
	mbus_remote_register_write(SNS_ADDR,0x03,snsv11_r03.as_int);

	// Turn on SNS LDO
	// snsv11_r00
	//snsv11_r00.LDO_VREF_I_AMP	= 0;
	//snsv11_r00.LDO_SEL_TSNS	= 4;
	//snsv11_r00.LDO_SEL_CDC 	= 4;
	snsv11_r00.LDO_EN_VREF 	= 1;
	mbus_remote_register_write(SNS_ADDR,0,snsv11_r00.as_int);
	delay(MBUS_DELAY*10);

	snsv11_r00.LDO_EN_IREF 	= 1;
	snsv11_r00.LDO_EN_TSNS_OUT	= 1; // Default: 1
	mbus_remote_register_write(SNS_ADDR,0,snsv11_r00.as_int);
	delay(MBUS_DELAY*10);

	// Turn on digital block
	snsv11_r01.TSNS_SEL_LDO = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	// Turn on analog block
	snsv11_r01.TSNS_EN_SENSOR_LDO = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);
	snsv11_r01.TSNS_ISOLATE = 0;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);
	delay(MBUS_DELAY*10);

	// Disable all timers
	//disable_timerwd();
	//*REG_MBUS_WD = 0;

	mbus_write_message32(0xAA,0xABCD1111);

	// Start temp measurement
	snsv11_r01.TSNS_RESETn = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	operation_sleep();

	WFI();

	mbus_write_message32(0xAA,0xABCD2222);

}

static void sns_wakeup_timer_test(void){

	// SNS Wakeup Timer Settings 
	snsv11_r17.WUP_ENABLE = 1;
	snsv11_r17.WUP_AUTO_RESET = 1;
	snsv11_r17.WUP_INT_RPLY_REG_ADDR = 0x02;
	mbus_remote_register_write(SNS_ADDR,0x17,snsv11_r17.as_int);

	// SNS Wakeup Timer Threshold
	mbus_remote_register_write(SNS_ADDR,0x19,0); // Extended
	mbus_remote_register_write(SNS_ADDR,0x1A,24000);

	// Turn on continuous mode
    snsv11_r01.TSNS_CONT_MODE = 1;
    mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	// Turn on SNS LDO
	// snsv11_r00
	//snsv11_r00.LDO_VREF_I_AMP	= 0;
	//snsv11_r00.LDO_SEL_TSNS	= 4;
	//snsv11_r00.LDO_SEL_CDC 	= 4;
	snsv11_r00.LDO_EN_VREF 	= 1;
	mbus_remote_register_write(SNS_ADDR,0,snsv11_r00.as_int);
	delay(MBUS_DELAY*10);

	snsv11_r00.LDO_EN_IREF 	= 1;
	snsv11_r00.LDO_EN_TSNS_OUT	= 1; // Default: 1
	snsv11_r00.LDO_EN_CDC_OUT	= 0;
	mbus_remote_register_write(SNS_ADDR,0,snsv11_r00.as_int);
	delay(MBUS_DELAY*10);

	// Turn on digital block
	snsv11_r01.TSNS_SEL_LDO = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	// Turn on analog block
	snsv11_r01.TSNS_EN_SENSOR_LDO = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);
	snsv11_r01.TSNS_ISOLATE = 0;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);
	delay(MBUS_DELAY*10);

	// Set temp sensor conversion time 
	snsv11_r03.TSNS_SEL_CONV_TIME = 0x0;
	mbus_remote_register_write(SNS_ADDR,0x03,snsv11_r03.as_int);

	// Start temp measurement
	snsv11_r01.TSNS_RESETn = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	WFI();

	mbus_write_message32(0xAA,0xABCD2222);

	snsv11_r01.TSNS_RESETn = 0;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	// Disable all timers
	//disable_timerwd();
	//*REG_MBUS_WD = 0;

	// Start clock
	snsv11_r01.TSNS_EN_CLK_REF = 1;
	mbus_remote_register_write(SNS_ADDR,1,snsv11_r01.as_int);

	operation_sleep();

	WFI();


}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

	// Wakeup timer value
	//mbus_write_message32(0xAA,*REG_WUPT_VAL);

    // Only enable relevant interrupts (PRCv17)
	//enable_reg_irq();
	//enable_all_irq();
	*NVIC_ISER = (1 << IRQ_WAKEUP) | (1 << IRQ_GOCEP) | (1 << IRQ_TIMER32) | (1 << IRQ_REG0)| (1 << IRQ_REG1)| (1 << IRQ_REG2)| (1 << IRQ_REG3);
  
    // Config watchdog timer to about 10 sec; default: 0x02FFFFFF
    config_timerwd(TIMERWD_VAL);

    // Initialization sequence
    if (enumerated != 0xDEADBEE0){
        operation_init();
		sns_wakeup_timer_test();
    }

	set_halt_until_mbus_tx();
	mbus_write_message32(0xAA,0xABCD1234);
    delay(MBUS_DELAY);


	//sns_temp_test();


	//set_wakeup_timer(WAKEUP_PERIOD_CONT, 0x1, 0x1);
	//mbus_write_message32(0xFF,*REG_WUPT_VAL);
    operation_sleep();

    while(1);
}


