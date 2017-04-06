//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PRCv14 tape-out for verification
//*******************************************************************
#include "PRCv14.h"
#include "FLPv2S_RF.h"
#include "PMUv3H_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLP_ADDR    0x4
#define NODE_A_ADDR 0x8
#define NODE_B_ADDR 0xC
#define PMU_ADDR    0xE

// FLPv2S Payloads
#define ERASE_PASS  0x4F

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t cyc_num;
volatile uint32_t irq_history;

volatile flpv2s_r0F_t FLPv2S_R0F_IRQ      = FLPv2S_R0F_DEFAULT;
volatile flpv2s_r12_t FLPv2S_R12_PWR_CONF = FLPv2S_R12_DEFAULT;
volatile flpv2s_r07_t FLPv2S_R07_GO       = FLPv2S_R07_DEFAULT;

volatile pmuv3h_r52_t PMUv3_R52_IRQ  = PMUv3H_R52_DEFAULT;

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

void handler_ext_int_0(void) { *NVIC_ICPR = (0x1 << 0); // TIMER32
    irq_history |= (0x1 << 0);
    *REG0 = 0x1000;
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    arb_debug_reg (0xA0000000);
    arb_debug_reg ((0xA1 << 24) | *REG0); // 0x1000
    arb_debug_reg ((0xA1 << 24) | *REG1); // TIMER32_CNT
    arb_debug_reg ((0xA1 << 24) | *REG2); // TIMER32_STAT
    }
void handler_ext_int_1(void) { *NVIC_ICPR = (0x1 << 1); // TIMER16
    irq_history |= (0x1 << 1);
    *REG0 = 0x1001;
    *REG1 = *TIMER16_CNT;
    *REG2 = *TIMER16_STAT;
    *TIMER16_STAT = 0x0;
    arb_debug_reg (0xA0000001);
    arb_debug_reg ((0xA1 << 24) | *REG0); // 0x1001
    arb_debug_reg ((0xA1 << 24) | *REG1); // TIMER16_CNT
    arb_debug_reg ((0xA1 << 24) | *REG2); // TIMER16_STAT
    }
void handler_ext_int_2(void) { *NVIC_ICPR = (0x1 << 2); // REG0
    irq_history |= (0x1 << 2);
    arb_debug_reg (0xA0000002);
}
void handler_ext_int_3(void) { *NVIC_ICPR = (0x1 << 3); // REG1
    irq_history |= (0x1 << 3);
    arb_debug_reg (0xA0000003);
}
void handler_ext_int_4(void) { *NVIC_ICPR = (0x1 << 4); // REG2
    irq_history |= (0x1 << 4);
    arb_debug_reg (0xA0000004);
}
void handler_ext_int_5(void) { *NVIC_ICPR = (0x1 << 5); // REG3
    irq_history |= (0x1 << 5);
    arb_debug_reg (0xA0000005);
}
void handler_ext_int_6(void) { *NVIC_ICPR = (0x1 << 6); // REG4
    irq_history |= (0x1 << 6);
    arb_debug_reg (0xA0000006);
}
void handler_ext_int_7(void) { *NVIC_ICPR = (0x1 << 7); // REG5
    irq_history |= (0x1 << 7);
    arb_debug_reg (0xA0000007);
}
void handler_ext_int_8(void) { *NVIC_ICPR = (0x1 << 8); // REG6
    irq_history |= (0x1 << 8);
    arb_debug_reg (0xA0000008);
}
void handler_ext_int_9(void) { *NVIC_ICPR = (0x1 << 9); // REG7
    irq_history |= (0x1 << 9);
    arb_debug_reg (0xA0000009);
}
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); // MEM WR
    irq_history |= (0x1 << 10);
    arb_debug_reg (0xA000000A);
}
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); // MBUS_RX
    irq_history |= (0x1 << 11);
    arb_debug_reg (0xA000000B);
}
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); // MBUS_TX
    irq_history |= (0x1 << 12);
    arb_debug_reg (0xA000000C);
}
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); // MBUS_FWD
    irq_history |= (0x1 << 13);
    arb_debug_reg (0xA000000D);
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    cyc_num = 0;
    irq_history = 0;

    // Set Halt
    set_halt_until_mbus_rx();

    // Enumeration
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(NODE_A_ADDR);
    mbus_enumerate(NODE_B_ADDR);
    mbus_enumerate(PMU_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();
}

void pass (uint32_t id, uint32_t data) {
    arb_debug_reg (0x0EA7F00D);
    arb_debug_reg (id);
    arb_debug_reg (data);
}

void fail (uint32_t id, uint32_t data) {
    arb_debug_reg (0xDEADBEEF);
    arb_debug_reg (id);
    arb_debug_reg (data);
    *REG_CHIP_ID = 0xFFFF; // This will stop the verilog sim.
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    //Initialize Interrupts
    disable_all_irq();

    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization();
    }

    // Read from PMU General Register
    set_halt_until_mbus_rx();
    mbus_copy_registers_from_remote_to_local(PMU_ADDR, 0x4B, 0x00, 1);

    set_halt_until_mbus_tx();
    uint32_t cyc_num = (*REG0 << 16) | *REG1;

    // Send MBus Messages to let the world know!
    uint32_t idx;
    for (idx=0; idx < cyc_num+1; idx++) {
        mbus_write_message32 (0xAA, idx);
    }

    if (cyc_num == 9) *REG_CHIP_ID = 0xFFFF; // This will stop the verilog sim.

    // Update PMU General Register
    cyc_num++;
    uint32_t cyc_num_upper = cyc_num >> 16;
    uint32_t cyc_num_lower = (cyc_num << 16) >> 16;
    mbus_remote_register_write (PMU_ADDR, 0x4B, cyc_num_upper);
    mbus_remote_register_write (PMU_ADDR, 0x4C, cyc_num_lower);
    
    // Configure Deep Sleep Cycle
    uint32_t dslp_cycle = 100;
    uint32_t dslp_cycle_upper = dslp_cycle >> 16;
    uint32_t dslp_cycle_lower = (dslp_cycle << 16) >> 16;
    mbus_remote_register_write (PMU_ADDR, 0x4D, dslp_cycle_upper);
    mbus_remote_register_write (PMU_ADDR, 0x4E, dslp_cycle_lower);

    // Send Deep-Sleep Request
    mbus_remote_register_write (PMU_ADDR, 0x4F, 0xDEAD);

    while(1){  //Never Quit (should not come here.)
        asm("nop;"); 
    }

    return 1;
}

