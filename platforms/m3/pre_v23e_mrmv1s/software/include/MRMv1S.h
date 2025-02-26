//*******************************************************************
//Author: Yejoong Kim
//Description: MRMv1S header file
//*******************************************************************
//
//********************************************************************************************************************
// ADDRESS SCHEME (MRMv1S)
//********************************************************************************************************************
// A macro: 2MB (16Mb), 128-bit I/O, 128B (1kb) Page, 16384 pages, 32:1 Y-MUX
//
//  +---------------+-----------+-----------+-------------------------------------------+---------------------------+
//  |            \  |           |           |                   MRAM                    |           SRAM            |
//  | Number      \ |   Per     |   Per     +-----------+---------------+---------------+-------------+-------------+
//  | of           \|   Qword   |   Page    | Per Row   | Per Macro     | Per MRAM      | Per Macro   | Per SRAM    |
//  +---------------+-----------+-----------+-----------+---------------+---------------+-------------+-------------+
//  | SRAM Macros   |    -      |    -      |    -      |        -      |        -      |      1 ( 0) |      2 ( 1) |
//  | MRAM Macros   |    -      |    -      |    -      |        1 ( 0) |        2 ( 1) |      -      |      -      |
//  +---------------+-----------+-----------+-----------+---------------+---------------+-------------+-------------+
//  | Rows          |    -      |    -      |    1 ( 0) |     4096 (12) |     8192 (13) |     64 ( 6) |    128 ( 7) |
//  | Pages         |    -      |    1 ( 0) |    4 ( 2) |    16384 (14) |    32768 (15) |    256 ( 8) |    512 ( 9) |
//  +---------------+-----------+-----------+-----------+---------------+---------------+-------------+-------------+
//  | Qwords (128b) |    1 ( 0) |    8 ( 3) |   32 ( 5) |   131072 (17) |   262144 (18) |   2048 (11) |   4096 (12) |
//  | Words  (32b)  |    4 ( 2) |   32 ( 5) |  128 ( 7) |   524288 (19) |  1048576 (20) |   8192 (13) |  16384 (14) |
//  +---------------+-----------+-----------+-----------+---------------+---------------+-------------+-------------+
//  | Bytes         |   16 ( 4) |  128 ( 7) |  512 ( 9) |  2097152 (21) |  4194304 (22) |  32768 (15) |  65536 (16) |
//  | Bits          |  128 ( 7) | 1024 (10) | 4096 (12) | 16777216 (24) | 33554432 (25) | 262144 (18) | 524288 (19) |
//  +---------------+-----------+-----------+-----------+---------------+---------------+-------------+-------------+
//
//********************************************************************************************************************
// TMC_CLOCK_MODE
//********************************************************************************************************************
//
//      0:  CLK_FAST is only used to generate the write pulses (i.e., YE pulses)
//          CLK_SLOW is used elsewhere all the time.
//
//      1:  During Ping-Pong:
//              During MRAM Writing Process (i.e., pp_state=PP_CP_SRAM_MACRO), 
//                  CLK_FAST is used everywhere (including Layer Ctrl, all FSMs).
//              If pp_state != PP_CP_SRAM_MACRO,
//                  CLK_SLOW is used everywhere (including Layer Ctrl, all FSMs).
//          If NOT in Ping-Pong:
//              CLK_FAST is only used to generate the write pulses (i.e., YE pulses)
//              CLK_SLOW is used elsewhere.
//
//********************************************************************************************************************

#ifndef MRMV1S_H
#define MRMV1S_H
#define MRMV1S

//*******************************************************************
// INCLUDES...
//*******************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "mbus.h"
#include "PREv23E.h"

//*******************************************************************
// PREDEFINED CONSTANTS
//*******************************************************************

#define MRM_NUM_MRAM_MACROS     2

//--- MRAM GO Command
#define MRM_CMD_NOP             0x0
#define MRM_CMD_CP_MRAM2SRAM    0x1
#define MRM_CMD_CP_SRAM2MRAM    0x2
#define MRM_CMD_TMC_CMD         0x3
#define MRM_CMD_TMC_JTAG        0x4
#define MRM_CMD_EXT_WR_SRAM     0x6
#define MRM_CMD_START_BOOT      0x8
#define MRM_CMD_MEAS_CLK_FREQ   0x9

#define MRM_EXP_NONE            0x00
#define MRM_EXP_CP_MRAM2SRAM    0x2F
#define MRM_EXP_CP_SRAM2MRAM    0x3F
//#define MRM_EXP_TMC_CMD         0x0
//#define MRM_EXP_TMC_JTAG        0x0
//#define MRM_EXP_EXT_WR_SRAM     0x0
//#define MRM_EXP_START_BOOT      0x0
//#define MRM_EXP_MEAS_CLK_FREQ   0x0
#define MRM_EXP_PP_STR_SUCCESS  0xE2    // PP_STR_2
#define MRM_EXP_PP_STR_STOPPED  0xE4    // PP_STR_EXT_STOPPED_1        
#define MRM_EXP_PP_STR_OVERRUN  0xE5    // PP_STR_ERR_BUF_OVERRUN_0    
#define MRM_EXP_PP_STR_TOO_FAST 0xE6    // PP_STR_ERR_EXT_TOO_FAST_0   

//--- TMC_CLOCK_MODE
#define MRM_SLOW_CLOCK_MODE     0x0
#define MRM_FAST_PP_CLOCK_MODE  0x1


//*******************************************************************
// GLOBAL VARIABLES
//*******************************************************************
volatile uint32_t  __mrm_prefix__;
volatile uint32_t  __mrm_irq_reg_idx__;
volatile uint32_t* __mrm_irq_reg_addr__;


//********************************************************************************************************************
// MRM INITIALIZATION FUNCTIONS
//********************************************************************************************************************

//-------------------------------------------------------------------
// Function: mrm_init
// Args    :    
//      mrm_prefix  - MRM Short Prefix
//      irq_reg_idx - PRE's Register ID to receive MBus IRQ messages coming from MRM.
//                      Valid Range: [0, 7]
//                  Example: If irq_reg_id = 4, MRM sends IRQ messages to PRE's
//                          Register 0x04.
// Description:
//           Initializes MRMv1S
// Return  : None
//-------------------------------------------------------------------
void mrm_init(uint32_t mrm_prefix, uint32_t irq_reg_idx);

//-------------------------------------------------------------------
// Function: mrm_disable_tmc_rst_auto_wk
// Args    : none
// Description:
//          It sets the following variables:
//              TMC_RST_AUTO_WK = 0
//              TMC_DO_REG_LOAD = 1
//              TMC_DO_AWK = 1
//          The 'TMC Register Load' is performed only when TMC_DO_AWK=1.
//          Thus, here it disables TMC_RST_AUTO_WK to avoid the redundant AWK.
//
//          NOTE: For Ping-Pong operations, use mrm_enable_tmc_rst_auto_wk().
//
// Return  : None
//-------------------------------------------------------------------
void mrm_disable_tmc_rst_auto_wk(void);

//-------------------------------------------------------------------
// Function: mrm_enable_tmc_rst_auto_wk
// Args    : none
// Description:
//          It sets the following variables:
//              TMC_RST_AUTO_WK = 1
//              TMC_DO_REG_LOAD = 1
//              TMC_DO_AWK = 1
//
//          In Ping-Pong mode:
//              the FSM does not issue the AWK command even if TMC_DO_AWK=1,
//              because of the following line: 
//                  TMC_CMD <= `SD is_ping_pong_str ? next_pp_tmc_cmd : next_tmc_cmd;
//              The 'TMC Register Load' is still performed as long as TMC_DO_AWK=1.
//              Thus, in order to perform AWK, you need to use TMC_RST_AUTO_WK=1.
//
// Return  : None
//-------------------------------------------------------------------
void mrm_enable_tmc_rst_auto_wk(void);

//-------------------------------------------------------------------
// Function: mrm_enable_bist
// Args    : None   
// Description:
//          It sets the following registers.
//              TMC_SCAN_RST_N = 1
//              TMC_SCAN_TEST  = 1
//              BIST_ENABLE    = 1
// Return  : None
//-------------------------------------------------------------------
void mrm_enable_bist(void);

//-------------------------------------------------------------------
// Function: mrm_disable_bist
// Args    : None   
// Description:
//          It sets the following registers.
//              TMC_SCAN_RST_N = 0
//              TMC_SCAN_TEST  = 0
//              BIST_ENABLE    = 0
// Return  : None
//-------------------------------------------------------------------
void mrm_disable_bist(void);

//-------------------------------------------------------------------
// Function: mrm_set_clock_mode
// Args    :    
//      clock_mode  - Set TMC_CLOCK_MODE. See 'TMC_CLOCK_MODE' in the header section.
//                      Use PRE-DEFINED constants:
//                          clock_mode=MRM_SLOW_CLOCK_MODE    sets TMC_CLOCK_MODE=0.
//                          clock_mode=MRM_FAST_PP_CLOCK_MODE sets TMC_CLOCK_MODE=1.
// Description:
//      Set TMC_CLOCK_MODE. See 'TMC_CLOCK_MODE' in the header section.
// Return  : None
//-------------------------------------------------------------------
void mrm_set_clock_mode(uint32_t clock_mode);

//********************************************************************************************************************
// MRM POWER CONTROL FUNCTIONS
//********************************************************************************************************************

//-------------------------------------------------------------------
// Function: mrm_tune_ldo
// Args    :    
//      i0p8 - Sets LDO_SELB_I_CTRL_LDO_0P8 (Default: 5'h01)
//      i1p8 - Sets LDO_SELB_I_CTRL_LDO_1P8 (Default: 5'h01)
//      v0p8 - Sets LDO_SELB_VOUT_LDO_BUF (Default: 4'h4)
//      v1p8 - Sets LDO_SELB_VOUT_LDO_1P8 (Default: 4'h7)
// Description:
//      Sets the selective LDO tuning bits in MRM Register 0x23.
// Return  : None
//-------------------------------------------------------------------
void mrm_tune_ldo (uint32_t i0p8, uint32_t i1p8, uint32_t v0p8, uint32_t v1p8);

//-------------------------------------------------------------------
// Function: mrm_turn_on_ldo
// Args    : none
// Description:
//           Turn on the LDO
// Return  : 1  - Received the correct IRQ
//           0  - Received an incorrect IRQ
//-------------------------------------------------------------------
uint32_t mrm_turn_on_ldo(void);

//-------------------------------------------------------------------
// Function: mrm_turn_off_ldo
// Args    : none
// Description:
//           Turn off the LDO
// Return  : 1  - Received the correct IRQ
//           0  - Received an incorrect IRQ
//-------------------------------------------------------------------
uint32_t mrm_turn_off_ldo(void);

//-------------------------------------------------------------------
// Function: mrm_turn_on_macro
// Args    : 
//      mid - Macro ID (valid range: 0 - 1)
//              Valid Range: [0, 1]
// Description:
//           Turn on the specified MRAM macro
// Return  : 1  - Received the correct IRQ
//           0  - Received an incorrect IRQ
//-------------------------------------------------------------------
uint32_t mrm_turn_on_macro(uint32_t mid);

//-------------------------------------------------------------------
// Function: mrm_turn_off_macro
// Args    : none
// Description:
//           Turn off all MRAM macros
// Return  : 1  - Received the correct IRQ
//           0  - Received an incorrect IRQ
//-------------------------------------------------------------------
uint32_t mrm_turn_off_macro(void);

//-------------------------------------------------------------------
// Function: mrm_set_tpwr
// Args    : tpwr   - Tpwr value
//                      MRMv1S Default: 200 (=1ms)
//                      Valid Range: [0, 65535] (= 0 ~ 327ms)
// Description:
//           Sets the Tpwr parameter
// Return  : none
//-------------------------------------------------------------------
void mrm_set_tpwr(uint32_t tpwr);

//********************************************************************************************************************
// MRM ELEMENTARY FUNCTIONS
//********************************************************************************************************************

//-------------------------------------------------------------------
// Function: mrm_cmd_go
// Args    : 
//      cmd         - MRAM Command
//      len_1       - 'Num Words - 1'
//                      Valid Range: [0, 16383]
//      expected    - Expected IRQ value. 
//                      If expected=0, it does not check the IRQ value.
// Description:
//           Set the MRAM command and writes GO=1, with IRQ_EN=1.
// Return  : 
//      If it receives the correct IRQ value -OR- 'expected=0',
//          it returns 1.
//      If it receives an incorrect IRQ value -AND- 'expected!=0',
//          it returns the received incorrect IRQ value.
//-------------------------------------------------------------------
uint32_t mrm_cmd_go (uint32_t cmd, uint32_t len_1, uint32_t expected);

//-------------------------------------------------------------------
// Function: mrm_cmd_go_noirq
// Args    : 
//      cmd     - MRAM Command
//      len_1   - 'Num Words - 1'
//                  Valid Range: [0, 16383]
// Description:
//           Set the MRAM command and writes GO=1, with IRQ_EN=0.
// Return  : none
//-------------------------------------------------------------------
void mrm_cmd_go_noirq (uint32_t cmd, uint32_t len_1);

//-------------------------------------------------------------------
// Function: mrm_set_sram_addr
// Args    : 
//      addr - SRAM Address
//              Valid Range: [0, 16383]
// Description:
//           Set the SRAM address (SRAM_START_ADDR)
// Return  : none
//-------------------------------------------------------------------
void mrm_set_sram_addr (uint32_t addr);

//-------------------------------------------------------------------
// Function: mrm_set_mram_addr
// Args    : 
//      addr - MRAM Address
//              Valid Range: [0, 1048575]
// Description:
//           Set the MRAM address (MRAM_START_ADDR)
// Return  : none
//-------------------------------------------------------------------
void mrm_set_mram_addr (uint32_t addr);


//********************************************************************************************************************
// MRM's SRAM <-> MRM's MRAM
//********************************************************************************************************************

//-------------------------------------------------------------------
// Function: mrm_mram2sram
// Args    : 
//      mram_pid    - Start Page ID of MRAM.
//                      Valid Range: [0, 32767]
//      num_pages   - Number of pages to be copied.
//                      Valid Range: [1, 512]
//      sram_pid    - Start Page ID of SRAM.
//                      Valid Range: [0, 511]
// Description:
//           Copy the given number of pages ('num_pages') from MRAM to SRAM (i.e., MRAM Read).
//           MRAM's Page#(mram_pid+n) is written to SRAM's Page#(sram_pid+n), where n = 0, 1, ... , (num_pages-1).
// Return  : 
//      If it receives the correct IRQ value, it returns 1.
//      If it receives an incorrect IRQ value, it returns the received incorrect IRQ value.
//-------------------------------------------------------------------
uint32_t mrm_mram2sram (uint32_t mram_pid, uint32_t num_pages, uint32_t sram_pid);

//-------------------------------------------------------------------
// Function: mrm_sram2mram
// Args    : 
//      sram_pid    - Start Page ID of SRAM.
//                      Valid Range: [0, 511]
//      num_pages   - Number of pages to be copied.
//                      Valid Range: [1, 512]
//      mram_pid    - Start Page ID of MRAM.
//                      Valid Range: [0, 32767]
// Description:
//           Copy the given number of pages ('num_pages') from SRAM to MRAM (i.e., MRAM Write).
//           SRAM's Page#(sram_pid+n) is written to MRAM's Page#(mram_pid+n), 
//              where n = 0, 1, ... , (num_pages-1).
// Return  : 
//      If it receives the correct IRQ value, it returns 1.
//      If it receives an incorrect IRQ value, it returns the received incorrect IRQ value.
//-------------------------------------------------------------------
uint32_t mrm_sram2mram (uint32_t sram_pid, uint32_t num_pages, uint32_t mram_pid);




//********************************************************************************************************************
// PRC/PRE's SRAM <-> MRM's SRAM
//********************************************************************************************************************

//-------------------------------------------------------------------
// Function: mrm_read_sram
// Args    : 
//      mrm_sram_addr   - SRAM Byte Address (word-aligned) of MRM to start reading from.
//                          Valid Range: [0x0, 0xFFFC]
//      num_words       - Number of words to read/write.
//                          Valid Range: [1, 16384]
//      prc_sram_addr   - SRAM Byte Address (word-aligned) of PRC/PRE to write the data.
//                          Valid Range: [0x0, 0x3FFC]
// Description:
//           Read from MRM's SRAM, and write the data into PRC/PRE's SRAM.
// Return  : 
//      none
//-------------------------------------------------------------------
void mrm_read_sram (uint32_t* mrm_sram_addr, uint32_t num_words, uint32_t* prc_sram_addr);

//-------------------------------------------------------------------
// Function: mrm_read_sram_debug
// Args    : 
//      mrm_sram_addr   - SRAM Byte Address (word-aligned) of MRM to start reading from.
//                          Valid Range: [0x0, 0xFFFC]
//      num_words       - Number of words to read/write.
//                          Valid Range: [1, 16384]
//      dest_prefix     - MBus Short Prefix to be used in the reply
//                          Valid Range: [0x2, 0xE]
// Description:
//           Read from MRM's SRAM, and send it through MBus
// Return  : 
//      none
//-------------------------------------------------------------------
void mrm_read_sram_debug (uint32_t* mrm_sram_addr, uint32_t num_words, uint32_t dest_prefix);

//-------------------------------------------------------------------
// Function: mrm_write_sram
// Args    : 
//      prc_sram_addr   - SRAM Byte Address (word-aligned) of PRC/PRE to start reading from.
//                          Valid Range: [0x0, 0x3FFC]
//      num_words       - Number of words to read/write.
//                          Valid Range: [1, 16384]
//      mrm_sram_addr   - SRAM Byte Address (word-aligned) of MRM to write the data.
//                          Valid Range: [0x0, 0xFFFC]
// Description:
//           Read from PRC/PRE's SRAM, and write the data into MRM's SRAM.
// Return  : 
//      none
//-------------------------------------------------------------------
void mrm_write_sram (uint32_t* prc_sram_addr, uint32_t num_words, uint32_t* mrm_sram_addr);

//-------------------------------------------------------------------
// Function: mrm_read_sram_page
// Args    : 
//      mrm_sram_pid    - MRM SRAM's Page ID to start reading from.
//                          Valid Range: [0, 511]
//      num_pages       - Number of pages to read/write.
//                          Valid Range: [1, 512]
//      prc_sram_addr   - SRAM Byte Address (word-aligned) of PRC/PRE to write the data.
//                          Valid Range: [0x0, 0x3FFC]
// Description:
//           Read from MRM's SRAM, and write the data into PRC/PRE's SRAM.
// Return  : 
//      none
//-------------------------------------------------------------------
void mrm_read_sram_page (uint32_t mrm_sram_pid, uint32_t num_pages, uint32_t* prc_sram_addr);

//-------------------------------------------------------------------
// Function: mrm_read_sram_page_debug
// Args    : 
//      mrm_sram_pid    - MRM SRAM's Page ID to start reading from.
//                          Valid Range: [0, 511]
//      num_pages       - Number of pages to read/write.
//                          Valid Range: [1, 512]
//      dest_prefix     - MBus Short Prefix to be used in the reply
//                          Valid Range: [0x2, 0xE]
// Description:
//           Read from MRM's SRAM, and send it through MBus
// Return  : 
//      none
//-------------------------------------------------------------------
void mrm_read_sram_page_debug (uint32_t mrm_sram_pid, uint32_t num_pages, uint32_t dest_prefix);

//-------------------------------------------------------------------
// Function: mrm_write_sram_page
// Args    : 
//      prc_sram_addr   - SRAM Byte Address (word-aligned) of PRC/PRE to start reading from.
//                          Valid Range: [0x0, 0x3FFC]
//      num_pages       - Number of pages to read/write.
//                          Valid Range: [1, 512]
//      mrm_sram_pid    - MRM SRAM's Page ID to write the data.
//                          Valid Range: [0, 511]
// Description:
//           Read from PRC/PRE's SRAM, and write the data into MRM's SRAM.
// Return  : 
//      none
//-------------------------------------------------------------------
void mrm_write_sram_page (uint32_t* prc_sram_addr, uint32_t num_pages, uint32_t mrm_sram_pid);


//********************************************************************************************************************
// PRC/PRE's SRAM <-> MRM's MRAM
// -------------------------------------------------------------------------------------------------------------------
// NOTE: These functions overwrites MRAM's SRAM.
//********************************************************************************************************************

//-------------------------------------------------------------------
// Function: mrm_read_mram_page
// Args    : 
//      mrm_mram_pid    - MRM MRAM's Page ID to start reading from.
//                          Valid Range: [0, 32767]
//      num_pages       - Number of pages to read/write.
//                          Valid Range: [1, 512]
//      prc_sram_addr   - SRAM Byte Address (word-aligned) of PRC/PRE to write the data.
//                          Valid Range: [0x0, 0x3FFC]
// Description:
//           Read from MRM's MRAM, and write the data into PRC/PRE's SRAM.
// Return  : 
//      If it receives the correct IRQ value during MRAM->SRAM, it returns 1.
//      If it receives an incorrect IRQ value during MRAM->SRAM, it returns the received incorrect IRQ value.
//-------------------------------------------------------------------
uint32_t mrm_read_mram_page (uint32_t mrm_mram_pid, uint32_t num_pages, uint32_t* prc_sram_addr);

//-------------------------------------------------------------------
// Function: mrm_read_mram_page_debug
// Args    : 
//      mrm_mram_pid    - MRM MRAM's Page ID to start reading from.
//                          Valid Range: [0, 32767]
//      num_pages       - Number of pages to read/write.
//                          Valid Range: [1, 512]
//      dest_prefix     - MBus Short Prefix to be used in the reply
//                          Valid Range: [0x2, 0xE]
// Description:
//           Read from MRM's MRAM, and send it through MBus
// Return  : 
//      If it receives the correct IRQ value during MRAM->SRAM, it returns 1.
//      If it receives an incorrect IRQ value during MRAM->SRAM, it returns the received incorrect IRQ value.
//-------------------------------------------------------------------
uint32_t mrm_read_mram_page_debug (uint32_t mrm_mram_pid, uint32_t num_pages, uint32_t dest_prefix);

//-------------------------------------------------------------------
// Function: mrm_write_mram_page
// Args    : 
//      prc_sram_addr   - SRAM Byte Address (word-aligned) of PRC/PRE to start reading from.
//                          Valid Range: [0x0, 0x3FFC]
//      num_pages       - Number of pages to read/write.
//                          Valid Range: [1, 512]
//      mrm_mram_pid    - MRM MRAM's Page ID to write the data.
//                          Valid Range: [0, 32767]
// Description:
//           Read from PRC/PRE's SRAM, and write the data into MRM's MRAM.
// Return  : 
//      none
//-------------------------------------------------------------------
uint32_t mrm_write_mram_page (uint32_t* prc_sram_addr, uint32_t num_pages, uint32_t mrm_mram_pid);


//********************************************************************************************************************
// EXTERNAL PING-PONG STREAMING
//********************************************************************************************************************

//-------------------------------------------------------------------
// Function: mrm_pp_ext_stream
// Args    : 
//      bit_en      - External Data pads selection.
//                      Valid Values:
//                          1: Use DATA_EXT[0] only (1-bit mode)
//                          2: Use DATA_EXT[1] only (1-bit mode)
//                          3: Use DATA_EXT[1:0] (2-bit mode)
//      num_pages   - Number of pages to be streamed.
//                      Valid Range: [0, 32768]
//                      Once it receives the specified number of pages,
//                          the ping-pong mode automatically finishes.
//                      num_pages=0 enables 'unlimited' ping-pong stream. 
//                          However, it is recommeded to use mrm_pp_ext_stream_unlim()
//                          for unlimited length of ping-pong streaming.
//      mrm_page_id - MRM MRAM's Page ID to start writing the received streaming data.
//                      Valid Range: [0, 32767]
// Description:
//      Configure and start the Ping-Pong External Streaming mode.
//      Once MRM receives the specified number of pages ('num_pages'), 
//      it finishes the ping-pong mode, and goes back to standby.
// Return  : 
//      IRQ data received from MRM.
//          0xE2 - MRM successfully received the number of pages specified in 'num_pages', hence finished the ping-pong mode.
//          0xE4 - Ping-pong mode has been forcefully stopped by user.
//          0xE5 - Buffer overrun has occurred.
//          0xE6 - The external streaming speed is too fast.
//-------------------------------------------------------------------
uint32_t mrm_pp_ext_stream (uint32_t bit_en, uint32_t num_pages, uint32_t mram_page_id);

//-------------------------------------------------------------------
// Function: mrm_pp_ext_stream_unlim.
// Args    : 
//      bit_en      - External Data pads selection.
//                      Valid Values:
//                          1: Use DATA_EXT[0] only (1-bit mode)
//                          2: Use DATA_EXT[1] only (1-bit mode)
//                          3: Use DATA_EXT[1:0] (2-bit mode)
//      mrm_page_id - MRM MRAM's Page ID to start writing the received streaming data.
//                      Valid Range: [0, 32767]
// Description:
//      Configure and start the Unlimited Ping-Pong External Streaming mode.
//      It stays in the ping-pong mode until one of the following events happens.
//          - Ping-pong mode has been forcefully stopped by user.
//          - Buffer overrun has occurred.
//          - The external streaming speed is too fast.
//      Use mrm_stop_pp_ext_stream() to forcefully stop the ping-pong mode.
// Return  : 
//      none
//-------------------------------------------------------------------
void mrm_pp_ext_stream_unlim (uint32_t bit_en, uint32_t mram_page_id);

//-------------------------------------------------------------------
// Function: mrm_stop_pp_ext_stream
// Args    : none
// Description:
//      Forcefully stop the ping-pong mode.
//      It is supposed to be used after enabling the unlimited ping-pong mode (mrm_pp_ext_stream_unlim())
// Return  : 
//      IRQ data received from MRM.
//          0xE4 - Ping-pong mode has been forcefully stopped by user.
//          0xE5 - Buffer overrun has occurred.
//          0xE6 - The external streaming speed is too fast.
//-------------------------------------------------------------------
uint32_t mrm_stop_pp_ext_stream (void);

#endif  // MRMV1S_H
