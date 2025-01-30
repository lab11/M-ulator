//*******************************************************************
//Author: Yejoong Kim
//Description: MRMv1L header file
//*******************************************************************
//
//********************************************************************************************************************
// ADDRESS SCHEME (MRMv1L)
//********************************************************************************************************************
// A macro: 2MB (16Mb), 128-bit I/O, 128B (1kb) Page, 16384 pages, 32:1 Y-MUX
//
//  +---------------+-----------+-----------+--------------------------------------------+---------------------------+
//  |               |           |           |                   MRAM                     |           SRAM            |
//  |   Number of   |   Per     |   Per     +-----------+---------------+----------------+-------------+-------------+
//  |               |   Qword   |   Page    | Per Row   | Per Macro     | Per MRAM       | Per Macro   | Per SRAM    |
//  +---------------+-----------+-----------+-----------+---------------+----------------+-------------+-------------+
//  | SRAM Macros   |    -      |    -      |    -      |        -      |         -      |      1 ( 0) |      2 ( 1) |
//  | MRAM Macros   |    -      |    -      |    -      |        1 ( 0) |         6 ( 3) |      -      |      -      |
//  +---------------+-----------+-----------+-----------+---------------+----------------+-------------+-------------+
//  | Rows          |    -      |    -      |    1 ( 0) |     4096 (12) |     24576 (15) |     64 ( 6) |    128 ( 7) |
//  | Pages         |    -      |    1 ( 0) |    4 ( 2) |    16384 (14) |     98304 (17) |    256 ( 8) |    512 ( 9) |
//  +---------------+-----------+-----------+-----------+---------------+----------------+-------------+-------------+
//  | Qwords (128b) |    1 ( 0) |    8 ( 3) |   32 ( 5) |   131072 (17) |    786432 (20) |   2048 (11) |   4096 (12) |
//  | Words  (32b)  |    4 ( 2) |   32 ( 5) |  128 ( 7) |   524288 (19) |   3145728 (22) |   8192 (13) |  16384 (14) |
//  +---------------+-----------+-----------+-----------+---------------+----------------+-------------+-------------+
//  | Bytes         |   16 ( 4) |  128 ( 7) |  512 ( 9) |  2097152 (21) |  12582912 (24) |  32768 (15) |  65536 (16) |
//  | Bits          |  128 ( 7) | 1024 (10) | 4096 (12) | 16777216 (24) | 100663296 (27) | 262144 (18) | 524288 (19) |
//  +---------------+-----------+-----------+-----------+---------------+----------------+-------------+-------------+
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

#ifndef MRMV1L_H
#define MRMV1L_H
#define MRMV1L

//*******************************************************************
// INCLUDES...
//*******************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "mbus.h"
#include "PREv23E.h"

//*******************************************************************************************
// MRM CONFIGURATIONS
//*******************************************************************************************
//              Chip    Recommended 
//                      V0P8    V1P8
//          ----------------------------------------
//           MRMv1L#1:  
//           MRMv1L#2:  
//           MRMv1L#3:  
//           MRMv1L#4:  3       5
//           MRMv1L#5:  8       6 (to be confirmed)
//           MRMv1L#6:  
//           ARETE#7 :  0       4
#define MRM_LDO_VOUT_V0P8       0   // Default: 4 [Range: 0 - 15] Sets LDO_SELB_VOUT_LDO_BUF
#define MRM_LDO_VOUT_V1P8       4   // Default: 7 [Range: 0 - 15] Sets LDO_SELB_VOUT_LDO_0P8

// MRMv1L's Clock Generator Tuning and MRAM Timing Parameters. 
// See the comments section of mrm_init() in MRMv1L.h.
//              Chip    Recommended 
//                      clk_gen_S
//          ----------------------------------------
//           MRMv1L#1:  58
//           MRMv1L#2:  not measured
//           MRMv1L#3:  not measured
//           MRMv1L#4:  51  (200.472 kHz / 51.3 MHz)
//           MRMv1L#5:  53  (200.982 kHz / 51.5 MHz)
//           MRMv1L#6:  53  (Not yet measured, but 53 worked fine for Macro#0-#2)
//           ARETE#7 :  64
#define MRM_CLK_GEN_S       64

//---   Timing                  CLK_FAST Freq       Parameter
//      Parameter           50MHz   75MHz   100MHz  Description
#define MRM_TPGM            11    /*16 */ /*22 */   //[FAST CLOCK] Write pulse width; = (TPGM + 1) x Tclk_fast; Min 250ns, Max 275ns
#define MRM_TW2R_S          0     /*0  */ /*0  */   //[SLOW CLOCK] Write to Read; = (16^TW2R[7]) x TW2R[6:0] x Tclk_slow; Min 1us
#define MRM_TWRS_S          0     /*1  */ /*1  */   //[SLOW CLOCK] Read to Write; = (16^TWRS[7]) x TWRS[6:0] x Tclk_slow; Min 3us
#define MRM_TRDL_S          0     /*0  */ /*0  */   //[SLOW CLOCK] READ low pulse; = (TRDL + 1) x Tclk_slow; Min 200ns
#define MRM_TCYCRD1_S       0     /*0  */ /*0  */   //[SLOW CLOCK] When the read state exeed 16 clocks, it uses TCYCRD1 to control for BL leak measurement
#define MRM_TRDS_S          0     /*0  */ /*0  */   //[SLOW CLOCK] READ Rising to 1st Clk; = TRDS[8] x (Twk[7:0] + 1) x Tclk_slow; Min 100ns
#define MRM_TWK_S           0     /*1  */ /*1  */   //[SLOW CLOCK] Wakeup Time; = (16^TWK[7]) x TWK[6:0] x Tclk_slow; Min 3us
#define MRM_TCYCRD_S        0     /*0  */ /*0  */   //[SLOW CLOCK] Read Cycle; = (TCYCRD + 1) x Tclk_slow; Min 16.5ns
#define MRM_TPGM_OTP_RBD    29    /*43 */ /*57 */   //[FAST CLOCK] OTP RBD Tprog Pulse Width = (16 x TPGM_OTP_RBD + 1) x Tclk_fast; Min 9000ns, Max 11000ns
#define MRM_TPGM_OTP_RAP    5     /*7  */ /*10 */   //[FAST CLOCK] OTP RAP Tprog Pulse Width = (TPGM_OTP_RAP + 1) x Tclk_fast; Min 100ns, Max 140ns
#define MRM_TPGM_OTP_RP     5     /*7  */ /*10 */   //[FAST CLOCK] OTP RP Tprog Pulse Width = (TPGM_OTP_RP + 1) x Tclk_fast; Min 100ns, Max 140ns
#define MRM_TW2R_F          50    /*75 */ /*100*/   //[FAST CLOCK] Write to Read; = (16^TW2R[7]) x TW2R[6:0] x Tclk_fast; Min 1us
#define MRM_TWRS_F          137   /*142*/ /*146*/   //[FAST CLOCK] Read to Write; = (16^TWRS[7]) x TWRS[6:0] x Tclk_fast; Min 3us
#define MRM_TWK_F           134   /*137*/ /*140*/   //[FAST CLOCK] Wakeup Time; = (16^TWK[7]) x TWK[6:0] x Tclk_fast; Min 3us
#define MRM_TRDL_F          10    /*15 */ /*20 */   //[FAST CLOCK] READ low pulse; = (TRDL + 1) x Tclk_fast; Min 200ns
#define MRM_TRDS_F          5     /*7  */ /*10 */   //[FAST CLOCK] READ Rising to 1st Clk; = TRDS[8] x (Twk[7:0] + 1) x Tclk_fast; Min 100ns
#define MRM_TCYCRD1_F       0     /*0  */ /*0  */   //[FAST CLOCK] When the read state exeed 16 clocks, it uses TCYCRD1 to control for BL leak measurement
#define MRM_TCYCRD_F        0     /*1  */ /*1  */   //[FAST CLOCK] Read Cycle; = (TCYCRD + 1) x Tclk_fast; Min 16.5ns

//--- Group the timing parameters together
#define MRM_TPAR_0x30   (MRM_TPGM<<16)|(MRM_TW2R_S<<8)|(MRM_TWRS_S<<0)
#define MRM_TPAR_0x31   (MRM_TRDL_S<<4)|(MRM_TCYCRD1_S<<0)
#define MRM_TPAR_0x32   (MRM_TRDS_S<<12)|(MRM_TWK_S<<4)|(MRM_TCYCRD_S<<0)
#define MRM_TPAR_0x33   (MRM_TPGM_OTP_RBD<<12)|(MRM_TPGM_OTP_RAP<<6)|(MRM_TPGM_OTP_RP<<0)
#define MRM_TPAR_0x34   (MRM_TW2R_F<<16)|(MRM_TWRS_F<<8)|(MRM_TWK_F<<0)
#define MRM_TPAR_0x35   (MRM_TRDL_F<<17)|(MRM_TRDS_F<<8)|(MRM_TCYCRD1_F<<4)|(MRM_TCYCRD_F<<0)

//*******************************************************************
// PREDEFINED CONSTANTS
//*******************************************************************

#define MRM_NUM_SRAM_MACROS             2
#define MRM_NUM_MRAM_MACROS             6
#define MRM_NUM_PAGES_SRAM              512
#define MRM_NUM_PAGES_MRAM              98304
#define MRM_NUM_PAGES_MRAM_MACRO        16384
#define MRM_LOG2_NUM_PAGES_MRAM_MACRO   14
#define MRM_LOG2_NUM_WORDS_PER_PAGE     5

#define MRM_NUM_BITS_PER_WORD   32
#define MRM_NUM_WORDS_PER_PAGE  32

// Each MBus message can carry up to 64kB (512 pages)
// IMPORTANT: It MUST be NUM_MAX_PAGES_MBUS <= MRM_NUM_PAGES_SRAM.
//            Otherwise, the code may not work.
//NOTE: MBus Analyzer in Salaea Logic 1 can decode the 64kB per MBus message without an issue.
//      However, MBus Analyzer in Salaea Logic 2 can only decode up to 7.99kB per MBus message.
//      Thus, NUM_MAX_PAGES_MBUS must be 32 (pages), which corresponds to 4kB.
#define NUM_MAX_PAGES_MBUS      512
//#define NUM_MAX_PAGES_MBUS      32  

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
#define MRM_EXP_TMC_CMD         0xC7
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

//--- TMC COMMAND (Use with MRM_CMD_TMC_CMD)
#define MRM_TMC_READ            1
#define MRM_TMC_LOAD            2
#define MRM_TMC_WRITE           3
#define MRM_TMC_CLR_LOAD        4
#define MRM_TMC_WRITE_CONFIG    5
#define MRM_TMC_READ_CONFIG     6
#define MRM_TMC_WAKEUP          12
#define MRM_TMC_AUTO_WAKEUP     13
#define MRM_TMC_WRITE_2CPB      14
#define MRM_TMC_WRITE_OTP       15
#define MRM_TMC_RECALL          22
#define MRM_TMC_WRITE_SRAM      23
#define MRM_TMC_READ_SRAM       24
#define MRM_TMC_SET_TRIM        25

//*******************************************************************
// GLOBAL VARIABLES
//*******************************************************************
volatile uint32_t  __mrm_prefix__;
volatile uint32_t  __mrm_irq_reg_idx__;
volatile uint32_t* __mrm_irq_reg_addr__;
volatile uint32_t  __mrm_ldo_vout_v0p8__;
volatile uint32_t  __mrm_ldo_vout_v1p8__;


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
//      clk_gen_s   - CLK_GEN_S value.
//                      Valid Range: [0 - 127]; Less value means a higher frequency.
//                      GOAL: Try to make CLK_SLOW=199kHz (CLK_FAST = 50MHz)
//
//    // [NOTE] Seems like the MEAS_CLK_FREQ command does not work.
//    //          It may be due to the wrong condition defined in MRMv1L_def_common.v, which is:
//    //              `define TPCOND_FULL (time_par_cnt[19:0] == {`OP_TIMEPAR_WIDTH{1'b1}})
//    //          Since `OP_TIMEPAR_WIDTH is 24, the above condition may never be met.
//    //
//    //          Thus, In 'test_0' program, I am using an indirect approach, using Tpwr.
//    //          I turn on the MRAM macro w/ Tpwr=0 and Tpwr=0xFFFF, and see the difference.
//    //            
//    //          See the comment section in test_0.c, "Clock Frequency Measurement"
//
//      tpar_30     -   (TPGM<<16)|(TW2R_S<<8)|(TWRS_S<<0)
//                          TPGM        : [FAST CLOCK] Write pulse width; = (TPGM + 1) x Tclk_fast; Min 250ns, Max 275ns
//                          TW2R_S      : [SLOW CLOCK] Write to Read; = (16^TW2R[7]) x TW2R[6:0] x Tclk_slow; Min 1us
//                          TWRS_S      : [SLOW CLOCK] Read to Write; = (16^TWRS[7]) x TWRS[6:0] x Tclk_slow; Min 3us
//      tpar_31     -   (TRDL_S<<4)|(TCYCRD1_S<<0)
//                          TRDL_S      : [SLOW CLOCK] READ low pulse; = (TRDL + 1) x Tclk_slow; Min 200ns
//                          TCYCRD1_S   : [SLOW CLOCK] When the read state exeed 16 clocks, it uses TCYCRD1 to control for BL leak measurement
//      tpar_32     -   (TRDS_S<<12)|(TWK_S<<4)|(TCYCRD_S<<0)
//                          TRDS_S      : [SLOW CLOCK] READ Rising to 1st Clk; = TRDS[8] x (Twk[7:0] + 1) x Tclk_slow; Min 100ns
//                          TWK_S       : [SLOW CLOCK] Wakeup Time; = (16^TWK[7]) x TWK[6:0] x Tclk_slow; Min 3us
//                          TCYCRD_S    : [SLOW CLOCK] Read Cycle; = (TCYCRD + 1) x Tclk_slow; Min 16.5ns
//      tpar_33     -   (TPGM_OTP_RBD<<12)|(TPGM_OTP_RAP<<6)|(TPGM_OTP_RP<<0)
//                          TPGM_OTP_RBD: [FAST CLOCK] OTP RBD Tprog Pulse Width = (16 x TPGM_OTP_RBD + 1) x Tclk_fast; Min 9000ns, Max 11000ns
//                          TPGM_OTP_RAP: [FAST CLOCK] OTP RAP Tprog Pulse Width = (TPGM_OTP_RAP + 1) x Tclk_fast; Min 100ns, Max 140ns
//                          TPGM_OTP_RP : [FAST CLOCK] OTP RP Tprog Pulse Width = (TPGM_OTP_RP + 1) x Tclk_fast; Min 100ns, Max 140ns
//      tpar_34     -   (TW2R_F<<16)|(TWRS_F<<8)|(TWK_F<<0)
//                          TW2R_F      : [FAST CLOCK] Write to Read; = (16^TW2R[7]) x TW2R[6:0] x Tclk_fast; Min 1us
//                          TWRS_F      : [FAST CLOCK] Read to Write; = (16^TWRS[7]) x TWRS[6:0] x Tclk_fast; Min 3us
//                          TWK_F       : [FAST CLOCK] Wakeup Time; = (16^TWK[7]) x TWK[6:0] x Tclk_fast; Min 3us
//      tpar_35     -   (TRDL_F<<17)|(TRDS_F<<8)|(TCYCRD1_F<<4)|(TCYCRD_F<<0)
//                          TRDL_F      : [FAST CLOCK] READ low pulse; = (TRDL + 1) x Tclk_fast; Min 200ns
//                          TRDS_F      : [FAST CLOCK] READ Rising to 1st Clk; = TRDS[8] x (Twk[7:0] + 1) x Tclk_fast; Min 100ns
//                          TCYCRD1_F   : [FAST CLOCK] When the read state exeed 16 clocks, it uses TCYCRD1 to control for BL leak measurement
//                          TCYCRD_F    : [FAST CLOCK] Read Cycle; = (TCYCRD + 1) x Tclk_fast; Min 16.5ns
//
// Description:
//           Initializes MRMv1L
//
//              Chip    Recommended 
//                      clk_gen_S
//          ----------------------------------------
//           MRMv1L#1:  58
//           MRMv1L#2:  not measured
//           MRMv1L#3:  not measured
//           MRMv1L#4:  51  (200.472 kHz / 51.3 MHz)
//           MRMv1L#5:  53  (200.982 kHz / 51.5 MHz)
//
// Return  : None
//-------------------------------------------------------------------
void mrm_init(uint32_t mrm_prefix, uint32_t irq_reg_idx, uint32_t clk_gen_s, uint32_t tpar_30, uint32_t tpar_31, uint32_t tpar_32, uint32_t tpar_33, uint32_t tpar_34, uint32_t tpar_35);

//-------------------------------------------------------------------
// Function: mrm_enable_auto_power_on_off
// Args    : None
// Description:
//           Enable the Auto-Power On/Off feature
// Return  : None
//-------------------------------------------------------------------
void mrm_enable_auto_power_on_off(void);

//-------------------------------------------------------------------
// Function: mrm_disable_auto_power_on_off
// Args    : None
// Description:
//           Disable the Auto-Power On/Off feature
// Return  : None
//-------------------------------------------------------------------
void mrm_disable_auto_power_on_off(void);

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

//-------------------------------------------------------------------
// Function: mrm_set_clock_tune
// Args    :    
//      s   - Set CLK_GEN_S = s
// Description:
//      Set CLK_GEN_S
// Return  : None
//-------------------------------------------------------------------
void mrm_set_clock_tune(uint32_t s);

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
//      mid - Macro ID 
//              Valid Range: [0, 5]
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
//                      MRMv1L Default: 200 (=1ms)
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
// Function: mrm_cmd_go_irq_nowait
// Args    : 
//      cmd     - MRAM Command
//      len_1   - 'Num Words - 1'
//                  Valid Range: [0, 16383]
// Description:
//           Set the MRAM command and writes GO=1, with IRQ_EN=1.
//           It does not wait for the IRQ.
// Return  : none
//-------------------------------------------------------------------
void mrm_cmd_go_irq_nowait (uint32_t cmd, uint32_t len_1);

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
//              Valid Range: [0, 3145727]
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
//                      Valid Range: [0, 98303]
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
//                      Valid Range: [0, 98303]
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
//                          Valid Range: [0, 98303]
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
//                          Valid Range: [0, 98303]
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
//                          Valid Range: [0, 98303]
// Description:
//           Read from PRC/PRE's SRAM, and write the data into MRM's MRAM.
// Return  : 
//      none
//-------------------------------------------------------------------
uint32_t mrm_write_mram_page (uint32_t* prc_sram_addr, uint32_t num_pages, uint32_t mrm_mram_pid);

//********************************************************************************************************************
// EXTERNAL STREAMING
//********************************************************************************************************************

//-------------------------------------------------------------------
// Function: mrm_ext_stream
// Args    : 
//      bit_en      - External Data pads selection.
//                      Valid Values:
//                          1: Use DATA_EXT[0] only (1-bit mode)
//                          2: Use DATA_EXT[1] only (1-bit mode)
//                          3: Use DATA_EXT[1:0] (2-bit mode)
//      num_pages   - Number of pages to be streamed.
//                      Valid Range: [1, 512]
//                      Once it receives the specified number of pages,
//                          the external streaming mode automatically finishes.
//                      'num_pages=0' case is NOT implemented.
//      mrm_sram_pid - MRM SRAM's Page ID to start writing the received streaming data.
//                      Valid Range: [0, 511]
// Description:
//      Configure and start the External Streaming mode.
//      Once MRM receives the specified number of pages ('num_pages'), 
//      it finishes the external streaming mode, and goes back to standby.
// Return  : 
//      IRQ data received from MRM.
//          0xE2 - MRM successfully received the number of pages specified in 'num_pages', hence finished the ping-pong mode.
//          0xE4 - Ping-pong mode has been forcefully stopped by user.
//          0xE5 - Buffer overrun has occurred.
//          0xE6 - The external streaming speed is too fast.
//-------------------------------------------------------------------
uint32_t mrm_ext_stream (uint32_t bit_en, uint32_t num_pages, uint32_t mrm_sram_pid);


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
//                      Valid Range: [0, 98304]
//                      Once it receives the specified number of pages,
//                          the ping-pong mode automatically finishes.
//                      num_pages=0 enables 'unlimited' ping-pong stream. 
//                          However, it is recommeded to use mrm_pp_ext_stream_unlim()
//                          for unlimited length of ping-pong streaming.
//      mrm_page_id - MRM MRAM's Page ID to start writing the received streaming data.
//                      Valid Range: [0, 98303]
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
//                      Valid Range: [0, 98303]
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

//-------------------------------------------------------------------
// Function: mrm_tmc_cmd
// Args    : cmd    - TMC Command
//           xadr   - TMC XADR
//           yadr   - TMC YADR
//           din    - TMC DIN (currently supports the lower 32-bit only)
//           result - Pointer to which the TMC results will be stored.
// Description:
//      Issue the given TMC command
//      Once the execution is done, you can check its output, 'result'
//      which is the first 15 words in MRM SRAM.
// Return  : 
//      IRQ data received from MRM.
//          0xC7 - TMC command execution completed successfully
//-------------------------------------------------------------------
uint32_t mrm_tmc_cmd (uint32_t cmd, uint32_t xadr, uint32_t yadr, uint32_t din, uint32_t* result);

//-------------------------------------------------------------------
// Function: mrm_tmc_write_test_reg
// Args    : xadr   - Test Register Address (xadr)
//           wdata  - Data to be written to the Test Register
// Description:
//      It writes wdata into the TMC's Test Register (xadr).
//      It first writes 16'h0001 in R_TEST, then writes the wdata to the specified Test Register,
//      then writes 16'h0000 in R_TEST.
//
//      IMPORTANT: You must DISABLE the auto-power on/off feature,
//                  because R_TEST gets reset whenever the TMC gets reset.
//
// Return  : 
//      None
//-------------------------------------------------------------------
void mrm_tmc_write_test_reg (uint32_t xadr, uint32_t wdata);

#endif  // MRMV1L_H
