//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PRCv17 tape-out for verification
//*******************************************************************
#include "PRCv17.h"
#include "PMUv9_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define PMU_ADDR    0xE

// Flag Idx
#define FLAG_ENUM       0
#define FLAG_PMU_SUB_0  1
#define FLAG_PMU_SUB_1  2
#define FLAG_VCO        3

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;
volatile uint32_t temp_val;

volatile uint32_t mem_rsvd_0[10];
volatile uint32_t mem_rsvd_1[10];

volatile pmuv9_r51_t PMUv9_R51_CONF = PMUv9_R51_DEFAULT;
volatile pmuv9_r52_t PMUv9_R52_IRQ  = PMUv9_R52_DEFAULT;
volatile pmuv9_r53_t PMUv9_R53_VCO  = PMUv9_R53_DEFAULT;

// Select Testing
volatile uint32_t do_cycle0  = 0; // System Halt and Resume
volatile uint32_t do_cycle1  = 1; // PMU General Testing
volatile uint32_t do_cycle2  = 1; // PMU VCO Testing
volatile uint32_t do_cycle3  = 0; // 
volatile uint32_t do_cycle4  = 0; // 
volatile uint32_t do_cycle5  = 0; // 
volatile uint32_t do_cycle6  = 0; // 
volatile uint32_t do_cycle7  = 0; // 
volatile uint32_t do_cycle8  = 0; // 
#ifdef PREv17
volatile uint32_t do_cycle9  = 0; // 
volatile uint32_t do_cycle10 = 0; // 
#else
volatile uint32_t do_cycle9  = 0;
volatile uint32_t do_cycle10 = 0;
#endif
volatile uint32_t do_cycle11 = 1; // Watch-Dog

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
    arb_debug_reg(IRQ_TIMER32, 0x00000000);
    arb_debug_reg(IRQ_TIMER32, (0x10 << 24) | *REG1); // TIMER32_CNT
    arb_debug_reg(IRQ_TIMER32, (0x20 << 24) | *REG2); // TIMER32_STAT
    }
void handler_ext_int_timer16(void) { // TIMER16
    *NVIC_ICPR = (0x1 << IRQ_TIMER16); irq_history |= (0x1 << IRQ_TIMER16);
    *REG1 = *TIMER16_CNT;
    *REG2 = *TIMER16_STAT;
    *TIMER16_STAT = 0x0;
    arb_debug_reg(IRQ_TIMER16, 0x00000000);
    arb_debug_reg(IRQ_TIMER16, (0x10 << 24) | *REG1); // TIMER16_CNT
    arb_debug_reg(IRQ_TIMER16, (0x20 << 24) | *REG2); // TIMER16_STAT
    }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0); irq_history |= (0x1 << IRQ_REG0);
    arb_debug_reg(IRQ_REG0, 0x00000000);
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1); irq_history |= (0x1 << IRQ_REG1);
    arb_debug_reg(IRQ_REG1, 0x00000000);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2); irq_history |= (0x1 << IRQ_REG2);
    arb_debug_reg(IRQ_REG2, 0x00000000);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3); irq_history |= (0x1 << IRQ_REG3);
    arb_debug_reg(IRQ_REG3, 0x00000000);
}
void handler_ext_int_reg4(void) { // REG4
    *NVIC_ICPR = (0x1 << IRQ_REG4); irq_history |= (0x1 << IRQ_REG4);
    arb_debug_reg(IRQ_REG4, 0x00000000);
}
void handler_ext_int_reg5(void) { // REG5
    *NVIC_ICPR = (0x1 << IRQ_REG5); irq_history |= (0x1 << IRQ_REG5);
    arb_debug_reg(IRQ_REG5, 0x00000000);
}
void handler_ext_int_reg6(void) { // REG6
    *NVIC_ICPR = (0x1 << IRQ_REG6); irq_history |= (0x1 << IRQ_REG6);
    arb_debug_reg(IRQ_REG6, 0x00000000);
}
void handler_ext_int_reg7(void) { // REG7
    *NVIC_ICPR = (0x1 << IRQ_REG7); irq_history |= (0x1 << IRQ_REG7);
    arb_debug_reg(IRQ_REG7, 0x00000000);
}
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM); irq_history |= (0x1 << IRQ_MBUS_MEM);
    arb_debug_reg(IRQ_MBUS_MEM, 0x00000000);
}
void handler_ext_int_mbusrx(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_RX); irq_history |= (0x1 << IRQ_MBUS_RX);
    arb_debug_reg(IRQ_MBUS_RX, 0x00000000);
}
void handler_ext_int_mbustx(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_TX); irq_history |= (0x1 << IRQ_MBUS_TX);
    arb_debug_reg(IRQ_MBUS_TX, 0x00000000);
}
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD); irq_history |= (0x1 << IRQ_MBUS_FWD);
    arb_debug_reg(IRQ_MBUS_FWD, 0x00000000);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP); irq_history |= (0x1 << IRQ_GOCEP);
    arb_debug_reg(IRQ_GOCEP, 0x00000000);
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); irq_history |= (0x1 << IRQ_SOFT_RESET);
    arb_debug_reg(IRQ_SOFT_RESET, 0x00000000);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); irq_history |= (0x1 << IRQ_WAKEUP);
    arb_debug_reg(IRQ_WAKEUP, (0x10 << 24) | *SREG_WAKEUP_SOURCE);
    *SREG_WAKEUP_SOURCE = 0;
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    set_flag(FLAG_ENUM, 1);
    cyc_num = 0;
    irq_history = 0;

    // Set Halt
    set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(PMU_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();
}

void pass (uint32_t id, uint32_t data) {
    arb_debug_reg(0xE0, 0x0);
    arb_debug_reg(0xE1, id);
    arb_debug_reg(0xE2, data);
}

void fail (uint32_t id, uint32_t data) {
    arb_debug_reg(0xE8, 0x0);
    arb_debug_reg(0xE9, id);
    arb_debug_reg(0xEA, data);
    *REG_CHIP_ID = 0xFFFF; // This will stop the verilog sim.
}

void cycle0 (void) {
    if (do_cycle0 == 1) {
    }
}

void cycle1 (void) {
    if (do_cycle1 == 1) {
        // Sub-Test 0: PMU Read/Write
        if ((get_flag(FLAG_PMU_SUB_1)==0) & (get_flag(FLAG_PMU_SUB_0)==0)) {
            arb_debug_reg(0x31, 0x00000000);
            set_halt_until_mbus_trx();

            uint32_t idx;

            // Read from PMU Registers
            for (idx=0; idx<72; idx++) {
                mbus_remote_register_write (PMU_ADDR, 0x00, idx);
            }

            // Write to PMU Registers (without write check)
            mbus_remote_register_write(PMU_ADDR, 0x3C, (( 1 << 0) | (0 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))); // PMU_CONTROLLER_DESIRED_STATE Active
            mbus_remote_register_write(PMU_ADDR, 0x36, 0xFFFFFFFF); // Register 0x36: TICK_REPEAT_VBAT_ADJUST
            mbus_remote_register_write(PMU_ADDR, 0x3C, ( (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))); // PMU_CONTROLLER_DESIRED_STATE Active
            mbus_remote_register_write(PMU_ADDR, 0x17, ( (3 << 14) | (0 << 13) | (1 << 9) | (2 << 5) | (1))); // Register 0x17: V3P6 Upconverter Sleep Settings
            mbus_remote_register_write(PMU_ADDR, 0x18, ( (3 << 14) | (0 << 13) | (2 << 9) | (0 << 5) | (16))); // Register 0x18: V3P6 Upconverter Active Settings
            mbus_remote_register_write(PMU_ADDR, 0x19, ( (0 << 13) | (1 << 9) | (1 << 5) | (1))); // Register 0x19: DOWNCONV_TRIM_V3_SLEEP
            mbus_remote_register_write(PMU_ADDR, 0x1A, ( (0 << 13) | (4 << 9) | (0 << 5) | (16))); // Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
            mbus_remote_register_write(PMU_ADDR, 0x15, ( (0 << 19) | (0 << 18) | (0 << 17) | (3 << 14) | (0 << 13) | (1 << 9) | (2 << 5) | (1) )); // Register 0x15: V1P2 SAR_TRIM_v3_SLEEP
            mbus_remote_register_write(PMU_ADDR, 0x16, ( (0 << 19) | (0 << 18) | (0 << 17) | (3 << 14) | (0 << 13) | (4 << 9) | (0 << 5) | (16) )); // Register 0x16: V1P2 SAR_TRIM_v3_ACTIVE
            mbus_remote_register_write(PMU_ADDR, 0x05, ( (0 << 13) | (0 << 12) | (1 << 11) | (0 << 10) | (1 << 9) | (0 << 8) | (1 << 7) | (44) )); // SAR_RATIO_OVERRIDE
            mbus_remote_register_write(PMU_ADDR, 0x3C, ( (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))); // PMU_CONTROLLER_DESIRED_STATE Active
            mbus_remote_register_write(PMU_ADDR, 0x3B, ( (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))); // PMU_CONTROLLER_DESIRED_STATE Sleep
            mbus_remote_register_write(PMU_ADDR, 0x0E, ( (1 << 10) | (1 << 9) | (0 << 8) | (1 << 4) | (0) )); // PMU_SOLAR_SHORT

            // Enable PMU Write Check
            set_halt_until_mbus_tx();
            PMUv9_R51_CONF.PMU_IRQ_EN = 0x1;
            PMUv9_R51_CONF.PMU_CHECK_WRITE = 0x1;
            mbus_remote_register_write(PMU_ADDR, 0x51, PMUv9_R51_CONF.as_int);

            // Write to PMU Registers (without write check)
            set_halt_until_mbus_trx();
            mbus_remote_register_write(PMU_ADDR, 0x3C, (( 1 << 0) | (0 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))); // PMU_CONTROLLER_DESIRED_STATE Active
            mbus_remote_register_write(PMU_ADDR, 0x36, 0xFFFFFFFF); // Register 0x36: TICK_REPEAT_VBAT_ADJUST
            mbus_remote_register_write(PMU_ADDR, 0x3C, ( (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))); // PMU_CONTROLLER_DESIRED_STATE Active
            mbus_remote_register_write(PMU_ADDR, 0x17, ( (3 << 14) | (0 << 13) | (1 << 9) | (2 << 5) | (1))); // Register 0x17: V3P6 Upconverter Sleep Settings
            mbus_remote_register_write(PMU_ADDR, 0x18, ( (3 << 14) | (0 << 13) | (2 << 9) | (0 << 5) | (16))); // Register 0x18: V3P6 Upconverter Active Settings
            mbus_remote_register_write(PMU_ADDR, 0x19, ( (0 << 13) | (1 << 9) | (1 << 5) | (1))); // Register 0x19: DOWNCONV_TRIM_V3_SLEEP
            mbus_remote_register_write(PMU_ADDR, 0x1A, ( (0 << 13) | (4 << 9) | (0 << 5) | (16))); // Register 0x1A: DOWNCONV_TRIM_V3_ACTIVE
            mbus_remote_register_write(PMU_ADDR, 0x15, ( (0 << 19) | (0 << 18) | (0 << 17) | (3 << 14) | (0 << 13) | (1 << 9) | (2 << 5) | (1) )); // Register 0x15: V1P2 SAR_TRIM_v3_SLEEP
            mbus_remote_register_write(PMU_ADDR, 0x16, ( (0 << 19) | (0 << 18) | (0 << 17) | (3 << 14) | (0 << 13) | (4 << 9) | (0 << 5) | (16) )); // Register 0x16: V1P2 SAR_TRIM_v3_ACTIVE
            mbus_remote_register_write(PMU_ADDR, 0x05, ( (0 << 13) | (0 << 12) | (1 << 11) | (0 << 10) | (1 << 9) | (0 << 8) | (1 << 7) | (44) )); // SAR_RATIO_OVERRIDE
            mbus_remote_register_write(PMU_ADDR, 0x3C, ( (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))); // PMU_CONTROLLER_DESIRED_STATE Active
            mbus_remote_register_write(PMU_ADDR, 0x3B, ( (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))); // PMU_CONTROLLER_DESIRED_STATE Sleep
            mbus_remote_register_write(PMU_ADDR, 0x0E, ( (1 << 10) | (1 << 9) | (0 << 8) | (1 << 4) | (0) )); // PMU_SOLAR_SHORT

            // Set Halt
            set_halt_until_mbus_tx();

            // Set Flags
            set_flag(FLAG_PMU_SUB_1, 0);
            set_flag(FLAG_PMU_SUB_0, 1);

            // Go Sleep 
            set_wakeup_timer(100, 1, 1);
            arb_debug_reg(0x31, 0x00000001);
            mbus_sleep_all();
        }
        // Sub-Test 1: PMU DIN Glitch
        else if ((get_flag(FLAG_PMU_SUB_1)==0) & (get_flag(FLAG_PMU_SUB_0)==1)) {

            // Enable PMU Write Check
            set_halt_until_mbus_tx();
            PMUv9_R51_CONF.PMU_IRQ_EN = 0x1;
            PMUv9_R51_CONF.PMU_CHECK_WRITE = 0x0;
            PMUv9_R51_CONF.EN_GLITCH_FILT = 0x1;
            mbus_remote_register_write(PMU_ADDR, 0x51, PMUv9_R51_CONF.as_int);

            // Set Flags
            set_flag(FLAG_PMU_SUB_1, 1);
            set_flag(FLAG_PMU_SUB_0, 0);

            // Go Sleep 
            set_wakeup_timer(100, 1, 1);
            arb_debug_reg(0x31, 0x00000002);
            mbus_sleep_all();
        }
        else {
            arb_debug_reg(0x31, 0x00000003);
        }
    }

}

void cycle2 (void) {
    if (do_cycle2 == 1) {
        arb_debug_reg(0x70, 0x00000000);

        if (!get_flag(FLAG_VCO)) {
            set_flag(FLAG_VCO, 1);

            // Reset VCO_CNT_ENABLE (unnecessary, but..)
            PMUv9_R53_VCO.VCO_CNT_ENABLE    = 0x0;
            PMUv9_R53_VCO.VCO_CNT_MODE      = 0x0;
            PMUv9_R53_VCO.VCO_CNT_THRESHOLD = 0x0;
            mbus_remote_register_write(PMU_ADDR, 0x53, PMUv9_R53_VCO.as_int);

            // Reset TIMER32 (unnecessary, but..)
            *TIMER32_GO   = 0;
            *TIMER32_CMP  = 0;
	        *TIMER32_ROI  = 0;
	        *TIMER32_CNT  = 0;
	        *TIMER32_STAT = 0;
	        *TIMER32_GO   = 0x1;

            // Start Active Measurement
            arb_debug_reg(0x70, 0x10000000);
            set_halt_until_mbus_trx();
            PMUv9_R53_VCO.VCO_CNT_ENABLE    = 0x1;
            PMUv9_R53_VCO.VCO_CNT_MODE      = 0x1;
            PMUv9_R53_VCO.VCO_CNT_THRESHOLD = 0x1000;
            mbus_remote_register_write(PMU_ADDR, 0x53, PMUv9_R53_VCO.as_int);

            // Get TIMER32 measurement
            temp_val = *TIMER32_CNT;
            *TIMER32_GO = 0;
            set_halt_until_mbus_tx();

            arb_debug_reg(0x70, (0x20 << 24) | temp_val);

            // Reset VCO_CNT_ENABLE
            PMUv9_R53_VCO.VCO_CNT_ENABLE    = 0x0;
            PMUv9_R53_VCO.VCO_CNT_MODE      = 0x0;
            PMUv9_R53_VCO.VCO_CNT_THRESHOLD = 0x0;
            mbus_remote_register_write(PMU_ADDR, 0x53, PMUv9_R53_VCO.as_int);

            // Start Sleep Measurement
            PMUv9_R53_VCO.VCO_CNT_ENABLE    = 0x1;
            PMUv9_R53_VCO.VCO_CNT_MODE      = 0x0;
            PMUv9_R53_VCO.VCO_CNT_THRESHOLD = 0x1000;
            mbus_remote_register_write(PMU_ADDR, 0x53, PMUv9_R53_VCO.as_int);

            // Go to sleep 
            arb_debug_reg(0x70, 0x30000000);
            set_wakeup_timer(100, 0, 1);
            mbus_sleep_all();
        }
        else {
            // Reset VCO_CNT_ENABLE
            PMUv9_R53_VCO.VCO_CNT_ENABLE    = 0x0;
            PMUv9_R53_VCO.VCO_CNT_MODE      = 0x0;
            PMUv9_R53_VCO.VCO_CNT_THRESHOLD = 0x0;
            mbus_remote_register_write(PMU_ADDR, 0x53, PMUv9_R53_VCO.as_int);

            arb_debug_reg(0x70, 0x40000000);
        }
    }
}

void cycle3 (void) {
    if (do_cycle3 == 1) {
    }
}

void cycle4 (void) {
    if (do_cycle4 == 1) {
    }
}

void cycle5 (void) {
    if (do_cycle5 == 1) {
    }
}

void cycle6 (void) {
    if (do_cycle6 == 1) {
    }
}

void cycle7 (void) {
    if (do_cycle7 == 1) {
    }
}

void cycle8 (void) {
    if (do_cycle8 == 1) {
    }
}

void cycle9 (void) {
    if (do_cycle9 == 1) {
    }
}

void cycle10 (void) {
    if (do_cycle10 == 1) {
    }
}

void cycle11 (void) {
    if (do_cycle11 == 1) {
        arb_debug_reg (0x3B, 0x00000000);
        set_halt_until_mbus_tx();

        // Watch-Dog Timer (You should see TIMERWD triggered)
        arb_debug_reg (0x3B, 0x00000001);
        config_timerwd(100);

        // Wait here until PMU resets everything
        while(1);
    }
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

    arb_debug_reg(0x20, cyc_num);

    // Testing Sequence
    if      (cyc_num == 0)  cycle0();
    else if (cyc_num == 1)  cycle1();
    else if (cyc_num == 2)  cycle2();
    else if (cyc_num == 3)  cycle3();
    else if (cyc_num == 4)  cycle4();
    else if (cyc_num == 5)  cycle5();
    else if (cyc_num == 6)  cycle6();
    else if (cyc_num == 7)  cycle7();
    else if (cyc_num == 8)  cycle8();
    else if (cyc_num == 9)  cycle9();
    else if (cyc_num == 10) cycle10();
    else if (cyc_num == 11) cycle11();
    else cyc_num = 999;

    arb_debug_reg(0x2F, cyc_num);

    // Sleep/Wakeup OR Terminate operation
    if (cyc_num == 999) *REG_CHIP_ID = 0xFFFF; // This will stop the verilog sim.
    else {
        cyc_num++;
        set_wakeup_timer(5, 1, 1);
        mbus_sleep_all();
    }

    while(1){  //Never Quit (should not come here.)
        arb_debug_reg(0xFF, 0xDEADBEEF);
        asm("nop;"); 
    }

    return 1;
}
