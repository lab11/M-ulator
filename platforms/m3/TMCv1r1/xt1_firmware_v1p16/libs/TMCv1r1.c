//*******************************************************************************************
// TMC SOURCE FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//      PREv22E -> SNTv5 -> EIDv1 -> MRRv11A -> MEMv3 -> PMUv13
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*********************************************************
// HEADER AND OTHER SOURCE FILES
//*********************************************************
#include "TMCv1r1.h"

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

//-------------------------------------------------------------------
// Math Functions
//-------------------------------------------------------------------

uint32_t get_lead1_pos (uint32_t val) {
    uint32_t m = 32;
    if (val!=0) {
        while (m>0) {
            m--;
            if (val&(0x1<<m)) break;
        }
    }
    return m;
}

uint32_t div (uint32_t numer, uint32_t denom, uint32_t n) {
    #ifdef DEVEL
        mbus_write_message32(0xC6, numer);
        mbus_write_message32(0xC6, denom);
        mbus_write_message32(0xC6, n);
    #endif

    uint32_t result = 0;
    uint32_t overflow = 0;
    uint32_t underflow = 0;

    if (denom==0) {
        overflow = 1;
    }
    else if (numer!=0) {

        uint32_t inc = (0x1<<n);
        uint32_t numer_m = get_lead1_pos(numer);
        uint32_t denom_m = get_lead1_pos(denom);

        numer <<= (31 - numer_m);
        denom <<= (31 - denom_m);

        if (numer_m > denom_m) {
            inc <<= (numer_m - denom_m);
            if (inc==0) overflow = 1;
        }
        else if (denom_m > numer_m) {
            inc >>= (denom_m - numer_m);
            if (inc==0) underflow = 1;
        }

        if (overflow) result = 0xFFFFFFFF;
        else if (underflow) result = 0;
        else {
            while ((numer!=0)&&(inc!=0)) {
                if (numer>=denom) {
                    numer -= denom;
                    result |= inc;
                }
                denom >>= 1;
                inc   >>= 1;
            }
        }

    }

    #ifdef DEVEL
        mbus_write_message32(0xC7, result);
    #endif
    return result;
}

uint32_t mult(uint32_t num_a, uint32_t num_b) {
    #ifdef DEVEL
        mbus_write_message32(0xC0, num_a);
        mbus_write_message32(0xC0, num_b);
    #endif

    uint32_t i;
    uint32_t result = 0;
    uint32_t ac = 0;    // num_a carry-over. Becomes 1 if there is any lost 1 during lsh.
    uint32_t overflow = 0;

    for(i=0; i<32; i++) {
        if (num_b&0x1) {
            result += num_a;
            // Overflow Checking
            if (ac || (result < num_a)) {
                overflow = 1;
                break;
            }
        }
        if (ac==0) ac = (num_a>>31)&0x1;
        num_b >>= 1;
        num_a <<= 1;
    }

    if (overflow) result = 0xFFFFFFFF;

    #ifdef DEVEL
        mbus_write_message32(0xC1, result);
    #endif
    return result;
}

uint32_t mult_b28 (uint32_t num_a, uint32_t num_b) {
// Number of decimal digits in num_b
#define __N_B__ 28

    #ifdef DEVEL
        mbus_write_message32(0xC2, num_a);
        mbus_write_message32(0xC2, num_b);
    #endif

    uint32_t i;
    uint32_t result = 0;
    uint32_t overflow = 0;

    if ((num_a==0)||(num_b==0)) {
        result = 0;
    }
    // Full Fixed-Point Multiplication
    else {
        uint32_t insig;
        uint32_t error = 0;
        uint32_t n = get_lead1_pos(num_a);
        uint32_t m = get_lead1_pos(num_b);

        if (m>=__N_B__) {
            if (n+(m-__N_B__)>31) overflow = 1;
            num_a<<=(m-__N_B__);
            insig = 0;
        }
        else {
            insig = get_bits(num_a, __N_B__-m, 0)<<(31-(__N_B__-m));
            num_a>>=(__N_B__-m);
        }

        if (!overflow) {
            for (i=(m+1); i>0; i--) {
                if ((num_b>>(i-1))&0x1) {
                    result += num_a;
                    if (result < num_a) {
                        overflow = 1;
                        break;
                    }
                    if (insig>(0xFFFFFFFF-error)) {
                        result++; // insig + error > max -> Carry-out
                        if (result==0) {
                            overflow = 1;
                            break;
                        }
                    }
                    error += insig;
                }
                insig = ((num_a&0x1)<<31)|(insig>>1);
                num_a>>=1;
            }

            // round-up the error and get an integer value
            if ((error>>31)&0x1) {
                result++;
                if (result==0) overflow = 1;
            }
        }
    }

    if (overflow) result = 0xFFFFFFFF;

    #ifdef DEVEL
        mbus_write_message32(0xC3, result);
    #endif
    return result;
}

//-------------------------------------------------------------------
// High Power History
//-------------------------------------------------------------------

void set_high_power_history (uint32_t val) {
    __high_power_history__ = val;
}

uint32_t get_high_power_history (void) {
    return __high_power_history__;
}

//*******************************************************************
// PRE FUNCTIONS (Specific to TMCv1r1)
//*******************************************************************

void restart_xo(uint32_t delay_a, uint32_t delay_b) {
    //--- Stop XO
    xo_stop();

    //--- Start XO clock (NOTE: xo_stop() disables both the clock and the timer)
    xo_start(/*delay_a*/delay_a, /*delay_b*/delay_b, /*start_cnt*/0);

    //--- Configure and Start the XO Counter
    set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0);
    start_xo_cnt();
}

//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

//-------------------------------------------------------------------
// PMU Floor Loop-Up Tables
//-------------------------------------------------------------------

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
            | (0x0 << 8 )   // 0x0      // 0x3: Let the SAR do UP-conversion (use if VBAT < 1.2V)
                                        // 0x2: Let the SAR do DOWN-conversion (use if VBAT > 1.2V)
                                        // 0x0: Let PMU handle.
            | (0x1 << 7 )   // 0x0      // If 1, override SAR ratio with [6:0].
            | (ratio)       // 0x00     // SAR ratio for overriding (valid only when [7]=1)
        ));

        __pmu_sar_ratio__ = ratio;

        delay(10000); // ~0.4s with 140kHz CPU clock
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

uint32_t pmu_calc_new_sar_ratio(uint32_t adc_val, uint32_t offset, uint32_t sel_margin, uint32_t hysteresis) {

    // Equations - Credit: Seokhyeon Jeong, Zhiyoong Foo (06/16/2022)
    //-------------------------------------------------------
    //  (1)  (SAR_RATIO / 128) x VBAT = V1P2
    //  (2)  VBAT = (ADC x VREF x 4) / 256
    // Plugging in (2) into (1) gives
    //  (3)  (SAR_RATIO / 128) x (ADC x VREF x 4) / 256 = V1P2
    //      -> (SAR_RATIO x ADC) / 8192 = V1P2 / VREF
    // Thus, in order to guarantee V1P2 > VREF, 
    //  the following condition must be met:
    //  (4)  SAR_RATIO > 8192 / ADC
    // With n% margin:
    //  (5)  SAR_RATIO > (8192 x (1+(n/100))) / ADC
    //-------------------------------------------------------

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

            //// ORIGINAL Margin
            ////---------------------------------
            //// sel_margin   Margin      1+(n/100) in binary
            ////---------------------------------
            ////  5           No margin   1.00000
            ////  4           50.00%      1.10000
            ////  3           25.00%      1.01000
            ////  2           12.50%      1.00100
            ////  1            6.25%      1.00010
            ////  0            3.13%      1.00001
            ////---------------------------------
            //if (sel_margin > 5) sel_margin = 5;
            //uint32_t factor = (0x1<<5)|(0x1<<sel_margin);   // factor = 1+(n/100) in 5-fixed-point representation

            // ORIGINAL METHOD ------------------------//
            //new_val = div(  /*numer*/
            //                    mult(/*num_a*/ 8192, /*num_b*/ factor), // Calculate 8192 x factor. Result is in 5-fixed-point representation
            //                /*denom*/
            //                    adc_val<<5, // ADC value in 5-fixed-point representation
            //                /*n*/
            //                    0           // Final result should be an integer
            //              );
            //new_val++;  // Increment by 1 (round-up)
            //-------------------------------------------//


            //// SECOND METHOD - SAR Compensation
            //// SAR = (8192 x M / ADC) x (1 + beta / ADC)
            ////  where M = 1+(n/100) and 'beta' is an empirical value (ADC = alpha x VBAT + beta)
            //uint32_t a_n5 = mult(/*num_a*/ 8192, /*num_b*/ factor); // a = 8192 x M. This is a 5-fixed-point representation.
            //uint32_t b_n5 = 0x6 << 5;                               // b = empirical 'beta' value from the equation ADC = alpha x VBAT + beta. This is a 5-fixed-point representation.
            //uint32_t c_n5 = (0x1<<5) + div(/*numer*/b_n5, /*denom*/(adc_val<<5), /*n*/5); // c = 1 + beta/ADC. This is a 5-fixed-point representation.
            //uint32_t d_n5 = div (/*numer*/a_n5, /*denom*/(adc_val<<5), /*n*/5);     // d = a / ADC = 8192 x M / ADC. This is a 5-fixed-point representation.

            //new_val = mult(/*num_a*/d_n5, /*num_b*/c_n5) >> 10;
            ////-------------------------------------------//


            // NEW Margin
            //---------------------------------
            // sel_margin   Margin      1+(n/100) in binary
            //---------------------------------
            //  0           No margin   1.00_0000_0000 // invalid
            //---------------------------------

            //---------------------------------
            // sel_margin   Margin      1+(n/100) in binary
            //---------------------------------
            //  8           25.00%      1.01_0000_0000
            //  7           20.00%      1.00_1100_1100
            //  6           15.00%      1.00_1001_1001
            //  5           10.00%      1.00_0110_0110
            //  4            5.00%      1.00_0011_0011
            //  3            0.00%      1.00_0000_0000
            //  2           -5.00%      0.11_1100_1100
            //  1          -10.00%      0.11_1001_1001
            //  0           No margin   1.00_0000_0000 // invalid
            //---------------------------------
            // NOTE: With original equation (SAR=8192/ADC), the system crashes with M=0.9 at VBAT=2.95V. Active V1P2 is ~1.20V at VBAT=3.0V.

            if (sel_margin  > 8) sel_margin = 8;
            if (sel_margin == 0) sel_margin = 1;

//            //-------------------------------------------//
//            // EMPIRICAL CALIBRATION
//            // It uses Excel's trendline to calculate a and b
//            //----------------------------------------------------------
//            // For V1P2 = a x VBAT + b
//            // Margin=-10% makes the system crash with the original equation
//            //----------------------------------------------------------------------------------------------------------
//            // margin       -10%        -5%         0%          5%          6%          7%          8%        9%      10%         15%         20%         25%
//            //----------------------------------------------------------------------------------------------------------
//            // M            0.90        0.95        1.00        1.05        1.06        1.07        1.08      1.09    1.10        1.15        1.20        1.25
//            // a            -           0.0457      0.0801      0.0857      0.0753      0.0931      0.0915    0.0966  0.1095      0.1044      0.1274      0.1498
//            // b            -           1.1376      1.1020      1.1181      1.1523      1.1244      1.1449    1.1418  1.1209      1.194       1.1955      1.1947
//            // vref         1.30        <-          <-          <-          <-          <-                            <-          <-          <-          <-
//            // v0           1.40        <-          <-          <-          <-          <-                            <-          <-          <-          <-
//            // M (10-np)    0x399       0x3CC       0x400       0x433       0x43D       0x447       0x451     0x45C   0x466       0x499       0x4CC       0x500
//            // a (16-np)    -           0x0BB2      0x1481      0x15F0      0x1346      0x17D5                        0x1C08      0x1AB9      0x209D      0x2659
//            // b (16-np)    -           0x12339     0x11A1C     0x11E3B     0x126FD     0x11FD8                       0x11EF3     0x131A9     0x1320C     0x131D7
//            // vref (16-np) 0x14CCC     <-          <-          <-          <-          <-          <-        <-      <-          <-          <-          <-
//            // v0   (16-np) 0x16666     <-          <-          <-          <-          <-          <-        <-      <-          <-          <-          <-
//            //----------------------------------------------------------------------------------------------------------
//            // 2-point Fitting: It only considers V1P2@2V and V1P2@3V to calculate a and b.
//            //----------------------------------------------------------------------------------------------------------
//            // a            -           0.066       0.074       0.095       0.082       0.095       0x094     0.094   0.155       0.129       0.153       0.166
//            // b            -           1.070       1.115       1.074       1.112       1.097       1.123     1.123   1.000       1.111       1.107       1.134
//            // a (16-np)    -           0x10E5      0x12F1      0x1851      0x14FD      0x1851                        0x27AE      0x2106      0x272B      0x2A7E
//            // b (16-np)    -           0x111EB     0x11D70     0x112F1     0x11CAC     0x118D4                       0x10000     0x11C6A     0x11B64     0x1224D
//            //----------------------------------------------------------------------------------------------------------
//            uint32_t M;     // M = 1+(n/100) in 10-fixed-point representation
//            uint32_t a;     // value of a; 16-fixed-point representation
//            uint32_t b;     // value of b; 16-fixed-point representation
//            uint32_t vref;  // VREF=1.35 (fixed). 16-fixed-point representation
//            uint32_t v0;    // V0=1.40 (fixed). Desired V1P2 value. 16-fixed-point representation
//            
//            //----------------------------------------------------------------------
//            // 21-point fitting
//            //----------------------------------------------------------------------
//            if      (sel_margin==1) { M=0x433; a=0x15F0; b=0x11E3B;}    // M=1.05
//            else if (sel_margin==2) { M=0x43D; a=0x1346; b=0x126FD;}    // M=1.06
//            else if (sel_margin==3) { M=0x447; a=0x17D5; b=0x11FD8;}    // M=1.07
//            if      (sel_margin==1) { M=0x399; a=0x0BB2; b=0x12339;}    // M=0.90
//            else if (sel_margin==2) { M=0x3CC; a=0x0BB2; b=0x12339;}    // M=0.95
//            else if (sel_margin==3) { M=0x400; a=0x1481; b=0x11A1C;}    // M=1.00
//            else if (sel_margin==4) { M=0x433; a=0x15F0; b=0x11E3B;}    // M=1.05
//            else if (sel_margin==5) { M=0x466; a=0x1C08; b=0x11EF3;}    // M=1.10
//            else if (sel_margin==6) { M=0x499; a=0x1AB9; b=0x131A9;}    // M=1.15
//            else if (sel_margin==7) { M=0x4CC; a=0x209D; b=0x1320C;}    // M=1.20
//            else if (sel_margin==8) { M=0x500; a=0x2659; b=0x131D7;}    // M=1.25
//            else                    { M=0x400; a=0x1481; b=0x11A1C;}    // invalid
//            //----------------------------------------------------------------------
//
//            //----------------------------------------------------------------------
//            // 2-point fitting
//            //----------------------------------------------------------------------
//            if      (sel_margin==1) { a=0x10E5; b=0x111EB;}    // M=0.90
//            else if (sel_margin==2) { a=0x10E5; b=0x111EB;}    // M=0.95
//            else if (sel_margin==3) { a=0x12F1; b=0x11D70;}    // M=1.00
//            else if (sel_margin==4) { a=0x1851; b=0x112F1;}    // M=1.05
//            else if (sel_margin==5) { a=0x27AE; b=0x10000;}    // M=1.10
//            else if (sel_margin==6) { a=0x2106; b=0x11C6A;}    // M=1.15
//            else if (sel_margin==7) { a=0x272B; b=0x11B64;}    // M=1.20
//            else if (sel_margin==8) { a=0x2A7E; b=0x1224D;}    // M=1.25
//            else                    { a=0x10E5; b=0x111EB;}    // invalid
//            //----------------------------------------------------------------------
//
//            //vref = 0x13333; // 1.20; 16-point
//            //vref = 0x14000; // 1.25; 16-point
//            vref = 0x14CCC; // 1.30; 16-point
//            //vref = 0x15999; // 1.35; 16-point
//            v0   = 0x16666;

//            new_val = div (/*numer*/  mult(/*num_a*/8192, /*num_b*/M),
//                           /*denom*/  adc_val<<10,
//                           /*n*/      0);

//            uint32_t M;
//            if      (sel_margin==1)  {   M=0x451;} // 8%
//            if      (sel_margin==2)  {   M=0x45C;} // 9%
//
//            new_val = div (/*numer*/  mult(/*num_a*/8192, /*num_b*/M),
//                           /*denom*/  adc_val<<10,
//                           /*n*/      0);

//-----------------------------------------------------------------------------------------------
// EMPIRICAL FITTING - 1
//-----------------------------------------------------------------------------------------------
//            // sar_orig = 8192 / ADC; 16-point
//            uint32_t sar_orig = div (/*numer*/ 8192,
//                                     /*denom*/ adc_val,
//                                     /*n*/     16);
//
//            // a128 = 128 x a; 16-point
//            uint32_t a128 = mult(/*num_a*/128, /*num_b*/a);
//
//            // mv0 = M x v0; 16-point
//            uint32_t mv0 = mult(/*num_a*/M, /*num_b*/v0) >> 10; 
//
//            // term1 = M + (M x v0 - b) / VREF
//            uint32_t term1;
//            if (mv0 > b) {
//                // term1 = M + (M x v0 - b) / VREF;    16-point
//                term1 = (M << 6) + div (/*numer*/ mv0 - b,
//                                        /*denom*/ vref,
//                                        /*n*/     16);
//            }
//            else {
//                // term1 = M - (b - M x v0) / VREF;    16-point
//                term1 = (M << 6) - div (/*numer*/ b - mv0,
//                                        /*denom*/ vref,
//                                        /*n*/     16);
//            }
//
//            // new_sar = sar_orig x term1 - 128a
//            new_val = (mult(/*num_a*/sar_orig>>16, /*num_b*/term1) - a128) >> 16;

//-----------------------------------------------------------------------------------------------
// EMPIRICAL FITTING - 2
//-----------------------------------------------------------------------------------------------

//            // sar_orig = 8192 x M / ADC; integer
//            uint32_t sar_orig = div (/*numer*/ 
//                                                mult(/*num_a*/8192, /*num_b*/M),
//                                     /*denom*/  adc_val<<10,
//                                     /*n*/      0);
//            
//            // mv0x64 = 64 x M x v0; 16-fp
//            uint32_t mv0x64 = (mult(/*num_a*/M, /*num_b*/v0) >> 10) << 6; 
//
//            // bx64 = 64 x b; 16-fp
//            uint32_t bx64 = b << 6;
//
//            // axvrefxadc = a x vref x ADC; 16-fp
//            uint32_t axvrefxadc = mult (/*num_a*/   a >> 8,
//                                        /*num_b*/   mult(/*num_a*/vref, /*num_b*/adc_val) >> 8   // 16-fp
//                                        );
//
//            // term1 = a x vref x ADC + 64b; 16-fp
//            uint32_t term1 = axvrefxadc + bx64;
//
//            // factor = (64 x M x v0) / (a x vref x ADC + 64b); 16-fp
//            uint32_t factor = div ( /*numer*/   mv0x64,
//                                    /*denom*/   term1, 
//                                    /*n*/       16
//                                    );
//
//            // Final SAR = sar_orig x factor; integer
//            new_val = mult(/*num_a*/sar_orig, /*num_b*/factor) >> 16;

//-----------------------------------------------------------------------------------------------
// LINEAR APPROXIMATION -> Not a negligible error
//-----------------------------------------------------------------------------------------------
// For Option 3 (Empirical Fitting - 2), 10% Margin, 2pt Meas. (a=0.155, b=1.000), VREF=1.30, V0=1.4
//            new_val = (0xC7FD98/*199.990604 in 16-fp*/ - mult(/*num_a*/0xF498/*0.955454 w/ 16-fp)*/, /*num_b*/adc_val)) >> 16;

//-----------------------------------------------------------------------------------------------
// TAYLOR-2 (centered at 110) APPROXIMATION
//-----------------------------------------------------------------------------------------------

            uint32_t cent_val = 110;
            uint32_t c0, c1, c2;

//            // For Option 3 (Empirical Fitting - 2), 10% Margin, 2pt Meas. (a=0.155, b=1.000), VREF=1.30, V0=1.4
//            // Equation: SAR = 36223.058253 x ADC^(-1.267595) for ADC=[95, 140]
//            // Taylor Approximation:
//            //          SAR = 93.61079931 - 1.078732556 x (ADC - 110) + 0.011118766 x (ADC - 110)^2
//            //              = 0x5D9C5D    - 0x11427     x (ADC - 110) + 0x2D8       x (ADC - 110)^2  (hex numbers are in 16-fp)
//            c0 = 0x5D9C5D; // 93.61079931 (16-fp)
//            c1 = 0x11427;  // 1.078732556 (16-fp)
//            c2 = 0x2D8;    // 0.011118766 (16-fp)

//            // For Option 3 (Empirical Fitting - 2), 5% Margin, 2pt Meas. (a=0.095, b=1.074), VREF=1.30, V0=1.4
//            // Equation: SAR = 22114.206806 x ADC^(-1.172637) for ADC=[95, 140]
//            // Taylor Approximation:
//            //          SAR = 89.30150385 - 0.951984069 x (ADC - 110) + 0.009401436 x (ADC - 110)^2
//            //              = 0x594D2F    - 0xF3B5      x (ADC - 110) + 0x268       x (ADC - 110)^2  (hex numbers are in 16-fp)
//            c0 = 0x594D2F; // 89.30150385 (16-fp)
//            c1 = 0xF3B5;   // 0.951984069 (16-fp)
//            c2 = 0x268;    // 0.009401436 (16-fp)

//            // For Option 3 (Empirical Fitting - 2), 6% Margin, 2pt Meas. (a=0.082, b=1.112), VREF=1.30, V0=1.4
//            // Equation: SAR = 19954.623963 x ADC^(-1.1482) for ADC=[95, 140]
//            // Taylor Approximation:
//            //          SAR = 90.38918426 - 0.94349874 x (ADC - 110) + 0.009212836 x (ADC - 110)^2
//            //              = 0x5A63A1    - 0xF189     x (ADC - 110) + 0x25B       x (ADC - 110)^2  (hex numbers are in 16-fp)
//            if (sel_margin==2) {
//                c0 = 0x5A63A1;
//                c1 = 0xF189;
//                c2 = 0x25B;
//            }

//            // For Option 3 (Empirical Fitting - 2), 7% Margin, 2pt Meas. (a=0.095, b=1.097), VREF=1.30, V0=1.4
//            // Equation: SAR = 22245.330076 x ADC^(-1.169634) for ADC=[95, 140]
//            // Taylor Approximation:
//            //          SAR = 91.10801031 - 0.968754787 x (ADC - 110) + 0.009553833 x (ADC - 110)^2
//            //              = 0x5B1BA6    - 0xF800      x (ADC - 110) + 0x272       x (ADC - 110)^2  (hex numbers are in 16-fp)
//            if (sel_margin==3) {
//                c0 = 0x5B1BA6;
//                c1 = 0xF800;
//                c2 = 0x272;
//            }

//            // For Option 3 (Empirical Fitting - 2), 6% Margin, 21pt Meas. (a=0.0753, b=1.1523), VREF=1.30, V0=1.4
//            // Equation: SAR = 18274.5732 x ADC^(-1.133598) for ADC=[95, 140]
//            // Taylor Approximation:
//            //          SAR = 88.66017371 - 0.913681778 x (ADC - 110) + 0.008861044 x (ADC - 110)^2
//            //              = 0x58A901    - 0xE9E7      x (ADC - 110) + 0x244       x (ADC - 110)^2  (hex numbers are in 16-fp)
//            if (sel_margin==2) {
//                c0 = 0x58A901;
//                c1 = 0xE9E7;
//                c2 = 0x244;
//            }

//            // For Option 3 (Empirical Fitting - 2), 7% Margin, 21pt Meas. (a=0.0931, b=1.1244), VREF=1.30, V0=1.4
//            // Equation: SAR = 21229.29852 x ADC^(-1.163414) for ADC=[95, 140]
//            // Taylor Approximation:
//            //          SAR = 89.52633679 - 0.946874487 x (ADC - 110) + 0.00931128 x (ADC - 110)^2
//            //              = 0x5986BE    - 0xF266      x (ADC - 110) + 0x262      x (ADC - 110)^2  (hex numbers are in 16-fp)
//            if (sel_margin==3) {
//                c0 = 0x5986BE;
//                c1 = 0xF266;
//                c2 = 0x262;
//            }

//            // For Option 3 (Empirical Fitting - 2), 5% Margin, 21pt Meas. (a=0.0.0857, b=1.1181), VREF=1.30, V0=1.4
//            // Equation: SAR = 19819.884449 x ADC^(-1.153142) for ADC=[95, 140]
//            // Taylor Approximation:
//            //          SAR = 87.71734426 - 0.919550489 x (ADC - 110) + 0.008999649 x (ADC - 110)^2
//            //              = 0x57B7A3    - 0xEB67      x (ADC - 110) + 0x24D       x (ADC - 110)^2  (hex numbers are in 16-fp)
//            if (sel_margin==2) {
//                c0 = 0x57B7A3;
//                c1 = 0xEB67;
//                c2 = 0x24D;
//            }

//            // For Option 3 (Empirical Fitting - 2), 10% Margin, 21pt Meas. (a=0.1095, b=1.1209), VREF=1.30, V0=1.4
//            // Equation: SAR = 24487.430221 x ADC^(-1.187268) for ADC=[95, 140]
//            // Taylor Approximation:
//            //          SAR = 92.3130289 - 0.996366411 x (ADC - 110) + 0.009906002 x (ADC - 110)^2
//            //              = 0x5C5022   - 0xFF11      x (ADC - 110) + 0x289      x (ADC - 110)^2  (hex numbers are in 16-fp)
//            if (sel_margin==3) {
//                c0 = 0x5C5022;
//                c1 = 0xFF11;
//                c2 = 0x289;
//            }

//          Below are using cent_val=115, and a/b from 21-point measurements. VREF=1.3, Vo=1.4
            cent_val = 115;
          //if      (sel_margin==1)  {   c0 = 0x4D5CCE;  c1 = 0x0C536;    c2 = 0x1BF; }   // 0%
            if      (sel_margin==1)  {   c0 = 0x536778;  c1 = 0x0D5DB;    c2 = 0x1E5; }   // 5%
            else if (sel_margin==2)  {   c0 = 0x545DCE;  c1 = 0x0D4AD;    c2 = 0x1E0; }   // 6%
            else if (sel_margin==3)  {   c0 = 0x5516E1;  c1 = 0x0DC1D;    c2 = 0x1F5; }   // 7%
            else if (sel_margin==4)  {   c0 = 0x559E24;  c1 = 0x0DC93;    c2 = 0x1F5; }   // 8%
            else if (sel_margin==5)  {   c0 = 0x56A605;  c1 = 0x0E0B7;    c2 = 0x1FF; }   // 9%
            else if (sel_margin==6)  {   c0 = 0x57A761;  c1 = 0x0E760;    c2 = 0x212; }   // 10%
            else if (sel_margin==7)  {   c0 = 0x5BBA07;  c1 = 0x0EED2;    c2 = 0x220; }   // 15%
            else if (sel_margin==8)  {   c0 = 0x602E9D;  c1 = 0x100C9;    c2 = 0x24E; }   // 20%
          //else if (sel_margin==8)  {   c0 = 0x64E205;  c1 = 0x11373;    c2 = 0x27E; }   // 25%


            // Actual Implementation
            uint32_t pwr1, pwr1_sign;
            if (adc_val < cent_val) { pwr1_sign = 0; pwr1 = cent_val - adc_val;}
            else                    { pwr1_sign = 1; pwr1 = adc_val - cent_val;}
            uint32_t pwr2 = mult(/*num_a*/pwr1, /*num_b*/pwr1);

            if (pwr1_sign) {
                new_val = c0 - mult(/*num_a*/c1, /*num_b*/pwr1) + mult(/*num_a*/c2, /*num_b*/pwr2);
            }
            else {
                new_val = c0 + mult(/*num_a*/c1, /*num_b*/pwr1) + mult(/*num_a*/c2, /*num_b*/pwr2);
            }

            new_val = new_val >> 16;

//-----------------------------------------------------------------------------------------------
            // Limit the SAR ratio
            if (new_val < 64) new_val = 64;

            // Updates __pmu_last_effective_adc_val__
            if ((__pmu_last_effective_adc_val__==0) || (new_val != __pmu_sar_ratio__))
                __pmu_last_effective_adc_val__ = adc_val;
        }

    }

    //mbus_write_message32(0xC8, (adc_val<<16)|new_val);

    return new_val;

}

uint32_t pmu_check_low_vbat (uint32_t adc_val, uint32_t offset, uint32_t threshold, uint32_t num_cons_meas) {
    num_cons_meas = 5;  // FIXME: Delete this in the final version
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
    num_cons_meas = 5;  // FIXME: Delete this in the final version
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
    pmu_set_sar_ratio(0x4C);        // See pmu_calc_new_sar_ratio()
    __pmu_last_effective_adc_val__ = 0;

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
    //---------------------------------------------------------------------------------------
    // RAT_CONFIG
    // NOTE: Unless overridden, changes in RAT_CONFIG[19:13] do not take effect
    //       until the next sleep or wakeup-up transition.
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x47, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (1 << 19)   // 1'h0     // Enable RAT
        | (0   << 18)   // 1'h0     // Enable SAR Slow-Loop in Active (NOTE: When RAT is enabled, SAR Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (0   << 17)   // 1'h0     // Enable UPC Slow-Loop in Active (NOTE: When RAT is enabled, UPC Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (0   << 16)   // 1'h0     // Enable DNC Slow-Loop in Active (NOTE: When RAT is enabled, DNC Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (1 << 15)   // 1'h0     // Enable SAR Slow-Loop in Sleep
        | (1 << 14)   // 1'h0     // Enable UPC Slow-Loop in Sleep
        | (1 << 13)   // 1'h0     // Enable DNC Slow-Loop in Sleep
        | (1   << 11)   // 2'h0     // Clock Ring Tuning (0x0: 13 stages; 0x1: 11 stages; 0x2: 9 stages; 0x3: 7 stages)
        | (4   << 8 )   // 3'h0     // Clock Divider Tuning for SAR Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (4   << 5 )   // 3'h0     // Clock Divider Tuning for UPC Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (4   << 2 )   // 3'h0     // Clock Divider Tuning for DNC Charge Pump Pull-Up (0x0: div by 1; 0x1: div by 2; 0x2: div by 4; 0x3: div by 8; 0x4: div by 16)
        | (3   << 0 )   // 2'h0     // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up
    ));


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

    // Configure GPO and IT_TIME
    nfc_power_on();
    nfc_i2c_present_password();

    //--- Enable GPO
    nfc_i2c_byte_write( /*e2*/1, 
                        /*addr*/    ST25DV_ADDR_GPO, 
                        /*data*/    (0x1 << 7)  //  GPO_EN 
                                |   (0x0 << 6)  //  RF_WRITE_EN 
                                |   (0x0 << 5)  //  RF_GET_MSG_EN 
                                |   (0x0 << 4)  //  RF_PUT_MSG_EN 
                                |   (0x0 << 3)  //  FIELD_CHANGE_EN 
                                |   (0x1 << 2)  //  RF_INTERRUPT_EN 
                                |   (0x0 << 1)  //  RF_ACTIVITY_EN 
                                |   (0x0 << 0)  //  RF_USER_EN
                                ,
                        /*nb*/  1
                        );

    // Config GPO wakeup
    config_gpio_posedge_wirq((1<<__NFC_GPO__));

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
        set_high_power_history(1);

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
        // I2C token is automatically released
        __nfc_i2c_token__ = 0x0;
    }
}

uint32_t nfc_i2c_get_timeout(void) {
    return __nfc_i2c_timeout__;
}

void nfc_i2c_start(void) {
    __nfc_i2c_timeout__ = 0;
    nfc_power_on();
#ifdef __USE_FAST_I2C__
    *GPIO_DATA = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=1, SCL=1
    *GPIO_DIR  = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=output(1), SCL=output(1);
    *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=0, SCL=1
    *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=0, SCL=0
    // update the retentive memory
    *R_GPIO_DIR  = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__);
    *R_GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__);
#else
    gpio_write_data_with_mask(__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_set_dir_with_mask   (__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SDA__)|(0<<__NFC_SCL__));
#endif
}

void nfc_i2c_stop(void) {
#ifdef __USE_FAST_I2C__
    *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=0, SCL=0
    *GPIO_DIR  = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=output(1), SCL=output(1);
    *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=0, SCL=1
    *GPIO_DATA = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=1, SCL=1
    // update the retentive memory
    *R_GPIO_DIR  = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__);
    *R_GPIO_DATA = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__);
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
        *GPIO_DIR  = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=output(1), SCL=output(1);
        while(i>0) {
            if ((byte&0x80)!=0) {
                *GPIO_DATA = (0x1<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=1, SCL=0
                *GPIO_DATA = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=1, SCL=1
                *GPIO_DATA = (0x1<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=1, SCL=0
            } else {
                *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=0, SCL=0
                *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=0, SCL=1
                *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=0, SCL=0
            }
            byte = byte << 1;
            i--;
        }
    
        // Send byte[0]
        if ((byte&0x80)!=0) { *GPIO_DIR  = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); } // SDA=input(0), SCL=output(1);
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=z, SCL=0
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=z, SCL=1
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=z, SCL=0
        if ((byte&0x80)!=0) { *GPIO_DIR  = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); } // SDA=input(0), SCL=output(1);
    
        //Wait for ACK
        nfc_i2c_wait_for_ack();
    
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=z, SCL=1
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=z, SCL=0
    
        // update the retentive memory
        *R_GPIO_DIR  = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__);
        *R_GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__);
    
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
    
        *GPIO_DIR  = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=output(1), SCL=output(1);
        uint32_t i = 7;
        while(i>0) {
            if ((byte&0x80)!=0) {
                *GPIO_DATA = (0x1<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=1, SCL=0
                *GPIO_DATA = (0x1<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=1, SCL=1
                *GPIO_DATA = (0x1<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=1, SCL=0
            } else {
                *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=0, SCL=0
                *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=0, SCL=1
                *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=0, SCL=0
            }
            byte = byte << 1;
            i--;
        }
    
        // Send byte[0]
        if ((byte&0x80)!=0) { *GPIO_DIR  = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); } // SDA=input(0), SCL=output(1);
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=z, SCL=0
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=z, SCL=1
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=z, SCL=0
        if ((byte&0x80)!=0) { *GPIO_DIR  = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); } // SDA=input(0), SCL=output(1);
    
        // Check ACK
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=z, SCL=1
        no_ack = (*GPIO_DATA>>__NFC_SDA__)&0x1;
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=z, SCL=0
    
        // update the retentive memory
        *R_GPIO_DIR  = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__);
        *R_GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__);
    
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
                    #ifdef DEVEL
                        mbus_write_message32(0xE0, 0x01000000);
                    #endif
                    return 0;
                }
                // If everything went well, get the token
                else {
                    nfc_i2c_polling();
                    __nfc_i2c_token__ = 1;
                    #ifdef DEVEL
                        mbus_write_message32(0xE0, 0x01000001);
                    #endif
                    return 1;
                }
            }
            nfc_i2c_stop();
            num_try++;

            // Some delay before the next attempt.
            delay(__NFC_TTOKEN_RETRY__);
        }
        // Number of Trials exceeds
        #ifdef DEVEL
            mbus_write_message32(0xE0, 0x01000002);
        #endif
        return 0;
    }
    // You already got the token.
    else {
        #ifdef DEVEL
            mbus_write_message32(0xE0, 0x01000003);
        #endif
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
    #ifdef DEVEL
        mbus_write_message32(0xD0, (e2<<28)|(nb<<24)|addr);
        mbus_write_message32(0xD1, data);
    #endif
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
    #ifdef DEVEL
        mbus_write_message32(0xD2, (e2<<28)|(nb<<24)|addr);
    #endif
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
        #ifdef DEVEL
            mbus_write_message32(0xD3, data);
        #endif
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

    // NOTE: EID_CTRL and WD_CTRL use a clock whose frequency is 16x slower than the raw timer clock.
    //       The raw clock should be ~2.125kHz, thus the EID_CTRL and WD_CTRL may use ~133Hz clock.

    __eid_current_display__     = 0;
    __eid_cp_duration__         = 250;

    __crsh_seqa__.value = (/*vin*/ 0x0 << 11) | (/*te*/ 0x0 << 10) | (/*fd*/ 0x1 << 9) | (/*seg*/ 0x1FF << 0);
    __crsh_seqb__.value = (/*vin*/ 0x0 << 11) | (/*te*/ 0x1 << 10) | (/*fd*/ 0x0 << 9) | (/*seg*/ 0x000 << 0);
    __crsh_seqc__.value = (/*vin*/ 0x0 << 11) | (/*te*/ 0x0 << 10) | (/*fd*/ 0x0 << 9) | (/*seg*/ 0x018 << 0);

    eid_r00.as_int = EID_R00_DEFAULT_AS_INT;
    eid_r01.as_int = EID_R01_DEFAULT_AS_INT;
    eid_r02.as_int = EID_R02_DEFAULT_AS_INT;

    // Start EID Timer 
    eid_enable_timer();

    // Charge Pump Clock Generator
	eid_config_cp_clk_gen(/*ring*/ring, /*te_div*/te_div, /*fd_div*/fd_div, /*seg_div*/seg_div);

    // Charge Pump Pulse Width
    eid_set_duration(__eid_cp_duration__);

    // Configuration for Crash Behavior (See comment section in the top)
    //--- Watchdog Threshold; based on the 133Hz clock; Max: 16777215 (35 hours)
    mbus_remote_register_write(EID_ADDR,0x10,
                                  (16758000 << 0) // WCTR_THRESHOLD // 35 hours
                                  );

    mbus_remote_register_write(EID_ADDR,0x11,
                                  (    0x1 << 14) // WCTR_CR_ECP_PG_DIODE_A0 
                                | (    0x0 << 13) // WCTR_CR_ECP_PG_DIODE_A  
                                | (    0x0 << 12) // WCTR_CR_ECP_PG_DIODE_A1 
                                | (    0x0 << 11) // WCTR_CR_ECP_PG_DIODE_B  
                                | (    0x0 << 10) // WCTR_CR_ECP_PG_DIODE_B1 
                                | (    0x0 <<  9) // WCTR_CR_ECP_PG_DIODE_C  
                                | (    0x0 <<  8) // WCTR_CR_ECP_PG_DIODE_C1 
                                | (   ring <<  6) // WCTR_CR_ECP_SEL_RING    
                                | ( te_div <<  4) // WCTR_CR_ECP_SEL_TE_DIV  
                                | ( fd_div <<  2) // WCTR_CR_ECP_SEL_FD_DIV  
                                | (seg_div <<  0) // WCTR_CR_ECP_SEL_SEG_DIV 
                                );

    mbus_remote_register_write(EID_ADDR,0x12,
                                  (0x7 << 16) // WCTR_SEL_SEQ    (Default: 0x7) See above waveform. Each bit in [2:0] enables/disables Seq C, Seq B, Seq A, respectively. 
                                              //    If 1, the corresponding sequence is enabled. 
                                              //    If 0, the corresponding sequence is skipped.
                                | (400 <<  0) // WCTR_IDLE_WIDTH (Default: 400; Max: 65535) Duration of Idle time
                                );

    mbus_remote_register_write(EID_ADDR,0x13,
                                  (400 << 0) // WCTR_PULSE_WIDTH (Default: 400; Max: 65535) Duration of Charge Pump Activation.
                                );

    mbus_remote_register_write(EID_ADDR,0x15,
                                  (   1                                     << 23)  // WCTR_RESETB_CP_A
                                | (   __crsh_seqa__.vin                     << 22)  // WCTR_VIN_CP_A
                                | ((~(__crsh_seqa__.value & 0x7FF) & 0x7FF) << 11)  // WCTR_EN_CP_PD_A
                                | (  (__crsh_seqa__.value & 0x7FF)          <<  0)  // WCTR_EN_CP_CK_A
                                );

    mbus_remote_register_write(EID_ADDR,0x16,
                                  (   1                                     << 23)  // WCTR_RESETB_CP_B
                                | (   __crsh_seqb__.vin                     << 22)  // WCTR_VIN_CP_B
                                | ((~(__crsh_seqb__.value & 0x7FF) & 0x7FF) << 11)  // WCTR_EN_CP_PD_B
                                | (  (__crsh_seqb__.value & 0x7FF)          <<  0)  // WCTR_EN_CP_CK_B
                                );

    mbus_remote_register_write(EID_ADDR,0x17,
                                  (   1                                     << 23)  // WCTR_RESETB_CP_C
                                | (   __crsh_seqc__.vin                     << 22)  // WCTR_VIN_CP_C
                                | ((~(__crsh_seqc__.value & 0x7FF) & 0x7FF) << 11)  // WCTR_EN_CP_PD_C
                                | (  (__crsh_seqc__.value & 0x7FF)          <<  0)  // WCTR_EN_CP_CK_C
                                );
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
    delay(120000); // Wait for >3s
    eid_r01.TMR_EN_OSC = 0;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_SEL_LDO = 1; // 1: use VBAT; 0: use V1P2
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

void eid_set_duration(uint32_t duration){
    __eid_cp_duration__ = duration;
    mbus_remote_register_write(EID_ADDR,0x07,__eid_cp_duration__);
}

void eid_enable_cp_ck(uint32_t te, uint32_t fd, uint32_t seg) {
    uint32_t cp_ck = ((te << 10) | (fd << 9) | (seg << 0)) & 0x7FF;
    uint32_t cp_pd = (~cp_ck) & 0x7FF;

    // Make PG_DIODE=0
    eid_r02.ECP_PG_DIODE = 0;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);

    // Enable charge pumps
    set_halt_until_mbus_trx();
    mbus_remote_register_write(EID_ADDR,0x09,
                                  (0x1   << 23) // ECTR_RESETB_CP
                                | (0x0   << 22) // ECTR_VIN_CP
                                | (cp_pd << 11) // ECTR_EN_CP_PD
                                | (cp_ck <<  0) // ECTR_EN_CP_CK
                                );
    set_halt_until_mbus_tx();

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

void eid_update_global(uint32_t seg) { 
    // Make all black segments white
    if (__eid_current_display__!=0) eid_enable_cp_ck(0x1, 0x1, (~__eid_current_display__ & 0x1FF));
    // Make all segments/field black
    eid_enable_cp_ck(0x0, 0x1, 0x1FF);
    // Make selected segments white
    eid_enable_cp_ck(0x1, 0x0, seg & 0x1FF);
}

void eid_update_local(uint32_t seg) {
    // Erase the whole
    eid_enable_cp_ck(0x1, 0x0, 0x000);
    // Make each segment black, one at a time
    uint32_t pattern = 0x1;
    uint32_t seg_temp = seg;
    while (seg_temp!=0) {
        if (seg_temp&0x1) eid_enable_cp_ck(0x0, 0x0, pattern);
        seg_temp >>= 1;
        pattern <<= 1;
    }
    // Update __eid_current_display__
    __eid_current_display__ = seg;
}

void eid_set_current_display(uint32_t seg) {
    __eid_current_display__ = seg;
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
    snt_r08.as_int = SNT_R08_DEFAULT_AS_INT;
    snt_r09.as_int = SNT_R09_DEFAULT_AS_INT;
    snt_r17.as_int = SNT_R17_DEFAULT_AS_INT;

    // Temp Sensor Initialization
    snt_r01.TSNS_RESETn     = 0;
    snt_r01.TSNS_EN_IRQ     = 1;
    snt_r01.TSNS_BURST_MODE = 0;
    snt_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

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
        | (0x7 <<  0)   // 0x6      // TSNS_SEL_CONV_TIME    
        ));

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
    snt_r01.TSNS_SEL_V1P2 = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
    // Turn on analog block
    snt_r01.TSNS_EN_SENSOR_V1P2 = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
    // Delay (~2ms @ 100kHz clock speed)
    delay(200);
}

void snt_temp_sensor_power_off(void){
    snt_r01.TSNS_RESETn         = 0;
    snt_r01.TSNS_SEL_V1P2       = 0;
    snt_r01.TSNS_EN_SENSOR_V1P2 = 0;
    snt_r01.TSNS_ISOLATE        = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

void snt_temp_sensor_start(void){
    snt_r01.TSNS_RESETn  = 1;
    snt_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

void snt_temp_sensor_reset(void){
    snt_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

void snt_ldo_vref_on(void){
    snt_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
    // Delay (~50ms @ 100kHz clock speed); NOTE: Start-Up Time of VREF @ TT, 1.1V, 27C is ~40ms.
    delay(5000);
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

//-------------------------------------------------------------------
// Temperature Conversion
//-------------------------------------------------------------------

//-------------------------
// Index       MSBs    Hex
// -----------------------
//   0 -   91  3'b000  0x0
//  92 -  192  3'b001  0x1
// 193 -  302  3'b010  0x2
// 303 -  422  3'b011  0x3
// 423 -  553  3'b100  0x4
// 554 -  696  3'b101  0x5
// 697 -  852  3'b110  0x6
// 853 - 1022  3'b111  0x7
//-------------------------

// tconv_log2_lut[] provides the LSB only.
// You need to prepend the MSBs shown above,
// which is handled in tconv_log2()
volatile uint32_t tconv_log2_lut[] = {
    // Index    Value           Table Index
    /*   0 */   0x0C090603, //    3 -    0
    /*   1 */   0x1714110E, //    7 -    4
    /*   2 */   0x22201D1A, //   11 -    8
    /*   3 */   0x2E2B2825, //   15 -   12
    /*   4 */   0x39363331, //   19 -   16
    /*   5 */   0x44423F3C, //   23 -   20
    /*   6 */   0x504D4A47, //   27 -   24
    /*   7 */   0x5B585553, //   31 -   28
    /*   8 */   0x6663615E, //   35 -   32
    /*   9 */   0x716E6C69, //   39 -   36
    /*  10 */   0x7C7A7774, //   43 -   40
    /*  11 */   0x8785827F, //   47 -   44
    /*  12 */   0x92908D8A, //   51 -   48
    /*  13 */   0x9D9B9895, //   55 -   52
    /*  14 */   0xA8A6A3A0, //   59 -   56
    /*  15 */   0xB3B0AEAB, //   63 -   60
    /*  16 */   0xBEBBB9B6, //   67 -   64
    /*  17 */   0xC9C6C3C1, //   71 -   68
    /*  18 */   0xD4D1CECB, //   75 -   72
    /*  19 */   0xDEDCD9D6, //   79 -   76
    /*  20 */   0xE9E6E4E1, //   83 -   80
    /*  21 */   0xF4F1EEEC, //   87 -   84
    /*  22 */   0xFEFCF9F6, //   91 -   88
    /*  23 */   0x09060301, //   95 -   92
    /*  24 */   0x13110E0B, //   99 -   96
    /*  25 */   0x1E1B1916, //  103 -  100
    /*  26 */   0x28262320, //  107 -  104
    /*  27 */   0x33302D2B, //  111 -  108
    /*  28 */   0x3D3A3835, //  115 -  112
    /*  29 */   0x47454240, //  119 -  116
    /*  30 */   0x524F4D4A, //  123 -  120
    /*  31 */   0x5C595754, //  127 -  124
    /*  32 */   0x6664615F, //  131 -  128
    /*  33 */   0x706E6B69, //  135 -  132
    /*  34 */   0x7B787673, //  139 -  136
    /*  35 */   0x8582807D, //  143 -  140
    /*  36 */   0x8F8C8A87, //  147 -  144
    /*  37 */   0x99969491, //  151 -  148
    /*  38 */   0xA3A09E9B, //  155 -  152
    /*  39 */   0xADAAA8A5, //  159 -  156
    /*  40 */   0xB7B4B2AF, //  163 -  160
    /*  41 */   0xC1BEBCB9, //  167 -  164
    /*  42 */   0xCBC8C6C3, //  171 -  168
    /*  43 */   0xD5D2D0CD, //  175 -  172
    /*  44 */   0xDEDCDAD7, //  179 -  176
    /*  45 */   0xE8E6E3E1, //  183 -  180
    /*  46 */   0xF2F0EDEB, //  187 -  184
    /*  47 */   0xFCF9F7F4, //  191 -  188
    /*  48 */   0x050301FE, //  195 -  192
    /*  49 */   0x0F0D0A08, //  199 -  196
    /*  50 */   0x19161412, //  203 -  200
    /*  51 */   0x22201E1B, //  207 -  204
    /*  52 */   0x2C2A2725, //  211 -  208
    /*  53 */   0x3633312E, //  215 -  212
    /*  54 */   0x3F3D3A38, //  219 -  216
    /*  55 */   0x49464441, //  223 -  220
    /*  56 */   0x52504D4B, //  227 -  224
    /*  57 */   0x5B595754, //  231 -  228
    /*  58 */   0x6562605E, //  235 -  232
    /*  59 */   0x6E6C6967, //  239 -  236
    /*  60 */   0x77757370, //  243 -  240
    /*  61 */   0x817E7C7A, //  247 -  244
    /*  62 */   0x8A888583, //  251 -  248
    /*  63 */   0x93918F8C, //  255 -  252
    /*  64 */   0x9D9A9896, //  259 -  256
    /*  65 */   0xA6A3A19F, //  263 -  260
    /*  66 */   0xAFADAAA8, //  267 -  264
    /*  67 */   0xB8B6B3B1, //  271 -  268
    /*  68 */   0xC1BFBDBA, //  275 -  272
    /*  69 */   0xCAC8C6C3, //  279 -  276
    /*  70 */   0xD3D1CFCC, //  283 -  280
    /*  71 */   0xDCDAD8D6, //  287 -  284
    /*  72 */   0xE5E3E1DF, //  291 -  288
    /*  73 */   0xEEECEAE8, //  295 -  292
    /*  74 */   0xF7F5F3F0, //  299 -  296
    /*  75 */   0x00FEFCF9, //  303 -  300
    /*  76 */   0x09070502, //  307 -  304
    /*  77 */   0x12100D0B, //  311 -  308
    /*  78 */   0x1B181614, //  315 -  312
    /*  79 */   0x23211F1D, //  319 -  316
    /*  80 */   0x2C2A2826, //  323 -  320
    /*  81 */   0x3533312E, //  327 -  324
    /*  82 */   0x3E3C3937, //  331 -  328
    /*  83 */   0x46444240, //  335 -  332
    /*  84 */   0x4F4D4B49, //  339 -  336
    /*  85 */   0x58565351, //  343 -  340
    /*  86 */   0x605E5C5A, //  347 -  344
    /*  87 */   0x69676563, //  351 -  348
    /*  88 */   0x726F6D6B, //  355 -  352
    /*  89 */   0x7A787674, //  359 -  356
    /*  90 */   0x83817E7C, //  363 -  360
    /*  91 */   0x8B898785, //  367 -  364
    /*  92 */   0x94928F8D, //  371 -  368
    /*  93 */   0x9C9A9896, //  375 -  372
    /*  94 */   0xA5A2A09E, //  379 -  376
    /*  95 */   0xADABA9A7, //  383 -  380
    /*  96 */   0xB5B3B1AF, //  387 -  384
    /*  97 */   0xBEBCB9B7, //  391 -  388
    /*  98 */   0xC6C4C2C0, //  395 -  392
    /*  99 */   0xCECCCAC8, //  399 -  396
    /* 100 */   0xD7D5D2D0, //  403 -  400
    /* 101 */   0xDFDDDBD9, //  407 -  404
    /* 102 */   0xE7E5E3E1, //  411 -  408
    /* 103 */   0xEFEDEBE9, //  415 -  412
    /* 104 */   0xF8F5F3F1, //  419 -  416
    /* 105 */   0x00FEFCFA, //  423 -  420
    /* 106 */   0x08060402, //  427 -  424
    /* 107 */   0x100E0C0A, //  431 -  428
    /* 108 */   0x18161412, //  435 -  432
    /* 109 */   0x201E1C1A, //  439 -  436
    /* 110 */   0x28262422, //  443 -  440
    /* 111 */   0x302E2C2A, //  447 -  444
    /* 112 */   0x38363432, //  451 -  448
    /* 113 */   0x403E3C3A, //  455 -  452
    /* 114 */   0x48464442, //  459 -  456
    /* 115 */   0x504E4C4A, //  463 -  460
    /* 116 */   0x58565452, //  467 -  464
    /* 117 */   0x605E5C5A, //  471 -  468
    /* 118 */   0x68666462, //  475 -  472
    /* 119 */   0x706E6C6A, //  479 -  476
    /* 120 */   0x78767472, //  483 -  480
    /* 121 */   0x7F7E7C7A, //  487 -  484
    /* 122 */   0x87858381, //  491 -  488
    /* 123 */   0x8F8D8B89, //  495 -  492
    /* 124 */   0x97959391, //  499 -  496
    /* 125 */   0x9F9D9B99, //  503 -  500
    /* 126 */   0xA6A4A2A1, //  507 -  504
    /* 127 */   0xAEACAAA8, //  511 -  508
    /* 128 */   0xB6B4B2B0, //  515 -  512
    /* 129 */   0xBDBBBAB8, //  519 -  516
    /* 130 */   0xC5C3C1BF, //  523 -  520
    /* 131 */   0xCDCBC9C7, //  527 -  524
    /* 132 */   0xD4D2D0CF, //  531 -  528
    /* 133 */   0xDCDAD8D6, //  535 -  532
    /* 134 */   0xE3E1E0DE, //  539 -  536
    /* 135 */   0xEBE9E7E5, //  543 -  540
    /* 136 */   0xF2F1EFED, //  547 -  544
    /* 137 */   0xFAF8F6F4, //  551 -  548
    /* 138 */   0x0100FEFC, //  555 -  552
    /* 139 */   0x09070503, //  559 -  556
    /* 140 */   0x100F0D0B, //  563 -  560
    /* 141 */   0x18161412, //  567 -  564
    /* 142 */   0x1F1D1C1A, //  571 -  568
    /* 143 */   0x27252321, //  575 -  572
    /* 144 */   0x2E2C2A28, //  579 -  576
    /* 145 */   0x35343230, //  583 -  580
    /* 146 */   0x3D3B3937, //  587 -  584
    /* 147 */   0x4442403F, //  591 -  588
    /* 148 */   0x4B494846, //  595 -  592
    /* 149 */   0x53514F4D, //  599 -  596
    /* 150 */   0x5A585654, //  603 -  600
    /* 151 */   0x615F5E5C, //  607 -  604
    /* 152 */   0x68676563, //  611 -  608
    /* 153 */   0x706E6C6A, //  615 -  612
    /* 154 */   0x77757371, //  619 -  616
    /* 155 */   0x7E7C7A79, //  623 -  620
    /* 156 */   0x85838280, //  627 -  624
    /* 157 */   0x8C8A8987, //  631 -  628
    /* 158 */   0x9392908E, //  635 -  632
    /* 159 */   0x9B999795, //  639 -  636
    /* 160 */   0xA2A09E9C, //  643 -  640
    /* 161 */   0xA9A7A5A3, //  647 -  644
    /* 162 */   0xB0AEACAA, //  651 -  648
    /* 163 */   0xB7B5B3B1, //  655 -  652
    /* 164 */   0xBEBCBAB9, //  659 -  656
    /* 165 */   0xC5C3C1C0, //  663 -  660
    /* 166 */   0xCCCAC8C7, //  667 -  664
    /* 167 */   0xD3D1CFCE, //  671 -  668
    /* 168 */   0xDAD8D6D5, //  675 -  672
    /* 169 */   0xE1DFDDDB, //  679 -  676
    /* 170 */   0xE8E6E4E2, //  683 -  680
    /* 171 */   0xEFEDEBE9, //  687 -  684
    /* 172 */   0xF5F4F2F0, //  691 -  688
    /* 173 */   0xFCFBF9F7, //  695 -  692
    /* 174 */   0x030100FE, //  699 -  696
    /* 175 */   0x0A080705, //  703 -  700
    /* 176 */   0x110F0D0C, //  707 -  704
    /* 177 */   0x18161413, //  711 -  708
    /* 178 */   0x1E1D1B19, //  715 -  712
    /* 179 */   0x25242220, //  719 -  716
    /* 180 */   0x2C2A2927, //  723 -  720
    /* 181 */   0x33312F2E, //  727 -  724
    /* 182 */   0x3A383634, //  731 -  728
    /* 183 */   0x403F3D3B, //  735 -  732
    /* 184 */   0x47454442, //  739 -  736
    /* 185 */   0x4E4C4A49, //  743 -  740
    /* 186 */   0x5453514F, //  747 -  744
    /* 187 */   0x5B595856, //  751 -  748
    /* 188 */   0x62605E5D, //  755 -  752
    /* 189 */   0x68676563, //  759 -  756
    /* 190 */   0x6F6D6C6A, //  763 -  760
    /* 191 */   0x75747271, //  767 -  764
    /* 192 */   0x7C7A7977, //  771 -  768
    /* 193 */   0x83817F7E, //  775 -  772
    /* 194 */   0x89888684, //  779 -  776
    /* 195 */   0x908E8C8B, //  783 -  780
    /* 196 */   0x96959391, //  787 -  784
    /* 197 */   0x9D9B9A98, //  791 -  788
    /* 198 */   0xA3A2A09E, //  795 -  792
    /* 199 */   0xAAA8A7A5, //  799 -  796
    /* 200 */   0xB0AFADAB, //  803 -  800
    /* 201 */   0xB7B5B3B2, //  807 -  804
    /* 202 */   0xBDBCBAB8, //  811 -  808
    /* 203 */   0xC4C2C0BF, //  815 -  812
    /* 204 */   0xCAC8C7C5, //  819 -  816
    /* 205 */   0xD0CFCDCC, //  823 -  820
    /* 206 */   0xD7D5D4D2, //  827 -  824
    /* 207 */   0xDDDCDAD8, //  831 -  828
    /* 208 */   0xE4E2E0DF, //  835 -  832
    /* 209 */   0xEAE8E7E5, //  839 -  836
    /* 210 */   0xF0EFEDEB, //  843 -  840
    /* 211 */   0xF7F5F3F2, //  847 -  844
    /* 212 */   0xFDFBFAF8, //  851 -  848
    /* 213 */   0x030200FE, //  855 -  852                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 214 */   0x09080605, //  859 -  856                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 215 */   0x100E0D0B, //  863 -  860                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 216 */   0x16141311, //  867 -  864                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 217 */   0x1C1B1917, //  871 -  868                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 218 */   0x22211F1E, //  875 -  872                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 219 */   0x29272524, //  879 -  876                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 220 */   0x2F2D2C2A, //  883 -  880                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    /* 221 */   0x35333230, //  887 -  884                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    /* 222 */   0x3B3A3837, //  891 -  888                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    /* 223 */   0x41403E3D, //  895 -  892                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    /* 224 */   0x47464443, //  899 -  896                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    /* 225 */   0x4E4C4B49, //  903 -  900                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 226 */   0x5452514F, //  907 -  904                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 227 */   0x5A585755, //  911 -  908                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 228 */   0x605E5D5B, //  915 -  912                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 229 */   0x66646361, //  919 -  916                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 230 */   0x6C6B6968, //  923 -  920                                                                                                                                                                                                                                                                                                                                                                                                                                    
    /* 231 */   0x72716F6E, //  927 -  924                                                                                                                                                                                                                                                                                                                                                                                                                                    
    /* 232 */   0x78777574, //  931 -  928                                                                                                                                                                                                                                                                                             
    /* 233 */   0x7E7D7B7A, //  935 -  932
    /* 234 */   0x84838180, //  939 -  936
    /* 235 */   0x8A898786, //  943 -  940
    /* 236 */   0x908F8D8C, //  947 -  944
    /* 237 */   0x96959392, //  951 -  948
    /* 238 */   0x9C9B9998, //  955 -  952
    /* 239 */   0xA2A19F9E, //  959 -  956
    /* 240 */   0xA8A7A5A4, //  963 -  960
    /* 241 */   0xAEADABAA, //  967 -  964
    /* 242 */   0xB4B3B1B0, //  971 -  968
    /* 243 */   0xBAB8B7B5, //  975 -  972
    /* 244 */   0xC0BEBDBB, //  979 -  976
    /* 245 */   0xC6C4C3C1, //  983 -  980
    /* 246 */   0xCCCAC9C7, //  987 -  984
    /* 247 */   0xD1D0CFCD, //  991 -  988
    /* 248 */   0xD7D6D4D3, //  995 -  992
    /* 249 */   0xDDDCDAD9, //  999 -  996
    /* 250 */   0xE3E2E0DF, // 1003 - 1000
    /* 251 */   0xE9E7E6E4, // 1007 - 1004
    /* 252 */   0xEFEDECEA, // 1011 - 1008
    /* 253 */   0xF4F3F2F0, // 1015 - 1012
    /* 254 */   0xFAF9F7F6, // 1019 - 1016
    /* 255 */   0x00FFFDFC, // 1023 - 1020 (1023 is dummy)
    };


uint32_t tconv_log2 (uint32_t idx) {
    uint32_t msb, lsb;

    if (idx==1023) return 0;

    lsb = (tconv_log2_lut[idx>>2]>>((idx&0x3)<<3))&0xFF;

    if      (idx <  92) msb = 0x0;
    else if (idx < 193) msb = 0x1;
    else if (idx < 303) msb = 0x2;
    else if (idx < 423) msb = 0x3;
    else if (idx < 554) msb = 0x4;
    else if (idx < 697) msb = 0x5;
    else if (idx < 853) msb = 0x6;
    else                msb = 0x7;

    return (msb<<8)|lsb;
}

uint32_t tconv (uint32_t dout, uint32_t a, uint32_t b, uint32_t offset) {
    // NOTE: dout is a 16-bit raw code
    //       a is a fixed-point number with N=10 accuracy
    //       b is a fixed-point number with N=10 accuracy

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA0, dout);
    mbus_write_message32(0xA0, a);
    mbus_write_message32(0xA0, b);
    mbus_write_message32(0xA0, offset);
#endif

    // If dout=0, just return 0.
    if (dout==0) return 0;

    // Find out the position of leading 1
    uint32_t m = 16;
    while (m>0) {
        m--;
        if (dout&(0x1<<m)) break;
    }

    if (m>10) {
        if ((dout>>(m-11))&0x1) { 
            dout += (0x1<<(m-11));
            if (((dout>>(m-10))&0x3FF)==0) 
                m++;
        }
    }

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA1, m);
#endif

    // Calculate log2(dout)
    // NOTE: idx=1023 returns 0 (i.e., table[1023]=0)
    uint32_t idx;
    //--- If dout is a power of 2
    if ((dout&(~(0x1<<m)))==0) {
        idx = 1024;
    }
    //--- If dout is not a power of 2
    else {
        // CASE III) m>10
        if (m>10) {
            idx = ((dout>>(m-10))&0x3FF);
            if (idx==0) idx=1024;
        }
        // CASE I) m=10 or CASE II) m<10
        else {
            idx = ((dout<<(10-m))&0x3FF);
        }
    }
    // Decrement idx
    idx--;

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA2, idx);
#endif

    // NOTE: log2_dout is N=11 accuracy
    uint32_t log2_dout = (m<<11) + tconv_log2(idx);

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA3, log2_dout);
#endif

    // NOTE: c = log2(Fclk / 2^(ConvTime+5)) = 2.09506730160705 (for Fclk=17500, ConvTime=7)
    //         = ~2.0947265625
    //         = 0000_0000_0000_0000_0000_1000_0110_0001 (N=10 fixed-point)
    uint32_t c = 0x00000861;

    // Division:  d = b / (a - c - log2_dout)
    // NOTE: a, b, c have N=10 accuracy, but log2_dout has N=11 accuracy
    //       -> Multiply a, b, c by 2.
    // NOTE: temp_n11 has N=11 accuracy.
    uint32_t temp_n11 = div(/*dividend*/b<<1, /*divisor*/(a<<1)-(c<<1)-log2_dout, /*n*/11);

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA4, temp_n11);
#endif

    // temp10 = (temp_n11 x 10) with decimal points truncated
    uint32_t temp10 = ((temp_n11<<3) + (temp_n11<<1));
    if ((temp10>>10)&0x1) temp10+=(0x1<<10); // (optional) Add a round-up
    temp10 = temp10 >> 11;  // truncate the decimal points

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA5, temp10);
#endif

    // 'result' subtracts the offset from temp10, 
    uint32_t offset_10k = (offset<<3) + (offset<<1);
    if (temp10<offset_10k) temp10 = offset_10k;
    uint32_t result = temp10 - offset_10k;

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA6, result);
#endif

    return result&0xFFFF;  // 10x(T+273-OFFSET)
}

