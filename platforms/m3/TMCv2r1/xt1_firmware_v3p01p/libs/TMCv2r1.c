//*******************************************************************************************
// TMC SOURCE FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//      PREv23E -> EIDv1 -> SNTv6 -> PMUv13r1
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*********************************************************
// HEADER AND OTHER SOURCE FILES
//*********************************************************
#include "TMCv2r1.h"

//*******************************************************************
// TMCv2r1 FUNCTIONS
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
//        mbus_write_message32(0xC6, numer);
//        mbus_write_message32(0xC6, denom);
//        mbus_write_message32(0xC6, n);
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
//        mbus_write_message32(0xC7, result);
    #endif
    return result;
}

uint32_t mult(uint32_t num_a, uint32_t num_b) {
    #ifdef DEVEL
//        mbus_write_message32(0xC0, num_a);
//        mbus_write_message32(0xC0, num_b);
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
//        mbus_write_message32(0xC1, result);
    #endif
    return result;
}

uint32_t mult_b28 (uint32_t num_a, uint32_t num_b) {
// Number of decimal digits in num_b
#define __N_B__ 28

    #ifdef DEVEL
//        mbus_write_message32(0xC2, num_a);
//        mbus_write_message32(0xC2, num_b);
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
//        mbus_write_message32(0xC3, result);
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
// PRE FUNCTIONS (Specific to TMCv2r1)
//*******************************************************************

void restart_xo(uint32_t delay_a, uint32_t delay_b) {
    //--- Stop XO
    xo_stop();

    //--- Start XO clock (NOTE: xo_stop() disables both the clock and the timer)
    xo_start(/*delay_a*/delay_a, /*delay_b*/delay_b);

    //--- Configure and Start the XO Counter
    set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0, /*auto_reset*/0);
    start_xo_cnt();
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
    start_timeout32_check(/*id*/FAIL_ID_PMU, /*val*/TIMEOUT_TH);
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
    pmu_set_floor(PMU_ACTIVE, 0x1, 0x0, 0x1, 0x0);
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

    //--------------------------------------------------------------------------------------------------------------------------------
    //  New SAR Ratio Calculation
    //--------------------------------------------------------------------------------------------------------------------------------
    //  
    //  Relationship among VBAT, SAR, ADC, VREF, V1P2 are given as below.
    //
    //      (1)  (SAR / 128) x VBAT = V1P2
    //      (2)  VBAT = (ADC x VREF x 4) / 256
    //
    //  Plugging in (2) into (1) gives
    //
    //      (3)  (SAR / 128) x (ADC x VREF x 4) / 256 = V1P2
    //          -> (SAR x ADC) / 8192 = V1P2 / VREF
    //
    //  Thus, in order to guarantee V1P2 > VREF, the following condition must be met:
    //
    //      (4)  SAR > 8192 / ADC
    //
    //  For n% margin, let's define M as below:
    //
    //      (5) M = 1 + n/100
    //
    //  Then, SAR can be calculated as below to include the margin.
    //
    //      (6)  SAR = (8192 x M) / ADC 
    //
    //  However, we have seen that VDDs drop more as VBAT goes lower (i.e., as SAR becomes higher)
    //  This would be because the SAR converter has a worse efficiency at higher SAR ratios.
    //  
    //  Here we are trying a SAR compensation through an 'empirical' fitting.
    //
    //  From measurement, V1P2 seems to have a linear relationship with VBAT.
    //  (NOTE: Theoretically, V1P2 follows VREF. It seems like VREF has a slight line sensitivity.)
    //
    //  Thus, we can model V1P2 as below
    //
    //      (7) V1P2 = a x VBAT + b , where a and b are empirical constants.
    //
    //  Let's say Vo is the 'desired' V1P2 level, which is constant over VBAT.
    //  We need to add C (shown below) to V1P2 to make V1P2 same as Vo.
    //
    //      (8) C = M x Vo - V1P2 = M x Vo - (a x VBAT + b)
    //
    //  In order to change V1P2, we need to change SAR. 
    //  The amount of change in V1P2 caused by +1 change SAR is V1P2/SAR,
    //  which can be written as 
    //
    //      (9) V1P2 / SAR = (a x VBAT + b) / SAR
    //
    //  Thus, we need to change SAR by delta_SAR as shown below.
    //
    //      (10) delta_SAR  = C / (V1P2 / SAR) = (M x Vo - (a x VBAT + b)) / ((a x VBAT + b) / (8192 x M / ADC))
    //                      = (8192 x M / ADC) x [(M x Vo / (a x VBAT + b)) - 1]
    //
    //  Now, the new SAR can be written as below.
    //
    //      (11) new_SAR = SAR + delta_SAR = (8192 x M / ADC) + (8192 x M / ADC) x [(M x Vo / (a x VBAT + b)) - 1]
    //                   = (8192 x M / ADC) x ( M x Vo / (a x VBAT + b))
    //
    //  Using (2), it can be written as below.
    //  
    //      (12) new_SAR = (8192 x M / ADC) x ( 64 x M x Vo / (a x VREF x ADC + 64 x b))
    //
    //  a and b values can be found using the method below.
    //
    //      (A) Use the equation (6) and measure V1P2 for VBAT=[2V, 3V]. Use 50mV step for VBAT sweep.
    //      (B) Let VBATn = 2 + 0.05 x n where n = 0, 1, 2, ..., 20.
    //      (C) Let V1P2n = Measured V1P2 @ VBATn where n = 0, 1, 2, ..., 20.
    //      (D) The modeling error is given as Sum(| V1P2n - (a x VBATn + b) |^2).
    //      (E) Take a derivative of Eq (D) for a and b. And find a and b values to make them 0. i.e., 
    //              d(Sum(| V1P2n - (a x VBATn + b) |^2)) / d(a) = 0
    //              d(Sum(| V1P2n - (a x VBATn + b) |^2)) / d(b) = 0
    //
    //  The solution is:
    //
    //      (13) a = (X x Y - N x XY) / (X x X - N x X2)
    //           b = (Y - a x X) / N
    //              where
    //                  N  = 21 (i.e., number of the measurement points)
    //                  X  = Sum(VBATn) for n=0, 1, ..., 20
    //                  Y  = Sum(V1P2n) for n=0, 1, ..., 20
    //                  XY = Sum(VBATn x V1P2n) for n=0, 1, ..., 20
    //                  X2 = Sum(VBATn x VBATn) for n=0, 1, ..., 20
    //  
    //  Now we have the equation to calculate the new SAR (Eq (12)), and the a and b values (Eq. (13))
    //  However, the equation is too complicated, making it a very expensive operation.
    //
    //  Thus, we will try the Taylor Series approximation.
    //
    //      (14) approx_SAR = C0 + C1 x (ADC - A0) + C2 x (ADC - A0)^2 ...
    //
    //  where C0 = f(A0), C1 = f'(A0), C2 = f''(A0)/2! ..., where f(ADC) indicates Eq. (12).
    //  A0 is an arbitrary ADC value that makes the approximation (Eq. (14)) have the least error.
    //
    //  Finding out C0 ~ C2 is a lengthy analytical process. Let's first define some intermediate constants & variables as below:
    //
    //      (15) j = 8192 x 64 x (M^2) x Vo
    //           k = a x VREF
    //           m = 64 x b
    //           n = k x (A0^2) + m x A0
    //           p = 2 x k x A0 + m
    //
    //  Solving for C0 ~ C2 results in
    //
    //      (16) C0 = j / n
    //           C1 = -1 x j x p / (n^2)
    //           C2 = j x (p^2) / (n^3) - j x k / (n^2)
    //
    //  We do not use a higher order since using C0 ~ C2 is sufficient. The approximation error is less than 1.
    //
    //  We use the following values
    //
    //      (17) VREF = 1.3
    //           Vo   = 1.4
    //           A0   = 115
    //
    //  We use 10-fp for M, and 16-fp for C0, C1, C2. Table 1 shows C0 ~ C2 for various M values.
    //
    //      [TABLE 1] C0 ~ C2 values with 16-fp. NOTE: C0 > 0, C1 < 0, C2 > 0.
    //          
    //          -------------------------------------------------
    //              Margin      C0          |C1|        C2
    //          -------------------------------------------------
    //              0%          0x4D5CCE    0x0C536     0x1BF
    //              5%          0x536778    0x0D5DB     0x1E5
    //              6%          0x545DCE    0x0D4AD     0x1E0
    //              7%          0x5516E1    0x0DC1D     0x1F5
    //              8%          0x559E24    0x0DC93     0x1F5
    //              9%          0x56A605    0x0E0B7     0x1FF
    //              10%         0x57A761    0x0E760     0x212
    //              15%         0x5BBA07    0x0EED2     0x220
    //              20%         0x602E9D    0x100C9     0x24E
    //              25%         0x64E205    0x11373     0x27E
    //          -------------------------------------------------
    //
    //          (NOTE) Some of the values above were calculated with slightly different calculations,
    //                 thus including some minor (negligible) error.
    //
    //  Furthermore, we limit the min. SAR value at 64, since SAR < 64 *always* causes a high current (i.e., run-away)
    //
    //  [Reference] CubeWorks, Inc. Dropbox/Yejoong Kim/Designs/202106_XT1_Firmware/sar_and_vdd/auto_sar_ratio_adjustment.pptx
    //              CubeWorks, Inc. Dropbox/Yejoong Kim/Designs/202106_XT1_Firmware/sar_and_vdd/auto_sar_comparison.xlsx
    //              NOTE: The algorithm shown here is called an empirical 'Option 3 with 2-pt meas.' in the above docs.
    //
    //--------------------------------------------------------------------------------------------------------------------------------
    //  Written on August 22, 2022
    //  Yejoong Kim (yejoong@cubeworks.io)
    //--------------------------------------------------------------------------------------------------------------------------------

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

            // Center Value
            uint32_t cent_val = 115;
            uint32_t c0, c1, c2;

          //             /*default*/  c0 = 0x559E24; c1 = 0x0DC93; c2 = 0x1F5;      // 8%
          //if      (sel_margin==1) { c0 = 0x56A605; c1 = 0x0E0B7; c2 = 0x1FF; }    // 9%
          //else if (sel_margin==2) { c0 = 0x536778; c1 = 0x0D5DB; c2 = 0x1E5; }    // 5%  
          //else if (sel_margin==3) { c0 = 0x5516E1; c1 = 0x0DC1D; c2 = 0x1F5; }    // 7%  
          //else if (sel_margin==4) { c0 = 0x57A761; c1 = 0x0E760; c2 = 0x212; }    // 10% 
          //else if (sel_margin==4) { c0 = 0x5BBA07; c1 = 0x0EED2; c2 = 0x220; }    // 15%
          c0 = 0x57A761; c1 = 0x0E760; c2 = 0x212; // 10% 

            // Taylor Series Implementation
            uint32_t pwr1, pwr1_sign;
            if (adc_val < cent_val) { pwr1_sign = 0; pwr1 = cent_val - adc_val;}
            else                    { pwr1_sign = 1; pwr1 = adc_val - cent_val;}
            uint32_t pwr2 = mult(/*num_a*/pwr1, /*num_b*/pwr1);
            if (pwr1_sign) new_val = c0 - mult(/*num_a*/c1, /*num_b*/pwr1) + mult(/*num_a*/c2, /*num_b*/pwr2);
            else           new_val = c0 + mult(/*num_a*/c1, /*num_b*/pwr1) + mult(/*num_a*/c2, /*num_b*/pwr2);
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

void pmu_normal_sleep_floor(void) {
    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x15,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 0 << 19)    // 1'h0     // Enable PFM even during periodic reset
        | ( 0 << 18)    // 1'h0     // Enable PFM even when VREF is not used as reference
        | ( 0 << 17)    // 1'h0     // Enable PFM
        | ( 1 << 14)    // 3'h3     // Comparator clock division ratio (0x1 being slowest)
        | ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (15 <<  9)    // 4'h8     // Frequency multiplier R
        | ( 1 <<  5)    // 4'h8     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x17,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 3 << 14)    // 2'h0     // Desired Vout/Vin ratio
        | ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | ( 1 <<  9)    // 4'h8     // Frequency multiplier R
        | (12 <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x19,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | ( 1 <<  9)    // 4'h8     // Frequency multiplier R
        | (12 <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h08    // Floor frequency base (0-63)
    ));
}

void pmu_temp_sleep_floor(void) {
    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x15,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 0 << 19)    // 1'h0     // Enable PFM even during periodic reset
        | ( 0 << 18)    // 1'h0     // Enable PFM even when VREF is not used as reference
        | ( 0 << 17)    // 1'h0     // Enable PFM
        | ( 1 << 14)    // 3'h3     // Comparator clock division ratio (0x1 being slowest)
        | ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (15 <<  9)    // 4'h8     // Frequency multiplier R
        | ( 1 <<  5)    // 4'h8     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x17,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 3 << 14)    // 2'h0     // Desired Vout/Vin ratio
        | ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | ( 1 <<  9)    // 4'h8     // Frequency multiplier R
        | (12 <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_SLEEP
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x19,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | ( 1 <<  9)    // 4'h8     // Frequency multiplier R
        | (12 <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h08    // Floor frequency base (0-63)
    ));
}

void pmu_normal_active_floor(void) {
    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_ACTIVE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x16,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 0 << 19)    // 1'h0     // Enable PFM even during periodic reset
        | ( 0 << 18)    // 1'h0     // Enable PFM even when VREF is not used as reference
        | ( 0 << 17)    // 1'h0     // Enable PFM
        | ( 1 << 14)    // 3'h3     // Comparator clock division ratio (0x1 being slowest)
        | ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | (15 <<  9)    // 4'h8     // Frequency multiplier R
        | ( 1 <<  5)    // 4'h8     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_ACTIVE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x18,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 3 << 14)    // 2'h0     // Desired Vout/Vin ratio
        | ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | ( 1 <<  9)    // 4'h8     // Frequency multiplier R
        | (12 <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_ACTIVE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x1A,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( ( 0 << 13)    // 1'h0     // Makes the converter clock 2x slower
        | ( 1 <<  9)    // 4'h8     // Frequency multiplier R
        | (12 <<  5)    // 4'h4     // Frequency multiplier L (actually L+1)
        | (16      )    // 5'h08    // Floor frequency base (0-63)
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
//        pmu_set_active_temp_based();
//        pmu_set_sleep_temp_based();
    #else
//        pmu_set_floor(PMU_ACTIVE, 
//                        /*R*/       0xF,
//                        /*L*/       0x3,
//                        /*BASE*/    0x0F,
//                        /*L_SAR*/   0x7
//                        );
//        pmu_set_floor(PMU_SLEEP, 
//                        /*R*/       0xF,
//                        /*L*/       0x0,
//                        /*BASE*/    0x01,
//                        /*L_SAR*/   0x1
//                        );
    #endif

    // Floor Setting
    pmu_normal_active_floor();
    pmu_normal_sleep_floor();

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
        | (0x0 << 14)   // 0x3      // Comparator clock division ratio
        | (0x0 << 13)   // 0x0      // Makes the converter clock 2x slower
        | (0xF <<  9)   // 0x2      // Frequency multiplier R
        | (0x0 <<  5)   // 0x2      // Frequency multiplier L
        | (0x01)        // 0x6      // Floor frequency base
        ));
    
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
    //__i2c_password__.upper = 0x0;
    //__i2c_password__.lower = 0x0;
    __i2c_password__.value = UINT64_C(0);

    // Initialize the status
    __nfc_on__ = 0x0;
    __nfc_i2c_token__ = 0x0;
    __nfc_i2c_nak__ = 0x0;

    #ifndef __USE_HCODE__
        // Initialize the direction (*R_GPIO_DIR needs to be explicitly reset)
        gpio_set_dir(0x0);
        // Enable the GPIO pads
        set_gpio_pad_with_mask(__I2C_MASK__,(1<<__NFC_SDA__)|(1<<__NFC_SCL__));
        set_gpio_pad_with_mask(__GPO_MASK__,(1<<__NFC_GPO__));
    #else
        // Initialize the direction (*R_GPIO_DIR needs to be explicitly reset)
        *R_GPIO_DIR = 0x0;
        *GPIO_DIR = *R_GPIO_DIR;
        // Enable the GPIO pads
        *REG_PERIPHERAL = (*REG_PERIPHERAL & ~__NFC_MASK__) | (__NFC_MASK__ & ((1<<__NFC_SDA__)|(1<<__NFC_SCL__)|(1<<__NFC_GPO__)));
    #endif


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
        gpio_set_dir((1<<__NFC_SCL__)|(1<<__NFC_SDA__));

        // Write SCL=SDA=1
        gpio_write_data((1<<__NFC_SCL__)|(1<<__NFC_SDA__));

        // Power the NFC chip
        // NOTE: At this point, SCL/SDA are still in 'input' mode as GPIO pads are still frozen.
        //       As VNFC goes high, SDA also goes high by the pull-up resistor. SCL remains low.
        set_cps(/*cps_full*/0x0,              /*cps_weak*/0x1<<__NFC_CPS__);
        delay(10);
        set_cps(/*cps_full*/0x1<<__NFC_CPS__, /*cps_weak*/0x1<<__NFC_CPS__);

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
        set_cps(/*cps_full*/0x0, /*cps_weak*/0x0);
        // Write SCL=SDA=0
        gpio_write_data((0<<__NFC_SCL__)|(0<<__NFC_SDA__));
        // Set direction (Direction- 1: output, 0: input)
        gpio_set_dir((0<<__NFC_SCL__)|(0<<__NFC_SDA__));
        // Freeze the pads
        freeze_gpio_out();
        // Update the status flag
        __nfc_on__ = 0x0;
        // I2C token is automatically released
        __nfc_i2c_token__ = 0x0;
    }
}

void nfc_i2c_start(void) {
    __nfc_i2c_nak__ = 0;
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
    gpio_write_data((1<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_set_dir((1<<__NFC_SCL__)|(1<<__NFC_SDA__));
    gpio_write_data((0<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_write_data((0<<__NFC_SDA__)|(0<<__NFC_SCL__));
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
    gpio_write_data((0<<__NFC_SDA__)|(0<<__NFC_SCL__));
    gpio_set_dir((1<<__NFC_SCL__)|(1<<__NFC_SDA__));
    gpio_write_data((0<<__NFC_SDA__)|(1<<__NFC_SCL__));
    gpio_write_data((1<<__NFC_SDA__)|(1<<__NFC_SCL__));
#endif
}

uint32_t nfc_i2c_byte(uint8_t byte){
    if (!__nfc_i2c_nak__) {
        uint32_t nak;

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
    
        //Check ACK
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__); // SDA=z, SCL=1
        nak = (*GPIO_DATA>>__NFC_SDA__)&0x1;
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=z, SCL=0
    
        // update the retentive memory
        *R_GPIO_DIR  = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__);
        *R_GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__);
    
    #else
        uint32_t i;
    
        // Direction: SCL (output), SDA (output)
        gpio_set_dir((1<<__NFC_SCL__)|(1<<__NFC_SDA__));
    
        // Send byte[7:1]
        for (i=7; i>0; i--) {
            gpio_write_data((((byte>>i)&0x1)<<__NFC_SDA__)|(0<<__NFC_SCL__));
            gpio_write_data((((byte>>i)&0x1)<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data((((byte>>i)&0x1)<<__NFC_SDA__)|(0<<__NFC_SCL__));
        }
        // Send byte[0]
        if (byte&0x1) {
            // Direction: SCL (output), SDA (input)
            gpio_set_dir((1<<__NFC_SCL__)|(0<<__NFC_SDA__));
            gpio_write_data(0<<__NFC_SCL__);
            gpio_write_data(1<<__NFC_SCL__);
            gpio_write_data(0<<__NFC_SCL__);
        }
        else {
            gpio_write_data((0<<__NFC_SDA__)|(0<<__NFC_SCL__));
            gpio_write_data((0<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data((0<<__NFC_SDA__)|(0<<__NFC_SCL__));
            // Direction: SCL (output), SDA (input)
            gpio_set_dir((1<<__NFC_SCL__)|(0<<__NFC_SDA__));
        }
    
        //Check for ACK
        gpio_write_data(1<<__NFC_SCL__);
        nak = (*GPIO_DATA>>__NFC_SDA__)&0x1;
        gpio_write_data(0<<__NFC_SCL__);
    
    #endif
        __nfc_i2c_nak__ = nak;
        #ifdef DEVEL
            if (nak) mbus_write_message32(0xEF, 0x12C0DEAD);
        #endif
        return !nak;

    }
    else {
        return 0;
    }
}

uint32_t nfc_i2c_byte_imm (uint8_t byte){
    uint32_t nak;

    if (!__nfc_i2c_nak__) {

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
        nak = (*GPIO_DATA>>__NFC_SDA__)&0x1;
        *GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__); // SDA=z, SCL=0
    
        // update the retentive memory
        *R_GPIO_DIR  = (0x0<<__NFC_SDA__)|(0x1<<__NFC_SCL__);
        *R_GPIO_DATA = (0x0<<__NFC_SDA__)|(0x0<<__NFC_SCL__);
    
    #else
        uint32_t i;
    
        // Direction: SCL (output), SDA (output)
        gpio_set_dir((1<<__NFC_SCL__)|(1<<__NFC_SDA__));
    
        // Send byte[7:1]
        for (i=7; i>0; i--) {
            gpio_write_data((((byte>>i)&0x1)<<__NFC_SDA__)|(0<<__NFC_SCL__));
            gpio_write_data((((byte>>i)&0x1)<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data((((byte>>i)&0x1)<<__NFC_SDA__)|(0<<__NFC_SCL__));
        }
        // Send byte[0]
        if (byte&0x1) {
            // Direction: SCL (output), SDA (input)
            gpio_set_dir((1<<__NFC_SCL__)|(0<<__NFC_SDA__));
            gpio_write_data(0<<__NFC_SCL__);
            gpio_write_data(1<<__NFC_SCL__);
            gpio_write_data(0<<__NFC_SCL__);
        }
        else {
            gpio_write_data((0<<__NFC_SDA__)|(0<<__NFC_SCL__));
            gpio_write_data((0<<__NFC_SDA__)|(1<<__NFC_SCL__));
            gpio_write_data((0<<__NFC_SDA__)|(0<<__NFC_SCL__));
            // Direction: SCL (output), SDA (input)
            gpio_set_dir((1<<__NFC_SCL__)|(0<<__NFC_SDA__));
        }
        
        // Check ACK
        gpio_write_data(1<<__NFC_SCL__);
        nak = (*GPIO_DATA>>__NFC_SDA__)&0x1;
        gpio_write_data(0<<__NFC_SCL__);
    #endif
        return !nak;
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
    nfc_i2c_byte(__i2c_password__.upper >> 24);
    nfc_i2c_byte(__i2c_password__.upper >> 16);
    nfc_i2c_byte(__i2c_password__.upper >>  8);
    nfc_i2c_byte(__i2c_password__.upper >>  0);
    nfc_i2c_byte(__i2c_password__.upper >> 24);
    nfc_i2c_byte(__i2c_password__.upper >> 16);
    nfc_i2c_byte(__i2c_password__.upper >>  8);
    nfc_i2c_byte(__i2c_password__.upper >>  0);
    nfc_i2c_byte(0x09);
    nfc_i2c_byte(__i2c_password__.lower >> 24);
    nfc_i2c_byte(__i2c_password__.lower >> 16);
    nfc_i2c_byte(__i2c_password__.lower >>  8);
    nfc_i2c_byte(__i2c_password__.lower >>  0);
    nfc_i2c_byte(__i2c_password__.lower >> 24);
    nfc_i2c_byte(__i2c_password__.lower >> 16);
    nfc_i2c_byte(__i2c_password__.lower >>  8);
    nfc_i2c_byte(__i2c_password__.lower >>  0);
    nfc_i2c_stop();
    return !__nfc_i2c_nak__;
}

uint32_t nfc_i2c_update_password(uint64_t password) {
    uint32_t i2c_dsc   = 0xAE;      // Device Select Code
    uint32_t i2c_addr  = 0x0900;    // Address
    __i2c_password__.value = password;
    nfc_i2c_start();
    nfc_i2c_byte(i2c_dsc);
    nfc_i2c_byte(i2c_addr>>8);
    nfc_i2c_byte(i2c_addr);
    nfc_i2c_byte(__i2c_password__.upper >> 24);
    nfc_i2c_byte(__i2c_password__.upper >> 16);
    nfc_i2c_byte(__i2c_password__.upper >>  8);
    nfc_i2c_byte(__i2c_password__.upper >>  0);
    nfc_i2c_byte(__i2c_password__.upper >> 24);
    nfc_i2c_byte(__i2c_password__.upper >> 16);
    nfc_i2c_byte(__i2c_password__.upper >>  8);
    nfc_i2c_byte(__i2c_password__.upper >>  0);
    nfc_i2c_byte(0x07);
    nfc_i2c_byte(__i2c_password__.lower >> 24);
    nfc_i2c_byte(__i2c_password__.lower >> 16);
    nfc_i2c_byte(__i2c_password__.lower >>  8);
    nfc_i2c_byte(__i2c_password__.lower >>  0);
    nfc_i2c_byte(__i2c_password__.lower >> 24);
    nfc_i2c_byte(__i2c_password__.lower >> 16);
    nfc_i2c_byte(__i2c_password__.lower >>  8);
    nfc_i2c_byte(__i2c_password__.lower >>  0);
    nfc_i2c_stop();
    return !__nfc_i2c_nak__;
}

uint8_t nfc_i2c_rd(uint8_t ack){
    if (!__nfc_i2c_nak__) {
        uint8_t data = 0;

        // Direction: SCL (output), SDA (input)
        gpio_set_dir((1<<__NFC_SCL__)|(0<<__NFC_SDA__));

        // Read byte[7:0]
        uint32_t i = 7;
        while (1) {
            gpio_write_data(0<<__NFC_SCL__);
            gpio_write_data(1<<__NFC_SCL__);
            data = data | (((*GPIO_DATA>>__NFC_SDA__)&0x1)<<i);
            if (i==0) break;
            else i--;
        }
        gpio_write_data(0<<__NFC_SCL__);

        // Acknowledge
        if (ack) {
            gpio_write_data((0<<__NFC_SDA__) | (0<<__NFC_SCL__));
            gpio_set_dir((1<<__NFC_SCL__)|(1<<__NFC_SDA__));
            gpio_write_data((0<<__NFC_SDA__) | (1<<__NFC_SCL__));
            gpio_write_data((0<<__NFC_SDA__) | (0<<__NFC_SCL__));
            gpio_set_dir((1<<__NFC_SCL__)|(0<<__NFC_SDA__));
        }
        else{
            gpio_set_dir((1<<__NFC_SCL__)|(0<<__NFC_SDA__));
            gpio_write_data(1<<__NFC_SCL__);
            gpio_write_data(0<<__NFC_SCL__);
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
                // If Nak occurs
                if (__nfc_i2c_nak__) {
                    #ifdef DEVEL
                        mbus_write_message32(0xE0, 0x01000000);
                    #endif
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
            #ifdef DEVEL
                mbus_write_message32(0xE0, 0x02000001);
            #endif
            return 1;
        }
        // Not Acked
        else {
            #ifdef DEVEL
                mbus_write_message32(0xE0, 0x02000000);
            #endif
            return 0;
        }
    }
    // There was no token
    else {
        #ifdef DEVEL
            mbus_write_message32(0xE0, 0x02000003);
        #endif
        return 1;
    }
}

//uint32_t nfc_i2c_byte_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t nb){
//    #ifdef DEVEL
//        mbus_write_message32(0xD0, (e2<<28)|(nb<<24)|addr);
//        mbus_write_message32(0xD1, data);
//    #endif
//    // In order to reduce the code space,
//    // this function does not generate an error when 'nb' is out of the valid range.
//    nfc_i2c_start();
//    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
//    nfc_i2c_byte(addr>>8);
//    nfc_i2c_byte(addr);
//    //-------------------------------------------------
//              nfc_i2c_byte((data>> 0)&0xFF);
//    if (nb>1) nfc_i2c_byte((data>> 8)&0xFF);
//    if (nb>2) nfc_i2c_byte((data>>16)&0xFF);
//    if (nb>3) nfc_i2c_byte((data>>24)&0xFF);
//    //-------------------------------------------------
//    nfc_i2c_stop();
//    // If NAK'd
//    if (__nfc_i2c_nak__) return 0;
//    // If everything went well
//    else {
//        nfc_i2c_polling();
//        return 1;
//    }
//}

uint32_t nfc_i2c_byte_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t nb){
    // In order to reduce the code space,
    // this function does not generate an error when 'nb' is out of the valid range.
    
    uint32_t iter = 0;
    while (iter<__NFC_NUM_RETRY__) {
        #ifdef DEVEL
            if (iter==0) {
                mbus_write_message32(0xD0, (e2<<28)|(nb<<24)|addr);
                mbus_write_message32(0xD1, data);
            }
            else {
                mbus_write_message32(0xDF, iter);
            }
        #endif
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
        // If NAK'd
        if (__nfc_i2c_nak__) {
            iter++;
            // Some delay before the next attempt.
            delay(__NFC_RETRY_DELAY__);
        }
        // If everything went well
        else {
            nfc_i2c_polling();
            return 1;
        }
    }

    // Still NAK'd after __NUM_NUM_RETRY__ tries.
    // -> Trigger I2C ACK Failure Handling
    set_reg_pend_irq(I2C_TARGET_REG_IDX);
    return 0;
}

//uint32_t nfc_i2c_byte_read(uint32_t e2, uint32_t addr, uint32_t nb){
//    #ifdef DEVEL
//        mbus_write_message32(0xD2, (e2<<28)|(nb<<24)|addr);
//    #endif
//    // In order to reduce the code space,
//    // this function does not generate an error when 'nb' is out of the valid range.
//    uint32_t data;
//    nfc_i2c_start();
//    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
//    nfc_i2c_byte(addr >> 8);
//    nfc_i2c_byte(addr);
//    nfc_i2c_start();
//    nfc_i2c_byte(0xA7 | ((e2&0x1) << 3));
//    //-------------------------------------------------
//               data  =  nfc_i2c_rd(!(nb==1));
//    if (nb>1)  data |= (nfc_i2c_rd(!(nb==2)) << 8);
//    if (nb>2)  data |= (nfc_i2c_rd(!(nb==3)) << 16);
//    if (nb>3)  data |= (nfc_i2c_rd(!(nb==4)) << 24);
//    //-------------------------------------------------
//    nfc_i2c_stop();
//    // If NAK'd
//    if (__nfc_i2c_nak__) return 0;
//    // If everything went well
//    else {
//        #ifdef DEVEL
//            mbus_write_message32(0xD3, data);
//        #endif
//        return data;
//    }
//}

uint32_t nfc_i2c_byte_read(uint32_t e2, uint32_t addr, uint32_t nb){
    // In order to reduce the code space,
    // this function does not generate an error when 'nb' is out of the valid range.
    
    uint32_t iter = 0;
    while (iter<__NFC_NUM_RETRY__) {
        #ifdef DEVEL
            if (iter==0) {
                mbus_write_message32(0xD2, (e2<<28)|(nb<<24)|addr);
            }
            else {
                mbus_write_message32(0xDF, iter);
            }
        #endif
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
        // If NAK'd
        if (__nfc_i2c_nak__) {
            iter++;
            // Some delay before the next attempt.
            delay(__NFC_RETRY_DELAY__);
        }
        // If everything went well
        else {
            #ifdef DEVEL
                mbus_write_message32(0xD3, data);
            #endif
            return data;
        }
    }

    // Still NAK'd after __NUM_NUM_RETRY__ tries.
    // -> Trigger I2C ACK Failure Handling
    set_reg_pend_irq(I2C_TARGET_REG_IDX);
    return 0;
}

//uint32_t nfc_i2c_seq_byte_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t nb){
//    // In order to reduce the code space,
//    // this function does not generate an error when 'nb' is out of the valid range.
//    uint32_t i;
//    uint32_t s;
//    nfc_i2c_start();
//    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
//    nfc_i2c_byte(addr>>8);
//    nfc_i2c_byte(addr);
//    s=0;
//    for (i=0; i<nb; i++) {
//        if(s==4) s=0;
//        if      (s==0) nfc_i2c_byte((data[i]>> 0)&0xFF);
//        else if (s==1) nfc_i2c_byte((data[i]>> 8)&0xFF);
//        else if (s==2) nfc_i2c_byte((data[i]>>16)&0xFF);
//        else if (s==3) nfc_i2c_byte((data[i]>>24)&0xFF);
//        s++;
//    }
//    nfc_i2c_stop();
//    // If NAK'd
//    if (__nfc_i2c_nak__) return 0;
//    // If everything went well
//    else {
//        nfc_i2c_polling();
//        return 1;
//    }
//}

uint32_t nfc_i2c_seq_byte_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t nb){
    // In order to reduce the code space,
    // this function does not generate an error when 'nb' is out of the valid range.
    uint32_t iter = 0;
    while (iter<__NFC_NUM_RETRY__) {
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
        // If NAK'd
        if (__nfc_i2c_nak__) {
            iter++;
            // Some delay before the next attempt.
            delay(__NFC_RETRY_DELAY__);
        }
        // If everything went well
        else {
            nfc_i2c_polling();
            return 1;
        }
    }

    // Still NAK'd after __NUM_NUM_RETRY__ tries.
    // -> Trigger I2C ACK Failure Handling
    set_reg_pend_irq(I2C_TARGET_REG_IDX);
    return 0;
}

//uint32_t nfc_i2c_word_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t nw){
//    // In order to reduce the code space,
//    // this function does not generate an error when 'nw' is out of the valid range.
//    uint32_t i, j;
//    uint32_t word;
//    addr &= 0xFFFC; // Force the 2 LSBs to 0 to make it word-aligned.
//    nfc_i2c_start();
//    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
//    nfc_i2c_byte(addr>>8);
//    nfc_i2c_byte(addr);
//    for (i=0; i<nw; i++) {
//        word = data[i];
//        for (j=0; j<4; j++) {
//            nfc_i2c_byte(word&0xFF);
//            word = word>>8;
//        }
//    }
//    nfc_i2c_stop();
//    // If NAK'd
//    if (__nfc_i2c_nak__) return 0;
//    // If everything went well
//    else {
//        nfc_i2c_polling();
//        return 1;
//    }
//}

uint32_t nfc_i2c_word_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t nw){
    // In order to reduce the code space,
    // this function does not generate an error when 'nw' is out of the valid range.

    uint32_t iter = 0;
    while (iter<__NFC_NUM_RETRY__) {
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
        // If NAK'd
        if (__nfc_i2c_nak__) {
            iter++;
            // Some delay before the next attempt.
            delay(__NFC_RETRY_DELAY__);
        }
        // If everything went well
        else {
            nfc_i2c_polling();
            return 1;
        }
    }

    // Still NAK'd after __NUM_NUM_RETRY__ tries.
    // -> Trigger I2C ACK Failure Handling
    set_reg_pend_irq(I2C_TARGET_REG_IDX);
    return 0;
}

//uint32_t nfc_i2c_word_read(uint32_t e2, uint32_t addr, uint32_t nw, volatile uint32_t* ptr){
//    // In order to reduce the code space,
//    // this function does not generate an error when 'nw' is out of the valid range.
//
//    #ifdef DEVEL
//        mbus_write_message32(0xD4, (nw<<24)|addr);
//        mbus_write_message32(0xD5, (uint32_t) ptr);
//    #endif
//    // This assumes a memory boundary between byte 31 and byte 32.
//
//    uint32_t data = 0;
//    uint32_t bcnt = 0;
//    uint32_t enda = addr + (nw<<2);
//
//    nfc_i2c_start();
//    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
//    nfc_i2c_byte(addr >> 8);
//    nfc_i2c_byte(addr);
//    nfc_i2c_start();
//    nfc_i2c_byte(0xA7 | ((e2&0x1) << 3));
//
//    while (addr<enda) {
//
//        data |= (nfc_i2c_rd((addr!=31)&&(addr!=(enda-1))) << (bcnt << 3));
//
//        addr++;
//        bcnt++;
//
//        if (bcnt==4) {
//            #ifdef DEVEL
//                mbus_write_message32(0xD7, (uint32_t) ptr);
//                mbus_write_message32(0xD8, (uint32_t) data);
//            #endif
//            *ptr = data;
//            data = 0;
//            bcnt = 0;
//            ptr++;
//        }
//
//        if ((addr==32)&&(addr!=enda)) {
//            nfc_i2c_stop();
//            nfc_i2c_start();
//            nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
//            nfc_i2c_byte(addr >> 8);
//            nfc_i2c_byte(addr);
//            nfc_i2c_start();
//            nfc_i2c_byte(0xA7 | ((e2&0x1) << 3));
//        }
//
//    }
//    nfc_i2c_stop();
//
//    // If NAK'd
//    if (__nfc_i2c_nak__) {
//        #ifdef DEVEL
//            mbus_write_message32(0xD6, 0x0);
//        #endif
//        return 0;
//    }
//    // If everything went well
//    else {
//        #ifdef DEVEL
//            mbus_write_message32(0xD6, 0x1);
//        #endif
//        return 1;
//    }
//}

uint32_t nfc_i2c_word_read(uint32_t e2, uint32_t addr, uint32_t nw, volatile uint32_t* ptr){
    // In order to reduce the code space,
    // this function does not generate an error when 'nw' is out of the valid range.

    uint32_t iter = 0;
    while (iter<__NFC_NUM_RETRY__) {
        #ifdef DEVEL
            if (iter==0) {
                mbus_write_message32(0xD4, (nw<<24)|addr);
                mbus_write_message32(0xD5, (uint32_t) ptr);
            }
            else {
                mbus_write_message32(0xDF, iter);
            }
        #endif
        // This assumes a memory boundary between byte 31 and byte 32.

        uint32_t adri = addr;
        uint32_t* ptri = (uint32_t *) ptr;
        uint32_t data = 0;
        uint32_t bcnt = 0;
        uint32_t enda = adri + (nw<<2);

        nfc_i2c_start();
        nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
        nfc_i2c_byte(adri >> 8);
        nfc_i2c_byte(adri);
        nfc_i2c_start();
        nfc_i2c_byte(0xA7 | ((e2&0x1) << 3));

        while (adri<enda) {

            data |= (nfc_i2c_rd((adri!=31)&&(adri!=(enda-1))) << (bcnt << 3));

            adri++;
            bcnt++;

            if (bcnt==4) {
                #ifdef DEVEL
                    mbus_write_message32(0xD7, (uint32_t) ptri);
                    mbus_write_message32(0xD8, (uint32_t) data);
                #endif
                *ptri = data;
                data = 0;
                bcnt = 0;
                ptri++;
            }

            if ((adri==32)&&(adri!=enda)) {
                nfc_i2c_stop();
                nfc_i2c_start();
                nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
                nfc_i2c_byte(adri >> 8);
                nfc_i2c_byte(adri);
                nfc_i2c_start();
                nfc_i2c_byte(0xA7 | ((e2&0x1) << 3));
            }

        }
        nfc_i2c_stop();

        // If NAK'd
        if (__nfc_i2c_nak__) {
            #ifdef DEVEL
                mbus_write_message32(0xD6, 0x0);
            #endif
            iter++;
            // Some delay before the next attempt.
            delay(__NFC_RETRY_DELAY__);
        }
        // If everything went well
        else {
            #ifdef DEVEL
                mbus_write_message32(0xD6, 0x1);
            #endif
            return 1;
        }
    }

    // Still NAK'd after __NUM_NUM_RETRY__ tries.
    // -> Trigger I2C ACK Failure Handling
    set_reg_pend_irq(I2C_TARGET_REG_IDX);
    return 0;
}

//uint32_t nfc_i2c_word_pattern_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t nw){
//    // In order to reduce the code space,
//    // this function does not generate an error when 'nw' is out of the valid range.
//    uint32_t i;
//    addr &= 0xFFFC; // Force the 2 LSBs to 0 to make it word-aligned.
//    nfc_i2c_start();
//    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
//    nfc_i2c_byte(addr>>8);
//    nfc_i2c_byte(addr);
//    for (i=0; i<nw; i++) {
//        nfc_i2c_byte((data>> 0)&0xFF);
//        nfc_i2c_byte((data>> 8)&0xFF);
//        nfc_i2c_byte((data>>16)&0xFF);
//        nfc_i2c_byte((data>>24)&0xFF);
//    }
//    nfc_i2c_stop();
//    // If NAK'd
//    if (__nfc_i2c_nak__) return 0;
//    // If everything went well
//    else {
//        nfc_i2c_polling();
//        return 1;
//    }
//}

uint32_t nfc_i2c_word_pattern_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t nw){
    // In order to reduce the code space,
    // this function does not generate an error when 'nw' is out of the valid range.
    uint32_t iter = 0;
    while (iter<__NFC_NUM_RETRY__) {
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
        // If NAK'd
        if (__nfc_i2c_nak__) {
            iter++;
            // Some delay before the next attempt.
            delay(__NFC_RETRY_DELAY__);
        }
        // If everything went well
        else {
            nfc_i2c_polling();
            return 1;
        }
    }

    // Still NAK'd after __NUM_NUM_RETRY__ tries.
    // -> Trigger I2C ACK Failure Handling
    set_reg_pend_irq(I2C_TARGET_REG_IDX);
    return 0;
}


//*******************************************************************
// EID FUNCTIONS
//*******************************************************************

void eid_init(void) {

    // NOTE: EID_CTRL and WD_CTRL use a clock whose frequency is 16x slower than the raw timer clock.
    //       The raw clock should be ~2.125kHz, thus the EID_CTRL and WD_CTRL may use ~133Hz clock.

    // EID Clock Tuning
    // ring   : (Default: 0x0; Max: 0x3) Selects # of rings. 0x0: 11 stages; 0x3: 5 stages.
    // te_div : (Default: 0x0; Max: 0x3) Selects clock division ratio for Top Electrode (TE) charge pump. Divide the core clock by 2^te_div.
    // fd_div : (Default: 0x0; Max: 0x3) Selects clock division ratio for Field (FD) charge pump. Divide the core clock by 2^fd_div.
    // seg_div: (Default: 0x0; Max: 0x3) Selects clock division ratio for Segment (SEG) charge pumps. Divide the core clock by 2^seg_div.

    __eid_current_display__     = 0;
    __eid_cp_duration__         = 250;

    #ifndef __USE_HCODE__
        __crsh_seqa__.value = (/*vin*/ 0x0 << 11) | (/*te*/ 0x0 << 10) | (/*fd*/ 0x1 << 9) | (/*seg*/ 0x1FF << 0);
        __crsh_seqb__.value = (/*vin*/ 0x0 << 11) | (/*te*/ 0x1 << 10) | (/*fd*/ 0x0 << 9) | (/*seg*/ 0x000 << 0);
      //__crsh_seqc__.value = (/*vin*/ 0x0 << 11) | (/*te*/ 0x0 << 10) | (/*fd*/ 0x0 << 9) | (/*seg*/ 0x018 << 0);  // CROSS
        __crsh_seqc__.value = (/*vin*/ 0x0 << 11) | (/*te*/ 0x0 << 10) | (/*fd*/ 0x0 << 9) | (/*seg*/ 0x004 << 0);  // LOW-BATT

        eid_r00.as_int = EID_R00_DEFAULT_AS_INT;
        eid_r01.as_int = EID_R01_DEFAULT_AS_INT;
        eid_r02.as_int = EID_R02_DEFAULT_AS_INT;
    #endif

    // Start EID Timer 
    eid_enable_timer();

    #ifndef __USE_HCODE__
        // Charge Pump Clock Generator
	    eid_config_cp_clk_gen(/*ring*/ring, /*te_div*/te_div, /*fd_div*/fd_div, /*seg_div*/seg_div);
    #else
        mbus_remote_register_write(EID_ADDR,0x02,
                                      ( 1 << 8)  // ECP_PG_DIODE
                                    | ( 0 << 6)  // ECP_SEL_RING
                                    | ( 3 << 4)  // ECP_SEL_TE_DIV
                                    | ( 3 << 2)  // ECP_SEL_FD_DIV
                                    | ( 3 << 0)  // ECP_SEL_SEG_DIV
                                    );
    #endif

    // Charge Pump Pulse Width
    eid_set_duration(__eid_cp_duration__);

    // Configuration for Crash Behavior (See comment section in the top)
    //--- Watchdog Threshold; based on the 133Hz clock; Max: 16777215 (35 hours)
    mbus_remote_register_write(EID_ADDR,0x10,
                                  (11491200 << 0) // WCTR_THRESHOLD // 24 hours
                                  );

    #ifndef __USE_HCODE__
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
    #else
        mbus_remote_register_write(EID_ADDR,0x11,
                                      ( 0x1 << 14) // WCTR_CR_ECP_PG_DIODE_A0 
                                    | ( 0x0 << 13) // WCTR_CR_ECP_PG_DIODE_A  
                                    | ( 0x0 << 12) // WCTR_CR_ECP_PG_DIODE_A1 
                                    | ( 0x0 << 11) // WCTR_CR_ECP_PG_DIODE_B  
                                    | ( 0x0 << 10) // WCTR_CR_ECP_PG_DIODE_B1 
                                    | ( 0x0 <<  9) // WCTR_CR_ECP_PG_DIODE_C  
                                    | ( 0x0 <<  8) // WCTR_CR_ECP_PG_DIODE_C1 
                                    | ( 0x0 <<  6) // WCTR_CR_ECP_SEL_RING    
                                    | ( 0x3 <<  4) // WCTR_CR_ECP_SEL_TE_DIV  
                                    | ( 0x3 <<  2) // WCTR_CR_ECP_SEL_FD_DIV  
                                    | ( 0x3 <<  0) // WCTR_CR_ECP_SEL_SEG_DIV 
                                    );
    #endif

    mbus_remote_register_write(EID_ADDR,0x12,
                                  (0x7 << 16) // WCTR_SEL_SEQ    (Default: 0x7) See above waveform. Each bit in [2:0] enables/disables Seq C, Seq B, Seq A, respectively. 
                                              //    If 1, the corresponding sequence is enabled. 
                                              //    If 0, the corresponding sequence is skipped.
                                | (400 <<  0) // WCTR_IDLE_WIDTH (Default: 400; Max: 65535) Duration of Idle time
                                );

    mbus_remote_register_write(EID_ADDR,0x13,
                                  (400 << 0) // WCTR_PULSE_WIDTH (Default: 400; Max: 65535) Duration of Charge Pump Activation.
                                );

    #ifndef __USE_HCODE__
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
    #else
        mbus_remote_register_write(EID_ADDR,0x15,
                                      (  1              << 23)  // WCTR_RESETB_CP_A
                                    | (  0              << 22)  // WCTR_VIN_CP_A
                                    | ((~0x3FF & 0x7FF) << 11)  // WCTR_EN_CP_PD_A
                                    | ( 0x3FF           <<  0)  // WCTR_EN_CP_CK_A
                                    );

        mbus_remote_register_write(EID_ADDR,0x16,
                                      ( 1               << 23)  // WCTR_RESETB_CP_B
                                    | ( 0               << 22)  // WCTR_VIN_CP_B
                                    | ((~0x400 & 0x7FF) << 11)  // WCTR_EN_CP_PD_B
                                    | ( 0x400           <<  0)  // WCTR_EN_CP_CK_B
                                    );

        mbus_remote_register_write(EID_ADDR,0x17,
                                      ( 1               << 23)  // WCTR_RESETB_CP_C
                                    | ( 0               << 22)  // WCTR_VIN_CP_C
                                    | ((~0x004 & 0x7FF) << 11)  // WCTR_EN_CP_PD_C
                                    | ( 0x004           <<  0)  // WCTR_EN_CP_CK_C
                                    );
    #endif

}

void eid_enable_timer(void){
    #ifndef __USE_HCODE__
        eid_r01.TMR_SELF_EN = 0;
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
        eid_r01.TMR_EN_OSC = 1;
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
        eid_r01.TMR_RESETB = 1;
        eid_r01.TMR_RESETB_DIV = 1;
        eid_r01.TMR_RESETB_DCDC = 1;
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
        delay(50000); // Wait for >2s
        eid_r01.TMR_EN_SELF_CLK = 1;
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
        eid_r01.TMR_SELF_EN = 1;
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
        eid_r01.TMR_EN_OSC = 0;
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
        eid_r01.TMR_SEL_LDO = 1; // 1: use VBAT; 0: use V1P2
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
        eid_r01.TMR_ISOL_CLK = 0;
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
        eid_r00.TMR_EN_CLK_DIV = 1;
        mbus_remote_register_write(EID_ADDR,0x00,eid_r00.as_int);
    #else
        mbus_remote_register_write(EID_ADDR,0x01,(/*TMR_RESETB_DCDC*/0<<0)|(/*TMR_EN_SELF_CLK*/0<<1)|(/*TMR_SELF_EN*/0<<2)|(/*TMR_RESETB_DIV*/0<<3)|(/*TMR_RESETB*/0<<4)|(/*TMR_SEL_LDO*/0<<5)|(/*TMR_EN_OSC*/0<<6)|(/*TMR_ISOL_CLK*/1<<7));
        mbus_remote_register_write(EID_ADDR,0x01,(/*TMR_RESETB_DCDC*/0<<0)|(/*TMR_EN_SELF_CLK*/0<<1)|(/*TMR_SELF_EN*/0<<2)|(/*TMR_RESETB_DIV*/0<<3)|(/*TMR_RESETB*/0<<4)|(/*TMR_SEL_LDO*/0<<5)|(/*TMR_EN_OSC*/1<<6)|(/*TMR_ISOL_CLK*/1<<7));
        mbus_remote_register_write(EID_ADDR,0x01,(/*TMR_RESETB_DCDC*/1<<0)|(/*TMR_EN_SELF_CLK*/0<<1)|(/*TMR_SELF_EN*/0<<2)|(/*TMR_RESETB_DIV*/1<<3)|(/*TMR_RESETB*/1<<4)|(/*TMR_SEL_LDO*/0<<5)|(/*TMR_EN_OSC*/1<<6)|(/*TMR_ISOL_CLK*/1<<7));
        delay(50000); // Wait for >2s
        mbus_remote_register_write(EID_ADDR,0x01,(/*TMR_RESETB_DCDC*/1<<0)|(/*TMR_EN_SELF_CLK*/1<<1)|(/*TMR_SELF_EN*/0<<2)|(/*TMR_RESETB_DIV*/1<<3)|(/*TMR_RESETB*/1<<4)|(/*TMR_SEL_LDO*/0<<5)|(/*TMR_EN_OSC*/1<<6)|(/*TMR_ISOL_CLK*/1<<7));
        mbus_remote_register_write(EID_ADDR,0x01,(/*TMR_RESETB_DCDC*/1<<0)|(/*TMR_EN_SELF_CLK*/1<<1)|(/*TMR_SELF_EN*/1<<2)|(/*TMR_RESETB_DIV*/1<<3)|(/*TMR_RESETB*/1<<4)|(/*TMR_SEL_LDO*/0<<5)|(/*TMR_EN_OSC*/1<<6)|(/*TMR_ISOL_CLK*/1<<7));
        mbus_remote_register_write(EID_ADDR,0x01,(/*TMR_RESETB_DCDC*/1<<0)|(/*TMR_EN_SELF_CLK*/1<<1)|(/*TMR_SELF_EN*/1<<2)|(/*TMR_RESETB_DIV*/1<<3)|(/*TMR_RESETB*/1<<4)|(/*TMR_SEL_LDO*/0<<5)|(/*TMR_EN_OSC*/0<<6)|(/*TMR_ISOL_CLK*/1<<7));
        mbus_remote_register_write(EID_ADDR,0x01,(/*TMR_RESETB_DCDC*/1<<0)|(/*TMR_EN_SELF_CLK*/1<<1)|(/*TMR_SELF_EN*/1<<2)|(/*TMR_RESETB_DIV*/1<<3)|(/*TMR_RESETB*/1<<4)|(/*TMR_SEL_LDO*/1<<5)|(/*TMR_EN_OSC*/0<<6)|(/*TMR_ISOL_CLK*/1<<7));
        mbus_remote_register_write(EID_ADDR,0x01,(/*TMR_RESETB_DCDC*/1<<0)|(/*TMR_EN_SELF_CLK*/1<<1)|(/*TMR_SELF_EN*/1<<2)|(/*TMR_RESETB_DIV*/1<<3)|(/*TMR_RESETB*/1<<4)|(/*TMR_SEL_LDO*/1<<5)|(/*TMR_EN_OSC*/0<<6)|(/*TMR_ISOL_CLK*/0<<7));
        mbus_remote_register_write(EID_ADDR,0x00,(/*EN_WATCHDOG*/0<<0)|(/*TMR_EN_CLK_DIV*/1<<1)|(/*TMR_EN_CLK_OUT*/0<<2));
    #endif
}

#ifndef __USE_HCODE__
void eid_config_cp_clk_gen(uint32_t ring, uint32_t te_div, uint32_t fd_div, uint32_t seg_div) {
    eid_r02.ECP_PG_DIODE    = 1;
    eid_r02.ECP_SEL_RING    = ring;
    eid_r02.ECP_SEL_TE_DIV  = te_div;
    eid_r02.ECP_SEL_FD_DIV  = fd_div;
    eid_r02.ECP_SEL_SEG_DIV = seg_div;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);
}
#endif

void eid_set_duration(uint32_t duration){
    __eid_cp_duration__ = duration;
    mbus_remote_register_write(EID_ADDR,0x07,__eid_cp_duration__);
}

void eid_enable_cp_ck(uint32_t te, uint32_t fd, uint32_t seg) {
    uint32_t cp_ck = ((te << 10) | (fd << 9) | (seg << 0)) & 0x7FF;
    uint32_t cp_pd = (~cp_ck) & 0x7FF;

    // Make PG_DIODE=0
    #ifndef __USE_HCODE__
        eid_r02.ECP_PG_DIODE = 0;
        mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);
    #else
        mbus_remote_register_write(EID_ADDR,0x02,
                                      ( 0 << 8)  // ECP_PG_DIODE
                                    | ( 0 << 6)  // ECP_SEL_RING
                                    | ( 3 << 4)  // ECP_SEL_TE_DIV
                                    | ( 3 << 2)  // ECP_SEL_FD_DIV
                                    | ( 3 << 0)  // ECP_SEL_SEG_DIV
                                    );
    #endif

    // Enable charge pumps
    enable_reg_irq(EID_TARGET_REG_IDX);
    start_timeout32_check(/*id*/FAIL_ID_EID, /*val*/TIMEOUT_TH<<4); // ~16s timeout
    mbus_remote_register_write(EID_ADDR,0x09,
                                  (0x1   << 23) // ECTR_RESETB_CP
                                | (0x0   << 22) // ECTR_VIN_CP
                                | (cp_pd << 11) // ECTR_EN_CP_PD
                                | (cp_ck <<  0) // ECTR_EN_CP_CK
                                );
    WFI();

    // Make PG_DIODE=1
    #ifndef __USE_HCODE__
        eid_r02.ECP_PG_DIODE = 1;
        mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);
    #else
        mbus_remote_register_write(EID_ADDR,0x02,
                                      ( 1 << 8)  // ECP_PG_DIODE
                                    | ( 0 << 6)  // ECP_SEL_RING
                                    | ( 3 << 4)  // ECP_SEL_TE_DIV
                                    | ( 3 << 2)  // ECP_SEL_FD_DIV
                                    | ( 3 << 0)  // ECP_SEL_SEG_DIV
                                    );
    #endif

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
#ifndef __USE_HCODE__
    eid_r00.EN_WATCHDOG = 1;
    mbus_remote_register_write(EID_ADDR,0x00,eid_r00.as_int);
#else
    mbus_remote_register_write(EID_ADDR,0x00,(/*EN_WATCHDOG*/1<<0)|(/*TMR_EN_CLK_DIV*/1<<1)|(/*TMR_EN_CLK_OUT*/0<<2));
#endif
}

void eid_check_in() {
    mbus_remote_register_write(EID_ADDR,0x18,0x000000);
}

void eid_trigger_crash() {
    mbus_remote_register_write(EID_ADDR,0x19,0x00DEAD);
    delay(50000);
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
        | (0x7 <<  0)   // 0x6      // TSNS_SEL_CONV_TIME (NOTE: This must be consistent with 'c' value in tconv())
        ));

    // Wakeup Timer configuration
    snt_r17.WUP_INT_RPLY_REG_ADDR = PLD_TARGET_REG_IDX;
    snt_r17.WUP_INT_RPLY_SHORT_ADDR = 0x10;
    snt_r17.WUP_CLK_SEL = 0x0; // 0: Use CLK_TMR, 1: Use CLK_TSNS
    snt_r17.WUP_AUTO_RESET = 0x0; // Automatically reset counter to 0 upon sleep 
    mbus_remote_register_write(SNT_ADDR,0x17,snt_r17.as_int);

    // Turn on LDO VREF
    snt_ldo_vref_on();
}

void snt_temp_sensor_power_on(uint32_t sel_ldo){

    if (sel_ldo) {
        // Turn on LDO
        snt_r00.LDO_EN_IREF = 1;
        snt_r00.LDO_EN_LDO  = 1;
        mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);

        // Delay (~10ms) @ 100kHz clock speed)
        delay(1000); 
    }

    // Turn on digital block
    if (sel_ldo) snt_r01.TSNS_SEL_LDO  = 1;
    else         snt_r01.TSNS_SEL_V1P2 = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    // Turn on analog block
    if (sel_ldo) snt_r01.TSNS_EN_SENSOR_LDO  = 1;
    else         snt_r01.TSNS_EN_SENSOR_V1P2 = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    // Delay (~2ms @ 100kHz clock speed)
    delay(200);
}


void snt_temp_sensor_power_off(void){

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
    // Newly implemented following the input vector at:
    //  /afs/eecs.umich.edu/vlsida/projects/m3_hdk/layer/SNT/SNTv6/spice/4_wakeup_timer/input.vec
    
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

//    // Delay (must be >400ms at RT)
//    delay(wait_time);

    // [3] TMR_EN_OSC: 1'h1 -> 1'h0
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

uint32_t snt_set_timer_threshold(uint32_t threshold){
    mbus_remote_register_write(SNT_ADDR,0x19,(0x10<<16)|(WP1_TARGET_REG_IDX<<8)|((threshold>>24)&0xFF));
    enable_reg_irq(WP1_TARGET_REG_IDX);
    start_timeout32_check(/*id*/FAIL_ID_WUP, /*val*/TIMEOUT_TH);
    mbus_remote_register_write(SNT_ADDR,0x1A,threshold & 0xFFFFFF);
    WFI();
    // NOTE: In normal operation, 'Threshold Update' returns the lower 24-bit of the data and it gets written to WP1_TARGET_REG_IDX.
    return (*WP1_TARGET_REG_ADDR==(threshold&0xFFFFFF));
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

uint32_t snt_set_wup_timer(uint32_t auto_reset, uint32_t threshold){
    snt_enable_wup_timer(auto_reset);
    return snt_set_timer_threshold(threshold);
}

uint32_t snt_sync_read_wup_timer(void) {
    enable_reg_irq(WP1_TARGET_REG_IDX);
    start_timeout32_check(/*id*/FAIL_ID_WUP, /*val*/TIMEOUT_TH);
    mbus_remote_register_write(SNT_ADDR,0x14,
                             (0x0  << 16)   // 0: Synchronous 32-bit; 1: Synchronous lower 24-bit, 2: Synchronous Upper 8-bit, 3: Invalid
                            |(0x10 <<  8)   // MBus Target Address
                            |(WP0_TARGET_REG_IDX << 0)   // Destination Register ID
                            );
    WFI();
    uint32_t result = ((*WP0_TARGET_REG_ADDR << 24) | *WP1_TARGET_REG_ADDR);
    #ifdef DEVEL
        mbus_write_message32(0xEE, result);
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

    if (num_try >= SNT_WUP_READ_MAX_TRY) {
        __wfi_id__ = FAIL_ID_MET;
        *NVIC_ISPR = (1 << IRQ_TIMER32);    // Trigger the timeout IRQ
        return 0;
    }
    else {
        return temp;
    }
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
// which is handled in tconv_log2_table()
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


uint32_t tconv_log2_table (uint32_t idx) {
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

#define LOG2_ACC    28  // Internal accuracy
#define LOG2_ACCH   14  // LOG2_ACC / 2
#define LOG2_ORDER  8   // Max order for Taylor approximation
uint32_t tconv_log2_taylor (uint32_t idx) {

    // x = 1 + (idx+1)/1024
    // log2(x) = log2(a) + (1/ln2)*Sigma(n=1, inf)[((-1)^(n+1))*(1/n)*(((x-a)/a)^n]
    // a = 1.5
    
    if (idx<1023) {

        uint32_t x = (1<<LOG2_ACC) + div(/*numer*/ (idx+1)<<16, /*denom*/ 1024<<16, /*n*/LOG2_ACC);
        uint32_t c0 = 157024676;            // log2(1.5) in n=28 accuracy; NOTE: log2(1.5)=0.584962500721156181453
        uint32_t c1 = 186065279;            // ln2 in n=28 accuracy; NOTE: ln2=0.69314718
        uint32_t a = 0x3 << (LOG2_ACC-1);   // a = 1.5 in n=28 accuracy

        uint32_t c2;
        if (x > a) c2 = div (/*numer*/ x - a, /*denom*/ a, /*n*/LOG2_ACC);
        else       c2 = div (/*numer*/ a - x, /*denom*/ a, /*n*/LOG2_ACC);

        uint32_t c2n = 1 << LOG2_ACC;
        uint32_t temp = 0; 
        uint32_t sign = 1; // 1:+, 0:-

        uint32_t n, result;
        for (n=1; n<(LOG2_ORDER+1); n++) {
            c2n = mult(/*num_a*/c2n>>LOG2_ACCH, /*num_b*/c2>>LOG2_ACCH);
            if ((x>a) && (sign==0)) {
                temp -= div(/*numer*/c2n, /*denom*/n<<LOG2_ACC, /*n*/LOG2_ACC);
                sign = 1;
            }
            else {
                temp += div(/*numer*/c2n, /*denom*/n<<LOG2_ACC, /*n*/LOG2_ACC);
                sign = 0;
            }
        }

        if (x>a) result = c0 + div(/*numer*/temp, /*denom*/c1, /*n*/LOG2_ACC);
        else     result = c0 - div(/*numer*/temp, /*denom*/c1, /*n*/LOG2_ACC);
            
        // Make it n=11 accurate
        result >>= (LOG2_ACC-12);
        if (result&0x1) return (result>>1)+1;
        else            return (result>>1);
    }
    else return 0; // For idx=1023 or higher
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
    uint32_t log2_dout = (m<<11) + tconv_log2_taylor(idx);

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


