//*******************************************************************************************
// ARETE SYSTEM HEADER FILE
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//-------------------------------------------------------------------------------------------
//      PREv23E -> ADOv1A -> MRMv1L -> PMUv13r1
//-------------------------------------------------------------------------------------------
// Short Address
//-------------------------------------------------------------------------------------------
//      PRE: 0x1
//      ADO: 0x2
//      MRM: 0x3
//      PMU: 0x9
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//#define DEVEL

#ifndef ARETE_H
#define ARETE_H
#define ARETE

#include "mbus.h"
#include "PREv23E.h"
#include "MRMv1L.h"
#include "ADOv1A_RF.h"
#include "MRMv1L_RF.h"
#include "PMUv13r1_RF.h"

//-------------------------------------------------------------------
// Options
//-------------------------------------------------------------------
#define __USE_HCODE__               // Use hard-coded values (sacrificing readability, reducing code size)

//-------------------------------------------------------------------
// Short Addresses
//-------------------------------------------------------------------
#define PRE_ADDR    0x1
#define ADO_ADDR    0x2
#define MRM_ADDR    0x3
#define PMU_ADDR    0x9

//*******************************************************************************************
// TARGET REGISTER INDEX FOR LAYER COMMUNICATIONS
//-------------------------------------------------------------------------------------------
// A register write message from a certain layer will be written in the following register in PRE.
// NOTE: Reg0x0-0x3 are retentive; Reg0x4-0x7 are non-retentive.
//*******************************************************************************************
#define PMU_TARGET_REG_IDX  0x0
#define MRM_TARGET_REG_IDX  0x1

#define PMU_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (PMU_TARGET_REG_IDX << 2)))
#define MRM_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (MRM_TARGET_REG_IDX << 2)))

//-------------------------------------------------------------------
// PRE-DEFINED FREQUENCIES / DELAYS
//-------------------------------------------------------------------

// PRE Clock Generator Frequency
#define CPU_CLK_FREQ    140000  // XT1F#1, SAR_RATIO=0x60 (Sleep Voltages: 4.55V/1.59V/0.79V)

// PRE Clock-based Delay (5 instructions @ CPU_CLK_FREQ)
#define DLY_1S      ((CPU_CLK_FREQ>>3)+(CPU_CLK_FREQ>>4)+(CPU_CLK_FREQ>>5))  // = CPU_CLK_FREQx(1/8 + 1/16 + 1/32) = CPU_CLK_FREQx(35/160) ~= (CPU_CLK_FREQ/5)
#define DLY_1MS     DLY_1S >> 10                                             // = DLY_1S / 1024 ~= DLY_1S / 1000

// Amount of delay needed to turn on ADC sub-blocks.
#define ADO_LDO_DELAY   1000
#define ADO_SAFR_DELAY  1000
#define ADO_ADC_DELAY   500
#define ADO_CP_DELAY    50000

// ADO Streaming Frequency in bps (Must be a multiple of 1024)
#define ADO_STREAM_BPS  65536

// MRM Num Pages for 1-second recording
// e.g., MRM_NUM_PAGES_PER_SEC = 64 pages when ADO_STREAM_BPS = 65536 bps
#define MRM_NUM_PAGES_PER_SEC   (ADO_STREAM_BPS>>10)

// MRM_LOG2_NUM_PAGES_PER_SEC = log2(MRM_NUM_PAGES_PER_SEC)
#define MRM_LOG2_NUM_PAGES_PER_SEC  6

// MRM Total Record Duration
// e.g., MRM_RECORD_DURATION = 1536 seconds (25.6 min) when ADO_STREAM_BPS = 65536 bps
#define MRM_RECORD_DURATION     (MRM_NUM_PAGES_MRAM>>MRM_LOG2_NUM_PAGES_PER_SEC)

//*******************************************************************
// ARETE FUNCTIONS
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
// Function: get_min
// Args    : a  - Value a
//           b  - Value b
// Description:
//          Return the smaller one between a and b
// Return  : a if a <= b
//           b if b <  a
//-------------------------------------------------------------------
uint32_t get_min (uint32_t a, uint32_t b);

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
void pmu_init(uint32_t irq_reg_idx);

//*******************************************************************
// ADO FUNCTIONS
//*******************************************************************

volatile ado_r00_t ado_r00;
volatile ado_r01_t ado_r01;
volatile ado_r02_t ado_r02;
volatile ado_r03_t ado_r03;
volatile ado_r04_t ado_r04;
volatile ado_r05_t ado_r05;
volatile ado_r06_t ado_r06;
volatile ado_r07_t ado_r07;
volatile ado_r08_t ado_r08;
volatile ado_r09_t ado_r09;
volatile ado_r0A_t ado_r0A;
volatile ado_r0B_t ado_r0B;
volatile ado_r0E_t ado_r0E;

//-------------------------------------------------------------------
// Function: ado_*
// Args    : 
// Description:
//           These functions are copied from Hung's previous
//              adov1a_firmware_v1
//          TODO: Add appropriate descriptions for each function.
// Return  : 
//-------------------------------------------------------------------
void ado_init(void);
void ado_ldo_set_mode(uint8_t mode);
void ado_safr_set_mode(uint8_t mode);
void ado_amp_set_mode(uint8_t mode);
void ado_adc_set_mode(uint8_t mode);
void ado_cp_set_mode(uint32_t mode);
void ado_set_rec_pga_gcon(uint32_t rec_pga_gcon);
void ado_set_div1(uint32_t div1);

//*******************************************************************
// MRM FUNCTIONS
//*******************************************************************

//-------------------------------------------------------------------
// Function: mrm_start_rec
// Args    : 
//      offset_sec  - Used to determined the start address of the MRAM for Ping-Pong.
//                      Start Page ID = offset_sec << MRM_LOG2_NUM_PAGES_PER_SEC
//                      Example:
//                          If the streaming speed is 65536 bps (ADO_STREAM_BPS = 65536),
//                          64 pages can store 1-second-long recording.
//                          In this case, 'offset_sec=n' results in 'Start Page ID = 64 x n'.
//      duration_sec - Record Duration in seconds.
//                      This is used to calculate the 'Number of Pages' to be recorded.
//                      Example:
//                          If the streaming speed is 65536 bps (ADO_STREAM_BPS = 65536),
//                          64 pages can store 1-second-long recording.
//                          In this case, 'duration_sec=n' results in 'Num Pages to be recorded = 64 x n'.
//      wait_for_irq - If 1, it waits for the IRQ message after sending the GO signal.
//                     If 0, it returns after sending the GO signal, without waiting for the IRQ message.
// Description:
//      It starts the ping-pong listening mode in MRM.
//      The ping-pong mode terminates once the designated recording length (duration_sec) has been recorded.
//
//      The received streaming gets written to the MRAM, starting from the 'Start Page ID', which is translated from 'offset_sec'
//      (See above description for 'offset_sec')
//
//      Once the ping-pong mode terminates, it sends the IRQ message back to PRC/PRE, if wait_for_irq=1.
//      It does NOT send the IRQ message, if wait_for_irq=0.
//
// Return  : 
//      None
//-------------------------------------------------------------------
void mrm_start_rec (uint32_t offset_sec, uint32_t duration_sec, uint16_t wait_for_irq);

//-------------------------------------------------------------------
// Function: mrm_read
// Args    : 
//      offset_sec  - Used to determined the start address of the MRAM for Read.
//                      Start Page ID = offset_sec << MRM_LOG2_NUM_PAGES_PER_SEC
//                      Example:
//                          If the streaming speed is 65536 bps (ADO_STREAM_BPS = 65536),
//                          64 pages can store 1-second-long recording.
//                          In this case, 'offset_sec=n' results in 'Start Page ID = 64 x n'.
//      duration_sec - Record Duration to be read in seconds.
//                      This is used to calculate the 'Number of Pages' to be read.
//                      Example:
//                          If the streaming speed is 65536 bps (ADO_STREAM_BPS = 65536),
//                          64 pages can store 1-second-long recording.
//                          In this case, 'duration_sec=n' results in 'Num Pages to be read = 64 x n'.
//      dst_prefix  - Destination Short Prefix
//                      The generated MBus message will have 'dst_prefix' in its MBus Address field.
// Description:
//      This function starts read the MRAM to generated an MBus message, 
//      starting from the 'Start Page ID' (which is translated from 'offset_sec'),
//      for the record duration set by 'duration_sec'. 
//      The total number of 'pages' to be read is calculated from 'duration_sec'.
//      (See above description for 'offset_sec' and 'duration_sec')
//
//      The generated MBus message will have 'dst_prefix' in its MBus Address field.
//
//      It generates multiple MBus messages, if the number of bits to be read is larger than 64kB.
//      In this case, each MBus message, except the last one, contains 64kB of data.
//
// Return  : 
//      None
//-------------------------------------------------------------------
void mrm_read (uint32_t offset_sec, uint32_t duration_sec, uint16_t dst_prefix);

#endif // ARETE_H

