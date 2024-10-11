//*******************************************************************************************
// MRMv1S Testing Suite
//-------------------------------------------------------------------------------------------
// GOC/EP IRQ 
//-------------------------------------------------------------------------------------------
//
//  ------------------------------------------------------------
//   goc_head = 0x00: Access PRC/PRE SRAM
//  ------------------------------------------------------------
//        goc_data = 0x000000: Set data_tx[n] = goc_data_ext w/ shuffle=0
//        goc_data = 0x000001: Set data_tx[n] = goc_data_ext w/ shuffle=1
//        goc_data = 0x000002: Send data_tx[] out through MBus
//        goc_data = 0x000003: Send data_rx[] out through MBus
//        goc_data = 0x000004: Send data_rand[] out through MBus
//  
//  ------------------------------------------------------------
//   goc_head = 0x01: Basic Power Control
//  ------------------------------------------------------------
//        goc_data = 0x00000n: Turn on Macro#n
//        goc_data = 0x000006: Turn off all macros
//        goc_data = 0x000007: Turn on the LDO
//        goc_data = 0x000008: Turn off the LDO
//        goc_data = 0x00000F: Send MBus Sleep Message
//  
//  ------------------------------------------------------------
//   goc_head = 0x02: Accessing MRM SRAM
//  ------------------------------------------------------------
//        goc_data = 0x000000: Read data_tx[] and Write into MRM SRAM.   i.e., MRM_SRAM[goc_data_ext+n] = data_tx[n], where n=0, ..., BUF_SIZE-1
//        goc_data = 0x000001: Read MRM SRAM and Write into data_rx[].   i.e., data_rx[n] = MRM_SRAM[goc_data_ext+n], where n=0, ..., BUF_SIZE-1
//        goc_data = 0x000002: Repeatedly write data_tx[] into MRM_SRAM. i.e., MRM_SRAM[BUF_SIZE x i + n] = data_tx[n], where n=0, ..., BUF_SIZE-1, and i=0, ..., 511
//        goc_data = 0x000003: Write all-1 in MRM_SRAM. i.e., MRM_SRAM[n] = 0xFFFFFFFF, where n=0, ..., 16383; NOTE: This alters data_tx[].
//        goc_data = 0x000004: Write all-0 in MRM_SRAM. i.e., MRM_SRAM[n] = 0x00000000, where n=0, ..., 16383; NOTE: This alters data_tx[].
//        goc_data = 0x000005: Write goc_data_ext (w/ shuffle) in MRM_SRAM. i.e., MRM_SRAM[n] = goc_ext_data (w/ shuffle), where n=0, ..., 16383; NOTE: This alters data_tx[].
//  
//  ------------------------------------------------------------
//   goc_head = 0x03: Read MRM SRAM and put it on the MBus
//  ------------------------------------------------------------
//        goc_data = n: Read MRM SRAM, starting from Page#(goc_data_ext), for n pages, and put it on MBus.
//  
//  ------------------------------------------------------------
//   goc_head = 0x04: Read MRM MRAM and put it on the MBus
//  ------------------------------------------------------------
//        goc_data = n: Read MRM MRAM, starting from Page#(goc_data_ext), for n pages, and put it on MBus.
//  
//  ------------------------------------------------------------
//   goc_head = 0x07: Ping-Pong (TMC_CLOCK_MODE=0)
//   goc_head = 0x08: Ping-Pong (TMC_CLOCK_MODE=1)
//  ------------------------------------------------------------
//        goc_data = 0x000000: Force stop the Ping-Pong mode.
//        goc_data = 0x000001: Goes into Ping-Pong Mode using DATA_EXT[0], with num_pages = goc_data_ext.
//        goc_data = 0x000002: Goes into Ping-Pong Mode using DATA_EXT[1], with num_pages = goc_data_ext.
//        goc_data = 0x000003: Goes into Ping-Pong Mode using DATA_EXT[1:0], with num_pages = goc_data_ext.
//  
//  ------------------------------------------------------------
//   goc_head = 0x0A: Normal (Non-Ping-Pong) Test Suite (TMC_RST_AUTO_WK=0)
//   goc_head = 0x0B: Normal (Non-Ping-Pong) Test Suite (TMC_RST_AUTO_WK=1)
//  ------------------------------------------------------------
//        goc_data = 0x000000: All-0 Test
//        goc_data = 0x000001: All-1 Test
//        goc_data = 0x000002: Random Data Test
//        goc_data = 0x000003: Run All-0, All-1, Random Data, sequentially.
//        goc_data = other   : Send the result through MBus
//
//  ------------------------------------------------------------
//   goc_head = 0xFF: Miscellaneous
//  ------------------------------------------------------------
//        goc_data = 0xFFFFFF: NOP
//
//-------------------------------------------------------------------------------------------
// Use of Cortex-M0 Vector Table
//-------------------------------------------------------------------------------------------
//
//  ----------------------------------------------------------
//   MEM_ADDR  M0-Usage       PRE-Usage       PRE-Usage       
//                            (GPIO)          (AES)           
//  ----------------------------------------------------------
//    0 (0x00) STACK_TOP      STACK_TOP       STACK_TOP       
//    1 (0x04) RESET_VECTOR   RESET_VECTOR    RESET_VECTOR    
//    2 (0x08) NMI            GOC_DATA_IRQ    GOC_DATA_IRQ    
//    3 (0x0C) HardFault                      GOC_AES_PASS    
//    4 (0x10) RESERVED                       GOC_AES_CT[0]   
//    5 (0x14) RESERVED                       GOC_AES_CT[1]   
//    6 (0x18) RESERVED                       GOC_AES_CT[2]   
//    7 (0x1C) RESERVED                       GOC_AES_CT[3]   
//    8 (0x20) RESERVED                       GOC_AES_PT[0]   
//    9 (0x24) RESERVED                       GOC_AES_PT[1]   
//   10 (0x28) RESERVED                       GOC_AES_PT[2]   
//   11 (0x2C) SVCall                         GOC_AES_PT[3]   
//   12 (0x30) RESERVED                       GOC_AES_KEY[0]  
//   13 (0x34) RESERVED       R_GPIO_DATA     GOC_AES_KEY[1]  
//   14 (0x38) PendSV         R_GPIO_DIR      GOC_AES_KEY[2]  
//   15 (0x3C) SysTick        R_GPIO_IRQ_MASK GOC_AES_KEY[3]  
//
//-------------------------------------------------------------------------------------------
// WAKEUP_SOURCE (wakeup_source) definition
//-------------------------------------------------------------------------------------------
//
//  Use get_bit(wakeup_source, n) to get the current value, where n is 0-5, 8-11 as shown below.
//
//  [31:12] - Reserved
//     [11] - GPIO_PAD[3] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [10] - GPIO_PAD[2] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [ 9] - GPIO_PAD[1] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [ 8] - GPIO_PAD[0] has triggered wakeup (valid only when wakeup_source[3]=1)
//      [7] - Reserved (NOTE: wakeup_source[7] is used to indicate that the previous 'sleep' has been skipped)
//      [6] - Reserved
//      [5] - GIT (GOC Instant Trigger) has triggered wakeup
//              NOTE: If GIT is triggered while the system is in active, the GIT is NOT immediately handled.
//                    Instead, it waits until the system goes in sleep, and then, the (pending) GIT will wake up the system.
//                    Thus, you can safely assume that GIT is (effectively) triggered only while the system is in Sleep.
//      [4] - MBus message has triggered wakeup (e.g., Flash Auto Boot-up)
//      [3] - One of GPIO_PAD[3:0] has triggered wakeup
//      [2] - XO Timer has triggered wakeup
//      [1] - Wake-up Timer has triggered wakeup
//      [0] - GOC/EP has triggered wakeup
//
//------------------------------------------------------------------------------------------- 
// set_halt and IRQ
//-------------------------------------------------------------------------------------------
//
// NOTE: Do NOT enable IRQ_REGn if: 
//                    REGn is a target register that a reply msg writes into 
//              -AND- set_halt_until_mbus_trx() is used
//              -AND- REGn IRQ handler generates an MBus message.
//      Example)
//              PMU Reply message writes into REG0.
//              In main()
//                  *NVIC_ISER = (0x1 << IRQ_REG0);
//                  set_halt_until_mbus_trx();
//                  mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
//                  set_halt_until_mbus_tx();
//              In handler_ext_int_reg0
//                  mbus_write_message32(0x71, 0x3);
//
//              -> The reply msg from PMU writes into REG0, triggering IRQ_REG0, 
//                 which then sends out the msg 0x71, 0x3.
//                 If this happens BEFORE the system releases the halt state, 
//                 the system may still stay in 'set_halt_until_mbus_trx()'
//
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*******************************************************************************************
// HEADER FILES
//*******************************************************************************************

#include "mbus.h"
#include "PREv23E.h"
#include "MRMv1S.h"

//*******************************************************************************************
// COMPILE SETTING
//*******************************************************************************************
//#define DEVEL // Not used for now.

//*******************************************************************************************
// ENUMERATION
//*******************************************************************************************
#define MRM_ADDR 0x4
#define DBG_ADDR 0xA
#define RES_ADDR 0xC

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1

//*******************************************************************************************
// WAKEUP SOURCE DEFINITIONS
//*******************************************************************************************
#define WAKEUP_BY_GOCEP     get_bit(wakeup_source, 0)
#define WAKEUP_BY_WUPT      get_bit(wakeup_source, 1)
#define WAKEUP_BY_XOT       get_bit(wakeup_source, 2)
#define WAKEUP_BY_GPIO      get_bit(wakeup_source, 3)
#define WAKEUP_BY_MBUS      get_bit(wakeup_source, 4)
#define WAKEUP_BY_GIT       get_bit(wakeup_source, 5)
#define WAKEUP_BY_GPIO0     (WAKEUP_BY_GPIO && get_bit(wakeup_source, 8))
#define WAKEUP_BY_GPIO1     (WAKEUP_BY_GPIO && get_bit(wakeup_source, 9))
#define WAKEUP_BY_GPIO2     (WAKEUP_BY_GPIO && get_bit(wakeup_source, 10))
#define WAKEUP_BY_GPIO3     (WAKEUP_BY_GPIO && get_bit(wakeup_source, 11))

volatile uint32_t wakeup_source;            // Wakeup Source. Updated each time the system wakes up. See 'WAKEUP_SOURCE definition'.

//-------------------------------------------------------------------------------------------
// Other Global Variables
//-------------------------------------------------------------------------------------------

volatile uint32_t goc_irq_count;
volatile uint32_t goc_head;
volatile uint32_t goc_data;
volatile uint32_t goc_data_ext;

#define BUF_SIZE 32
volatile uint32_t data_rand[BUF_SIZE];
volatile uint32_t data_tx[BUF_SIZE];
volatile uint32_t data_rx[BUF_SIZE];

volatile uint32_t result_num_page_err[3];
volatile uint32_t result_num_word_err[3];
volatile uint32_t result_num_bit_err[3];
volatile uint32_t result_page_id[3];
volatile uint32_t result_word_offset[3];
volatile uint32_t result_word_correct[3];
volatile uint32_t result_word_actual[3];

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep/IRQ Handling
static void operation_sleep (void);
static void operation_init(void);

//*******************************************************************************************
// FUNCTIONS IMPLEMENTATION
//*******************************************************************************************

//-------------------------------------------------------------------
// Initialization/Sleep Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: operation_sleep
// Args    : none
// Description:
//           Sends the MBus Sleep message
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep (void) {
    mbus_sleep_all(); 
    while(1);
}

//-------------------------------------------------------------------
// Function: operation_init
// Args    : None
// Description:
//           Initializes the system
// Return  : None
//-------------------------------------------------------------------
static void operation_init (void) {

    //-------------------------------------------------
    // Global Variable Initialization
    //-------------------------------------------------
    goc_irq_count = 0;

    //-------------------------------------------------
    // PRE Tuning
    //-------------------------------------------------

    //--- Set CPU & MBus Clock Speeds      Default
    *REG_CLKGEN_TUNE =    (0x0 << 21)   // 1'h0     CLK_GEN_HIGH_FREQ
                        | (0x3 << 18)   // 3'h3     CLK_GEN_DIV_CORE    #Default CPU Freq = 96kHz (pre-pex simulation, TT, 25C)
                        | (0x2 << 15)   // 3'h2     CLK_GEN_DIV_MBC     #Default MBus Freq = 193kHz (pre-pex simulation, TT, 25C)
                        | (0x1 << 13)   // 2'h1     CLK_GEN_RING
                        | (0x0 << 12)   // 1'h0     RESERVED
                        | (0x0 << 10)   // 2'h1     GOC_CLK_GEN_SEL_DIV
                        | (0x5 <<  7)   // 3'h7     GOC_CLK_GEN_SEL_FREQ
                        | (0x0 <<  6)   // 1'h0     GOC_ONECLK_MODE
                        | (0x0 <<  4)   // 2'h0     GOC_SEL_DLY
                        | (0xF <<  0);  // 4'h8     GOC_SEL

    //--- Pending Wakeup Handling          Default
    *REG_SYS_CONF =       (0x0 << 9)    // 1'h1     NO_SLEEP_WITH_PEND_IRQ  #If 1, it replaces the sleep message with a selective wakeup message if there is a pending M0 IRQ. If 0, it goes to sleep even when there is a pending M0 IRQ.
                        | (0x0 << 8)    // 1'h0     ENABLE_SOFT_RESET   #If 1, Soft Reset will occur when there is an MBus Memory Bulk Write message received and the MEM Start Address is 0x2000
                        | (0x1 << 7)    // 1'h1     PUF_CHIP_ID_SLEEP
                        | (0x1 << 6)    // 1'h1     PUF_CHIP_ID_ISOLATE
                        | (0x0 << 0);   // 5'h1E    WAKEUP_ON_PEND_REQ  #[4]: GIT (PRE_E Only), [3]: GPIO (PRE only), [2]: XO TIMER (PRE only), [1]: WUP TIMER, [0]: GOC/EP

    //-------------------------------------------------
    // Release MRMv1S RESET Signal (CPS0)
    //-------------------------------------------------
    set_cps (/*cps_full*/0x1, /*cps_weak*/0x0); // cps_weak is only for CPS2
    
    //-------------------------------------------------
    // Enumeration
    //-------------------------------------------------
    set_halt_until_mbus_trx();
    mbus_enumerate(MRM_ADDR);
    set_halt_until_mbus_tx();

    //-------------------------------------------------
    // MRMv1S Setting
    //-------------------------------------------------
    mrm_init(/*mrm_prefix*/MRM_ADDR, /*irq_reg_idx*/0x0);

    //-------------------------------------------------
    // End of Initialization
    //-------------------------------------------------
    set_flag(FLAG_INITIALIZED, 1);
}

//*******************************************************************************************
// MRAM Test Functions
//*******************************************************************************************

void set_data_tx (uint32_t pattern, uint32_t shuffle) {
    uint32_t i;
    for (i=0; i<BUF_SIZE; i++) {
        data_tx[i] = pattern;
        if (shuffle) pattern = (pattern << 1) | (pattern >> 31);
    }
}

//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_gocep (void) {
    goc_head     = *GOC_DATA_IRQ >> 24;
    goc_data     = *GOC_DATA_IRQ & 0xFFFFFF;
    goc_data_ext = *(GOC_DATA_IRQ+1);
    *GOC_DATA_IRQ = 0xFFFFFFFF;
}

void handler_ext_int_reg0 (void) { 
    disable_reg_irq(/*reg*/0);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

    uint32_t i, j, k, l, t;

    // Disable PRE Watchdog & MBus Watchdog
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;

    // Get the info on who woke up the system
    wakeup_source = *SREG_WAKEUP_SOURCE;
    mbus_write_message32(0x70, wakeup_source);

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    // Enable GOC/EP IRQ
    *NVIC_ISER = (0x1 << IRQ_GOCEP);

    // GOC/EP IRQ
    while(1) {

        WFI();

        goc_irq_count++;
        mbus_write_message32(0x80, goc_irq_count);
        mbus_write_message32(0x81, goc_head);
        mbus_write_message32(0x82, goc_data);
        mbus_write_message32(0x83, goc_data_ext);

    //////////////////////////////////////////////////////////////////////////////////////////
        
        if (goc_head == 0x00) {
            //--------------------------------------------------------
            if      (goc_data == 0x000000) set_data_tx(/*shuffle*/0, /*pattern*/goc_data_ext);
            //--------------------------------------------------------
            else if (goc_data == 0x000001) set_data_tx(/*shuffle*/1, /*pattern*/goc_data_ext);
            //--------------------------------------------------------
            else if (goc_data == 0x000002) {
                mbus_copy_mem_from_local_to_remote_bulk(
                    /*remote_prefix  */ DBG_ADDR,
                    /*remote_mem_addr*/ (uint32_t*) 0x0,
                    /*local_mem_addr */ (uint32_t*) data_tx,
                    /*length_minus_1 */ BUF_SIZE - 1
                    );
            }
            //--------------------------------------------------------
            else if (goc_data == 0x000003) {
                mbus_copy_mem_from_local_to_remote_bulk(
                    /*remote_prefix  */ DBG_ADDR,
                    /*remote_mem_addr*/ (uint32_t*) 0x0,
                    /*local_mem_addr */ (uint32_t*) data_rx,
                    /*length_minus_1 */ BUF_SIZE - 1
                    );
            }
            //--------------------------------------------------------
            else if (goc_data == 0x000004) {
                mbus_copy_mem_from_local_to_remote_bulk(
                    /*remote_prefix  */ DBG_ADDR,
                    /*remote_mem_addr*/ (uint32_t*) 0x0,
                    /*local_mem_addr */ (uint32_t*) data_rand,
                    /*length_minus_1 */ BUF_SIZE - 1
                    );
            }
            //--------------------------------------------------------
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x01) {
            mrm_set_tmc_for_nopp();
            //--------------------------------------------------------
            if (goc_data < MRM_NUM_MRAM_MACROS) {
                mrm_turn_on_macro(/*mid*/goc_data);
            }
            //--------------------------------------------------------
            else if (goc_data == 0x000006) mrm_turn_off_macro();
            //--------------------------------------------------------
            else if (goc_data == 0x000007) mrm_turn_on_ldo();
            //--------------------------------------------------------
            else if (goc_data == 0x000008) mrm_turn_off_ldo();
            //--------------------------------------------------------
            else if (goc_data == 0x00000F) operation_sleep();
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x02) {
        
            mrm_set_tmc_for_nopp();
            mrm_turn_on_ldo();  // Need to turn on LDO first.

            //--------------------------------------------------------
            if (goc_data == 0x000000) 
                mrm_write_sram(/*prc_sram_addr*/(uint32_t*)data_tx, /*num_words*/BUF_SIZE, /*mrm_sram_addr*/(uint32_t*)goc_data_ext);
            //--------------------------------------------------------
            else if (goc_data == 0x000001) 
                mrm_read_sram(/*mrm_sram_addr*/(uint32_t*)goc_data_ext, /*num_words*/BUF_SIZE, /*prc_sram_addr*/(uint32_t*)data_rx);
            //--------------------------------------------------------
            else if ((goc_data == 0x000002) | (goc_data == 0x000003) | (goc_data == 0x000004) | (goc_data == 0x000005)) {

                if      (goc_data == 0x000002) {}
                else if (goc_data == 0x000003) set_data_tx(/*shuffle*/0, /*pattern*/0xFFFFFFFF);
                else if (goc_data == 0x000004) set_data_tx(/*shuffle*/0, /*pattern*/0x00000000);
                else if (goc_data == 0x000005) set_data_tx(/*shuffle*/1, /*pattern*/goc_data_ext);

                for(i=0; i<512; i++) mrm_write_sram(/*prc_sram_addr*/(uint32_t*)data_tx, /*num_words*/BUF_SIZE, /*mrm_sram_addr*/(uint32_t*)((i<<5)<<2));
            }
            //--------------------------------------------------------
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x03) {
            mrm_turn_on_ldo();  // Need to turn on LDO first.
            mrm_read_sram_page_debug (/*mrm_sram_pid*/goc_data_ext, /*num_pages*/goc_data, /*dest_prefix*/DBG_ADDR);
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x04) {
            mrm_set_tmc_for_nopp();
            mrm_turn_on_ldo();  // Need to turn on LDO first.
            mrm_read_mram_page_debug (/*mrm_mram_pid*/goc_data_ext, /*num_pages*/goc_data, /*dest_prefix*/DBG_ADDR);
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if ((goc_head == 0x07) | (goc_head == 0x08)) {

            mrm_set_tmc_for_pp();
            mrm_set_clock_mode(/*clock_mode*/goc_head>>3);

            mrm_turn_on_ldo();  // Need to turn on LDO first.

            //--------------------------------------------------------
            if (goc_data == 0x000000) mrm_stop_pp_ext_stream();
            //--------------------------------------------------------
            else if ((goc_data == 0x000001) | (goc_data == 0x000002) | (goc_data == 0x000003)) {
                if (goc_data_ext==0) 
                    mrm_pp_ext_stream_unlim (/*bit_en*/goc_data, /*mram_page_id*/0);
                else
                    mrm_pp_ext_stream (/*bit_en*/goc_data, /*num_pages*/goc_data_ext, /*mram_page_id*/0);
            }
            //--------------------------------------------------------
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if ((goc_head == 0x0A) | (goc_head == 0x0B)) {

            mrm_set_tmc_for_nopp();
            mrm_set_clock_mode(/*clock_mode*/0);

            if (goc_head == 0x0A) { // Tpwr = ~1ms (default)
                mrm_set_tpwr(/*tpwr*/200);
            } else if (goc_head == 0x0B) { // Tpwr = ~300ms
                mrm_set_tpwr(/*tpwr*/60000);
            }

            mrm_turn_on_ldo();  // Need to turn on LDO first.

            // Run the test procedure
            if (goc_data < 0x000004) {

                uint32_t t_start, t_stop;
                if      (goc_data == 0x000000) {t_start = 0; t_stop = 0;}
                else if (goc_data == 0x000001) {t_start = 1; t_stop = 1;}
                else if (goc_data == 0x000002) {t_start = 2; t_stop = 2;}
                else if (goc_data == 0x000003) {t_start = 0; t_stop = 2;}
                else                           {t_start = 0; t_stop = 0;}

                for (t=t_start; t<(t_stop+1); t++) {

                    // Prepare data_tx[]
                    if      (t==0) for(i=0; i<BUF_SIZE; i++) data_tx[i] = 0x00000000;
                    else if (t==1) for(i=0; i<BUF_SIZE; i++) data_tx[i] = 0xFFFFFFFF;
                    else if (t==2) for(i=0; i<BUF_SIZE; i++) data_tx[i] = data_rand[i];

                    // Copy data_tx into the entire MRM SRAM (64kB = 512 pages)
                    for(i=0; i<512; i++) mrm_write_sram(/*prc_sram_addr*/(uint32_t*)data_tx, /*num_words*/BUF_SIZE, /*mrm_sram_addr*/(uint32_t*)((i<<5)<<2));

                    // Copy MRM SRAM into MRM MRAM (4MB = 32768 pages)
                    for(i=0; i<32768; i=i+512) mrm_sram2mram(/*sram_pid*/i&0x1FF, /*num_pages*/512, /*mram_pid*/i);

                    // Check the result

                    // --- Initialize the error counters
                    result_page_id[t] = 0xFFFFFFFF;
                    result_num_page_err[t] = 0;
                    result_num_word_err[t] = 0;
                    result_num_bit_err[t]  = 0;

                    // --- i: MRM MRAM Page ID (32768 pages)
                    for(i=0; i<32768; i=i+512) {
                        mrm_mram2sram(/*mram_pid*/i, /*num_pages*/512, /*sram_pid*/0);

                        // --- j: MRM SRAM Page ID (512 pages/SRAM)
                        for(j=0; j<512; j++) { 
                            mrm_read_sram_page (/*mrm_sram_pid*/j, /*num_pages*/1, /*prc_sram_addr*/(uint32_t*)data_rx);

                            // --- k: Word Offset (32 words/page)
                            uint32_t page_err = 0;
                            for(k=0; k<32; k++) {
                                if (data_tx[k] != data_rx[k]) {
                                    page_err = 1;
                                    result_num_word_err[t]++;

                                    // Log the very first error word
                                    if (result_page_id[t] == 0xFFFFFFFF) {
                                        result_page_id[t] = i + j;
                                        result_word_offset[t] = k;
                                        result_word_correct[t] = data_tx[k];
                                        result_word_actual[t]  = data_rx[k];
                                    }

                                    // --- l: Bit offset (32 bits/word)
                                    uint32_t temp_tx = data_tx[k];
                                    uint32_t temp_rx = data_rx[k];
                                    for (l=0; l<32; l++) {
                                        if ((temp_tx&0x1) != (temp_rx&0x1)) result_num_bit_err[t]++;
                                        temp_tx >>=1;
                                        temp_rx >>=1;
                                    }
                                }
                            }

                            if (page_err) result_num_page_err[t]++;

                        }
                    }
                }
            }
            // Spit out the results
            else {
                for (t=0; t<3; t++) {
                    mbus_write_message32((RES_ADDR<<4)|0x0, result_num_page_err[t] );
                    mbus_write_message32((RES_ADDR<<4)|0x1, result_num_word_err[t] );
                    mbus_write_message32((RES_ADDR<<4)|0x2, result_num_bit_err[t]  );
                    mbus_write_message32((RES_ADDR<<4)|0x3, result_page_id[t]      );
                    mbus_write_message32((RES_ADDR<<4)|0x4, result_word_offset[t]  );
                    mbus_write_message32((RES_ADDR<<4)|0x5, result_word_correct[t] );
                    mbus_write_message32((RES_ADDR<<4)|0x6, result_word_actual[t]  );
                }
            }
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0xFF) {
        
            //--------------------------------------------------------
            if (goc_data == 0xFFFFFF) {} // NOP
            //--------------------------------------------------------
            
        }

    }

    //--------------------------------------------------------------------------
    // End of Execution; Never reaches here.
    //--------------------------------------------------------------------------
    return 1;
}
