//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PREv14 tape-out for verification
//*******************************************************************
#include "PREv14.h"
#include "mbus.h"

//********************************************************************
// Global Variables
//********************************************************************

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
void handler_ext_int_15(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void) { *NVIC_ICPR = (0x1 << 0); // TIMER32
}
void handler_ext_int_1(void) { *NVIC_ICPR = (0x1 << 1); // TIMER16
}
void handler_ext_int_2(void) { *NVIC_ICPR = (0x1 << 2); // REG0
}
void handler_ext_int_3(void) { *NVIC_ICPR = (0x1 << 3); // REG1
}
void handler_ext_int_4(void) { *NVIC_ICPR = (0x1 << 4); // REG2
}
void handler_ext_int_5(void) { *NVIC_ICPR = (0x1 << 5); // REG3
}
void handler_ext_int_6(void) { *NVIC_ICPR = (0x1 << 6); // REG4
}
void handler_ext_int_7(void) { *NVIC_ICPR = (0x1 << 7); // REG5
}
void handler_ext_int_8(void) { *NVIC_ICPR = (0x1 << 8); // REG6
}
void handler_ext_int_9(void) { *NVIC_ICPR = (0x1 << 9); // REG7
}
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); // MEM WR
}
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); // MBUS_RX
}
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); // MBUS_TX
}
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); // MBUS_FWD
}
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); // SPI
}
void handler_ext_int_15(void) { *NVIC_ICPR = (0x1 << 15); // GPIO
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    *((volatile uint32_t *) 0xA0001204) = 0;

    while(1);

    return 1;
}

