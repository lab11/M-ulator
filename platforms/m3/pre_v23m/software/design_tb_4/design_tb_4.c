//*******************************************************************
// C SOURCE CODE FOR PRC/PRE VERILOG TESTBENCH
// design_tb_4 (PRE(E))
//-------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jul 11 2022 -   PRCv23 Family
//                      - Member Layer Version Change
//                          SNTv5  -> SNTv6
//                          PMUv12 -> PMUv13
//                      - handler_ext_int_gocep()
//                          Added '(1<<9) // NO_SLEEP_WITH_PEND_IRQ' in setting *REG_SYS_CONF
//  Nov 03 2022 -   Added support for PREv23M
//-------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@umich.edu)
//*******************************************************************
#include "PREv23M.h"
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

// Flag Idx
#define FLAG_ENUM       0
#define FLAG_VCO        1

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;
volatile uint32_t irq_handled;
volatile uint32_t temp_val;

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
    arb_debug_reg(IRQ_GOCEP, *REG0);

    // Copy *REG0 and reset the original
    uint32_t reg0_temp = *REG0;
    *REG0 = 0;

    // Simple delay
    if (reg0_temp == 0x000001) {
        if (*REG1 == 0) delay (1000);
        else delay(*REG1);
    }
    // Set up GPIO IRQ
    else if (reg0_temp == 0x000002) {

        // WAKEUP_ON_PEND_IRQ = 0
        *REG_SYS_CONF =   (1 << 9)  // NO_SLEEP_WITH_PEND_IRQ
                        | (0 << 8)  // ENABLE_SOFT_RESET
                        | (1 << 7)  // PUF_CHIP_ID_SLEEP
                        | (1 << 6)  // PUF_CHIP_ID_ISOLATE
                        | (0 << 4)  // WAKEUP_ON_PEND_REQ (GIT)
                        | (0 << 3)  // WAKEUP_ON_PEND_REQ (GPIO)
                        | (0 << 2)  // WAKEUP_ON_PEND_REQ (XO TIMER)
                        | (0 << 1)  // WAKEUP_ON_PEND_REQ (WUP TIMER)
                        | (0 << 0); // WAKEUP_ON_PEND_REQ (GOC/EP)

        // GPIO Direction
        *GPIO_DIR = 0xFE;

        // GPIO Freeze Output & Direction; WIRQ Setting
        *REG_PERIPHERAL = (/*FREEZE*/ 0x1 << 16) | (/*WIRQ_POSEDGE*/ 0x1 << 12) | (/*WIRQ_NEGEDGE*/ 0x0 << 8) | (/*PAD_ENABLE*/ 0x1 << 0);

    }
    // Clears pending wakeup request
    else if (reg0_temp == 0x000003) {
        mbus_write_message32(0xBB, 0xBBBBBBBB);
        *SCTR_REG_CLR_WUP_PEND_REQ = 0x8; // Clear GPIO pending wakeup request
    }
    // Fire watchdog timer
    else if (reg0_temp == 0xFFFFFF) {
        arb_debug_reg (0x7B, 0x00000000);

        // Watch-Dog Timer (You should see TIMERWD triggered)
        arb_debug_reg (0x7B, 0x00000001);
        config_timerwd(100);

        // Wait here until PMU resets everything
        while(1);
    }
    // Terminate the simulation
    else if (reg0_temp == 0x00DEAD) {
        *REG_CHIP_ID = 0xFFFFFF;
        while(1);
    }
    // Not a meaningful input
    else {
        mbus_write_message32(0xAA, 0xAAAAAAAA);
    }

    // Finish up the service routine
    arb_debug_reg(IRQ_GOCEP, 0xF0000000);
    irq_handled = 1;
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); irq_history |= (0x1 << IRQ_SOFT_RESET);
    arb_debug_reg(IRQ_SOFT_RESET, 0x00000000);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); irq_history |= (0x1 << IRQ_WAKEUP);
    uint32_t wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1;
    arb_debug_reg(IRQ_WAKEUP, (0x10 << 24) | wakeup_source);

    // If waken up by GPIO[0], go into sleep.
    if (wakeup_source == 0x108) {
        cyc_num++;
        mbus_sleep_all();
        while(1);
    }
    // If it is from GIT (GOC Instant Trigger):
    // Actually, you may want to check the bit[5] only. 
    //  It may be possible that other sources trigger the wakeup at the same time,
    //  for example, the wakeup timer expires around the same time the GIT pulse happens.
    //  Then, both bit[5] (=GIT) and bit[1] (=WUP Timer) may be 1.
    else if (wakeup_source == 0x20) {
        // Display Reg0x08 (REG_GOC_CONFIG). It must be 0x0.
        arb_debug_reg(IRQ_WAKEUP, (0x11 << 24) | *REG_GOC_CONFIG);
        // Disable Wakeup timer
        set_wakeup_timer(0, 0, 1);
        // Go to sleep
        mbus_sleep_all();
        while(1);
    }
    // If the wake up is from the wakeup timer while GIT being enabled
    else if ((wakeup_source == 0x2) && (*REG_GOC_CONFIG == 0x10000)) {
        // Display GIT_ENABLE information
        arb_debug_reg(IRQ_WAKEUP, (0x12 << 24) | *REG_GOC_CONFIG);
        // Setting up Wakeup timer
        set_wakeup_timer(10, 1, 1);
        // Go to sleep
        mbus_sleep_all();
        while(1);
    }
    // Something wrong happend while GIT being enabled
    else if (*REG_GOC_CONFIG == 0x10000) {
        arb_debug_reg(IRQ_WAKEUP, (0x12 << 24) | 0x00DEAD);
        while(1);
    }
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
    mbus_enumerate(SNT_ADDR);
    mbus_enumerate(RDC_ADDR);
    mbus_enumerate(MRR_ADDR);
    mbus_enumerate(SRR_ADDR);
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

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Initialization Sequence
    if (!get_flag(FLAG_ENUM)) {
        initialization();
        // Enable GIT (GOC Instant Trigger)
        *REG_GOC_CONFIG = 0x10000;
        // Setting up Wakeup timer
        set_wakeup_timer(10, 1, 1);
        // Go to sleep
        mbus_sleep_all();
        while(1);
    }

    // Display cycle number for debugging purpose
    arb_debug_reg(0x20, cyc_num);

    // Enable IRQs
    irq_handled = 0;
    *NVIC_ISER = (0x1 << IRQ_GOCEP) | (0x1 << IRQ_WAKEUP);

    // Wait until the IRQ has been handled, then go to sleep
    while (1) {
        if (irq_handled) {
            cyc_num++;
            mbus_sleep_all();
            break;
        }
    }

    //Never Quit (should not come here.)
    while(1){          
        arb_debug_reg(0xFF, 0xDEADBEEF);
        asm("nop;"); 
    }

    return 1;
}

