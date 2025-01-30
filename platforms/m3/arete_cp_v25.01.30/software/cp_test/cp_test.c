//*******************************************************************************************
// ARETE CP TEST FIRMWARE
// Version 25.01.30 (devel)
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//-------------------------------------------------------------------------------------------
//
//      PREv23E -> ADOv1A -> MRMv1L -> PMUv13r1
//
//-------------------------------------------------------------------------------------------
// GOC/EP TRIGGERS
//-------------------------------------------------------------------------------------------
//
//  - This firmware is based on GOC/EP Triggers and their IRQ handlings.
//
//      - In the GOC/EP HEADER, CONTROL[7:6] must be set to 2'b11, which means 'GOCEP_IRQ'.
//      - Write up to 2 words starting from Memory Address 0x8. 
//          These are stored in the goc_head, goc_data, goc_data_ext variables as shown below.
//
//              goc_head     = Bit [31:24] of MEM[0x8]
//              goc_data     = Bit [23:0]  of MEM[0x8]
//              goc_data_ext = Bit [31:0]  of MEM[0xC]
//
//-------------------------------------------------------------------------------------------
// CHIP PREPARATIONS
//-------------------------------------------------------------------------------------------
//
//  0. PRE-REQUISITES
//      
//      - You need 4 SourceMeters for the following voltages
//
//          VBAT
//          VOTP
//          DCP_0P8
//          DCP_1P8
//
//      - Check the GPIB address of each SourceMeter, and update 'SOURCE_METERS' variable in the Python CP program (cw_mram_cp.py)
//
//          NOTE: 
//              VBAT is NOT controlled by the Python CP program.
//              DCP_OP8 is called VDD in the Python CP program.
//              DCP_1P8 is called VDIO in the Python CP program.
//
//      - Set each SourceMeter to the 'Source-V Measure-I' mode.
//      - Set each SourceMeter's compliance level, as well as the display digit, to 100mA.
//      - Set each SourceMeter's voltage to the following:
//
//          VBAT    = 2.2V
//          VOTP    = 0.0V
//          DCP_0P8 = 0.8V
//          DCP_1P8 = 1.8V
//
//      - You need a FT232H board to drive the JTAG wires. Update 'FT232H_JTAG_ADDR' in the Python CP program (cw_mram_cp.py)
//
//
//  1. Connect the following pins
//
//      - VSS
//      - EP (ECI, EMO, EDI) or GOC: Connect to your programmer (e.g., LabVIEW, etc)
//      - JTAG (TCK, TMO, TDI, TDO): Connect to the FT232H board (through a Level Converter)
//      - MBus (PRE_COUT, PRE_DOUT): Connect to Salaea for monitoring
//      - DCP_1P8: Connect to the SourceMeter.
//      - DCP_0P8: Conenct to the SourceMeter.
//      - VOTP: Connect to the SourceMeter.
//      - VBAT: Connect to the SourceMeter.
//
//  2. Boot the system and load this program
//
//  3. Use goc_header=0xE1 to find out the right LDO tuning.
//      Increment goc_data, from 0 to 15.
//      Find out the point where the DCP_1P8 SourceMeter current changes from 'negative' to 'positive'.
//      Write down the goc_data value where the SourceMeter shows the last negative value (i.e., one with the 'minimum' absolute value with a minus sign)
//
//  4. Use goc_header=0xE0 to find out the right LDO tuning.
//      Increment goc_data, from 0 to 15.
//      Find out the point where the DCP_0P8 SourceMeter current changes from 'negative' to 'positive'.
//      Write down the goc_data value where the SourceMeter shows the last negative value (i.e., one with the 'minimum' absolute value with a minus sign)
//
//  5. Use goc_header=0x0C to find out the optimum clock frequency tuning.
//      See 'Clock Frequency Measurement' below.
//
//  6. Open include/MRMv1L.h file.
//      Set 'MRM_LDO_VOUT_V1P8' to the value found in Step 3.
//      Set 'MRM_LDO_VOUT_V0P8' to the value found in Step 4.
//      Set 'MRM_CLK_GEN_S' to the value found in Step 5.
//
//  7. Recompile the source file by executing 'make clean' followed by 'make'.
//
//  8. Boot the system and load this program.
//
//  9. For n=0,1,2,3,4,5, do the following:
//
//      9-A) Trigger with goc_header=0x0F, goc_data=n
//              This enables the CP mode for Macro#n.
//      9-B) Run the Python CP program (cw_mram_cp.py). Wait until it beeps and shows 'TEST PASSED!'.
//      9-C) Trigger with goc_header=0x0F, goc_data=0xF
//              This disables the CP mode and goes back to idle.
//
//  10. Now your MRAM is ready, and you can disconnect all the wires.
//
//
//-------------------------------------------------------------------------------------------
// DESCRIPTION
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
//              goc_head    = 0x0F
//              goc_data    = Macro ID
//
//          2) Run the Python code (cw_mram_cp.py)
//
//          3) Disable the CP Testing
//
//              goc_head    = 0x0F
//              goc_data    = 0xF
//
//  * LDO Tuning
//      
//      goc_head = 0xE0 (for V0P8)
//                 0xE1 (for V1P8)
//      goc_data = n, where n = 0, ..., 15
//                  Smaller n results in a higher voltage
//
//          NOTE: It also turns on Macro#0.
//
//      goc_head = 0xEE
//      goc_data = any
//
//          -> This turns off ALL macros and the LDO.
//      
//
//  * Clock Frequency Measurement
//
//      goc_head = 0x0C
//      goc_data = any
//      goc_data_ext = CLK_GEN_S, where CLK_GEN_S = 0, ..., 127
//                  Smaller CLK_GEN_S results in a faster clock frequency
//
//      GOAL: Make CLK_FAST = 50MHz
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
//              CLK_SLOW = 65536 / ((Td - Tc) - (Tb - Ta))
//                      = 65536 / (Ta - Tb - Tc + Td)
//                      = 65536 / (2.720135200000000 - 2.720744320000000 - 2.729130880000000 + 3.099683520000000)
//                      = 177.151 kHz
//              CLK_FAST = CLK_SLOW x 256 = 45.35 MHz
//
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
//      2) Run the Python code
//
//      3) Disable the CP Testing
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
// LIST OF AVAILABLE GOC/EP IRQ 
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
//        goc_data = 0: Writes goc_data_ext[23:0] into MRM Register#(goc_data_ext[31:24])
//
//  ------------------------------------------------------------
//   goc_head = 0xE0: LDO Tuning (V0P8)
//  ------------------------------------------------------------
//        goc_data = n: Change LDO tuning (Register 0x23)
//                          LDO_SELB_VOUT_LDO_BUF (V0P8) = n
//
//  ------------------------------------------------------------
//   goc_head = 0xE1: LDO Tuning (V1P8)
//  ------------------------------------------------------------
//        goc_data = n: Change LDO tuning (Register 0x23)
//                          LDO_SELB_VOUT_LDO_1P8 (V1P8) = n
//
//  ------------------------------------------------------------
//   goc_head = 0x0F: CP Testing
//      --------------------------------------------------------
//      NOTE: CP Testing Procedure:
//          1) Enable the CP testing using goc_head=0x0F, goc_data = macro_id
//          2) Run the python code
//          3) Disable the CP testing using goc_head=0x0F, goc_data = 0xF
//          4) Repeat for each macro_id
//  ------------------------------------------------------------
//        goc_data = n  : n=[0,5] Enable the CP testing mode for MID=n
//        goc_data = 0xF: Disable the CP testing mode
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
#include "PMUv13.h"
#include "MRMv1L.h"

//*******************************************************************************************
// COMPILE SETTING
//*******************************************************************************************
//#define DEVEL // Not used for now.

//*******************************************************************************************
// ENUMERATION
//*******************************************************************************************
#define ADO_ADDR 0x3
#define MRM_ADDR 0x4
#define PMU_ADDR 0x5
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
    mbus_enumerate(ADO_ADDR);
    mbus_enumerate(MRM_ADDR);
    mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();

    //-------------------------------------------------
    // PMUv13 Setting
    //-------------------------------------------------
    pmu_init(/*pmu_prefix*/PMU_ADDR, /*irq_reg_idx*/PMU_TARGET_REG_IDX);

    //-------------------------------------------------
    // MRMv1L Setting
    //-------------------------------------------------
    // See MRMv1L.h file for the recommneded 'clk_gen_s' value for each chip.
    mrm_init(/*mrm_prefix*/MRM_ADDR, /*irq_reg_idx*/0x0, /*clk_gen_s*/MRM_CLK_GEN_S, /*tpar_30*/MRM_TPAR_0x30, /*tpar_31*/MRM_TPAR_0x31, /*tpar_32*/MRM_TPAR_0x32, /*tpar_33*/MRM_TPAR_0x33, /*tpar_34*/MRM_TPAR_0x34, /*tpar_35*/MRM_TPAR_0x35);

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
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_gocep (void) {
    goc_head     = *GOC_DATA_IRQ >> 24;
    goc_data     = *GOC_DATA_IRQ & 0xFFFFFF;
    goc_data_ext = *(GOC_DATA_IRQ+1);
    *GOC_DATA_IRQ = 0xFFFFFFFF;
}

void handler_ext_int_reg0 (void) { 
    disable_reg_irq(/*reg*/0);
}
void handler_ext_int_reg7 (void) { 
    disable_reg_irq(/*reg*/7);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

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
            if (goc_data == 0x0) {
                mbus_remote_register_write(MRM_ADDR, (goc_data_ext>>24)&0xFF, goc_data_ext & 0xFFFFFF);
            } 
        }

    //////////////////////////////////////////////////////////////////////////////////////////
    
        else if ((goc_head == 0xE0) || (goc_head == 0xE1)) {

            if (goc_head == 0xE0) {
                set_halt_until_mbus_trx();
                mbus_remote_register_read(__mrm_prefix__, 0x23, __mrm_irq_reg_idx__);
                set_halt_until_mbus_tx();

                uint32_t v1p8 = (*__mrm_irq_reg_addr__ >> 10) & 0xF;
                mrm_tune_ldo (/*i0p8*/0x1F, /*i1p8*/0x1F, /*v0p8*/goc_data&0xF, /*v1p8*/v1p8);
            }

            else if (goc_head == 0xE1) {
                set_halt_until_mbus_trx();
                mbus_remote_register_read(__mrm_prefix__, 0x23, __mrm_irq_reg_idx__);
                set_halt_until_mbus_tx();

                uint32_t v0p8 = (*__mrm_irq_reg_addr__ >> 6) & 0xF;
                mrm_tune_ldo (/*i0p8*/0x1F, /*i1p8*/0x1F, /*v0p8*/v0p8, /*v1p8*/goc_data&0xF);
            }

            // Turn on the LDO and the Macro #0
            mrm_turn_on_ldo();
            mrm_turn_on_macro(/*mid*/0);
        }

        else if (goc_head == 0xEE) {
            mrm_turn_off_macro();
            mrm_turn_off_ldo();
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
                temp_reg |=   (0x1 << 5)  // TMC_RST_AUTO_WK = 1
                            | (0x0 << 1)  // TMC_DO_REG_LOAD = 0
                            | (0x0 << 0); // TMC_DO_AWK = 0
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
