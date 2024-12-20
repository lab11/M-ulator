//*******************************************************************************************
// PMU Counter Testing for XT1r1
//-------------------------------------------------------------------------------------------
// NOTE: This file is identical to the one in xt1_firmware_v1p16
//-------------------------------------------------------------------------------------------
//
//  --------------------------
//   Measured Sleep Duration
//  --------------------------
//   0x4000 (16384): 672s
//   0x3000 (12288): 446s
//   0x2000 ( 8192): 224s
//   0x1800 ( 6144): 112s
//   0x1600 ( 5632):  83.1s
//   0x1400 ( 5120):  55.2s
//   0x1200 ( 4608):  27.2s
//   0x1000 ( 4096):   0.106s
//  --------------------------
// 
//  [ Equation ]
//
//  For pmu_threshold > 4096
//
//  sleep_duration = 0.054688*(pmu_threshold - 4096)
//
//  pmu_threshold = 18.28571*sleep_duration + 4096
//
//      NOTE: sleep_duration is in sec
//
//  By default, pmu_threshold is set to 0x10B7, which would result in ~10sec sleep duration
//
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jul 26 2022 - First commit
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*******************************************************************************************
// HEADER FILES
//*******************************************************************************************

#include "TMCv1r1.h"

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED     0
#define FLAG_INITIALIZED    1
#define FLAG_XO_INITIALIZED 2

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// Sleep Duration in seconds
#define SLEEP_DURATION  10  

// PRE Clock Generator Frequency
#define CPU_CLK_FREQ    140000  // XT1F#1, SAR_RATIO=0x60 (Sleep Voltages: 4.55V/1.59V/0.79V)

// PRE Clock-based Delay (5 instructions @ CPU_CLK_FREQ)
#define DLY_1S      ((CPU_CLK_FREQ>>3)+(CPU_CLK_FREQ>>4)+(CPU_CLK_FREQ>>5))  // = CPU_CLK_FREQx(1/8 + 1/16 + 1/32) = CPU_CLK_FREQx(35/160) ~= (CPU_CLK_FREQ/5)
#define DLY_1MS     DLY_1S >> 10                                             // = DLY_1S / 1024 ~= DLY_1S / 1000


//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************
uint32_t eeprom_addr;
uint32_t wakeup_source;
uint32_t pmu_threshold;
uint32_t snt_prev;
uint32_t snt_curr;
uint32_t adc_offset;
uint32_t pmu_sel_margin;
uint32_t hysteresis;

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
static uint32_t calc_new_sar_ratio(uint32_t adc_val, uint32_t offset, uint32_t num_cons_meas, uint32_t hyst);

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

        // Initialize SAR Ratio
        pmu_set_sar_ratio(0x48); // See calc_new_sar_ratio()

        //pmu_threshold = 0x0010B7;    // 22-bit ~10 seconds
        pmu_threshold = 0x001725;    // 22-bit ~100 seconds
        //--------------------------
        // Measured Sleep Duration
        //--------------------------
        // 0x4000 (16384): 672s
        // 0x3000 (12288): 446s
        // 0x2000 ( 8192): 224s
        // 0x1800 ( 6144): 112s
        // 0x1600 ( 5632):  83.1s
        // 0x1400 ( 5120):  55.2s
        // 0x1200 ( 4608):  27.2s
        // 0x1000 ( 4096):   0.106s
        //--------------------------
        // Equation (NOTE: sleep_duratio in sec)
        //  sleep_duration = 0.054688*(pmu_threshold - 4096)
        //  pmu_threshold = 18.28571*sleep_duration + 4096
        //

        //adc_offset = 0xFFFFFFFD; // -3
        //adc_offset = 0xFFFFFFFA; // -6
        adc_offset = 0;

        //-------------------------
        // pmu_sel_margin   Margin     
        //-------------------------
        //  0               5%
        //  1               6%
        //  2               7%
        //  3               8%
        //-------------------------
        pmu_sel_margin = 0;


        // Hysteresis
        hysteresis = 1; // hysteresis is used only when "decreasing" the SAR ratio (i.e., when the ADC value goes "up")
                        // If (adc_val < previous_adc_value) -OR- (adc_val > (previous_adc_value + hysteresis)),
                        // it changes the SAR ratio as needed. Otherwise, the SAR ratio does not change.


        //---------------------------------------------------------------------------------------
        // HORIZON_CONFIG
        //---------------------------------------------------------------------------------------
        pmu_reg_write(0x45, // Default  // Description
        //---------------------------------------------------------------------------------------
            // NOTE: [14:11] is ignored if the corresponding DESIRED=0 -OR- STALL=1.
            //------------------------------------------------------------------------
            ( (0x1 << 14)   // 0x1      // If 1, 'horizon' enqueues 'wait_clock_count' with TICK_REPEAT_VBAT_ADJUST
            | (0x0 << 13)   // 0x1      // If 1, 'horizon' enqueues 'adjust_adc'
            | (0x0 << 12)   // 0x1      // If 1, 'horizon' enqueues 'adjust_sar_ratio'
            | (0x1 << 11)   // 0x0		// If 1, 'horizon' enqueues 'vbat_read_only'
            //------------------------------------------------------------------------
            | (0x1 << 9 )   // 0x0      // 0x0: Disable clock monitoring
                                        // 0x1: Monitoring SAR clock
                                        // 0x2: Monitoring UPC clock
                                        // 0x3: Monitoring DNC clock
            | (0x0  << 8)   // 0x0      // Reserved
            | (64   << 0)   // 0x48     // Sets ADC_SAMPLING_BIT in 'vbat_read_only' task (0<=N<=255); ADC measures (N/256)xVBAT in Flip mode.
        ));

        *PMU_TARGET_REG_ADDR = 0;

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

        snt_curr = snt_read_wup_timer();
        
        // Set the Active floor setting to the minimum (b/c RAT is enabled at this point)
        pmu_set_active_min();

        // Disable the PRC wakeup timer
        *REG_WUPT_CONFIG = *REG_WUPT_CONFIG & 0xFF3FFFFF; // WUP_ENABLE=0, WUP_WREQ_EN=0

        //-------------------------------------------------
        // NFC 
        //-------------------------------------------------
        nfc_init();
        eeprom_addr = 0;

        // Wipe out the EEPROM
        uint32_t idx;
        for (idx=0; idx<8192; idx+=256) {
            nfc_i2c_word_pattern_write(
                /*e2*/      0, 
                /*addr*/    idx, 
                /*data*/    0x0, 
                /*nw*/      64);
        }

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);

    }
}

static uint32_t calc_new_sar_ratio(uint32_t adc_val, uint32_t offset, uint32_t num_cons_meas, uint32_t hyst) {
    // Get the current value
    uint32_t new_val = __pmu_sar_ratio__;

    // If ADC value is valid
    if (pmu_validate_adc_val(adc_val)) {
        //---------------------------------------------------
        // Adjust SAR RATIO
        //---------------------------------------------------
        // 2.0V ( 95) < VBAT < 2.3V (109): 0x60
        // 2.3V (109) < VBAT < 2.5V (118): 0x54
        // 2.5V (118) < VBAT < 2.7V (127): 0x4C
        // 2.7V (127) < VBAT < 3.0V (137): 0x48
        //---------------------------------------------------
        // Hysteresis: 65mV (3)
        //uint32_t hyst = 3; // hysteresis
        //---------------------------------------------------

        uint32_t new_down, new_up, upper_threshold, lower_threshold;

        if (__pmu_sar_ratio__ == 0x60) {
            new_up   = 0x54; upper_threshold = 109 + hyst + offset;
            new_down = 0x60; lower_threshold =  95 - hyst + offset;
        }
        else if (__pmu_sar_ratio__ == 0x54) {
            new_up   = 0x4C; upper_threshold = 118 + hyst + offset;
            new_down = 0x60; lower_threshold = 109 - hyst + offset;
        }
        else if (__pmu_sar_ratio__ == 0x4C) {
            new_up   = 0x48; upper_threshold = 127 + hyst + offset;
            new_down = 0x54; lower_threshold = 118 - hyst + offset;
        }
        else if (__pmu_sar_ratio__ == 0x48) {
            new_up   = 0x48; upper_threshold = 137 + hyst + offset;
            new_down = 0x4C; lower_threshold = 127 - hyst + offset;
        }
        // dummy values
        else {
            new_up   = 0x48; upper_threshold = 137 + hyst + offset;
            new_down = 0x4C; lower_threshold = 127 - hyst + offset;
        }

        if (adc_val > upper_threshold) {
            __pmu_sar_ratio_lower_streak__ = 0;
            if (__pmu_sar_ratio_upper_streak__ < num_cons_meas) __pmu_sar_ratio_upper_streak__++;
            if (__pmu_sar_ratio_upper_streak__ >= num_cons_meas) {
                new_val = new_up;
                __pmu_sar_ratio_upper_streak__ = 0;
            }
        }
        else if (adc_val < lower_threshold) {
            __pmu_sar_ratio_upper_streak__ = 0;
            if (__pmu_sar_ratio_lower_streak__ < num_cons_meas) __pmu_sar_ratio_lower_streak__++;
            if (__pmu_sar_ratio_lower_streak__ >= num_cons_meas) {
                new_val = new_down;
                __pmu_sar_ratio_lower_streak__ = 0;
            }
        }
        else {
            __pmu_sar_ratio_upper_streak__ = 0;
            __pmu_sar_ratio_lower_streak__ = 0;
        }
    }

    return new_val;
}

uint32_t calc_auto_sar_ratio (uint32_t adc_val, uint32_t offset, uint32_t sel_margin, uint32_t hysteresis) {
// SEE pmu_calc_new_sar_ratio() in TMCv1r1.c

    // Get the current value
    uint32_t new_val = __pmu_sar_ratio__;

    // Add the ADC offset
    adc_val += offset;
    adc_val &= 0xFF;

    // If ADC value is valid
    if (pmu_validate_adc_val(adc_val)) {

        // Calculate the new SAR ratio
        if (
               (__pmu_last_effective_adc_val__==0)                       // The very first-time
            || (adc_val < __pmu_last_effective_adc_val__)                // ADC values went down
            || (adc_val > (__pmu_last_effective_adc_val__ + hysteresis)) // ADC values went up (+ hysteresis)
        ) {

            if (sel_margin  > 3) sel_margin = 3;

            uint32_t cent_val = 115;
            uint32_t c0, c1, c2;

            if      (sel_margin==0)  {   c0 = 0x536778;  c1 = 0x0D5DB;    c2 = 0x1E5; }   // 5%
            else if (sel_margin==1)  {   c0 = 0x545DCE;  c1 = 0x0D4AD;    c2 = 0x1E0; }   // 6%
            else if (sel_margin==2)  {   c0 = 0x5516E1;  c1 = 0x0DC1D;    c2 = 0x1F5; }   // 7%
            else if (sel_margin==3)  {   c0 = 0x559E24;  c1 = 0x0DC93;    c2 = 0x1F5; }   // 8%

            // Actual Implementation
            uint32_t pwr1, pwr1_sign;
            if (adc_val < cent_val) { pwr1_sign = 0; pwr1 = cent_val - adc_val;}
            else                    { pwr1_sign = 1; pwr1 = adc_val - cent_val;}
            uint32_t pwr2 = mult(/*num_a*/pwr1, /*num_b*/pwr1);

            if (pwr1_sign)
                new_val = c0 - mult(/*num_a*/c1, /*num_b*/pwr1) + mult(/*num_a*/c2, /*num_b*/pwr2);
            else
                new_val = c0 + mult(/*num_a*/c1, /*num_b*/pwr1) + mult(/*num_a*/c2, /*num_b*/pwr2);

            new_val = new_val >> 16;

            // Limit the SAR ratio
            if (new_val < 64) new_val = 64;

            // Updates __pmu_last_effective_adc_val__
            if ((__pmu_last_effective_adc_val__==0) || (new_val != __pmu_sar_ratio__))
                __pmu_last_effective_adc_val__ = adc_val;
        }

    }

    return new_val;

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
        // Stop the VCO counter
        mbus_write_message32(PMU_ADDR<<4,
                             (0x53 << 24)   // REG ADDR
                            |( 0x0 << 23)   // VCO_CNT_ENABLE
                            |( 0x0 << 22)   // VCO_CNT_MODE
                            |( 0x0 <<  0)   // VCO_CNT_THRESHOLD
                            );

        snt_prev = snt_curr;
        snt_curr = snt_read_wup_timer();

        // Waken up by PMU
        if (get_bit(wakeup_source, 4)) {
            // Waken up by PMU VCO Counter
            if ((((*PMU_TARGET_REG_ADDR)>>22)&0x3)==0x2) {
                *PMU_TARGET_REG_ADDR = 0;

                nfc_i2c_byte_write( /*e2*/  0,
                                    /*addr*/eeprom_addr,
                                    /*data*/(pmu_get_sar_ratio()<<24)|((snt_curr-snt_prev)&0xFFFFFF),
                                    /*nb*/  4
                                    );

                mbus_write_message32(0xA0, eeprom_addr);
                mbus_write_message32(0xA1, (pmu_get_sar_ratio()<<24)|((snt_curr-snt_prev)&0xFFFFFF));
                eeprom_addr += 4;

                // VBAT Measurement and SAR_RATIO Adjustment
                uint32_t pmu_adc_vbat_val = pmu_read_adc();
                //uint32_t pmu_sar_ratio    = calc_new_sar_ratio( /*adc_val*/         pmu_adc_vbat_val, 
                //                                                /*offset*/          adc_offset, 
                //                                                /*num_cons_meas*/   sel_margin,
                //                                                /*hyst*/            hysteresis
                //                                                );
                //uint32_t pmu_sar_ratio    = pmu_calc_new_sar_ratio( /*adc_val*/     pmu_adc_vbat_val, 
                //                                                    /*offset*/      adc_offset, 
                //                                                    /*sel_margin*/  pmu_sel_margin,
                //                                                    /*hysteresis*/  hysteresis
                //                                                    );
                uint32_t pmu_sar_ratio    = calc_auto_sar_ratio   ( /*adc_val*/     pmu_adc_vbat_val, 
                                                                    /*offset*/      adc_offset, 
                                                                    /*sel_margin*/  pmu_sel_margin,
                                                                    /*hysteresis*/  hysteresis
                                                                    );

                // Change the SAR ratio
                pmu_set_sar_ratio(pmu_sar_ratio);

                nfc_i2c_byte_write( /*e2*/  0,
                                    /*addr*/eeprom_addr,
                                    /*data*/(pmu_get_sar_ratio()<<24)|(pmu_adc_vbat_val<<16)|(adc_offset&0xFFFF),
                                    /*nb*/  4
                                    );

                mbus_write_message32(0xA2, eeprom_addr);
                mbus_write_message32(0xA3, (pmu_get_sar_ratio()<<24)|(pmu_adc_vbat_val<<16)|(adc_offset&0xFFFF));
                eeprom_addr += 4;

            }
            // Something Wrong happened
            else {
            }
        }
        // Error
        else {
        }

        // If waken up by GOC/EP
        uint32_t goc_raw = *GOC_DATA_IRQ;
        if (goc_raw!=0) {
            *GOC_DATA_IRQ   = 0;

            uint32_t goc_header = (goc_raw>>24)&0xFF;
            uint32_t goc_data   = goc_raw&0xFFFFFF;

            if (goc_header == 0x00) {
                pmu_set_sar_ratio(goc_data&0xFF);
                *PMU_TARGET_REG_ADDR = 0;
            }
            else if (goc_header == 0x01) {
                pmu_threshold = goc_data&0x3FFFFF;
            }
            else if (goc_header == 0x02) {
                // Sign Bit Extension
                if ((goc_data>>23)&0x1)
                    adc_offset = 0xFF000000 | goc_data;
                else
                    adc_offset = goc_data;
            }
            else if (goc_header == 0x03) {
                pmu_sel_margin = goc_data;
            }
            else if (goc_header == 0x04) {
                hysteresis = goc_data;
            }
            else if (goc_header == 0xEE) {
                uint32_t idx;
                for (idx=0; idx<8192; idx+=256) {
                    nfc_i2c_word_pattern_write(
                        /*e2*/      0, 
                        /*addr*/    idx, 
                        /*data*/    0x0, 
                        /*nw*/      64);
                }
            }
            else if (goc_header == 0xFF) {
                eeprom_addr = goc_data;
            }

        }

    }
    

    // Restart the VCO counter
    mbus_write_message32(PMU_ADDR<<4,
                         (    0x53 << 24)   // REG ADDR
                        |(     0x1 << 23)   // VCO_CNT_ENABLE
                        |(     0x0 << 22)   // VCO_CNT_MODE
                        |(pmu_threshold <<  0)   // VCO_CNT_THRESHOLD
                        );

    operation_sleep();

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
