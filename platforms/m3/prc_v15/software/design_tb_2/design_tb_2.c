//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PRCv15 tape-out for verification
//*******************************************************************

//***************************************
// ARB DEBUG REGISTER MESSAGES
//***************************************
// 0xAAAAAANN: Interrupt Port N is triggered
// 0xFFNNNNNN: Current session is Nth deep-sleep cycle (starting from 1)
// 0xEENNNNNN: Current session is Nth normal-sleep cycle (starting from 0)
// 0xCC000000: Going into while(1) waiting for MBus Watchdog
// 0xCC1000NN: Starting sending a long MBus Message (ID: N)
// 0xCC2000NN: Going into while(1) waiting for GOC Hard Reset
// 0xDEE956E9: Deep-Sleep is requested
// 0x0EA7F00D: pass() is called
//                  Pass ID
//                  0x771000NN : Sim finishes normally (ID = N)
// 0xDEADBEEF: fail() is called. The messages below will follow.
//             0xNNNNNNNN: Fail ID
//             0xNNNNNNNN: Fail Data
//                  Fail ID
//                  0xDD1000NN : PMU Register Read Check (Reg ID = N)
//                  0xDD2000NN : Cycle2 Fail during Nth check
//                  0xDD3000NN : Cycle3 Fail during Nth check
//                  0xDD4000NN : Cycle4 Fail during Nth check
//                  0xDD7000NN : Cycle7 Fail during Nth check
//                  0xDD8000NN : Cycle8 Fail during Nth check
//                  0xDDFFFFNN : It is not supposed to be here (ID: N)
//*********************************


//***************************************
// HEADER FILES AND COMPILER DIRECTIVES
//***************************************
#include "PRCv15.h"
#include "FLPv2S_RF.h"
#include "PMUv5H.h"
#include "PMUv5H_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLP_ADDR    0x2
#define NODE_A_ADDR 0x3
#define RAD_ADDR    0x4
#define PMU_ADDR    0x5

#define MBUS_DEBUG  0xDD

// FLPv2S Payloads
#define ERASE_PASS  0x4F

// Sleep Cycle
#define NORM_SLEEP_CYCLE 10     // Normal Sleep Cycle
#define DEEP_SLEEP_CYCLE 20     // Deep Sleep Cycle

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t cyc_num;
volatile uint32_t dslp_cyc_num;
volatile uint32_t irq_history;

volatile flpv2s_r0F_t FLPv2S_R0F_IRQ      = FLPv2S_R0F_DEFAULT;
volatile flpv2s_r12_t FLPv2S_R12_PWR_CONF = FLPv2S_R12_DEFAULT;
volatile flpv2s_r07_t FLPv2S_R07_GO       = FLPv2S_R07_DEFAULT;
volatile flpv2s_r09_t FLPv2S_R09_FLSH_ADDR= FLPv2S_R09_DEFAULT;

//********************************************************************
// Debug Functions
//********************************************************************
void show_info (uint32_t info) {
    uint32_t old_config = set_halt_until_mbus_tx();
    arb_debug_reg (info);
    mbus_write_message32(MBUS_DEBUG, info);
    set_halt_config(old_config);
}

void pass (uint32_t id, uint32_t data) {
    show_info (0x0EA7F00D);
    show_info (id);
    show_info (data);
}

void fail (uint32_t id, uint32_t data) {
    show_info (0xDEADBEEF);
    show_info (id);
    show_info (data);
    *REG_CHIP_ID = 0xFFFF; // This will stop the verilog sim.
}

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
void handler_ext_int_16(void) __attribute__ ((interrupt ("IRQ")));

// TIMER32
void handler_ext_int_0(void) { *NVIC_ICPR = (0x1 << 0);
    irq_history |= (0x1 << 0);
    *REG0 = 0x1000;
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    show_info (0xAAAAAA00);
    show_info ((0xA1 << 24) | *REG0); // 0x1000
    show_info ((0xA1 << 24) | *REG1); // TIMER32_CNT
    show_info ((0xA1 << 24) | *REG2); // TIMER32_STAT
    }
// TIMER16
void handler_ext_int_1(void) { *NVIC_ICPR = (0x1 << 1);
    irq_history |= (0x1 << 1);
    *REG0 = 0x1001;
    *REG1 = *TIMER16_CNT;
    *REG2 = *TIMER16_STAT;
    *TIMER16_STAT = 0x0;
    show_info (0xAAAAAA01);
    show_info ((0xA1 << 24) | *REG0); // 0x1001
    show_info ((0xA1 << 24) | *REG1); // TIMER16_CNT
    show_info ((0xA1 << 24) | *REG2); // TIMER16_STAT
    }
// REG0 - REG7
void handler_ext_int_2(void) { *NVIC_ICPR = (0x1 << 2); irq_history |= (0x1 << 2); show_info (0xAAAAAA02); } // REG0
void handler_ext_int_3(void) { *NVIC_ICPR = (0x1 << 3); irq_history |= (0x1 << 3); show_info (0xAAAAAA03); } // REG1
void handler_ext_int_4(void) { *NVIC_ICPR = (0x1 << 4); irq_history |= (0x1 << 4); show_info (0xAAAAAA04); } // REG2
void handler_ext_int_5(void) { *NVIC_ICPR = (0x1 << 5); irq_history |= (0x1 << 5); show_info (0xAAAAAA05); } // REG3
void handler_ext_int_6(void) { *NVIC_ICPR = (0x1 << 6); irq_history |= (0x1 << 6); show_info (0xAAAAAA06); } // REG4
void handler_ext_int_7(void) { *NVIC_ICPR = (0x1 << 7); irq_history |= (0x1 << 7); show_info (0xAAAAAA07); } // REG5
void handler_ext_int_8(void) { *NVIC_ICPR = (0x1 << 8); irq_history |= (0x1 << 8); show_info (0xAAAAAA08); } // REG6
void handler_ext_int_9(void) { *NVIC_ICPR = (0x1 << 9); irq_history |= (0x1 << 9); show_info (0xAAAAAA09); } // REG7
// MBUS Interrupts
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); irq_history |= (0x1 << 10); show_info (0xAAAAAA0A); } // MBUS_MEM_WR
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); irq_history |= (0x1 << 11); show_info (0xAAAAAA0B); } // MBUS_RX
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); irq_history |= (0x1 << 12); show_info (0xAAAAAA0C); } // MBUS_TX
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); irq_history |= (0x1 << 13); show_info (0xAAAAAA0D); } // MBUS_FWD
// GOCEP
void handler_ext_int_14(void) { *NVIC_ICPR = (0x1 << 14);
    irq_history |= (0x1 << 14);
    show_info (0xAAAAAA0E);
    if (*REG_MBUS_FLAG == 0x1) {
        show_info (0xA0000078);
        show_info (*REG_MBUS_FLAG);
    }
}
// PRE Only
void handler_ext_int_15(void) { *NVIC_ICPR = (0x1 << 15); irq_history |= (0x1 << 15); show_info (0xAAAAAA0F); } // SPI
void handler_ext_int_16(void) { *NVIC_ICPR = (0x1 << 16); irq_history |= (0x1 << 16); show_info (0xAAAAAA10); } // GPIO

//*******************************************************************
// PMU Functions
//*******************************************************************
inline static void mbus_pmu_register_write(uint32_t reg_id, uint32_t reg_data){
    uint32_t old_config = set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_id,reg_data); 
    set_halt_config(old_config);
}

inline static void mbus_pmu_register_read(uint32_t reg_id){
    uint32_t old_config = set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,0x00,reg_id); 
    set_halt_config(old_config);
}

void mbus_pmu_register_read_check (uint32_t reg_id, uint32_t expected) {
    mbus_pmu_register_read(reg_id);
    if (*REG0 != expected) { fail ((0xDD100000 | reg_id), expected);}
}

void set_dslp_osc_reg (uint32_t reg_val) {
    uint32_t old_config = set_halt_until_mbus_trx();
	mbus_remote_register_write(PMU_ADDR, DSLP_OSC_REG, reg_val);
    set_halt_config(old_config);
}

void set_dslp_gen_reg (uint32_t reg_val) {
    uint32_t old_config = set_halt_until_mbus_tx();
	mbus_remote_register_write(PMU_ADDR, DSLP_GEN_REG_UPPER, (reg_val >> 16));
    set_halt_until_mbus_trx();
	mbus_remote_register_write(PMU_ADDR, DSLP_GEN_REG_LOWER, (reg_val & 0x0000FFFF));
    set_halt_config(old_config);
}

void set_dslp_cycle (uint32_t cycle) {
    uint32_t old_config = set_halt_until_mbus_tx();
	mbus_remote_register_write(PMU_ADDR, DSLP_CYCLE_UPPER, (cycle >> 16));
    set_halt_until_mbus_trx();
	mbus_remote_register_write(PMU_ADDR, DSLP_CYCLE_LOWER, (cycle & 0x0000FFFF));
    set_halt_config(old_config);
}

uint32_t get_dslp_osc_reg (void) {
    uint32_t old_config = set_halt_until_mbus_trx();
	mbus_remote_register_read(PMU_ADDR, DSLP_OSC_REG, 0x00);
    uint32_t lower = *REG0;
    set_halt_config(old_config);
    return lower;
}

uint32_t get_dslp_gen_reg (void) {
    uint32_t old_config = set_halt_until_mbus_trx();
	mbus_remote_register_read(PMU_ADDR, DSLP_GEN_REG_LOWER, 0x00);
    uint32_t lower = *REG0;
	mbus_remote_register_read(PMU_ADDR, DSLP_GEN_REG_UPPER, 0x00);
    uint32_t upper = *REG0;
    set_halt_config(old_config);
    return ((upper << 16) | lower);
}

uint32_t get_dslp_cycle (void) {
    uint32_t old_config = set_halt_until_mbus_trx();
	mbus_remote_register_read(PMU_ADDR, DSLP_CYCLE_LOWER, 0x00);
    uint32_t lower = *REG0;
	mbus_remote_register_read(PMU_ADDR, DSLP_CYCLE_UPPER, 0x00);
    uint32_t upper = *REG0;
    set_halt_config(old_config);
    return ((upper << 16) | lower);
}

void go_deep_sleep (uint32_t sleep_cycle) {
    // Stop and Re-start Deep Sleep Timer
    set_halt_until_mbus_tx();
    uint32_t dslp_conf = get_dslp_osc_reg();
    set_dslp_osc_reg(dslp_conf & 0xFFFFFFFE);   // Stop the Timer
    set_dslp_cycle(sleep_cycle);           // Set the number of cycle
    set_dslp_osc_reg(dslp_conf | 0x00000001);   // Start the Timer

    // Send Deep-Sleep Request
    show_info(0xDEE956E9);
    set_halt_until_mbus_trx();
	mbus_remote_register_write(PMU_ADDR, DSLP_REQ, DSLP_KEY);
    set_halt_until_mbus_tx();
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    cyc_num = 0;
    irq_history = 0;

    // Set Halt
    set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(NODE_A_ADDR);
    mbus_enumerate(RAD_ADDR);
    mbus_enumerate(PMU_ADDR);

    // Set Halt
    set_halt_until_mbus_tx();

    // Update the Deep Sleep Cycle Number
    dslp_cyc_num = get_dslp_gen_reg();
    dslp_cyc_num++;
    set_dslp_gen_reg(dslp_cyc_num);
}

void go_sleep (uint32_t sleep_cycle) {
    cyc_num++;
    set_wakeup_timer(sleep_cycle, 1, 1);
    mbus_sleep_all();
}

void go_indefinite_sleep (void) {
    set_wakeup_timer(0, 0, 1);
    mbus_sleep_all();
}

//********************************************************************
// Sub-Tasks
//********************************************************************

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    //Initialize Interrupts
    *NVIC_ISER = /*SPI & GPIO IRQ*/ (0x3 << 15) | /*GOCEP IRQ*/ (0x1  << 14);

    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization();
    }

    // Default Values
    mbus_remote_register_write(RAD_ADDR,0x00,
          ( 0x00 << 0  ) // RADIO_TUNE_POWER
        | ( 0x0  << 5  ) // RADIO_TUNE_FREQ1
        | ( 0x0  << 9  ) // RADIO_TUNE_FREQ2
        | ( 0x1  << 13 ) // RADIO_EXT_CTRL_EN
        | ( 0x1  << 14 ) // RADIO_EXT_OSC_EN
        | ( 0x0  << 15 ) // RADIO_TUNE_TX_TIME
        | ( 0x3F << 18 ) // RADIO_TUNE_CURRENT_LIMITER
    );

    delay(1000);

    // Change TUNE_FREQ1
    mbus_remote_register_write(RAD_ADDR,0x00,
          ( 0x00 << 0  ) // RADIO_TUNE_POWER
        | ( 0x1  << 5  ) // RADIO_TUNE_FREQ1
        | ( 0x0  << 9  ) // RADIO_TUNE_FREQ2
        | ( 0x1  << 13 ) // RADIO_EXT_CTRL_EN
        | ( 0x1  << 14 ) // RADIO_EXT_OSC_EN
        | ( 0x0  << 15 ) // RADIO_TUNE_TX_TIME
        | ( 0x3F << 18 ) // RADIO_TUNE_CURRENT_LIMITER
    );

    delay(1000);

    // Change TUNE_FREQ1
    mbus_remote_register_write(RAD_ADDR,0x00,
          ( 0x00 << 0  ) // RADIO_TUNE_POWER
        | ( 0x2  << 5  ) // RADIO_TUNE_FREQ1
        | ( 0x0  << 9  ) // RADIO_TUNE_FREQ2
        | ( 0x1  << 13 ) // RADIO_EXT_CTRL_EN
        | ( 0x1  << 14 ) // RADIO_EXT_OSC_EN
        | ( 0x0  << 15 ) // RADIO_TUNE_TX_TIME
        | ( 0x3F << 18 ) // RADIO_TUNE_CURRENT_LIMITER
    );

    delay(1000);

    go_indefinite_sleep();


    //--------------------------------------------------------------------
    // Shoud not come here...
    //--------------------------------------------------------------------
    while(1){  //Never Quit (should not come here.)
        show_info (0xDEADBEEF);
        asm("nop;"); 
    }

    return 1;
}

