//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv18 Initial Check
// PRCv18 -> MEMv2 -> SNSv11 -> PMUv9 (optional)
//*******************************************************************
#include "PRCv18.h"
#include "PMUv9_RF.h"
#include "SNSv11_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define MEM_ADDR    0x4
#define SNS_ADDR    0x8
#define PMU_ADDR    0xC

// Define if you use PMU
//#define USE_PMU

// Flag Idx
#define FLAG_ENUM       0

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t irq_history;

volatile uint32_t mem_rsvd_0[10];
volatile uint32_t mem_rsvd_1[10];

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32); irq_history |= (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    }
void handler_ext_int_timer16(void) { // TIMER16
    *NVIC_ICPR = (0x1 << IRQ_TIMER16); irq_history |= (0x1 << IRQ_TIMER16);
    *REG1 = *TIMER16_CNT;
    *REG2 = *TIMER16_STAT;
    *TIMER16_STAT = 0x0;
    }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0); irq_history |= (0x1 << IRQ_REG0);
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1); irq_history |= (0x1 << IRQ_REG1);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2); irq_history |= (0x1 << IRQ_REG2);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3); irq_history |= (0x1 << IRQ_REG3);
}
void handler_ext_int_reg4(void) { // REG4
    *NVIC_ICPR = (0x1 << IRQ_REG4); irq_history |= (0x1 << IRQ_REG4);
}
void handler_ext_int_reg5(void) { // REG5
    *NVIC_ICPR = (0x1 << IRQ_REG5); irq_history |= (0x1 << IRQ_REG5);
}
void handler_ext_int_reg6(void) { // REG6
    *NVIC_ICPR = (0x1 << IRQ_REG6); irq_history |= (0x1 << IRQ_REG6);
}
void handler_ext_int_reg7(void) { // REG7
    *NVIC_ICPR = (0x1 << IRQ_REG7); irq_history |= (0x1 << IRQ_REG7);
}
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM); irq_history |= (0x1 << IRQ_MBUS_MEM);
}
void handler_ext_int_mbusrx(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_RX); irq_history |= (0x1 << IRQ_MBUS_RX);
}
void handler_ext_int_mbustx(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_TX); irq_history |= (0x1 << IRQ_MBUS_TX);
}
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD); irq_history |= (0x1 << IRQ_MBUS_FWD);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP); irq_history |= (0x1 << IRQ_GOCEP);
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); irq_history |= (0x1 << IRQ_SOFT_RESET);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); irq_history |= (0x1 << IRQ_WAKEUP);
    *SREG_WAKEUP_SOURCE = 0;
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    set_flag(FLAG_ENUM, 1);
    irq_history = 0;

    // Set Halt
    set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(MEM_ADDR);
    mbus_enumerate(SNS_ADDR);
   #ifdef USE_PMU
    mbus_enumerate(PMU_ADDR);
   #endif

    //Set Halt
    set_halt_until_mbus_tx();

    // Go to sleep
    set_wakeup_timer(0, 0, 0);
    mbus_sleep_all();
    while(1);
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Enable Wake-Up IRQ
    *NVIC_ISER = (0x1 << IRQ_WAKEUP);

    // Initialization Sequence
    if (!get_flag(FLAG_ENUM)) { 
        initialization();
    }

    // CHIP ID (PUF) Testing
    if (*REG0 == 0x0000F0) {
	while (*REG1 > 0) {
		// Power Up PUF
		*REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);

		// Wait (~20ms)
		delay(1000);

		// Release Isolation
		*REG_SYS_CONF = (0x0/*PUF_SLEEP*/ << 6) | (0x0/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);

		// Store the Chip ID
		*REG_CHIP_ID = *REG_PUF_CHIP_ID;

		// Power-Off PUF
		*REG_SYS_CONF = (0x1/*PUF_SLEEP*/ << 6) | (0x1/*PUF_ISOL*/ << 5) | (0x0/*SOFT_RESET*/ << 4) | (0x0/*PEND_WAKEUP*/ << 0);

		// Send out the CHIP ID
		mbus_write_message32 (0xE0, *REG_CHIP_ID);

		// Reset
		*REG_CHIP_ID = 0;

		// Decrement REG1
		*REG1 = *REG1 - 1;
	}
    }

    // Go to sleep
    set_wakeup_timer(0, 0, 0);
    mbus_sleep_all();
    while(1);

    return 1;
}
