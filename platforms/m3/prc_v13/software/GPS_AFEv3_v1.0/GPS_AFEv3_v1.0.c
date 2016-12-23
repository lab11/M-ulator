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

    snsv7_r18.ADC_LDO_ADC_LDO_DLY_ENB = 0x0;
    mbus_remote_register_write(SNS_ADDR,18,snsv7_r18.as_int);
    // Release Temp Sensor Reset
    temp_sensor_release_reset();

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


