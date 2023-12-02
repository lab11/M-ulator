//*******************************************************************************************
// TMC HEADER FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//-------------------------------------------------------------------------------------------
//      PREv23E -> EIDv1A -> SATv1 -> PMUv13r2
//-------------------------------------------------------------------------------------------
// Short Address
//-------------------------------------------------------------------------------------------
//      PRE: 0x1
//      EID: 0x2
//      SAT: 0x3
//      PMU: 0x4
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//  Seokhyeon Jeong (seokhyeon@cubeworks.io)
//******************************************************************************************* 

//#define DEVEL

#ifndef TMCV2A_H
#define TMCV2A_H
#define TMCV2A

#include "mbus.h"
#include "ST25DV64K.h"
#include "PREv23E.h"
#include "SATv1_RF.h"
#include "EIDv1A_RF.h"
#include "PMUv13r2_RF.h"

//-------------------------------------------------------------------
// Options
//-------------------------------------------------------------------
#define __USE_HCODE__  // Use hard-coded values (sacrificing readability, reducing code size)

//-------------------------------------------------------------------
// Short Addresses
//-------------------------------------------------------------------
#define PRE_ADDR    0x1
#define EID_ADDR    0x2
#define SAT_ADDR    0x3
#define PMU_ADDR    0x4

//*******************************************************************************************
// TARGET REGISTER INDEX FOR LAYER COMMUNICATIONS
//-------------------------------------------------------------------------------------------
// A register write message from a certain layer will be written 
// in the following register in PRE.
// NOTE: Reg0x0-0x3 are retentive; Reg0x4-0x7 are non-retentive.
//*******************************************************************************************
#define PMU_TARGET_REG_IDX  0x0
#define SNT_TARGET_REG_IDX  0x1 // SNT Temperature Sensor
#define EID_TARGET_REG_IDX  0x2
#define ACC_TARGET_REG_IDX  0x3 // SAT MD CNT
#define WP0_TARGET_REG_IDX  0x4 // SNT Timer (Timer Read, Threshold Update)
#define WP1_TARGET_REG_IDX  0x5 //  NOTE: Only WP1_TARGET_REG_IDX is used to trigger an M0 IRQ. WP0_TARGET_REG_IDX is used to handle 32-bit data.
#define PLD_TARGET_REG_IDX  0x6 // Payload Register (Wakeup by SNT Timer, etc); It is not used to trigger an IRQ.
#define I2C_TARGET_REG_IDX  0x7 // Used to handle I2C NAK failure

#define PMU_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (PMU_TARGET_REG_IDX << 2)))
#define SNT_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (SNT_TARGET_REG_IDX << 2)))
#define EID_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (EID_TARGET_REG_IDX << 2)))
#define ACC_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (ACC_TARGET_REG_IDX << 2)))
#define WP0_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (WP0_TARGET_REG_IDX << 2)))
#define WP1_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (WP1_TARGET_REG_IDX << 2)))
#define PLD_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (PLD_TARGET_REG_IDX << 2)))
#define I2C_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (I2C_TARGET_REG_IDX << 2)))

//-------------------------------------------------------------------
// TIMEOUT CHECK
//-------------------------------------------------------------------
#define FAIL_ID_EID     1           // Display Update
#define FAIL_ID_WUP     2           // SNT Timer Access (Timeout)
#define FAIL_ID_PMU     3           // PMU Register Access
#define FAIL_ID_SNT     4           // SNT Temperature Measurement
#define FAIL_ID_I2C     5           // I2C NAK Failure
#define FAIL_ID_MET     6           // SNT Timer Reading (Meta-stability)
#define FAIL_ID_GEN     7           // Generic/Unknown
#define FAIL_ID_INIT    0x1010DEAD  // Timeout/Fail during Initialization

#define TIMEOUT_1S      140000      // TIMER32 Threshold to measure 1 second
#define TIMEOUT_TH      TIMEOUT_1S  // Timeout Threshold

//-------------------------------------------------------------------
// PMU MODE DEFINITIONS
//-------------------------------------------------------------------
#define PMU_ACTIVE 0x1
#define PMU_SLEEP  0x0

//-------------------------------------------------------------------
// PMU TEMPERATURE STATE VALUES
//-------------------------------------------------------------------
#define PMU_10C 0x0
#define PMU_20C 0x1
#define PMU_25C 0x2
#define PMU_35C 0x3
#define PMU_55C 0x4
#define PMU_75C 0x5
#define PMU_95C 0x6

//-------------------------------------------------------------------
// HIGH POWER HISTORY VARIABLE
//-------------------------------------------------------------------
volatile uint32_t __high_power_history__;

//*******************************************************************
// TMCv2A FUNCTIONS
//*******************************************************************

//-------------------------------------------------------------------
// Basic Building Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: set_bits
// Args    : var        - Original Value
//           msb_idx    - Index of MSB
//           lsb_idx    - Index of LSB
//           value      - New value
// Description:
//          set_bits() replaces [msb_idx:lsb_idx] in var with value
//          and returns the result
// Return  : var with [msb_idx:lsb_idx] replaced with value
//-------------------------------------------------------------------
uint32_t set_bits (uint32_t var, uint32_t msb_idx, uint32_t lsb_idx, uint32_t value);

//-------------------------------------------------------------------
// Function: set_bit
// Args    : var    - Original Value
//           idx    - Bit Index
//           value  - New bit value
// Description:
//          set_bit() replaces [idx] in var with value
//          and returns the result
// Return  : var with [idx] replaced with value
//-------------------------------------------------------------------
uint32_t set_bit (uint32_t var, uint32_t idx, uint32_t value);

//-------------------------------------------------------------------
// Function: get_bits
// Args    : variable   - Original Value
//           msb_idx    - Index of MSB
//           lsb_idx    - Index of LSB
// Description:
//          get_bits() grabs [msb_idx:lsb_idx] in variable,
//          right-shifts it by lsb_idx and returns the result.
// Return  : [msb_idx:lsb_idx] in variable right-shifted by lsb_idx
//-------------------------------------------------------------------
uint32_t get_bits (uint32_t variable, uint32_t msb_idx, uint32_t lsb_idx);

//-------------------------------------------------------------------
// Function: get_bit
// Args    : variable - Original Value
//           idx      - Bit Index
// Description:
//          get_bit() returns variable[idx]
// Return  : variable[idx] (it is always 1 bit)
//-------------------------------------------------------------------
uint32_t get_bit (uint32_t variable, uint32_t idx);

//-------------------------------------------------------------------
//[NOTE] The function below, rsh64, has been commented out
//       since it is no longer used in the firmware.
//-------------------------------------------------------------------
// Function: rsh64
// Args    : var  - 64-bit variable to be shifted by right
//           nb     - number of bits to be shifted
// Description:
//          It right-shifts 'var' by 'nb' bits and return.
//          Internally it breaks down 'var' into two 32-bit variables
//          and properly shifts them and then stitches them together
//          to return a 64-bit variable.
//
//          <NOTE>
//          The compiler creates '__aeabi_llsr' function
//          to handle a 64-bit variable shift.
//          It seems to work fine if the number of bits to be shifted 
//          is a constant (e.g., a = b >> 40).
//          However, our CPU gets stuck if the number of bits to shifted
//          is a variable (e.g., a = b >> c).
//          The assembly code looks like below:
//
//              000000a8 <__aeabi_llsr>:
//                a8:	e2523020 	subs	r3, r2, #32
//                ac:	e262c020 	rsb	ip, r2, #32
//                b0:	41a00230 	lsrmi	r0, r0, r2
//                b4:	51a00331 	lsrpl	r0, r1, r3
//                b8:	41800c11 	orrmi	r0, r0, r1, lsl ip
//                bc:	e1a01231 	lsr	r1, r1, r2
//                c0:	e12fff1e 	bx	lr
//              ...
//
//              0000341c <snt_operation>:
//              ...
//              398a:	9b04      	ldr	r3, [sp, #16]
//              398c:	2280      	movs	r2, #128	; 0x80
//              398e:	1b52      	subs	r2, r2, r5
//              3990:	9803      	ldr	r0, [sp, #12]
//              3992:	1c39      	adds	r1, r7, #0
//              3994:	401c      	ands	r4, r3
//              3996:	f7fc fb87 	bl	a8 <__aeabi_llsr>
//              399a:	0136      	lsls	r6, r6, #4
//              399c:	1c02      	adds	r2, r0, #0
//              399e:	1c0b      	adds	r3, r1, #0
//              39a0:	1c20      	adds	r0, r4, #0
//              39a2:	2100      	movs	r1, #0
//              ...
//
// Return  : 'var' right-shifted by 'nb' bits
//-------------------------------------------------------------------
//uint64_t rsh64 (uint64_t var, uint32_t nb);

//-------------------------------------------------------------------
// Math Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: get_lead1_pos
// Args    : val    - value
// Description:
//          Returns the bit position of the leading one.
//          LSB's bit position is 0,
//          thus the return value is in [0, 31].
//          If val is 0, it returns 32.
// Return  : Bit position of the leading one
//          32 if val=0
//-------------------------------------------------------------------
uint32_t get_lead1_pos (uint32_t val);

//-------------------------------------------------------------------
// Function: div
// Args    : numer - Numerator   (m fixed-point)
//           denom - Denominator (m fixed-point)
//           n     - Number of fixed-point decimal bits in result
// Description:
//          Returns the result of 'numer / denom'
//          numer and denom must be in the m fixed-point format,
//          and the result is in the n fixed-point format.
//          m and n can be same or different,
//          but both numer and denom must be m fixed-point numbers.
// Return  : numer / denom (n fixed-point)
//-------------------------------------------------------------------
uint32_t div (uint32_t numer, uint32_t denom, uint32_t n);

//-------------------------------------------------------------------
// Function: mult
// Args    : num_a - number (m fixed-point)
//           num_b - number (n fixed-point)
// Description:
//          Returns the result of 'num_a * num_b'
//          in the (m+n) fixed-point format
// NOTE:    Integers are 0 fixed-point numbers.
// Return  : num_a * num_b (m+n fixed-point)
//           0xFFFFFFFF    (if over-flow occurs)
//-------------------------------------------------------------------
uint32_t mult (uint32_t num_a, uint32_t num_b);

//-------------------------------------------------------------------
// Function: mult_b28
// Args    : num_a - number (m fixed-point)
//           num_b - number (N=28 fixed-point)
// Description:
//          Returns the result of 'num_a * num_b'
//          in the m fixed-point format
// Return  : num_a * num_b (m fixed-point))
//-------------------------------------------------------------------
uint32_t mult_b28 (uint32_t num_a, uint32_t num_b);

//-------------------------------------------------------------------
// High Power History
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: set_high_power_history
// Args    : val    - New value for __high_power_history__
// Description:
//          Set __high_power_history__ = val
// Return  : None
//-------------------------------------------------------------------
void set_high_power_history (uint32_t val);

//-------------------------------------------------------------------
// Function: get_high_power_history
// Args    : None
// Description:
//          Return __high_power_history__
// Return  : __high_power_history__
//-------------------------------------------------------------------
uint32_t get_high_power_history (void);


//*******************************************************************
// PRE FUNCTIONS (Specific to TMCv2A)
//*******************************************************************

//-------------------------------------------------------------------
// Function: restart_xo
// Args    : delay_a: (Recommended: 20000) Must be ~1 second delay. Delay for XO Start-Up. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
//           delay_b: (Recommended: 20000) Must be ~1 second delay. Delay for VLDO & IBIAS Generation. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
// Description:
//           Restart the XO driver
// Return  : None
//-------------------------------------------------------------------
void restart_xo(uint32_t delay_a, uint32_t delay_b);


//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

#define __PMU_CHECK_WRITE__                 // If enabled, PMU replies with an actual read-out data from the analog block, rather than just forwarding the write data.

#ifndef __USE_HCODE__
//-------------------------------------------------------------------
// PMU State
//-------------------------------------------------------------------
//                                  Default Value
// idx  state_name                  sleep   active
//-------------------------------------------------------------------
// [20] horizon                     1       0
// [19] vbat_read_only              0       0
// [18] vdd_0p6_turned_on           1       1
// [17] vdd_1p2_turned_on           1       1
// [16] vdd_3p6_turned_on           1       1
// [15] dnc_stabilized              1       1
// [14] dnc_on                      1       1
// [13] sar_ratio_adjusted          1       0
// [12] adc_adjusted                1       0
// [11] adc_output_ready            1       1
// [10] refgen_on                   1       1
// [9]  upc_stabilized              1       1
// [8]  upc_on                      1       1
// [7]  control_supply_switched     1       1
// [6]  clock_supply_switched       1       1
// [5]  sar_ratio_roughly_adjusted  1       1
// [4]  sar_stabilized              1       1
// [3]  sar_reset                   1       1
// [2]  wait_for_cap_charge         1       1
// [1]  wait_for_clock_cycles       1       1
// [0]  sar_on                      1       1
//-------------------------------------------------------------------
union pmu_state_t {
    struct {
        unsigned sar_on                     : 1;    // [0]
        unsigned wait_for_clock_cycles      : 1;    // [1]
        unsigned wait_for_cap_charge        : 1;    // [2]
        unsigned sar_reset                  : 1;    // [3]
        unsigned sar_stabilized             : 1;    // [4]
        unsigned sar_ratio_roughly_adjusted : 1;    // [5]
        unsigned clock_supply_switched      : 1;    // [6]
        unsigned control_supply_switched    : 1;    // [7]
        unsigned upc_on                     : 1;    // [8]
        unsigned upc_stabilized             : 1;    // [9]
        unsigned refgen_on                  : 1;    // [10]
        unsigned adc_output_ready           : 1;    // [11]
        unsigned adc_adjusted               : 1;    // [12]
        unsigned sar_ratio_adjusted         : 1;    // [13]
        unsigned dnc_on                     : 1;    // [14]
        unsigned dnc_stabilized             : 1;    // [15]
        unsigned vdd_3p6_turned_on          : 1;    // [16]
        unsigned vdd_1p2_turned_on          : 1;    // [17]
        unsigned vdd_0p6_turned_on          : 1;    // [18]
        unsigned vbat_read_only             : 1;    // [19]
        unsigned horizon                    : 1;    // [20]
    };
    uint32_t value;
};

#define __PMU_DESIRED_STATE_SLEEP_DEFAULT__  0x17FFFF
#define __PMU_DESIRED_STATE_ACTIVE_DEFAULT__ 0x07CFFF

volatile union pmu_state_t __pmu_desired_state_sleep__;   // Register 0x3B
volatile union pmu_state_t __pmu_desired_state_active__;  // Register 0x3C

//-------------------------------------------------------------------
// PMU Floor
//-------------------------------------------------------------------

union pmu_floor_t {
    struct {
        unsigned R      : 4;
        unsigned L      : 4;
        unsigned BASE   : 5;
        unsigned L_SAR  : 4;
    };
    uint32_t value;
};

volatile uint32_t __pmu_temp_state__; // Current PMU temperature. e.g., PMU_10C, PMU_20C, ...
#endif

//-------------------------------------------------------------------
// PMU ADC & SAR_RATIO CONFIGURATION
//-------------------------------------------------------------------
#define __PMU_ADC_PERIOD__          1   // Period of ADC VBAT measurement (Default: 500)
#define __PMU_TICK_ADC_RESET__      2   // Pulse Width of PMU's ADC_RESET signal (Default: 50)
#define __PMU_TICK_ADC_CLK__        2   // Pulse Width of PMU's ADC_CLK signal (Default: 2)

#define __PMU_ADC_UPPER_LIMIT__     164 // The ADC value is ignored if it is higher than __PMU_ADC_UPPER_LIMIT__
#define __PMU_ADC_LOWER_LIMIT__     64  // The ADC value is ignored if it is lower than __PMU_ADC_LOWER_LIMIT__

#define __PMU_ADC_SAR_HYSTERESIS__  1   // Hysteresis used in PMU SAR Adjustment

#define __PMU_NUM_CONS_MEAS__       5   // Number of consecutive measurements required to trigger Low VBAT warning/Crash Handler.

//-------------------------------------------------------------------
// PMU SAR MARGIN
//-------------------------------------------------------------------
// Only one must be enabled.
//#define __PMU_SAR_MARGIN_5_PERCENT__
//#define __PMU_SAR_MARGIN_7_PERCENT__
//#define __PMU_SAR_MARGIN_8_PERCENT__
//#define __PMU_SAR_MARGIN_9_PERCENT__
#define __PMU_SAR_MARGIN_10_PERCENT__
//#define __PMU_SAR_MARGIN_12_PERCENT__   // interpolated
//#define __PMU_SAR_MARGIN_15_PERCENT__

volatile uint32_t __pmu_sar_ratio__;                // Current PMU SAR Ratio
volatile uint32_t __pmu_sar_ratio_upper_streak__;   // Counter for SAR ratio change (upper direction - lower SAR ratio)
volatile uint32_t __pmu_sar_ratio_lower_streak__;   // Counter for SAR ratio change (lower direction - higher SAR ratio)
volatile uint32_t __pmu_low_vbat_streak__;          // Counter for LOW VBAT indicator
volatile uint32_t __pmu_crit_vbat_streak__;         // Counter for Crash Handler
volatile uint32_t __pmu_last_effective_adc_val__;   // Last ADC value that was used to change the SAR ratio

//-------------------------------------------------------------------
// PMU VSOLAR SHORT BEHAVIOR
//-------------------------------------------------------------------
// __PMU_VSOLAR_SHORT__ | EN_VSOLAR_SHORT
// Value                | Behavior
// ---------------------------------------
//   0x08 (default)     | 0 until POR activates; then 1 until V0P6 output turns on; then follows vbat_high_latched.
//   0x0A               | Always 0
//   0x0B               | Always 1
//   0x0C               | vbat_high_latched     (comparator output from PMU)
//   0x18               | vbat_high_lat_latched (latched comparator output from PMU)
// --------------------------------------
#define __PMU_VSOLAR_SHORT__        0x08    // (Default 5'h08) See table above
#define __PMU_VSOLAR_CLAMP_TOP__    0x0     // (Default 3'h0) Decrease clamp threshold voltage
#define __PMU_VSOLAR_CLAMP_BOTTOM__ 0x1     // (Default 3'h1) Increase clamp threshold voltage

//-------------------------------------------------------------------
// Function: pmu_reg_write_core
// Args    : reg_addr  - Register Address
//           reg_data  - Register Data
// Description:
//          pmu_reg_write_core() writes reg_data in PMU's reg_addr
// Return  : None
//-------------------------------------------------------------------
void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data);

//-------------------------------------------------------------------
// Function: pmu_reg_write
// Args    : reg_addr - Register Address
//           reg_data - Register Data
// Description:
//          pmu_reg_write() writes reg_data in PMU's reg_addr
// Return  : None
//-------------------------------------------------------------------
void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data);

//-------------------------------------------------------------------
// Function: pmu_reg_read
// Args    : reg_addr
// Description:
//          pmu_reg_read() read from reg_addr in PMU.
// Return  : Read Data
//-------------------------------------------------------------------
uint32_t pmu_reg_read (uint32_t reg_addr);

//-------------------------------------------------------------------
// Function: pmu_config_rat
// Args    : rat    - If 1, it enables RAT. It also enables slow-loop in Sleep. (Slow-loop in Active is OFF).
//                    If 0, it disables RAT. It also disables slow-loop in Sleep and Active.
// Description:
//           Enables/Disables RAT and Slow-Loop in Sleep.
//           Unless overridden, changes in RAT_CONFIG[19:13] do not take effect
//           until the next sleep or wakeup-up transition.
// Return  : None
//-------------------------------------------------------------------
void pmu_config_rat(uint8_t rat);

//-------------------------------------------------------------------
// Function: pmu_set_floor
// Args    : mode   - If 1, change settings for Active registers.
//                    If 0, change settings for Sleep registers.
//           r      - Right-Leg Multiplier
//           l      - Left-Leg Multiplier (for UPC & DNC)
//           base   - Base Value
//           l_sar  - Left-Leg Multiplier (for SAR)
// Description:
//           Set the VCO floor setting
// Return  : None
//-------------------------------------------------------------------
void pmu_set_floor(uint8_t mode, uint8_t r, uint8_t l, uint8_t base, uint8_t l_sar);

#ifndef __USE_HCODE__
//-------------------------------------------------------------------
// Function: pmu_set_temp_state
// Args    : val    - new value for __pmu_temp_state__
// Description:
//           Set __pmu_temp_state__, the state for the current temperature.
//           __pmu_temp_state__ must be valid before executing pmu_set_*_temp_based() functions.
//           By default, pmu_init() sets __pmu_temp_state__ = 0x2 (PMU_25C)
// Return  : None
//-------------------------------------------------------------------
void pmu_set_temp_state (uint32_t val);

//-------------------------------------------------------------------
// Function: pmu_get_temp_state
// Args    : None
// Description:
//           Get the current __pmu_temp_state__, the state for the current temperature.
//           __pmu_temp_state__ must be valid before executing pmu_set_*_temp_based() functions.
//           By default, pmu_init() sets __pmu_temp_state__ = 0x2 (PMU_25C)
// Return  : Current __pmu_temp_state__
//-------------------------------------------------------------------
uint32_t pmu_get_temp_state (void);

//-------------------------------------------------------------------
// Function: pmu_set_active_temp_based
// Args    : None
// Description:
//           Sets PMU active floors based on temperature. 
//           __pmu_temp_state__ must be properly set before using this function.
// Return  : None
//-------------------------------------------------------------------
void pmu_set_active_temp_based();

//-------------------------------------------------------------------
// Function: pmu_set_sleep_temp_based
// Args    : None
// Description:
//           Sets PMU sleep floors based on temperature.
//           __pmu_temp_state__ must be properly set before using this function.
// Return  : None
//-------------------------------------------------------------------
void pmu_set_sleep_temp_based();

//-------------------------------------------------------------------
// Function: pmu_set_sleep_low
// Args    : None
// Description:
//           Sets PMU sleep floors to its lowest setting.
// Return  : None
//-------------------------------------------------------------------
void pmu_set_sleep_low();
#endif

//-------------------------------------------------------------------
// Function: pmu_set_active_min
// Args    : None
// Description:
//           Sets PMU active floors to the minimum
//           R/L/Base = 1/0/1 for UPC/SAR/DNC
// Return  : None
//-------------------------------------------------------------------
void pmu_set_active_min();

//-------------------------------------------------------------------
// Function: pmu_set_sar_ratio
// Args    : ratio - desired SAR ratio
// Description:
//           Only if ratio is different from __pmu_sar_ratio__,
//           it changes the SAR ratio to 'ratio', and then performs
//           sar_reset and sar/upc/dnc_stabilized tasks
//           It also updates __pmu_sar_ratio__.
// Return  : None
//-------------------------------------------------------------------
void pmu_set_sar_ratio (uint32_t ratio);

//-------------------------------------------------------------------
// Function: pmu_get_sar_ratio
// Args    : None
// Description:
//           Get the current __pmu_sar_ratio__.
//           By default, pmu_init() sets __pmu_sar_ratio__ = 0x4C.
// Return  : Current __pmu_sar_ratio__ 
//-------------------------------------------------------------------
uint32_t pmu_get_sar_ratio (void);

//-------------------------------------------------------------------
// Function: pmu_read_adc
// Args    : None
// Description:
//           Read the latest ADC VBAT measurement from PMU.
// Return  : PMU ADC VBAT raw reading
//-------------------------------------------------------------------
uint32_t pmu_read_adc(void);

//-------------------------------------------------------------------
// Function: pmu_validate_adc_val
// Args    : adc_val
// Description:
//           Determines whether the given adc value is valid or not
// Return  : 1 if the adc value is in the range of __PMU_ADC_LOWER_LIMIT__ 
//              and __PMU_ADC_UPPER_LIMIT__
//           0 if not
//-------------------------------------------------------------------
uint32_t pmu_validate_adc_val (uint32_t adc_val);

//-------------------------------------------------------------------
// Function: pmu_calc_new_sar_ratio
// Args    : adc_val        - ADC raw code
//           offset         - ADC offset to be added to the threshold values
//                            for SAR ratio adjustment.
//                            8-bit, 2's complement.
//
// Description:
//           Find out a new SAR Ratio based on 
//           the current VBAT measurements, using the following equation:
//           For detailed algorithm, see TMCv2r1.c
//           NOTE: It does NOT actually change the SAR ratio
//
//           SAR margin is set using compiler directives, __PMU_SAR_MARGIN_X_PERCENT__,
//              where X is one of 5, 7, 8, 9, 10, 12, 15
//
//           Hysteresis is set using a compiler directive, __PMU_ADC_SAR_HYSTERESIS__.
//              This hysteresis is used only when "lowering" the SAR ratio (i.e., when the ADC value goes "up").
//              If (adc_val < previous_adc_value) -OR- (adc_val > (previous_adc_value + hysteresis)),
//              it calculates the new SAR ratio and returns the value. 
//              Otherwise, it skips the calculation and returns the current SAR ratio.
//
// Return  : New SAR Ratio
//-------------------------------------------------------------------
uint32_t pmu_calc_new_sar_ratio(uint32_t adc_val, uint32_t offset);

//-------------------------------------------------------------------
// Function: pmu_check_low_vbat
// Args    : adc_val        - ADC raw code
//           offset         - ADC offset to be added to the threshold values
//                            for SAR ratio adjustment.
//                            8-bit, 2's complement.
//           threshold      - ADC code threshold
// Description:
//           Check whether the VBAT becomes lower than the low value.
//
//           The number of consecutive measurements required for triggering the Low VBAT alarm
//           is set using a compiler directive, __PMU_NUM_CONS_MEAS__.
//
// Return  : 1 if adc_val < (threshold + offset) for more than 
//              num_cons_meas times consecutively.
//           0 otherwise.
//-------------------------------------------------------------------
uint32_t pmu_check_low_vbat (uint32_t adc_val, uint32_t offset, uint32_t threshold);

//-------------------------------------------------------------------
// Function: pmu_check_crit_vbat
// Args    : adc_val        - ADC raw code
//           offset         - ADC offset to be added to the threshold values
//                            for SAR ratio adjustment.
//                            8-bit, 2's complement.
//           threshold      - ADC code threshold
// Description:
//           Check whether the VBAT becomes lower than the critical value
//
//           The number of consecutive measurements required for triggering the Critical VBAT alarm
//           is set using a compiler directive, __PMU_NUM_CONS_MEAS__.
//
// Return  : 1 if adc_val < (threshold + offset) for more than 
//              num_cons_meas times consecutively.
//           0 otherwise.
//-------------------------------------------------------------------
uint32_t pmu_check_crit_vbat (uint32_t adc_val, uint32_t offset, uint32_t threshold);

//-------------------------------------------------------------------
// Function: pmu_sleep_floor
// Args    : type   -   If 1, it uses the floor settings for Temp Meas Sleep.
//                      If 0, it uses the floor settings for Normal Sleep.
// Description:
//           Set floor settings for Sleep
// Return  : None
//-------------------------------------------------------------------
void pmu_sleep_floor(uint32_t type);

//-------------------------------------------------------------------
// Function: pmu_init
// Args    : None
// Description:
//           Initializes the PMU's floor settings & SAR ratio
//              for the nominal condition.
//           It also disables PMU's ADC operation for Active mode
//              and sets ADC timing parameters.
// Return  : None
//-------------------------------------------------------------------
void pmu_init(void);


//*******************************************************************
// NFC FUNCTIONS
//*******************************************************************

// COTS Power Switch
#define __NFC_CPS__ 2

// GPIO Interface
// NOTE: Following variables may be overridden
#define __NFC_GPO__ 1
#define __NFC_SCL__ 3
#define __NFC_SDA__ 0
#define __I2C_MASK__        (1<<__NFC_SDA__)|(1<<__NFC_SCL__)
#define __I2C_SCL_MASK__    (1<<__NFC_SCL__)
#define __I2C_SDA_MASK__    (1<<__NFC_SDA__)
#define __GPO_MASK__        (1<<__NFC_GPO__)
#define __NFC_MASK__        (__I2C_MASK__|__GPO_MASK__)

// NFC Boot-Up Delay (t_bootDC); Must be >0.6ms; See datasheet
#define __NFC_TBOOTDC__     100  // NOTE: Boot-Up Delay is approx. 0.3ms + (__NFC_TBOOTDC__ x 0.03ms)

// EEPROM Write Time (Tw); Must be >5ms; See datasheet
#define __NFC_TW__  400

// Token "Retry" delay. When it fails to get the token, it waits for __NFC_TTOKEN_RETRY__ before the next attempt.
#define __NFC_TTOKEN_RETRY__  14000

// Max Num Retry for NAK handling
#define __NFC_NUM_RETRY__   5

// I2C "Retry" delay. When it is NAK'd, it waits for __NFC_RETRY_DELAY__ before the next attempt.
#define __NFC_RETRY_DELAY__  30000  // ~1s

// NFC Operating Temperature Range (10x(T+80))
#define __NFC_OP_MIN_TEMP__     500     // -30C


// Fast I2C
//      If enabled, TMC executes the low-level implementation of the I2C, 
//      which is much faster than the generic version.
//      This ignores __NFC_SCL__, __NFC_SDA__, and __I2C_*MASK__ directives.
// [WARNING] For XT1   (PP): Fast I2C seems to cause more coupling with the XO wires (hence more frequent XO failure)
// [WARNING] For XT1r1 (FP): Fast I2C seems to cause occasional failure
//#define __USE_FAST_I2C__

//-------------------------------------------------------------------
//[NOTE] __i2c_password_t__ and its instantiation (__i2c_password__)
//       have been commented out, since the firmware does not change the password.
//-------------------------------------------------------------------
//// NFC I2C password
//union __i2c_password_t__ {
//    struct {
//        unsigned lower: 32;
//        unsigned upper: 32;
//    };
//    uint64_t value;
//};
//volatile union __i2c_password_t__ __i2c_password__;

// Status Flag
volatile uint32_t __nfc_on__;
volatile uint32_t __nfc_i2c_token__;
volatile uint32_t __nfc_i2c_nak__;

//-------------------------------------------------------------------
// Function: nfc_init
// Args    : None
// Description:
//           Initialization for the NFC chip communication
// Return  : None
//-------------------------------------------------------------------
void nfc_init(void);

//-------------------------------------------------------------------
// Function: nfc_power_on
// Args    : None
// Description:
//           Initialize NFC GPIO pads and turn on the NFC.
//           It also unfreezes the GPIO pads.
// Return  : None
//-------------------------------------------------------------------
void nfc_power_on(void);

//-------------------------------------------------------------------
// Function: nfc_power_off
// Args    : None
// Description:
//           Turn off the NFC and freeze the GPIO.
//           After the execution, SCL/SDA are set to 'input'.
// Return  : None
//-------------------------------------------------------------------
void nfc_power_off(void);

//-------------------------------------------------------------------
// Function: nfc_i2c_start
// Args    : None
// Description:
//           I2C Start Sequence. 
//           If the NFC is not on, it first calls nfc_power_on() 
//           before executing the I2C start sequence.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(output) = 0
//           It also resets __nfc_i2c_nak__ status to 0.
// Return  : None
// Waveform:
//          If starting from SCL=SDA=1
//              SCL  ********|______________
//              SDA  ****|__________________
//          If starting from SCL(output)=0, SDA(input)=1
//              SCL  __|**********|________   
//              SDA  **********|___________   
//-------------------------------------------------------------------
void nfc_i2c_start(void);

//-------------------------------------------------------------------
// Function: nfc_i2c_stop
// Args    : None
// Description:
//           I2C Stop Sequence
//           At the end of the execution:
//              SCL(output) = 1
//              SDA(output) = 1
// Return  : None
// Waveform:
//          If starting from SCL(output)=0, SDA(output)=0
//              SCL  ____|******************
//              SDA  ________|**************
//          If starting from SCL(output)=0, SDA(input)=1
//              SCL  _______|***************
//              SDA  **|________|***********
//-------------------------------------------------------------------
void nfc_i2c_stop(void);

//-------------------------------------------------------------------
// Function: nfc_i2c_byte
// Args    : byte   - Byte to be sent (8 bits)
// Description:
//           Send 'byte' from MSB to LSB, then check ACK.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
//           NOTE: If __nfc_i2c_nak__=1, this function does nothing
//              and returns 0.
// Return  : 1  -   Acked
//           0  -   Not Acked (or it has been already Nacked before) 
//-------------------------------------------------------------------
uint32_t nfc_i2c_byte(uint8_t byte);

//-------------------------------------------------------------------
// Function: nfc_i2c_byte_imm
// Args    : byte   - Byte to be sent (8 bits)
// Description:
//           Send 'byte' from MSB to LSB, and return the ACK status.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
//           NOTE: If __nfc_i2c_nak__=1, this function does nothing 
//              and returns 0.
// Return  : 1: Acked
//           0: Not Acked
//-------------------------------------------------------------------
uint32_t nfc_i2c_byte_imm(uint8_t byte);

//-------------------------------------------------------------------
// Function: nfc_i2c_polling
// Args    : none
// Description:
//           This implements the polling on ACK
//           as described in Section 6.4.3. in the datasheet.
// Return  : none
//-------------------------------------------------------------------
void nfc_i2c_polling(void);

//-------------------------------------------------------------------
// Function: nfc_i2c_present_password()
// Args    : none
// Description:
//           It present the I2C password to open the I2C security session
//           You need to set the password using nfc_i2c_update_password() function.
//           If the password has not been set using nfc_i2c_update_password(),
//           then it uses the factory default password, which is 0x0000000000000000.
// Return  : 1  -   Acked
//           0  -   Not Acked
//-------------------------------------------------------------------
uint32_t nfc_i2c_present_password(void);

//-------------------------------------------------------------------
//[NOTE] The function below, nfc_i2c_update_password, has been commented out
//       since it is no longer used in the firmware.
//-------------------------------------------------------------------
// Function: nfc_i2c_update_password()
// Args    : password  : new password (64-bit)
// Description:
//           It updates the I2C password.
//           The I2C security session must be open before using nfc_i2c_update_password().
// Return  : 1  -   Acked
//           0  -   Not Acked
//-------------------------------------------------------------------
//uint32_t nfc_i2c_update_password(uint64_t password);

//-------------------------------------------------------------------
// Function: nfc_i2c_rd
// Args    : ack    - 1: Ack
//                    0: No Ack
// Description:
//           Provide 8 SCL cycles and read SDA at every posedge.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
//           NOTE: If __nfc_i2c_nak__=1, this function does nothing and immediately returns 0.
// Return  : 8-bit read data (uint8_t)
//           0 if there is an un-cleared Nak
//-------------------------------------------------------------------
uint8_t nfc_i2c_rd(uint8_t ack);


//-------------------------------------------------------------------
// NFC - I2C Token
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: nfc_i2c_get_token
// Args    : max_num_try    - Maximum number of trials to get the token
// Description:
//           Get the I2C token by disabling the RF function
// Return  : 1  -   Successfully got the token
//           0  -   Failed to get the token
//-------------------------------------------------------------------
uint32_t nfc_i2c_get_token(uint32_t max_num_try);

//-------------------------------------------------------------------
// Function: nfc_i2c_release_token
// Args    : None
// Description:
//           Release the I2C token and enable the RF function
// Return  : 1  -   Successfully released the token
//           0  -   Failed to release the token
//-------------------------------------------------------------------
uint32_t nfc_i2c_release_token(void);


//-------------------------------------------------------------------
// NFC - Byte Read/Write
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: nfc_i2c_byte_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
//           data   : Data to be written
//           nb     : Number of bytes to be sent.
//                      Valid Range: 1 - 4
// Description:
//           Write data starting at the address 'addr'
//              addr        =   data[7:0]
//              addr+1      =   data[15:8]
//              addr+2      =   data[23:16]
//              addr+3      =   data[31:24]
// Return  : 1  -   Acked
//           0  -   Not Acked
//-------------------------------------------------------------------
uint32_t nfc_i2c_byte_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t nb);

//-------------------------------------------------------------------
// Function: nfc_i2c_byte_read
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
//           nb     : Number of bytes to be read. 
//                      Valid Range: 1 - 4
// Description:
//           Read the specified number (nb) of bytes starting at the address 'addr'
// Return  : If nb=1: {        24'h0, data@(addr)}
//           If nb=2: {        16'h0, data@(addr+1), data@(addr)}
//           If nb=3: {         8'h0, data@(addr+2), data@(addr+1), data@(addr)}
//           If nb=4: {data@(addr+3), data@(addr+2), data@(addr+1), data@(addr)}
//           If nb=0 or nb>4: 32'h0
//           0 if NAK'd
//           NOTE: There is no distinction in the return value
//                 between the NAK'd (0) and the valid 'zero' data.
//-------------------------------------------------------------------
uint32_t nfc_i2c_byte_read(uint32_t e2, uint32_t addr, uint32_t nb);

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_byte_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
//           data   : Memory address of the data array.
//           nb     : Number of bytes to be sent.
//                      Valid Range: 1 - 256
// Description:
//           Write data starting at the address 'addr'
//              addr        =   data[0][7:0]
//              addr+1      =   data[0][15:8]
//              addr+2      =   data[0][23:16]
//              addr+3      =   data[0][31:24]
//              addr+4      =   data[1][7:0]
//              addr+5      =   data[1][15:8]
//              addr+6      =   data[1][23:16]
//              addr+7      =   data[1][31:24]
//              ...
//              addr+nb-1   =   data[int((nb-1)/4)][((nb-1)%4)*8+7:((nb-1)%4)*8]
// Return  : 1  -   Acked
//           0  -   Not Acked
//-------------------------------------------------------------------
uint32_t nfc_i2c_seq_byte_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t nb);



//-------------------------------------------------------------------
// NFC - Word Read/Write
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Function: nfc_i2c_word_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address ( 2 LSBs are internally forced to 0 to make it word-aligned)
//           data   : Memory address of the data array,
//                    where each element is a word (4 bytes)
//           nw     : Number of words (4-bytes) to be sent.
//                      Valid Range: 1 - 64
// Description:
//           Write data starting at the address 'addr'
//              addr        =   data[0][ 7: 0]
//              addr+1      =   data[0][15: 8]
//              addr+2      =   data[0][24:16]
//              addr+3      =   data[0][31:25]
//              addr+4      =   data[1][ 7: 0]
//              ...
//              addr+4*nw-1 =   data[nw-1][31:25]
// Return  : 1  -   Acked
//           0  -   Not Acked
//-------------------------------------------------------------------
uint32_t nfc_i2c_word_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t nw);

//-------------------------------------------------------------------
// Function: nfc_i2c_word_read
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
//           nw     : Number of words to be read. 
//                      Valid Range: 1 - 64
//           ptr    : (volatile) Pointer where the read data is stored
// Description:
//           Read the specified number (nw) of words starting at the address 'addr'
//           The result is stored in ptr
//           *(ptr+0) = {data[addr+3], data[addr+2], data[addr+1], data[addr+0]}
//           *(ptr+1) = {data[addr+7], data[addr+6], data[addr+5], data[addr+4]}
//           ...
// Return  : 1 if successful
//           0 if NAK occurs
//-------------------------------------------------------------------
uint32_t nfc_i2c_word_read(uint32_t e2, uint32_t addr, uint32_t nw, volatile uint32_t* ptr);

//-------------------------------------------------------------------
// Function: nfc_i2c_word_pattern_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address ( 2 LSBs are internally forced to 0 to make it word-aligned)
//           data   : 32-bit data to be written
//           nw     : Number of words (4-bytes) to be sent.
//                      Valid Range: 1 - 64
// Description:
//           Write data starting at the address 'addr'
//              addr        =   data[ 7: 0]
//              addr+1      =   data[15: 8]
//              addr+2      =   data[24:16]
//              addr+3      =   data[31:25]
//              addr+4      =   data[ 7: 0]
//              ...
//              addr+4*nw-1 =   data[31:25]
// Return  : 1  -   Acked
//           0  -   Not Acked
//-------------------------------------------------------------------
uint32_t nfc_i2c_word_pattern_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t nw);


//*******************************************************************
// EID FUNCTIONS
//*******************************************************************

//-------------------------------------------------------------------
// __eid_current_display__
//-------------------------------------------------------------------
// Stores the current e-Ink display.
// It gets updated in eid_enable_cp_ck().
// Its default value is set to 0 in eid_init(), thus its value may be
// invalid until the first use of eid_enable_cp_ck();
//-------------------------------------------------------------------
volatile uint32_t __eid_current_display__;

//-------------------------------------------------------------------
// __eid_cp_duration__
//-------------------------------------------------------------------
// Charge Pump Activation Duration
//-------------------------------------------------------------------
// NOTE: In silicon, the FSM clock speed (=TMR clock frequency / 16) 
//       is 120~125Hz at RT, which means, LSB corresponds to ~8ms.
//-------------------------------------------------------------------
// Default: 250; Max: 65535
//-------------------------------------------------------------------
volatile uint32_t __eid_cp_duration__;

//--------------------------------------------------------
// EID Crash Detector & Handler
//--------------------------------------------------------
//
// X: WCTR_IDLE_WIDTH
// Y: WCTR_PULSE_WIDTH
//
//    crash detected
//           v
//                  <-Seq A->       <-Seq B->       <-Seq C->
//  VOUT_*   _______|*******|_______|*******|_______|*******|_______________
// Duration  <--X--> <--Y--> <--X--> <--Y--> <--X--> <--Y--> <--X-->
// dead      _______________________________________________________|*******
// PG_DIODE  <--A0-> <--A--> <--A1-> <--B--> <--B1-> <--C--> <--C1->|*******
//
// PG_DIODE  *******|_______________________________________________|*******
// (intended)
//
//////////////////////////////////////////////////////////

#ifndef __USE_HCODE__
union crsh_seq_t {
    struct {
        unsigned seg        : 9; //[8:0] CK Value for Segments (SEG[8:0]
        unsigned fd         : 1; //[  9] CK Value for Field (FD)
        unsigned te         : 1; //[ 10] CK Value for Top Electrode (TE)
        unsigned vin        : 1; //[ 11] Use 0 if VBAT is low; Use 1 if VBAT is high.
    };
    uint32_t value;
};

volatile union crsh_seq_t __crsh_seqa__;    //Sequence A (valid only if WCTR_SEL_SEQ[0]=1)
volatile union crsh_seq_t __crsh_seqb__;    //Sequence B (valid only if WCTR_SEL_SEQ[1]=1)
volatile union crsh_seq_t __crsh_seqc__;    //Sequence C (valid only if WCTR_SEL_SEQ[2]=1)
#endif

//-------------------------------------------------------------------
// EID Register File
//-------------------------------------------------------------------
#ifndef __USE_HCODE__
volatile eid_r00_t eid_r00;
volatile eid_r01_t eid_r01;
volatile eid_r02_t eid_r02;
#endif

//-------------------------------------------------------------------
// Function: eid_init
// Args    : None
// Description:
//           Initializes the EID layer
//           NOTE: EID_CTRL and WD_CTRL use a clock whose frequency is 16x slower than the raw timer clock.
//                 The raw clock should be ~1.5kHz, thus the EID_CTRL and WD_CTRL may use ~94Hz clock.
// Return  : None
//-------------------------------------------------------------------
void eid_init(void);

//-------------------------------------------------------------------
// Function: eid_enable_timer
// Args    : None
// Description:
//           Enable the EID timer
// Return  : None
//-------------------------------------------------------------------
void eid_enable_timer(void);

#ifndef __USE_HCODE__
//-------------------------------------------------------------------
// Function: eid_config_cp_clk_gen
// Args    : ring   : Sets ECP_SEL_RING    (Default: 0x0; Max: 0x3) Selects # of rings. 0x0: 11 stages; 0x3: 5 stages.
//           te_div : Sets ECP_SEL_TE_DIV  (Default: 0x0; Max: 0x3) Selects clock division ratio for Top Electrode (TE) charge pump. Divide the core clock by 2^EID_CP_CLK_SEL_TE_DIV.
//           fd_div : Sets ECP_SEL_FD_DIV  (Default: 0x0; Max: 0x3) Selects clock division ratio for Field (FD) charge pump. Divide the core clock by 2^EID_CP_CLK_SEL_FD_DIV.
//           seg_div: Sets ECP_SEL_SEG_DIV (Default: 0x0; Max: 0x3) Selects clock division ratio for Segment (SEG) charge pumps. Divide the core clock by 2^EID_CP_CLK_SEL_SEG_DIV.
// Description:
//           Configures the CP Clock Generator
// Return  : None
//-------------------------------------------------------------------
void eid_config_cp_clk_gen(uint32_t ring, uint32_t te_div, uint32_t fd_div, uint32_t seg_div);
#endif

//-------------------------------------------------------------------
// Function: eid_set_vin
// Args    : vin - Charge Pump's VIN value
//              1: Use VSS  as the charge pump input (recommended when VBAT is high) 
//                  [WARNING: VIN=1 may affect the EID timer when used with TMR_SEL_LDO=1]
//              0: Use VBAT as the charge pump input (recommended when VBAT is low)
// Description:
//           Set the charge pump's VIN value
// Return  : None
//-------------------------------------------------------------------
void eid_set_vin(uint32_t vin);

//-------------------------------------------------------------------
// Function: eid_set_duration
// Args    : duration - Duration of Charge Pump Activation 
//                          (Default: 400; Max: 65535)
// Description:
//           Set the duration of the charge pump activation 
//           It also sets the duration for the Field Erase
//              If duration > 1: Field Erase Duration = duration / 2
//              Otherwise, Field Erase Duration is set to duration.
// Return  : None
//-------------------------------------------------------------------
void eid_set_duration(uint32_t duration);

//-------------------------------------------------------------------
// Function: eid_enable_cp_ck
// Args    : te  - If 1, Top Electrode (TE) charge pump output goes high (~15V).
//                 If 0, Top Electrode (TE) charge pump output remains low at 0V.
//           fd  - If 1, Field (FD) charge pump output goes high (~15V).
//                 If 0, Field (FD) charge pump output remains low at 0V.
//           seg - For n=0,1,...,8:
//                 If bit[n]=1, SEG[n] charge pump output goes high (~15V).
//                 If bit[n]=0, SEG[n] charge pump output remains low at 0V.
// Description:
//           Enable the clock for the specified charge pumps
//              to make the outputs go high (~15V)
// Return  : None
//-------------------------------------------------------------------
void eid_enable_cp_ck(uint32_t te, uint32_t fd, uint32_t seg);

//-------------------------------------------------------------------
// Function: eid_all_black
// Args    : None
// Description:
//           Make the entire display black
// Return  : None
//-------------------------------------------------------------------
void eid_all_black(void);

//-------------------------------------------------------------------
// Function: eid_all_white
// Args    : None
// Description:
//           Make the entire display white
// Return  : None
//-------------------------------------------------------------------
void eid_all_white(void);

//-------------------------------------------------------------------
// Function: eid_seg_black
// Args    : seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] black.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the selected segment(s) black.
//           NOTE: Field (FD) must be white first.
// Return  : None
//-------------------------------------------------------------------
void eid_seg_black(uint32_t seg);

//-------------------------------------------------------------------
// Function: eid_seg_white
// Args    : seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] white.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the selected segment(s) white.
//           NOTE: Field (FD) must be black first.
// Return  : None
//-------------------------------------------------------------------
void eid_seg_white(uint32_t seg);

//-------------------------------------------------------------------
// Function: eid_update_global
// Args    : seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] black.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the entire display white,
//           and then make the selected segment(s) black.
// Return  : None
//-------------------------------------------------------------------
void eid_update_global(uint32_t seg);

//-------------------------------------------------------------------
// Function: eid_update_local
// Args    : seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] black.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Perform Local Update.
// Return  : None
//-------------------------------------------------------------------
void eid_update_local(uint32_t seg);

//-------------------------------------------------------------------
// Function: eid_enable_crash_handler
// Args    : None
// Description:
//           Enable EID crash detector & handler
// Return  : None
//-------------------------------------------------------------------
void eid_enable_crash_handler(void);

//-------------------------------------------------------------------
// Function: eid_check_in
// Args    : None
// Description:
//           EID Watchdog Check-In.
//           Must be done before the watchdog expires.
// Return  : None
//-------------------------------------------------------------------
void eid_check_in(void);

//-------------------------------------------------------------------
// Function: eid_trigger_crash
// Args    : None
// Description:
//           Trigger the crash behavior in EID
// Return  : None
//-------------------------------------------------------------------
void eid_trigger_crash(void);


//*******************************************************************
// SNT FUNCTIONS
//*******************************************************************

// ACK Timeout
#define __SNT_WUP_READ_TIMEOUT__         50000   // Checked using TIMER32
#define __FCODE_SNT_WUP_READ_TIMEOUT__   7       // Failure code to be displayed if timeout during SNT WUP Timer Read

//-------------------------------------------------------------------
// SNT Register File
//-------------------------------------------------------------------
#ifndef __USE_HCODE__
volatile sat_r00_t sat_r00;
volatile sat_r01_t sat_r01;
volatile sat_r08_t sat_r08;
volatile sat_r09_t sat_r09;
#endif
volatile sat_r0C_t sat_r0C;
volatile sat_r0F_t sat_r0F;
volatile sat_r13_t sat_r13;
volatile sat_r17_t sat_r17;

//-------------------------------------------------------------------
// Function: sat_init
// Args    : None
// Description:
//           Initializes SNT variables
// Return  : None
//-------------------------------------------------------------------
void sat_init(void);

void sat_acc_ldo_power_on(void);
void sat_acc_ldo_power_off(void);

void sat_acc_cp_power_on(void);
void sat_acc_cp_power_off(void);
void sat_acc_safr_clk_on(void);
void sat_acc_safr_clk_off(void);

void sat_acc_md_enable(void);
void sat_acc_md_disable(void);
void sat_acc_get_md_cnt(void);
void sat_acc_scan_phi_h(void);
//-------------------------------------------------------------------
// Function: sat_temp_sensor_power_on
// Description:
//           Power on SNT Temp Sensor
//           The Temp Sensor is configured to run off the LDO.
// Return  : None
//-------------------------------------------------------------------
void sat_temp_sensor_power_on(void);

//-------------------------------------------------------------------
// Function: sat_temp_sensor_power_off
// Args    : None
// Description:
//           Turns off SNT Temp Sensor
//           NOTE: It is supposed to be used *after* sat_temp_sensor_start().
// Return  : None
//-------------------------------------------------------------------
void sat_temp_sensor_power_off(void);

//-------------------------------------------------------------------
// Function: sat_temp_sensor_start
// Args    : None
// Description:
//           Release the reset of SNT Temp Sensor
// Return  : None
//-------------------------------------------------------------------
void sat_temp_sensor_start(void);

//-------------------------------------------------------------------
// Function: sat_start_timer
// Args    : wait_time  - Wait time before turning off the sloscillator
//                        It measure the delay using delay(wait_time).
//                        Requirement: >400ms at RT
// Description:
//           Start the SNT timer.
// Return  : None
//-------------------------------------------------------------------
void sat_start_timer(uint32_t wait_time);

//-------------------------------------------------------------------
// Function: sat_stop_timer
// Args    : None
// Description:
//           Stop the SNT timer
// Return  : None
//-------------------------------------------------------------------
void sat_stop_timer(void);

//-------------------------------------------------------------------
// Function: sat_set_timer_threshold
// Args    : threshold  - SNT Wakeup timer threshold (32-bit)
// Description:
//           (Synchronously) set the threshold for the SNT Wakeup timer
// Return  : 1 if SNT returns the correct threshold[23:0]
//           0 if SNT return value does not match threshold[23:0]
//-------------------------------------------------------------------
uint32_t sat_set_timer_threshold(uint32_t threshold);

//-------------------------------------------------------------------
// Function: sat_enable_wup_timer
// Args    : auto_reset -   0: Auto Reset is disabled
//                          1: Auto Reset is enabled
// Description:
//           Enable the SNT wakeup timer so that the wakeup counter starts running.
//           If auto_reset=1, the Auto Reset feature is also enabled,
//           where the SNT wakeup counter gets automatically reset upon the system
//           going into sleep.
// Return  : None
//-------------------------------------------------------------------
void sat_enable_wup_timer (uint32_t auto_reset);

//-------------------------------------------------------------------
// Function: sat_disable_wup_timer
// Args    : None
// Description:
//           Disable the SNT wakeup timer. Once executed, the SNT
//           wakeup counter stops running and becomes 0.
// Return  : None
//-------------------------------------------------------------------
void sat_disable_wup_timer (void);

//-------------------------------------------------------------------
// Function: sat_set_wup_timer
// Args    : threshold  -   SNT Wakeup timer threshold (32-bit)
//           auto_reset -   0: Auto Reset is disabled
//                          1: Auto Reset is enabled
// Description:
//           (Synchronously) set the threshold for the SNT wakeup timer,
//           then enables the SNT wakeup timer so that the wakeup counter starts running.
//           wakeup counter stops running.
//           If auto_reset=1, the Auto Reset feature is also enabled,
//           where the SNT wakeup counter gets automatically reset upon the system
//           going into sleep.
// Return  : 1 if SNT returns the correct threshold[23:0]
//           0 if SNT return value does not match threshold[23:0]
//-------------------------------------------------------------------
uint32_t sat_set_wup_timer(uint32_t auto_reset, uint32_t threshold);

//-------------------------------------------------------------------
// Function: sat_sync_read_wup_timer
// Args    : none
// Description:
//           Synchronous read the SNT wakeup timer counter value.
//           The upper 8-bit is written to REG4 (in PRE)
//           and the lower 24-bit is written to REG5 (in PRE).
//           Then it combines REG4 and REG5 and returns the 32-bit value.
// Return  : The current 32-bit SNT Wakeup counter value read synchronously
//-------------------------------------------------------------------
uint32_t sat_sync_read_wup_timer(void);

//-------------------------------------------------------------------
// Function: sat_read_wup_timer
// Args    : none
// Description:
//           Perform sat_sync_read_wup_timer() up to SNT_WUP_READ_MAX_TRY times
//           until it returns close value twice consecutively.
//           The 'closeness' is defined as the difference of the two values
//           being less than SNT_WUP_READ_MAX_ERROR.
// Return  : The current 32-bit SNT Wakeup counter value
//-------------------------------------------------------------------
#define SNT_WUP_READ_MAX_TRY    5
#define SNT_WUP_READ_MAX_ERROR  16
uint32_t sat_read_wup_timer(void);


//-------------------------------------------------------------------
// Temperature Conversion
//-------------------------------------------------------------------

//#define TCONV_DEBUG

//-------------------------------------------------------------------
// Function: tconv_log2_table/tconv_log2_taylor
// Args    : idx - table index (0 - 1023)
// Description:
//           Returns the indexed value from: 
//              the log2 lookup table (tconv_log2_table) -OR-
//              the 8-th order Taylor approximation (tconv_log2_taylor)
//           idx=1023 returns a dummy value, which is 0.
// Return  : log2(1 + (idx+1)/1024) in n=11 accuracy
//-------------------------------------------------------------------
uint32_t tconv_log2_table (uint32_t idx); // Using a look-up table
uint32_t tconv_log2_taylor (uint32_t idx);     // Using a Taylor approximation

//-------------------------------------------------------------------
// Function: tconv
// Args    : dout   - raw code (integer)
//           a      - Temperature Coefficient a (fixed-point, N=10)
//           b      - Temperature Coefficient b (fixed-point, N=10)
//           offset - Offset to be subtracted from the result (integer, unit: K)
// Description:
//          Performs the temperature conversion.
//          Returned value is
//              10 x (T + 273 - offset)
//              which is an integer, 
//              where T is the actual temperature in celsius degree
//          If (T+273 < offset), it returns 0.
// Return  : 10 x (T + 273 - offset) (positive integer)
//-------------------------------------------------------------------
uint32_t tconv (uint32_t dout, uint32_t a, uint32_t b, uint32_t offset);

#endif // TMCV2A_H
