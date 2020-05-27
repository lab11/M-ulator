//*******************************************************************
//Author: Inhee Lee
//Description: Change the conversion ratio of HRVv8
// PRCv18 -> HRVv8
//*******************************************************************
#include "PRCv18.h"
#include "HRVv5.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define HRV_ADDR    0x3

#define MBUS_DELAY 100 // Amount of delay between successive messages; 100: 6-7ms

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t wakeup_data;
volatile uint32_t wfi_timeout_flag;
volatile hrvv5_r0_t hrvv5_r0 = HRVv5_R0_DEFAULT;

//*******************************************************************
// INTERRUPT HANDLERS
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
    // Report who woke up
    delay(MBUS_DELAY);
    mbus_write_message32(0xAA,*SREG_WAKEUP_SOURCE); // 0x1: GOC; 0x2: PRC Timer; 0x10: SNT
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0x54436020;

    // Set Halt
    set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(HRV_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();

    mbus_write_message32(0xDD,0xAB0101);
    delay(MBUS_DELAY*10);

    // Go to sleep
    set_wakeup_timer(0, 0, 0);
    mbus_sleep_all();
    while(1);
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
  
    // Initialization sequence
    if (enumerated != 0x54436020){
        initialization();
    }

    mbus_write_message32(0xDD,0xAB0202);
    delay(MBUS_DELAY*10);

    // Check if wakeup is due to GOC interrupt  
    // 0x8C is reserved for GOC-triggered wakeup (Named GOC_DATA_IRQ)
    // 8 MSB bits of the wakeup data are used for function ID
    wakeup_data = *GOC_DATA_IRQ;
    uint32_t wakeup_data_header = (wakeup_data>>24) & 0xFF;
    uint32_t wakeup_data_field_0 = wakeup_data & 0xFF;

    if(wakeup_data_header == 0xDD){
        // Change the conversion ratio of HRV
        // wakeup_data[7:0] is the conversion ratio of HRV. '+9' is the actual value. a
    	hrvv5_r0.HRV_TOP_CONV_RATIO = wakeup_data_field_0;
    	mbus_remote_register_write(HRV_ADDR,0,hrvv5_r0.as_int);
    	delay(MBUS_DELAY*10);
        mbus_write_message32(0xDD,wakeup_data);
    	delay(MBUS_DELAY*10);
	}

    // Go to sleep
    set_wakeup_timer(0, 0, 0);
    mbus_sleep_all();
    while(1);

    return 1;
}
