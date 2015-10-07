//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12_FLSv1L
//*******************************************************************
#include "PRCv12.h"
#include "FLSv1L_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLS_ADDR    0x4

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t num_cycle;

//*******************************************************************
// INTERRUPTS
//*******************************************************************
void init_interrupt (void) {
  *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
  *NVIC_ISER = 0x7FFF; //Enable Interrupts
}

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
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void) { // TIMER32
    *NVIC_ICPR = (0x1 << 0);
}
void handler_ext_int_1(void) { // TIMER16
    *NVIC_ICPR = (0x1 << 1);
}
void handler_ext_int_2(void) { // REG0
    *NVIC_ICPR = (0x1 << 2); 
}
void handler_ext_int_3(void) { // REG1
    *NVIC_ICPR = (0x1 << 3); 
}
void handler_ext_int_4(void) { // REG2
    *NVIC_ICPR = (0x1 << 4); 
}
void handler_ext_int_5(void) { // REG3
    *NVIC_ICPR = (0x1 << 5); 
}
void handler_ext_int_6(void) { // REG4
    *NVIC_ICPR = (0x1 << 6); 
}
void handler_ext_int_7(void) { // REG5  
    *NVIC_ICPR = (0x1 << 7); 
}
void handler_ext_int_8(void) { // REG6
    *NVIC_ICPR = (0x1 << 8); 
}
void handler_ext_int_9(void) { // REG7
    *NVIC_ICPR = (0x1 << 9); 
}
void handler_ext_int_10(void) { // MEM WR
    *NVIC_ICPR = (0x1 << 10); 
}
void handler_ext_int_11(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << 11); 
}
void handler_ext_int_12(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << 12); 
}
void handler_ext_int_13(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << 13); 
}
void handler_ext_int_14(void) { // GOCEP
    *NVIC_ICPR = (0x1 << 14); 
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    num_cycle = 0;

    //Chip ID
    write_regfile (REG_CHIP_ID, 0xDEAD);

    // Notify the start of the program
    mbus_write_message32(0xAA, 0xAAAAAAAA);

    //Set Halt
    set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(FLS_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //Initialize Interrupts
    init_interrupt();
  
    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization(); // Enumeration.
    }

    // Display num_cycle;
    mbus_write_message32(0xEF, num_cycle);
    
    // Testing Sequence
    if (num_cycle == 0) {
        mbus_write_message32(0xEF, 0xAAAAAAAA);
    }
    else if (num_cycle == 1) {
        mbus_write_message32(0xEF, 0xBBBBBBBB);
    }
    else {
        mbus_write_message32(0xEF, 0x0EA7F00D);
        while(1);
    }
        
    num_cycle++;

    set_wakeup_timer(5, 1, 1);
    mbus_sleep_all();

    while(1);

    return 1;
}

