//*******************************************************************
// C SOURCE CODE FOR PRC/PRE VERILOG TESTBENCH
// design_tb_2 (PRE(E))
//-------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jul 11 2022 -   PRCv23 Family
//                      - Member Layer Version Change
//                          SNTv5  -> SNTv6
//                          PMUv12 -> PMUv13
//                      - Added set_halt_until_mbus_trx() before writing SNT wakeup timer threshold (SNT Reg 0x1A)
//                      - For arb_debug_reg(0x52, 0x00000001): Changed the snt_wup_start() threshold from 37 to 55.
//                      - For arb_debug_reg(0x52, 0x00000002): Changed the snt_wup_start() threshold from 16 to 30.
//                      - For arb_debug_reg(0x53, 0x00000001): Changed the snt_wup_start() threshold from 3 to 30.
//                      - For arb_debug_reg(0x53, 0x00000002): Changed the snt_wup_start() threshold from 32 to 50.
//                      - Added cycle4 to test the NO_SLEEP_WITH_PEND_IRQ=0 configuration
//-------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@umich.edu)
//*******************************************************************
#include "PREv23E.h"
#include "FLPv3S.h"
#include "FLPv3S_RF.h"
#include "SNTv6_RF.h"
#include "RDCv4_RF.h"
#include "MRRv11A_RF.h"
#include "SRRv8_RF.h"
#include "PMUv13_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLP_ADDR    0x3
#define NODE_A_ADDR 0x4
#define SNT_ADDR    0x5
#define RDC_ADDR    0x6
#define MRR_ADDR    0x7
#define SRR_ADDR    0x8
#define PMU_ADDR    0x9

// FLPv3S Payloads
#define ERASE_PASS  0x4F

// Flag Idx
#define FLAG_ENUM        0

#define FLAG_SNTWUP     1
#define FLAG_SNTWUP_LEN 3

#define FLAG_PENDIRQ     4
#define FLAG_PENDIRQ_LEN 3

#define FLAG_PENDIRQ0     7
#define FLAG_PENDIRQ0_LEN 3

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;

volatile uint32_t mem_rsvd_0[10];
volatile uint32_t mem_rsvd_1[10];


volatile flpv3s_r0F_t FLPv3S_REG_IRQ    = FLPv3S_R0F_DEFAULT;
volatile flpv3s_r12_t FLPv3S_PWR_CNF    = FLPv3S_R12_DEFAULT;
volatile flpv3s_r09_t FLPv3S_R07_GO     = FLPv3S_R09_DEFAULT;

volatile pmuv13_r51_t PMUv13_REG_CNF = PMUv13_R51_DEFAULT;
volatile pmuv13_r52_t PMUv13_REG_IRQ = PMUv13_R52_DEFAULT;

volatile sntv6_r08_t SNTv6_REG_TMR_CNF    = SNTv6_R08_DEFAULT;
volatile sntv6_r17_t SNTv6_REG_WUP_CNF    = SNTv6_R17_DEFAULT;
volatile sntv6_r19_t SNTv6_REG_WUP_THRESE = SNTv6_R19_DEFAULT;
volatile sntv6_r1A_t SNTv6_REG_WUP_THRES  = SNTv6_R1A_DEFAULT;

// NOTE: Below is for use with RDCv2. 
//  It has not been verified with RDCv4,
//  and it will NOT work with RDCv4.
//volatile rdcv4_r10_t RDCv4_REG_BRDC_IRQ_1 = RDCv4_R10_DEFAULT;
//volatile rdcv4_r11_t RDCv4_REG_BRDC_IRQ_2 = RDCv4_R11_DEFAULT;
//volatile rdcv4_r12_t RDCv4_REG_BRDC_RST   = RDCv4_R12_DEFAULT;
//volatile rdcv4_r13_t RDCv4_REG_BRDC_ISOL  = RDCv4_R13_DEFAULT;

// Select Testing
volatile uint32_t do_cycle0  = 1; // GOCEP GEN_IRQ Check
volatile uint32_t do_cycle1  = 1; // RDC (not implemented for RDCv4)
volatile uint32_t do_cycle2  = 1; // Member Layer (SNT) with pending INT_VECTOR
volatile uint32_t do_cycle3  = 1; // Pending M0 IRQ Handling (NO_SLEEP_WITH_PEND_IRQ=1)
volatile uint32_t do_cycle4  = 1; // Pending M0 IRQ Handling (NO_SLEEP_WITH_PEND_IRQ=0)
volatile uint32_t do_cycle5  = 0; // 
volatile uint32_t do_cycle6  = 0; // 
volatile uint32_t do_cycle7  = 0; // 
volatile uint32_t do_cycle8  = 0; // 
volatile uint32_t do_cycle9  = 0; // 
volatile uint32_t do_cycle10 = 0; // 
volatile uint32_t do_cycle11 = 1; // Watch-Dog Timer

//*******************************************************************
// Function Definition
//*******************************************************************
void selective_sleep_with_timer (uint32_t threshold, uint32_t pattern);

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
    irq_history |= (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    arb_debug_reg(IRQ_TIMER32, 0x00000000);
    arb_debug_reg(IRQ_TIMER32, (0x10 << 24) | *REG1); // TIMER32_CNT
    arb_debug_reg(IRQ_TIMER32, (0x20 << 24) | *REG2); // TIMER32_STAT
    }
void handler_ext_int_timer16(void) { // TIMER16
    irq_history |= (0x1 << IRQ_TIMER16);
    *REG1 = *TIMER16_CNT;
    *REG2 = *TIMER16_STAT;
    *TIMER16_STAT = 0x0;
    arb_debug_reg(IRQ_TIMER16, 0x00000000);
    arb_debug_reg(IRQ_TIMER16, (0x10 << 24) | *REG1); // TIMER16_CNT
    arb_debug_reg(IRQ_TIMER16, (0x20 << 24) | *REG2); // TIMER16_STAT
    }
void handler_ext_int_reg0(void) { // REG0
    irq_history |= (0x1 << IRQ_REG0);
    arb_debug_reg(IRQ_REG0, 0x00000000);
}
void handler_ext_int_reg1(void) { // REG1
    irq_history |= (0x1 << IRQ_REG1);
    arb_debug_reg(IRQ_REG1, 0x00000000);
}
void handler_ext_int_reg2(void) { // REG2
    irq_history |= (0x1 << IRQ_REG2);
    arb_debug_reg(IRQ_REG2, 0x00000000);
}
void handler_ext_int_reg3(void) { // REG3
    irq_history |= (0x1 << IRQ_REG3);
    arb_debug_reg(IRQ_REG3, 0x00000000);
}
void handler_ext_int_reg4(void) { // REG4
    irq_history |= (0x1 << IRQ_REG4);
    arb_debug_reg(IRQ_REG4, 0x00000000);
    selective_sleep_with_timer(400, 0xFFFF);
}
void handler_ext_int_reg5(void) { // REG5
    irq_history |= (0x1 << IRQ_REG5);
    arb_debug_reg(IRQ_REG5, 0x00000000);
    selective_sleep_with_timer(500, 0xFFFF);
}
void handler_ext_int_reg6(void) { // REG6
    irq_history |= (0x1 << IRQ_REG6);
    arb_debug_reg(IRQ_REG6, 0x00000000);
}
void handler_ext_int_reg7(void) { // REG7
    irq_history |= (0x1 << IRQ_REG7);
    arb_debug_reg(IRQ_REG7, 0x00000000);
}
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    irq_history |= (0x1 << IRQ_MBUS_MEM);
    arb_debug_reg(IRQ_MBUS_MEM, 0x00000000);
}
void handler_ext_int_mbusrx(void) { // MBUS_RX
    irq_history |= (0x1 << IRQ_MBUS_RX);
    arb_debug_reg(IRQ_MBUS_RX, 0x00000000);
}
void handler_ext_int_mbustx(void) { // MBUS_TX
    irq_history |= (0x1 << IRQ_MBUS_TX);
    arb_debug_reg(IRQ_MBUS_TX, 0x00000000);
}
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    irq_history |= (0x1 << IRQ_MBUS_FWD);
    arb_debug_reg(IRQ_MBUS_FWD, 0x00000000);
}
void handler_ext_int_gocep(void) { // GOCEP
    irq_history |= (0x1 << IRQ_GOCEP);
    arb_debug_reg(IRQ_GOCEP, 0x00000000);
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    irq_history |= (0x1 << IRQ_SOFT_RESET);
    arb_debug_reg(IRQ_SOFT_RESET, 0x00000000);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    irq_history |= (0x1 << IRQ_WAKEUP);
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

    // Set Halt
    set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(NODE_A_ADDR);
    mbus_enumerate(SNT_ADDR);
    mbus_enumerate(RDC_ADDR);
    mbus_enumerate(MRR_ADDR);
    mbus_enumerate(SRR_ADDR);
    mbus_enumerate(PMU_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();
}

void snt_tmr_reset (void) {
    // Reset registers to default
    SNTv6_REG_TMR_CNF.as_int    = SNTv6_R08_DEFAULT_AS_INT;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);
}

void snt_wup_reset (void) {
    // Reset registers to default
    SNTv6_REG_WUP_CNF.as_int    = SNTv6_R17_DEFAULT_AS_INT;
    SNTv6_REG_WUP_THRESE.as_int = SNTv6_R19_DEFAULT_AS_INT;
    SNTv6_REG_WUP_THRES.as_int  = SNTv6_R1A_DEFAULT_AS_INT;
    mbus_remote_register_write(SNT_ADDR, 0x17, SNTv6_REG_WUP_CNF.as_int);
    mbus_remote_register_write(SNT_ADDR, 0x19, SNTv6_REG_WUP_THRESE.as_int);
    set_halt_until_mbus_trx();
    mbus_remote_register_write(SNT_ADDR, 0x1A, SNTv6_REG_WUP_THRES.as_int);
    set_halt_until_mbus_tx();
}

void snt_tmr_start (void) {
    // Configure the 5kHz Timer
    SNTv6_REG_TMR_CNF.TMR_SLEEP        = 0;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);

    SNTv6_REG_TMR_CNF.TMR_ISOLATE      = 0;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);

    SNTv6_REG_TMR_CNF.TMR_RESETB_DCDC  = 1;
    SNTv6_REG_TMR_CNF.TMR_RESETB_DIV   = 1;
    SNTv6_REG_TMR_CNF.TMR_RESETB       = 1;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);

    SNTv6_REG_TMR_CNF.TMR_EN_OSC       = 1;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);

    SNTv6_REG_TMR_CNF.TMR_EN_SELF_CLK  = 1;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);

    SNTv6_REG_TMR_CNF.TMR_EN_OSC       = 0;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);
}

void snt_wup_start (uint32_t threshold, uint32_t auto_reset, uint32_t num_regs) {
    // Configure the Wakeup Timer
    SNTv6_REG_WUP_CNF.WUP_INT_RPLY_REG_ADDR   = 0x04;
    SNTv6_REG_WUP_CNF.WUP_INT_RPLY_SHORT_ADDR = 0x10;
    SNTv6_REG_WUP_CNF.WUP_INT_RPLY_NUM_REGS_1 = num_regs-1;
    SNTv6_REG_WUP_CNF.WUP_ENABLE_CLK_SLP_OUT  = 0;
    SNTv6_REG_WUP_CNF.WUP_CLK_SEL             = 0;
    SNTv6_REG_WUP_CNF.WUP_AUTO_RESET          = auto_reset;
    SNTv6_REG_WUP_CNF.WUP_IRQ_EN              = 1;
    SNTv6_REG_WUP_CNF.WUP_ENABLE              = 1;
    mbus_remote_register_write(SNT_ADDR, 0x17, SNTv6_REG_WUP_CNF.as_int);

    // Configure the Wakeup Timer
    SNTv6_REG_WUP_THRESE.WUP_THRESHOLD_EXT = (threshold >> 24);
    mbus_remote_register_write(SNT_ADDR, 0x19, SNTv6_REG_WUP_THRESE.as_int);

    SNTv6_REG_WUP_THRES.WUP_THRESHOLD = threshold & 0x00FFFFFF;
    set_halt_until_mbus_trx();
    mbus_remote_register_write(SNT_ADDR, 0x1A, SNTv6_REG_WUP_THRES.as_int);
    set_halt_until_mbus_tx();

}

void snt_wup_stop () {
    SNTv6_REG_WUP_CNF.WUP_ENABLE = 0;
    mbus_remote_register_write(SNT_ADDR, 0x17, SNTv6_REG_WUP_CNF.as_int);
}

void snt_tmr_stop () {
    // Turn off the Timer
    SNTv6_REG_TMR_CNF.TMR_EN_SELF_CLK  = 0;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);

    SNTv6_REG_TMR_CNF.TMR_RESETB_DCDC  = 0;
    SNTv6_REG_TMR_CNF.TMR_RESETB_DIV   = 0;
    SNTv6_REG_TMR_CNF.TMR_RESETB       = 0;
    SNTv6_REG_TMR_CNF.TMR_ISOLATE      = 1;
    SNTv6_REG_TMR_CNF.TMR_SLEEP        = 1;
    mbus_remote_register_write(SNT_ADDR, 0x08, SNTv6_REG_TMR_CNF.as_int);
}

void sleep_with_timer (uint32_t threshold) {
    set_wakeup_timer(threshold, 1, 1);
    mbus_sleep_all();
    while(1);
}

// Using Selective Sleep Msg: pattern[n] corresponds to Short Prefix = n
//  NOTE: pattern[0] and pattern[15] are ignored by MBus.
void selective_sleep_with_timer (uint32_t threshold, uint32_t pattern) {
    set_wakeup_timer(threshold, 1, 1);
    mbus_write_message32(0x01, (0x2 << 28) | (pattern << 12)); // Selective Sleep by Short Prefix
    while(1);
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
        arb_debug_reg(0x50, 0x00000000);
    
        if (irq_history == ((0x1 << IRQ_WAKEUP) | (0x1 << IRQ_GOCEP))) {
        //if (irq_history == ((0x1 << IRQ_WAKEUP))) {   // Use this when you use 'load_prog', instead of 'ep_send_prog' or 'goc_send_prog'.
               pass (0x0, irq_history); irq_history = 0; disable_all_irq(); }
        else { fail (0x0, irq_history); disable_all_irq(); }
    } 
}

void cycle1 (void) { 
    if (do_cycle1 == 1) { 
        arb_debug_reg(0x51, 0x00000000);

// NOTE: Below is for use with RDCv2. 
//  It has not been verified with RDCv4,
//  and it will NOT work with RDCv4.
//
//        // Set up RDC IRQ Configuration
//        RDCv4_REG_BRDC_IRQ_2.BRDC_IRQ_LENGTH_1 = 3;
//        RDCv4_REG_BRDC_IRQ_2.BRDC_IRQ_PAYLOAD_ADDR = 0x20;
//        mbus_remote_register_write(RDC_ADDR, 0x11, RDCv4_REG_BRDC_IRQ_2.as_int);
//
//        // Un-isolate BRDC
//        RDCv4_REG_BRDC_ISOL.BRDC_ISOLATE = 0;
//        RDCv4_REG_BRDC_ISOL.BRDC_V3P6_PG_EN = 0;
//        RDCv4_REG_BRDC_ISOL.BRDC_V3P6_ISOLB = 1;
//        RDCv4_REG_BRDC_ISOL.BRDC_V1P2_PG_EN = 0;
//        RDCv4_REG_BRDC_ISOL.BRDC_OSC_V1P2_PG_EN = 0;
//        RDCv4_REG_BRDC_ISOL.BRDC_OSC_ISOLATE = 0;
//        RDCv4_REG_BRDC_ISOL.BRDC_ISOLATE_DOUT = 0;
//        mbus_remote_register_write(RDC_ADDR, 0x13, RDCv4_REG_BRDC_ISOL.as_int);
//
//        // Start BRDC
//        set_halt_until_mbus_trx();
//        RDCv4_REG_BRDC_RST.BRDC_RSTB = 1;
//        RDCv4_REG_BRDC_RST.BRDC_IB_ENB = 0;
//        RDCv4_REG_BRDC_RST.BRDC_OSC_RESET = 0;
//        mbus_remote_register_write(RDC_ADDR, 0x12, RDCv4_REG_BRDC_RST.as_int);
//
//        set_halt_until_mbus_tx();
//
//        // Isolate BRDC
//        RDCv4_REG_BRDC_ISOL.BRDC_ISOLATE = 1;
//        RDCv4_REG_BRDC_ISOL.BRDC_V3P6_PG_EN = 1;
//        RDCv4_REG_BRDC_ISOL.BRDC_V3P6_ISOLB = 0;
//        RDCv4_REG_BRDC_ISOL.BRDC_V1P2_PG_EN = 1;
//        RDCv4_REG_BRDC_ISOL.BRDC_OSC_V1P2_PG_EN = 1;
//        RDCv4_REG_BRDC_ISOL.BRDC_OSC_ISOLATE = 1;
//        RDCv4_REG_BRDC_ISOL.BRDC_ISOLATE_DOUT = 1;
//        mbus_remote_register_write(RDC_ADDR, 0x13, RDCv4_REG_BRDC_ISOL.as_int);
    } 
}

void cycle2 (void) {
    if (do_cycle2 == 1) { 
        arb_debug_reg(0x52, 0x00000000);

        // WAKEUP_REQ happens in the middle of the Sleep Msg
        if (get_mb_flag(FLAG_SNTWUP, FLAG_SNTWUP_LEN) == 0) {
            set_mb_flag(FLAG_SNTWUP, FLAG_SNTWUP_LEN, 1);

            arb_debug_reg(0x52, 0x00000001);

            snt_tmr_reset();
            snt_wup_reset();
            snt_tmr_start();
            snt_wup_start (/*threshold*/55, /*auto_reset*/0, /*num_regs*/1);

            // Go to Sleep
            delay(100); // Give a specific delay so that the wakeup request from SNT overlaps with the sleep message. It's okay to generate the wakeup request in active (i.e., before sleep message)
            sleep_with_timer(100); // Timer Threshold = 100 (safety feature)
        }
        // WAKEUP_REQ happens after the sleep msg
        else if (get_mb_flag(FLAG_SNTWUP, FLAG_SNTWUP_LEN) == 1) {
            set_mb_flag(FLAG_SNTWUP, FLAG_SNTWUP_LEN, 2);

            arb_debug_reg(0x52, 0x00000002);

            snt_wup_stop();
            snt_wup_start(/*threshold*/30, /*auto_reset*/1, /*num_regs*/1);

            // Go to Sleep
            sleep_with_timer(100); // Timer Threshold = 100 (safety feature)
        }
        // WAKEUP_REQ happens just before the sleep msg
        else if (get_mb_flag(FLAG_SNTWUP, FLAG_SNTWUP_LEN) == 2) {
            set_mb_flag(FLAG_SNTWUP, FLAG_SNTWUP_LEN, 3);

            arb_debug_reg(0x52, 0x00000003);

            snt_wup_stop();
            snt_wup_start(/*threshold*/33, /*auto_reset*/0, /*num_regs*/1);

            // Go to Sleep
            delay(90); // Give a specific delay so that the wakeup request from SNT overlaps with the sleep message. It's okay to generate the wakeup request in active (i.e., before sleep message)
            sleep_with_timer(100); // Timer Threshold = 100 (safety feature)
        }
        // WAKEUP_REQ happens just before the sleep msg and the SNT Msg is 64-bit-long
        else if (get_mb_flag(FLAG_SNTWUP, FLAG_SNTWUP_LEN) == 3) {
            set_mb_flag(FLAG_SNTWUP, FLAG_SNTWUP_LEN, 4);

            arb_debug_reg(0x52, 0x00000004);

            snt_wup_stop();
            snt_wup_start(/*threshold*/34, /*auto_reset*/0, /*num_regs*/2);

            // Go to Sleep
            delay(94); // Give a specific delay so that the wakeup request from SNT overlaps with the sleep message. It's okay to generate the wakeup request in active (i.e., before sleep message)
            sleep_with_timer(100); // Timer Threshold = 100 (safety feature)
        }
        // Terminating the Cycle
        else {
            arb_debug_reg(0x52, 0x00000005);

            snt_tmr_stop();
            snt_wup_stop();
        }
    } 
}

// Pending M0 IRQ Handling
void cycle3 (void) { 
    if (do_cycle3 == 1) { 
        arb_debug_reg(0x53, 0x00000000);

        // Enable REG4/REG5 IRQs (for SNT Wakeup Timer)
        *NVIC_ISER = (0x1 << IRQ_REG4) | (0x1 << IRQ_REG5);

        // M0 IRQ generated way before the sleep message
        if (get_mb_flag(FLAG_PENDIRQ, FLAG_PENDIRQ_LEN) == 0) {
            set_mb_flag(FLAG_PENDIRQ, FLAG_PENDIRQ_LEN, 1);
    
            arb_debug_reg(0x53, 0x00000001);

            snt_tmr_reset();
            snt_wup_reset();
            snt_tmr_start();
            snt_wup_start (/*threshold*/30, /*auto_reset*/0, /*num_regs*/1);

            // Go to Sleep
            delay(100);
            sleep_with_timer(100); // Timer Threshold = 100 (safety feature)

        }
        // M0 IRQ generated right before sending the sleep message
        else if (get_mb_flag(FLAG_PENDIRQ, FLAG_PENDIRQ_LEN) == 1) {
            set_mb_flag(FLAG_PENDIRQ, FLAG_PENDIRQ_LEN, 2);
    
            arb_debug_reg(0x53, 0x00000002);

            snt_wup_stop();
            snt_wup_start(/*threshold*/50, /*auto_reset*/0, /*num_regs*/1);

            // Go to Sleep
            delay(90);
            sleep_with_timer(100); // Timer Threshold = 100 (safety feature)
        }
        // Terminating the cycle
        else {

            arb_debug_reg(0x53, 0x00000003);

            snt_tmr_stop();
            snt_wup_stop();
        }
    } 
}
void cycle4 (void) {
    if (do_cycle4 == 1) { 
        arb_debug_reg(0x54, 0x00000000);

        // NO_SLEEP_WITH_PEND_IRQ=0
        *REG_SYS_CONF = *REG_SYS_CONF & 0xFFFFFDFF;

        // Enable REG4/REG5 IRQs (for SNT Wakeup Timer)
        *NVIC_ISER = (0x1 << IRQ_REG4) | (0x1 << IRQ_REG5);

        // M0 IRQ generated way before the sleep message
        if (get_mb_flag(FLAG_PENDIRQ0, FLAG_PENDIRQ0_LEN) == 0) {
            set_mb_flag(FLAG_PENDIRQ0, FLAG_PENDIRQ0_LEN, 1);
    
            arb_debug_reg(0x54, 0x00000001);

            snt_tmr_reset();
            snt_wup_reset();
            snt_tmr_start();
            snt_wup_start (/*threshold*/30, /*auto_reset*/0, /*num_regs*/1);

            // Go to Sleep
            delay(100);
            sleep_with_timer(100); // Timer Threshold = 100 (safety feature)

        }
        // M0 IRQ generated right before sending the sleep message
        else if (get_mb_flag(FLAG_PENDIRQ0, FLAG_PENDIRQ0_LEN) == 1) {
            set_mb_flag(FLAG_PENDIRQ0, FLAG_PENDIRQ0_LEN, 2);
    
            arb_debug_reg(0x54, 0x00000002);

            snt_wup_stop();
            snt_wup_start(/*threshold*/50, /*auto_reset*/0, /*num_regs*/1);

            // Go to Sleep
            delay(90);
            sleep_with_timer(100); // Timer Threshold = 100 (safety feature)
        }
        // Terminating the cycle
        else {

            arb_debug_reg(0x54, 0x00000003);

            snt_tmr_stop();
            snt_wup_stop();
        }
    } 
}
void cycle5 (void) { if (do_cycle5 == 1) { } }
void cycle6 (void) { if (do_cycle6 == 1) { } }
void cycle7 (void) { if (do_cycle7 == 1) { } }
void cycle8 (void) { if (do_cycle8 == 1) { } }
void cycle9 (void) { if (do_cycle9 == 1) { } }
void cycle10 (void) { if (do_cycle10 == 1) { } }

void cycle11 (void) {
    if (do_cycle11 == 1) {
        arb_debug_reg (0x5B, 0x00000000);
        set_halt_until_mbus_tx();

        // Watch-Dog Timer (You should see TIMERWD triggered)
        arb_debug_reg (0x5B, 0x00000001);
        config_timerwd(100);

        // Wait here until PMU resets everything
        while(1);
    }
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    irq_history = 0;

    // Enable Wake-Up/Soft-Reset/GOCEP IRQs
    *NVIC_ISER = (0x1 << IRQ_WAKEUP) | (0x1 << IRQ_SOFT_RESET) | (0x1 << IRQ_GOCEP);

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


