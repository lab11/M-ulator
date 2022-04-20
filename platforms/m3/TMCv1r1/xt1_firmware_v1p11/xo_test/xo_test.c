//*******************************************************************************************
// XT1 (TMCv1r1) XO Test
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

// At RT
//      Started (550bps)
//      GOC Trigger (200bps) to enable the pad out
// At -40C
//      XO is still running
//      100bps GOC does not work
//      80bps GOC does not work
//      LabVIEW cannot generate 70bps GOC...

//*******************************************************************************************
// HEADER FILES
//*******************************************************************************************

#include "TMCv1r1.h"

//*******************************************************************************************
// DEVEL Mode
//*******************************************************************************************
// Enable 'DEVEL' for the following features:
//      - Send debug messages
//      - Use default values rather than grabbing the values from EEPROM
#define DEVEL

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED     0
#define FLAG_INITIALIZED    1
#define FLAG_XO_INITIALIZED 2

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// PRE Clock Generator Frequency
#define CPU_CLK_FREQ    140000  // XT1F#1, SAR_RATIO=0x60 (Sleep Voltages: 4.55V/1.59V/0.79V)

// PRE Clock-based Delay (5 instructions @ CPU_CLK_FREQ)
#define DLY_1S      ((CPU_CLK_FREQ>>3)+(CPU_CLK_FREQ>>4)+(CPU_CLK_FREQ>>5))  // = CPU_CLK_FREQx(1/8 + 1/16 + 1/32) = CPU_CLK_FREQx(35/160) ~= (CPU_CLK_FREQ/5)
#define DLY_1MS     DLY_1S >> 10                                             // = DLY_1S / 1024 ~= DLY_1S / 1000

// XO Frequency
#define XO_FREQ_SEL 0                       // XO Frequency (in kHz) = 2 ^ XO_FREQ_SEL
#define XO_FREQ     1<<(XO_FREQ_SEL+10)     // XO Frequency (in Hz)

// XO Initialization Wait Duration
#define XO_WAIT_A   2*DLY_1S    // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S    // Must be ~1 second delay.


//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************

//-------------------------------------------------------------------------------------------
// Other Global Variables
//-------------------------------------------------------------------------------------------

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep Functions
static void operation_sleep (uint32_t lp_act);
static void operation_init (void);

//*******************************************************************************************
// FUNCTIONS IMPLEMENTATION
//*******************************************************************************************

//-------------------------------------------------------------------
// Initialization/Sleep Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: operation_sleep
// Args    : lp_act  - Perform the low-power active mode 
//                     before sending the sleep message
// Description:
//           Sends the MBus Sleep message
//           If check_snt=1, it first checks the SNT counter value.
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep (uint32_t lp_act) {

    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // FIXME: Stop any ongoing EID operation (but how...?)

    // Power off NFC
    nfc_power_off();

    // Clear all pending IRQs; otherwise, PREv22E replaces the sleep msg with a selective wakeup msg
    *NVIC_ICER = 0xFFFFFFFF;

    // Perform the low-power active mode if needed
    if (lp_act) {
        // Low-Power Active mode: 18~20uA for ~6 seconds
        *REG_MBUS_WD = 1680000; // 6s with 280kHz;
        halt_cpu();
    }
    // Go to sleep
    else {
        mbus_sleep_all(); 
        while(1);
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
        *REG_SYS_CONF =       (0x0 << 8)    // 1'h0     ENABLE_SOFT_RESET	#If 1, Soft Reset will occur when there is an MBus Memory Bulk Write message received and the MEM Start Address is 0x2000
                            | (0x1 << 7)    // 1'h1     PUF_CHIP_ID_SLEEP
                            | (0x1 << 6)    // 1'h1     PUF_CHIP_ID_ISOLATE
                            | (0x8 << 0);   // 5'h1E    WAKEUP_ON_PEND_REQ	#[4]: GIT (PRE_E Only), [3]: GPIO (PRE only), [2]: XO TIMER (PRE only), [1]: WUP TIMER, [0]: GOC/EP

        //-------------------------------------------------
        // Enumeration
        //-------------------------------------------------
        set_halt_until_mbus_trx();
        mbus_enumerate(SNT_ADDR);
        mbus_enumerate(EID_ADDR);
        mbus_enumerate(MRR_ADDR);
        mbus_enumerate(MEM_ADDR);
        mbus_enumerate(PMU_ADDR);
        set_halt_until_mbus_tx();

        //-------------------------------------------------
        // Target Register Index
        //-------------------------------------------------
        mbus_remote_register_write(PMU_ADDR, 0x52, (0x10 << 8) | PMU_TARGET_REG_IDX);
        mbus_remote_register_write(SNT_ADDR, 0x07, (0x10 << 8) | SNT_TARGET_REG_IDX);
        mbus_remote_register_write(EID_ADDR, 0x05, (0x1 << 16) | (0x10 << 8) | EID_TARGET_REG_IDX);
        mbus_remote_register_write(MRR_ADDR, 0x1E, (0x10 << 8) | MRR_TARGET_REG_IDX); // FSM_IRQ_REPLY_PACKET
        mbus_remote_register_write(MRR_ADDR, 0x23, (0x10 << 8) | MRR_TARGET_REG_IDX); // TRX_IRQ_REPLY_PACKET

        //-------------------------------------------------
        // PMU Settings
        //-------------------------------------------------
        pmu_init();

        //-------------------------------------------------
        // SNT Settings
        //-------------------------------------------------
        snt_init();

        // Update the flag
        set_flag(FLAG_ENUMERATED, 1);

        // Turn on the SNT timer clock
        snt_start_timer(/*wait_time*/2*DLY_1S);

        // Start the SNT counter
        snt_enable_wup_timer(/*auto_reset*/0);

        //-------------------------------------------------
        // XO Settings
        //-------------------------------------------------
        //--- XO Frequency
        *REG_XO_CONF2 =             // Default  // Description
            //-----------------------------------------------------------------------------------------------------------
            ( (0x2          << 13)  // 2'h2     // XO_INJ	            #Adjusts injection period
            | (0x1          << 10)  // 3'h1     // XO_SEL_DLY	        #Adjusts pulse delay
            | (0x1          <<  8)  // 2'h1     // XO_SEL_CLK_SCN	    #Selects division ratio for SCN CLK
            | (XO_FREQ_SEL  <<  5)  // 3'h1     // XO_SEL_CLK_OUT_DIV   #Selects division ratio for the XO CLK output; freq = (2^XO_SEL_CLK_OUT_DIV) in kHz.
            | (0x1          <<  4)  // 1'h1     // XO_SEL_LDO	        #Selects LDO output as an input to SCN
            | (0x0          <<  3)  // 1'h0     // XO_SEL_0P6	        #Selects V0P6 as an input to SCN
            | (0x0          <<  0)  // 3'h0     // XO_I_AMP	            #Adjusts VREF body bias buffer current
            );

        //--- Start XO clock
        xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B, /*start_cnt*/0);
        // Update the flag
        set_flag(FLAG_XO_INITIALIZED, 1);

        //--- Configure and Start the XO Counter
        set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0);
        start_xo_cnt();
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

        //-------------------------------------------------
        // NFC 
        //-------------------------------------------------
        nfc_init();

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);

    }
}


//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

//void handler_ext_int_wakeup   (void) { *NVIC_ICPR = (0x1 << IRQ_WAKEUP);     }
//void handler_ext_int_softreset(void) { *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); }
void handler_ext_int_gocep    (void) { 
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);      
    uint32_t goc_raw = *GOC_DATA_IRQ;
    *GOC_DATA_IRQ   = 0;

    uint32_t goc_lp_act = (goc_raw>>28)&0xF;
    uint32_t goc_header = (goc_raw>>24)&0xF;
    uint32_t goc_data   = goc_raw&0xFFFFFF;

    // if goc_lp_act==1, it performs the low power active mode before going into sleep

    // 0xn0abcdef
    //  -> Stay active for delay(abcdef)
    if (goc_header==0x0) {
        delay(goc_data);
    }
    else if (goc_header==0x1) {
    // 0xn1000000
    //  -> Restart the XO using the default delay setting (delay_a=delay_b=delay(56000))
    //     Pad out disabled
        if (goc_data==0) {
            restart_xo(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);
        }
    // 0xn1abcdef
    //  -> Restart the XO using the given delay setting (delay_a=delay_b=delay(abcdef))
    //     Pad out disabled
        else {
            restart_xo(/*delay_a*/goc_data, /*delay_b*/goc_data);
        }
        stop_xo_cout();
    }
    else if (goc_header==0x2) {
    // 0xn2000000
    //  -> Restart the XO using the default delay setting (delay_a=delay_b=delay(56000))
    //     Pad out enabled
        if (goc_data==0) {
            restart_xo(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);
        }
    // 0xn2abcdef
    //  -> Restart the XO using the given delay setting (delay_a=delay_b=delay(abcdef))
    //     Pad out enabled
        else {
            restart_xo(/*delay_a*/goc_data, /*delay_b*/goc_data);
        }
        start_xo_cout();
    }
    else if (goc_header==0x3) {
    // 0xn3000000
    //  -> Disable pad out
        if (goc_data==0x000000) {
            stop_xo_cout();
        }
    // 0xn3000001
    //  -> Enable pad out
        else if (goc_data==0x000001) {
            start_xo_cout();
        }
    }
    else if (goc_header==0x4) {
    // 0xn4abcdef
    //  -> Write *(GOC_DATA_IRQ+1) to EEPROM[abcdef]
        nfc_i2c_byte_write(/*e2*/0, /*addr*/goc_data, /*data*/*(GOC_DATA_IRQ+1), /*nb*/4);
    }
    else if (goc_header==0x5) {
    // 0xn5abcdef
    //  -> Read from EEPROM[abcdef]
        nfc_i2c_byte_read(/*e2*/0, /*addr*/goc_data, /*nb*/4);
    }
    else if (goc_header==0x6) {
    // 0xn6000000
    //  -> Write *(GOC_DATA_IRQ+1) to EEPROM[goc_data] ~ EEPROM[goc_data+256]
        nfc_i2c_word_pattern_write(/*e2*/0, /*addr*/goc_data, /*data*/*(GOC_DATA_IRQ+1), /*nw*/64);
    }
    // 0xn7abcdef
    //  -> Toggle gpio pin(s) whose index is the bit pattern of "a" by "bcdef" times.
    else if (goc_header==0x7) {
        uint32_t i;
        uint32_t gpio = (goc_data>>20)&0xF;
        uint32_t num = goc_data&0xFFFFF;
        unfreeze_gpio_out();
        *GPIO_DIR = 0xF; 
        for (i=0; i<num; i++) { 
            *GPIO_DATA = 0x0; 
            *GPIO_DATA = gpio;
        }
    }

    // Go to sleep either with lp_act or not.
    freeze_gpio_out();
    if (goc_lp_act) {
        operation_sleep(/*lp_act*/1);
    }
    else {
        operation_sleep(/*lp_act*/0);
    }
}

//void handler_ext_int_timer32  (void) {
//    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
//    *REG1 = *TIMER32_CNT;
//    *REG2 = *TIMER32_STAT;
//    *TIMER32_STAT = 0x0;
//    __wfi_timeout_flag__ = 1; // Declared in PREv22E.h
//    set_halt_until_mbus_tx();
//    }
//void handler_ext_int_timer16  (void) { *NVIC_ICPR = (0x1 << IRQ_TIMER16);    }
//void handler_ext_int_mbustx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);    }
//void handler_ext_int_mbusrx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);    }
//void handler_ext_int_mbusfwd  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);   }
//void handler_ext_int_reg0     (void) { *NVIC_ICPR = (0x1 << IRQ_REG0);       }
//void handler_ext_int_reg1     (void) { 
//    // Used in snt_operation()
//    *NVIC_ICPR = (0x1 << IRQ_REG1);       
//}
//void handler_ext_int_reg2     (void) { *NVIC_ICPR = (0x1 << IRQ_REG2);       }
//void handler_ext_int_reg3     (void) { *NVIC_ICPR = (0x1 << IRQ_REG3);       }
//void handler_ext_int_reg4     (void) { *NVIC_ICPR = (0x1 << IRQ_REG4);       }
//void handler_ext_int_reg5     (void) { *NVIC_ICPR = (0x1 << IRQ_REG5);       }
//void handler_ext_int_reg6     (void) { *NVIC_ICPR = (0x1 << IRQ_REG6);       }
//void handler_ext_int_reg7     (void) { *NVIC_ICPR = (0x1 << IRQ_REG7);       }
//void handler_ext_int_mbusmem  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);   }
//void handler_ext_int_aes      (void) { *NVIC_ICPR = (0x1 << IRQ_AES);        }
//void handler_ext_int_gpio     (void) { *NVIC_ICPR = (0x1 << IRQ_GPIO);       }
//void handler_ext_int_spi      (void) { *NVIC_ICPR = (0x1 << IRQ_SPI);        }
//void handler_ext_int_xot      (void) { *NVIC_ICPR = (0x1 << IRQ_XOT);        }


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_GOCEP);

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    delay(10000);

    snt_set_wup_timer(/*auto_reset*/1, /*threshold*/15000);
    operation_sleep(/*lp_act*/0);

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
