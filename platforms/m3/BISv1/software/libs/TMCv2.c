//*******************************************************************************************
// TMC SOURCE FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//  PREv23E -> SNTv6r1 -> MRRv11A -> MEMv3 -> LNTv2A -> PMUv13r2
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*********************************************************
// HEADER AND OTHER SOURCE FILES
//*********************************************************
#include "TMCv2.h"


//*******************************************************************
// TMCv1 FUNCTIONS
//*******************************************************************
// SEE HEADER FILE FOR FUNCTION BRIEFS


//-------------------------------------------------------------------
// Basic Building Functions
//-------------------------------------------------------------------

uint32_t set_bits (uint32_t original_var, uint32_t msb_idx, uint32_t lsb_idx, uint32_t value) {
    uint32_t mask_pattern = (0xFFFFFFFF << (msb_idx + 1)) | (0xFFFFFFFF >> (32 - lsb_idx));
    return (original_var & mask_pattern) | ((value << lsb_idx) & ~mask_pattern);
}

uint32_t set_bit (uint32_t original_var, uint32_t idx, uint32_t value) {
    if (value & 0x1) return original_var |  (0x1 << idx);
    else             return original_var & ~(0x1 << idx);
}

uint32_t get_bits (uint32_t variable, uint32_t msb_idx, uint32_t lsb_idx) {
    return ((variable << (31-msb_idx)) >> (31-msb_idx)) >> lsb_idx;
}

uint32_t get_bit (uint32_t variable, uint32_t idx) {
    return get_bits(variable, idx, idx);
}

//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

//-------------------------------------------------------------------
// PMU Floor Loop-Up Tables
//-------------------------------------------------------------------

// ACTIVE
volatile union pmu_floor __pmu_floor_active__[] = {
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
volatile union pmu_floor __pmu_floor_sleep__[] = {
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
volatile union pmu_floor __pmu_floor_sleep_tsnt__[] = {
    //  Temp Range        idx     R    L    BASE  L_SAR
    /*       ~PMU_10C*/ /* 0*/  {{0xF, 0xA, 0x07, 0xF}},
    /*PMU_20C~PMU_25C*/ /* 1*/  {{0xF, 0xA, 0x05, 0xF}},
    /*PMU_35C~PMU_55C*/ /* 2*/  {{0xA, 0xA, 0x05, 0xF}},
    /*PMU_75C~       */ /* 3*/  {{0x5, 0xA, 0x05, 0xF}},
    };

// SLEEP (LOW)
volatile union pmu_floor __pmu_floor_sleep_low__[] = {
    //  idx     R    L    BASE  L_SAR
      /* 0*/  {{0xF, 0x0, 0x01, 0x1}},
      };

// SLEEP (RADIO)
volatile union pmu_floor __pmu_floor_sleep_radio__[] = {
    //  idx     R    L    BASE  L_SAR
      /* 0*/  {{0xF, 0xA, 0x05, 0xF}},
      };

//-------------------------------------------------------------------
// PMU Functions
//-------------------------------------------------------------------

void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data) {
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    set_halt_until_mbus_tx();
}

void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    pmu_reg_write_core(reg_addr, reg_data);
}

uint32_t pmu_reg_read (uint32_t reg_addr) {
    pmu_reg_write_core(0x00, reg_addr);
    return *PMU_TARGET_REG_ADDR;
}

void pmu_config_rat(uint8_t rat) {

    //---------------------------------------------------------------------------------------
    // RAT_CONFIG
    // NOTE: Unless overridden, changes in RAT_CONFIG[19:13] do not take effect
    //       until the next sleep or wakeup-up transition.
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x47, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (rat << 19)   // 1'h0     // Enable RAT
        | (0   << 18)   // 1'h0     // Enable SAR Slow-Loop in Active (NOTE: When RAT is enabled, SAR Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (0   << 17)   // 1'h0     // Enable UPC Slow-Loop in Active (NOTE: When RAT is enabled, UPC Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (0   << 16)   // 1'h0     // Enable DNC Slow-Loop in Active (NOTE: When RAT is enabled, DNC Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (rat << 15)   // 1'h0     // Enable SAR Slow-Loop in Sleep
        | (rat << 14)   // 1'h0     // Enable UPC Slow-Loop in Sleep
        | (rat << 13)   // 1'h0     // Enable DNC Slow-Loop in Sleep
        | (1   << 11)   // 2'h0     // Clock Ring Tuning (0x0: 13 stages; 0x1: 11 stages; 0x2: 9 stages; 0x3: 7 stages)
        | (4   << 8 )   // 3'h0     // Clock Divider Tuning for SAR Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (4   << 5 )   // 3'h0     // Clock Divider Tuning for UPC Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (4   << 2 )   // 3'h0     // Clock Divider Tuning for DNC Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (3   << 0 )   // 2'h0     // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up
    ));

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

void pmu_set_active_min(){
    pmu_set_floor(PMU_ACTIVE, 0x1, 0x0, 0x1, 0x0);
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

void pmu_set_sar_ratio (uint32_t ratio) {
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

    delay(10000); // ~0.4s with 140kHz CPU clock
}

uint32_t pmu_get_sar_ratio (void) {
    return __pmu_sar_ratio__;
}

void pmu_set_adc_period(uint32_t val){
    //---------------------------------------------------
    // TICK_REPEAT_VBAT_ADJUST
    //---------------------------------------------------
    pmu_reg_write(0x36, val); 

    //---------------------------------------------------
    // TICK_ADC_RESET
    //---------------------------------------------------
    pmu_reg_write(0x33, __PMU_TICK_ADC_RESET__);

    //---------------------------------------------------
    // TICK_ADC_CLK
    //---------------------------------------------------
    pmu_reg_write(0x34, __PMU_TICK_ADC_CLK__);
}

uint32_t pmu_read_adc(void) {
    return pmu_reg_read(0x03) & 0xFF;
}

uint32_t pmu_validate_adc_val (uint32_t adc_val) {
    return ((adc_val > __PMU_ADC_LOWER_LIMIT__) && (adc_val < __PMU_ADC_UPPER_LIMIT__));
}

uint32_t pmu_calc_new_sar_ratio(uint32_t adc_val, uint32_t offset, uint32_t num_cons_meas) {
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
        uint32_t hyst = 3; // hysteresis
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

uint32_t pmu_check_low_vbat (uint32_t adc_val, uint32_t offset, uint32_t threshold, uint32_t num_cons_meas) {
    if (pmu_validate_adc_val(adc_val)) {
        if (adc_val < (threshold + offset)) {
            if (__pmu_low_vbat_streak__ < num_cons_meas) __pmu_low_vbat_streak__++;
        }
        else {
            __pmu_low_vbat_streak__ = 0;
        }
    }
    return (__pmu_low_vbat_streak__>=num_cons_meas);
}

uint32_t pmu_check_crit_vbat (uint32_t adc_val, uint32_t offset, uint32_t threshold, uint32_t num_cons_meas) {
    if (pmu_validate_adc_val(adc_val)) {
        if (adc_val < (threshold + offset)) {
            if (__pmu_crit_vbat_streak__ < num_cons_meas) __pmu_crit_vbat_streak__++;
        }
        else {
            __pmu_crit_vbat_streak__ = 0;
        }
    }
    return (__pmu_crit_vbat_streak__>=num_cons_meas);
}

void pmu_init(void){

    #ifdef __PMU_CHECK_WRITE__ // See PMU Behavior section at the top)
        //[5]: PMU_CHECK_WRITE(1'h0); [4]: PMU_IRQ_EN(1'h1); [3:2]: LC_CLK_DIV(2'h3); [1:0]: LC_CLK_RING(2'h1)
        mbus_remote_register_write(PMU_ADDR, 0x51, (0x1 << 5) | (0x1 << 4) | (0x3 << 2) | (0x1 << 0));
    #endif

    // Initialize PMU State Variables
    __pmu_desired_state_sleep__.value  = __PMU_DESIRED_STATE_SLEEP_DEFAULT__;
    __pmu_desired_state_active__.value = __PMU_DESIRED_STATE_ACTIVE_DEFAULT__;
    __pmu_stall_state_sleep__.value    = __PMU_STALL_STATE_SLEEP_DEFAULT__;
    __pmu_stall_state_active__.value   = __PMU_STALL_STATE_ACTIVE_DEFAULT__;

    // Initialized the temperature to 25C and adjust floors
    __pmu_temp_state__ = PMU_25C;
    pmu_set_active_temp_based();
    pmu_set_sleep_temp_based();

    // Initialize other PMU variables
    __pmu_sar_ratio_upper_streak__  = 0;
    __pmu_sar_ratio_lower_streak__  = 0;
    __pmu_low_vbat_streak__         = 0;
    __pmu_crit_vbat_streak__        = 0;

    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x15, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0x0 << 19)   // 0x0      // Enable PFM even during periodic reset
        | (0x0 << 18)   // 0x0      // Enable PFM even when VREF is not used as reference
        | (0x0 << 17)   // 0x0      // Enable PFM
        | (0x1 << 14)   // 0x3      // Comparator clock division ratio
        | (0x0 << 13)   // 0x0      // Makes the converter clock 2x slower
        | (0xF <<  9)   // 0x2      // Frequency multiplier R
        | (0x0 <<  5)   // 0x2      // Frequency multiplier L
        | (0x01)        // 0x6      // Floor frequency base
        ));
    
    // Initialize SAR Ratio
    pmu_set_sar_ratio(0x4C); // See pmu_calc_new_sar_ratio()

    // Disable ADC in Active
    // PMU ADC will be automatically reset when system wakes up
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

    // Set ADC Period
    pmu_set_adc_period(__PMU_ADC_PERIOD__);

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
        | (0x1 << 11)   // 0x1      // If 1, 'horizon' enqueues 'vbat_read_only'
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

    // Enable Slow loop
    //---------------------------------------------------------------------------------------
    // SAR_TRIM_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x0F, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0x0 << 16)   // 0x0      // Disable reverse configuration for upconversion
        | (0x0 << 14)   // 0x2      // Reserved
        | (0x1 << 13)   // 0x1      // Enable [12]
        | (0x1 << 12)   // 0x0      // (Valid if [13]) Use VREF as frequency controller reference
        | (0x0 <<  4)   // 0x20     // Reserved
        | (0x4 <<  0)   // 0x4      // Bias current (current is proportional to the value)
    ));

    // Enables RAT (it won't take effect until the next sleep or wakeup-up transition)
    pmu_config_rat(1);

    // Maximize the sleep delay: This would add ~80ms delay before turning on the UDC/DNC slow loop.
    //---------------------------------------------------------------------------------------
    // TICK_SLEEP_WAIT
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x38, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0xFFF << 0)  // 0x1E     // (12-bit) Delay before turning on UPC/DNC slow loop
    ));

}

//*******************************************************************
// NFC FUNCTIONS
//*******************************************************************

void nfc_init(void){

    // Initialize passwords; Factory default: I2C_PWD[63:0]=0x0
    __i2c_password_upper__ = 0x0;
    __i2c_password_lower__ = 0x0;

    // Initialize the status
    __nfc_on__ = 0x0;
    __nfc_i2c_token__ = 0x0;
    __nfc_i2c_timeout__ = 0x0;

    // Initialize the direction (*R_GPIO_DIR needs to be explicitly reset)
    gpio_set_dir(0x0);

    // Enable the GPIO pads
    set_gpio_pad_with_mask(__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    set_gpio_pad_with_mask(__GPO_MASK__,(1<<__NFC_GPO__));

    // At this point, SCL/SDA are set to 'input' and TMC does not driving them.
    // They should be properly configured in nfc_power_on().

    // Config GPO wakeup (NOTE: it does not work in TMCv1/PREv22E)
    //config_gpio_posedge_wirq((1<<__NFC_GPO__));

}

void nfc_power_on(void) {
    if (!__nfc_on__) {
        // Set directions (1: output, 0: input); Default direction is 0 (input).
        gpio_set_dir_with_mask(__I2C_MASK__,(1<<__NFC_SCL__)|(1<<__NFC_SDA__));

        // Write SCL=SDA=1
        gpio_write_data_with_mask(__I2C_MASK__,(1<<__NFC_SCL__)|(1<<__NFC_SDA__));

        // Power the NFC chip
        // NOTE: At this point, SCL/SDA are still in 'input' mode as GPIO pads are still frozen.
        //       As VNFC goes high, SDA also goes high by the pull-up resistor. SCL remains low.
        set_cps(0x1 << __NFC_CPS__);

        // Unfreeze the pads
        unfreeze_gpio_out();

        // Update the status flag
        __nfc_on__ = 0x1;

        // Provide delay to meeting the boot-up time constraint.
        delay(__NFC_TBOOTDC__);
    }
}

void nfc_power_off(void) {
    if (__nfc_on__) {
        // Power off the NFC chip
        set_cps(0x0 << __NFC_CPS__);
        // Write SCL=SDA=0
        gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SCL__)|(0<<__NFC_SDA__));
        // Set direction (Direction- 1: output, 0: input)
        gpio_set_dir_with_mask(__I2C_MASK__,(0<<__NFC_SCL__)|(0<<__NFC_SDA__));
        // Freeze the pads
        freeze_gpio_out();
        // Update the status flag
        __nfc_on__ = 0x0;
    }
}

uint32_t nfc_i2c_get_timeout(void) {
    return __nfc_i2c_timeout__;
}

void nfc_i2c_start(void) {
    __nfc_i2c_timeout__ = 0;
    nfc_power_on();
#ifdef __USE_FAST_I2C__
    *GPIO_DATA = 0x06; // SDA=1, SCL=1
    *GPIO_DIR  = 0x06; // SDA=output(1), SCL=output(1);
    *GPIO_DATA = 0x02; // SDA=0, SCL=1
    *GPIO_DATA = 0x00; // SDA=0, SCL=0
    // update the retentive memory
    *R_GPIO_DIR  = 0x06;
    *R_GPIO_DATA = 0x00;
#else
    gpio_write_data_with_mask(__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_set_dir_with_mask   (__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(0<<__NFC_SCL__));
#endif
}

void nfc_i2c_stop(void) {
#ifdef __USE_FAST_I2C__
    *GPIO_DATA = 0x00; // SDA=0, SCL=0
    *GPIO_DIR  = 0x06; // SDA=output(1), SCL=output(1);
    *GPIO_DATA = 0x02; // SDA=0, SCL=1
    *GPIO_DATA = 0x06; // SDA=1, SCL=1
    // update the retentive memory
    *R_GPIO_DIR  = 0x06;
    *R_GPIO_DATA = 0x06;
#else
    gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(0<<__NFC_SCL__));
    gpio_set_dir_with_mask   (__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_write_data_with_mask(__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
#endif
}

void nfc_i2c_wait_for_ack(void) {
    set_timeout32_check(__I2C_ACK_TIMEOUT__);
    while(((*GPIO_DATA>>__NFC_SDA__)&0x1) && !__wfi_timeout_flag__);
    __nfc_i2c_timeout__ = !stop_timeout32_check(__FCODE_I2C_ACK_TIMEOUT__);
}

void nfc_i2c_byte(uint8_t byte){
    if (!__nfc_i2c_timeout__) {
    #ifdef __USE_FAST_I2C__
        uint32_t i = 7;
        *GPIO_DIR  = 0x06; // SDA=output(1), SCL=output(1);
        while(i>0) {
            if ((byte&0x80)!=0) {
                *GPIO_DATA = 0x04; // SDA=1, SCL=0
                *GPIO_DATA = 0x06; // SDA=1, SCL=1
                *GPIO_DATA = 0x04; // SDA=1, SCL=0
            } else {
                *GPIO_DATA = 0x00; // SDA=0, SCL=0
                *GPIO_DATA = 0x02; // SDA=0, SCL=1
                *GPIO_DATA = 0x00; // SDA=0, SCL=0
            }
            byte = byte << 1;
            i--;
        }
    
        // Send byte[0]
        if ((byte&0x80)!=0) { *GPIO_DIR  = 0x02; } // SDA=input(0), SCL=output(1);
        *GPIO_DATA = 0x00; // SDA=z, SCL=0
        *GPIO_DATA = 0x02; // SDA=z, SCL=1
        *GPIO_DATA = 0x00; // SDA=z, SCL=0
        if ((byte&0x80)!=0) { *GPIO_DIR  = 0x02; } // SDA=input(0), SCL=output(1);
    
        //Wait for ACK
        nfc_i2c_wait_for_ack();
    
        *GPIO_DATA = 0x02; // SDA=z, SCL=1
        *GPIO_DATA = 0x00; // SDA=z, SCL=0
    
        // update the retentive memory
        *R_GPIO_DIR  = 0x02;
        *R_GPIO_DATA = 0x00;
    
    #else
        uint32_t i;
    
        // Direction: SCL (output), SDA (output)
        gpio_set_dir_with_mask(__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    
        // Send byte[7:1]
        for (i=7; i>0; i--) {
            gpio_write_data_with_mask(__I2C_MASK__,(((byte>>i)&0x1)<<__NFC_SDA__)|(0<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(((byte>>i)&0x1)<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(((byte>>i)&0x1)<<__NFC_SDA__)|(0<<__NFC_SCL__));
        }
        // Send byte[0]
        if (byte&0x1) {
            // Direction: SCL (output), SDA (input)
            gpio_set_dir_with_mask   (__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);
            gpio_write_data_with_mask(__I2C_MASK__,1<<__NFC_SCL__);
            gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);
        }
        else {
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(0<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(0<<__NFC_SCL__));
            // Direction: SCL (output), SDA (input)
            gpio_set_dir_with_mask   (__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
        }
    
        //Wait for ACK
        nfc_i2c_wait_for_ack();
    
        gpio_write_data_with_mask(__I2C_MASK__,1<<__NFC_SCL__);
        gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);
    
    #endif
    }
}

uint32_t nfc_i2c_byte_imm (uint8_t byte){
    uint32_t no_ack;

    if (!__nfc_i2c_timeout__) {

    #ifdef __USE_FAST_I2C__
    
        *GPIO_DIR  = 0x06; // SDA=output(1), SCL=output(1);
        uint32_t i = 7;
        while(i>0) {
            if ((byte&0x80)!=0) {
                *GPIO_DATA = 0x04; // SDA=1, SCL=0
                *GPIO_DATA = 0x06; // SDA=1, SCL=1
                *GPIO_DATA = 0x04; // SDA=1, SCL=0
            } else {
                *GPIO_DATA = 0x00; // SDA=0, SCL=0
                *GPIO_DATA = 0x02; // SDA=0, SCL=1
                *GPIO_DATA = 0x00; // SDA=0, SCL=0
            }
            byte = byte << 1;
            i--;
        }
    
        // Send byte[0]
        if ((byte&0x80)!=0) { *GPIO_DIR  = 0x02; } // SDA=input(0), SCL=output(1);
        *GPIO_DATA = 0x00; // SDA=z, SCL=0
        *GPIO_DATA = 0x02; // SDA=z, SCL=1
        *GPIO_DATA = 0x00; // SDA=z, SCL=0
        if ((byte&0x80)!=0) { *GPIO_DIR  = 0x02; } // SDA=input(0), SCL=output(1);
    
        // Check ACK
        *GPIO_DATA = 0x02; // SDA=z, SCL=1
        no_ack = (*GPIO_DATA>>__NFC_SDA__)&0x1;
        *GPIO_DATA = 0x00; // SDA=z, SCL=0
    
        // update the retentive memory
        *R_GPIO_DIR  = 0x02;
        *R_GPIO_DATA = 0x00;
    
    #else
        uint32_t i;
    
        // Direction: SCL (output), SDA (output)
        gpio_set_dir_with_mask(__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    
        // Send byte[7:1]
        for (i=7; i>0; i--) {
            gpio_write_data_with_mask(__I2C_MASK__,(((byte>>i)&0x1)<<__NFC_SDA__)|(0<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(((byte>>i)&0x1)<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(((byte>>i)&0x1)<<__NFC_SDA__)|(0<<__NFC_SCL__));
        }
        // Send byte[0]
        if (byte&0x1) {
            // Direction: SCL (output), SDA (input)
            gpio_set_dir_with_mask   (__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);
            gpio_write_data_with_mask(__I2C_MASK__,1<<__NFC_SCL__);
            gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);
        }
        else {
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(0<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(0<<__NFC_SCL__));
            // Direction: SCL (output), SDA (input)
            gpio_set_dir_with_mask   (__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
        }
        
        // Check ACK
        gpio_write_data_with_mask(__I2C_MASK__,1<<__NFC_SCL__);
        no_ack = (*GPIO_DATA>>__NFC_SDA__)&0x1;
        gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);
    #endif
        return !no_ack;
    }
    else {
        return 0;
    }
}

void nfc_i2c_polling (void) {
    uint32_t done = 0;
    while (!done) {
        nfc_i2c_start();
        done = nfc_i2c_byte_imm(0xA6);
    }
    nfc_i2c_stop();
}

uint32_t nfc_i2c_present_password(void) {
    uint32_t i2c_dsc   = 0xAE;      // Device Select Code
    uint32_t i2c_addr  = 0x0900;    // Address
    nfc_i2c_start();
    nfc_i2c_byte(i2c_dsc);
    nfc_i2c_byte(i2c_addr>>8);
    nfc_i2c_byte(i2c_addr);
    nfc_i2c_byte(__i2c_password_upper__ >> 24);
    nfc_i2c_byte(__i2c_password_upper__ >> 16);
    nfc_i2c_byte(__i2c_password_upper__ >>  8);
    nfc_i2c_byte(__i2c_password_upper__ >>  0);
    nfc_i2c_byte(__i2c_password_upper__ >> 24);
    nfc_i2c_byte(__i2c_password_upper__ >> 16);
    nfc_i2c_byte(__i2c_password_upper__ >>  8);
    nfc_i2c_byte(__i2c_password_upper__ >>  0);
    nfc_i2c_byte(0x09);
    nfc_i2c_byte(__i2c_password_lower__ >> 24);
    nfc_i2c_byte(__i2c_password_lower__ >> 16);
    nfc_i2c_byte(__i2c_password_lower__ >>  8);
    nfc_i2c_byte(__i2c_password_lower__ >>  0);
    nfc_i2c_byte(__i2c_password_lower__ >> 24);
    nfc_i2c_byte(__i2c_password_lower__ >> 16);
    nfc_i2c_byte(__i2c_password_lower__ >>  8);
    nfc_i2c_byte(__i2c_password_lower__ >>  0);
    nfc_i2c_stop();
    return !__nfc_i2c_timeout__;
}

uint32_t nfc_i2c_update_password(uint32_t upper, uint32_t lower) {
    uint32_t i2c_dsc   = 0xAE;      // Device Select Code
    uint32_t i2c_addr  = 0x0900;    // Address
    __i2c_password_upper__ = upper;
    __i2c_password_lower__ = lower;
    nfc_i2c_start();
    nfc_i2c_byte(i2c_dsc);
    nfc_i2c_byte(i2c_addr>>8);
    nfc_i2c_byte(i2c_addr);
    nfc_i2c_byte(__i2c_password_upper__ >> 24);
    nfc_i2c_byte(__i2c_password_upper__ >> 16);
    nfc_i2c_byte(__i2c_password_upper__ >>  8);
    nfc_i2c_byte(__i2c_password_upper__ >>  0);
    nfc_i2c_byte(__i2c_password_upper__ >> 24);
    nfc_i2c_byte(__i2c_password_upper__ >> 16);
    nfc_i2c_byte(__i2c_password_upper__ >>  8);
    nfc_i2c_byte(__i2c_password_upper__ >>  0);
    nfc_i2c_byte(0x07);
    nfc_i2c_byte(__i2c_password_lower__ >> 24);
    nfc_i2c_byte(__i2c_password_lower__ >> 16);
    nfc_i2c_byte(__i2c_password_lower__ >>  8);
    nfc_i2c_byte(__i2c_password_lower__ >>  0);
    nfc_i2c_byte(__i2c_password_lower__ >> 24);
    nfc_i2c_byte(__i2c_password_lower__ >> 16);
    nfc_i2c_byte(__i2c_password_lower__ >>  8);
    nfc_i2c_byte(__i2c_password_lower__ >>  0);
    nfc_i2c_stop();
    return !__nfc_i2c_timeout__;
}

uint8_t nfc_i2c_rd(uint8_t ack){
    if (!__nfc_i2c_timeout__) {
        uint8_t data = 0;

        // Direction: SCL (output), SDA (input)
        gpio_set_dir_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));

        // Read byte[7:0]
        uint32_t i = 7;
        while (1) {
            gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);
            gpio_write_data_with_mask(__I2C_MASK__,1<<__NFC_SCL__);
            data = data | (((*GPIO_DATA>>__NFC_SDA__)&0x1)<<i);
            if (i==0) break;
            else i--;
        }
        gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);

        // Acknowledge
        if (ack) {
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__) | (0<<__NFC_SCL__));
            gpio_set_dir_with_mask   (__I2C_MASK__,(1<<__NFC_SDA__) | (1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__) | (1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__) | (0<<__NFC_SCL__));
            gpio_set_dir_with_mask   (__I2C_MASK__,(0<<__NFC_SDA__) | (1<<__NFC_SCL__));
        }
        else{
            gpio_set_dir_with_mask   (__I2C_MASK__,(0<<__NFC_SDA__) | (1<<__NFC_SCL__));
            gpio_write_data_with_mask(__I2C_MASK__,1<<__NFC_SCL__);
            gpio_write_data_with_mask(__I2C_MASK__,0<<__NFC_SCL__);
        }

        return data;
    }
    else {
        return 0;
    } 
}

uint32_t nfc_i2c_get_token(uint32_t max_num_try) {
    if (!__nfc_i2c_token__) {
        // Disable RF (RF_DISABLE=1) @ RF_MNGT_DYN
        uint32_t num_try = 0;
        while (num_try < max_num_try) {
            nfc_i2c_start();
            if (nfc_i2c_byte_imm(0xA6)) {
                nfc_i2c_byte((ST25DV_ADDR_RF_MNGT_DYN>>8)&0xFF);
                nfc_i2c_byte(ST25DV_ADDR_RF_MNGT_DYN&0xFF);
                nfc_i2c_byte(0x1<<0);
                nfc_i2c_stop();
                // If timeout occurs
                if (__nfc_i2c_timeout__) {
                    return 0;
                }
                // If everything went well, get the token
                else {
                    nfc_i2c_polling();
                    __nfc_i2c_token__ = 1;
                    return 1;
                }
            }
            nfc_i2c_stop();
            num_try++;

            // Some delay before the next attempt.
            delay(__NFC_TTOKEN_RETRY__);
        }
        // Number of Trials exceeds
        return 0;
    }
    // You already got the token.
    else {
        return 1;
    }
}

uint32_t nfc_i2c_release_token(void) {
    if (__nfc_i2c_token__) {
        // Enable RF (RF_DISABLE=0) @ RF_MNGT_DYN
        if (nfc_i2c_byte_write(/*e2*/0, /*addr*/ ST25DV_ADDR_RF_MNGT_DYN, /*data*/ (0x0 << 0), /*nb*/ 1)) {
            __nfc_i2c_token__ = 0;
            return 1;
        }
        // Not Acked
        else {
            return 0;
        }
    }
    // There was no token
    else {
        return 1;
    }
}

uint32_t nfc_i2c_byte_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t nb){
    // In order to reduce the code space,
    // this function does not generate an error when 'nb' is out of the valid range.
    nfc_i2c_start();
    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
    nfc_i2c_byte(addr>>8);
    nfc_i2c_byte(addr);
    //-------------------------------------------------
              nfc_i2c_byte((data>> 0)&0xFF);
    if (nb>1) nfc_i2c_byte((data>> 8)&0xFF);
    if (nb>2) nfc_i2c_byte((data>>16)&0xFF);
    if (nb>3) nfc_i2c_byte((data>>24)&0xFF);
    //-------------------------------------------------
    nfc_i2c_stop();
    // If timeout occurs
    if (__nfc_i2c_timeout__) return 0;
    // If everything went well
    else {
        nfc_i2c_polling();
        return 1;
    }
}

uint32_t nfc_i2c_byte_read(uint32_t e2, uint32_t addr, uint32_t nb){
    // In order to reduce the code space,
    // this function does not generate an error when 'nb' is out of the valid range.
    uint32_t data;
    nfc_i2c_start();
    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
    nfc_i2c_byte(addr >> 8);
    nfc_i2c_byte(addr);
    nfc_i2c_start();
    nfc_i2c_byte(0xA7 | ((e2&0x1) << 3));
    //-------------------------------------------------
               data  =  nfc_i2c_rd(!(nb==1));
    if (nb>1)  data |= (nfc_i2c_rd(!(nb==2)) << 8);
    if (nb>2)  data |= (nfc_i2c_rd(!(nb==3)) << 16);
    if (nb>3)  data |= (nfc_i2c_rd(!(nb==4)) << 24);
    //-------------------------------------------------
    nfc_i2c_stop();
    // If timeout occurs
    if (__nfc_i2c_timeout__) return 0;
    // If everything went well
    else {
        return data;
    }
}

uint32_t nfc_i2c_seq_byte_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t nb){
    // In order to reduce the code space,
    // this function does not generate an error when 'nb' is out of the valid range.
    uint32_t i;
    uint32_t s;
    nfc_i2c_start();
    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
    nfc_i2c_byte(addr>>8);
    nfc_i2c_byte(addr);
    s=0;
    for (i=0; i<nb; i++) {
        if(s==4) s=0;
        if      (s==0) nfc_i2c_byte((data[i]>> 0)&0xFF);
        else if (s==1) nfc_i2c_byte((data[i]>> 8)&0xFF);
        else if (s==2) nfc_i2c_byte((data[i]>>16)&0xFF);
        else if (s==3) nfc_i2c_byte((data[i]>>24)&0xFF);
        s++;
    }
    nfc_i2c_stop();
    // If timeout occurs
    if (__nfc_i2c_timeout__) return 0;
    // If everything went well
    else {
        nfc_i2c_polling();
        return 1;
    }
}

uint32_t nfc_i2c_word_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t nw){
    // In order to reduce the code space,
    // this function does not generate an error when 'nw' is out of the valid range.
    uint32_t i, j;
    uint32_t word;
    addr &= 0xFFFC; // Force the 2 LSBs to 0 to make it word-aligned.
    nfc_i2c_start();
    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
    nfc_i2c_byte(addr>>8);
    nfc_i2c_byte(addr);
    for (i=0; i<nw; i++) {
        word = data[i];
        for (j=0; j<4; j++) {
            nfc_i2c_byte(word&0xFF);
            word = word>>8;
        }
    }
    nfc_i2c_stop();
    // If timeout occurs
    if (__nfc_i2c_timeout__) return 0;
    // If everything went well
    else {
        nfc_i2c_polling();
        return 1;
    }
}

uint32_t nfc_i2c_word_pattern_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t nw){
    // In order to reduce the code space,
    // this function does not generate an error when 'nw' is out of the valid range.
    uint32_t i;
    addr &= 0xFFFC; // Force the 2 LSBs to 0 to make it word-aligned.
    nfc_i2c_start();
    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
    nfc_i2c_byte(addr>>8);
    nfc_i2c_byte(addr);
    for (i=0; i<nw; i++) {
        nfc_i2c_byte((data>> 0)&0xFF);
        nfc_i2c_byte((data>> 8)&0xFF);
        nfc_i2c_byte((data>>16)&0xFF);
        nfc_i2c_byte((data>>24)&0xFF);
    }
    nfc_i2c_stop();
    // If timeout occurs
    if (__nfc_i2c_timeout__) return 0;
    // If everything went well
    else {
        nfc_i2c_polling();
        return 1;
    }
}


//*******************************************************************
// EID FUNCTIONS
//*******************************************************************

void eid_init(uint32_t ring, uint32_t te_div, uint32_t fd_div, uint32_t seg_div){

    __eid_current_display__     = 0;
    __eid_tmr_sel_ldo__         = 1;
    __eid_tmr_init_delay__      = 120000;
    __eid_cp_duration__         = 250;
    __eid_cp_fe_duration__      = 125;
    __eid_clear_fd__            = 1;
    __eid_vin__                 = 0;
    __crsh_wd_thrshld__         = 360000;
    __crsh_cp_idle_width__      = 400;
    __crsh_cp_pulse_width__     = 400;
    __crsh_sel_seq__            = 0x7;
    __crsh_seqa_vin__           = 0x0;
    __crsh_seqa_ck_te__         = 0x0;
    __crsh_seqa_ck_fd__         = 0x1;
    __crsh_seqa_ck_seg__        = 0x1FF;
    __crsh_seqb_vin__           = 0x0;
    __crsh_seqb_ck_te__         = 0x1;
    __crsh_seqb_ck_fd__         = 0x0;
    __crsh_seqb_ck_seg__        = 0x000;
    __crsh_seqc_vin__           = 0x0;
    __crsh_seqc_ck_te__         = 0x0;
    __crsh_seqc_ck_fd__         = 0x0;
    __crsh_seqc_ck_seg__        = 0x018;

    eid_r00.as_int = EID_R00_DEFAULT_AS_INT;
    eid_r01.as_int = EID_R01_DEFAULT_AS_INT;
    eid_r02.as_int = EID_R02_DEFAULT_AS_INT;
    eid_r07.as_int = EID_R07_DEFAULT_AS_INT;
    eid_r09.as_int = EID_R09_DEFAULT_AS_INT;
    eid_r10.as_int = EID_R10_DEFAULT_AS_INT;
    eid_r11.as_int = EID_R11_DEFAULT_AS_INT;
    eid_r12.as_int = EID_R12_DEFAULT_AS_INT;
    eid_r13.as_int = EID_R13_DEFAULT_AS_INT;
    eid_r15.as_int = EID_R15_DEFAULT_AS_INT;
    eid_r16.as_int = EID_R16_DEFAULT_AS_INT;
    eid_r17.as_int = EID_R17_DEFAULT_AS_INT;

    // Start EID Timer 
    eid_enable_timer();

    // Charge Pump Clock Generator
	eid_config_cp_clk_gen(/*ring*/ring, /*te_div*/te_div, /*fd_div*/fd_div, /*seg_div*/seg_div);

    // Charge Pump Pulse Width
    eid_set_duration(__eid_cp_duration__);

    // Configuration for Crash Behavior (See comment section in the top)
    eid_r10.WCTR_THRESHOLD = __crsh_wd_thrshld__;
    mbus_remote_register_write(EID_ADDR,0x10,eid_r10.as_int);

    eid_r11.WCTR_CR_ECP_SEL_SEG_DIV     = seg_div;
    eid_r11.WCTR_CR_ECP_SEL_FD_DIV      = fd_div;
    eid_r11.WCTR_CR_ECP_SEL_TE_DIV      = te_div;
    eid_r11.WCTR_CR_ECP_SEL_RING        = ring;
    eid_r11.WCTR_CR_ECP_PG_DIODE_C1     = 0;
    eid_r11.WCTR_CR_ECP_PG_DIODE_C      = 0;
    eid_r11.WCTR_CR_ECP_PG_DIODE_B1     = 0;
    eid_r11.WCTR_CR_ECP_PG_DIODE_B      = 0;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A1     = 0;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A      = 0;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A0     = 1;
    mbus_remote_register_write(EID_ADDR,0x11,eid_r11.as_int);

    eid_r12.WCTR_IDLE_WIDTH = __crsh_cp_idle_width__;
    eid_r12.WCTR_SEL_SEQ    = __crsh_sel_seq__;
    mbus_remote_register_write(EID_ADDR,0x12,eid_r12.as_int);

    eid_r13.WCTR_PULSE_WIDTH    = __crsh_cp_pulse_width__;
    mbus_remote_register_write(EID_ADDR,0x13,eid_r13.as_int);

    eid_r15.WCTR_RESETB_CP_A = 1;
    eid_r15.WCTR_VIN_CP_A    = __crsh_seqa_vin__;
    eid_r15.WCTR_EN_CP_CK_A  =   ((__crsh_seqa_ck_te__ << 10) | (__crsh_seqa_ck_fd__ << 9) | (__crsh_seqa_ck_seg__ << 0)) & 0x7FF;
    eid_r15.WCTR_EN_CP_PD_A  = ~(((__crsh_seqa_ck_te__ << 10) | (__crsh_seqa_ck_fd__ << 9) | (__crsh_seqa_ck_seg__ << 0)) & 0x7FF) & 0x7FF;
    mbus_remote_register_write(EID_ADDR,0x15,eid_r15.as_int);

    eid_r16.WCTR_RESETB_CP_B = 1;
    eid_r16.WCTR_VIN_CP_B    = __crsh_seqb_vin__;
    eid_r16.WCTR_EN_CP_CK_B  =   ((__crsh_seqb_ck_te__ << 10) | (__crsh_seqb_ck_fd__ << 9) | (__crsh_seqb_ck_seg__ << 0)) & 0x7FF;
    eid_r16.WCTR_EN_CP_PD_B  = ~(((__crsh_seqb_ck_te__ << 10) | (__crsh_seqb_ck_fd__ << 9) | (__crsh_seqb_ck_seg__ << 0)) & 0x7FF) & 0x7FF;
    mbus_remote_register_write(EID_ADDR,0x16,eid_r16.as_int);

    eid_r17.WCTR_RESETB_CP_C = 1;
    eid_r17.WCTR_VIN_CP_C    = __crsh_seqc_vin__;
    eid_r17.WCTR_EN_CP_CK_C  =   ((__crsh_seqc_ck_te__ << 10) | (__crsh_seqc_ck_fd__ << 9) | (__crsh_seqc_ck_seg__ << 0)) & 0x7FF;
    eid_r17.WCTR_EN_CP_PD_C  = ~(((__crsh_seqc_ck_te__ << 10) | (__crsh_seqc_ck_fd__ << 9) | (__crsh_seqc_ck_seg__ << 0)) & 0x7FF) & 0x7FF;
    mbus_remote_register_write(EID_ADDR,0x17,eid_r17.as_int);
}

void eid_enable_timer(void){
    eid_r01.TMR_SELF_EN = 0;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_EN_OSC = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_RESETB = 1;
    eid_r01.TMR_RESETB_DIV = 1;
    eid_r01.TMR_RESETB_DCDC = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_EN_SELF_CLK = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_SELF_EN = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    delay(__eid_tmr_init_delay__); // Wait for >3s
    eid_r01.TMR_EN_OSC = 0;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_SEL_LDO = __eid_tmr_sel_ldo__; // 1: use VBAT; 0: use V1P2
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_ISOL_CLK = 0;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r00.TMR_EN_CLK_DIV = 1;
    mbus_remote_register_write(EID_ADDR,0x00,eid_r00.as_int);
}

void eid_config_cp_clk_gen(uint32_t ring, uint32_t te_div, uint32_t fd_div, uint32_t seg_div) {
    eid_r02.ECP_PG_DIODE    = 1;
    eid_r02.ECP_SEL_RING    = ring;
    eid_r02.ECP_SEL_TE_DIV  = te_div;
    eid_r02.ECP_SEL_FD_DIV  = fd_div;
    eid_r02.ECP_SEL_SEG_DIV = seg_div;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);
}

void eid_set_vin(uint32_t vin) {
    __eid_vin__ = vin;
}

void eid_set_duration(uint32_t duration){
    __eid_cp_duration__ = duration;
    eid_r07.ECTR_PULSE_WIDTH = duration;
    mbus_remote_register_write(EID_ADDR,0x07,eid_r07.as_int);
}

void eid_set_fe_duration(uint32_t duration){
    __eid_cp_fe_duration__ = duration;
}

void eid_enable_cp_ck(uint32_t te, uint32_t fd, uint32_t seg) {
    uint32_t cp_ck = ((te << 10) | (fd << 9) | (seg << 0)) & 0x7FF;
    uint32_t cp_pd = (~cp_ck) & 0x7FF;

    // Make PG_DIODE=0
    eid_r02.ECP_PG_DIODE = 0;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);

    // Enable charge pumps
    eid_r09.ECTR_RESETB_CP = 1;
    eid_r09.ECTR_VIN_CP    = __eid_vin__;
    eid_r09.ECTR_EN_CP_PD  = cp_pd;
    eid_r09.ECTR_EN_CP_CK  = cp_ck;
    set_halt_until_mbus_trx();
    mbus_remote_register_write(EID_ADDR,0x09,eid_r09.as_int);
    set_halt_until_mbus_tx();

    // Make FD White
    if (__eid_clear_fd__ && (seg != 0) && (__eid_cp_fe_duration__ != 0)) {
        cp_ck = ((0x1 << 10) | (0x0 << 9) | (seg << 0)) & 0x7FF;
        cp_pd = (~cp_ck) & 0x7FF;
        eid_set_duration(__eid_cp_fe_duration__);
        eid_r09.ECTR_EN_CP_PD  = cp_pd;
        eid_r09.ECTR_EN_CP_CK  = cp_ck;
        set_halt_until_mbus_trx();
        mbus_remote_register_write(EID_ADDR,0x09,eid_r09.as_int);
        set_halt_until_mbus_tx();
        eid_set_duration(__eid_cp_duration__);
    }

    // Make PG_DIODE=1
    eid_r02.ECP_PG_DIODE = 1;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);

    // Update __eid_current_display__
    __eid_current_display__ = seg;
}

void eid_all_black(void) { 
    eid_enable_cp_ck(0x0, 0x1, 0x1FF); 
}

void eid_all_white(void) { 
    eid_enable_cp_ck(0x1, 0x0, 0x000); 
}

void eid_seg_black(uint32_t seg) { 
    eid_enable_cp_ck(0x0, 0x0, seg);
}

void eid_seg_white(uint32_t seg) { 
    eid_enable_cp_ck(0x1, 0x1, (~seg & 0x1FF));
}

void eid_update(uint32_t seg) { 
    eid_all_white();
    if(seg!=0) eid_seg_black(seg);
}

uint32_t eid_get_current_display(void) {
    return __eid_current_display__;
}

void eid_enable_crash_handler(void) {
    eid_r00.EN_WATCHDOG = 1;
    mbus_remote_register_write(EID_ADDR,0x00,eid_r00.as_int);
}

void eid_check_in() {
    mbus_remote_register_write(EID_ADDR,0x18,0x000000);
}

void eid_trigger_crash() {
    mbus_remote_register_write(EID_ADDR,0x19,0x00DEAD);
}

//*******************************************************************
// SNT FUNCTIONS
//*******************************************************************

void snt_init(void) {

    // Regiser File variables
    snt_r00.as_int = SNT_R00_DEFAULT_AS_INT;
    snt_r01.as_int = SNT_R01_DEFAULT_AS_INT;
    snt_r03.as_int = SNT_R03_DEFAULT_AS_INT;
    snt_r08.as_int = SNT_R08_DEFAULT_AS_INT;
    snt_r09.as_int = SNT_R09_DEFAULT_AS_INT;
    snt_r17.as_int = SNT_R17_DEFAULT_AS_INT;

    // Temp Sensor Initialization
    snt_r01.TSNS_RESETn     = 0;
    snt_r01.TSNS_EN_IRQ     = 1;
    snt_r01.TSNS_BURST_MODE = 0;
    snt_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    // Set temp sensor conversion time
    snt_r03.TSNS_SEL_STB_TIME  = 0x3; // Default: 0x1
    snt_r03.TSNS_SEL_CONV_TIME = 0x7; // Default: 0x6
    mbus_remote_register_write(SNT_ADDR,0x03,snt_r03.as_int);

    // Wakeup Timer configuration
    snt_r17.WUP_INT_RPLY_REG_ADDR = 0x07;
    snt_r17.WUP_INT_RPLY_SHORT_ADDR = 0x10;
    snt_r17.WUP_CLK_SEL = 0x0; // 0: Use CLK_TMR, 1: Use CLK_TSNS
    snt_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
    mbus_remote_register_write(SNT_ADDR,0x17,snt_r17.as_int);

    // Turn on LDO VREF
    snt_ldo_vref_on();
}

void snt_temp_sensor_power_on(void){
    // Turn on digital block
    snt_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
    // Turn on analog block
    snt_r01.TSNS_EN_SENSOR_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
    // Delay (~2ms @ 100kHz clock speed)
    delay(200);
    // Release isolation
    snt_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
    // Delay (~2ms @ 100kHz clock speed)
    delay(200);
}

void snt_temp_sensor_power_off(void){
    snt_r01.TSNS_RESETn         = 0;
    snt_r01.TSNS_SEL_LDO        = 0;
    snt_r01.TSNS_EN_SENSOR_LDO  = 0;
    snt_r01.TSNS_ISOLATE        = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

void snt_temp_sensor_start(void){
    snt_r01.TSNS_RESETn = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

void snt_temp_sensor_reset(void){
    snt_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

void snt_ldo_power_off(void){
    snt_r00.LDO_EN_IREF   = 0;
    snt_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
}

void snt_ldo_vref_on(void){
    snt_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
    // Delay (~50ms @ 100kHz clock speed); NOTE: Start-Up Time of VREF @ TT, 1.1V, 27C is ~40ms.
    delay(5000);
}

void snt_ldo_power_on(void){
    snt_r00.LDO_EN_IREF = 1;
    snt_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
}

void snt_start_timer(uint32_t wait_time){

    // New for SNTv3
    snt_r08.TMR_SLEEP = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);
    snt_r08.TMR_ISOLATE = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // TIMER SELF_EN Disable 
    snt_r09.TMR_SELF_EN = 0x0; // Default : 0x1
    mbus_remote_register_write(SNT_ADDR,0x09,snt_r09.as_int);

    // EN_OSC 
    snt_r08.TMR_EN_OSC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // Release Reset 
    snt_r08.TMR_RESETB = 0x1; // Default : 0x0
    snt_r08.TMR_RESETB_DIV = 0x1; // Default : 0x0
    snt_r08.TMR_RESETB_DCDC = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // TIMER EN_SEL_CLK Reset 
    snt_r08.TMR_EN_SELF_CLK = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

    // TIMER SELF_EN 
    snt_r09.TMR_SELF_EN = 0x1; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x09,snt_r09.as_int);

    // Delay (must be >2 seconds at RT)
    delay(wait_time);

    // Turn off sloscillator
    snt_r08.TMR_EN_OSC = 0x0; // Default : 0x0
    mbus_remote_register_write(SNT_ADDR,0x08,snt_r08.as_int);

}

void snt_stop_timer(void){
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
}

void snt_set_timer_threshold(uint32_t threshold){
    mbus_remote_register_write(SNT_ADDR,0x19,threshold>>24);
    mbus_remote_register_write(SNT_ADDR,0x1A,threshold & 0xFFFFFF);
    
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

void snt_set_wup_timer(uint32_t auto_reset, uint32_t threshold){
    snt_set_timer_threshold(threshold);
    snt_enable_wup_timer(auto_reset);
}

uint32_t snt_read_wup_timer(void){
    set_halt_until_mbus_trx();
    mbus_remote_register_write(SNT_ADDR,0x14,
                             (0x0  << 16)   // 0: Synchronous 32-bit; 1: Synchronous lower 24-bit, 2: Synchronous Upper 8-bit, 3: Invalid
                            |(0x10 <<  8)   // MBus Target Address
                            |(0x04 <<  0)   // Destination Register ID
                            );
    set_halt_until_mbus_tx();
    return ((*REG4 << 24) | *REG5);
}
    
void set_timeout_snt_check(uint32_t threshold){
    __wfi_timeout_flag__ = 0; // Declared in PREv22E.h
    snt_set_timer_threshold(/*threshold*/threshold);
    snt_enable_wup_timer(/*auto_reset*/0);
}

uint32_t stop_timeout_snt_check(uint32_t code){
    if (__wfi_timeout_flag__){
        __wfi_timeout_flag__ = 0;
        mbus_write_message32(0xEF, code);
        return 0;
    }
    return 1;
}

