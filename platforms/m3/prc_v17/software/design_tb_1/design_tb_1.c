//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PRCv17 tape-out for verification
//*******************************************************************
#include "PRCv17.h"
#include "FLPv2S_RF.h"
#include "PMUv7H_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLP_ADDR    0x4
#define NODE_A_ADDR 0x8
#define NODE_B_ADDR 0xC
#define PMU_ADDR    0xE

// FLPv2S Payloads
#define ERASE_PASS  0x4F

// Flag Idx
#define FLAG_ENUM        0
#define FLAG_GPIO_SUB_0  1
#define FLAG_GPIO_SUB_1  2
#define FLAG_XO_SUB      3
#define FLAG_SOFT_RESET  4
#define FLAG_GOCEP_SUB   5

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

// Select Testing
#ifdef PREv17
volatile uint32_t do_cycle0  = 1; // Wakeup through GPIO (only for PRE)
volatile uint32_t do_cycle1  = 1; // Wakeup through XO (only for PRE)
#else
volatile uint32_t do_cycle0  = 0;
volatile uint32_t do_cycle1  = 0;
#endif
volatile uint32_t do_cycle2  = 1; // Soft-Reset
volatile uint32_t do_cycle3  = 1; // GOC in Active
volatile uint32_t do_cycle4  = 1; // GOC in Sleep
volatile uint32_t do_cycle5  = 1; // GOC Write Access
volatile uint32_t do_cycle6  = 1; // GOC Read Access
volatile uint32_t do_cycle7  = 0; // 
volatile uint32_t do_cycle8  = 0; // 
volatile uint32_t do_cycle9  = 0; // 
volatile uint32_t do_cycle10 = 0; // 
volatile uint32_t do_cycle11 = 1; // Watch-Dog Timer

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
    mbus_enumerate(NODE_A_ADDR);
    mbus_enumerate(NODE_B_ADDR);
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
    #ifdef PREv17
        arb_debug_reg(0x40, 0x00000000);

        //----------------------------------------------------------------------------------------------------------    
        // Posedge WIRQ
        if ((get_flag(FLAG_GPIO_SUB_1)==0) & (get_flag(FLAG_GPIO_SUB_0)==0)) {

            set_flag(FLAG_GPIO_SUB_1, 0);
            set_flag(FLAG_GPIO_SUB_0, 1);

            // Configure GPIO
            gpio_init(0xF0); // [7:4] Output, [3:0] Input
            config_gpio_posedge_wirq (0xF);
            config_gpio_negedge_wirq (0x0);
            freeze_gpio_out();

            arb_debug_reg(0x40, 0x00000001);
            mbus_sleep_all();
        }
        //----------------------------------------------------------------------------------------------------------    
        // Negedge WIRQ
        else if ((get_flag(FLAG_GPIO_SUB_1)==0) & (get_flag(FLAG_GPIO_SUB_0)==1)) {

            set_flag(FLAG_GPIO_SUB_1, 1);
            set_flag(FLAG_GPIO_SUB_0, 0);

            // Configure GPIO
            gpio_init(0xF0); // [7:4] Output, [3:0] Input
            config_gpio_posedge_wirq (0x0);
            config_gpio_negedge_wirq (0xF);
            freeze_gpio_out();

            arb_debug_reg(0x40, 0x00000002);
            mbus_sleep_all();
        }
        //----------------------------------------------------------------------------------------------------------    
        // No Edge Enabled
        else if ((get_flag(FLAG_GPIO_SUB_1)==1) & (get_flag(FLAG_GPIO_SUB_0)==0)) {

            set_flag(FLAG_GPIO_SUB_1, 1);
            set_flag(FLAG_GPIO_SUB_0, 1);

            // Configure GPIO
            gpio_init(0xF0); // [7:4] Output, [3:0] Input
            config_gpio_posedge_wirq (0x0);
            config_gpio_negedge_wirq (0x0);
            freeze_gpio_out();

            arb_debug_reg(0x40, 0x00000003);
            set_wakeup_timer(100, 1, 1);
            mbus_sleep_all();
        }
        //----------------------------------------------------------------------------------------------------------    
        // End of Testing
        else if ((get_flag(FLAG_GPIO_SUB_1)==1) & (get_flag(FLAG_GPIO_SUB_0)==1)) {
            arb_debug_reg(0x40, 0x00000004);
        }

    #endif
    }
}

void cycle1 (void) {
    if (do_cycle1 == 1) {
    #ifdef PREv17
        arb_debug_reg(0x41, 0x00000000);

        //----------------------------------------------------------------------------------------------------------    
        // XO Timer
        if (!get_flag(FLAG_XO_SUB)) {

            set_flag(FLAG_XO_SUB, 1);

            // Configure XO
            // In reality, the XO driver must be properly configured and started
            arb_debug_reg(0x41, 0x00000001);
            set_wakeup_timer (100, 0, 1);
            set_xo_timer (100, 1, 1);
            mbus_sleep_all();
        }
        //----------------------------------------------------------------------------------------------------------    
        // End of Testing
        else {
            uint32_t temp_val = 0;

            while (temp_val == 0) { temp_val = *XOT_VAL;}
            arb_debug_reg(0x41, (0x10 << 24) | temp_val);

            set_xo_timer (100, 0, 1);

            arb_debug_reg(0x41, 0x00000002);
        }

    #endif
    }
}

void cycle2 (void) {
    if (do_cycle2 == 1) {
        arb_debug_reg(0x42, 0x00000000);

        //----------------------------------------------------------------------------------------------------------    
        // Copy from Flash and triggers Soft Reset
        if (!get_flag(FLAG_SOFT_RESET)) {

            set_flag(FLAG_SOFT_RESET, 1);

            *REG_SOFT_RESET = 0;

            arb_debug_reg(0x42, 0x00000001);
            mbus_copy_mem_from_local_to_remote_bulk (FLP_ADDR, 0x0, 0x0, 15);

            set_halt_until_mbus_trx();
            arb_debug_reg(0x42, 0x00000002);
            mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, 0x0, PRC_ADDR, (uint32_t *) 0x6C840000, 15);

            set_halt_until_mbus_tx();

            *REG_SOFT_RESET = 1;

            set_halt_until_mbus_trx();
            arb_debug_reg(0x42, 0x00000003);
            mbus_copy_mem_from_remote_to_any_bulk (FLP_ADDR, 0x0, PRC_ADDR, (uint32_t *) 0x6C840000, 15);

            set_halt_until_mbus_tx();
        }
        //----------------------------------------------------------------------------------------------------------    
        // End of Testing
        else {
            *REG_SOFT_RESET = 0;

            if (irq_history == (0x1 << IRQ_SOFT_RESET)) {
                   pass (0x0, irq_history); irq_history = 0; disable_all_irq(); }
            else { fail (0x0, irq_history); disable_all_irq(); }

            arb_debug_reg(0x42, 0x00000004);
        }
    }
}

void cycle3 (void) {
    if (do_cycle3 == 1) {
        arb_debug_reg(0x43, 0x00000000);
        *REG0 = 0;
        while (*REG0==0);
        disable_all_irq();
    }
}

void cycle4 (void) {
    if (do_cycle2 == 1) {
        arb_debug_reg(0x44, 0x00000000);

        //----------------------------------------------------------------------------------------------------------    
        // Go into indefinite sleep
        if (!get_flag(FLAG_GOCEP_SUB)) {

            set_flag(FLAG_GOCEP_SUB, 1);

            arb_debug_reg(0x44, 0x00000001);
            set_wakeup_timer(5, 0, 1);
            mbus_sleep_all();
        }
        //----------------------------------------------------------------------------------------------------------    
        // After wakeup by GOCEP
        else {
            if (irq_history == ((0x1 << IRQ_WAKEUP) | (0x1 << IRQ_GOCEP))) {
                   pass (0x1, irq_history); irq_history = 0; disable_all_irq(); }
            else { fail (0x1, irq_history); disable_all_irq(); }

            arb_debug_reg(0x44, 0x00000002);
        }
    }
}

void cycle5 (void) { 
    if (do_cycle5 == 1) { 
        arb_debug_reg(0x45, 0x00000000);

        if (*REG0 != 0xDEAD) {
            arb_debug_reg(0x45, 0x00000001);
            set_wakeup_timer(100, 0, 1);
            mbus_sleep_all();
        }
        else {
            *REG0 = 0x123456;
            arb_debug_reg(0x45, 0x00000002);
        }
    } 
}

void cycle6 (void) { 
    if (do_cycle6 == 1) { 
        arb_debug_reg(0x46, 0x00000000);

        if (*REG0 != 0xDEAD) {
            arb_debug_reg(0x46, 0x00000001);
            set_wakeup_timer(100, 0, 1);
            mbus_sleep_all();
        }
        else {
            *REG0 = 0;
            arb_debug_reg(0x46, 0x00000002);
        }
    } 
}

void cycle7 (void) { if (do_cycle7 == 1) { } }
void cycle8 (void) { if (do_cycle8 == 1) { } }
void cycle9 (void) { if (do_cycle9 == 1) { } }
void cycle10 (void) { if (do_cycle10 == 1) { } }

void cycle11 (void) {
    if (do_cycle11 == 1) {
        arb_debug_reg (0x4B, 0x00000000);
        set_halt_until_mbus_tx();

        // Watch-Dog Timer (You should see TIMERWD triggered)
        arb_debug_reg (0x4B, 0x00000001);
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


