//*******************************************************************************************
// CISv4Br1 SOURCE FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//-------------------------------------------------------------------------------------------
//      PREv23E -> RDCv4 -> SNTv6r1 -> PMUv13r2
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*********************************************************
// HEADER AND OTHER SOURCE FILES
//*********************************************************
#include "CISv4Br1.h"

//*******************************************************************
// CISv4Br1 FUNCTIONS
//*******************************************************************
// SEE HEADER FILE FOR FUNCTION BRIEFS


//-------------------------------------------------------------------
// Basic Building Functions
//-------------------------------------------------------------------

uint32_t set_bits (uint32_t var, uint32_t msb_idx, uint32_t lsb_idx, uint32_t value) {
    uint32_t mask_pattern = (0xFFFFFFFF << (msb_idx + 1)) | (0xFFFFFFFF >> (32 - lsb_idx));
    return (var & mask_pattern) | ((value << lsb_idx) & ~mask_pattern);
}

uint32_t set_bit (uint32_t var, uint32_t idx, uint32_t value) {
    if (value & 0x1) return var |  (0x1 << idx);
    else             return var & ~(0x1 << idx);
}

uint32_t get_bits (uint32_t variable, uint32_t msb_idx, uint32_t lsb_idx) {
    return ((variable << (31-msb_idx)) >> (31-msb_idx)) >> lsb_idx;
}

uint32_t get_bit (uint32_t variable, uint32_t idx) {
    return (variable >> idx) & 0x1;
}

//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

//-------------------------------------------------------------------
// PMU Floor Loop-Up Tables
//-------------------------------------------------------------------

#ifndef __USE_HCODE__
// ACTIVE
volatile union pmu_floor_t __pmu_floor_active__[] = {
    //  Temp       idx     R    L    BASE  L_SAR
    /*PMU_10C */ /* 0*/  {{0xF, 0x7, 0x0F, 0xE}},
    /*PMU_20C */ /* 1*/  {{0xF, 0x7, 0x0F, 0xE}},
    /*PMU_25C */ /* 2*/  {{0xF, 0x3, 0x0F, 0x7}},
    /*PMU_35C */ /* 3*/  {{0xF, 0x2, 0x0F, 0x4}},
    /*PMU_55C */ /* 4*/  {{0x6, 0x1, 0x0F, 0x2}},
    /*PMU_75C */ /* 5*/  {{0x6, 0x1, 0x0F, 0x2}},
    /*PMU_95C */ /* 6*/  {{0x6, 0x1, 0x0F, 0x2}},
    };

// SLEEP
volatile union pmu_floor_t __pmu_floor_sleep__[] = {
    //  Temp       idx     R    L    BASE  L_SAR
    /*PMU_10C */ /* 0*/  {{0xF, 0x1, 0x01, 0x2}},
    /*PMU_20C */ /* 1*/  {{0xF, 0x1, 0x01, 0x2}},
    /*PMU_25C */ /* 2*/  {{0xF, 0x0, 0x01, 0x1}},
    /*PMU_35C */ /* 3*/  {{0xF, 0x0, 0x01, 0x1}},
    /*PMU_55C */ /* 4*/  {{0x6, 0x0, 0x01, 0x1}},
    /*PMU_75C */ /* 5*/  {{0x6, 0x0, 0x01, 0x1}},
    /*PMU_95C */ /* 6*/  {{0x6, 0x0, 0x01, 0x1}},
    };

// SLEEP (TEMP SENSOR)
volatile union pmu_floor_t __pmu_floor_sleep_tsnt__[] = {
    //  Temp Range        idx     R    L    BASE  L_SAR
    /*       ~PMU_10C*/ /* 0*/  {{0xF, 0xA, 0x07, 0xF}},
    /*PMU_20C~PMU_25C*/ /* 1*/  {{0xF, 0xA, 0x05, 0xF}},
    /*PMU_35C~PMU_55C*/ /* 2*/  {{0xA, 0xA, 0x05, 0xF}},
    /*PMU_75C~       */ /* 3*/  {{0x5, 0xA, 0x05, 0xF}},
    };

// SLEEP (LOW)
volatile union pmu_floor_t __pmu_floor_sleep_low__[] = {
    //  idx     R    L    BASE  L_SAR
      /* 0*/  {{0xF, 0x0, 0x01, 0x1}},
      };

// SLEEP (RADIO)
volatile union pmu_floor_t __pmu_floor_sleep_radio__[] = {
    //  idx     R    L    BASE  L_SAR
      /* 0*/  {{0xF, 0xA, 0x05, 0xF}},
      };
#endif

//-------------------------------------------------------------------
// PMU Functions
//-------------------------------------------------------------------

void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data) {
    enable_reg_irq(PMU_TARGET_REG_IDX);
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    WFI();
}

void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    pmu_reg_write_core(reg_addr, reg_data);
}

uint32_t pmu_reg_read (uint32_t reg_addr) {
    pmu_reg_write_core(0x00, reg_addr);
    return *PMU_TARGET_REG_ADDR;
}

void pmu_set_floor(uint8_t mode, uint8_t r, uint8_t l, uint8_t base, uint8_t l_sar){

    // Register Addresses
    uint32_t sar = (mode == PMU_ACTIVE) ? 0x16 : 0x15;
    uint32_t upc = (mode == PMU_ACTIVE) ? 0x18 : 0x17;
    uint32_t dnc = (mode == PMU_ACTIVE) ? 0x1A : 0x19;

    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(sar,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0 << 19)     // 1'h0     // Enable PFM even during periodic reset
        | (0 << 18)     // 1'h0     // Enable PFM even when VREF is not used as reference
        | (0 << 17)     // 1'h0     // Enable PFM
        | (1 << 14)     // 3'h3     // Comparator clock division ratio (0x1 being slowest)
        | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l_sar << 5)  // 4'h8     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(upc,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (3 << 14)     // 2'h0     // Desired Vout/Vin ratio
        | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l << 5)      // 4'h4     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(dnc,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l << 5)      // 4'h4     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h08    // Floor frequency base (0-63)
    ));
}

void pmu_active_floor(uint8_t r, uint8_t l, uint8_t base, uint8_t l_sar){

    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_ACTIVE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x16, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0 << 19)     // 1'h0     // Enable PFM even during periodic reset
        | (0 << 18)     // 1'h0     // Enable PFM even when VREF is not used as reference
        | (0 << 17)     // 1'h0     // Enable PFM
        | (1 << 14)     // 3'h3     // Comparator clock division ratio (0x1 being slowest)
        | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l_sar << 5)  // 4'h8     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_ACTIVE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x18, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (3 << 14)     // 2'h0     // Desired Vout/Vin ratio
        | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l << 5)      // 4'h4     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_ACTIVE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x1A,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l << 5)      // 4'h4     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h08    // Floor frequency base (0-63)
    ));
}

#ifndef __USE_HCODE__
void pmu_set_temp_state (uint32_t val) {
    __pmu_temp_state__ = val;
}

uint32_t pmu_get_temp_state (void) {
    return __pmu_temp_state__;
}

void pmu_set_active_temp_based(){
    pmu_set_floor(PMU_ACTIVE, 
                    __pmu_floor_active__[__pmu_temp_state__].R,
                    __pmu_floor_active__[__pmu_temp_state__].L,
                    __pmu_floor_active__[__pmu_temp_state__].BASE,
                    __pmu_floor_active__[__pmu_temp_state__].L_SAR
                    );
}

void pmu_set_sleep_temp_based(){
    pmu_set_floor(PMU_SLEEP, 
                    __pmu_floor_sleep__[__pmu_temp_state__].R,
                    __pmu_floor_sleep__[__pmu_temp_state__].L,
                    __pmu_floor_sleep__[__pmu_temp_state__].BASE,
                    __pmu_floor_sleep__[__pmu_temp_state__].L_SAR
                    );
}

void pmu_set_sleep_low(){
    pmu_set_floor(PMU_SLEEP, 
                    __pmu_floor_sleep_low__[0].R,
                    __pmu_floor_sleep_low__[0].L,
                    __pmu_floor_sleep_low__[0].BASE,
                    __pmu_floor_sleep_low__[0].L_SAR
                    );
}
#endif

void pmu_set_active_min(){
    pmu_active_floor(0x1, 0x0, 0x1, 0x0);
}

void pmu_set_sar_ratio (uint32_t ratio) {

    #ifdef DEVEL
        mbus_write_message32(0xA4, (__pmu_sar_ratio__<<8)|ratio);
    #endif

    if (ratio != __pmu_sar_ratio__) {

        //---------------------------------------------------------------------------------------
        // SAR_RATIO_OVERRIDE
        //---------------------------------------------------------------------------------------
        pmu_reg_write(0x05, // Default  // Description
        //---------------------------------------------------------------------------------------
            ( (0x3 << 14)   // 0x0      // 0x3: Perform sar_reset followed by sar/upc/dnc_stabilized upon writing into this register
                                        // 0x2: Perform sar_reset upon writing into this register
                                        // 0x1: Perform sar/upc/dnc_stabilized upon writing into this register
                                        // 0x0: Do nothing
            | (0x2 << 12)   // 0x0      // 0x3: Let VDD_CLK always connected to VBAT; 
                                        // 0x2: Let VDD_CLK always connected to V1P2;
                                        // 0x0: Let PMU handle.
            | (0x2 << 10)   // 0x2      // 0x3: Enable the periodic SAR reset; 
                                        // 0x2: Disable the periodic SAR reset; 
                                        // 0x0: Let PMU handle.
                                        // This setting is temporarily ignored when [15] is triggered.
            | (0x0 << 8 )   // 0x0      // 0x3: Let the SAR do UP-conversion (use if VBAT < 1.2V)
                                        // 0x2: Let the SAR do DOWN-conversion (use if VBAT > 1.2V)
                                        // 0x0: Let PMU handle.
            | (0x1 << 7 )   // 0x0      // If 1, override SAR ratio with [6:0].
            | (ratio)       // 0x00     // SAR ratio for overriding (valid only when [7]=1)
        ));

        __pmu_sar_ratio__ = ratio;

        //delay(10000); // ~0.4s with 140kHz CPU clock
        delay(200); // ~10ms with 140kHz CPU clock
    }
}

uint32_t pmu_get_sar_ratio (void) {
    return __pmu_sar_ratio__;
}

uint32_t pmu_read_adc(void) {
    return pmu_reg_read(0x03) & 0xFF;
}

uint32_t pmu_validate_adc_val (uint32_t adc_val) {
    return ((adc_val > __PMU_ADC_LOWER_LIMIT__) && (adc_val < __PMU_ADC_UPPER_LIMIT__));
}

void pmu_sleep_floor(uint32_t type) {
    // type
    //  0 : normal sleep
    //  1 : temperature meas sleep
    uint32_t ul = (type) ? 3 : 0;
    uint32_t sl = (type) ? 4 : 0;
    uint32_t b  = (type) ? 4 : 1;

    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x15,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 0 << 19)    // 1'h0     // Enable PFM even during periodic reset
        | ( 0 << 18)    // 1'h0     // Enable PFM even when VREF is not used as reference
        | ( 0 << 17)    // 1'h0     // Enable PFM
        | ( 1 << 14)    // 3'h3     // Comparator clock division ratio
        | ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (15 <<  9)    // 4'h8     // Frequency multiplier R
        | (sl <<  5)    // 4'h8     // Frequency multiplier L (actually L+1)
        | ( b      )    // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x17,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 3 << 14)    // 2'h0     // Desired Vout/Vin ratio
        | ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (15 <<  9)    // 4'h8     // Frequency multiplier R
        | (ul <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | ( b      )    // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x19,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (15 <<  9)    // 4'h8     // Frequency multiplier R
        | ( 0 <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | ( 1      )    // 5'h08    // Floor frequency base (0-63)
    ));
}

void pmu_init(void){

    #ifdef __PMU_CHECK_WRITE__ // See PMU Behavior section at the top)
        //[5]: PMU_CHECK_WRITE(1'h0); [4]: PMU_IRQ_EN(1'h1); [3:2]: LC_CLK_DIV(2'h3); [1:0]: LC_CLK_RING(2'h1)
        mbus_remote_register_write(PMU_ADDR, 0x51, (0x1 << 5) | (0x1 << 4) | (0x3 << 2) | (0x1 << 0));
    #endif

    #ifndef __USE_HCODE__
        // Initialize PMU State Variables
        __pmu_desired_state_sleep__.value  = __PMU_DESIRED_STATE_SLEEP_DEFAULT__;
        __pmu_desired_state_active__.value = __PMU_DESIRED_STATE_ACTIVE_DEFAULT__;
    #endif

    // Initialized the temperature to 25C and adjust floors
    #ifndef __USE_HCODE__
        __pmu_temp_state__ = PMU_25C;
        pmu_set_active_temp_based();
    #else
        //pmu_set_floor(PMU_ACTIVE, 
        //                /*R*/       0xF,
        //                /*L*/       0x3,
        //                /*BASE*/    0x0F,
        //                /*L_SAR*/   0x7
        //                );
        pmu_active_floor(   /*R*/       0xF,
                            /*L*/       0x3,
                            /*BASE*/    0x0F,
                            /*L_SAR*/   0x7
                            );
    #endif

    // Sleep Floor Setting
    pmu_sleep_floor(/*type*/0);

    // Initialize other PMU variables
    __pmu_sar_ratio_upper_streak__  = 0;
    __pmu_sar_ratio_lower_streak__  = 0;
    __pmu_low_vbat_streak__         = 0;
    __pmu_crit_vbat_streak__        = 0;

    // Initialize SAR Ratio
    pmu_set_sar_ratio(0x4C); // See pmu_calc_new_sar_ratio()
    __pmu_last_effective_adc_val__ = 0;

    // Disable ADC in Active
    // PMU ADC will be automatically reset when system wakes up
    #ifndef __USE_HCODE__
        //---------------------------------------------------
        // CTRL_DESIRED_STATE_ACTIVE; Disable ADC
        //---------------------------------------------------
        __pmu_desired_state_active__.adc_output_ready   = 0;
        __pmu_desired_state_active__.adc_adjusted       = 0;
        __pmu_desired_state_active__.sar_ratio_adjusted = 0;
        __pmu_desired_state_active__.vbat_read_only     = 0;
        pmu_reg_write(0x3C, __pmu_desired_state_active__.value);

        // Turn off ADC offset measurement in Sleep
        //---------------------------------------------------
        // CTRL_DESIRED_STATE_SLEEP; Use ADC in Flip-Mode
        //---------------------------------------------------
        __pmu_desired_state_sleep__.adc_output_ready   = 1;
        __pmu_desired_state_sleep__.adc_adjusted       = 0;
        __pmu_desired_state_sleep__.sar_ratio_adjusted = 0;
        __pmu_desired_state_sleep__.vbat_read_only     = 1;
        pmu_reg_write(0x3B, __pmu_desired_state_sleep__.value);
    #else
        pmu_reg_write(0x3C, 0x0017C7FF);
        pmu_reg_write(0x3B, 0x001FCFFF);
    #endif

    // Set ADC Period
    //---------------------------------------------------
    // TICK_REPEAT_VBAT_ADJUST
    //---------------------------------------------------
    pmu_reg_write(0x36, __PMU_ADC_PERIOD__); 

    //---------------------------------------------------
    // TICK_ADC_RESET
    //---------------------------------------------------
    pmu_reg_write(0x33, __PMU_TICK_ADC_RESET__);

    //---------------------------------------------------
    // TICK_ADC_CLK
    //---------------------------------------------------
    pmu_reg_write(0x34, __PMU_TICK_ADC_CLK__);

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
        | (0x0 << 9 )   // 0x0      // 0x0: Disable clock monitoring
                                    // 0x1: Monitoring SAR clock
                                    // 0x2: Monitoring UPC clock
                                    // 0x3: Monitoring DNC clock
        | (0x0  << 8)   // 0x0      // Reserved
        | (64   << 0)   // 0x48     // Sets ADC_SAMPLING_BIT in 'vbat_read_only' task (0<=N<=255); ADC measures (N/256)xVBAT in Flip mode.
    ));

    // VOLTAGE_CLAMP_TRIM (See PMU VSOLAR SHORT BEHAVIOR section at the top)
    pmu_reg_write(0x0E, (__PMU_VSOLAR_SHORT__ << 8) | (__PMU_VSOLAR_CLAMP_BOTTOM__ << 4) | (__PMU_VSOLAR_CLAMP_TOP__ << 0));

    // Increase reference voltage level 
    //---------------------------------------------------------------------------------------
    // REFERENCE_TRIM
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x0C, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0x0 <<  5)   // 0x0      // (Coarse) Number of stacked pull-down diodes
        | (0x0 <<  0)   // 0x18     // (Fine)   Width of pull-up NMOS
    ));

    // Reduce LDO bias current in active mode
    //---------------------------------------------------------------------------------------
    // SAR_TRIM_ACTIVE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x10, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0x0 << 16)   // 0x0      // Disable reverse configuration for upconversion
        | (0x0 << 14)   // 0x2      // Reserved
        | (0x1 << 13)   // 0x1      // Enable [12]
        | (0x0 << 12)   // 0x0      // (Valid if [13]) Use VREF as frequency controller reference
        | (0x0 <<  4)   // 0x20     // Reserved
        | (0x1 <<  0)   // 0x4      // Bias current (current is proportional to the value)
    ));

    //---------------------------------------------------------------------------------------
    // TICK_UDC_SLEEP_WAIT
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x2E, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (1 << 0)      // 12'd64   // Wait time for UDC during active->sleep conversion
    ));

    //---------------------------------------------------------------------------------------
    // TICK_SAR_SLEEP_WAIT
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x2D, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (1 << 0)      // 12'd64   // Wait time for SAR during active->sleep conversion
    ));

    // Adjust canary-based active pmu strength
    //---------------------------------------------------------------------------------------
    // CP_CONFIG
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x48, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0x0 << 10)   // 0x0      // SAR_V1P2_DIV_SEL[1:0]
        | (0x1 <<  9)   // 0x0      // SAR_CMP_EN_OVERRIDE. If 1, it always enables the SAR comparator. If 0, PMU_CTRL automatically enables/disables.
        | (0x1 <<  8)   // 0x0      // UPC_CMP_EN_OVERRIDE. If 1, it always enables the UPC comparator. If 0, PMU_CTRL automatically enables/disables.
        | (0x1 <<  7)   // 0x0      // DNC_CMP_EN_OVERRIDE. If 1, it always enables the DNC comparator. If 0, PMU_CTRL automatically enables/disables.
        | (0x2 <<  4)   // 0x3      // SAR_CP_CAP
        | (0x0 <<  2)   // 0x3      // UPC_CP_CAP
        | (0x1 <<  0)   // 0x3      // DNC_CP_CAP
    ));

//    // Enable Slow loop
//    //---------------------------------------------------------------------------------------
//    // SAR_TRIM_SLEEP
//    //---------------------------------------------------------------------------------------
//    pmu_reg_write(0x0F, // Default  // Description
//    //---------------------------------------------------------------------------------------
//        ( (0x0 << 16)   // 0x0      // Disable reverse configuration for upconversion
//        | (0x0 << 14)   // 0x2      // Reserved
//        | (0x1 << 13)   // 0x1      // Enable [12]
//        | (0x1 << 12)   // 0x0      // (Valid if [13]) Use VREF as frequency controller reference
//        | (0x0 <<  4)   // 0x20     // Reserved
//        | (0x4 <<  0)   // 0x4      // Bias current (current is proportional to the value)
//    ));

    // Enables RAT (it won't take effect until the next sleep or wakeup-up transition)
    //---------------------------------------------------------------------------------------
    // RAT_CONFIG
    // NOTE: Unless overridden, changes in RAT_CONFIG[19:13] do not take effect
    //       until the next sleep or wakeup-up transition.
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x47, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (1 << 19)   // 1'h0     // Enable RAT
        | (0 << 18)   // 1'h0     // Enable SAR Slow-Loop in Active (NOTE: When RAT is enabled, SAR Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (0 << 17)   // 1'h0     // Enable UPC Slow-Loop in Active (NOTE: When RAT is enabled, UPC Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (0 << 16)   // 1'h0     // Enable DNC Slow-Loop in Active (NOTE: When RAT is enabled, DNC Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (1 << 15)   // 1'h0     // Enable SAR Slow-Loop in Sleep
        | (1 << 14)   // 1'h0     // Enable UPC Slow-Loop in Sleep
        | (1 << 13)   // 1'h0     // Enable DNC Slow-Loop in Sleep
        | (1 << 11)   // 2'h0     // Clock Ring Tuning (0x0: 13 stages; 0x1: 11 stages; 0x2: 9 stages; 0x3: 7 stages)
        | (4 << 8 )   // 3'h0     // Clock Divider Tuning for SAR Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (4 << 5 )   // 3'h0     // Clock Divider Tuning for UPC Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (4 << 2 )   // 3'h0     // Clock Divider Tuning for DNC Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (3 << 0 )   // 2'h0     // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up
    ));


//    // Maximize the sleep delay: This would add ~80ms delay before turning on the UDC/DNC slow loop.
//    //---------------------------------------------------------------------------------------
//    // TICK_SLEEP_WAIT
//    //---------------------------------------------------------------------------------------
//    pmu_reg_write(0x38, // Default  // Description
//    //---------------------------------------------------------------------------------------
//        ( (0xFFF << 0)  // 0x1E     // (12-bit) Delay before turning on UPC/DNC slow loop
//    ));

}

//*******************************************************************
// SNT FUNCTIONS
//*******************************************************************

void snt_init(void) {

    // Regiser File variables
#ifndef __USE_HCODE__
    snt_r00.as_int = SNT_R00_DEFAULT_AS_INT;
    snt_r01.as_int = SNT_R01_DEFAULT_AS_INT;
    snt_r08.as_int = SNT_R08_DEFAULT_AS_INT;
    snt_r09.as_int = SNT_R09_DEFAULT_AS_INT;
#endif
    snt_r17.as_int = SNT_R17_DEFAULT_AS_INT;

#ifdef __USE_HCODE__
    // Temp Sensor Initialization
    //snt_r01.TSNS_RESETn     = 0;
    //snt_r01.TSNS_EN_IRQ     = 1;
    //snt_r01.TSNS_BURST_MODE = 0;
    //snt_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,0x000902);
#else
    // Temp Sensor Initialization
    snt_r01.TSNS_RESETn     = 0;
    snt_r01.TSNS_EN_IRQ     = 1;
    snt_r01.TSNS_BURST_MODE = 0;
    snt_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
#endif

    //---------------------------------------------------------------------------------------
    // Set temp sensor conversion time
    //---------------------------------------------------------------------------------------
    mbus_remote_register_write(SNT_ADDR,0x03,
    //---------------------------------------------------------------------------------------
                        // Default  // Description
        ( (0x4 << 16)   // 0x4      // TSNS_MIM              
        | (0x4 << 13)   // 0x4      // TSNS_MOM              
        | (0xE <<  9)   // 0xE      // TSNS_SEL_VVDD         
        | (0x3 <<  6)   // 0x1      // TSNS_SEL_STB_TIME     
        | (0x2 <<  4)   // 0x2      // TSNS_SEL_REF_STB_TIME 
        | (0x7 <<  0)   // 0x6      // TSNS_SEL_CONV_TIME (NOTE: This must be consistent with 'c' value in tconv())
        ));

    // Wakeup Timer configuration
    snt_r17.WUP_INT_RPLY_REG_ADDR = PLD_TARGET_REG_IDX;
    snt_r17.WUP_INT_RPLY_SHORT_ADDR = 0x10;
    snt_r17.WUP_CLK_SEL = 0x0; // 0: Use CLK_TMR, 1: Use CLK_TSNS
    snt_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
    mbus_remote_register_write(SNT_ADDR,0x17,snt_r17.as_int);

    // Turn on LDO VREF (previously snt_ldo_vref_on())
#ifdef __USE_HCODE__
    //snt_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,0x000024);
#else
    snt_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
#endif
    // Delay (~50ms @ 100kHz clock speed); NOTE: Start-Up Time of VREF @ TT, 1.1V, 27C is ~40ms.
    delay(5000);
}

void snt_temp_sensor_power_on(void) {

#ifdef __USE_HCODE__
    // Turn on LDO
    //snt_r00.LDO_EN_IREF = 1;
    //snt_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,0x000027);

    // Delay (~10ms) @ 100kHz clock speed)
    delay(1000); 

    // Turn on digital block
    //snt_r01.TSNS_SEL_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,0x00090A);

    // Turn on analog block
    //snt_r01.TSNS_EN_SENSOR_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,0x00092A);

    // Delay (~2ms @ 100kHz clock speed)
    delay(200);
#else
    // Turn on LDO
    snt_r00.LDO_EN_IREF = 1;
    snt_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);

    // Delay (~10ms) @ 100kHz clock speed)
    delay(1000); 

    // Turn on digital block
    snt_r01.TSNS_SEL_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    // Turn on analog block
    snt_r01.TSNS_EN_SENSOR_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    // Delay (~2ms @ 100kHz clock speed)
    delay(200);
#endif
}


void snt_temp_sensor_power_off(void){
#ifdef __USE_HCODE__
    //snt_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,0x000928);

    //snt_r01.TSNS_RESETn         = 0;
    //snt_r01.TSNS_ISOLATE        = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,0x00092A);

    //snt_r01.TSNS_SEL_V1P2       = 0;
    //snt_r01.TSNS_EN_SENSOR_V1P2 = 0;
    //snt_r01.TSNS_SEL_LDO        = 0;
    //snt_r01.TSNS_EN_SENSOR_LDO  = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,0x000902);

    // Turn off LDO
    //snt_r00.LDO_EN_IREF   = 0;
    //snt_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0x00,0x000024);
#else
    snt_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    snt_r01.TSNS_RESETn         = 0;
    snt_r01.TSNS_ISOLATE        = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    snt_r01.TSNS_SEL_V1P2       = 0;
    snt_r01.TSNS_EN_SENSOR_V1P2 = 0;
    snt_r01.TSNS_SEL_LDO        = 0;
    snt_r01.TSNS_EN_SENSOR_LDO  = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    // Turn off LDO
    snt_r00.LDO_EN_IREF   = 0;
    snt_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
#endif
}

void snt_temp_sensor_start(void){

#ifdef __USE_HCODE__
    //snt_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,0x00092A);

    //snt_r01.TSNS_RESETn  = 1;
    //snt_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,0x000929);
#else
    snt_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    snt_r01.TSNS_RESETn  = 1;
    snt_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
#endif
}

void snt_start_timer(uint32_t wait_time){
    // Newly implemented following the input vector at:
    //  /afs/eecs.umich.edu/vlsida/projects/m3_hdk/layer/SNT/SNTv6/spice/4_wakeup_timer/input.vec
    
#ifdef __USE_HCODE__
    // [6] TMR_SLEEP: 1'h1 -> 1'h0
    //snt_r08.TMR_SLEEP = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x08,0x000020);

    // [5] TMR_ISOLATE: 1'h1 -> 1'h0
    //snt_r08.TMR_ISOLATE = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x08,0x000000);

    // [21] TMR_SELF_EN: 1'h1 -> 1'h0
    //snt_r09.TMR_SELF_EN = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x09,0x4880FF);

    // [3] TMR_EN_OSC: 1'h0 -> 1'h1
    //snt_r08.TMR_EN_OSC = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,0x000008);

    // Release Reset 
    // [4] TMR_RESETB: 1'h0 -> 1'h1
    // [2] TMR_RESETB_DIV: 1'h0 -> 1'h1
    // [1] TMR_RESETB_DCDC: 1'h0 -> 1'h1
    //snt_r08.TMR_RESETB = 0x1;
    //snt_r08.TMR_RESETB_DIV = 0x1;
    //snt_r08.TMR_RESETB_DCDC = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,0x00001E);

    // Delay (must be >400ms at RT)
    delay(wait_time);

    // [0] TMR_EN_SELF_CLK: 1'h0 -> 1'h1
    //snt_r08.TMR_EN_SELF_CLK = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,0x00001F);

    // [21] TMR_SELF_EN: 1'h0 -> 1'h1
    //snt_r09.TMR_SELF_EN = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x09,0x6880FF);

    // [3] TMR_EN_OSC: 1'h1 -> 1'h0
    //snt_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,0x000017);
#else
    // [6] TMR_SLEEP: 1'h1 -> 1'h0
    snt_r08.TMR_SLEEP = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // [5] TMR_ISOLATE: 1'h1 -> 1'h0
    snt_r08.TMR_ISOLATE = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // [21] TMR_SELF_EN: 1'h1 -> 1'h0
    snt_r09.TMR_SELF_EN = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x09,snt_r09.as_int);

    // [3] TMR_EN_OSC: 1'h0 -> 1'h1
    snt_r08.TMR_EN_OSC = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // Release Reset 
    // [4] TMR_RESETB: 1'h0 -> 1'h1
    // [2] TMR_RESETB_DIV: 1'h0 -> 1'h1
    // [1] TMR_RESETB_DCDC: 1'h0 -> 1'h1
    snt_r08.TMR_RESETB = 0x1;
    snt_r08.TMR_RESETB_DIV = 0x1;
    snt_r08.TMR_RESETB_DCDC = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // Delay (must be >400ms at RT)
    delay(wait_time);

    // [0] TMR_EN_SELF_CLK: 1'h0 -> 1'h1
    snt_r08.TMR_EN_SELF_CLK = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // [21] TMR_SELF_EN: 1'h0 -> 1'h1
    snt_r09.TMR_SELF_EN = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x09,snt_r09.as_int);

    // [3] TMR_EN_OSC: 1'h1 -> 1'h0
    snt_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);
#endif
}

void snt_stop_timer(void){
#ifdef __USE_HCODE__
    //snt_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    //snt_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
    //snt_r08.TMR_RESETB = 0x0;// Default : 0x0
    //snt_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
    //snt_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,0x000000);

    snt_r17.WUP_ENABLE = 0x0; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,snt_r17.as_int);

	// New for SNTv3
	//snt_r08.TMR_SLEEP = 0x1; // Default : 0x1
	//snt_r08.TMR_ISOLATE = 0x1; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x08,0x000060);
#else
    // EN_OSC
    snt_r08.TMR_EN_OSC = 0x0; // Default : 0x0

    // RESET
    snt_r08.TMR_EN_SELF_CLK = 0x0; // Default : 0x0
    snt_r08.TMR_RESETB = 0x0;// Default : 0x0
    snt_r08.TMR_RESETB_DIV = 0x0; // Default : 0x0
    snt_r08.TMR_RESETB_DCDC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    snt_r17.WUP_ENABLE = 0x0; // Default : 0x
    mbus_remote_register_write(SNT_ADDR,0x17,snt_r17.as_int);

	// New for SNTv3
	snt_r08.TMR_SLEEP = 0x1; // Default : 0x1
	snt_r08.TMR_ISOLATE = 0x1; // Default : 0x1
	mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);
#endif
}

void snt_restart_timer(void) {
    // Stop the SNT Timer
    snt_stop_timer();
    delay(DLY_1S);
    // Turn on the SNT timer clock
    snt_start_timer(/*wait_time*/DLY_1S);
    // Start the SNT counter
    snt_enable_wup_timer(/*auto_reset*/0);

}

uint32_t snt_set_timer_threshold(uint32_t threshold) {

    mbus_remote_register_write(SNT_ADDR,0x19,(0x10<<16)|(WP1_TARGET_REG_IDX<<8)|((threshold>>24)&0xFF));
    enable_reg_irq(WP1_TARGET_REG_IDX);
    mbus_remote_register_write(SNT_ADDR,0x1A,threshold & 0xFFFFFF);
    WFI();
    // NOTE: In normal operation, 'Threshold Update' returns the lower 24-bit of the data and it gets written to WP1_TARGET_REG_IDX.

    return threshold;

}

void snt_enable_wup_timer (uint32_t auto_reset) {
    snt_r17.WUP_AUTO_RESET = auto_reset;
    snt_r17.WUP_ENABLE = 0x1;
    mbus_remote_register_write(SNT_ADDR,0x17,snt_r17.as_int);
}

void snt_disable_wup_timer (void) {
    snt_r17.WUP_ENABLE = 0x0;
    mbus_remote_register_write(SNT_ADDR,0x17,snt_r17.as_int);
}

uint32_t snt_set_wup_timer(uint32_t auto_reset, uint32_t threshold) {
    snt_enable_wup_timer(auto_reset);
    return snt_set_timer_threshold(/*threshold*/threshold);
}

uint32_t snt_sync_read_wup_timer(void) {

    uint32_t result;

    enable_reg_irq(WP1_TARGET_REG_IDX);

    mbus_remote_register_write(SNT_ADDR,0x14,
                             (0x0  << 16)   // 0: Synchronous 32-bit; 1: Synchronous lower 24-bit, 2: Synchronous Upper 8-bit, 3: Invalid
                            |(0x10 <<  8)   // MBus Target Address
                            |(WP0_TARGET_REG_IDX << 0)   // Destination Register ID
                            );
    WFI();

    result = ((*WP0_TARGET_REG_ADDR << 24) | *WP1_TARGET_REG_ADDR);

    #ifdef DEVEL
        mbus_write_message32(0x8E, result);
    #endif

    return result;
}

uint32_t snt_read_wup_timer(void) {

    //----------------------------------------
    // GETTING THE SAME VALUES TWICE CONSECUTIVELY
    //----------------------------------------

    // Get the current reading
    uint32_t temp = snt_sync_read_wup_timer();
    uint32_t temp_prev;
    uint32_t num_stable_reads = 1;
    uint32_t num_try = 0;

    while ((num_stable_reads < 2) && (num_try<SNT_WUP_READ_MAX_TRY)) {
        temp_prev = temp;
        temp = snt_sync_read_wup_timer();
        if ((temp - temp_prev) < SNT_WUP_READ_MAX_ERROR) num_stable_reads++;
        else num_stable_reads=1;
        num_try++;
    }

    return temp;
}

//*******************************************************************
// RDC FUNCTIONS
//*******************************************************************

void rdc_enable_vref(){
	rdc_r2C.RDC_ENb_PG_VREF = 1;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdc_r2C.as_int);
}

void rdc_disable_vref(){
	rdc_r2C.RDC_ENb_PG_VREF = 0;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdc_r2C.as_int);
}

void rdc_disable_pg_v1p2(){
	rdc_r2C.RDC_EN_PG_FSM = 0;
	rdc_r2C.RDC_EN_PG_RC_OSC = 0;
	rdc_r2C.RDC_EN_PG_AMP_V1P2 = 0;
	rdc_r2C.RDC_EN_PG_ADC_V1P2 = 0;
	rdc_r2C.RDC_EN_PG_BUF_VH_V1P2 = 0;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdc_r2C.as_int);
}

void rdc_enable_pg_v1p2(){
	rdc_r2C.RDC_EN_PG_FSM = 1;
	rdc_r2C.RDC_EN_PG_RC_OSC = 1;
	rdc_r2C.RDC_EN_PG_AMP_V1P2 = 1;
	rdc_r2C.RDC_EN_PG_ADC_V1P2 = 1;
	rdc_r2C.RDC_EN_PG_BUF_VH_V1P2 = 1;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdc_r2C.as_int);
}

void rdc_disable_pg_vbat(){
	rdc_r2C.RDC_ENb_PG_AMP_VBAT = 1;
	rdc_r2C.RDC_ENb_PG_ADC_VBAT = 1;
	rdc_r2C.RDC_ENb_PG_BUF_VCM = 1;
	rdc_r2C.RDC_ENb_PG_BUF_VH_VBAT = 1;
	rdc_r2C.RDC_ENb_MIRROR_LDO = 0;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdc_r2C.as_int);
}

void rdc_enable_pg_vbat(){
	rdc_r2C.RDC_ENb_PG_AMP_VBAT = 0;
	rdc_r2C.RDC_ENb_PG_ADC_VBAT = 0;
	rdc_r2C.RDC_ENb_PG_BUF_VCM = 0;
	rdc_r2C.RDC_ENb_PG_BUF_VH_VBAT = 0;
	rdc_r2C.RDC_ENb_MIRROR_LDO = 1;
	mbus_remote_register_write(RDC_ADDR,0x2C,rdc_r2C.as_int);
}

void rdc_disable_clock(){
	rdc_r28.RDC_RESET_RC_OSC = 1;
	rdc_r2B.RDC_CLK_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,0x28,rdc_r28.as_int);
	mbus_remote_register_write(RDC_ADDR,0x2B,rdc_r2B.as_int);
}

void rdc_enable_clock(){
	rdc_r28.RDC_RESET_RC_OSC = 0;
	rdc_r2B.RDC_CLK_ISOLATE = 0;
	mbus_remote_register_write(RDC_ADDR,0x28,rdc_r28.as_int);
	mbus_remote_register_write(RDC_ADDR,0x2B,rdc_r2B.as_int);
}

uint32_t rdc_start_meas(){

    // Enable REG IRQ
    enable_reg_irq(RDC_TARGET_REG_IDX); // REG2

	rdc_r20.RDC_RESETn_FSM = 1;
	rdc_r20.RDC_ISOLATE = 0;
	mbus_remote_register_write(RDC_ADDR,0x20,rdc_r20.as_int);

    WFI();

	rdc_reset();

    return (*RDC_TARGET_REG_ADDR) & 0x1FFFF;
}

void rdc_reset(){
	rdc_r20.RDC_RESETn_FSM = 0;
	rdc_r20.RDC_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,0x20,rdc_r20.as_int);
}

void rdc_reset2(){
	rdc_r20.RDC_ISOLATE = 1;
	mbus_remote_register_write(RDC_ADDR,0x20,rdc_r20.as_int);
}

void rdc_power_on(){

	rdc_reset();
	rdc_disable_pg_v1p2();
	delay(200);

	rdc_disable_pg_vbat();
	delay(200);

	rdc_enable_clock();
}

void rdc_power_off(){

	rdc_reset2();
	rdc_disable_clock();
	rdc_enable_pg_vbat();
	rdc_enable_pg_v1p2();
	rdc_disable_vref();  

};

void rdc_init(void) {

    // Regiser File variables
    rdc_r20.as_int = RDC_R20_DEFAULT_AS_INT;
    rdc_r25.as_int = RDC_R25_DEFAULT_AS_INT;
    rdc_r28.as_int = RDC_R28_DEFAULT_AS_INT;
    rdc_r2B.as_int = RDC_R2B_DEFAULT_AS_INT;
    rdc_r2C.as_int = RDC_R2C_DEFAULT_AS_INT;

}

