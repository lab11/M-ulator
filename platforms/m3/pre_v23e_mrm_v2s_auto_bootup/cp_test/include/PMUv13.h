//*******************************************************************************************
// PMU HEADER FILE FOR PMUV13 SYSTEM
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//#define DEVEL

#ifndef PMUV13_H
#define PMUV13_H
#define PMUV13

#include "mbus.h"
#include "PREv23E.h"
#include "PMUv13r1_RF.h"

//-------------------------------------------------------------------
// Options
//-------------------------------------------------------------------
#define __USE_HCODE__               // Use hard-coded values (sacrificing readability, reducing code size)

//-------------------------------------------------------------------
// PMU Short Prefix
//-------------------------------------------------------------------
volatile uint32_t __pmu_prefix__;

//*******************************************************************************************
// TARGET REGISTER INDEX FOR LAYER COMMUNICATIONS
//-------------------------------------------------------------------------------------------
// A register write message from a certain layer will be written in the following register in PRE.
// NOTE: Reg0x0-0x3 are retentive; Reg0x4-0x7 are non-retentive.
//*******************************************************************************************
#define PMU_TARGET_REG_IDX  0x7

#define PMU_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (PMU_TARGET_REG_IDX << 2)))

//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

#define __PMU_CHECK_WRITE__                 // If enabled, PMU replies with an actual read-out data from the analog block, rather than just forwarding the write data.

//-------------------------------------------------------------------
// PMU MODE DEFINITIONS
//-------------------------------------------------------------------
#define PMU_ACTIVE 0x1
#define PMU_SLEEP  0x0

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

#endif

//-------------------------------------------------------------------
// PMU ADC & SAR_RATIO CONFIGURATION
//-------------------------------------------------------------------
#define __PMU_ADC_PERIOD__          1   // Period of ADC VBAT measurement (Default: 500)
#define __PMU_TICK_ADC_RESET__      2   // Pulse Width of PMU's ADC_RESET signal (Default: 50)
#define __PMU_TICK_ADC_CLK__        2   // Pulse Width of PMU's ADC_CLK signal (Default: 2)

#define __PMU_ADC_UPPER_LIMIT__     164 // The ADC value is ignored if it is higher than __PMU_ADC_UPPER_LIMIT__
#define __PMU_ADC_LOWER_LIMIT__     64  // The ADC value is ignored if it is lower than __PMU_ADC_LOWER_LIMIT__

//-------------------------------------------------------------------
// PMU SAR MARGIN
//-------------------------------------------------------------------

volatile uint32_t __pmu_sar_ratio__;                // Current PMU SAR Ratio

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
// Function: pmu_set_floor
// Args    : mode - If 1, change the Active floor.
//                  If 0, change the Sleep floor.
//           r  - Multiplier R for all three converters
//           ul - Multiplier L (actually L+1) for UP Converter (V3P6)
//           sl - Multiplier L (actually L+1) for SAR Converter (V1P2)
//           dl - Multiplier L (actually L+1) for DN Converter (V0P6)
//           b  - Floor Frequency Base for all three converters
//                      If 0, it uses the floor settings for Normal Sleep.
// Description:
//           Set floor settings for Active or Sleep
// Return  : None
//-------------------------------------------------------------------
void pmu_set_floor(uint8_t mode, uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b);

//-------------------------------------------------------------------
// Function: pmu_active_floor
// Args    : r  - Multiplier R for all three converters
//           ul - Multiplier L (actually L+1) for UP Converter (V3P6)
//           sl - Multiplier L (actually L+1) for SAR Converter (V1P2)
//           dl - Multiplier L (actually L+1) for DN Converter (V0P6)
//           b  - Floor Frequency Base for all three converters
//                      If 0, it uses the floor settings for Normal Sleep.
// Description:
//           Set floor settings for Active
// Return  : None
//-------------------------------------------------------------------
void pmu_active_floor(uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b);

//-------------------------------------------------------------------
// Function: pmu_sleep_floor
// Args    : r  - Multiplier R for all three converters
//           ul - Multiplier L (actually L+1) for UP Converter (V3P6)
//           sl - Multiplier L (actually L+1) for SAR Converter (V1P2)
//           dl - Multiplier L (actually L+1) for DN Converter (V0P6)
//           b  - Floor Frequency Base for all three converters
//                      If 0, it uses the floor settings for Normal Sleep.
// Description:
//           Set floor settings for Sleep
// Return  : None
//-------------------------------------------------------------------
void pmu_sleep_floor(uint8_t r, uint8_t ul, uint8_t sl, uint8_t dl, uint8_t b);

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
// Function: pmu_init
// Args    : 
//      pmu_prefix  - PMU's Short Prefix
//      irq_reg_idx - PRE's Register ID to receive MBus IRQ messages coming from PMU.
//                      Valid Range: [0, 7]
//                  Example: If irq_reg_id = 4, PMU sends IRQ messages to PRE's
//                          Register 0x04.
// Description:
//           Initializes the PMU's floor settings & SAR ratio
//              for the nominal condition.
//           It also disables PMU's ADC operation for Active mode
//              and sets ADC timing parameters.
// Return  : None
//-------------------------------------------------------------------
void pmu_init(uint32_t pmu_prefix, uint32_t irq_reg_idx);

#endif // PMUV13_H
