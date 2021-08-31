//*******************************************************************************************
// TMC HEADER FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//      PREv22E -> SNTv5 -> EIDv1 -> MRRv11A -> MEMv3 -> PMUv13
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jun 22 2021 -   First commit 
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

#ifndef TMCV1_H
#define TMCV1_H
#define TMCV1

#include "mbus.h"
#include "PREv22E.h"
#include "SNTv5_RF.h"
#include "EIDv1_RF.h"
#include "MRRv11A_RF.h"
#include "MEMv3_RF.h"
#include "PMUv13_RF.h"

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
union pmu_state {
    struct {
        unsigned horizon                    : 1;    // [20]
        unsigned vbat_read_only             : 1;    // [19]
        unsigned vdd_0p6_turned_on          : 1;    // [18]
        unsigned vdd_1p2_turned_on          : 1;    // [17]
        unsigned vdd_3p6_turned_on          : 1;    // [16]
        unsigned dnc_stabilized             : 1;    // [15]
        unsigned dnc_on                     : 1;    // [14]
        unsigned sar_ratio_adjusted         : 1;    // [13]
        unsigned adc_adjusted               : 1;    // [12]
        unsigned adc_output_ready           : 1;    // [11]
        unsigned refgen_on                  : 1;    // [10]
        unsigned upc_stabilized             : 1;    // [9]
        unsigned upc_on                     : 1;    // [8]
        unsigned control_supply_switched    : 1;    // [7]
        unsigned clock_supply_switched      : 1;    // [6]
        unsigned sar_ratio_roughly_adjusted : 1;    // [5]
        unsigned sar_stabilized             : 1;    // [4]
        unsigned sar_reset                  : 1;    // [3]
        unsigned wait_for_cap_charge        : 1;    // [2]
        unsigned wait_for_clock_cycles      : 1;    // [1]
        unsigned sar_on                     : 1;    // [0]
    };
    uint32_t value;
};

#define PMU_DESIRED_STATE_SLEEP_DEFAULT  0x17FFFF
#define PMU_DESIRED_STATE_ACTIVE_DEFAULT 0x07CFFF
#define PMU_STALL_STATE_SLEEP_DEFAULT    0x000000
#define PMU_STALL_STATE_ACTIVE_DEFAULT   0x000000

//-------------------------------------------------------------------
// PMU Floor
//-------------------------------------------------------------------
union pmu_floor {
    struct {
        unsigned R      : 4;
        unsigned L      : 4;
        unsigned BASE   : 5;
        unsigned L_SAR  : 4;
    };
    uint32_t value;
};

//*******************************************************************
// TMCv1 FUNCTIONS
//*******************************************************************


//-------------------------------------------------------------------
// Basic Building Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: set_bits
// Args    : original_var - Original Value
//           msb_idx      - Index of MSB
//           lsb_idx      - Index of LSB
//           value        - New value
// Description:
//          set_bits() replaces [msb_idx:lsb_idx] in original_var with value
//          and returns the result
// Return  : original_var with [msb_idx:lsb_idx] replaced with value
//-------------------------------------------------------------------
uint32_t set_bits (uint32_t original_var, uint32_t msb_idx, uint32_t lsb_idx, uint32_t value);

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


#endif // TMCV1_H
