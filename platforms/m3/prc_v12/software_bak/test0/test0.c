//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12_FLSv1L
//*******************************************************************
#include "PRCv12.h"
#include "FLSv1L_RF.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLS_ADDR    0x4

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t num_cycle;

// Register File (Comment out unused ones)
volatile prcv12_r08_t prcv12_r08 = PRCv12_R08_DEFAULT; // CHIP ID
//volatile prcv12_r09_t prcv12_r09 = PRCv12_R09_DEFAULT; // MBUS THRESHOLD
volatile prcv12_r0A_t prcv12_r0A = PRCv12_R0A_DEFAULT; // MBUS IRQ REGISTER
//volatile prcv12_r0B_t prcv12_r0B = PRCv12_R0B_DEFAULT; // CLK GEN TUNING
//volatile prcv12_r0C_t prcv12_r0C = PRCv12_R0C_DEFAULT; // SRAM TUNING
//volatile prcv12_r0D_t prcv12_r0D = PRCv12_R0D_DEFAULT; // WUP TIMER SETTING
volatile prcv12_r10_t prcv12_r10 = PRCv12_R10_DEFAULT; // RUN CPU

// FLS Register File
volatile flsv1l_r05_t flsv1l_r05 = FLSv1L_R05_DEFAULT; // FLS GO Register
volatile flsv1l_r0C_t flsv1l_r0C = FLSv1L_R0C_DEFAULT; // FLS IRQ Addresses

//*******************************************************************
// INTERRUPTS
//*******************************************************************
void init_interrupt (void) {
  *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
  //*NVIC_ISER = 0x7FFF; //Enable Interrupts
  *NVIC_ISER = 0x0; //Disable Interrupts
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
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000002);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_3(void) { // REG1
    *NVIC_ICPR = (0x1 << 3); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000003);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_4(void) { // REG2
    *NVIC_ICPR = (0x1 << 4); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000004);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_5(void) { // REG3
    *NVIC_ICPR = (0x1 << 5); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000005);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_6(void) { // REG4
    *NVIC_ICPR = (0x1 << 6); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000006);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_7(void) { // REG5  
    *NVIC_ICPR = (0x1 << 7); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000007);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_8(void) { // REG6
    *NVIC_ICPR = (0x1 << 8); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000008);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_9(void) { // REG7
    *NVIC_ICPR = (0x1 << 9); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000009);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_10(void) { // MEM WR
    *NVIC_ICPR = (0x1 << 10); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000010);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_11(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << 11); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000011);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_12(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << 12); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000012);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_13(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << 13); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000013);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}
void handler_ext_int_14(void) { // GOCEP
    *NVIC_ICPR = (0x1 << 14); 
    uint32_t prcv_irq_ctrl_reg = *REG_IRQ_CTRL;
    *REG_IRQ_CTRL = (HALT_UNTIL_MBUS_TX << 12);
    mbus_write_message32(0xEE, 0x00000014);
    *REG_IRQ_CTRL = prcv_irq_ctrl_reg;
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    num_cycle = 0;

    //Chip ID
    prcv12_r08.CHIP_ID = 0xDEAD;
    write_config_reg(0x8,prcv12_r08.as_int);
    
    //// NOTE ////
    // Register Access through prcv12_r08.CHIP_ID 
    // does not work!!












    //Set Halt
    //set_halt_until_mbus_rx();
    *((volatile uint32_t *) 0xA0000028) = 0x00019000; // Reg0x0A, enable backward-compatibility, halt_until_mbus_rx, disable all other irqs

    //Enumeration
    //mbus_enumerate(FLS_ADDR);
    *((volatile uint32_t *) 0xA0003000) = 0x24000000; // Enumeration (0x4)

    //Set Halt
    //set_halt_until_mbus_tx();
    *((volatile uint32_t *) 0xA0000028) = 0x0001A000; // Reg0x0A, enable backward-compatibility, halt_until_mbus_tx, disable all other irqs
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

    // Display num_cycle;
    mbus_write_message32(0xEF, num_cycle);
    
    *((volatile uint32_t *) 0xA0003EF0) = 0xCCCCCCCC;

    // Testing Sequence
    if (num_cycle == 0) {
        mbus_write_message32(0xEF, 0xAAAAAAAA);
    }
    else if (num_cycle == 1) {
        mbus_write_message32(0xEF, 0xBBBBBBBB);
        delay(10000);
    }
    else {
        mbus_write_message32(0xEF, 0x0EA7F00D);
        while(1);
    }
        
    num_cycle++;

    set_wakeup_timer(10, 1, 1);
    mbus_write_message32(0x01, 0x00000000);
    //mbus_sleep_all();

    while(1);

    return 1;
}

