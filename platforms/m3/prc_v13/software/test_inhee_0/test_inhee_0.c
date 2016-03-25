//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PRCv13 tape-out for verification
//*******************************************************************
#include "PRCv13.h"
#include "PMUv2_RF.h"
#include "mbus.h"

#define SNS_ADDR    0x4
#define PMU_ADDR    0x5


#define MBUS_DELAY	10000

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t cyc_num;

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

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    cyc_num = 0;

	// Clock Speed (By default, CLK_CORE is 2x slower than CLK_MBC)
//	*REG_CLKGEN_TUNE = 	  (0x0 << 19) // CLK_GEN_HIGH_FREQ
//                       	| (0x3 << 17) // CLK_GEN_DIV_CORE
//						| (0x1 << 15) // CLK_GEN_DIV_MBC
//						| (0x1 << 13) // CLK_GEN_RING
//						| (0x0 << 12) // DSLP_CLK_GEN_FAST_MODE
//						| (0x0 << 10) // GOC_CLK_GEN_SEL_DIV
//						| (0x6 << 7)  // GOC_CLK_GEN_SEL_FREQ
//						| (0x0 << 6)  // GOC_ONECLK_MODE
//						| (0x0 << 4)  // GOC_SEL_DLY
//						| (0x8 << 0); // GOC_SEL

	// Disable Halting --> Use manual delays
	set_halt_disable();

    // Set Halt RX --> Use for register read e.g.
    // set_halt_until_mbus_rx();

    // Enumeration
    mbus_enumerate(SNS_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);

	// Change PMU Setting (each msg will get a response from PMU, so use set_halt_until_mbus_rx);

	// Register Write
	// mbus_write_message32((PMU_ADDR << 4), (REG_ID << 24) | (REG_DATA));
	mbus_write_message32((PMU_ADDR << 4), (0x05 << 24) | 0x000EBD);   // EB0
	delay(MBUS_DELAY);
	mbus_write_message32((PMU_ADDR << 4), (0x18 << 24) | 0x00E24A);
	delay(MBUS_DELAY);
	mbus_write_message32((PMU_ADDR << 4), (0x15 << 24) | 0x00E206);
	delay(MBUS_DELAY);
	mbus_write_message32((PMU_ADDR << 4), (0x36 << 24) | 0x000001);
	delay(MBUS_DELAY);

	// Register Read
	// mbus_write_message32((PMU_ADDR << 4), (REG_ID));
	// mbus_write_message32((PMU_ADDR << 4), 0xF);
	// delay(MBUS_DELAY);

    // Set Halt --> Use for register write e.g.
    // set_halt_until_mbus_tx();
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
	uint32_t sample_msg[4] = {0x12345678, 0x00000000, 0x87654321, 0xAAAAAAAA};

    //Initialize Interrupts
    disable_all_irq();

    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization();
    }

	mbus_write_message32(0xE1, cyc_num);
	delay(MBUS_DELAY);

	mbus_write_message(0xE2, sample_msg, 4);
	delay(MBUS_DELAY);


	// Set wakeup timer and go to sleep
    cyc_num++;
    set_wakeup_timer(2, 1, 1); // 500 for High Temp.
    mbus_sleep_all();

    while(1){  //Never Quit (should not come here.)
        asm("nop;"); 
    }

    return 1;
}
