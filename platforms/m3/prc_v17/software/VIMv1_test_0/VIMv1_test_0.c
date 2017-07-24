//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during VIMv1 tape-out for verification
//*******************************************************************
#include "PRCv17.h"
#include "FLPv2S_RF.h"
#include "VIMv1_RF.h"
#include "PMUv7H_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLP_ADDR    0x4
#define VIM_ADDR    0x5
#define PMU_ADDR    0xE

// FLPv2S Payloads
#define ERASE_PASS  0x4F

// Flag Idx
#define FLAG_ENUM       0

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;

volatile uint32_t mem_rsvd_0[10];
volatile uint32_t mem_rsvd_1[10];

volatile flpv2s_r0F_t FLPv2S_R0F_IRQ      = FLPv2S_R0F_DEFAULT;
volatile flpv2s_r12_t FLPv2S_R12_PWR_CONF = FLPv2S_R12_DEFAULT;
volatile flpv2s_r07_t FLPv2S_R07_GO       = FLPv2S_R07_DEFAULT;

volatile pmuv7h_r51_t PMUv7H_R51_CONF = PMUv7H_R51_DEFAULT;
volatile pmuv7h_r52_t PMUv7H_R52_IRQ  = PMUv7H_R52_DEFAULT;

volatile vimv1_r1A_t VIMv1_R1A_CTRL_POWER    = VIMv1_R1A_DEFAULT;
volatile vimv1_r1B_t VIMv1_R1B_IRQ_SHORT_ADDR= VIMv1_R1B_DEFAULT;
volatile vimv1_r1C_t VIMv1_R1C_IRQ_REG_ADDR  = VIMv1_R1C_DEFAULT;
volatile vimv1_r20_t VIMv1_R20_ENABLE_LC_MEM = VIMv1_R20_DEFAULT;

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
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(VIM_ADDR);
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

void VIMv1_load_imem() {
    // VIMv1_CTRL power must be on and un-isolated to access IMEM
    //---------------------------------------
    // NOTE: CTRL_POWER_CTRL[3]: CTRL_RESET
    //       CTRL_POWER_CTRL[2]: CTRL_ISOLATE
    //       CTRL_POWER_CTRL[1]: CTRL_SLEEP
    //       CTRL_POWER_CTRL[0]: IMEM_SLEEP
    //---------------------------------------

    // Release Sleep
    VIMv1_R1A_CTRL_POWER.CTRL_POWER_CTRL = 0xC;
    mbus_remote_register_write(VIM_ADDR, 0x1A, VIMv1_R1A_CTRL_POWER.as_int);

    // Release Isolation
    VIMv1_R1A_CTRL_POWER.CTRL_POWER_CTRL = 0x8;
    mbus_remote_register_write(VIM_ADDR, 0x1A, VIMv1_R1A_CTRL_POWER.as_int);

    // Enable LC_MEM interface in VIMv1
    VIMv1_R20_ENABLE_LC_MEM.ENABLE_LC_MEM = 0x1;
    mbus_remote_register_write(VIM_ADDR, 0x20, VIMv1_R20_ENABLE_LC_MEM.as_int);

    // Copy the instruction from Flash (Total 2kB)
    set_halt_until_mbus_fwd();
    mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, (uint32_t *) 0x0, VIM_ADDR, (uint32_t *) 0x0, 0x1FF);
    set_halt_until_mbus_tx();

    // Disable LC_MEM interface in VIMv1
    VIMv1_R20_ENABLE_LC_MEM.ENABLE_LC_MEM = 0x0;
    mbus_remote_register_write(VIM_ADDR, 0x20, VIMv1_R20_ENABLE_LC_MEM.as_int);
}

void VIMv1_start() {
    //---------------------------------------
    // NOTE: CTRL_POWER_CTRL[3]: CTRL_RESET
    //       CTRL_POWER_CTRL[2]: CTRL_ISOLATE
    //       CTRL_POWER_CTRL[1]: CTRL_SLEEP
    //       CTRL_POWER_CTRL[0]: IMEM_SLEEP
    //---------------------------------------

    // Release Reset; this should start the clock
    VIMv1_R1A_CTRL_POWER.CTRL_POWER_CTRL = 0x0;
    mbus_remote_register_write(VIM_ADDR, 0x1A, VIMv1_R1A_CTRL_POWER.as_int);
}

void VIMv1_stop() {
    // Power off the controller 
    //---------------------------------------
    // NOTE: CTRL_POWER_CTRL[3]: CTRL_RESET
    //       CTRL_POWER_CTRL[2]: CTRL_ISOLATE
    //       CTRL_POWER_CTRL[1]: CTRL_SLEEP
    //       CTRL_POWER_CTRL[0]: IMEM_SLEEP
    //---------------------------------------

    // Assert Reset; this should stop the clock
    VIMv1_R1A_CTRL_POWER.CTRL_POWER_CTRL = 0x8;
    mbus_remote_register_write(VIM_ADDR, 0x1A, VIMv1_R1A_CTRL_POWER.as_int);

    // Assert Isolation
    VIMv1_R1A_CTRL_POWER.CTRL_POWER_CTRL = 0xC;
    mbus_remote_register_write(VIM_ADDR, 0x1A, VIMv1_R1A_CTRL_POWER.as_int);

    // Assert Sleep
    VIMv1_R1A_CTRL_POWER.CTRL_POWER_CTRL = 0xF;
    mbus_remote_register_write(VIM_ADDR, 0x1A, VIMv1_R1A_CTRL_POWER.as_int);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Enable Wake-Up IRQ & REG7 IRQ 
    *NVIC_ISER = ((0x1 << IRQ_WAKEUP) | (0x1 << IRQ_REG7));

    // Initialization Sequence
    if (!get_flag(FLAG_ENUM)) { 
        initialization();
    }

    // Testing Sequence
    VIMv1_load_imem();
    VIMv1_start();

    while (1) {
        WFI();
        if (*REG7 == 0xABC) break;
    }

    VIMv1_stop();

    // Sleep/Wakeup OR Terminate operation
    cyc_num = 999;
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
