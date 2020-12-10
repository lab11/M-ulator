//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PREv21E tape-out for verification
//*******************************************************************
#include "PREv21E.h"
#include "FLPv3S.h"
#include "FLPv3S_RF.h"
#include "SNTv5_RF.h"
#include "RDCv4_RF.h"
#include "MRRv11A_RF.h"
#include "SRRv8_RF.h"
#include "PMUv12_RF.h"
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
void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_aes(void) { // AES
    *NVIC_ICPR = (0x1 << IRQ_AES); irq_history |= (0x1 << IRQ_AES);
    arb_debug_reg(IRQ_AES, 0x00000000);
    // Check the error
    if  (  (*AES_TEXT_0 == *(GOC_AES_CT+0))
        && (*AES_TEXT_1 == *(GOC_AES_CT+1))
        && (*AES_TEXT_2 == *(GOC_AES_CT+2))
        && (*AES_TEXT_3 == *(GOC_AES_CT+3))
        ) {
        arb_debug_reg(IRQ_AES, 0x01000000);
        // Send out the CipherText
        mbus_copy_mem_from_local_to_remote_stream(
            1,          // stream_channel
            0xA,        // remote_prefix
            AES_TEXT_0, // local_address
            3           // length_in_words_minus_one
        );
    }
    else {
        arb_debug_reg(IRQ_AES, 0x11000000);
        // Send out the Correct CipherText
        arb_debug_reg(IRQ_AES, 0x12000000);
        mbus_copy_mem_from_local_to_remote_stream(
            0,          // stream_channel
            0xA,        // remote_prefix
            GOC_AES_CT, // local_address
            3           // length_in_words_minus_one
        );
        // Send out the Wrong CipherText
        arb_debug_reg(IRQ_AES, 0x13000000);
        mbus_copy_mem_from_local_to_remote_stream(
            1,          // stream_channel
            0xA,        // remote_prefix
            AES_TEXT_0, // local_address
            3           // length_in_words_minus_one
        );

    }
    arb_debug_reg(IRQ_AES, 0xF0000000);
}

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

    if (*GOC_AES_PASS == 0x50AE5128) {
        *GOC_AES_PASS = 0x0;
        set_aes_key(GOC_AES_KEY);   // Set the Key
        set_aes_pt (GOC_AES_PT);    // Set the PlainText
        // Start the AES
        *AES_GO = 0x1;
    }
    else if (*GOC_AES_PASS == 0xDEADBEEF) {
        *GOC_AES_PASS = 0x0;

        arb_debug_reg (0x8B, 0x00000000);
        set_halt_until_mbus_tx();

        // Watch-Dog Timer (You should see TIMERWD triggered)
        arb_debug_reg (0x8B, 0x00000001);
        config_timerwd(100);

        // Wait here until PMU resets everything
        while(1);
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
    arb_debug_reg(IRQ_WAKEUP, (0x10 << 24) | *SREG_WAKEUP_SOURCE);
    *SCTR_REG_CLR_WUP_SOURCE = 1;
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
    disable_timerwd();
    *REG_MBUS_WD = 0x0;

    // Initialization Sequence
    if (!get_flag(FLAG_ENUM)) {
        initialization();
        mbus_sleep_all();
        while(1);
    }

    // Enable IRQs
    irq_handled = 0;
    *NVIC_ISER = (0x1 << IRQ_GOCEP) | (0x1 << IRQ_WAKEUP) | (0x1 << IRQ_AES);

    // Stay here
    while (1) { WFI(); }

    return 1;
}

