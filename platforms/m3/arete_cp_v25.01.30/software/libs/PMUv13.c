//*******************************************************************************************
// PMU SOURCE FILE FOR PMUV13 SYSTEM
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*********************************************************
// HEADER AND OTHER SOURCE FILES
//*********************************************************
#include "PMUv13.h"

//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data) {
    enable_reg_irq(PMU_TARGET_REG_IDX);
    mbus_remote_register_write(__pmu_prefix__,reg_addr,reg_data);
    WFI();
}

void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    pmu_reg_write_core(reg_addr, reg_data);
}

uint32_t pmu_reg_read (uint32_t reg_addr) {
    pmu_reg_write_core(0x00, reg_addr);
    return *PMU_TARGET_REG_ADDR;
}

void pmu_set_sar_ratio (uint32_t ratio) {

    #ifdef DEVEL
        mbus_write_message32(0x94, (__pmu_sar_ratio__<<8)|ratio);
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
            | (0x2 << 8 )   // 0x0      // 0x3: Let the SAR do UP-conversion (use if VBAT < 1.2V)
                                        // 0x2: Let the SAR do DOWN-conversion (use if VBAT > 1.2V)
                                        // 0x0: Let PMU handle.
            | (0x1 << 7 )   // 0x0      // If 1, override SAR ratio with [6:0].
            | (ratio)       // 0x00     // SAR ratio for overriding (valid only when [7]=1)
        ));

        __pmu_sar_ratio__ = ratio;

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

void pmu_set_floor(uint8_t mode, uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b) {

    // Register Addresses
    uint32_t sar = (mode == PMU_ACTIVE) ? 0x16 : 0x15;
    uint32_t upc = (mode == PMU_ACTIVE) ? 0x18 : 0x17;
    uint32_t dnc = (mode == PMU_ACTIVE) ? 0x1A : 0x19;

    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(sar,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0  << 19)    // 1'h0     // Enable PFM even during periodic reset
        | (0  << 18)    // 1'h0     // Enable PFM even when VREF is not used as reference
        | (0  << 17)    // 1'h0     // Enable PFM
        | (1  << 14)    // 3'h3     // Comparator clock division ratio (0x1 being slowest)
        | (0  << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (r  <<  9)    // 4'h8     // Frequency multiplier R
        | (sl <<  5)    // 4'h8     // Frequency multiplier L (actually L+1)
        | (b       )    // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(upc,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (3  << 14)    // 2'h0     // Desired Vout/Vin ratio
        | (0  << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (r  <<  9)    // 4'h8     // Frequency multiplier R
        | (ul <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (b       )    // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(dnc,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0  << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (r  <<  9)    // 4'h8     // Frequency multiplier R
        | (dl <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (b       )    // 5'h08    // Floor frequency base (0-63)
    ));
}

void pmu_active_floor(uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b) {
    pmu_set_floor (/*mode*/PMU_ACTIVE, /*r*/r, /*ul*/ul, /*sl*/sl, /*dl*/dl, /*b*/b);
}

void pmu_sleep_floor(uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b) {
    pmu_set_floor (/*mode*/PMU_SLEEP, /*r*/r, /*ul*/ul, /*sl*/sl, /*dl*/dl, /*b*/b);
}

void pmu_set_active_min(){
    pmu_active_floor(/*r*/1, /*ul*/0, /*sl*/0, /*dl*/0, /*b*/1);
}

void pmu_init(uint32_t pmu_prefix, uint32_t irq_reg_idx){

    __pmu_prefix__ = pmu_prefix;

    // Set IRQ Destination
    mbus_remote_register_write(__pmu_prefix__, 0x52, (0x10 << 8) | irq_reg_idx);

    #ifdef __PMU_CHECK_WRITE__ // See PMU Behavior section at the top)
        //[5]: PMU_CHECK_WRITE(1'h0); [4]: PMU_IRQ_EN(1'h1); [3:2]: LC_CLK_DIV(2'h3); [1:0]: LC_CLK_RING(2'h1)
        mbus_remote_register_write(__pmu_prefix__, 0x51, (0x1 << 5) | (0x1 << 4) | (0x3 << 2) | (0x1 << 0));
    #endif

    // Floor Settings
    pmu_sleep_floor (/*r*/6, /*ul*/4, /*sl*/8, /*dl*/4, /*b*/4);
    pmu_active_floor(/*r*/9, /*ul*/6, /*sl*/6, /*dl*/6, /*b*/16);

    // Initialize SAR Ratio
    pmu_set_sar_ratio(76);

    // Disable ADC in Active
    // PMU ADC will be automatically reset when system wakes up
    #ifndef __USE_HCODE__
        //---------------------------------------------------
        // CTRL_DESIRED_STATE_ACTIVE; Disable ADC
        //---------------------------------------------------
        __pmu_desired_state_active__.value              = __PMU_DESIRED_STATE_ACTIVE_DEFAULT__;
        __pmu_desired_state_active__.adc_output_ready   = 0;
        __pmu_desired_state_active__.adc_adjusted       = 0;
        __pmu_desired_state_active__.sar_ratio_adjusted = 0;
        __pmu_desired_state_active__.vbat_read_only     = 0;
        pmu_reg_write(0x3C, __pmu_desired_state_active__.value);

        // Turn off ADC offset measurement in Sleep
        //---------------------------------------------------
        // CTRL_DESIRED_STATE_SLEEP; Use ADC in Flip-Mode
        //---------------------------------------------------
        __pmu_desired_state_sleep__.value              = __PMU_DESIRED_STATE_SLEEP_DEFAULT__;
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

}

