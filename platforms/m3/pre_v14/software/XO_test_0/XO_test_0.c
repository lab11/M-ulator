//*******************************************************************
//Author: Yejoong Kim
//Description: Testcode for XO Driver in PREv14
//             Crystal Information
//                  Description: 32.768kHz 6pF 35kOhm Through-Hole Cylindrical
//                  Digikey P/N: 300-8301-ND
//                  MFG P/N    : CFS206-32.768KDZB-UB
//*******************************************************************
#include "PREv14.h"
#include "mbus.h"

// System Configuration: PREv14 -> FLPv2LL -> PMUv3
#define PRE_ADDR    0x1
#define FLS_ADDR    0x4
#define PMU_ADDR    0x7

// Sleep Duration (clock frequency = 32.768kHz)
#define XOT_SAT_VAL 163840  // 5 sec

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
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_15(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0 (void) { *NVIC_ICPR = (0x1 << 0);  } // TIMER32
void handler_ext_int_1 (void) { *NVIC_ICPR = (0x1 << 1);  } // TIMER16
void handler_ext_int_2 (void) { *NVIC_ICPR = (0x1 << 2);  } // REG0
void handler_ext_int_3 (void) { *NVIC_ICPR = (0x1 << 3);  } // REG1
void handler_ext_int_4 (void) { *NVIC_ICPR = (0x1 << 4);  } // REG2
void handler_ext_int_5 (void) { *NVIC_ICPR = (0x1 << 5);  } // REG3
void handler_ext_int_6 (void) { *NVIC_ICPR = (0x1 << 6);  } // REG4
void handler_ext_int_7 (void) { *NVIC_ICPR = (0x1 << 7);  } // REG5
void handler_ext_int_8 (void) { *NVIC_ICPR = (0x1 << 8);  } // REG6
void handler_ext_int_9 (void) { *NVIC_ICPR = (0x1 << 9);  } // REG7
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); } // MEM WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); } // MBUS_FWD
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14); } // SPI
void handler_ext_int_15(void) { *NVIC_ICPR = (0x1 << 15); } // GPIO

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
inline static void mbus_pmu_register_write(uint32_t reg_id, uint32_t reg_data){
    set_halt_until_mbus_rx();
    mbus_remote_register_write(PMU_ADDR,reg_id,reg_data); 
    set_halt_until_mbus_tx();
}

inline static void mbus_pmu_register_read(uint32_t reg_id){
    set_halt_until_mbus_rx();
    mbus_remote_register_write(PMU_ADDR,0x00,reg_id); 
    set_halt_until_mbus_tx();
}

inline static void PMU_optimization(){
	// Register 0x17: UPCONV_TRIM_V3_SLEEP
    mbus_pmu_register_write(0x17, 
		( (3 << 14) // [0, 3]  Desired Vout/Vin ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1)
		| (4) 		// [2, 31] Floor frequency base
	));
	// Register 0x18: UPCONV_TRIM_V3_ACTIVE
    mbus_pmu_register_write(0x18, 
		( (3 << 14) // [0, 3]  Desired Vout/Vin ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R
		| (2 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (10) 		// [2, 31] Floor frequency base
	));
	// Register 0x19: DOWNCONV_TRIM_V3_SLEEP
    mbus_pmu_register_write(0x19,
		( (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R (1)
		| (0 << 5)  // [0, 15] Frequency multiplier L (actually L+1) (0)
		| (2) 		// [1, 31] Floor frequency base (2)
	));
	// Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
    mbus_pmu_register_write(0x1A,
		( (1 << 13) // [1, 1]  Enable main feedback loop
		| (8 << 9)  // [4, 15] Frequency multiplier R
		| (4 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (8) 		// [8, 31] Floor frequency base
	));
	// Register 0x15: SAR_TRIM_v3_SLEEP
    mbus_pmu_register_write(0x15, 
		( (0 << 19) // [0, 1]  Enable PFM even during periodic reset
		| (0 << 18) // [0, 1]  Enable PFM even when Vref is not used as ref
		| (0 << 17) // [0, 1]  Enable PFM
		| (3 << 14) // [3, 7]  Comparator clock division ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (1 << 9)  // [1, 15] Frequency multiplier R (1)
		| (1 << 5)  // [0, 15] Frequency multiplier L (actually L+1) (0)
		| (5) 		// [1, 31] Floor frequency base (4)
	));
	// Register 0x16: SAR_TRIM_v3_ACTIVE
    mbus_pmu_register_write(0x16, 
		( (0 << 19) // [0, 1]  Enable PFM even during periodic reset
		| (0 << 18) // [0, 1]  Enable PFM even when Vref is not used as ref
		| (0 << 17) // [0, 1]  Enable PFM
		| (3 << 14) // [3, 7]  Comparator clock division ratio
		| (1 << 13) // [1, 1]  Enable main feedback loop
		| (8 << 9)  // [4, 15] Frequency multiplier R
		| (8 << 5)  // [2, 15] Frequency multiplier L (actually L+1)
		| (15) 		// [8, 31] Floor frequency base
	));
	// Register 0x05: SAR_RATIO_OVERRIDE
    mbus_pmu_register_write(0x05,
		( (1 << 13) // [1, 1]
        | (0 << 12) // [0, 1]
        | (1 << 11) // [0, 1]   Enable override setting [10]
		| (0 << 10) // [0, 1]   Have the converter have the periodic reset
		| (1 << 9)  // [0, 1]   Enable override setting [8]
		| (0 << 8)  // [0, 1]   Switch input / output power rails for upconversion
		| (1 << 7)  // [0, 1]   Enable override setting [6:0]
		| (44) 		// [0, 127] Binary converter's conversion ratio
	));

	// Register 0x36: TICK_REPEAT_VBAT_ADJUST
    mbus_pmu_register_write(0x36, 1);   // [500, 16777215] Num of clock cycles the controller waits per each periodic conversion ratio adjustment after being fully turned-on

	// Register 0x37: TICK_WAKEUP_WAIT
    mbus_pmu_register_write(0x37, 2000); // [500, 16777215] Num of clock cycles the controller waits for internal state transition from sleep to wakeup mode

	// Register 0x3A: PMU_CONTROLLER_STALL_ACTIVE
    mbus_pmu_register_write(0x3A, 0x00080800); // state_horizon=1, state_adc_output_ready=1
	// Register 0x3B: PMU_CONTROLLER_DESIRED_STATE_SLEEP
	mbus_pmu_register_write(0x3B, 0x000FEFFF); // state_adc_adjusted=0
	// Register 0x3C: PMU_CONTROLLER_DESIRED_STATE_ACTIVE
	mbus_pmu_register_write(0x3C, 0x0007CFFF); // state_horizon=0, state_sar_scn_ratio_adjusted=0, state_adc_adjusted=0
}

void XO_update_config (uint32_t pul, uint32_t start, uint32_t core, uint32_t res, uint32_t clk_sel, uint32_t scn_enb) {
    *REG_XO_CONFIG =( (pul     << 8)   // Pulse Length Selection
                    | (start   << 7)   // XO_DRV_START_UP
                    | (core    << 6)   // XO_DRV_CORE
                    | (res     << 2)   // Pseudo-Resistor Selection
                    | (clk_sel << 1)   // XO_SCN_CLK_SEL
                    | (scn_enb << 0)); // XO_SCN_ENB
}

void XO_initialization (void) {
    // XO_CLK Output Pad (0: 32kHz, 1: 16kHz, 2: 8kHz, 3: 4kHz)
    *REG_CLK_OUT_SEL = 0x3;

    // Parasitic Capacitance Tuning (6-bit for each; Each 1 adds 1.8pF)
    uint32_t cap_in  = 0x1F; // Additional Cap on OSC_IN
    uint32_t cap_drv = 0x00; // Additional Cap on OSC_DRV

    // Pulse Length Selection
    uint32_t pul =   (0x4 << 3)  // XO_PULSE_SEL
                   | (0x3 << 0); // XO_DELAY_EN

    // Pseudo-Resistor Selection
    uint32_t res =   (0x0 << 3) // XO_RP_LOW
                   | (0x1 << 2) // XO_RP_MEDIA
                   | (0x0 << 1) // XO_RP_MVT
                   | (0x0 << 0);// XO_RP_SVT

    // Parasitic Capacitance Tuning
    *REG_XO_CAP_TUNE = (cap_in << 6) | (cap_drv << 0);

    // Start XO Clock
    XO_update_config (pul, 0, 0, res, 0, 1); delay(10000); // Set Default
    XO_update_config (pul, 1, 0, res, 0, 1); delay(10000); // XO_DRV_START_UP = 1
    XO_update_config (pul, 1, 0, res, 1, 1); delay(100);   // XO_SCN_CLK_SEL = 1
    XO_update_config (pul, 1, 0, res, 0, 0); delay(100);   // XO_SCN_CLK_SEL = 0 & XO_SCN_ENB = 0
    XO_update_config (pul, 0, 1, res, 0, 0); delay(100);   // XO_DRV_START_UP = 0 & XO_DRV_CORE = 1
}

void initialization (void) {
    // Disable MBus Watchdog Timer
    *REG_MBUS_WD = 0;

    // Global Variables
    enumerated = 0xDEADBEEF;
    cyc_num = 0;

    // Enumeration
    set_halt_until_mbus_rx();
    mbus_enumerate(FLS_ADDR);
    mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();

    // PMU Optimization
    PMU_optimization();
    
    // XO Setting
    XO_initialization ();

    // Set up XOT timer
    uint32_t xot_sat_val_upper = (XOT_SAT_VAL & 0xFFFF0000) >> 16;
    uint32_t xot_sat_val_lower = XOT_SAT_VAL & 0x0000FFFF;

    *REG_XOT_CONFIGU = xot_sat_val_upper;
    *REG_XOT_CONFIG  = (0x1 << 17) | (0x1 << 16) | (xot_sat_val_lower << 0);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Enable IRQ
    clear_all_pend_irq();
    enable_all_irq();

    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization();
    }

    // Send an MBus message
    mbus_write_message32 (0xEE, cyc_num);
    cyc_num++;

    // Reset XOT Timer
    *XOT_RESET = 1;

    // Send an MBus sleep message
    mbus_sleep_all();

    while(1){  //Never Quit (should not come here.)
        asm("nop;"); 
    }

    return 1;
}

