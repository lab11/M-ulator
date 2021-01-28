//*******************************************************************
//Author: Yejoong Kim
//Description: Silicon Verification for CISv2
//
//Board: Single-Socket Board
//Chips: PREv21E -> SNTv5 -> MRRv11A -> PMUv12 (CISv2)
//
// Behavior upon GOC/EP IRQ:
//
// REG0
// [23:16]  [15:8]  [7:0]   Description
// ----------------------------------------------
// 00       DE      AD      Send a sleep message
// ----------------------------------------------
// 11       --      --      SNT Timer Reading (Async/Sync must be chose in the code)
// 12       --      --      Print out prev_val and curr_val from the SNT Timer Reading test.
// ----------------------------------------------
//
//*******************************************************************
  
#include "PREv21E.h"
#include "PREv21E_RF.h"
#include "SNTv5_RF.h"
#include "MRRv11A_RF.h"
#include "PMUv12_RF.h"
#include "mbus.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// Short Prefixes
#define SNT_ADDR 0x4
#define MRR_ADDR 0x5
#define PMU_ADDR 0x6

//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;

// CPU IRQ List
volatile cpu_irq_list_t irq_pending = CPU_IRQ_LIST_DEFAULT;

// SNT Register File
volatile sntv5_r08_t sntv5_r08 = SNTv5_R08_DEFAULT;
volatile sntv5_r09_t sntv5_r09 = SNTv5_R09_DEFAULT;
volatile sntv5_r17_t sntv5_r17 = SNTv5_R17_DEFAULT;
volatile sntv5_r19_t sntv5_r19 = SNTv5_R19_DEFAULT;
volatile sntv5_r1A_t sntv5_r1A = SNTv5_R1A_DEFAULT;

//*******************************************************************
// INTERRUPT HANDLERS (Enable only what you need)
//*******************************************************************
//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ"))); 
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ"))); 
void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ"))); 
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ"))); 
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ"))); 
//-------------------------------------------------------------------
//void handler_ext_int_wakeup   (void){ *NVIC_ICPR = (0x1 << IRQ_WAKEUP    ); irq_pending.wakeup     = 0x1; }
//void handler_ext_int_softreset(void){ *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); irq_pending.soft_reset = 0x1; }
void handler_ext_int_gocep    (void){ *NVIC_ICPR = (0x1 << IRQ_GOCEP     ); irq_pending.gocep      = 0x1; }
//void handler_ext_int_timer32  (void){ *NVIC_ICPR = (0x1 << IRQ_TIMER32   ); irq_pending.timer32    = 0x1; }
//void handler_ext_int_timer16  (void){ *NVIC_ICPR = (0x1 << IRQ_TIMER16   ); irq_pending.timer16    = 0x1; }
//void handler_ext_int_mbustx   (void){ *NVIC_ICPR = (0x1 << IRQ_MBUS_TX   ); irq_pending.mbus_tx    = 0x1; }
//void handler_ext_int_mbusrx   (void){ *NVIC_ICPR = (0x1 << IRQ_MBUS_RX   ); irq_pending.mbus_rx    = 0x1; }
//void handler_ext_int_mbusfwd  (void){ *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD  ); irq_pending.mbus_fwd   = 0x1; }
//void handler_ext_int_reg0     (void){ *NVIC_ICPR = (0x1 << IRQ_REG0      ); irq_pending.reg0       = 0x1; }
//void handler_ext_int_reg1     (void){ *NVIC_ICPR = (0x1 << IRQ_REG1      ); irq_pending.reg1       = 0x1; }
//void handler_ext_int_reg2     (void){ *NVIC_ICPR = (0x1 << IRQ_REG2      ); irq_pending.reg2       = 0x1; }
//void handler_ext_int_reg3     (void){ *NVIC_ICPR = (0x1 << IRQ_REG3      ); irq_pending.reg3       = 0x1; }
//void handler_ext_int_reg4     (void){ *NVIC_ICPR = (0x1 << IRQ_REG4      ); irq_pending.reg4       = 0x1; }
//void handler_ext_int_reg5     (void){ *NVIC_ICPR = (0x1 << IRQ_REG5      ); irq_pending.reg5       = 0x1; }
void handler_ext_int_reg6     (void){ *NVIC_ICPR = (0x1 << IRQ_REG6      ); irq_pending.reg6       = 0x1; }
void handler_ext_int_reg7     (void){ *NVIC_ICPR = (0x1 << IRQ_REG7      ); irq_pending.reg7       = 0x1; }
//void handler_ext_int_mbusmem  (void){ *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM  ); irq_pending.mbus_mem   = 0x1; }
//void handler_ext_int_aes      (void){ *NVIC_ICPR = (0x1 << IRQ_AES       ); irq_pending.aes        = 0x1; }
//void handler_ext_int_gpio     (void){ *NVIC_ICPR = (0x1 << IRQ_GPIO      ); irq_pending.gpio       = 0x1; }
//void handler_ext_int_spi      (void){ *NVIC_ICPR = (0x1 << IRQ_SPI       ); irq_pending.spi        = 0x1; }
//void handler_ext_int_xot      (void){ *NVIC_ICPR = (0x1 << IRQ_XOT       ); irq_pending.xot        = 0x1; }

//***************************************************
// Function Declarations
//***************************************************
static void operation_init(void);
void start_snt_timer(void);
void stop_snt_timer(void);
void start_snt_wakeup_counter_free_running(void);
void stop_snt_wakeup_counter(void);


//********************************************************************
// User-Defined Functions
//********************************************************************

static void operation_init(void){

    //Enumerate & Initialize Registers
    enumerated = 0x5453104b; // 0x5453 is TS in ascii
  
    //Enumeration
    set_halt_until_mbus_trx();
    mbus_enumerate(SNT_ADDR);
    mbus_enumerate(MRR_ADDR);
    mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();

}

// Start the SNT Timer
void start_snt_timer(void) {
    // Copied from snt_start_timer_presleep() in TSstack_ondemand_v1.4b
    // --------------------------------------------------------------------------
    sntv5_r08.TMR_SLEEP = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    sntv5_r08.TMR_ISOLATE = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    
    // TIMER SELF_EN Disable 
    sntv5_r09.TMR_SELF_EN = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
    
    // EN_OSC 
    sntv5_r08.TMR_EN_OSC = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    
    // Release Reset 
    sntv5_r08.TMR_RESETB = 0x1;
    sntv5_r08.TMR_RESETB_DIV = 0x1;
    sntv5_r08.TMR_RESETB_DCDC = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    
    // TIMER EN_SEL_CLK Reset 
    sntv5_r08.TMR_EN_SELF_CLK = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
    
    // TIMER SELF_EN 
    sntv5_r09.TMR_SELF_EN = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x09,sntv5_r09.as_int);
    delay(100000); 
    // --------------------------------------------------------------------------
}

// Stop the SNT Timer
void stop_snt_timer(void) {
    sntv5_r08.TMR_SLEEP = 0x1;
    sntv5_r08.TMR_ISOLATE = 0x1;
    sntv5_r08.TMR_EN_OSC = 0x0;
    sntv5_r08.TMR_RESETB = 0x0;
    sntv5_r08.TMR_RESETB_DIV = 0x0;
    sntv5_r08.TMR_RESETB_DCDC = 0x0;
    sntv5_r08.TMR_EN_SELF_CLK = 0x0;
        mbus_remote_register_write(SNT_ADDR,0x08,sntv5_r08.as_int);
}

// Start the SNT Wakeup Counter (Free Running, No Auto-Reset)
void start_snt_wakeup_counter_free_running(void) {
    sntv5_r19.WUP_THRESHOLD_EXT = 0x0;
        mbus_remote_register_write(SNT_ADDR,0x19,sntv5_r19.as_int);
    sntv5_r1A.WUP_THRESHOLD     = 0x0;
        mbus_remote_register_write(SNT_ADDR,0x1A,sntv5_r1A.as_int);
    sntv5_r17.WUP_IRQ_EN     = 0x0;
    sntv5_r17.WUP_AUTO_RESET = 0x0;
        mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
    sntv5_r17.WUP_ENABLE = 0x1;
        mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
}
// Disable the SNT Wakeup Counter
void stop_snt_wakeup_counter(void) {
    sntv5_r17.WUP_ENABLE = 0x0;
        mbus_remote_register_write(SNT_ADDR,0x17,sntv5_r17.as_int);
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){

    uint32_t curr_val=0;
    uint32_t prev_val=0;

    // Only enable relevant interrupts
	*NVIC_ISER = (1 << IRQ_GOCEP) | (1 << IRQ_REG6) | (1 << IRQ_REG7);

    // Disable watchdog timers
    *TIMERWD_GO = 0;
    *REG_MBUS_WD = 0;

    // Initialization sequence
    if (enumerated != 0x5453104b){
        operation_init();
    }

    while (1) {

        // Wait for Interrupt
        WFI();

        // GOC/EP IRQ
        if (irq_pending.gocep) {
            irq_pending.gocep = 0x0;

            uint32_t r0 = *REG0;
            uint32_t r0_msb = (r0 >> 16) & (0x000000FF);
            uint32_t r0_mid = (r0 >>  8) & (0x000000FF);
            uint32_t r0_lsb = (r0 >>  0) & (0x000000FF);

            mbus_write_message32(0xA0, r0);

            // Go to Sleep
            if (r0_msb == 0x00) {
                if ((r0_mid == 0xDE) && (r0_lsb == 0xAD)) break;
            }
            // SNT Timer Read Testing (Async/Sync)
            else if (r0_msb == 0x11) {
                start_snt_timer();  // Start SNT Timer
                start_snt_wakeup_counter_free_running();    // Start the SNT Wakeup Counter (Free Running, No Auto-Reset)

                curr_val = 0;
                prev_val = 0;
                while (1) {
                    //delay(1000);
                    
                    // Asynchronous Reading
                    //      Reg0x1B: CNT_VAL[31:24]
                    //      Reg0x1C: CNT_VAL[23:0]
                    //mbus_copy_registers_from_remote_to_local(SNT_ADDR, 0x1C, 0x06, 0);
                    
                    // Synchronous Reading
                    //      Write a proper value in Reg0x14:
                    //          DATA[23:16] - 8'h0: Synchronous 32-bit Read. Upper 8-bit is sent to DEST_REG; Lower 24-bit is sent to DEST_REG+1.
                    //                        8'h1: Synchronous Lower 24-bit Read. The 24-bit is sent to DEST_REG.
                    //                        8'h2: Synchronous Upper  8-bit Read. The  8-bit is sent to DEST_REG.
                    //          DATA[15: 8] - MBUS_TARGET_ADDR
                    //          DATA[ 7: 0] - DEST_REG
                    mbus_remote_register_write(SNT_ADDR,0x14,0x011006);

                    WFI();
                    curr_val = *REG6;
                    if (curr_val < prev_val) break;
                    prev_val = curr_val;
                }

                stop_snt_timer();   // Turn off SNT Timer
                stop_snt_wakeup_counter();  // Disable SNT Wakeup Counter

                // Print out the result
                mbus_write_message32(0xAA, prev_val);
                mbus_write_message32(0xAB, curr_val);

            }
            // Print out the SNT Timer Result
            else if (r0_msb == 0x12) {
                // Print out the result
                mbus_write_message32(0xAA, prev_val);
                mbus_write_message32(0xAB, curr_val);
            }

            //Operation Done
            mbus_write_message32(0xA0, 0x0EA7F00D);

        }
        // Other IRQ (invalid)
        else {
            mbus_write_message32(0xAF, 0xDEADBEEF);
            break;
        }

    }

    // Go to Sleep
    mbus_sleep_all();

    while(1);
}
