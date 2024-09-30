//*******************************************************************************************
// XT1 (TMCv2r1) FIRMWARE
// Version 3.23 (devel)
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

//*******************************************************************************************
// COMPILE SETTING
//*******************************************************************************************
#define DEVEL

//*******************************************************************************************
// ENUMERATION
//*******************************************************************************************
#define MRM_ADDR 0x4

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1
#define FLAG_XO_INITIALIZED     2
#define FLAG_WD_ENABLED         3   // Only if ENABLE_EID_WATCHDOG is enabled.
#define FLAG_MAIN_CALLED_BY_SNT 4   // Sleep has been bypassed before the current active session. Thus, main() has called from within operation_sleep(). This implies that xo_val_curr is inaccurate.
#define FLAG_INVLD_XO_PREV      5   // Sleep has been bypassed before the previous active session. This implies that xo_val_prev is inaccurate.
#define FLAG_BOOTUP_DONE        6
#define FLAG_MAIN_CALLED_BY_NFC 7   // Same as FLAG_MAIN_CALLED_BY_SNT. The only difference is that it directly calls main() because it detects a GPO pulse during the 1st wakeup.
#define FLAG_TEMP_MEAS_PEND     8   // Indicates that the system has gone sleep for temperature measurement

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
#define WAKEUP_BY_SNT       WAKEUP_BY_MBUS
#define WAKEUP_BY_NFC       WAKEUP_BY_GPIO0
#define MAIN_CALLED_BY_SNT  (WAKEUP_BY_SNT && get_bit(wakeup_source, 7))
#define MAIN_CALLED_BY_NFC  (WAKEUP_BY_NFC && get_bit(wakeup_source, 7))

volatile uint32_t wakeup_source;            // Wakeup Source. Updated each time the system wakes up. See 'WAKEUP_SOURCE definition'.

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// XO Frequency
#define XO_FREQ_SEL 0                       // XO Frequency (in kHz) = 2 ^ XO_FREQ_SEL
#define XO_FREQ     1<<(XO_FREQ_SEL+10)     // XO Frequency (in Hz)

// XO Initialization Wait Duration
#define XO_WAIT_A   2*DLY_1S    // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S    // Must be ~1 second delay.

//-------------------------------------------------------------------------------------------
// Other Global Variables
//-------------------------------------------------------------------------------------------
#define BUF_SIZE 32
volatile uint32_t data[BUF_SIZE];

// MRAM GO Command
#define CMD_NOP             0x0
#define CMD_CP_MRAM2SRAM    0x1
#define CMD_CP_SRAM2MRAM    0x2
#define CMD_TMC_CMD         0x3
#define CMD_TMC_JTAG        0x4
#define CMD_EXT_WR_SRAM     0x6
#define CMD_START_BOOT      0x8
#define CMD_MEAS_CLK_FREQ   0x9

#define EXP_CP_MRAM2SRAM    0x2F
#define EXP_CP_SRAM2MRAM    0x3F
#define EXP_TMC_CMD         0x0
#define EXP_TMC_JTAG        0x0
#define EXP_EXT_WR_SRAM     0x0
#define EXP_START_BOOT      0x0
#define EXP_MEAS_CLK_FREQ   0x0

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep/IRQ Handling
static void operation_sleep (void);
static void operation_init(void);
void disable_all_irq_except_timer32_gocep(void);

//--- MRAM Functions
static void turn_on_ldo_manual (void);
static uint32_t turn_on_ldo (void);
static uint32_t sram_test (void);

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

    // Reset GOC_DATA_IRQ
    //*GOC_DATA_IRQ = 0; // GOC IRQ is not used.

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
    // XO Settings
    //-------------------------------------------------
    //--- XO Frequency
    *REG_XO_CONF2 =             // Default  // Description
        //-----------------------------------------------------------------------------------------------------------
        ( (0x1         << 20)   // (3'h1) XO_SEL_DLY            #Adjusts pulse delay
        | (0x1         << 18)   // (2'h1) XO_SEL_CLK_SCN        #Selects division ratio for SCN CLK
        | (XO_FREQ_SEL << 15)   // (3'h1) XO_SEL_CLK_OUT_DIV    #Selects division ratio for the XO CLK output. XO Freq = 2^XO_SEL_CLK_OUT_DIV (kHz)
        | (0x1         << 14)   // (1'h1) XO_SEL_LDO            #Selects LDO output as an input to SCN
        | (0x0         << 13)   // (1'h0) XO_SEL_0P6            #Selects V0P6 as an input to SCN
        | (0x0         << 10)   // (3'h0) XO_I_AMP              #Adjusts VREF body bias buffer current
        | (0x0         <<  3)   // (7'h0) XO_VREF_TUNEB         #Adjust VREF level and TC
        | (0x0         <<  0)   // (3'h0) XO_SEL_VREF           #Selects VREF output from its diode stack
        );

    //xo_stop();  // Default value of XO_START_UP is wrong in RegFile, so need to override it.

    //--- Start XO clock
    //xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);
    
    // Update the flag
    set_flag(FLAG_XO_INITIALIZED, 1);

    //--- Configure and Start the XO Counter
    //set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0, /*auto_reset*/0);
    //start_xo_cnt();

    //#ifdef ENABLE_XO_PAD
    //    start_xo_cout();
    //#endif

    //-------------------------------------------------
    // MRMv1S Setting
    //-------------------------------------------------
    // CLK_GEN Setting
    //      With CLK_GEN_S = 58, then the measured frequecies will be:
    //          CLK_SLOW = 199 kHz
    //          CLK_FAST = 51 MHz
    mbus_remote_register_write(MRM_ADDR, 0x26, /*CLK_GEN_S*/ 58);

    // Enable Auto Power ON/OFF
    mbus_remote_register_write(MRM_ADDR, 0x12, 0x0
        /* MRAM_PWR_AUTO_OFF (1'h0) */  | (0x1 << 1)
        /* MRAM_PWR_AUTO_ON  (1'h0) */  | (0x1 << 0)
    );

    // --- TMC Configuration
    mbus_remote_register_write(MRM_ADDR, 0x2F, 0x0
        /* TMC_RST_AUTO_WK  (1'h1) */   | (0x0 << 5)  // If 1, TMC automatically issues AUTO_WAKEUP upon its reset release. This is handled by TMC itself, not the CTRL. TMC_RST_AUTO_WK != TMC_DO_AWK is recommended.
        /* TMC_CLOCK_MODE   (1'h0) */   | (0x0 << 4)  // See the table in genRF.conf
        /* TMC_CHECK_ERR    (1'h0) */   | (0x0 << 3)  // If 1, it checks TMC_ERR and terminates the operation if an error occurs. If 0, it ignores TMC_ERR and proceeds.
        /* TMC_FAST_LOAD    (1'h1) */   | (0x1 << 2)  // If 1, it does not check TMC_BUSY or TMC_ERR for LOAD command. It also performs 4 SRAM readings without a cease.
        /* TMC_DO_REG_LOAD  (1'h1) */   | (0x1 << 1)  // If 1, it overwrites the selected TMC registers after auto-wakeup (Valid only when TMC_DO_AWK=1)
        /* TMC_DO_AWK       (1'h0) */   | (0x1 << 0)  // If 1, it issues TMC's Auto-Wakeup Command upon power-up. This is handled by the CTRL, not the TMC. TMC_RST_AUTO_WK != TMC_DO_AWK is recommended. For now, let's do TMC_DO_AWK=0.
    );
    
    set_flag(FLAG_INITIALIZED, 1);
}

//*******************************************************************************************
// MRAM Functions
//*******************************************************************************************

void turn_on_ldo_manual (void) {

    // Dummy Message to wakeup MRMv1S
    mbus_remote_register_write(MRM_ADDR, 0x00, 0x000000);

    delay(100000);

    // Turn on the LDO
    // MESSAGE #1
    // REGISTER 0x22
    //       [1] LDO_VREF_ENb: 1'h1 -> 1'h0
    //       [0] LDO_ISOL_LC: 1'h1 -> 1'h0
    // -----------------------------------------------
    mbus_remote_register_write(MRM_ADDR, 0x22, 0x000000);

    // MESSAGE #2
    // REGISTER 0x22
    //       [2] LDO_EN_LDO_BUF: 1'h0 -> 1'h1
    // -----------------------------------------------
    mbus_remote_register_write(MRM_ADDR, 0x22, 0x000004);

    // MESSAGE #2
    // REGISTER 0x22
    //       [3] LDO_EN_LDO_1P8: 1'h0 -> 1'h1
    //       [5] LDO_EN_DCP_1P8: 1'h0 -> 1'h1
    // -----------------------------------------------
    mbus_remote_register_write(MRM_ADDR, 0x22, 0x00002C);

    // Delay to charge the DCP_1P8 decap
    delay(1000);

    // MESSAGE #3
    // REGISTER 0x22
    //       [4] LDO_EN_LDO_0P8: 1'h0 -> 1'h1
    //       [6] LDO_EN_DCP_0P8: 1'h0 -> 1'h1
    // -----------------------------------------------
    mbus_remote_register_write(MRM_ADDR, 0x22, 0x00007C);

    // Delay to charge the DCP_0P8 decap
    delay(1000);
}

uint32_t turn_on_ldo (void) {

    // LDO Voltage Tune
    mbus_remote_register_write(MRM_ADDR, 0x23, 
       /* LDO_SELB_I_CTRL_LDO_0P8 (5'h01) */   (0x1F << 19) 
       /* LDO_SELB_I_CTRL_LDO_1P8 (5'h01) */ | (0x1F << 14) 
       /* LDO_SELB_VOUT_LDO_1P8   (4'h7 ) */ | (0x7  << 10) 
       /* LDO_SELB_VOUT_LDO_BUF   (4'h4 ) */ | (0x8  <<  6) 
       /* LDO_SEL_IBIAS_LDO_0P8   (2'h1 ) */ | (0x1  <<  4) 
       /* LDO_SEL_IBIAS_LDO_1P8   (2'h1 ) */ | (0x1  <<  2) 
       /* LDO_SEL_IBIAS_LDO_BUF   (2'h1 ) */ | (0x1  <<  0) 
    );

    *NVIC_ISER = (1 << IRQ_REG0);
    // REGISTER 0x21
    //       [2] IRQ_EN: 1'b1
    //       [1] SEL_ON: 1'b1
    //       [0] GO    : 1'b1
    // -----------------------------------------------
    mbus_remote_register_write(MRM_ADDR, 0x21, 0x000007);
    WFI();
    *NVIC_ICER = (1 << IRQ_REG0);

    if ((*REG0&0xFF) != 0x7C) return 0;

    // Delay to charge the DCP_0P8 decap
    delay(2000); // delay(1000) =~ 0.6s
    return 1;
}

//uint32_t meas_clk_freq (void) {
//    // [NOTE] Seems like the MEAS_CLK_FREQ command does not work.
//    //          It may be due to the wrong condition defined in MRMv1S_def_common.v, which is:
//    //              `define TPCOND_FULL (time_par_cnt[19:0] == {`OP_TIMEPAR_WIDTH{1'b1}})
//    //          Since `OP_TIMEPAR_WIDTH is 24, the above condition may never be met.
//    //
//    //          Thus, here I am using an indirect approach, using Tpwr.
//    //          I turn on the MRAM macro w/ Tpwr=0 and Tpwr=0xFFFF, and see the difference.
//    //
//    //          Results:
//    //              S=87:
//    //                  Measurement
//    //                      Delay w/ Tpwr=0x0000: 0.3922ms
//    //                      Delay w/ Tpwr=0xFFFF: 0.4798s
//    //                      CLK_SLOW = 65536 / (0.4798 - 0.0003922) = 136.702 kHz
//    //                      CLK_FAST = CLK_SLOW x 256 = 34.996 MHz
//    //                  Previous Simulation:
//    //                      CLK_SLOW = 240 kHz
//    //                      CLK_FAST = 61 MHz
//    //              S=58:
//    //                  Measurement
//    //                      Delay w/ Tpwr=0x0000: 0.2652ms
//    //                      Delay w/ Tpwr=0xFFFF: 0.3294s
//    //                      CLK_SLOW = 65536 / (0.3294 - 0.0002652) = 199.115 kHz
//    //                      CLK_FAST = CLK_SLOW x 256 = 50.973 MHz
//    //                  Previous Simulation:
//    //                      CLK_SLOW = 348 kHz
//    //                      CLK_FAST = 89 MHz
//    //
//    //          CONCLUSION:
//    //              Use CLK_GEN_S = 58, then the measured frequecies will be:
//    //                  CLK_SLOW = 199 kHz
//    //                  CLK_FAST = 51 MHz
//    
//    // --- CLK_GEN Tuning (Default: CLK_GEN_S = 87)
//    mbus_remote_register_write(MRM_ADDR, 0x26, /*CLK_GEN_S*/ 58);
//
//    // --- TMC Configuration
//    mbus_remote_register_write(MRM_ADDR, 0x2F, 0x0
//        /* TMC_RST_AUTO_WK  (1'h1) */   | (0x0 << 5)  // If 1, TMC automatically issues AUTO_WAKEUP upon its reset release. This is handled by TMC itself, not the CTRL. TMC_RST_AUTO_WK != TMC_DO_AWK is recommended.
//        /* TMC_CLOCK_MODE   (1'h0) */   | (0x0 << 4)  // See the table in genRF.conf
//        /* TMC_CHECK_ERR    (1'h0) */   | (0x0 << 3)  // If 1, it checks TMC_ERR and terminates the operation if an error occurs. If 0, it ignores TMC_ERR and proceeds.
//        /* TMC_FAST_LOAD    (1'h1) */   | (0x0 << 2)  // If 1, it does not check TMC_BUSY or TMC_ERR for LOAD command. It also performs 4 SRAM readings without a cease.
//        /* TMC_DO_REG_LOAD  (1'h1) */   | (0x0 << 1)  // If 1, it overwrites the selected TMC registers after auto-wakeup (Valid only when TMC_DO_AWK=1)
//        /* TMC_DO_AWK       (1'h0) */   | (0x0 << 0)  // If 1, it issues TMC's Auto-Wakeup Command upon power-up. This is handled by the CTRL, not the TMC. TMC_RST_AUTO_WK != TMC_DO_AWK is recommended. For now, let's do TMC_DO_AWK=0.
//    );
//
//    // --- Tpwr=0
//    mbus_remote_register_write(MRM_ADDR, 0x03, 0x0
//        /* Tpwr   (16'd200) */ | (0x0000 << 8)
//        /* T100ns ( 8'd0)   */ | (  0x00 << 0)
//    );
//
//    // --- Power-On 
//    *NVIC_ISER = (1 << IRQ_REG0);
//    // REGISTER 0x11
//    //       [2] MRAM_PWR_IRQ_EN: 1'b1
//    //       [1] MRAM_PWR_SEL_ON: 1'b1
//    //       [0] MRAM_PWR_GO    : 1'b1
//    // -----------------------------------------------
//    mbus_remote_register_write(MRM_ADDR, 0x11, 0x000007);
//    WFI();
//    *NVIC_ICER = (1 << IRQ_REG0);
//
//    if ((*REG0&0xFF) != 0x94) return 0;
//
//    // --- Power-Off
//    *NVIC_ISER = (1 << IRQ_REG0);
//    // REGISTER 0x11
//    //       [2] MRAM_PWR_IRQ_EN: 1'b1
//    //       [1] MRAM_PWR_SEL_ON: 1'b0
//    //       [0] MRAM_PWR_GO    : 1'b1
//    // -----------------------------------------------
//    mbus_remote_register_write(MRM_ADDR, 0x11, 0x000005);
//    WFI();
//    *NVIC_ICER = (1 << IRQ_REG0);
//
//    if ((*REG0&0xFF) != 0x98) return 0;
//
//    // --- Tpwr=0xFFFF
//    mbus_remote_register_write(MRM_ADDR, 0x03, 0x0
//        /* Tpwr   (16'd200) */ | (0xFFFF << 8)
//        /* T100ns ( 8'd0)   */ | (  0x00 << 0)
//    );
//
//    // --- Power-On 
//    *NVIC_ISER = (1 << IRQ_REG0);
//    // REGISTER 0x11
//    //       [2] MRAM_PWR_IRQ_EN: 1'b1
//    //       [1] MRAM_PWR_SEL_ON: 1'b1
//    //       [0] MRAM_PWR_GO    : 1'b1
//    // -----------------------------------------------
//    mbus_remote_register_write(MRM_ADDR, 0x11, 0x000007);
//    WFI();
//    *NVIC_ICER = (1 << IRQ_REG0);
//
//    if ((*REG0&0xFF) != 0x94) return 0;
//
//    // --- Power-Off
//    *NVIC_ISER = (1 << IRQ_REG0);
//    // REGISTER 0x11
//    //       [2] MRAM_PWR_IRQ_EN: 1'b1
//    //       [1] MRAM_PWR_SEL_ON: 1'b0
//    //       [0] MRAM_PWR_GO    : 1'b1
//    // -----------------------------------------------
//    mbus_remote_register_write(MRM_ADDR, 0x11, 0x000005);
//    WFI();
//    *NVIC_ICER = (1 << IRQ_REG0);
//
//    if ((*REG0&0xFF) != 0x98) return 0;
//
//
//    return 1;
//
//}

uint32_t turn_on_mram (uint32_t mid) {
    // --- Select Macro by Writing into MRAM_START_ADDR
    mbus_remote_register_write(MRM_ADDR, 0x08, /*MRAM_START_ADDR*/mid << 19);

    // --- Power-On 
    *NVIC_ISER = (1 << IRQ_REG0);
    // REGISTER 0x11
    //       [2] MRAM_PWR_IRQ_EN: 1'b1
    //       [1] MRAM_PWR_SEL_ON: 1'b1
    //       [0] MRAM_PWR_GO    : 1'b1
    // -----------------------------------------------
    mbus_remote_register_write(MRM_ADDR, 0x11, 0x000007);
    WFI();
    *NVIC_ICER = (1 << IRQ_REG0);

    if ((*REG0&0xFF) != 0x94) return 0;

    return 1;
}

uint32_t turn_off_mram (void) {
    // --- Power-On 
    *NVIC_ISER = (1 << IRQ_REG0);
    // REGISTER 0x11
    //       [2] MRAM_PWR_IRQ_EN: 1'b1
    //       [1] MRAM_PWR_SEL_ON: 1'b0
    //       [0] MRAM_PWR_GO    : 1'b1
    // -----------------------------------------------
    mbus_remote_register_write(MRM_ADDR, 0x11, 0x000005);
    WFI();
    *NVIC_ICER = (1 << IRQ_REG0);

    if ((*REG0&0xFF) != 0x98) return 0;

    return 1;
}

void init_presram (uint32_t type) {
    uint32_t i;
    for (i=0; i<BUF_SIZE; i++) {
        if      (type == 0x0) data[i] = 0x0;
        else if (type == 0x1) data[i] = i;
        else if (type == 0x2) data[i] = 0xDEADBEEF;
        else if (type == 0x3) data[i] = 0x0EA7F00D;
        else if (type == 0xF) data[i] = 0xFFFFFFFF;
    }
}

uint32_t send_go (uint32_t cmd, uint32_t length_1, uint32_t expected) {
    *NVIC_ISER = (1 << IRQ_REG0);
    mbus_remote_register_write(MRM_ADDR, 0x09, 0x0
        /* LENGTH (14'h0000) */ | (length_1 << 6)    
        /* IRQ_EN (    1'h0) */ | (0x1 << 5)    
        /* CMD    (    4'h0) */ | (cmd << 1)    
        /* GO     (    1'h0) */ | (0x1 << 0)    
    );
    WFI();
    *NVIC_ICER = (1 << IRQ_REG0);
    uint32_t response = (*REG0&0xFF);
    if (response != expected) return response;
    return 1;
}

uint32_t cp_mram2sram_page (uint32_t page_id, uint32_t num_pages) {
    // Set SRAM Start Address
    mbus_remote_register_write(MRM_ADDR, 0x07, 0x000000);
    // Set MRAM Start Address
    mbus_remote_register_write(MRM_ADDR, 0x08, (page_id << 5)); // 32 words/page
    // GO command
    return send_go (/*cmd*/CMD_CP_MRAM2SRAM, /*length_1*/(num_pages<<5)-1, /*expected*/EXP_CP_MRAM2SRAM);
}

uint32_t cp_sram2mram_page (uint32_t sram_page_id, uint32_t mram_page_id, uint32_t num_pages) {
    // Set SRAM Start Address
    mbus_remote_register_write(MRM_ADDR, 0x07, (sram_page_id << 5));    // 32 words/page
    // Set MRAM Start Address
    mbus_remote_register_write(MRM_ADDR, 0x08, (mram_page_id << 5));    // 32 words/page
    // GO command
    return send_go (/*cmd*/CMD_CP_SRAM2MRAM, /*length_1*/(num_pages<<5)-1, /*expected*/EXP_CP_SRAM2MRAM);
}

void read_sram_page (uint32_t page_id, uint32_t num_pages, uint32_t* dest_addr) {
    // SRAM -> PRE SRAM
    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(
        /*source_prefix  */ MRM_ADDR,
        /*source_mem_addr*/ (uint32_t*) ((page_id << 5) << 2),
        /*dest_prefix    */ 0x1,
        /*dest_mem_addr  */ dest_addr,
        /*length_minus_1 */ (num_pages << 5) - 1
        );
    set_halt_until_mbus_tx();
}

uint32_t read_mram_page (uint32_t page_id, uint32_t num_pages, uint32_t* dest_addr) {

    // MRAM -> SRAM
    if (cp_mram2sram_page(/*page_id*/page_id, /*num_pages*/num_pages)!=1) return 0;

    // SRAM -> PRE SRAM
    read_sram_page (/*page_id*/0, /*num_pages*/num_pages, /*dest_addr*/ dest_addr);

    return 1;
}

uint32_t sram_test (void) {

    uint32_t data_tx = 0xDEADBEEF;
    uint32_t data_rx = 0x00000000;

    mbus_write_message32(0x71, data_rx);

    mbus_copy_mem_from_local_to_remote_bulk(
        /*remote_prefix  */ MRM_ADDR,
        /*remote_mem_addr*/ (uint32_t*) 0x00000000,
        /*local_mem_addr */ &data_tx,
        /*length_minus_1 */ 0
        );

    set_halt_until_mbus_trx();
    mbus_copy_mem_from_remote_to_any_bulk(
        /*source_prefix  */ MRM_ADDR,
        /*source_mem_addr*/ (uint32_t*) 0x00000000,
        /*dest_prefix    */ 0x1,
        /*dest_mem_addr  */ &data_rx,
        /*length_minus_1 */ 0
        );
    set_halt_until_mbus_tx();

    mbus_write_message32(0x72, data_rx);

    return 1;
}


//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

void disable_all_irq_except_timer32_gocep(void) {
    *NVIC_ICPR = 0xFFFFFFFF;
    *NVIC_ICER = ~((1 << IRQ_TIMER32) | (1 << IRQ_GOCEP));
}

//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_gocep (void) {
    // LDO_SELB_I_CTRL_LDO_0P8 = 0x1F
    // LDO_SELB_I_CTRL_LDO_1P8 = 0x1F
    // LDO_SELB_VOUT_LDO_1P8 = *REG0
    // LDO_SELB_VOUT_LDO_BUF = *REG1
    // Reset [13:6]
    uint32_t reg_val = 0xFFDD15 & 0xFFC03F;
    // Calculate the new value
    reg_val = reg_val | (*REG1<<6) | (*REG0<<10);
    // Send MBus Msg
    mbus_remote_register_write(MRM_ADDR, 0x23, reg_val);
}

// TIMER32 - Timeout Check
void handler_ext_int_timer32 (void) {
    disable_all_irq_except_timer32_gocep();
    *TIMER32_STAT = 0x0;
    *TIMER32_GO = 0x0;
}
void handler_ext_int_reg0 (void) {
    disable_all_irq_except_timer32_gocep();
}
//void handler_ext_int_reg1 (void) {
//    disable_all_irq_except_timer32_gocep();
//}
//void handler_ext_int_reg2 (void) {
//    disable_all_irq_except_timer32_gocep();
//}
//void handler_ext_int_reg5 (void) {
//    disable_all_irq_except_timer32_gocep();
//}
//void handler_ext_int_reg7 (void) {
//    disable_all_irq_except_timer32_gocep();
//}
//void handler_ext_int_aes (void) { 
//    disable_all_irq_except_timer32_gocep();
//}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

    // Set PRE Watchdog
    *TIMERWD_GO  = 0;
    *TIMERWD_CNT = 16800000; // ~120s @ 140kHz
    *TIMERWD_GO  = 1;

    // Disable MBus Watchdog
    *REG_MBUS_WD = 0;

    // Enable IRQs for timeout check
    *NVIC_IPR0 = (0x3 << 30); // Lowest Priority for TIMER32 IRQ.
    *NVIC_IPR3 = (0x3 << 30); // Lowest Priority for REG7 IRQ.
    *NVIC_ISER = (0x1 << IRQ_TIMER32) | (0x1 << IRQ_GOCEP);

    // Get the info on who woke up the system, then reset WAKEUP_SOURCE register.
    wakeup_source = *SREG_WAKEUP_SOURCE;
    //*SCTR_REG_CLR_WUP_SOURCE = 1; // OBSOLETE

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    #ifdef DEVEL
        mbus_write_message32(0x70, wakeup_source);
    #endif




    turn_on_ldo();

//    delay(10000);
    
//    turn_on_mram(/*mid*/0);
//    WFI();
//
//    delay(10000);
//    // Tcyc_read = 0
//    mbus_remote_register_write(MRM_ADDR, 0x00, 0);
//    cp_mram2sram_page(/*page_id*/0, /*num_pages*/512);

    //delay(20000);
    //cp_sram2mram_page(/*sram_page_id*/0, /*mram_page_id*/0, /*num_pages*/512);

//    delay(20000);
//    // Tcyc_read = 1000
//    mbus_remote_register_write(MRM_ADDR, 0x00, 1000);
//    cp_mram2sram_page(/*page_id*/0, /*num_pages*/512);
//
//    delay(20000);
//    // Tcyc_read = 5000
//    mbus_remote_register_write(MRM_ADDR, 0x00, 5000);
//    cp_mram2sram_page(/*page_id*/0, /*num_pages*/512);
//
//    delay(20000);
//    // Tcyc_read = 10000
//    mbus_remote_register_write(MRM_ADDR, 0x00, 10000);
//    cp_mram2sram_page(/*page_id*/0, /*num_pages*/512);

    // Copy SRAM -> MRAM
    cp_sram2mram_page(/*sram_page_id*/ 0, /*mram_page_id*/ 0, /*num_pages*/ 256);

    WFI();
    while(1);




    if (!turn_on_ldo()) operation_sleep();

    // Turn on LDO (redundant but won't hurt)
    turn_on_ldo();

    // Measure Clock Freq
    //meas_clk_freq();
    
    // Marker
    mbus_write_message32(0x80, 0);












    // MRAM Read
    init_presram(/*type*/0x1); // data[i] = i
    read_mram_page(/*page_id*/ 0, /*num_pages*/ 1, /*dest_addr*/ (uint32_t *) data);

    // Marker
    mbus_write_message32(0x80, 1);
//
//    // Read SRAM Page 3
//    read_sram_page(/*page_id*/ 3, /*num_pages*/ 1, /*dest_addr*/ (uint32_t *) data);
//    // Copy SRAM Page 3 into MRAM Page 0
//    cp_sram2mram_page(/*sram_page_id*/ 3, /*mram_page_id*/ 0, /*num_pages*/ 1);
//    // Read MRAM Page 0
//    init_presram(/*type*/0x1); // data[i] = i
//    read_mram_page(/*page_id*/ 0, /*num_pages*/ 1, /*dest_addr*/ (uint32_t *) data);
//    
//    // Marker
//    mbus_write_message32(0x80, 2);






    //operation_sleep();

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    // Enable GOCEP for debugging
    disable_all_irq_except_timer32_gocep();
    while(1) WFI();
    return 1;
}


//print("Info: Up-stream SPIFI transaction - write MRAM_R_TEST to enter test mode")
//gpio_spi.mram_wrcfg(nc.MRAM_R_TEST, nc.MRAM_TEST_PW.to_bytes(4, 'big'))

# MRAM_R_4PHASE_WRITE0 = 0
//gpio_spi.mram_wrcfg(nc.MRAM_R_4PHASE_WRITE0, (0).to_bytes(4, 'big'))
//print("Info: Up-stream SPIFI transaction - MRAM_R_4PHASE_WRITE0 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_4PHASE_WRITE0))

# MRAM_R_4PHASE_WRITE1 = 0
//gpio_spi.mram_wrcfg(nc.MRAM_R_4PHASE_WRITE1, (0).to_bytes(4, 'big'))
//print("Info: Up-stream SPIFI transaction - MRAM_R_4PHASE_WRITE1 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_4PHASE_WRITE1))

# MRAM_R_4PHASE_MRG0 = 0000100000 0000000 000 100101 100
//wdata = int('0000100000' + 7*'0' + '000' + '100101' + '100', 2).to_bytes(4, 'big')
//gpio_spi.mram_wrcfg(nc.MRAM_R_4PHASE_MRG0, wdata)
//print("Info: Up-stream SPIFI transaction - MRAM_R_4PHASE_MRG0 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_4PHASE_MRG0))

# MRAM_R_4PHASE_MRG1 = 0000011000 0000000 000 000000 000
//wdata = int('0000011000' + 7*'0' + '000' + '000000' + '000', 2).to_bytes(4, 'big')
//gpio_spi.mram_wrcfg(nc.MRAM_R_4PHASE_MRG1, wdata)
//print("Info: Up-stream SPIFI transaction - MRAM_R_4PHASE_MRG1 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_4PHASE_MRG1))

# MRAM_R_CFG0 = A0 54 21 08
//wdata = b'\xa0\x54\x21\x08'
//gpio_spi.mram_wrcfg(nc.MRAM_R_CFG0, wdata)
//print("Info: Up-stream SPIFI transaction - MRAM_R_CFG0 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_CFG0))

# MRAM_R_CFG1 = 00 2A 1E 2E
//wdata = b'\x00\x2a\x1e\x2e'
//gpio_spi.mram_wrcfg(nc.MRAM_R_CFG1, wdata)
//print("Info: Up-stream SPIFI transaction - MRAM_R_CFG1 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_CFG1))

# MRAM_R_CFG5 = 0C 00 8D D3
//wdata = b'\x0c\x00\x8d\xd3'
//gpio_spi.mram_wrcfg(nc.MRAM_R_CFG5, wdata)
//print("Info: Up-stream SPIFI transaction - MRAM_R_CFG5 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_CFG5))

//print("Info: Up-stream SPIFI transaction - write MRAM_R_TEST to exit test mode")
//gpio_spi.mram_wrcfg(nc.MRAM_R_TEST, (0).to_bytes(4, 'big'))

