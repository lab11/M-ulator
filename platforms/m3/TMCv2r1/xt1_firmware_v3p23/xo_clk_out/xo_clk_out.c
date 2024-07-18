//*******************************************************************************************
// XT1 (TMCv2r1) XO CLOCK OUT
//******************************************************************************************* 

//*******************************************************************************************
// HEADER FILES
//*******************************************************************************************

#include "TMCv2r1.h"

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1
#define FLAG_XO_INITIALIZED     2

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
void disable_all_irq_except_timer32(void);
static uint32_t get_xo_cnt(void);

void disable_all_irq_except_timer32(void) {
    *NVIC_ICPR = 0xFFFFFFFF;
    *NVIC_ICER = ~(1 << IRQ_TIMER32);
}

//-------------------------------------------------------------------
// Function: get_xo_cnt
// Args    : None
// Description:
//              Keep doing the asynchronous read.
//              Return the last value if two consecutive values are the same.
// Return  : XO counter value if no error
//           0 if #tries exceeds 5
//-------------------------------------------------------------------
static uint32_t get_xo_cnt(void) {
    //----------------------------------------
    // GETTING THE SAME VALUES TWICE CONSECUTIVELY
    //----------------------------------------

    uint32_t temp = *XOT_VAL_ASYNC;
    uint32_t temp_prev;
    uint32_t num_same_reads = 1;
    uint32_t num_try = 0;

    while ((num_same_reads < 2) && (num_try<5)) {
        temp_prev = temp;
        temp = *XOT_VAL_ASYNC;
        if (temp==temp_prev) num_same_reads++;
        else num_same_reads=1;
        num_try++;
    }

    if (num_try >= 5) {
        return 0;
    }
    else {
        if (temp==0) temp=1;    // Avoid returning 0, as 0 indicates an error.
        return temp;
    }
}

//-------------------------------------------------------------------
// Function: operation_init
// Args    : None
// Description:
//           Initializes the system
// Return  : None
//-------------------------------------------------------------------
static void operation_init (void) {

    if (!get_flag(FLAG_ENUMERATED)) {

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
        // Target Register Index
        //-------------------------------------------------
        mbus_remote_register_write(PMU_ADDR, 0x52, (0x10 << 8) | PMU_TARGET_REG_IDX);
        mbus_remote_register_write(SNT_ADDR, 0x07, (0x10 << 8) | SNT_TARGET_REG_IDX);
        mbus_remote_register_write(EID_ADDR, 0x05, (0x1 << 16) | (0x10 << 8) | EID_TARGET_REG_IDX);

        //-------------------------------------------------
        // PMU Settings
        //-------------------------------------------------
        pmu_init();

        // Update) the flag
        set_flag(FLAG_ENUMERATED, 1);

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

        //-------------------------------------------------
        // Go to Sleep
        //-------------------------------------------------
        // Go to sleep
        set_wakeup_timer (/*timestamp*/10, /*irq_en*/0x1, /*reset*/0x1); // About 2 seconds
        mbus_sleep_all();
        while(1);
    }
    else if (!get_flag(FLAG_INITIALIZED)) {

        // Set the Active floor setting to the minimum (b/c RAT is enabled at this point)
        pmu_set_active_min();

        // Disable the PRC wakeup timer
        *REG_WUPT_CONFIG = *REG_WUPT_CONFIG & 0xFF3FFFFF; // WUP_ENABLE=0, WUP_WREQ_EN=0

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);
    }

}

//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_wfi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

// TIMER32 - Timeout Check
void handler_ext_int_timer32 (void) {
    disable_all_irq_except_timer32();
    *TIMER32_STAT = 0x0;
    *TIMER32_GO = 0x0;
//    fail_handler(/*id*/__wfi_id__);
    // If __wfi_id__=FAIL_ID_SOFT, fail_handler() immediately returns here. 
    // Change it to FAIL_ID_PEND to notify the caller.
    __wfi_id__ = FAIL_ID_PEND;
}
void handler_ext_int_wfi (void) {
    // REG0: PMU
    // REG1: SNT (Temperature Sensor)
    // REG2: EID Display Update
    // REG5: SNT (Wakeup Timer)
    // AES : AES
    disable_all_irq_except_timer32();
    *TIMER32_GO = 0;
//    __wfi_id__ = FAIL_ID_NONE;
}
//// PMU (PMU_TARGET_REG_IDX)
//void handler_ext_int_reg0 (void) {
//    disable_all_irq_except_timer32();
//}
//// SNT (Temperature Sensor) (SNT_TARGET_REG_IDX)
//void handler_ext_int_reg1 (void) {
//    disable_all_irq_except_timer32();
//}
////  EID (EID_TARGET_REG_IDX)
//void handler_ext_int_reg2 (void) {
//    disable_all_irq_except_timer32();
//}
//// SNT (Timer Access) (WP1_TARGET_REG_IDX)
//void handler_ext_int_reg5 (void) {
//    disable_all_irq_except_timer32();
//}
// I2C ACK Failure Handling (I2C_TARGET_REG_IDX)
void handler_ext_int_reg7 (void) {
    disable_all_irq_except_timer32();
    *TIMER32_GO = 0;
    __wfi_id__ = FAIL_ID_I2C;
//    fail_handler(/*id*/__wfi_id__);
}
//// AES
//void handler_ext_int_aes (void) { 
//    disable_all_irq_except_timer32();
//}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

    uint32_t xo_prev;
    uint32_t xo_curr;
    uint32_t xo_restart;

    // Disable PRE Watchdog
    *TIMERWD_GO  = 0;

    // Disable MBus Watchdog
    *REG_MBUS_WD = 0;

    // Disable the PRC wakeup timer
    *REG_WUPT_CONFIG = *REG_WUPT_CONFIG & 0xFF3FFFFF; // WUP_ENABLE=0, WUP_WREQ_EN=0

    operation_init();

    //--------------------------------------------------------------------------
    // Check XO clock running
    //--------------------------------------------------------------------------
    xo_prev = get_xo_cnt();
    while(1) {
        xo_restart = 0;
        delay(DLY_1S);  // NOTE: XO normal freq is 1kHz. 

        // Check the counter
        xo_curr = get_xo_cnt();
        if (xo_curr > xo_prev) {
            if ((xo_curr - xo_prev) < 100) xo_restart = 1;
        } else {
            if ((xo_prev - xo_curr) < 100) xo_restart = 1;
        }

        // Restart the XO if needed
        if (xo_restart) restart_xo(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

        // Update xo_prev
        xo_prev = get_xo_cnt();
    }

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
