//*******************************************************************************************
// E-InkTesting for XT1r1
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Apr 08 2022 - First commit
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

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

//*******************************************************************************************
// EID LAYER CONFIGURATION
//*******************************************************************************************

// Segment ID : See 'External Connections' for details
#define SEG_PLAY        0
#define SEG_TICK        1
#define SEG_LOWBATT     2
#define SEG_BACKSLASH   3
#define SEG_SLASH       4
#define SEG_PLUS        5
#define SEG_MINUS       6

// Display Patterns: See 'External Connections' for details
#define DISP_PLAY       (0x1 << SEG_PLAY     )
#define DISP_TICK       (0x1 << SEG_TICK     )
#define DISP_LOWBATT    (0x1 << SEG_LOWBATT  )
#define DISP_BACKSLASH  (0x1 << SEG_BACKSLASH)
#define DISP_SLASH      (0x1 << SEG_SLASH    )
#define DISP_PLUS       (0x1 << SEG_PLUS     )
#define DISP_MINUS      (0x1 << SEG_MINUS    )

// Display Presets
#define DISP_NONE           (0)
#define DISP_CHECK          (DISP_TICK  | DISP_SLASH)
#define DISP_CROSS          (DISP_SLASH | DISP_BACKSLASH)
#define DISP_ALL            (0x7F)


//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************
uint32_t eeprom_addr;
uint32_t wakeup_source;

//-------------------------------------------------------------------------------------------
// Other Global Variables
//-------------------------------------------------------------------------------------------

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep Functions
static void operation_sleep (void);
static void operation_init (void);

//*******************************************************************************************
// FUNCTIONS IMPLEMENTATION
//*******************************************************************************************

//-------------------------------------------------------------------
// Initialization/Sleep Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: operation_sleep
// Args    : None
// Description:
//           Sends the MBus Sleep message
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep (void) {

    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // FIXME: Stop any ongoing EID operation (but how...?)

    // Power off NFC
    nfc_power_off();

    // Clear all pending IRQs; otherwise, PREv22E replaces the sleep msg with a selective wakeup msg
    *NVIC_ICER = 0xFFFFFFFF;

    // Go to sleep
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

        // Start the SNT counter and initialize snt_threshold
        snt_enable_wup_timer(/*auto_reset*/0);

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

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
	    eid_init(/*ring*/0, /*te_div*/3, /*fd_div*/3, /*seg_div*/3);

        // Set the duration
        eid_set_duration(500);  // 500 is the default duration for *_mid (=-5 ~ 15C) and *_low (< -15C) temparatures.

        eid_r00.TMR_EN_CLK_OUT = 1;
        mbus_remote_register_write(EID_ADDR,0x00,eid_r00.as_int);
        // EID Clock Frequency: 132.8 Hz
        // clk_tmr_bat: 132.8 Hz -> used in WD_CTRL
        // clk_tmr_raw_bat: 2124.8 Hz

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);

    }
}

//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
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
//void handler_ext_int_gocep    (void) { *NVIC_ICPR = (0x1 << IRQ_GOCEP);      }
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

static void global_update(uint32_t seg) { 
    // Make all black segments white
    if (__eid_current_display__!=0) eid_enable_cp_ck(0x1, 0x1, (~__eid_current_display__ & 0x1FF), 0);
    // Make all segments/field black
    eid_enable_cp_ck(0x0, 0x1, 0x1FF, 0);
    // Make selected segments white
    eid_enable_cp_ck(0x1, 0x0, seg & 0x1FF, 0);
}

static void simple_update(uint32_t seg) { 
    // Make all segments/field white
    eid_enable_cp_ck(0x1, 0x0, 0x0, 0);
    // Make selected segments black
    eid_enable_cp_ck(0x0, 0x0, seg&0x1FF, 0);
}

static void local_wb_update(uint32_t seg) { 
    // Segments to become white
    uint32_t seg_b2w = __eid_current_display__ & (~seg);
    // Segments to become black
    uint32_t seg_w2b = (~__eid_current_display__) & seg;
    // Perform Black to White
    if (seg_b2w != 0) eid_enable_cp_ck(0x1, 0x1, (~seg_b2w & 0x1FF), 0);
    // Perform White to Black
    if (seg_w2b != 0) eid_enable_cp_ck(0x0, 0x0, seg_w2b & 0x1FF, 0);
    // Need to explicitly set __eid_current_display__
    __eid_current_display__ = seg;
}

static void local_bw_update(uint32_t seg) { 
    // Segments to become white
    uint32_t seg_b2w = __eid_current_display__ & (~seg);
    // Segments to become black
    uint32_t seg_w2b = (~__eid_current_display__) & seg;
    // Perform White to Black
    if (seg_w2b != 0) eid_enable_cp_ck(0x0, 0x0, seg_w2b & 0x1FF, 0);
    // Perform Black to White
    if (seg_b2w != 0) eid_enable_cp_ck(0x1, 0x1, (~seg_b2w & 0x1FF), 0);
    // Need to explicitly set __eid_current_display__
    __eid_current_display__ = seg;
}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;

    // Get the wakeup source
    wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1;

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();
    else {

        // If waken up by GOC/EP
        uint32_t goc_raw = *GOC_DATA_IRQ;
        if (goc_raw!=0) {
            *GOC_DATA_IRQ   = 0;

            //uint32_t goc_header = (goc_raw>>24)&0xFF;
            //uint32_t goc_data   = goc_raw&0xFFFFFF;

        }
        // E-Ink Operation 
        else {
            delay(2*DLY_1S);
            global_update(DISP_ALL);
            delay(2*DLY_1S);
            simple_update(DISP_CHECK);
            delay(2*DLY_1S);
            local_wb_update(DISP_CROSS);
            delay(2*DLY_1S);
            local_bw_update(DISP_CHECK);
            delay(2*DLY_1S);
        }

    }
    

    snt_set_wup_timer(/*auto_reset*/1, /*threshold*/30*1500);
    operation_sleep();

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
