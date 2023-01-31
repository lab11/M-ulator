//*******************************************************************************************
// Temperature Conversion Testing
//------------------------

#include "mbus.h"
#include "PREv23E.h"
#include "TMCv2.h"

//#define COEFF_A 0x00007E8B
//#define COEFF_B 0x005479B9

#define FLAG_ENUMERATED         0

// PRE Clock Generator Frequency
#define CPU_CLK_FREQ    140000  // XT1F#1, SAR_RATIO=0x60 (Sleep Voltages: 4.55V/1.59V/0.79V)

// PRE Clock-based Delay (5 instructions @ CPU_CLK_FREQ)
#define DLY_1S      ((CPU_CLK_FREQ>>3)+(CPU_CLK_FREQ>>4)+(CPU_CLK_FREQ>>5))  // = CPU_CLK_FREQx(1/8 + 1/16 + 1/32) = CPU_CLK_FREQx(35/160) ~= (CPU_CLK_FREQ/5)

// XO Frequency
#define XO_FREQ_SEL 0                       // XO Frequency (in kHz) = 2 ^ XO_FREQ_SEL
#define XO_FREQ     1<<(XO_FREQ_SEL+10)     // XO Frequency (in Hz)

#define XO_WAIT_A   2*DLY_1S    // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S    // Must be ~1 second delay.

volatile uint32_t snt_threshold;

static void operation_init(void) {

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

        // Update the flag
        set_flag(FLAG_ENUMERATED, 1);


//        //-------------------------------------------------
//        // SNT Settings
//        //-------------------------------------------------
//        snt_init();
//
//        // Turn on the SNT timer clock
//        snt_start_timer(/*wait_time*/2*DLY_1S);
//
//        // Start the SNT counter
//        snt_enable_wup_timer(/*auto_reset*/0);
//
//        snt_threshold = 0;

        //-------------------------------------------------
        // XO Setting
        //-------------------------------------------------
        // (Reg 0x1A) 0xA0000068
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
        set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/1, /*irq_en*/0, /*auto_reset*/0);
        start_xo_cnt();

        start_xo_cout();
    }


}

static void operation_sleep (void) {
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // Clear all pending IRQs; otherwise, PREv22E replaces the sleep msg with a selective wakeup msg
    //*NVIC_ICER = 0xFFFFFFFF;

    // Go to sleep immediately
    mbus_sleep_all(); 
    while(1);
}

void handler_ext_int_wfi (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_wfi (void) {
    delay(10);
}

int main(void) {

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;



    //uint32_t idx;
    //for (idx=0; idx<1024; idx++) {
    //    mbus_write_message32((0xB<<4)|(idx&0xF), (tconv_log2_table(idx)<<16) | tconv_log2_taylor(idx));
    //}

    //uint32_t a = 0x2;
    //uint32_t b = 0xFFFFFFFE;

    //mbus_write_message32(0xA0, a - b);
    //mbus_write_message32(0xA0, b - a);
    //mbus_write_message32(0xA0, ((a-b)<5) ? 1 : 0);
    




//    if (!get_flag(FLAG_ENUMERATED)) operation_init();
//
//    snt_threshold += 7500;
//
//    snt_set_wup_timer(/*auto_reset*/0, /*threshold*/snt_threshold);
//
//    delay(1000);
//
//    operation_sleep();




    if (!get_flag(FLAG_ENUMERATED)) operation_init();

    *XOT_SAT = *XOT_VAL + (1024<<3);
    mbus_write_message32(0xA0, *XOT_SAT);

    mbus_sleep_all(); 
    while(1);





    return 1;
}
