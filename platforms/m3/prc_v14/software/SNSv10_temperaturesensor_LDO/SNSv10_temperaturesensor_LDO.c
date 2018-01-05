//****************************************************************************************************
//Author:       Seokhyoen Jeong
//Description:  temperature sensor test
//				- PRCv14 / PMUv4 / SNSv10
//****************************************************************************************************
#include "PRCv14.h"
#include "PRCv14_RF.h"
#include "mbus.h"
#include "SNSv10_RF.h"

// uncomment this for debug mbus message
#define DEBUG_MBUS_MSG
// uncomment this for debug radio message
//#define DEBUG_RADIO_MSG

// uncomment this to only transmit average
//#define TX_AVERAGE

// Stack order  PRC->SNS->PMU
#define SNS_ADDR 0x4
#define PMU_ADDR 0x6

// CDC parameters
#define	MBUS_DELAY 100 //Amount of delay between successive messages

//***************************************************
// Global variables
//***************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;
volatile uint32_t mbus_msg_flag;
volatile uint32_t cdc_timeout_flag;

volatile snsv10_r00_t snsv10_r00 = SNSv10_R00_DEFAULT;
volatile snsv10_r01_t snsv10_r01 = SNSv10_R01_DEFAULT;
volatile snsv10_r03_t snsv10_r03 = SNSv10_R03_DEFAULT;
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

void handler_ext_int_0(void)  { *NVIC_ICPR = (0x1 << 0);  cdc_timeout_flag = 1;} // TIMER32
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
    
    // Disable Timer
    set_wakeup_timer(0, 0, 0);

    // Go to sleep without timer
    delay(MBUS_DELAY);
    operation_sleep();

}


static void operation_init(void){
  
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
    enumerated = 0xDEADBEEF;
    mbus_msg_flag = 0;

    //Enumeration
    //delay(MBUS_DELAY*10);
    mbus_enumerate(SNS_ADDR);
    delay(MBUS_DELAY);
    mbus_enumerate(PMU_ADDR);
    //delay(MBUS_DELAY);

    
    // Go to sleep without timer
    //operation_sleep_notimer();
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

    }

//    snsv10_r03.TSNS_SEL_STB_TIME	= 1;
//    snsv10_r03.TSNS_SEL_CONV_TIME	= 3;
    snsv10_r03.TSNS_SEL_VVDD	= 14;

    mbus_remote_register_write(SNS_ADDR,3,snsv10_r03.as_int);
//	delay(MBUS_DELAY*100);

    // snsv10_r00
    //snsv10_r00.LDO_VREF_I_AMP	= 0;
    //snsv10_r00.LDO_SEL_TSNS	= 4;
    //snsv10_r00.LDO_SEL_CDC 	= 4;
    snsv10_r00.LDO_EN_VREF 	= 1;

	mbus_remote_register_write(SNS_ADDR,0,snsv10_r00.as_int);
//	delay(MBUS_DELAY*10);

    snsv10_r00.LDO_EN_IREF 	= 1;
    snsv10_r00.LDO_EN_TSNS_OUT	= 1; 

    mbus_remote_register_write(SNS_ADDR,0,snsv10_r00.as_int);
    //delay(MBUS_DELAY*1);

    snsv10_r01.TSNS_SEL_LDO = 1;
    snsv10_r01.TSNS_BURST_MODE = 0;
    snsv10_r01.TSNS_CONT_MODE = 1;
    snsv10_r01.TSNS_EN_MONITOR = 1;

    mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);

    snsv10_r01.TSNS_EN_SENSOR_LDO = 1;
    //snsv10_r01.TSNS_EN_SENSOR_V1P2 = 1;
    mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);

	uint32_t ii;
	for(ii=0; ii<100; ii++){

		snsv10_r01.TSNS_RESETn = 1;
		snsv10_r01.TSNS_ISOLATE = 0;
		snsv10_r01.TSNS_EN_IRQ = 1;
		mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);

		WFI();

		mbus_write_message32(0xBB, *REG0);

		snsv10_r01.TSNS_RESETn = 0;
		mbus_remote_register_write(SNS_ADDR,1,snsv10_r01.as_int);
	    delay(MBUS_DELAY);
	}

    // Proceed to continuous mode
    while(1){
        delay(1000);
    }

    // Should not reach here
    operation_sleep_notimer();

    while(1);
}

