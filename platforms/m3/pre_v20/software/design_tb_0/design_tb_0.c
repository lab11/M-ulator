//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PREv20 tape-out for verification
//*******************************************************************
#include "PREv20.h"
#include "FLPv3S.h"
#include "FLPv3S_RF.h"
#include "PMUv9_RF.h"
#include "SNSv11_RF.h"
#include "RDCv2_RF.h"
#include "mbus.h"

#define PRE_ADDR    0x1
#define FLP_ADDR    0x4
#define NODE_A_ADDR 0x8
#define SNS_ADDR    0xC
#define RDC_ADDR    0x5
#define PMU_ADDR    0xE

// FLPv3S Payloads
#define ERASE_PASS  0x4F

// Flag Idx
#define FLAG_ENUM       0
#define FLAG_PMU_SUB_0  1
#define FLAG_PMU_SUB_1  2
#define FLAG_GPIO_SUB   3

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;

volatile uint32_t mem_rsvd_0[10];
volatile uint32_t mem_rsvd_1[10];


volatile flpv3s_r0F_t FLPv3S_REG_IRQ     = FLPv3S_R0F_DEFAULT;
volatile flpv3s_r12_t FLPv3S_REG_PWR_CNF = FLPv3S_R12_DEFAULT;
volatile flpv3s_r09_t FLPv3S_REG_GO      = FLPv3S_R09_DEFAULT;

volatile pmuv9_r51_t PMUv9_REG_CONF = PMUv9_R51_DEFAULT;
volatile pmuv9_r52_t PMUv9_REG_IRQ  = PMUv9_R52_DEFAULT;

// Select Testing
volatile uint32_t do_cycle0  = 1; // System Halt and Resume
volatile uint32_t do_cycle1  = 1; // PMU Testing
volatile uint32_t do_cycle2  = 1; // Register test
volatile uint32_t do_cycle3  = 1; // MEM IRQ
volatile uint32_t do_cycle4  = 1; // Flash Erase
volatile uint32_t do_cycle5  = 1; // Memory Streaming 1
volatile uint32_t do_cycle6  = 1; // Memory Streaming 2
volatile uint32_t do_cycle7  = 1; // TIMER16
volatile uint32_t do_cycle8  = 1; // TIMER32
#ifdef PREv20
volatile uint32_t do_cycle9  = 1; // GPIO (only for PRE)
volatile uint32_t do_cycle10 = 1; // SPI (only for PRE)
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
void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));

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
void handler_ext_int_spi(void) { // SPI
    *NVIC_ICPR = (0x1 << IRQ_SPI); irq_history |= (0x1 << IRQ_SPI);
    arb_debug_reg(IRQ_SPI, 0x00000000);
}
void handler_ext_int_gpio(void) { // GPIO
    *NVIC_ICPR = (0x1 << IRQ_GPIO); irq_history |= (0x1 << IRQ_GPIO);
    arb_debug_reg(IRQ_GPIO, 0x00000000);
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
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(NODE_A_ADDR);
    mbus_enumerate(SNS_ADDR);
    mbus_enumerate(RDC_ADDR);
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
    *REG_CHIP_ID = 0xFFFFFF; // This will stop the verilog sim.
}

void cycle0 (void) {
    if (do_cycle0 == 1) {
        arb_debug_reg(0x30, 0x00000000);

        // --- Send a dummy reg write message to NODE A to wakeup its (fake) layer controller
        mbus_write_message32( (NODE_A_ADDR << 4), 0x00000000);

        // --- System Halt by CPU
        arb_debug_reg(0x30, 0x00000001);
        halt_cpu();

        // Other layer should send the specific mbus message to resume CPU operation
        // Once you see (0x30, 0x0000000l), you need to wait for a while, and then 
        // send the following messages in sequence:
        // MEM[0xAFFFF004] <= 0xCAFEF00D

        // --- System Halt by MBus Message
        // Once you see (0x30, 0x00000002), you need to send the following messages in sequence:
        // MEM[0xAFFFF000] <= 0xBAADF00D
        // MEM[0xA0000000] <= 0x00000001
        // MEM[0xAFFFF004] <= 0xCAFEF00D
        *REG0 = 0;
        arb_debug_reg(0x30, 0x00000002);

        while (*REG0 == 0) {delay(100);}

        // Testing Successful
        arb_debug_reg(0x30, 0x00000003);
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
            PMUv9_REG_CONF.PMU_IRQ_EN = 0x1;
            PMUv9_REG_CONF.PMU_CHECK_WRITE = 0x1;
            mbus_remote_register_write(PMU_ADDR, 0x51, PMUv9_REG_CONF.as_int);

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
            PMUv9_REG_CONF.PMU_IRQ_EN = 0x1;
            PMUv9_REG_CONF.PMU_CHECK_WRITE = 0x0;
            PMUv9_REG_CONF.EN_GLITCH_FILT = 0x1;
            mbus_remote_register_write(PMU_ADDR, 0x51, PMUv9_REG_CONF.as_int);

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
        arb_debug_reg (0x32, 0x00000000);

        set_halt_until_mbus_trx();
        disable_all_irq();
        irq_history = 0;
        uint32_t irq_set;

    //----------------------------------------------------------------------------------------------------------    
        // Copy Reg#1 in FLP and put them in Reg#0 in PRC/PRE.
        mbus_copy_registers_from_remote_to_local (FLP_ADDR, 0x01, 0x00, 0);

    //----------------------------------------------------------------------------------------------------------    
        // Copy Reg#1-4 in FLP and put them in Reg#0-3 in PRC/PRE.
        mbus_copy_registers_from_remote_to_local (FLP_ADDR, 0x01, 0x00, 3);
    
    //----------------------------------------------------------------------------------------------------------    
        // Enable MBus Tx/Rx IRQ, Enable REG IRQ (4-7)
        irq_set = (0xF << IRQ_REG4) | (0x1 << IRQ_MBUS_TX) | (0x1 << IRQ_MBUS_RX);

        clear_all_pend_irq();
        *NVIC_ISER = irq_set;

        // Copy Reg#1-4 in FLP and put them in Reg#4-7 in PRC/PRE.
        mbus_copy_registers_from_remote_to_local (FLP_ADDR, 0x01, 0x04, 3);

        // Here should be 6 IRQs: MBUS Tx/Rx and REG4-7. // OK
        if (irq_history == irq_set) {
               pass (0x0, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0x0, irq_history); disable_all_irq(); }
    
    //----------------------------------------------------------------------------------------------------------    
        // Enable MBus Tx/Rx IRQ
        irq_set = (0x1 << IRQ_MBUS_TX) | (0x1 << IRQ_MBUS_RX);

        clear_all_pend_irq();
        *NVIC_ISER = irq_set;
    
        // Copy Reg#1-4 in FLP and put them in Reg#4-7 in PRC/PRE. It must not generate REG IRQs
        mbus_copy_registers_from_remote_to_local (FLP_ADDR, 0x01, 0x04, 3);

        // Here should be 2 IRQs: MBUS Tx/Rx
        if (irq_history == irq_set) {
               pass (0x1, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0x1, irq_history); disable_all_irq(); }
    
    //----------------------------------------------------------------------------------------------------------    
        // Copy Reg#4 in FLP and put them in Reg#4. There must be no IRQ.
        irq_set = 0;

        mbus_copy_registers_from_remote_to_local (FLP_ADDR, 0x04, 0x04, 0);

        // Here should be 0 IRQs
        if (irq_history == irq_set) {
               pass (0x2, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0x2, irq_history); disable_all_irq(); }

    //----------------------------------------------------------------------------------------------------------    
        // Configure Halt Setting; Now use HALT_UNTIL_REG7
        set_halt_until_reg(7);
    
        // Enable REG IRQ (4-7)
        irq_set = (0xF << IRQ_REG4);

        clear_all_pend_irq();
        *NVIC_ISER = irq_set;

        // Copy Reg#1-4 in FLP and put them in Reg#4-7 in PRC/PRE. It must generate 4 IRQs due to Reg#4-7 in PRC/PRE.
        mbus_copy_registers_from_remote_to_local (FLP_ADDR, 0x01, 0x04, 3);
    
        // Here should be 4 IRQs: REG4-7.
        if (irq_history == irq_set) {
               pass (0x3, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0x3, irq_history); disable_all_irq(); }
    
    //----------------------------------------------------------------------------------------------------------    
        set_halt_until_mbus_tx();
    }
}

void cycle3 (void) {
    if (do_cycle3 == 1) {
        arb_debug_reg (0x33, 0x00000000);
        set_halt_until_mbus_trx();

        uint32_t irq_set;
        irq_history = 0;

    //----------------------------------------------------------------------------------------------------------    
        // Copy MEM 0x00000000 in FLP and store those in PRC/PRE's mem_rsvd_0
        irq_set = (0x1 << IRQ_MBUS_MEM);

        clear_all_pend_irq();
        *NVIC_ISER = irq_set;
        
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, 0x00000000, PRE_ADDR, (uint32_t *) mem_rsvd_0, 0);

        if (irq_history == irq_set) {
               pass (0x4, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0x4, irq_history); disable_all_irq(); }

    //----------------------------------------------------------------------------------------------------------    
        // Copy MEM 0x00000000~0x0000003C in FLP and store those in PRC/PRE's mem_rsvd_1
        irq_set = (0x1 << IRQ_MBUS_MEM);

        clear_all_pend_irq();
        *NVIC_ISER = irq_set;
        
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, 0x00000000, PRE_ADDR, (uint32_t *) mem_rsvd_1, 9);

        if (irq_history == irq_set) {
               pass (0x5, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0x5, irq_history); disable_all_irq(); }

    //----------------------------------------------------------------------------------------------------------    
        // Before streaming, set the pointer to start from.
        *REG_STR0_OFF = ((uint32_t) mem_rsvd_0) >> 2;
        *REG_STR1_OFF = ((uint32_t) mem_rsvd_1) >> 2;

        // Copy MEM 0x00000000 in FLP and store those in PRC/PRE's mem_rsvd_1
        mbus_copy_mem_from_remote_to_any_stream (1, FLP_ADDR, 0x00000000, PRE_ADDR, 0);

        if (*REG_STR1_OFF == (((uint32_t) mem_rsvd_1) >> 2) + 1) pass (0x6, *REG_STR1_OFF);
        else fail (0x6, *REG_STR1_OFF);

    //----------------------------------------------------------------------------------------------------------    
        // Copy MEM 0x00000000~0x0000003C in FLP and store those in PRC/PRE's mem_rsvd_0
        mbus_copy_mem_from_remote_to_any_stream (0, FLP_ADDR, 0x00000000, PRE_ADDR, 9);

        if (*REG_STR0_OFF == (((uint32_t) mem_rsvd_0) >> 2) + 10) pass (0x7, *REG_STR0_OFF);
        else fail (0x7, *REG_STR0_OFF);

    //----------------------------------------------------------------------------------------------------------    
        set_halt_until_mbus_tx();
    }
}

void cycle4 (void) {
    if (do_cycle4 == 1) {
        arb_debug_reg (0x34, 0x00000000);
        set_halt_until_mbus_tx();
    
    //----------------------------------------------------------------------------------------------------------    
    ///////////////////////////
    // Do Flash Erase (REG0) //
    ///////////////////////////
    
        // Set FLP INT_SHORT_PREFIX = 10, REG_ADDR = 00
        FLPv3S_REG_IRQ.INT_RPLY_REG_ADDR = 0x00;
        FLPv3S_REG_IRQ.INT_RPLY_SHORT_ADDR = 0x10;
        mbus_remote_register_write(FLP_ADDR, 0x0F, FLPv3S_REG_IRQ.as_int);

        // Set FLP Power Configuration (Auto On/Off)
        FLPv3S_REG_PWR_CNF.FLASH_AUTO_ON = 0x1;
        FLPv3S_REG_PWR_CNF.FLASH_AUTO_OFF = 0x1;
        FLPv3S_REG_PWR_CNF.FLASH_AUTO_USE_CUSTOM = 0x0;
        FLPv3S_REG_PWR_CNF.SEL_PWR_ON_WUP = 0x0;
        FLPv3S_REG_PWR_CNF.IRQ_PWR_ON_WUP = 0x0;
        mbus_remote_register_write(FLP_ADDR, 0x12, FLPv3S_REG_PWR_CNF.as_int);
    
        // Set HALT_UNTIL_REG0
        set_halt_until_reg(0);
    
        // Start Flash Erase
        FLPv3S_REG_GO.GO = 0x1;
        FLPv3S_REG_GO.CMD = FLPV3_CMD_ERASE;
        FLPv3S_REG_GO.IRQ_EN = 0x1;
        FLPv3S_REG_GO.LENGTH = 0x0;
        mbus_remote_register_write(FLP_ADDR, 0x09, FLPv3S_REG_GO.as_int);

        // Check the payload
        if (*REG0 == ERASE_PASS) { pass (0x8, *REG0); }
        else { fail (0x8, *REG0); }

        // Set HALT_UNTIL_MBUS_TX
        set_halt_until_mbus_tx();

    //----------------------------------------------------------------------------------------------------------    
    ///////////////////////////
    // Do Flash Erase (REG4) //
    ///////////////////////////

        // Set FLP INT_SHORT_PREFIX = 10, REG_ADDR = 04
        FLPv3S_REG_IRQ.INT_RPLY_REG_ADDR = 0x04;
        FLPv3S_REG_IRQ.INT_RPLY_SHORT_ADDR = 0x10;
        mbus_remote_register_write(FLP_ADDR, 0x0F, FLPv3S_REG_IRQ.as_int);

        // Set HALT_UNTIL_REG0
        set_halt_until_reg(4);
    
        // Start Flash Erase
        FLPv3S_REG_GO.GO = 0x1;
        mbus_remote_register_write(FLP_ADDR, 0x09, FLPv3S_REG_GO.as_int);

        // Check the payload
        if (*REG4 == ERASE_PASS) { pass (0x9, *REG4); }
        else { fail (0x9, *REG4); }

        // Set HALT_UNTIL_MBUS_TX
        set_halt_until_mbus_tx();

    //----------------------------------------------------------------------------------------------------------    
    ///////////////////////////
    // Do Flash Erase (REG7) //
    ///////////////////////////

        // Set FLP INT_SHORT_PREFIX = 10, REG_ADDR = 07
        FLPv3S_REG_IRQ.INT_RPLY_REG_ADDR = 0x07;
        FLPv3S_REG_IRQ.INT_RPLY_SHORT_ADDR = 0x10;
        mbus_remote_register_write(FLP_ADDR, 0x0F, FLPv3S_REG_IRQ.as_int);

        // Set HALT_UNTIL_REG0
        set_halt_until_reg(7);
    
        // Start Flash Erase
        FLPv3S_REG_GO.GO = 0x1;
        mbus_remote_register_write(FLP_ADDR, 0x09, FLPv3S_REG_GO.as_int);

        // Check the payload
        if (*REG7 == ERASE_PASS) { pass (0xA, *REG7); }
        else { fail (0xA, *REG7); }

        // Set HALT_UNTIL_MBUS_TX
        set_halt_until_mbus_tx();
    }
}

void cycle5 (void) {
    if (do_cycle5 == 1) {
        arb_debug_reg (0x35, 0x00000000);
        set_halt_until_mbus_tx();
    
        // Copy entire SRAM data into Flash SRAM (bulk)
        // Source Addr: 0x00000000~ (2048 words)
        // Target Addr: 0x00000000~
        arb_debug_reg (0x35, 0x00000001);
        mbus_copy_mem_from_local_to_remote_bulk (FLP_ADDR, 0x00000000, 0x00000000, 2047);
    
        // Put the Flash SRAM data on the bus (to NODE_A)
        arb_debug_reg (0x35, 0x00000002);
        set_halt_until_mbus_fwd();
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, 0x00000000, NODE_A_ADDR, 0x00000000, 511);

        // Put the Flash SRAM data (very long) on the bus (to NODE_A)
        arb_debug_reg (0x35, 0x00000003);
        set_halt_until_mbus_tx();
        mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, 0x00000000, NODE_A_ADDR, 0x00000000, 300);

        // Halt CPU
        arb_debug_reg (0x35, 0x00000004);
        set_halt_until_mbus_fwd();  // CPU will resume when an MBus FWD operation is done.
        halt_cpu();                 // Halt CPU!
        arb_debug_reg (0x35, 0x00000005);

        // Read FLP's REG#0x23 ~ REG#0x27 (5 Registers) 
        arb_debug_reg (0x35, 0x00000006);
        set_halt_until_mbus_trx();
        mbus_copy_registers_from_remote_to_local (FLP_ADDR, 0x23, 0x00, 4);

        // Put those register values on the bus (to an fake address)
        arb_debug_reg (0x35, 0x00000007);
        set_halt_until_mbus_tx();
        mbus_copy_registers_from_local_to_remote (0xD, 0x15, 0x00, 4);
    }
}

void cycle6 (void) {
    if (do_cycle6 == 1) {
        arb_debug_reg (0x36, 0x00000000);
        set_halt_until_mbus_tx();

        // Copy SRAM data into Flash SRAM (stream)
        // From 0x00000000, a quarter of SRAM
        arb_debug_reg (0x36, 0x00000001);
        mbus_copy_mem_from_local_to_remote_stream (0, FLP_ADDR, 0x00000000, 511);
    
        // Put the Flash SRAM data on the bus (to NODE_A)
        arb_debug_reg (0x36, 0x00000002);
        set_halt_until_mbus_fwd();
        mbus_copy_mem_from_remote_to_any_stream (0, FLP_ADDR, 0x00000000, NODE_A_ADDR, 511);

        // Put the Flash SRAM data (very long) on the bus (to NODE_A). I will use halt_cpu() here.
        arb_debug_reg (0x36, 0x00000003);
        set_halt_until_mbus_tx();
        mbus_copy_mem_from_remote_to_any_stream (1, FLP_ADDR, 0x00000000, NODE_A_ADDR, 511);

        // Halt CPU
        arb_debug_reg (0x36, 0x00000004);
        set_halt_until_mbus_fwd();  // CPU will resume when an MBus FWD operation is done.
        halt_cpu();                 // Halt CPU!

        arb_debug_reg (0x36, 0x00000005);
        set_halt_until_mbus_tx();
    }
}

void cycle7 (void) {
    if (do_cycle7 == 1) {
        arb_debug_reg (0x37, 0x00000000);

        set_halt_until_mbus_tx();

        irq_history = 0;
        uint32_t irq_set;

        // Enable TIMER16 IRQ
        irq_set = (0x1 << IRQ_TIMER16);

        clear_all_pend_irq();
        *NVIC_ISER = irq_set;

        // Timer16
        config_timer16(/*cmp0*/ 0x1000, /*cmp1*/0xF000, /*irq_en*/0x3, /*cnt*/0x0, /*status*/0x0);  // Start Timer16. Trigger when it reaches 0x1000 or 0xF000.

        // Wait for Interrupt
        arb_debug_reg (0x37, 0x00000001);
        WFI();

        // 1 IRQ: TIMER16
        if (irq_history == irq_set) { 
               pass (0xB, irq_history); irq_history = 0;}
        else { fail (0xB, irq_history); }

        // Wait for Interrupt
        arb_debug_reg (0x37, 0x00000002);
        WFI();

        // 1 IRQ: TIMER16
        if (irq_history == irq_set) {
               pass (0xC, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0xC, irq_history); disable_all_irq(); }

        // Reset Timer16
        *TIMER16_GO = 0x0;
        *TIMER16_CNT = 0x0;
    }
}

void cycle8 (void) {
    if (do_cycle8 == 1) {
        arb_debug_reg (0x38, 0x00000000);

        set_halt_until_mbus_tx();

        irq_history = 0;
        uint32_t irq_set;

        // Enable TIMER32 IRQ
        irq_set = (0x1 << IRQ_TIMER32);

        clear_all_pend_irq();
        *NVIC_ISER = irq_set;

        // Timer32 with ROI (Reset-On-Interrupt)
        config_timer32(/*cmp*/ 0x2000, /*roi*/ 0x1, /*cnt*/ 0x0, /*status*/ 0x0); // Start Timer32. Trigger when it reaches 0x2000. Reset on Interrupt.

        // Wait for Interrupt
        arb_debug_reg (0x38, 0x00000001);
        WFI();

        // 1 IRQ: TIMER32
        if (irq_history == irq_set) { 
               pass (0xD, irq_history); irq_history = 0;}
        else { fail (0xD, irq_history); }

        // Timer32 without ROI (Reset-On-Interrupt)
        config_timer32(/*cmp*/ 0x3000, /*roi*/ 0x0, /*cnt*/ 0x0, /*status*/ 0x0); // Start Timer32. Trigger when it reaches 0x3000. Reset on Interrupt.

        // Wait for Interrupt
        arb_debug_reg (0x38, 0x00000002);
        WFI();

        // 1 IRQ: TIMER32
        if (irq_history == irq_set) {
               pass (0xE, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0xE, irq_history); disable_all_irq(); }

        // Reset Timer32
        *TIMER32_GO = 0x0;
        *TIMER32_CNT = 0x0;
    }
}

void cycle9 (void) {
    if (do_cycle9 == 1) {
    #ifdef PREv20
        arb_debug_reg (0x39, 0x00000000);
        if (!get_flag(FLAG_GPIO_SUB)) {
            set_flag(FLAG_GPIO_SUB, 1);
            set_halt_until_mbus_tx();
     
        //----------------------------------------------------------------------------------------------------------    
            // Basic GPIO In/Out Test
            gpio_init(0xF0); // [7:4] Output, [3:0] Input
            gpio_write_data(0xF0); // 1111 1010
            gpio_kill_bit(7);      // 0111 1010
            gpio_kill_bit(6);      // 0011 1010
            gpio_set_2bits(7, 6);  // 1111 1010
            gpio_kill_bit(4);      // 1110 1010
     
            if (gpio_get_data() == 0x0A) {pass (0xF, gpio_get_data());}
            else {fail (0xF, gpio_get_data());}
     
        //----------------------------------------------------------------------------------------------------------    
            // GPIO Interrupt Test
            irq_history = 0;
            uint32_t irq_set;
     
            // Enable GPIO IRQ
            *GPIO_IRQ_MASK = 0x0F; // Enable IRQ from GPIO[3:0]
            irq_set = (0x1 << IRQ_GPIO);
     
            clear_all_pend_irq();
            *NVIC_ISER = irq_set;
     
            arb_debug_reg (0x39, 0x00000001);
            WFI();
     
            // 1 IRQ: GPIO
            if (irq_history == irq_set) {
                   pass (0x10, irq_history); irq_history = 0; disable_all_irq(); }
            else { fail (0x10, irq_history); disable_all_irq(); }
     
            // Freeze GPIO pads
            arb_debug_reg (0x39, 0x00000002);
            freeze_gpio_out();

            // go to sleep
            set_wakeup_timer(100, 1, 1);
            mbus_sleep_all();
        }
        else {

            arb_debug_reg (0x39, 0x00000003);

            // Unfreeze GPIO pads. Output pads may go back to 'z' state.
            unfreeze_gpio_out();

            // Disable GPIO pads
            gpio_close();
        }
    #endif
    }
}

void cycle10 (void) {
    if (do_cycle10 == 1) {
    #ifdef PREv20
        arb_debug_reg (0x3A, 0x00000000);
        set_halt_until_mbus_tx();

        // Enable SPI Pads
        set_spi_pad(1);

        *SPI_SSPCR0 = 0x0287; // Motorola Mode
        *SPI_SSPCR1 = 0x2;
        *SPI_SSPCPSR = 0x02; // Clock Prescale Register
        //*SPI_SSPDMACR = 0x3; 
        *SPI_SSPDR = 0x1234;

        delay(1000);

        // Disable SPI Pads
        set_spi_pad(0);

        // Read from SPI Registers
        arb_debug_reg (0x3A, (0x01 << 24) | *SPI_SSPDR);
        arb_debug_reg (0x3A, (0x02 << 24) | *SPI_SSPSR);
        arb_debug_reg (0x3A, (0x03 << 24) | *SPI_SSPPID0);
        arb_debug_reg (0x3A, (0x04 << 24) | *SPI_SSPPID1);
        arb_debug_reg (0x3A, (0x05 << 24) | *SPI_SSPPID2);
        arb_debug_reg (0x3A, (0x06 << 24) | *SPI_SSPPID3);
        arb_debug_reg (0x3A, (0x07 << 24) | *SPI_SSPPCID0);
        arb_debug_reg (0x3A, (0x08 << 24) | *SPI_SSPPCID1);
        arb_debug_reg (0x3A, (0x09 << 24) | *SPI_SSPPCID2);
        arb_debug_reg (0x3A, (0x0A << 24) | *SPI_SSPPCID3);
    #endif
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
    if (cyc_num == 999) *REG_CHIP_ID = 0xFFFFFF; // This will stop the verilog sim.
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
