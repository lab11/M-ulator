//*******************************************************************
//Author: Yejoong Kim
//Description: Self Boot Up Testing
//*******************************************************************
#include "PRCv13.h"
#include "FLPv1S_RF.h"
#include "PMUv2_RF.h"
#include "mbus.h"

// NOTE: Enumeration should've been done during self-bootup
#define PRC_ADDR    0x1
#define FLS_ADDR    0x4
#define PMU_ADDR    0x8

// DELAYS (For MBus Snooping)
#define MBUS_DELAY  1000

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;

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

void handler_ext_int_0(void) { *NVIC_ICPR = (0x1 << 0); irq_history |= (0x1 << 0); // TIMER32
}
void handler_ext_int_1(void) { *NVIC_ICPR = (0x1 << 1); irq_history |= (0x1 << 1); // TIMER16
}
void handler_ext_int_2(void) { *NVIC_ICPR = (0x1 << 2); irq_history |= (0x1 << 2); // REG0
}
void handler_ext_int_3(void) { *NVIC_ICPR = (0x1 << 3); irq_history |= (0x1 << 3); // REG1
}
void handler_ext_int_4(void) { *NVIC_ICPR = (0x1 << 4); irq_history |= (0x1 << 4); // REG2
}
void handler_ext_int_5(void) { *NVIC_ICPR = (0x1 << 5); irq_history |= (0x1 << 5); // REG3
}
void handler_ext_int_6(void) { *NVIC_ICPR = (0x1 << 6); irq_history |= (0x1 << 6); // REG4
}
void handler_ext_int_7(void) { *NVIC_ICPR = (0x1 << 7); irq_history |= (0x1 << 7); // REG5
}
void handler_ext_int_8(void) { *NVIC_ICPR = (0x1 << 8); irq_history |= (0x1 << 8); // REG6
}
void handler_ext_int_9(void) { *NVIC_ICPR = (0x1 << 9); irq_history |= (0x1 << 9); // REG7
}
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); irq_history |= (0x1 << 10); // MEM_WR
}
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); irq_history |= (0x1 << 11); // MBUS_RX
}
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); irq_history |= (0x1 << 12); // MBUS_TX
}
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); irq_history |= (0x1 << 13); // MBUS_FWD
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {
    *REG_CHIP_ID = 0xBEEF;
    cyc_num = 0;
    irq_history = 0;

//    set_halt_until_mbus_rx();
//    mbus_enumerate(FLS_ADDR);
//    set_halt_until_mbus_tx();
}

void notify (uint32_t id) {

    uint32_t data = 0;

    if      (id == 0) data = *REG0;
    else if (id == 1) data = *REG1;
    else if (id == 2) data = *REG2;
    else if (id == 3) data = *REG3;
    else if (id == 4) data = *REG4;
    else if (id == 5) data = *REG5;
    else if (id == 6) data = *REG6;
    else if (id == 7) data = *REG7;

    mbus_write_message32(0xE0, cyc_num);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE1, id);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE2, data);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE3, 0xDEADBEEF);
    delay(MBUS_DELAY);

    if      (id == 0) *REG0 = 0x0;
    else if (id == 1) *REG1 = 0x0;
    else if (id == 2) *REG2 = 0x0;
    else if (id == 3) *REG3 = 0x0;
    else if (id == 4) *REG4 = 0x0;
    else if (id == 5) *REG5 = 0x0;
    else if (id == 6) *REG6 = 0x0;
    else if (id == 7) *REG7 = 0x0;

    mbus_sleep_all();
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Initialize IRQ
    disable_all_irq();

    if (*REG_CHIP_ID != 0xBEEF) {
        // Reset Sleep Timer
        set_wakeup_timer (100, 0, 1);
        // Variable Reset
        initialization();
        // Go to indefinite sleep
        mbus_sleep_all();
        while(1);
    }

    cyc_num++;

    if (*REG0 != 0x0) notify(0);
    if (*REG1 != 0x0) notify(1);
    if (*REG2 != 0x0) notify(2);
    if (*REG3 != 0x0) notify(3);
    if (*REG4 != 0x0) notify(4);
    if (*REG5 != 0x0) notify(5);
    if (*REG6 != 0x0) notify(6);
    if (*REG7 != 0x0) notify(7);

    // Go to indefinite sleep
    mbus_sleep_all();
    while(1) {
        asm("nop;");
    }

    return 1;
}

