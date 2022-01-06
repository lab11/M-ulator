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
// Short Addresses
//-------------------------------------------------------------------
#define PRE_ADDR    0x1
#define SNT_ADDR    0x2
#define EID_ADDR    0x3
#define MRR_ADDR    0x4
#define MEM_ADDR    0x5
#define PMU_ADDR    0x6

//*******************************************************************************************
// TARGET REGISTER INDEX FOR LAYER COMMUNICATIONS
//-------------------------------------------------------------------------------------------
// A register write message from a certain layer will be written 
// in the following register in PRE.
// NOTE: Reg0x0-0x3 are retentive; Reg0x4-0x7 are non-retentive.
//*******************************************************************************************
#define PMU_TARGET_REG_IDX  0x0
#define SNT_TARGET_REG_IDX  0x1
#define EID_TARGET_REG_IDX  0x2
#define MRR_TARGET_REG_IDX  0x3

#define PMU_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (PMU_TARGET_REG_IDX << 2)))
#define SNT_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (SNT_TARGET_REG_IDX << 2)))
#define EID_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (EID_TARGET_REG_IDX << 2)))
#define MRR_TARGET_REG_ADDR  ((volatile uint32_t *) (0xA0000000 | (MRR_TARGET_REG_IDX << 2)))

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
// Function: set_bit
// Args    : original_var - Original Value
//           idx          - Bit Index
//           value        - New bit value
// Description:
//          set_bit() replaces [idx] in original_var with value
//          and returns the result
// Return  : original_var with [idx] replaced with value
//-------------------------------------------------------------------
uint32_t set_bit (uint32_t original_var, uint32_t idx, uint32_t value);

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
// Function: [DEPRECATED] set_pre_nfc_flag
// Args    : bit_idx: bit index into which the value is written (valid range: 0 - 31).
//           value  : flag value
// Description:
//           Set the given flag in the PRE as well as in the EEPROM
//           By default, XT1 reserves a word (4 bytes) in EEPROM, starting from __NFC_FLAG_ADDR__.
// Return  : None
//-------------------------------------------------------------------
void set_pre_nfc_flag (uint32_t bit_idx, uint32_t value);

//-------------------------------------------------------------------
// PRE Register File
//-------------------------------------------------------------------
volatile pre_r0B_t pre_r0B;

//*******************************************************************
// PMU FUNCTIONS
//*******************************************************************

#define __PMU_CHECK_WRITE__  // If enabled, PMU replies with an actual read-out data from the analog block, rather than just forwarding the write data.

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
#define __PMU_STALL_STATE_SLEEP_DEFAULT__    0x000000
#define __PMU_STALL_STATE_ACTIVE_DEFAULT__   0x000000

volatile union pmu_state __pmu_desired_state_sleep__;   // Register 0x3B
volatile union pmu_state __pmu_desired_state_active__;  // Register 0x3C
volatile union pmu_state __pmu_stall_state_sleep__;     // Register 0x39
volatile union pmu_state __pmu_stall_state_active__;    // Register 0x3A

//-------------------------------------------------------------------
// PMU Floor
//-------------------------------------------------------------------
// See TMCv1.c file for the look-up tables.
union pmu_floor {
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
// PMU ADC CONFIGURATION
//-------------------------------------------------------------------
#define __PMU_ADC_PERIOD__      1   // Period of ADC VBAT measurement (Default: 500)
#define __PMU_TICK_ADC_RESET__  2   // Pulse Width of PMU's ADC_RESET signal (Default: 50)
#define __PMU_TICK_ADC_CLK__    2   // Pulse Width of PMU's ADC_CLK signal (Default: 2)

volatile uint32_t __pmu_adc_3p0_val__;      // (Default: 0x62 set in pmu_init()) PMU ADC value at VBAT = 3.0V.
volatile uint32_t __pmu_adc_low_val__;      // (Default: 0x97 set in pmu_init()) Threshold for Low VBAT (used for display). The default value corresponds to VBAT = ~2.0V.
volatile uint32_t __pmu_adc_crit_val__;     // (Default: 0x9E set in pmu_init()) Threshold for Critical VBAT (used to trigger the EID crash detector & handler). The default value corresponds to VBAT = ~1.9V.

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
//          It also performs a time-out check.
// Return  : None
//-------------------------------------------------------------------
void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data);

//-------------------------------------------------------------------
// Function: pmu_reg_read
// Args    : reg_addr
// Description:
//          pmu_reg_read() read from reg_addr in PMU.
//          It also performs a time-out check.
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
// Function: pmu_set_sar_ratio
// Args    : ratio - desired SAR ratio
// Description:
//           Overrides SAR ratio with 'ratio', and then performs
//           sar_reset and sar/upc/dnc_stabilized tasks
// Return  : None
//-------------------------------------------------------------------
void pmu_set_sar_ratio (uint32_t ratio);

//-------------------------------------------------------------------
// Function: pmu_set_adc_period
// Args    : val - desired period for ADC VBAT measurement
// Description:
//           Sets ADC VBAT measurement to 'val'.
//           Also sets the pulse width of ADC_RESET and ADC_CLK.
// Return  : None
//-------------------------------------------------------------------
void pmu_set_adc_period(uint32_t val);

//-------------------------------------------------------------------
// Function: pmu_adc_read_and_sar_ratio_adjustment
// Args    : None
// Description:
//           Read the latest ADC VBAT measurement from PMU.
//           Then adjust SAR Ratio based on the VBAT measurement.
// Return  : PMU ADC VBAT raw reading
//-------------------------------------------------------------------
uint32_t pmu_adc_read_and_sar_ratio_adjustment();

//-------------------------------------------------------------------
// Function: pmu_set_adc_3p0_val
// Args    : val    - new value for __pmu_adc_3p0_val__
// Description:
//           Set __pmu_adc_3p0_val__, the expected PMU ADC value for VBAT = 3.0V
//           By default, pmu_init() sets __pmu_adc_3p0_val__ = 0x62.
// Return  : None
//-------------------------------------------------------------------
void pmu_set_adc_3p0_val (uint32_t val);

//-------------------------------------------------------------------
// Function: pmu_get_adc_3p0_val
// Args    : None
// Description:
//           Get the current __pmu_adc_3p0_val__, the expected PMU ADC value for VBAT = 3.0V
//           By default, pmu_init() sets __pmu_adc_3p0_val__ = 0x62.
// Return  : Current __pmu_adc_3p0_val__ 
//-------------------------------------------------------------------
uint32_t pmu_get_adc_3p0_val (void);

//-------------------------------------------------------------------
// Function: pmu_set_adc_low_val
// Args    : val    - new value for __pmu_adc_low_val__
// Description:
//           Set __pmu_adc_low_val__, the expected PMU ADC value for 'Low VBAT' threshold,
//           __pmu_adc_low_val__ is intended to be used for updating the display (low battery indicator)
//           By default, pmu_init() sets __pmu_adc_low_val__ = 0x97.
// Return  : None
//-------------------------------------------------------------------
void pmu_set_adc_low_val (uint32_t val);

//-------------------------------------------------------------------
// Function: pmu_get_adc_low_val
// Args    : None
// Description:
//           Get the current __pmu_adc_low_val__, the expected PMU ADC value for 'Low VBAT' threshold,
//           __pmu_adc_low_val__ is intended to be used for updating the display (low battery indicator)
//           By default, pmu_init() sets __pmu_adc_low_val__ = 0x97.
// Return  : Current __pmu_adc_low_val__ 
//-------------------------------------------------------------------
uint32_t pmu_get_adc_low_val (void);

//-------------------------------------------------------------------
// Function: pmu_set_adc_crit_val
// Args    : val    - new value for __pmu_adc_crit_val__
// Description:
//           Set __pmu_adc_crit_val__, the expected PMU ADC value for 'Critical VBAT' threshold,
//           __pmu_adc_crit_val__ is intended to be used to trigger the EID crash detector & handler.
//           By default, pmu_init() sets __pmu_adc_crit_val__ = 0x9E.
// Return  : None
//-------------------------------------------------------------------
void pmu_set_adc_crit_val (uint32_t val);

//-------------------------------------------------------------------
// Function: pmu_get_adc_crit_val
// Args    : None
// Description:
//           Get the current __pmu_adc_crit_val__, the expected PMU ADC value for 'Critical VBAT' threshold,
//           __pmu_adc_crit_val__ is intended to be used to trigger the EID crash detector & handler.
//           By default, pmu_init() sets __pmu_adc_crit_val__ = 0x9E.
// Return  : Current __pmu_adc_crit_val__ 
//-------------------------------------------------------------------
uint32_t pmu_get_adc_crit_val (void);

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
void pmu_init();


//*******************************************************************
// NFC FUNCTIONS
//*******************************************************************

// COTS Power Switch
#define __NFC_CPS__ 2

// GPIO Interface
#define __NFC_GPO__         0
#define __NFC_SCL__         1
#define __NFC_SDA__         2
#define __I2C_MASK__        (1<<__NFC_SDA__)|(1<<__NFC_SCL__)
#define __I2C_SCL_MASK__    (1<<__NFC_SCL__)
#define __I2C_SDA_MASK__    (1<<__NFC_SDA__)
#define __GPO_MASK__        (1<<__NFC_GPO__)

// NFC Boot-Up Delay (t_bootDC); Must be >0.6ms; See datasheet
#define __NFC_TBOOTDC__     100  // NOTE: Boot-Up Delay is approx. 0.3ms + (__NFC_TBOOTDC__ x 0.03ms)

// EEPROM Write Time (Tw); Must be >5ms; See datasheet
#define __NFC_TW__  400

// Write Polling
//      If enabled, TMC uses polling to check whether the write is completed,
//      rather than waiting for Tw x Num Pages (See 6.4.3 in the datasheet)
#define __NFC_DO_POLLING__

// Fast I2C
//      If enabled, TMC executes the low-level implementation of the I2C, 
//      which is much faster than the generic version.
//      This ignores __NFC_SCL__, __NFC_SDA__, and __I2C_*MASK__ directives.
// [WARNING] Fast I2C seems to cause more coupling with the XO wires (hence more frequent XO failure)
//#define __USE_FAST_I2C__

// ACK Timeout
#define __I2C_ACK_TIMEOUT__         50000   // Checked using TIMER32
#define __FCODE_I2C_ACK_TIMEOUT__   6       // Failure code to be displayed if timeout during I2C ACK

//// EEPROM Addresses [DEPRECATED]
//#define __NFC_FLAG_ADDR__   0x0000      // The first byte address in EEPROM where the flags are stored

// Status Flag
volatile uint32_t __nfc_on__;

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
//           This performs the timeout check while waiting for ACK.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_byte(uint8_t byte);

//-------------------------------------------------------------------
// Function: nfc_i2c_byte_imm
// Args    : byte   - Byte to be sent (8 bits)
// Description:
//           Send 'byte' from MSB to LSB, and return the ACK status.
//           This does NOT perform the timeout check.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
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
// Return  : none
//-------------------------------------------------------------------
void nfc_i2c_present_password(void);

//-------------------------------------------------------------------
// Function: nfc_i2c_update_password()
// Args    : upper  : Upper 32-bit of the new password
//           lower  : Lower 32-bit of the new password
// Description:
//           It updates the I2C password.
//           The I2C security session must be open before using nfc_i2c_update_password().
// Return  : none
//-------------------------------------------------------------------
void nfc_i2c_update_password(uint32_t upper, uint32_t lower);

//-------------------------------------------------------------------
// Function: nfc_i2c_rd
// Args    : ack    - 1: Ack
//                    0: No Ack
// Description:
//           Provide 8 SCL cycles and read SDA at every posedge.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
// Return  : 8-bit read data (uint8_t)
//-------------------------------------------------------------------
uint8_t nfc_i2c_rd(uint8_t ack);

//-------------------------------------------------------------------
// Function: nfc_i2c_byte_read
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
// Description:
//           Read data at the address 'addr'
// Return  : 8-bit read data
//-------------------------------------------------------------------
uint8_t nfc_i2c_byte_read(uint32_t e2, uint32_t addr);

//-------------------------------------------------------------------
// Function: nfc_i2c_word_read
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
// Description:
//           Read 4-bytes of data starting at the address 'addr'
// Return  : 32-bit read data
//           {data@(addr+3), data@(addr+2), data@(addr+1), data@(addr)}
//-------------------------------------------------------------------
uint32_t nfc_i2c_word_read(uint32_t e2, uint32_t addr);

//-------------------------------------------------------------------
// Function: nfc_i2c_byte_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
//           data   : 8-bit data
// Description:
//           Write data at the address 'addr'
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_byte_write(uint32_t e2, uint32_t addr, uint8_t data);

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_byte_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
//           data   : Memory address of the data array.
//           len    : Number of bytes to be sent. Max: 256.
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
//              addr+len-1  =   data[int((len-1)/4)][((len-1)%4)*8+7:((len-1)%4)*8]
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_seq_byte_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t len);

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_byte_read
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address
//           len    : Number of bytes to be read. Min: 1, Max: 4
// Description:
//           Read the specified number (len) of bytes starting at the address 'addr'
// Return  : If len=1: {        24'h0, data@(addr)}
//           If len=2: {        16'h0, data@(addr+1), data@(addr)}
//           If len=3: {         8'h0, data@(addr+2), data@(addr+1), data@(addr)}
//           If len=4: {data@(addr+3), data@(addr+2), data@(addr+1), data@(addr)}
//           If len=0 or len>4: 32'h0
//-------------------------------------------------------------------
void nfc_i2c_seq_byte_read(uint32_t e2, uint32_t addr, uint32_t len);

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_word_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address ( 2 LSBs are internally forced to 0 to make it word-aligned)
//           data   : Memory address of the data array,
//                    where each element is a word (4 bytes)
//           len    : Number of words (4-bytes) to be sent. Max: 64.
// Description:
//           Write data starting at the address 'addr'
//              addr        =   data[0][ 7: 0]
//              addr+1      =   data[0][15: 8]
//              addr+2      =   data[0][24:16]
//              addr+3      =   data[0][31:25]
//              addr+4      =   data[1][ 7: 0]
//              ...
//              addr+len-1  =   data[len-1][31:25]
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_seq_word_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t len);

//-------------------------------------------------------------------
// Function: nfc_i2c_word_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address ( 2 LSBs are internally forced to 0 to make it word-aligned)
//           data   : 32-bit data to be written
// Description:
//           Write data starting at the address 'addr'
//              addr        =   data[ 7: 0]
//              addr+1      =   data[15: 8]
//              addr+2      =   data[24:16]
//              addr+3      =   data[31:25]
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_word_write(uint32_t e2, uint32_t addr, uint32_t data);

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_word_pattern_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit byte address ( 2 LSBs are internally forced to 0 to make it word-aligned)
//           data   : 32-bit data to be written
//           len    : Number of words (4-bytes) to be sent. Max: 64.
// Description:
//           Write data starting at the address 'addr'
//              addr        =   data[ 7: 0]
//              addr+1      =   data[15: 8]
//              addr+2      =   data[24:16]
//              addr+3      =   data[31:25]
//              addr+4      =   data[ 7: 0]
//              ...
//              addr+len-1  =   data[31:25]
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_seq_word_pattern_write(uint32_t e2, uint32_t addr, uint32_t data, uint32_t len);

//-------------------------------------------------------------------
// Function: [DEPRECATED] nfc_i2c_reset_flag
// Args    : none
// Description:
//           Reset the flag in the EEPROM.
//           After the execution, the flag word becomes all 0.
//           By default, XT1 reserves a word (4 bytes) in EEPROM, starting from __NFC_FLAG_ADDR__.
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_reset_flag ();

//-------------------------------------------------------------------
// Function: [DEPRECATED] nfc_i2c_set_flag
// Args    : bit_idx: bit index into which the value is written (valid range: 0 - 31).
//           value  : flag value
// Description:
//           Set the given flag in the EEPROM
//           By default, XT1 reserves a word (4 bytes) in EEPROM, starting from __NFC_FLAG_ADDR__.
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_set_flag (uint32_t bit_idx, uint32_t value);

//-------------------------------------------------------------------
// Function: [DEPRECATED] nfc_i2c_get_flag
// Args    : bit_idx: bit index to be read (valid range: 0 - 31).
// Description:
//           Read the flag bit at bit_idx in the EEPROM.
//           By default, XT1 reserves a word (4 bytes) in EEPROM, starting from __NFC_FLAG_ADDR__.
// Return  : flag bit value at bit_idx.
//-------------------------------------------------------------------
uint8_t nfc_i2c_get_flag (uint32_t bit_idx);


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
// Use eid_get_current_display() to read this value.
//-------------------------------------------------------------------
volatile uint32_t __eid_current_display__;

//-------------------------------------------------------------------
// __eid_tmr_sel_ldo__
//-------------------------------------------------------------------
// 1: (default) EID Timer uses the regulated VBAT from LDO.
// 0: EID Timer uses V1P2 (i.e., the LDO is bypassed).
//-------------------------------------------------------------------
volatile uint32_t __eid_tmr_sel_ldo__;

//-------------------------------------------------------------------
// __eid_tmr_init_delay__
//-------------------------------------------------------------------
// Timer Start-up Delay
//      Must be >3 seconds delay. Delay between TMR_SELF_EN=1 and TMR_EN_OSC=0. 
//      LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
//-------------------------------------------------------------------
// NOTE: In silicon, most chips are fine with __eid_tmr_init_delay__=60000. 
//       However, in some cases (e.g., XT1B#3), it must be 120000 to make a 4-second delay.
//-------------------------------------------------------------------
// Default: 120000
//-------------------------------------------------------------------
volatile uint32_t __eid_tmr_init_delay__;

//-------------------------------------------------------------------
// __eid_cp_pulse_width__
//-------------------------------------------------------------------
// Charge Pump Activation Duration
//-------------------------------------------------------------------
// NOTE: In silicon, the FSM clock speed (=TMR clock frequency / 16) 
//       is 120~125Hz at RT, which means, LSB corresponds to ~8ms.
//-------------------------------------------------------------------
// Default: 250; Max: 65535
//-------------------------------------------------------------------
volatile uint32_t __eid_cp_pulse_width__;

//-------------------------------------------------------------------
// __eid_clear_fd__
//-------------------------------------------------------------------
// 1: (default) Makes FD white after writing SEG(s).
// 0: No additional operation after writing SEG(s).
//-------------------------------------------------------------------
volatile uint32_t __eid_clear_fd__;

//-------------------------------------------------------------------
// __eid_vin__
//-------------------------------------------------------------------
// Use eid_set_vin() function to change this.
//-------------------------------------------------------------------
// 1: Use VSS  as the charge pump input (recommended when VBAT is high) 
//       [WARNING: VIN=1 may affect the EID timer when used with TMR_SEL_LDO=1]
// 0: (default) Use VBAT as the charge pump input (recommended when VBAT is low)
//-------------------------------------------------------------------
volatile uint32_t __eid_vin__;

//--------------------------------------------------------
// EID Crash Detector & Handler
//--------------------------------------------------------
//
// X: __crsh_cp_idle_width__
// Y: __crsh_cp_pulse_width__
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

// CP Clock setting for Crash handling
//-----------------------------------------------
volatile uint32_t __crsh_cp_clk_sel_ring__;     // (Default: 0x0; Max: 0x3) Selects # of rings. 0x0: 11 stages; 0x3: 5 stages.
volatile uint32_t __crsh_cp_clk_sel_te_div__;   // (Default: 0x0; Max: 0x3) Selects clock division ratio for Top Electrode (TE) charge pump.
volatile uint32_t __crsh_cp_clk_sel_fd_div__;   // (Default: 0x0; Max: 0x3) Selects clock division ratio for Field (FD) charge pump.
volatile uint32_t __crsh_cp_clk_sel_seg_div__;  // (Default: 0x0; Max: 0x3) Selects clock division ratio for Segment (SEG) charge pumps.

// Watchdog Threshold for Crash Detection
//-----------------------------------------------
// NOTE: In silicon, the FSM clock speed (=TMR clock frequency / 16) is 120~125Hz at RT, which means, LSB corresponds to ~8ms.
volatile uint32_t __crsh_wd_thrshld__;          // (Default: 360,000, Max:16,777,215) Crash Watchdog Threshold.

// Charge Pump Activation/Idle Duration (for Crash handling)
//-----------------------------------------------
// NOTE: In silicon, the FSM clock speed (=TMR clock frequency / 16) is 120~125Hz at RT, which means, LSB corresponds to ~8ms.
volatile uint32_t __crsh_cp_idle_width__;   // (Default: 400; Max: 65535) Duration of Charge Pump Activation.
volatile uint32_t __crsh_cp_pulse_width__;  // (Default: 400; Max: 65535) Duration of Charge Pump Activation.
volatile uint32_t __crsh_sel_seq__;         // (Default: 0x7) See above waveform. Each bit in [2:0] enables/disables Seq C, Seq B, Seq A, respectively. 
                                            //      If 1, the corresponding sequence is enabled. 
                                            //      If 0, the corresponding sequence is skipped.

//--- Sequence A (valid only if __crsh_sel_seq__[0]=1)
//-----------------------------------------------
volatile uint32_t __crsh_seqa_vin__;        // (Default: 0x0)   Use 0 if VBAT is low; Use 1 if VBAT is high.
volatile uint32_t __crsh_seqa_ck_te__;      // (Default: 0x0)   CK Value for Top Electrode (TE)
volatile uint32_t __crsh_seqa_ck_fd__;      // (Default: 0x1)   CK Value for Field (FD)
volatile uint32_t __crsh_seqa_ck_seg__;     // (Default: 0x1FF) CK Value for Segments (SEG[8:0])

//--- Sequence B (valid only if __crsh_sel_seq__[1]=1)
//-----------------------------------------------
volatile uint32_t __crsh_seqb_vin__;        // (Default: 0x0)   Use 0 if VBAT is low; Use 1 if VBAT is high.
volatile uint32_t __crsh_seqb_ck_te__;      // (Default: 0x1)   CK Value for Top Electrode (TE)
volatile uint32_t __crsh_seqb_ck_fd__;      // (Default: 0x0)   CK Value for Field (FD)
volatile uint32_t __crsh_seqb_ck_seg__;     // (Default: 0x000) CK Value for Segments (SEG[8:0])

//--- Sequence C (valid only if __crsh_sel_seq__[2]=1)
//-----------------------------------------------
volatile uint32_t __crsh_seqc_vin__;        // (Default: 0x0)   Use 0 if VBAT is low; Use 1 if VBAT is high.
volatile uint32_t __crsh_seqc_ck_te__;      // (Default: 0x0)   CK Value for Top Electrode (TE)
volatile uint32_t __crsh_seqc_ck_fd__;      // (Default: 0x0)   CK Value for Field (FD)
volatile uint32_t __crsh_seqc_ck_seg__;     // (Default: 0x018) CK Value for Segments (SEG[8:0])

//-------------------------------------------------------------------
// EID Register File
//-------------------------------------------------------------------
volatile eid_r00_t eid_r00;
volatile eid_r01_t eid_r01;
volatile eid_r02_t eid_r02;
volatile eid_r07_t eid_r07;
volatile eid_r09_t eid_r09;
volatile eid_r10_t eid_r10;
volatile eid_r11_t eid_r11;
volatile eid_r12_t eid_r12;
volatile eid_r13_t eid_r13;
volatile eid_r15_t eid_r15;
volatile eid_r16_t eid_r16;
volatile eid_r17_t eid_r17;

//-------------------------------------------------------------------
// Function: eid_init
// Args    : None
// Description:
//           Initializes the EID layer
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
// Function: eid_set_pulse_width
// Args    : pulse_width - Duration of Charge Pump Activation 
//                          (Default: 400; Max: 65535)
// Description:
//           Set the duration of the charge pump activation 
// Return  : None
//-------------------------------------------------------------------
void eid_set_pulse_width(uint32_t pulse_width);

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
// Function: eid_update
// Args    : seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] black.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the entire display white,
//           and then make the selected segment(s) black.
// Return  : None
//-------------------------------------------------------------------
void eid_update(uint32_t seg);

//-------------------------------------------------------------------
// Function: eid_get_current_display
// Args    : none
// Description:
//           Get the current display value.
//           [WARNING] The result may be invalid if eid_enable_cp_ck()
//                     has been never used.
// Return  : The current display value.
//              For n=0,1,...,8:
//              bit[n]=1 indicates that SEG[n] is black.
//              bit[n]=0 indicates that SEG[n] is white.
//-------------------------------------------------------------------
uint32_t eid_get_current_display(void);

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
// __snt_timer_status__
//-------------------------------------------------------------------
// 0: (default) SNT timer is not running
// 1: SNT timer is fully running
// 2: SNT timer has finishes the start-uhas finishes the start-up (i.e., snt_start_timer_presleep())
//-------------------------------------------------------------------
volatile uint32_t __snt_timer_status__;

//-------------------------------------------------------------------
// SNT Register File
//-------------------------------------------------------------------
volatile snt_r00_t snt_r00;
volatile snt_r01_t snt_r01;
volatile snt_r03_t snt_r03;
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
// Args    : None
// Description:
//           Power on SNT Temp Sensor
// Return  : None
//-------------------------------------------------------------------
void snt_temp_sensor_power_on(void);

//-------------------------------------------------------------------
// Function: snt_temp_sensor_power_off
// Args    : None
// Description:
//           Turns off SNT Temp Sensor
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
// Function: snt_temp_sensor_reset
// Args    : None
// Description:
//           Assert the reset of SNT Temp Sensor
// Return  : None
//-------------------------------------------------------------------
void snt_temp_sensor_reset(void);

//-------------------------------------------------------------------
// Function: snt_ldo_power_off
// Args    : None
// Description:
//           Turns off SNT LDO
// Return  : None
//-------------------------------------------------------------------
void snt_ldo_power_off(void);

//-------------------------------------------------------------------
// Function: snt_ldo_vref_on
// Args    : None
// Description:
//           Turns on SNT VREF
// Return  : None
//-------------------------------------------------------------------
void snt_ldo_vref_on(void);

//-------------------------------------------------------------------
// Function: snt_ldo_power_on
// Args    : None
// Description:
//           Turns on SNT LDO
// Return  : None
//-------------------------------------------------------------------
void snt_ldo_power_on(void);

//-------------------------------------------------------------------
// Function: set_snt_timer_status
// Args    : status - the new status to be written to __snt_timer_status__
// Description:
//           Set the SNT timer status variable (__snt_timer_status__)
// Return  : None
//-------------------------------------------------------------------
void set_snt_timer_status(uint32_t status);

//-------------------------------------------------------------------
// Function: get_snt_timer_status
// Args    : None
// Description:
//           Return the value of the SNT timer status varible (__snt_timer_status__)
// Return  : The value of the SNT timer status variable (__snt_timer_status__)
//              0: (default) SNT timer is not running
//              1: SNT timer is running
//-------------------------------------------------------------------
uint32_t get_snt_timer_status(void);

//-------------------------------------------------------------------
// Function: snt_start_timer
// Args    : None
// Description:
//           Start the SNT timer.
//           It includes a brief sleep duration (2~3 seconds)
//           Thus, it must be placed before any other 'sleep' command,
//           and needs to be reached after the wakeup following the brief sleep duration.
// Return  : None
//-------------------------------------------------------------------
void snt_start_timer(void);

//-------------------------------------------------------------------
// Function: snt_stop_timer
// Args    : None
// Description:
//           Stop the SNT timer
// Return  : None
//-------------------------------------------------------------------
void snt_stop_timer(void);

//-------------------------------------------------------------------
// Function: snt_set_timer_threshold
// Args    : threshold  - SNT Wakeup timer threshold (32-bit)
// Description:
//           Set the threshold for the SNT Wakeup timer
// Return  : None
//-------------------------------------------------------------------
void snt_set_timer_threshold(uint32_t threshold);

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
//           Set the threshold for the SNT wakeup timer,
//           then enables the SNT wakeup timer so that the wakeup counter starts running.
//           wakeup counter stops running.
//           If auto_reset=1, the Auto Reset feature is also enabled,
//           where the SNT wakeup counter gets automatically reset upon the system
//           going into sleep.
// Return  : None
//-------------------------------------------------------------------
void snt_set_wup_timer(uint32_t auto_reset, uint32_t threshold);

//-------------------------------------------------------------------
// Function: snt_read_wup_timer
// Args    : None
// Description:
//           Synchronous read the SNT wakeup timer counter value.
//           The upper 8-bit is written to REG6 (in PRE)
//           and the lower 24-bit is written to REG7 (in PRE).
//           Then it combines REG6 and REG7 and returns the 32-bit value.
// Return  : The current 32-bit SNT Wakeup counter value read synchronously
//-------------------------------------------------------------------
uint32_t snt_read_wup_timer(void);

#endif // TMCV1_H
