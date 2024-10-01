//*******************************************************************************************
// CISv4Br1 HEADER FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//-------------------------------------------------------------------------------------------
//      PREv23E -> RDCv4 -> SNTv6r1 -> PMUv13r2
//-------------------------------------------------------------------------------------------
// Short Address
//-------------------------------------------------------------------------------------------
//      PRE: 0x1
//      RDC: 0x2
//      SNT: 0x3
//      PMU: 0x4
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//#define DEVEL

#ifndef CISV4B_H
#define CISV4B_H
#define CISV4B

#include "mbus.h"
#include "PREv23E.h"
#include "RDCv4_RF.h"
#include "SNTv6_RF.h"
#include "PMUv13r1_RF.h"

//-------------------------------------------------------------------
// Options
//-------------------------------------------------------------------
#define __USE_HCODE__               // Use hard-coded values (sacrificing readability, reducing code size)

//-------------------------------------------------------------------
// Short Addresses
//-------------------------------------------------------------------
#define PRE_ADDR    0x1
#define RDC_ADDR    0x2
#define SNT_ADDR    0x3
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
#define RDC_TARGET_REG_IDX  0x2 // RDC Pressure Sensor
#define WP0_TARGET_REG_IDX  0x4 // SNT Timer (Timer Read, Threshold Update)
#define WP1_TARGET_REG_IDX  0x5 //  NOTE: Only WP1_TARGET_REG_IDX is used to trigger an M0 IRQ. WP0_TARGET_REG_IDX is used to handle 32-bit data.
#define PLD_TARGET_REG_IDX  0x6 // Payload Register (Wakeup by SNT Timer, etc); It is not used to trigger an IRQ.

#define PMU_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (PMU_TARGET_REG_IDX << 2)))
#define SNT_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (SNT_TARGET_REG_IDX << 2)))
#define RDC_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (RDC_TARGET_REG_IDX << 2)))
#define WP0_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (WP0_TARGET_REG_IDX << 2)))
#define WP1_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (WP1_TARGET_REG_IDX << 2)))
#define PLD_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (PLD_TARGET_REG_IDX << 2)))

//-------------------------------------------------------------------
// PRE-DEFINED FREQUENCIES
//-------------------------------------------------------------------
// PRE Clock Generator Frequency
#define CPU_CLK_FREQ    140000  // XT1F#1, SAR_RATIO=0x60 (Sleep Voltages: 4.55V/1.59V/0.79V)

// PRE Clock-based Delay (5 instructions @ CPU_CLK_FREQ)
#define DLY_1S      ((CPU_CLK_FREQ>>3)+(CPU_CLK_FREQ>>4)+(CPU_CLK_FREQ>>5))  // = CPU_CLK_FREQx(1/8 + 1/16 + 1/32) = CPU_CLK_FREQx(35/160) ~= (CPU_CLK_FREQ/5)
#define DLY_1MS     DLY_1S >> 10                                             // = DLY_1S / 1024 ~= DLY_1S / 1000

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

//*******************************************************************
// TMCv2r1 FUNCTIONS
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


//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

#define __PMU_CHECK_WRITE__                 // If enabled, PMU replies with an actual read-out data from the analog block, rather than just forwarding the write data.

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
#define __PMU_SAR_MARGIN_8_PERCENT__
//#define __PMU_SAR_MARGIN_9_PERCENT__
//#define __PMU_SAR_MARGIN_10_PERCENT__
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

//-------------------------------------------------------------------
// Function: pmu_active_floor
// Args    : r      - Right-Leg Multiplier
//           l      - Left-Leg Multiplier (for UPC & DNC)
//           base   - Base Value
//           l_sar  - Left-Leg Multiplier (for SAR)
// Description:
//           Set the VCO floor setting (Active)
// Return  : None
//-------------------------------------------------------------------
void pmu_active_floor(uint8_t r, uint8_t l, uint8_t base, uint8_t l_sar);

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
// RDC FUNCTIONS
//*******************************************************************


//*******************************************************************
// SNT FUNCTIONS
//*******************************************************************

//-------------------------------------------------------------------
// SNT Register File
//-------------------------------------------------------------------
volatile snt_r00_t snt_r00;
volatile snt_r01_t snt_r01;
volatile snt_r08_t snt_r08;
volatile snt_r09_t snt_r09;
volatile snt_r17_t snt_r17;

//-------------------------------------------------------------------
// Function: snt_init
// Args    : None
// Description:
//           Initializes SNT variables
// Return  : None
//-------------------------------------------------------------------
void snt_init(void);

//-------------------------------------------------------------------
// Function: snt_temp_sensor_power_on
// Description:
//           Power on SNT Temp Sensor
//           The Temp Sensor is configured to run off the LDO.
// Return  : None
//-------------------------------------------------------------------
void snt_temp_sensor_power_on(void);

//-------------------------------------------------------------------
// Function: snt_temp_sensor_power_off
// Args    : None
// Description:
//           Turns off SNT Temp Sensor
//           NOTE: It is supposed to be used *after* snt_temp_sensor_start().
// Return  : None
//-------------------------------------------------------------------
void snt_temp_sensor_power_off(void);

//-------------------------------------------------------------------
// Function: snt_temp_sensor_start
// Args    : None
// Description:
//           Release the reset of SNT Temp Sensor
// Return  : None
//-------------------------------------------------------------------
void snt_temp_sensor_start(void);

//-------------------------------------------------------------------
// Function: snt_start_timer
// Args    : wait_time  - Wait time before turning off the sloscillator
//                        It measure the delay using delay(wait_time).
//                        Requirement: >400ms at RT
// Description:
//           Start the SNT timer.
// Return  : None
//-------------------------------------------------------------------
void snt_start_timer(uint32_t wait_time);

//-------------------------------------------------------------------
// Function: snt_stop_timer
// Args    : None
// Description:
//           Stop the SNT timer
// Return  : None
//-------------------------------------------------------------------
void snt_stop_timer(void);

//-------------------------------------------------------------------
// Function: snt_restart_timer
// Args    : None
// Description:
//           Stop and restart the SNT timer
// Return  : None
//-------------------------------------------------------------------
void snt_restart_timer(void);

//-------------------------------------------------------------------
// Function: snt_set_timer_threshold
// Args    : threshold  - SNT Wakeup timer threshold (32-bit)
// Description:
//           (Synchronously) set the threshold for the SNT Wakeup timer
// Return  : threshold
//-------------------------------------------------------------------
uint32_t snt_set_timer_threshold(uint32_t threshold);

//-------------------------------------------------------------------
// Function: snt_enable_wup_timer
// Args    : auto_reset -   0: Auto Reset is disabled
//                          1: Auto Reset is enabled
// Description:
//           Enable the SNT wakeup timer so that the wakeup counter starts running.
//           If auto_reset=1, the Auto Reset feature is also enabled,
//           where the SNT wakeup counter gets automatically reset upon the system
//           going into sleep.
// Return  : None
//-------------------------------------------------------------------
void snt_enable_wup_timer (uint32_t auto_reset);

//-------------------------------------------------------------------
// Function: snt_disable_wup_timer
// Args    : None
// Description:
//           Disable the SNT wakeup timer. Once executed, the SNT
//           wakeup counter stops running and becomes 0.
// Return  : None
//-------------------------------------------------------------------
void snt_disable_wup_timer (void);

//-------------------------------------------------------------------
// Function: snt_set_wup_timer
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
//           This function internally calls snt_set_timer_threshold()
//           to set the threshold. Please see the description of
//           snt_set_timer_threshold() as well.
// Return  : threshold
//-------------------------------------------------------------------
uint32_t snt_set_wup_timer(uint32_t auto_reset, uint32_t threshold);

//-------------------------------------------------------------------
// Function: snt_sync_read_wup_timer
// Args    : none
// Description:
//           Synchronous read the SNT wakeup timer counter value.
//           The upper 8-bit is written to REG4 (in PRE)
//           and the lower 24-bit is written to REG5 (in PRE).
//           Then it combines REG4 and REG5 and returns the 32-bit value.
// Return  : The current 32-bit SNT Wakeup counter value read synchronously
//-------------------------------------------------------------------
uint32_t snt_sync_read_wup_timer(void);

//-------------------------------------------------------------------
// Function: snt_read_wup_timer
// Args    : None
// Description:
//           Perform snt_sync_read_wup_timer() up to SNT_WUP_READ_MAX_TRY times
//           until it returns close value twice consecutively.
//           The 'closeness' is defined as the difference of the two values
//           being less than SNT_WUP_READ_MAX_ERROR.
// Return  : The current 32-bit SNT Wakeup counter value
//-------------------------------------------------------------------
#define SNT_WUP_READ_MAX_TRY    5
#define SNT_WUP_READ_MAX_ERROR  16
uint32_t snt_read_wup_timer(void);

//*******************************************************************
// RDC FUNCTIONS
//*******************************************************************

//-------------------------------------------------------------------
// SNT Register File
//-------------------------------------------------------------------
volatile rdc_r20_t rdc_r20;
volatile rdc_r25_t rdc_r25;
volatile rdc_r28_t rdc_r28;
volatile rdc_r2B_t rdc_r2B;
volatile rdc_r2C_t rdc_r2C;


void rdc_enable_vref(void);
void rdc_disable_vref(void);
void rdc_disable_pg_v1p2(void);
void rdc_enable_pg_v1p2(void);
void rdc_disable_pg_vbat(void);
void rdc_enable_pg_vbat(void);
void rdc_disable_clock(void);
void rdc_enable_clock(void);
uint32_t rdc_start_meas(void);
void rdc_reset(void);
void rdc_reset2(void);
void rdc_power_on(void);
void rdc_power_off(void);

void rdc_init(void);

#endif // CISV4B_H
