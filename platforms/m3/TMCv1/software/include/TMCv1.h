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

// EEPROM Write Time (Tw): Must be >5ms; See datasheet
#define __NFC_TW__  400

// Write Polling
//      If enabled, TMC uses polling to check whether the write is completed,
//      rather than waiting for Tw x Num Pages (See 6.4.3 in the datasheet)
#define __NFC_DO_POLLING__

// Fast I2C
//      If enabled, TMC executes the low-level implementation of the I2C, 
//      which is much faster than the generic version.
//      This ignores __NFC_SCL__, __NFC_SDA__, and __I2C_*MASK__ directives.
//#define __USE_FAST_I2C__

// ACK Timeout
#define __I2C_ACK_TIMEOUT__         50000   // Checked using TIMER32
#define __FCODE_I2C_ACK_TIMEOUT__   6       // Failure code to be displayed if timeout during I2C ACK

// EEPROM Addresses
#define __NFC_FLAG_ADDR__   0x0000      // The first byte address in EEPROM where the flags are stored

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
//           data   : Memory address of the data array,
//                    where each element is a byte
//           len    : Number of bytes to be sent. Max: 256.
// Description:
//           Write data starting at the address 'addr'
//              addr        =   data[0]
//              addr+1      =   data[1]
//              addr+2      =   data[2]
//              ...
//              addr+len-1  =   data[len-1]
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_seq_byte_write(uint32_t e2, uint32_t addr, uint32_t data[], uint32_t len);

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
// Function: nfc_i2c_set_flag
// Args    : bit_idx: bit index into which the value is written (valid range: 0 - 31).
//           value  : flag value
// Description:
//           Set the given flag in the EEPROM
//           By default, XT1 reserves a word (4 bytes) in EEPROM, starting from __NFC_FLAG_ADDR__.
// Return  : None
//-------------------------------------------------------------------
void nfc_i2c_set_flag (uint32_t bit_idx, uint32_t value);

//-------------------------------------------------------------------
// Function: nfc_i2c_get_flag
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


#endif // TMCV1_H
