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
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Initialize IRQ
    disable_all_irq();

    if (*REG_CHIP_ID != 0xBEEF) {
        set_halt_until_mbus_rx();
        mbus_enumerate(FLS_ADDR);
        set_halt_until_mbus_tx();
    }

    mbus_write_message32(0xE0, *REG0);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE1, *REG1);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE2, *REG2);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE3, *REG3);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE4, *REG4);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE5, *REG5);
    delay(MBUS_DELAY);
    mbus_write_message32(0xE6, *REG6);
    delay(MBUS_DELAY);

    set_halt_until_mbus_rx();
    mbus_copy_registers_from_remote_to_local(FLS_ADDR, 0x26, 0x07, 0);
    delay(MBUS_DELAY);
    mbus_copy_registers_from_remote_to_local(FLS_ADDR, 0x27, 0x07, 0);
    delay(MBUS_DELAY);
    set_halt_until_mbus_rx();

    // Go to indefinite sleep
    mbus_sleep_all();
    while(1) {
        asm("nop;");
    }

    return 1;
}

