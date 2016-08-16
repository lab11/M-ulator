//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PRCv13 tape-out for verification
//*******************************************************************
#include "PRCv13.h"
#include "PRCv13_RF.h"
#include "mbus.h"
#include "SNSv7.h"
#include "HRVv2.h"
#include "RADv9.h"
#include "PMUv2_RF.h"

// TStack order  PRC->HAV->SNS->RAD->PMU
#define HRV_ADDR 0x3
#define SNS_ADDR 0x4
#define RAD_ADDR 0x5
#define PMU_ADDR 0x6

#define MBUS_DELAY	10000

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t cyc_num;
volatile uint32_t wakeup_period;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
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
void handler_ext_int_4(void)  { *NVIC_ICPR = (0x1 << 4);  } // REG
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

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    cyc_num = 0;
	wakeup_period = 5;

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



    // Set CPU Halt Option as RX --> Use for register read e.g.
//    set_halt_until_mbus_rx();

    // Enumeration
	delay(MBUS_DELAY);
    mbus_enumerate(HRV_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(SNS_ADDR);
	delay(MBUS_DELAY);
    mbus_enumerate(RAD_ADDR);
	delay(MBUS_DELAY);
 	mbus_enumerate(PMU_ADDR);
	delay(MBUS_DELAY);

	// Set CPU Halt OptioV as TX --> Use for register write e.g.
//   set_halt_until_mbus_tx();


	// Change PMU Setting (each msg will get a response from PMU, so use set_halt_until_mbus_rx);
	
	// dummy
	delay(MBUS_DELAY);
	// Set PMU settings
	// UPCONV_TRIM_V3 Sleep/Active
    mbus_remote_register_write(PMU_ADDR,0x17, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (2) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x18, 
		( (3 << 14) // Desired Vout/Vin ratio; defualt: 0
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (10) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// DOWNCONV_TRIM_V3 Sleep/Active
    mbus_remote_register_write(PMU_ADDR,0x19,
		( (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (0 << 5)  // Frequency multiplier L (actually L+1)
		| (1) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x1A,
		( (1 << 13) // Enable main feedback loop
		| (4 << 9)  // Frequency multiplier R
		| (2 << 5)  // Frequency multiplier L (actually L+1)
		| (8) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// Register 0x15: SAR_TRIM_v3_SLEEP/ACTIVE
    mbus_remote_register_write(PMU_ADDR,0x15, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (6) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
    mbus_remote_register_write(PMU_ADDR,0x16, 
		( (0 << 19) // Enable PFM even during periodic reset
		| (0 << 18) // Enable PFM even when Vref is not used as ref
		| (0 << 17) // Enable PFM
		| (3 << 14) // Comparator clock division ratio
		| (1 << 13) // Enable main feedback loop
		| (1 << 9)  // Frequency multiplier R
		| (1 << 5)  // Frequency multiplier L (actually L+1)
		| (9) 		// Floor frequency base (0-63)
	));
	delay(MBUS_DELAY);
	// SAR_RATIO_OVERRIDE
    mbus_remote_register_write(PMU_ADDR,0x05, //default 12'h000
		( (1 << 11) // Enable override setting [10] (1'h0)
		| (1 << 10) // Have the converter have the periodic reset (1'h0)
		| (1 << 9) // Enable override setting [8] (1'h0)
		| (0 << 8) // Switch input / output power rails for upconversion (1'h0)
		| (1 << 7) // Enable override setting [6:0] (1'h0)
		| (44) 		// Binary converter's conversion ratio (7'h00)
	));
	delay(MBUS_DELAY);
	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_remote_register_write(PMU_ADDR,0x36,0x000001);
	delay(MBUS_DELAY);

	// Set CPU Halt OptioV as TX --> Use for register write e.g.
//   set_halt_until_mbus_tx();

	// Set PMU settings
	// SAR_RATIO_OVERRIDE
//	mbus_write_message32((PMU_ADDR << 4), (0x05 << 24) | 0x000EB5);   
//	delay(MBUS_DELAY);
//	mbus_write_message32((PMU_ADDR << 4), (0x18 << 24) | 0x00E24A);
//	delay(MBUS_DELAY);
//	mbus_write_message32((PMU_ADDR << 4), (0x15 << 24) | 0x00E206);
//	delay(MBUS_DELAY);
//	mbus_write_message32((PMU_ADDR << 4), (0x16 << 24) | 0x00E84F);
//	delay(MBUS_DELAY);
//	mbus_write_message32((PMU_ADDR << 4), (0x36 << 24) | 0x000001);
//	delay(MBUS_DELAY);

}

//********************************************************************
// MAIN function starts here             
//***************************************************************-*****

int main() {

	uint32_t sample_msg[4] = {0x12345678, 0x00000000, 0x87654321, 0xAAAAAAAA};

    //Initialize Interrupts
    disable_all_irq();

    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization();
    }

	delay(MBUS_DELAY);

	mbus_write_message32(0xE1, cyc_num);
	delay(MBUS_DELAY);

	mbus_write_message(0xE2, sample_msg, 4);
	delay(MBUS_DELAY);


	// Set wakeup timer and go to sleep
    cyc_num++;
	if (cyc_num < 5){
		wakeup_period = 200;
	}else if (cyc_num <50){
		wakeup_period = 200;
	}else{
		wakeup_period = 200; // 15000 in decimal-> 3min. at 125C
	}

    set_wakeup_timer(wakeup_period, 1, 1);
    mbus_sleep_all();

    while(1){  //Never Quit (should not come here.)
    mbus_sleep_all();
    }

    return 1;
}
