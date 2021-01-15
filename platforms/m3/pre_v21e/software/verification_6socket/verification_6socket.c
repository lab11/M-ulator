//*******************************************************************
//Author: Yejoong Kim
//Description: Silicon Verification for PREv21E
//
//Board: 6-Socket Board (v8_HT)
//Chips: PREv21E -> MEMv3 (w/ DCPv13S)
//
// Behavior upon GOC/EP IRQ:
//
// REG0
// [23:16]  [15:8]  [7:0]   Description
// ----------------------------------------------
// 00       DE      AD      Send a sleep message
// ----------------------------------------------
// 11       00      00      Turn off CPS[0]
// 11       00      01      Turn on  CPS[0]
// 11       01      00      Turn off CPS[1]
// 11       01      01      Turn on  CPS[1]
// 11       02      00      Turn off CPS[2]
// 11       02      01      Turn on  CPS[2]
// ----------------------------------------------
// 12       -       -       Repeat Asynchronous WUP_TIMER reading until it reads counter=0x10000, followed by Synchronous WUP_TIMER Reading
// ----------------------------------------------
// 13       -       -       Repeat Synchronous WUP_TIMER reading until it reads counter=0x10000
// ----------------------------------------------
// 14       -       -       Repeat Asynchronous XO_TIMER reading until it reads counter=0x10000, followed by Synchronous XO_TIMER Reading
//
//*******************************************************************
  
#include "PREv21E.h"
#include "PREv21E_RF.h"
#include "mbus.h"

// uncomment this for debug mbus message
//#define DEBUG_MBUS_MSG

// Short Prefixes
#define MEM_ADDR 0x4

// System parameters
#define	MBUS_DELAY 200 // Amount of delay between successive messages; 200: 6-7ms

// PRE Timer Thresholds
#define TIMERWD_VAL 0 // Disable Watchdog Timer


//********************************************************************
// Global Variables
//********************************************************************
// "static" limits the variables to this file, giving compiler more freedom
// "volatile" should only be used for MMIO --> ensures memory storage
volatile uint32_t enumerated;

// CPU IRQ List
volatile cpu_irq_list_t irq_pending = CPU_IRQ_LIST_DEFAULT;

//***************************************************
// Function Declarations
//***************************************************
void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));

static void operation_init(void);

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************

void handler_ext_int_reg0(void)     { *NVIC_ICPR = (0x1 << IRQ_REG0);   irq_pending.reg0  = 0x1; }
void handler_ext_int_reg1(void)     { *NVIC_ICPR = (0x1 << IRQ_REG1);   irq_pending.reg1  = 0x1; }
void handler_ext_int_reg2(void)     { *NVIC_ICPR = (0x1 << IRQ_REG2);   irq_pending.reg2  = 0x1; }
void handler_ext_int_reg3(void)     { *NVIC_ICPR = (0x1 << IRQ_REG3);   irq_pending.reg3  = 0x1; }
void handler_ext_int_gocep(void)    { *NVIC_ICPR = (0x1 << IRQ_GOCEP);  irq_pending.gocep = 0x1; }
void handler_ext_int_timer32(void)  { *NVIC_ICPR = (0x1 << IRQ_TIMER32);irq_pending.timer32 = 0x1; }
void handler_ext_int_wakeup(void)   { *NVIC_ICPR = (0x1 << IRQ_WAKEUP); irq_pending.wakeup = 0x1; }
void handler_ext_int_aes(void)      { *NVIC_ICPR = (0x1 << IRQ_AES);    irq_pending.aes = 0x1; }

//********************************************************************
// Initialization
//********************************************************************

static void operation_init(void){

    //Enumerate & Initialize Registers
    enumerated = 0x5453104b; // 0x5453 is TS in ascii
  
    //Enumeration
    set_halt_until_mbus_trx();
    mbus_enumerate(MEM_ADDR);
    set_halt_until_mbus_tx();

}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(){

    // Only enable relevant interrupts
	*NVIC_ISER = (1 << IRQ_GOCEP);

    // Disable watchdog timers
    config_timerwd(0);
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
            // CPS Testing
            else if (r0_msb == 0x11) {

                // CPS[0]
                if (r0_mid == 0x00) {
                    if      (r0_lsb == 0x00) *REG_CPS = (*REG_CPS & 0xFFFFFFFE) | 0x00000000;
                    else if (r0_lsb == 0x01) *REG_CPS = (*REG_CPS & 0xFFFFFFFE) | 0x00000001;
                }
                // CPS[1]
                else if (r0_mid == 0x01) {
                    if      (r0_lsb == 0x00) *REG_CPS = (*REG_CPS & 0xFFFFFFFD) | 0x00000000;
                    else if (r0_lsb == 0x01) *REG_CPS = (*REG_CPS & 0xFFFFFFFD) | 0x00000002;
                }
                // CPS[2]
                else if (r0_mid == 0x02) {
                    if      (r0_lsb == 0x00) *REG_CPS = (*REG_CPS & 0xFFFFFFFB) | 0x00000000;
                    else if (r0_lsb == 0x01) *REG_CPS = (*REG_CPS & 0xFFFFFFFB) | 0x00000004;
                }
            }
            // WUP_TIMER Read (Asynchronous)
            else if (r0_msb == 0x12) {
                uint32_t curr_val;
                uint32_t prev_val;

                // Asynchronous
                prev_val =  0;
                while (1) {
                    curr_val = *REG_WUPT_VAL;
                    if (((curr_val - prev_val) > 1) || (curr_val < prev_val)) {
                        mbus_write_message32(0xA1, curr_val);
                        mbus_write_message32(0xA2, prev_val);
                    }
                    prev_val = curr_val;
                    if (curr_val == 0x10000) break;
                }

                mbus_write_message32(0xA0, 0x0EA70000);
                *WUPT_RESET = 0x1;

                // Synchronous
                prev_val = 0;
                while (1) {
                    curr_val = *WUPT_VAL;
                    if (((curr_val - prev_val) > 1) || (curr_val < prev_val)) {
                        mbus_write_message32(0xA1, curr_val);
                        mbus_write_message32(0xA2, prev_val);
                    }
                    prev_val = curr_val;
                    if (curr_val == 0x10000) break;
                }
            }
            // WUP_TIMER Read (Synchronous)
            else if (r0_msb == 0x13) {
                uint32_t curr_val;
                uint32_t prev_val;

                // Synchronous
                prev_val = 0;
                while (1) {
                    curr_val = *WUPT_VAL;
                    if (((curr_val - prev_val) > 1) || (curr_val < prev_val)) {
                        mbus_write_message32(0xA1, curr_val);
                        mbus_write_message32(0xA2, prev_val);
                    }
                    prev_val = curr_val;
                    if (curr_val == 0x10000) break;
                }
            }
            // Start/Stop XO Timer and XO CLK OUT
            else if (r0_msb == 0x14) {

                if (r0_lsb == 0x01) {

                    // Delay to allow starting the External Clock
                    // IT IS RECOMMENDED YOU START THE EXTERNAL CLOCK WHILE THE CODE EXECUTES THIS DELAY.
                    delay(1000);
    
                    // Configure XO_DRIVER to enable OSC_IN passthrough
                    *REG_XO_CONF1 =  (/*XO_SLEEP*/       0 << 22)
                                    |(/*XO_ISOLATE*/     0 << 21)
                                    |(/*XO_EN_DIV*/      1 << 20)
                                    |(/*XO_S*/           5 << 17)   // (w/ 32.768kHz External Clock) 3: 4.096kHz, 5: 1.024kHz
                                    |(/*XO_EN_OUT*/      1 << 15)
                                    |(/*XO_PULSE_SEL*/   1 << 11)
                                    |(/*XO_DELAY_EN*/    0 <<  8)
                                    |(/*XO_DRV_START_UP*/0 <<  7)
                                    |(/*XO_DRV_CORE*/    0 <<  6)
                                    |(/*XO_SCN_CLK_SEL*/ 0 <<  1)
                                    |(/*XO_SCN_ENB*/     0 <<  0);

                    // Enable XO Timer (Counter) with Threshold = 0
                    *REG_XOT_CONFIGU =    (/*XOT_CNT_SAT_UPPER*/0 <<  0);// XOT_CNT_SAT_UPPER=0
                    *REG_XOT_CONFIG  =    (/*XOT_ENABLE*/       1 << 23)
                                        | (/*XOT_MODE*/         0 << 22) // 0: Non-Masking Mode, 1: Masking Mode
                                        | (/*XOT_WREQ_EN*/      0 << 21) // If 1, generates Wakeup Request when the counter expires
                                        | (/*XOT_IRQ_EN*/       0 << 20) // If 1, generates M0 IRQ when the counter expires
                                        | (/*XOT_FORCE_COUT*/   0 << 19) // If 1, outputs XO CLK regardless of internal cout/cnt masking status
                                        | (/*XOT_CNT_SAT_LOWER*/0 <<  0);// Counter Threshold (lower 16-bits)

                    // Start the Counter and COUT
                    start_xo_cnt();
                    start_xo_cout();

                    // Start the testing
                    uint32_t curr_val;
                    uint32_t prev_val;
    
//                    // Asynchronous
//                    prev_val =  0;
//                    while (1) {
//                        curr_val = (*REG_XOT_VAL_U << 16) | (*REG_XOT_VAL_L & 0xFFFF);
//                        if (curr_val < prev_val) {
//                            mbus_write_message32(0xA1, curr_val);
//                            mbus_write_message32(0xA2, prev_val);
//                        }
//                        prev_val = curr_val;
//                        if (curr_val > 0xF0000) break;
//                    }
//    
//                    mbus_write_message32(0xA0, 0x0EA70000);
//                    reset_xo_cnt();
    
                    // Synchronous
                    prev_val = 0;
                    while (1) {
                        curr_val = *XOT_VAL;
                        if (curr_val < prev_val) {
                            mbus_write_message32(0xA1, curr_val);
                            mbus_write_message32(0xA2, prev_val);
                        }
                        prev_val = curr_val;
                        if (curr_val > 0xF0000) break;
                    }
    
                    // Stop the counter and cOUT
                    stop_xo_cnt();  // Stop the counter
                    stop_xo_cout(); // Stop the clock output through pad

                }
                else if (r0_lsb == 0x00) {

                    // Stop the Counter and COUT
                    stop_xo_cnt();
                    stop_xo_cout();

                    // Disable XO Timer (Counter) with Threshold = 0

                    // Stop the XO DRIVER


                }
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
