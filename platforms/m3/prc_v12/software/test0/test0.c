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
  //*NVIC_ISER = 0x7FFF; //Enable Interrupts
  *NVIC_ISER = 0x0; //Disable Interrupts
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
    *REG0 = 0x1000;
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    set_halt_until_mbus_tx();
    mbus_write_message32(0xE0, *REG0); // 0x1000
    mbus_write_message32(0xE1, *REG1); // TIMER32_CNT
    mbus_write_message32(0xE2, *REG2); // TIMER32_STAT
    }
void handler_ext_int_1(void) { // TIMER16
    *NVIC_ICPR = (0x1 << 1);
    *REG0 = 0x1001;
    *REG1 = *TIMER16_CNT;
    *REG2 = *TIMER16_STAT;
    *TIMER16_STAT = 0x0;
    set_halt_until_mbus_tx();
    mbus_write_message32(0xE0, *REG0); // 0x1001
    mbus_write_message32(0xE1, *REG1); // TIMER16_CNT
    mbus_write_message32(0xE2, *REG2); // TIMER16_STAT
    }
void handler_ext_int_2(void) { // REG0
    *NVIC_ICPR = (0x1 << 2); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000002);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_3(void) { // REG1
    *NVIC_ICPR = (0x1 << 3); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000003);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_4(void) { // REG2
    *NVIC_ICPR = (0x1 << 4); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000004);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_5(void) { // REG3
    *NVIC_ICPR = (0x1 << 5); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000005);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_6(void) { // REG4
    *NVIC_ICPR = (0x1 << 6); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000006);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_7(void) { // REG5  
    *NVIC_ICPR = (0x1 << 7); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000007);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_8(void) { // REG6
    *NVIC_ICPR = (0x1 << 8); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000008);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_9(void) { // REG7
    *NVIC_ICPR = (0x1 << 9); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000009);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_10(void) { // MEM WR
    *NVIC_ICPR = (0x1 << 10); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000010);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_11(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << 11); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000011);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_12(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << 12); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000012);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_13(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << 13); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000013);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_14(void) { // GOCEP
    *NVIC_ICPR = (0x1 << 14); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000014);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    num_cycle = 0;

    //Chip ID
    *REG0 = 0xFFFFFFFF;
    *REG1 = 0xFFFFFFFF;
    *REG2 = 0xFFFFFFFF;
    *REG3 = 0xFFFFFFFF;
    *REG4 = 0xFFFFFFFF;
    *REG5 = 0xFFFFFFFF;
    *REG6 = 0xFFFFFFFF;
    *REG7 = 0xFFFFFFFF;

    mbus_write_message32(0xEF, 0xDEADBEEF);

    mbus_write_message32(0xE0, *REG0);
    mbus_write_message32(0xE1, *REG1);
    mbus_write_message32(0xE2, *REG2);
    mbus_write_message32(0xE3, *REG3);
    mbus_write_message32(0xE4, *REG4);
    mbus_write_message32(0xE5, *REG5);
    mbus_write_message32(0xE6, *REG6);
    mbus_write_message32(0xE7, *REG7);

    mbus_write_message32(0xEF, 0xDEADBEEF);

    //Chip ID
    *REG0 = 0x00000000;
    *REG1 = 0x00000000;
    *REG2 = 0x00000000;
    *REG3 = 0x00000000;
    *REG4 = 0x00000000;
    *REG5 = 0x00000000;
    *REG6 = 0x00000000;
    *REG7 = 0x00000000;

    mbus_write_message32(0xEF, 0xDEADBEEF);

    mbus_write_message32(0xE0, *REG0);
    mbus_write_message32(0xE1, *REG1);
    mbus_write_message32(0xE2, *REG2);
    mbus_write_message32(0xE3, *REG3);
    mbus_write_message32(0xE4, *REG4);
    mbus_write_message32(0xE5, *REG5);
    mbus_write_message32(0xE6, *REG6);
    mbus_write_message32(0xE7, *REG7);

    mbus_write_message32(0xEF, 0xDEADBEEF);

    //Chip ID
    *REG0 = 0xFFFFFFFF;
    *REG1 = 0xFFFFFFFF;
    *REG2 = 0xFFFFFFFF;
    *REG3 = 0xFFFFFFFF;
    *REG4 = 0xFFFFFFFF;
    *REG5 = 0xFFFFFFFF;
    *REG6 = 0xFFFFFFFF;
    *REG7 = 0xFFFFFFFF;

    mbus_write_message32(0xEF, 0xDEADBEEF);

    mbus_write_message32(0xE0, *REG0);
    mbus_write_message32(0xE1, *REG1);
    mbus_write_message32(0xE2, *REG2);
    mbus_write_message32(0xE3, *REG3);
    mbus_write_message32(0xE4, *REG4);
    mbus_write_message32(0xE5, *REG5);
    mbus_write_message32(0xE6, *REG6);
    mbus_write_message32(0xE7, *REG7);

    mbus_write_message32(0xEF, 0xDEADBEEF);

    //Chip ID
    *REG0 = 0x00000000;
    *REG1 = 0x00000000;
    *REG2 = 0x00000000;
    *REG3 = 0x00000000;
    *REG4 = 0x00000000;
    *REG5 = 0x00000000;
    *REG6 = 0x00000000;
    *REG7 = 0x00000000;

    mbus_write_message32(0xEF, 0xDEADBEEF);

    mbus_write_message32(0xE0, *REG0);
    mbus_write_message32(0xE1, *REG1);
    mbus_write_message32(0xE2, *REG2);
    mbus_write_message32(0xE3, *REG3);
    mbus_write_message32(0xE4, *REG4);
    mbus_write_message32(0xE5, *REG5);
    mbus_write_message32(0xE6, *REG6);
    mbus_write_message32(0xE7, *REG7);

    mbus_write_message32(0xEF, 0xDEADBEEF);

    //Chip ID
    *REG0 = 0xAAAAAAAA;
    *REG1 = 0xAAAAAAAA;
    *REG2 = 0xAAAAAAAA;
    *REG3 = 0xAAAAAAAA;
    *REG4 = 0xAAAAAAAA;
    *REG5 = 0xAAAAAAAA;
    *REG6 = 0xAAAAAAAA;
    *REG7 = 0xAAAAAAAA;

    mbus_write_message32(0xEF, 0xDEADBEEF);

    mbus_write_message32(0xE0, *REG0);
    mbus_write_message32(0xE1, *REG1);
    mbus_write_message32(0xE2, *REG2);
    mbus_write_message32(0xE3, *REG3);
    mbus_write_message32(0xE4, *REG4);
    mbus_write_message32(0xE5, *REG5);
    mbus_write_message32(0xE6, *REG6);
    mbus_write_message32(0xE7, *REG7);

    mbus_write_message32(0xEF, 0xDEADBEEF);

    //Chip ID
    *REG0 = 0xBBBBBBBB;
    *REG1 = 0xBBBBBBBB;
    *REG2 = 0xBBBBBBBB;
    *REG3 = 0xBBBBBBBB;
    *REG4 = 0xBBBBBBBB;
    *REG5 = 0xBBBBBBBB;
    *REG6 = 0xBBBBBBBB;
    *REG7 = 0xBBBBBBBB;

    mbus_write_message32(0xEF, 0xDEADBEEF);

    mbus_write_message32(0xE0, *REG0);
    mbus_write_message32(0xE1, *REG1);
    mbus_write_message32(0xE2, *REG2);
    mbus_write_message32(0xE3, *REG3);
    mbus_write_message32(0xE4, *REG4);
    mbus_write_message32(0xE5, *REG5);
    mbus_write_message32(0xE6, *REG6);
    mbus_write_message32(0xE7, *REG7);

    mbus_write_message32(0xEF, 0xDEADBEEF);








    mbus_write_message32(0xE8, *REG_CHIP_ID);
    mbus_write_message32(0xE9, *REG_MBUS_THRES);
    mbus_write_message32(0xEA, *REG_IRQ_CTRL);
    mbus_write_message32(0xEB, *REG_CLKGEN_TUNE);
    mbus_write_message32(0xEC, *REG_SRAM_TUNE);

    mbus_write_message32(0xEF, 0xDEADBEEF);



    //Set Halt
    //set_halt_until_mbus_rx();
    //*((volatile uint32_t *) 0xA0000028) = 0x00019000; // Reg0x0A, enable backward-compatibility, halt_until_mbus_rx, disable all other irqs
    *((volatile uint32_t *) 0xA0000028) = 0xFFFFFFFF; // Reg0x0A, enable backward-compatibility, halt_until_mbus_rx, disable all other irqs

    mbus_write_message32(0xEA, *REG_IRQ_CTRL);
    mbus_write_message32(0xEF, 0xDEADBEEF);


    while(1);

    //Enumeration
    mbus_enumerate(FLS_ADDR);

    //Set Halt
    //set_halt_until_mbus_tx();
    *((volatile uint32_t *) 0xA0000028) = 0x0001A000; // Reg0x0A, enable backward-compatibility, halt_until_mbus_tx, disable all other irqs

    mbus_write_message32(0xEA, (uint32_t) *REG_CHIP_ID);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //Initialize Interrupts
    init_interrupt();
  
    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization(); // Enumeration. Enable MBus Tx/Rx IRQ.
    }

    // Display num_cycle;
    mbus_write_message32(0xEF, num_cycle);
    
    *((volatile uint32_t *) 0xA0003EF0) = 0xCCCCCCCC;

    // Testing Sequence
    if (num_cycle == 0) {
        mbus_write_message32(0xEF, 0xAAAAAAAA);
    }
    else if (num_cycle == 1) {
        mbus_write_message32(0xEF, 0xBBBBBBBB);
        delay(10000);
    }
    else {
        mbus_write_message32(0xEF, 0x0EA7F00D);
        while(1);
    }
        
    num_cycle++;

    set_wakeup_timer(10, 1, 1);
    mbus_write_message32(0x01, 0x00000000);
    //mbus_sleep_all();

    while(1);

    return 1;
}

