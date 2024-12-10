//*******************************************************************************************
// MRMv1L Testing Suite
//-------------------------------------------------------------------------------------------
//
// Summary of Operations
//
//  * ONE MACRO TESTING (SHORTCUT)
//
//      Set #bits        = 16777216
//      goc_data         = Macro ID
//      goc_data_ext     = Page Offset (0 - 16383)
//
//      1) Ping-Pong into One Macro w/ DATA_EXT[0]
//          
//              goc_head     = 0x06
//
//      2) Read one MRAM Macro
//
//              goc_head     = 0x05
//
//      3) CP Testing
//
//          1) Enable CP Testing
//
//              goc_head     = 0x0F
//
//          2) Set VOTP = 2.2V
//
//          3) Run the Python code
//
//          4) Set VOTP = 0V
//
//          5) Disable the CP Testing
//
//              goc_head    = 0x0F
//
//
//  * ENTIRE MRAM TESTING
//
//      1) Ping-Pong into the First Half MRAM w/ DATA_EXT[0]
//
//          goc_head     = 0x08
//          goc_data     = 0x1 (use 0x6 if DATA_EXT[1:0])
//          goc_data_ext = 0xC000 (49152)
//          #bits        = 50331648
//
//      2) Ping-Pong into the Second Half MRAM w/ DATA_EXT[0]
//
//          goc_head     = 0x08
//          goc_data     = 0x4 (use 0x6 if DATA_EXT[1:0])
//          goc_data_ext = 0xC000 (49152)
//          #bits        = 50331648
//
//      3) Read the entire MRAM
//
//          goc_head     = 0x05
//          goc_data     = 0x6
//          goc_data_ext = 0x0
//
//
//  * ONE MACRO TESTING
//
//      1) Ping-Pong into One Macro w/ DATA_EXT[0]
//          
//              goc_head     = 0x07
//              goc_data     = Macro ID 
//              goc_data_ext = 0x1 (use 0x3 if DATA_EXT[1:0])
//              #bits        = 16777216
//
//      2) Read one MRAM Macro
//
//              goc_head     = 0x05
//              goc_data     = Macro ID
//              goc_data_ext = 0x0
//
//
//  * CP TESTING
//
//      1) Enable CP Testing
//
//          goc_head     = 0x0F
//          goc_data     = Macro ID
//
//      2) Set VOTP = 2.2V
//
//      3) Run the Python code
//
//      4) Set VOTP = 0V
//
//      5) Disable the CP Testing
//
//          goc_head    = 0x0F
//          goc_data    = 0xF
//
//
//
//  * MISCELLANEOUS
//
//      * Ping-Pong into the entire MRAM w/ DATA_EXT[0] (NOTE: LabVIEW cannot handle this due to OutOfMemory)
//
//              goc_head     = 0x08
//              goc_data     = 0x1 (use 0x3 if DATA_EXT[1:0])
//              goc_data_ext = 0x18000 (98304)
//              #bits        = 100663296
//
//      * Read the MRAM SRAM
//
//              goc_head     = 0x05
//              goc_data     = 0x7
//              goc_data_ext = 0x0
//
//
//-------------------------------------------------------------------------------------------
// GOC/EP IRQ 
//-------------------------------------------------------------------------------------------
//
//  ------------------------------------------------------------
//   goc_head = 0x00: Access PRC/PRE SRAM
//  ------------------------------------------------------------
//        goc_data = 0: Set data_tx[n] = goc_data_ext w/ shuffle=0
//        goc_data = 1: Set data_tx[n] = goc_data_ext w/ shuffle=1
//        goc_data = 2: Send data_tx[] out through MBus
//        goc_data = 3: Send data_rx[] out through MBus
//        goc_data = 4: Send data_rand[] out through MBus
//  
//  ------------------------------------------------------------
//   goc_head = 0x01: Basic Power Control
//  ------------------------------------------------------------
//        goc_data = n: Turn on Macro#n (n = 1, 2, ..., 5)
//        goc_data = 6: Turn off all macros
//        goc_data = 7: Turn on the LDO
//        goc_data = 8: Turn off the LDO
//        goc_data = F: Send MBus Sleep Message
//  
//  ------------------------------------------------------------
//   goc_head = 0x02: Accessing MRM SRAM
//  ------------------------------------------------------------
//        goc_data = 0: Read data_tx[] and Write into MRM SRAM.   i.e., MRM_SRAM[goc_data_ext+n] = data_tx[n], where n=0, ..., BUF_SIZE-1
//        goc_data = 1: Read MRM SRAM and Write into data_rx[].   i.e., data_rx[n] = MRM_SRAM[goc_data_ext+n], where n=0, ..., BUF_SIZE-1
//        goc_data = 2: Repeatedly write data_tx[] into MRM_SRAM. i.e., MRM_SRAM[BUF_SIZE x i + n] = data_tx[n], where n=0, ..., BUF_SIZE-1, and i=0, ..., 511
//        goc_data = 3: Write all-1 in MRM_SRAM. i.e., MRM_SRAM[n] = 0xFFFFFFFF, where n=0, ..., 16383; NOTE: This alters data_tx[].
//        goc_data = 4: Write all-0 in MRM_SRAM. i.e., MRM_SRAM[n] = 0x00000000, where n=0, ..., 16383; NOTE: This alters data_tx[].
//        goc_data = 5: Write goc_data_ext (w/ shuffle) in MRM_SRAM. i.e., MRM_SRAM[n] = goc_ext_data (w/ shuffle), where n=0, ..., 16383; NOTE: This alters data_tx[].
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
//   goc_head = 0x05: Read the MRM SRAM or MRAM, and put it on the MBus
//  ------------------------------------------------------------
//        goc_data = n: (n=0, 1, ..., 5)
//                      Read the MRM MRAM, starting from Page#((16384xn)+goc_data_ext), for (MRM_NUM_PAGES_MRAM_MACRO(16384)) pages.
//                      Thus, goc_data_ext=0 means that it starts from the beginning of Macro#n.
//                      It sends out multiple MBus messages, with each carrying up to 64kB data.
//        goc_data = 6: Read the MRM MRAM, starting from Page#0, for 'goc_data_ext' pages. goc_data_ext=0 translates to all (MRM_NUM_PAGES_MRAM(98304)) pages.
//                      If goc_data_ext > NUM_MAX_PAGES_MBUS(512), it sends out multiple MBus messages, with each carrying up to 64kB data.
//        goc_data = 7: Read the MRM SRAM, starting from Page#0, for 'goc_data_ext' pages. goc_data_ext=0 translates to all (MRM_NUM_PAGES_SRAM(512)) pages.
//  
//  ------------------------------------------------------------
//   goc_head = 0x06: One-Macro Ping-Pong (TMC_CLOCK_MODE=1) w/ DATA_EXT[0]
//  ------------------------------------------------------------
//        goc_data = n: Goes into Ping-Pong Mode, with num_pages = 16384, starting from Page#((16384xn)+goc_data_ext)
//                      Thus, goc_data_ext=0 means that it starts from the beginning of Macro#n.
//
//  ------------------------------------------------------------
//   goc_head = 0x07: One-Macro Ping-Pong (TMC_CLOCK_MODE=0) w/ DATA_EXT[0]
//  ------------------------------------------------------------
//        goc_data = n: Goes into Ping-Pong Mode, with num_pages = 16384, starting from Page#((16384xn)+goc_data_ext)
//                      Thus, goc_data_ext=0 means that it starts from the beginning of Macro#n.
//
//  ------------------------------------------------------------
//   goc_head = 0x08: Ping-Pong (TMC_CLOCK_MODE=1)
//  ------------------------------------------------------------
//        goc_data = 0: Force stop the Ping-Pong mode.
//        goc_data = 1: Goes into Ping-Pong Mode using DATA_EXT[0],   with num_pages = goc_data_ext, starting from Page#0.
//        goc_data = 2: Goes into Ping-Pong Mode using DATA_EXT[1],   with num_pages = goc_data_ext, starting from Page#0.
//        goc_data = 3: Goes into Ping-Pong Mode using DATA_EXT[1:0], with num_pages = goc_data_ext, starting from Page#0.
//        goc_data = 4: Goes into Ping-Pong Mode using DATA_EXT[0],   with num_pages = goc_data_ext, starting from Page#(MRM_NUM_PAGES_MRAM/2).
//        goc_data = 5: Goes into Ping-Pong Mode using DATA_EXT[1],   with num_pages = goc_data_ext, starting from Page#(MRM_NUM_PAGES_MRAM/2).
//        goc_data = 6: Goes into Ping-Pong Mode using DATA_EXT[1:0], with num_pages = goc_data_ext, starting from Page#(MRM_NUM_PAGES_MRAM/2).
//  
//  ------------------------------------------------------------
//   goc_head = 0x09: 64kB External Streaming w/ DATA_EXT[0]
//  ------------------------------------------------------------
//        goc_data = 0: Goes into External Streaming mode, with num_pages = 512, starting from Page#(goc_data_ext) in SRAM.
//        goc_data = 1: Read the MRM SRAM, starting from Page#0, for 512 pages (i.e., the entire 64kB SRAM)
//
//  ------------------------------------------------------------
//   goc_head = 0x0A: One-Macro Ping-Pong (TMC_CLOCK_MODE=1) w/ DATA_EXT[0]. No REG_LOAD. Recall trim-code from IFREN1 instead of from OTP.
//  ------------------------------------------------------------
//        goc_data = n: Goes into Ping-Pong Mode, with num_pages = 16384, starting from Page#((16384xn)+goc_data_ext)
//                      Thus, goc_data_ext=0 means that it starts from the beginning of Macro#n.
//
//  ------------------------------------------------------------
//   goc_head = 0x0C: Clock Frequency Measurement
//  ------------------------------------------------------------
//        goc_data = any: Start the measurement, with CLK_GEN_S = goc_data_ext.
//
//                  Less CLK_GEN_S -> Higher Frequency.
//
//                  GOAL: Make CLK_FAST = 50MHz
//
//          Example (Salaea Capture)
//
//              ...
//              2.720135200000000, 0x40, 0x11000007 - Ta
//              2.720744320000000, 0x10, 0x00000094 - Tb
//              ...
//              2.729130880000000, 0x40, 0x11000007 - Tc
//              3.099683520000000, 0x10, 0x00000094 - Td
//              ...
//
//              CL_SLOW = 65536 / ((Td - Tc) - (Tb - Ta))
//                      = 65536 / (Ta - Tb - Tc + Td)
//                      = 65536 / (2.720135200000000 - 2.720744320000000 - 2.729130880000000 + 3.099683520000000)
//                      = 177.151 kHz
//              CLK_FAST = CLK_SLOW x 256 = 45.35 MHz
//
//
//  ------------------------------------------------------------
//   goc_head = 0x0E: MRM Register File Access
//  ------------------------------------------------------------
//        goc_data = n/a: Writes goc_data_ext[23:0] into MRM Register#(goc_data_ext[31:24])
//
//  ------------------------------------------------------------
//   goc_head = 0x0F: CP Testing
//      --------------------------------------------------------
//      NOTE: CP Testing Procedure:
//          1) Enable the CP testing using goc_head=0x0F, goc_data = macro_id
//          2) Set VOTP = 2.2V
//          3) Run the python code
//          4) Disable the CP testing using goc_head=0x0F, goc_data = 0xF
//          5) Repeat for each macro_id
//  ------------------------------------------------------------
//        goc_data = 0x000000: Enable the CP testing mode for MID=0
//        goc_data = 0x000001: Enable the CP testing mode for MID=1
//        goc_data = 0x00000F: Disable the CP testing mode
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
#include "MRMv1L.h"

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

#define BUF_SIZE MRM_NUM_WORDS_PER_PAGE // Corresponds to 1 Page (= 32 words = 8 Qwords)
volatile uint32_t data_rand[BUF_SIZE];
volatile uint32_t data_tx[BUF_SIZE];
volatile uint32_t data_rx[BUF_SIZE];

volatile uint32_t old_val;  // Generic Temporary Variable

uint32_t tmc_result[15];

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
    // Release MRMv1L RESET Signal (CPS0)
    //-------------------------------------------------
    set_cps (/*cps_full*/0x1, /*cps_weak*/0x0); // cps_weak is only for CPS2
    
    //-------------------------------------------------
    // Enumeration
    //-------------------------------------------------
    set_halt_until_mbus_trx();
    mbus_enumerate(MRM_ADDR);
    set_halt_until_mbus_tx();

    //-------------------------------------------------
    // MRMv1L Setting
    //-------------------------------------------------
    // See MRMv1L.h file for the recommneded 'clk_gen_s' value for each chip.
    mrm_init(/*mrm_prefix*/MRM_ADDR, /*irq_reg_idx*/0x0, /*clk_gen_s*/53);

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

    uint32_t i;

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

                for(i=0; i<MRM_NUM_PAGES_SRAM; i++) mrm_write_sram(/*prc_sram_addr*/(uint32_t*)data_tx, /*num_words*/BUF_SIZE, /*mrm_sram_addr*/(uint32_t*)((i<<5)<<2));
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
            mrm_turn_on_ldo();  // Need to turn on LDO first.
            mrm_read_mram_page_debug (/*mrm_mram_pid*/goc_data_ext, /*num_pages*/goc_data, /*dest_prefix*/DBG_ADDR);
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x05) {
            mrm_turn_on_ldo();  // Need to turn on LDO first.

            //--------------------------------------------------------
            if (goc_data < 0x6) {
                uint32_t mram_pid = (goc_data << MRM_LOG2_NUM_PAGES_MRAM_MACRO)+goc_data_ext;
                uint32_t remaining = MRM_NUM_PAGES_MRAM_MACRO;
                while (remaining > NUM_MAX_PAGES_MBUS) {
                    mrm_read_mram_page_debug (/*mrm_mram_pid*/mram_pid, /*num_pages*/NUM_MAX_PAGES_MBUS, /*dest_prefix*/DBG_ADDR);
                    mram_pid += NUM_MAX_PAGES_MBUS;
                    remaining -= NUM_MAX_PAGES_MBUS;
                }
                if (remaining > 0) {
                    mrm_read_mram_page_debug (/*mrm_mram_pid*/mram_pid, /*num_pages*/remaining, /*dest_prefix*/DBG_ADDR);
                }
            }
            //--------------------------------------------------------
            else if (goc_data == 0x6) {
                if (goc_data_ext == 0) goc_data_ext = MRM_NUM_PAGES_MRAM;
                uint32_t mram_pid = 0;
                uint32_t remaining = goc_data_ext;
                while (remaining > NUM_MAX_PAGES_MBUS) {
                    mrm_read_mram_page_debug (/*mrm_mram_pid*/mram_pid, /*num_pages*/NUM_MAX_PAGES_MBUS, /*dest_prefix*/DBG_ADDR);
                    mram_pid += NUM_MAX_PAGES_MBUS;
                    remaining -= NUM_MAX_PAGES_MBUS;
                }
                if (remaining > 0) {
                    mrm_read_mram_page_debug (/*mrm_mram_pid*/mram_pid, /*num_pages*/remaining, /*dest_prefix*/DBG_ADDR);
                }
            }
            //--------------------------------------------------------
            else if (goc_data == 0x7) {
                if (goc_data_ext == 0) goc_data_ext = MRM_NUM_PAGES_SRAM;
                uint32_t sram_pid = 0;
                uint32_t remaining = goc_data_ext;
                while (remaining > NUM_MAX_PAGES_MBUS) {
                    mrm_read_sram_page_debug (/*mrm_sram_pid*/sram_pid, /*num_pages*/NUM_MAX_PAGES_MBUS, /*dest_prefix*/DBG_ADDR);
                    sram_pid += NUM_MAX_PAGES_MBUS;
                    remaining -= NUM_MAX_PAGES_MBUS;
                }
                if (remaining > 0) {
                    mrm_read_sram_page_debug (/*mrm_sram_pid*/sram_pid, /*num_pages*/remaining, /*dest_prefix*/DBG_ADDR);
                }
            }
            //--------------------------------------------------------
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x06) {

            mrm_set_clock_mode(/*clock_mode*/1);
            mrm_turn_on_ldo();  // Need to turn on LDO first.
            mrm_pp_ext_stream (/*bit_en*/0x1, /*num_pages*/MRM_NUM_PAGES_MRAM_MACRO, /*mram_page_id*/(goc_data<<MRM_LOG2_NUM_PAGES_MRAM_MACRO)+goc_data_ext);

        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x07) {

            mrm_set_clock_mode(/*clock_mode*/0);
            mrm_turn_on_ldo();  // Need to turn on LDO first.
            mrm_pp_ext_stream (/*bit_en*/0x1, /*num_pages*/MRM_NUM_PAGES_MRAM_MACRO, /*mram_page_id*/(goc_data<<MRM_LOG2_NUM_PAGES_MRAM_MACRO)+goc_data_ext);

        }

//        else if (goc_head == 0x09) {
//
//            mrm_set_clock_mode(/*clock_mode*/1);
//            mrm_turn_on_ldo();  // Need to turn on LDO first.
//
//
//            // You must disable AUTO-POWER ON/OFF. Otherwise, R_TEST register gets reset.
//            mrm_disable_auto_power_on_off();
//            
//            mrm_turn_on_macro(/*mid*/goc_data);
//
//            // ECC Off
//            mrm_tmc_write_test_reg(/*xadr*/0x1, /*wdata*/0x3D8008C1);
//
//            mrm_pp_ext_stream (/*bit_en*/0x1, /*num_pages*/MRM_NUM_PAGES_MRAM_MACRO, /*mram_page_id*/goc_data<<MRM_LOG2_NUM_PAGES_MRAM_MACRO);
//
//            mrm_turn_off_macro();
//           
//            // Re-enable AUTO-POWER ON/OFF
//            mrm_enable_auto_power_on_off();
//        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x09) {

            mrm_turn_on_ldo();  // Need to turn on LDO first.

            //--------------------------------------------------------
            if (goc_data == 0x0) {
                mrm_ext_stream (/*bit_en*/0x1, /*num_pages*/MRM_NUM_PAGES_SRAM, /*mrm_sram_pid*/goc_data_ext);
            }
            //--------------------------------------------------------
            else if (goc_data == 0x1) {
                mrm_read_sram_page_debug (/*mrm_sram_pid*/0, /*num_pages*/MRM_NUM_PAGES_SRAM, /*dest_prefix*/DBG_ADDR);
            }
            //--------------------------------------------------------

        }

    //////////////////////////////////////////////////////////////////////////////////////////
        else if (goc_head == 0x08) {

            mrm_set_clock_mode(/*clock_mode*/1);
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
            else if ((goc_data == 0x000004) | (goc_data == 0x000005) | (goc_data == 0x000006)) {
                if (goc_data_ext==0) 
                    mrm_pp_ext_stream_unlim (/*bit_en*/goc_data, /*mram_page_id*/MRM_NUM_PAGES_MRAM>>1);
                else
                    mrm_pp_ext_stream (/*bit_en*/goc_data-3, /*num_pages*/goc_data_ext, /*mram_page_id*/MRM_NUM_PAGES_MRAM>>1);
            }
            //--------------------------------------------------------
        }

    //////////////////////////////////////////////////////////////////////////////////////////
    
        else if (goc_head == 0x0A) {

            // Just in case, reset.
            mrm_turn_off_ldo();

            // Configure...
            // Disable Auto-Power-On/Off, Disable Auto Wakeup and Register Load. Use TMC_CLOCK_MODE=1.
            mrm_disable_auto_power_on_off();
            mbus_remote_register_read (MRM_ADDR, 0x2F, 0x07);   // Store the old value in REG7
            mbus_remote_register_write(MRM_ADDR, 0x2F, 0x0
                /* TMC_RST_AUTO_WK (1'h1) */ | (0x0 << 5)
                /* TMC_CLOCK_MODE  (1'h0) */ | (0x1 << 4)
                /* TMC_CHECK_ERR   (1'h0) */ | (0x0 << 3)
                /* TMC_FAST_LOAD   (1'h1) */ | (0x1 << 2)
                /* TMC_DO_REG_LOAD (1'h1) */ | (0x0 << 1)
                /* TMC_DO_AWK      (1'h0) */ | (0x0 << 0)
            );

            // Turn on the LDO and the Macro
            mrm_turn_on_ldo();
            mrm_turn_on_macro(/*mid*/goc_data);

            // Issue WRITE_CONFIG command.
            // Make it recall the trim data from IFREN1, rather than from OTP)
            // --> Bit[29]: 1 -> 0
            mrm_tmc_write_test_reg(/*xadr*/0xB, /*wdata*/0x0 
                /* ( 1'h0) */ | (0x0 << 31) // Reserved
                /* ( 1'h0) */ | (0x0 << 30) // Used for load {YADR, MRAM.DOUT} to load-buffer when issuing read command. (See Doc)
                /* ( 1'h1) */ | (0x0 << 29) // Select recall/auto-wakeup location come from IFREN1 or OTP (0: IFREN1; 1: OTP)
                /* ( 1'h1) */ | (0x1 << 28) // Control IFREN for data comparison after waiting wakeup time (0: Selected Main Array; 1: Selected IFREN Array)
                /* ( 1'h1) */ | (0x1 << 27) // Bypass comparison checker during recall repair/trim-code/wakeup/auto_wakeup mode.
                /* ( 9'h0) */ | (0x0 << 18) // Set loop count for data comparison during wakeup and auto-wakeup sequence. Loop count = N + 1
                /* (13'h0) */ | (0x0 << 5 ) // Set XADR for data comparison during wakeup and auto-wakeup sequence. 
                /* ( 5'h0) */ | (0x0 << 0 ) // Set YADR for data comparison during wakeup and auto-wakeup sequence.
            );

            // Issue AUTO_WAKEUP Command
            mrm_tmc_cmd (/*cmd*/MRM_TMC_AUTO_WAKEUP, /*xadr*/0, /*yadr*/0, /*din*/0, /*result*/(uint32_t *)0x0);

            // Start Ping-Pong
            mrm_pp_ext_stream (/*bit_en*/0x1, /*num_pages*/MRM_NUM_PAGES_MRAM_MACRO, /*mram_page_id*/(goc_data<<MRM_LOG2_NUM_PAGES_MRAM_MACRO)+goc_data_ext);

            // Write the original TMC configuration
            mbus_remote_register_write(MRM_ADDR, 0x2F, *REG7);

            // Turn off Macro and LDO
            mrm_turn_off_macro();
            mrm_turn_off_ldo();

            // Enable Auto-Power-On/Off, as this is the default setting in mrm_init()
            mrm_enable_auto_power_on_off();

        }

    //////////////////////////////////////////////////////////////////////////////////////////
    
        else if (goc_head == 0x0B) {
            // Just in case, reset.
            mrm_turn_off_ldo();

            // Configure...
            // Disable Auto-Power-On/Off, Disable Auto Wakeup and Register Load. Use TMC_CLOCK_MODE=1.
            mrm_disable_auto_power_on_off();
            mbus_remote_register_read (MRM_ADDR, 0x2F, 0x07);   // Store the old value in REG7
            mbus_remote_register_write(MRM_ADDR, 0x2F, 0x0
                /* TMC_RST_AUTO_WK (1'h1) */ | (0x0 << 5)
                /* TMC_CLOCK_MODE  (1'h0) */ | (0x1 << 4)
                /* TMC_CHECK_ERR   (1'h0) */ | (0x0 << 3)
                /* TMC_FAST_LOAD   (1'h1) */ | (0x1 << 2)
                /* TMC_DO_REG_LOAD (1'h1) */ | (0x0 << 1)
                /* TMC_DO_AWK      (1'h0) */ | (0x0 << 0)
            );

            // Turn on the LDO and the Macro
            mrm_turn_on_ldo();
            mrm_turn_on_macro(/*mid*/goc_data);

            // Issue WRITE_CONFIG command.
            // Make it recall the trim data from IFREN1, rather than from OTP)
            // --> Bit[29]: 1 -> 0
            mrm_tmc_write_test_reg(/*xadr*/0xB, /*wdata*/0x0 
                /* ( 1'h0) */ | (0x0 << 31) // Reserved
                /* ( 1'h0) */ | (0x0 << 30) // Used for load {YADR, MRAM.DOUT} to load-buffer when issuing read command. (See Doc)
                /* ( 1'h1) */ | (0x0 << 29) // Select recall/auto-wakeup location come from IFREN1 or OTP (0: IFREN1; 1: OTP)
                /* ( 1'h1) */ | (0x1 << 28) // Control IFREN for data comparison after waiting wakeup time (0: Selected Main Array; 1: Selected IFREN Array)
                /* ( 1'h1) */ | (0x1 << 27) // Bypass comparison checker during recall repair/trim-code/wakeup/auto_wakeup mode.
                /* ( 9'h0) */ | (0x0 << 18) // Set loop count for data comparison during wakeup and auto-wakeup sequence. Loop count = N + 1
                /* (13'h0) */ | (0x0 << 5 ) // Set XADR for data comparison during wakeup and auto-wakeup sequence. 
                /* ( 5'h0) */ | (0x0 << 0 ) // Set YADR for data comparison during wakeup and auto-wakeup sequence.
            );

            // Issue AUTO_WAKEUP Command
            mrm_tmc_cmd (/*cmd*/MRM_TMC_AUTO_WAKEUP, /*xadr*/0, /*yadr*/0, /*din*/0, /*result*/(uint32_t *)0x0);

            // Main Operation
            uint32_t mram_pid = (goc_data << MRM_LOG2_NUM_PAGES_MRAM_MACRO)+goc_data_ext;
            uint32_t remaining = MRM_NUM_PAGES_MRAM_MACRO;
            while (remaining > NUM_MAX_PAGES_MBUS) {
                mrm_read_mram_page_debug (/*mrm_mram_pid*/mram_pid, /*num_pages*/NUM_MAX_PAGES_MBUS, /*dest_prefix*/DBG_ADDR);
                mram_pid += NUM_MAX_PAGES_MBUS;
                remaining -= NUM_MAX_PAGES_MBUS;
            }
            if (remaining > 0) {
                mrm_read_mram_page_debug (/*mrm_mram_pid*/mram_pid, /*num_pages*/remaining, /*dest_prefix*/DBG_ADDR);
            }

            // Write the original TMC configuration
            mbus_remote_register_write(MRM_ADDR, 0x2F, *REG7);

            // Turn off Macro and LDO
            mrm_turn_off_macro();
            mrm_turn_off_ldo();

            // Enable Auto-Power-On/Off, as this is the default setting in mrm_init()
            mrm_enable_auto_power_on_off();
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x0C) {

            // CLK_GEN Tuning (Default: CLK_GEN_S = 87)
            mrm_set_clock_tune(/*s*/goc_data_ext);
            mrm_turn_on_ldo();  // Need to turn on LDO first.

            // All Zero TMC_CONFIG
            set_halt_until_mbus_trx();
            mbus_remote_register_read(MRM_ADDR, 0x2F, 0x7);
            set_halt_until_mbus_tx();
            old_val = *REG7;
            mbus_remote_register_write(MRM_ADDR, 0x2F, 0x0);

            // Tpwr=0, Power-On/Off
            mrm_set_tpwr(/*tpwr*/0);
            mrm_turn_on_macro(/*mid*/0);
            mrm_turn_off_macro();

            // Tpwr=0xFFFF (65535)
            mrm_set_tpwr(/*tpwr*/0xFFFF);
            mrm_turn_on_macro(/*mid*/0);
            mrm_turn_off_macro();

            // Set TMC_CONFIG back to its original value
            mbus_remote_register_write(MRM_ADDR, 0x2F, old_val);
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x0E) {
            mbus_remote_register_write(MRM_ADDR, (goc_data_ext>>24)&0xFF, goc_data_ext & 0xFFFFFF);
        }

    //////////////////////////////////////////////////////////////////////////////////////////

        else if (goc_head == 0x0F) {

            //--------------------------------------------------------
            if (goc_data < 0x6) {

                // Disable REG_LOAD
                set_halt_until_mbus_trx();
                mbus_remote_register_read(MRM_ADDR, 0x2F, 0x0);
                set_halt_until_mbus_tx();
                old_val = *REG0;

                uint32_t temp_reg = old_val & 0xFFFFDC;   // Reset TMC_RST_AUTO_WK, TMC_DO_REG_LOAD, TMC_DO_AWK
                temp_reg |=   (0x1 << 5)  // TMC_RST_AUTO_WK = 0
                            | (0x0 << 1)  // TMC_DO_REG_LOAD = 1
                            | (0x0 << 0); // TMC_DO_AWK = 1
                mbus_remote_register_write(MRM_ADDR, 0x2F, temp_reg);


                mrm_set_clock_mode(/*clock_mode*/0);
                mrm_turn_on_ldo();
                mrm_turn_on_macro(/*mid*/goc_data);

                // Enable BIST
                mrm_enable_bist();
            }
            //--------------------------------------------------------
            else if (goc_data == 0x00000F) {

                // Disable BIST
                mrm_disable_bist();
                mrm_turn_off_macro();

                // Recover the old value
                mbus_remote_register_write(MRM_ADDR, 0x2F, old_val);
            }
            //--------------------------------------------------------
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
