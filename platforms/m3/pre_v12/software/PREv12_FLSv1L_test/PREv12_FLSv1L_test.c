//*******************************************************************
//Author: Yejoong Kim
//Description: PREv12_FLSv1L
//*******************************************************************
#include "PREv12.h"
#include "FLSv1L_RF.h"
#include "mbus.h"

#define PRE_ADDR    0x1
#define FLS_ADDR    0x4
#define NODE_A_ADDR 0x8
#define NODE_B_ADDR 0xC

//********************************************************************
// Global Variables
//********************************************************************
static uint32_t enumerated;
static uint32_t num_cycle;

// Just for testing...
static uint32_t do_cycle1 = 1; // Register test
static uint32_t do_cycle2 = 1; // MEM IRQ
static uint32_t do_cycle3 = 1; // Flash Erase and Backwards Compatibility
static uint32_t do_cycle4 = 1; // Memory Transactions with Flash layer (Bulk Writing)
static uint32_t do_cycle5 = 1; // Memory Transactions with Flash layer (Stream Writing)
static uint32_t do_cycle6 = 1; // Watch-Dog Timer & Timer16 Testing
static uint32_t do_cycle7 = 1; // Timer32 Testing
static uint32_t do_cycle8 = 1; // DIN double-latch debugging
static uint32_t do_cycle9 = 1; // GPIO
static uint32_t do_cycle10 = 1; // SPI

// Register File (Comment out unused ones)
volatile prev12_r08_t prev12_r08 = PREv12_R08_DEFAULT; // CHIP ID
//volatile prev12_r09_t prev12_r09 = PREv12_R09_DEFAULT; // MBUS THRESHOLD
volatile prev12_r0A_t prev12_r0A = PREv12_R0A_DEFAULT; // MBUS IRQ REGISTER
//volatile prev12_r0B_t prev12_r0B = PREv12_R0B_DEFAULT; // CLK GEN TUNING
//volatile prev12_r0C_t prev12_r0C = PREv12_R0C_DEFAULT; // SRAM TUNING
//volatile prev12_r0D_t prev12_r0D = PREv12_R0D_DEFAULT; // WUP TIMER SETTING
volatile prev12_r10_t prev12_r10 = PREv12_R10_DEFAULT; // RUN CPU
volatile prev12_r25_t prev12_r25 = PREv12_R10_DEFAULT; // XO CLOCK SETTING
volatile prev12_r26_t prev12_r26 = PREv12_R10_DEFAULT; // XO TIMER SETTING
volatile prev12_r28_t prev12_r28 = PREv12_R10_DEFAULT; // COTS POWER SWITCH
volatile prev12_r29_t prev12_r29 = PREv12_R10_DEFAULT; // PAD ENABLE

// FLS Register File
volatile flsv1l_r05_t flsv1l_r05 = FLSv1L_R05_DEFAULT; // FLS GO Register
volatile flsv1l_r0C_t flsv1l_r0C = FLSv1L_R0C_DEFAULT; // FLS IRQ Addresses

//*******************************************************************
// INTERRUPTS
//*******************************************************************
void init_interrupt (void) {
  *NVIC_ICPR = 0x1FFFF; //Clear All Pending Interrupts
  *NVIC_ISER = 0x1FFFF; //Enable Interrupts
}

void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_15(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_16(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void) { // TIMER32
    *NVIC_ICPR = (0x1 << 0);
    *REG0 = 0x1000;
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    set_halt_until_mbus_tx();
    mbus_write_message32(0xE0, *REG0); // 0x1000
    mbus_write_message32(0xE1, *REG1); // TIMER32_CNT
    mbus_write_message32(0xE2, *REG2); // TIMER32_STAT
    }
void handler_ext_int_1(void) { // TIMER16
    *NVIC_ICPR = (0x1 << 1);
    *REG0 = 0x1001;
    *REG1 = *TIMER16_CNT;
    *REG2 = *TIMER16_STAT;
    *TIMER16_STAT = 0x0;
    set_halt_until_mbus_tx();
    mbus_write_message32(0xE0, *REG0); // 0x1001
    mbus_write_message32(0xE1, *REG1); // TIMER16_CNT
    mbus_write_message32(0xE2, *REG2); // TIMER16_STAT
    }
void handler_ext_int_2(void) { // REG0
    *NVIC_ICPR = (0x1 << 2); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000002);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_3(void) { // REG1
    *NVIC_ICPR = (0x1 << 3); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000003);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_4(void) { // REG2
    *NVIC_ICPR = (0x1 << 4); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000004);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_5(void) { // REG3
    *NVIC_ICPR = (0x1 << 5); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000005);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_6(void) { // REG4
    *NVIC_ICPR = (0x1 << 6); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000006);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_7(void) { // REG5  
    *NVIC_ICPR = (0x1 << 7); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000007);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_8(void) { // REG6
    *NVIC_ICPR = (0x1 << 8); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000008);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_9(void) { // REG7
    *NVIC_ICPR = (0x1 << 9); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000009);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_10(void) { // MEM WR
    *NVIC_ICPR = (0x1 << 10); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000010);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_11(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << 11); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000011);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_12(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << 12); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000012);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_13(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << 13); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000013);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_14(void) { // GOCEP
    *NVIC_ICPR = (0x1 << 14); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000014);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_15(void) { // SPI IRQ
    *NVIC_ICPR = (0x1 << 15); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000015);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}
void handler_ext_int_16(void) { // GPIO IRQ
    *NVIC_ICPR = (0x1 << 16); 
    uint32_t prev_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000016);
    *REG_IRQ_CTRL = prev_irq_ctrl_reg;
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    num_cycle = 0;

    //Chip ID
    prev12_r08.CHIP_ID = 0xDEAD;
    write_config_reg(0x8,prev12_r08.as_int);

    //Disable all IRQs
    disable_all_mbus_irq();

    //Set Halt
    set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(FLS_ADDR);
    mbus_enumerate(NODE_A_ADDR);
    mbus_enumerate(NODE_B_ADDR);
}

// cycle1: Register Copy and 'IRQ Mask/Halt CPU' Testing
void cycle1 (void){
    if (do_cycle1 == 1) {
        // Set Halt
        set_halt_until_mbus_rx();
        
        // Copy Reg#1 in FLS and put them in Reg#0 in PRE.
        mbus_copy_registers_from_remote_to_local (FLS_ADDR, 0x01, 0x00, 0);

        // Copy Reg#1-4 in FLS and put them in Reg#0-3 in PRE.
        mbus_copy_registers_from_remote_to_local (FLS_ADDR, 0x01, 0x00, 3);
    
        // Enable MBus Tx/Rx IRQ, Enable REG IRQ (4-7)
        set_mbus_irq_reg(0xF0, FALSE, TRUE, TRUE, FALSE, TRUE, HALT_UNTIL_MBUS_RX);
    
        // Copy Reg#1-4 in FLS and put them in Reg#4-7 in PRE. It must generate 4 IRQs due to Reg#4-7 in PRE.
        mbus_copy_registers_from_remote_to_local (FLS_ADDR, 0x01, 0x04, 3);
        // Also, here should be 6 IRQs: MBUS Tx/Rx and REG4-7.
    
        // Disable IRQ in Reg#4-7
        set_mbus_irq_reg(0x00, FALSE, TRUE, TRUE, FALSE, TRUE, HALT_UNTIL_MBUS_RX);
    
        // Copy Reg#1-4 in FLS and put them in Reg#4-7 in PRE. It must not generate REG IRQs
        mbus_copy_registers_from_remote_to_local (FLS_ADDR, 0x01, 0x04, 3);
        // Also, here should be 2 IRQs: MBUS Tx/Rx
    
        // Disable all MBus IRQs
        disable_all_mbus_irq();
        set_halt_until_mbus_rx();
    
        // Copy Reg#4 in FLS and put them in Reg#4. There must be no IRQ.
        mbus_copy_registers_from_remote_to_local (FLS_ADDR, 0x04, 0x04, 0);
    
        // Configure Halt Setting; Now use HALT_UNTIL_REG7
        set_halt_until_reg(7);
    
        // Copy Reg#1-4 in FLS and put them in Reg#4-7 in PRE. It must generate 4 IRQs due to Reg#4-7 in PRE.
        mbus_copy_registers_from_remote_to_local (FLS_ADDR, 0x01, 0x04, 3);
    
        // Configre Halt Setting; Now use HALT_UNTIL_MBUS_TX (maybe the most useful one)
        set_halt_until_mbus_tx();
    }
}

// cycle2: MEM IRQ
void cycle2 (void){
    if (do_cycle2 == 1) {

        // Set IRQ
        set_mbus_irq_reg(0x00, TRUE, FALSE, FALSE, FALSE, TRUE, HALT_UNTIL_MBUS_RX);
        
        // Copy MEM 0x00000000 in FLS and store those in PRE's 0x00000F9C~
        mbus_copy_mem_from_remote_to_any_bulk (FLS_ADDR, 0x00000000, PRE_ADDR,    (uint32_t *) 0x00000F9C, 0);

        // Copy MEM 0x00000000~0x0000003C in FLS and store those in PRE's 0x00000FA0~
        mbus_copy_mem_from_remote_to_any_bulk (FLS_ADDR, 0x00000000, PRE_ADDR,    (uint32_t *) 0x00000FA0, 9);

        // Before streaming, set the pointer to start from.
        write_config_reg(0x1E, 0x000FB0); // Channel 0
        write_config_reg(0x1A, 0x000FA0); // Channel 1

        // Copy MEM 0x00000000 in FLS and store those in PRE's 0x00000F9C~
        mbus_copy_mem_from_remote_to_any_stream (1, FLS_ADDR, 0x00000000, PRE_ADDR, 0);

        // Copy MEM 0x00000000~0x0000003C in FLS and store those in PRE's 0x00000FA0~
        mbus_copy_mem_from_remote_to_any_stream (0, FLS_ADDR, 0x00000000, PRE_ADDR, 9);

        // Configre Halt Setting; Now use HALT_UNTIL_MBUS_TX (maybe the most useful one)
        set_mbus_irq_reg(0x00, FALSE, FALSE, FALSE, FALSE, TRUE, HALT_UNTIL_MBUS_TX);
    }
}

// cycle3: Flash Erase and Backwards-Compatibility Testing
void cycle3 (void){
    if (do_cycle3 == 1) {
        // By default, Backwards Compatibility is turned on.
        
        // Set Halt
        set_halt_until_mbus_tx();
    
    // Do Flash Erase. This writes 0x000074 into Reg#0. No Reg IRQ generated.
        // Set FLS INT_SHORT_PREFIX = 10, REG_ADDR = 00
        flsv1l_r0C.INT_RPLY_REG_ADDR = 0x00;
        flsv1l_r0C.INT_RPLY_SHORT_ADDR = 0x10;
        mbus_remote_register_write(FLS_ADDR, 0x0C, flsv1l_r0C.as_int);
    
        // Set HALT_UNTIL_REG0
        set_halt_until_reg(0);
    
        // Start Flash Erase
        flsv1l_r05.GO = 0x1;
        flsv1l_r05.CMD = 0x4; // Erase Flash
        flsv1l_r05.IRQ_EN = 0x1;
        mbus_remote_register_write(FLS_ADDR, 0x05, flsv1l_r05.as_int);
    
        // Set HALT_UNTIL_MBUS_TX
        set_halt_until_mbus_tx();
    
        // Check REG0 Data (Correct Data: 0x0000_0074)
        mbus_write_message32(0xFF, *REG0);
        
    
    // Do Flash Erase. This writes 0x000074 into Reg#4. A Reg IRQ#4 should be generated.
        // Set FLS INT_SHORT_PREFIX = 10, REG_ADDR = 04
        flsv1l_r0C.INT_RPLY_REG_ADDR = 0x04;
        flsv1l_r0C.INT_RPLY_SHORT_ADDR = 0x10;
        mbus_remote_register_write(FLS_ADDR, 0x0C, flsv1l_r0C.as_int);
    
        // Set HALT_UNTIL_REG4
        set_halt_until_reg(4);
    
        // Start Flash Erase
        flsv1l_r05.GO = 0x1;
        mbus_remote_register_write(FLS_ADDR, 0x05, flsv1l_r05.as_int);
        
    // Do Flash Erase. IRQ Msg heads to Short Prefix = 0x1F.
        // Set HALT_UNTIL_MBUS_TX
        set_halt_until_mbus_tx();
    
        // Set FLS INT_SHORT_PREFIX = 1F, REG_ADDR = 00
        flsv1l_r0C.INT_RPLY_REG_ADDR = 0x00;
        flsv1l_r0C.INT_RPLY_SHORT_ADDR = 0x1F;
        mbus_remote_register_write(FLS_ADDR, 0x0C, flsv1l_r0C.as_int);
    
        // Set HALT_UNTIL_REG7
        set_halt_until_reg(7);
    
        // Start Flash Erase
        flsv1l_r05.GO = 0x1;
        mbus_remote_register_write(FLS_ADDR, 0x05, flsv1l_r05.as_int);
    
        // Set HALT_UNTIL_MBUS_TX
        set_halt_until_mbus_tx();
    
        // Check REG7 Data (Correct Data: 0x0000_0074)
        mbus_write_message32(0xFF, *REG7);
    
    // Turn off the backward compatibility
        set_mbus_irq_reg(0x00, FALSE, FALSE, FALSE, FALSE, FALSE, HALT_UNTIL_MBUS_TX);
    
    // Do Flash Erase. IRQ Msg heads to Short Prefix = 0x1E.
    // It should be ACKed in PRE, but should not be written to any register.
    
        // Set FLS INT_SHORT_PREFIX = 1E, REG_ADDR = 00
        flsv1l_r0C.INT_RPLY_REG_ADDR = 0x00;
        flsv1l_r0C.INT_RPLY_SHORT_ADDR = 0x1E;
        mbus_remote_register_write(FLS_ADDR, 0x0C, flsv1l_r0C.as_int);
    
        // Set HALT_UNTIL_MBUS_RX
        set_halt_until_mbus_rx();
    
        // Start Flash Erase
        flsv1l_r05.GO = 0x1;
        mbus_remote_register_write(FLS_ADDR, 0x05, flsv1l_r05.as_int);
    
        // Set HALT_UNTIL_MBUS_TX
        set_halt_until_mbus_tx();
    
        // Check MBus Flag
        mbus_write_message32(0xFE, *REG_MBUS_FLAG);
    
        // Check REG6 Data (Correct Data: 0x0000_0074)
        mbus_write_message32(0xFF, *REG6);
    
    }
    // Recover the setting for MBUS IRQ
        set_mbus_irq_reg(0x00, FALSE, FALSE, FALSE, FALSE, TRUE, HALT_UNTIL_MBUS_TX);
}

// cycle4: Memory Transactions with Flash layer (Bulk)
void cycle4(void) {
    if (do_cycle4 == 1) {
        // Set Halt
        set_halt_until_mbus_tx();
    
        // Copy SRAM data into Flash SRAM (bulk)
        // Source Addr: 0x00000000 ~ 0x0000000F (16 words)
        // Target Addr: 0x00000000~
        mbus_copy_mem_from_local_to_remote_bulk (FLS_ADDR, 0x00000000, 0x00000000, 15);
    
        // Put the Flash SRAM data on the bus (to NODE_A)
        set_halt_until_mbus_fwd();
        mbus_copy_mem_from_remote_to_any_bulk (FLS_ADDR, 0x00000000, NODE_A_ADDR, 0x00000000, 15);

        // Put the Flash SRAM data (very long) on the bus (to NODE_B)
        set_halt_until_mbus_tx();
        mbus_copy_mem_from_remote_to_any_bulk (FLS_ADDR, 0x00000000, NODE_B_ADDR, 0x00000000, 127);

        // Halt CPU
        set_halt_until_mbus_fwd();  // CPU will resume when an MBus FWD operation is done.
        halt_cpu();                 // Halt CPU!

        // Read FLS's REG#248-252 (0xF8 - 0xFC) (5 Registers) 
        set_halt_until_mbus_rx();
        mbus_copy_registers_from_remote_to_local (FLS_ADDR, 0xF8, 0x00, 4);

        // Put those register values on the bus (to an fake address)
        set_halt_until_mbus_tx();
        mbus_copy_registers_from_local_to_remote (0xE, 0x15, 0x00, 4);
    }
}

// cycle5: Memory Transactions with Flash layer (Stream)
void cycle5(void) {
    if (do_cycle5 == 1) {
        // Set Halt
        set_halt_until_mbus_tx();
    
        // Copy SRAM data into Flash SRAM (stream)
        // From 0x00000000, 542 words // The entire program code
        mbus_copy_mem_from_local_to_remote_stream (0, FLS_ADDR, 0x00000000, 541);
    
        // Put the Flash SRAM data on the bus (to NODE_A)
        set_halt_until_mbus_fwd();
        mbus_copy_mem_from_remote_to_any_stream (0, FLS_ADDR, 0x00000000, NODE_A_ADDR, 541);

        // Put the Flash SRAM data (very long) on the bus (to NODE_B). I will use halt_cpu() here.
        set_halt_until_mbus_tx();
        mbus_copy_mem_from_remote_to_any_stream (1, FLS_ADDR, 0x00000000, NODE_B_ADDR, 541);

        // Halt CPU
        set_halt_until_mbus_fwd();  // CPU will resume when an MBus FWD operation is done.
        halt_cpu();                 // Halt CPU!

        // Set Halt
        set_halt_until_mbus_tx();
    }
}

// cycle6: Watch-Dog Timer & Timer16 Testing
void cycle6(void) {
    if (do_cycle6 == 1) {
        // Set Halt
        set_halt_until_mbus_tx();

        // Watch-Dog Timer (You should see TIMERWD triggered)
        config_timerwd(100);
        delay(200);

        // Reset Watch-Dog Timer
        config_timerwd(0x02FFFFFF); // Default value. Approx. 1min @ 800kHz)

        // Timer16
        config_timer16(0x1000, 0xF000, 0x3, 0x0, 0x0);  // Start Timer16. Trigger when it reaches 0x1000 or 0xF000.

        delay(20000);

        // Reset Timer16
        *TIMER16_GO = 0x0;
        *TIMER16_CNT = 0x0;
    }
}

// cycle7: Timer32 Testing
void cycle7(void) {
    if (do_cycle7 == 1) {
        // Set Halt
        set_halt_until_mbus_tx();

        // Timer32 with ROI
        config_timer32(0x1000, 0x1, 0x0, 0x0); // Start Timer32. Trigger when it reaches 0x100. Reset on Interrupt.

        // You might see several interrupts.
        delay(2000);

        // Reset Timer16
        *TIMER32_GO = 0x0;
        *TIMER32_CNT = 0x0;
    }
}

// cycle8: DIN double-latch debugging
void cycle8(void) {
    if (do_cycle8 == 1) {
        // Set Halt
        set_halt_until_mbus_tx();
    
    // Do Flash Erase. This writes 0x000074 into Reg#0. No Reg IRQ generated.
        // Set FLS INT_SHORT_PREFIX = 10, REG_ADDR = 00
        flsv1l_r0C.INT_RPLY_REG_ADDR = 0x00;
        flsv1l_r0C.INT_RPLY_SHORT_ADDR = 0x10;
        mbus_remote_register_write(FLS_ADDR, 0x0C, flsv1l_r0C.as_int);
    
        // Set HALT_UNTIL_REG0
        set_halt_until_reg(0);
    
        // Start Flash Erase
        flsv1l_r05.GO = 0x1;
        flsv1l_r05.CMD = 0x4; // Erase Flash
        flsv1l_r05.IRQ_EN = 0x1;
        mbus_remote_register_write(FLS_ADDR, 0x05, flsv1l_r05.as_int);

        // Now, there should be an MBus message that FLS sends. Look at the double-latch here.
    
        // Set HALT_UNTIL_MBUS_TX
        set_halt_until_mbus_tx();
    
        // Check REG0 Data (Correct Data: 0x0000_0074)
        mbus_write_message32(0xFF, *REG0);

        // Do the Register Copy Test again (cycle1)
        do_cycle1 = 1;
        cycle1();
        do_cycle1 = 0;
    }
}

// cycle9: GPIO
void cycle9(void) {
    if (do_cycle9 == 1) {
        // Set Halt
        set_halt_until_mbus_tx();

        gpio_init(0xFFF3); // [3:2] is Input. Others are Output
        gpio_write_data(0xF0F0); // 1111 0000 1111 0000
        gpio_kill_bit(15);       // 0111 0000 1111 0000
        gpio_set_bit(1);         // 0111 0000 1111 0010
        gpio_set_2bits(11, 8);   // 0111 1001 1111 0010
        gpio_kill_bit(12);       // 0110 1001 1111 0010
        mbus_write_message32(0xE0, gpio_get_data()); // Send 0x0000_69F2

        gpio_set_irq_mask (0x00000008); // [3] can generate IRQ
        gpio_set_bit(9);         // 0110 1011 1111 0010
        gpio_kill_bit(4);        // 0110 1011 1110 0010
        // Write 1 at [3] @ negedge [4]
        delay(200);
        mbus_write_message32(0xE0, gpio_get_data()); // Send 0x0000_6BEA

        gpio_set_irq_mask (0x00000000); 
        gpio_close();
    }
}

// cycle10: SPI
void cycle10(void) {
    if (do_cycle10 == 1) {
        // Set Halt
        set_halt_until_mbus_tx();

        enable_io_pad();

        *SPI_SSPCR0 = 0x0287; // Motorola Mode
        *SPI_SSPCR1 = 0x2;
        *SPI_SSPCPSR = 0x02; // Clock Prescale Register
        //*SPI_SSPDMACR = 0x3; 
        *SPI_SSPDR = 0x1234;

        delay(1000);

        disable_io_pad();
    }
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //Initialize Interrupts
    init_interrupt();
  
    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization(); // Enumeration. Enable MBus Tx/Rx IRQ.
    }
    
    // Testing Sequence
    if (num_cycle == 0) {/* Already did Enumeration */}
    else if (num_cycle == 1) cycle1(); // Register test
    else if (num_cycle == 2) cycle2(); // MEM IRQ
    else if (num_cycle == 3) cycle3(); // Flash Erase and Backwards Compatibility
    else if (num_cycle == 4) cycle4(); // Memory Transactions with Flash layer (Bulk Writing)
    else if (num_cycle == 5) cycle5(); // Memory Transactions with Flash layer (Stream Writing)
    else if (num_cycle == 6) cycle6(); // Watch-Dog Timer & Timer16 Testing
    else if (num_cycle == 7) cycle7(); // Timer32 Testing
    else if (num_cycle == 8) cycle8(); // DIN Double-Latch Debugging
    else if (num_cycle == 9) cycle9(); // GPIO
    else if (num_cycle == 10) cycle10(); // SPI
    else num_cycle = 999;


    // Sleep/Wakeup OR Terminate operation
    if (num_cycle == 999) {
        prev12_r10.RUN_CPU = 0x0;
        write_config_reg(0x10,prev12_r10.as_int); // Stop CPU. This will stop the verilog sim.
    }
    else {
        num_cycle++;
        set_wakeup_timer(5, 1, 1);
        mbus_sleep_all();
    }

    while(1){  //Never Quit (should not come here.)
        arb_debug_reg (0xDEADBEEF);
        asm("nop;"); 
    }

    return 1;

}

