//*******************************************************************************************
// XT1 (TMCv2r1) XO CLOCK OUT
//******************************************************************************************* 

//*******************************************************************************************
// HEADER FILES
//*******************************************************************************************

#include "TMCv2r1.h"

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// XO Frequency
#define XO_FREQ_SEL 0                       // XO Frequency (in kHz) = 2 ^ XO_FREQ_SEL
#define XO_FREQ     1<<(XO_FREQ_SEL+10)     // XO Frequency (in Hz)

// XO Initialization Wait Duration
#define XO_WAIT_A   2*DLY_1S    // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S    // Must be ~1 second delay.

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep/IRQ Handling
static void operation_init(void);

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
                        | (0x3 << 18)   // 3'h3     CLK_GEN_DIV_CORE	#Default CPU Freq = 96kHz (pre-pex simulation, TT, 25C)
                        | (0x2 << 15)   // 3'h2     CLK_GEN_DIV_MBC	    #Default MBus Freq = 193kHz (pre-pex simulation, TT, 25C)
                        | (0x1 << 13)   // 2'h1     CLK_GEN_RING
                        | (0x0 << 12)   // 1'h0     RESERVED
                        | (0x0 << 10)   // 2'h1     GOC_CLK_GEN_SEL_DIV
                        | (0x5 <<  7)   // 3'h7     GOC_CLK_GEN_SEL_FREQ
                        | (0x0 <<  6)   // 1'h0     GOC_ONECLK_MODE
                        | (0x0 <<  4)   // 2'h0     GOC_SEL_DLY
                        | (0xF <<  0);  // 4'h8     GOC_SEL

    //--- Pending Wakeup Handling          Default
    *REG_SYS_CONF =       (0x0 << 9)    // 1'h1     NO_SLEEP_WITH_PEND_IRQ  #If 1, it replaces the sleep message with a selective wakeup message if there is a pending M0 IRQ. If 0, it goes to sleep even when there is a pending M0 IRQ.
                        | (0x0 << 8)    // 1'h0     ENABLE_SOFT_RESET	#If 1, Soft Reset will occur when there is an MBus Memory Bulk Write message received and the MEM Start Address is 0x2000
                        | (0x1 << 7)    // 1'h1     PUF_CHIP_ID_SLEEP
                        | (0x1 << 6)    // 1'h1     PUF_CHIP_ID_ISOLATE
                        | (0x8 << 0);   // 5'h1E    WAKEUP_ON_PEND_REQ	#[4]: GIT (PRE_E Only), [3]: GPIO (PRE only), [2]: XO TIMER (PRE only), [1]: WUP TIMER, [0]: GOC/EP

    //-------------------------------------------------
    // Enumeration
    //-------------------------------------------------
    set_halt_until_mbus_trx();
    mbus_enumerate(EID_ADDR);
    mbus_enumerate(SNT_ADDR);
    mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();

    //-------------------------------------------------
    // XO Settings
    //-------------------------------------------------
    //--- XO Frequency
    *REG_XO_CONF2 =             // Default  // Description
        //-----------------------------------------------------------------------------------------------------------
        ( (0x1         << 20)   // (3'h1) XO_SEL_DLY	        #Adjusts pulse delay
        | (0x1         << 18)   // (2'h1) XO_SEL_CLK_SCN	    #Selects division ratio for SCN CLK
        | (XO_FREQ_SEL << 15)   // (3'h1) XO_SEL_CLK_OUT_DIV	#Selects division ratio for the XO CLK output. XO Freq = 2^XO_SEL_CLK_OUT_DIV (kHz)
        | (0x1         << 14)   // (1'h1) XO_SEL_LDO            #Selects LDO output as an input to SCN
        | (0x0         << 13)   // (1'h0) XO_SEL_0P6            #Selects V0P6 as an input to SCN
        | (0x0         << 10)   // (3'h0) XO_I_AMP	            #Adjusts VREF body bias buffer current
        | (0x0         <<  3)   // (7'h0) XO_VREF_TUNEB 	    #Adjust VREF level and TC
        | (0x0         <<  0)   // (3'h0) XO_SEL_VREF 	        #Selects VREF output from its diode stack
        );

    xo_stop();  // Default value of XO_START_UP is wrong in RegFile, so need to override it.

    //--- Start XO clock
    xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

    //--- Configure and Start the XO Counter
    set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0, /*auto_reset*/0);
    start_xo_cnt();

    // Enable XO pad
    start_xo_cout();

}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

    // Disable PRE Watchdog
    *TIMERWD_GO  = 0;

    // Disable MBus Watchdog
    *REG_MBUS_WD = 0;

    // Disable the PRC wakeup timer
    *REG_WUPT_CONFIG = *REG_WUPT_CONFIG & 0xFF3FFFFF; // WUP_ENABLE=0, WUP_WREQ_EN=0

    operation_init();

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
