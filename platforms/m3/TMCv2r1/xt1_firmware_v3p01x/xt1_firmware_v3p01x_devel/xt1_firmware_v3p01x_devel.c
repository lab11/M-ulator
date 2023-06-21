//*******************************************************************************************
// XT1 (TMCv2r1) FIRMWARE
// Version 3.01x (devel)
//------------------------
#define HARDWARE_ID 0x01005843  // XT1r1 Hardware ID
#define FIRMWARE_ID 0x0301      // [15:8] Integer part, [7:0]: Non-Integer part
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jan 05 2023 - Version 3.00
//                  - Hard-forked & modified from xt1_firmware_v2p10
//  See Google Doc for detailed update history.
//-------------------------------------------------------------------------------------------
//
// External Connections
//
//  XT1r1       NFC (ST25DVxxx)     XT1     e-Ink Display           Display    
//  ---------------------------     ------------------------                       
//  GPIO[0]     GPO                 SEG[0]  Play                       [==]                 .
//  GPIO[1]     SCL                 SEG[1]  Tick                                            .
//  GPIO[2]     Not Used            SEG[2]  Low Battery               \\  //                .
//  GPIO[3]     SDA                 SEG[3]  Back Slash          |\     \\//    +            .
//  VPG2_OUT    VCC                 SEG[4]  Slash               |/     //\\    -            .
//                                  SEG[5]  Plus                    \\//  \\                .
//                                  SEG[6]  Minus                                
//
//-------------------------------------------------------------------------------------------
// SNT Temperature Sensor
//-------------------------------------------------------------------------------------------
//
//  Temperature Range: -40C ~ +65C (FP Specification)
//
//-------------------------------------------------------------------------------------------
// Use of Cortex-M0 Vector Table
//  ----------------------------------------------------------
//   MEM_ADDR  M0-Usage       PRE-Usage       PRE-Usage       
//                            (GPIO)          (AES)           
//  ----------------------------------------------------------
//    0 (0x00) STACK_TOP      STACK_TOP       STACK_TOP       
//    1 (0x04) RESET_VECTOR   RESET_VECTOR    RESET_VECTOR    
//    2 (0x08) NMI            GOC_DATA_IRQ    GOC_DATA_IRQ    
//    3 (0x0C) HardFault                      GOC_AES_PASS    
//    4 (0x10) RESERVED                       GOC_AES_CT[0]   
//    5 (0x14) RESERVED                       GOC_AES_CT[1]   
//    6 (0x18) RESERVED                       GOC_AES_CT[2]   
//    7 (0x1C) RESERVED                       GOC_AES_CT[3]   
//    8 (0x20) RESERVED                       GOC_AES_PT[0]   
//    9 (0x24) RESERVED                       GOC_AES_PT[1]   
//   10 (0x28) RESERVED                       GOC_AES_PT[2]   
//   11 (0x2C) SVCall                         GOC_AES_PT[3]   
//   12 (0x30) RESERVED                       GOC_AES_KEY[0]  
//   13 (0x34) RESERVED       R_GPIO_DATA     GOC_AES_KEY[1]  
//   14 (0x38) PendSV         R_GPIO_DIR      GOC_AES_KEY[2]  
//   15 (0x3C) SysTick        R_GPIO_IRQ_MASK GOC_AES_KEY[3]  
//
//-------------------------------------------------------------------------------------------
// WAKEUP_SOURCE (wakeup_source) definition
//-------------------------------------------------------------------------------------------
//
//  Use get_bit(wakeup_source, n) to get the current value, where n is 0-5, 8-11 as shown below.
//
//  [31:12] - Reserved
//     [11] - GPIO_PAD[3] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [10] - GPIO_PAD[2] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [ 9] - GPIO_PAD[1] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [ 8] - GPIO_PAD[0] has triggered wakeup (valid only when wakeup_source[3]=1)
//      [7] - Reserved (NOTE: wakeup_source[7] is used to indicate that the previous 'sleep' has been skipped)
//      [6] - Reserved
//      [5] - GIT (GOC Instant Trigger) has triggered wakeup
//              NOTE: If GIT is triggered while the system is in active, the GIT is NOT immediately handled.
//                    Instead, it waits until the system goes in sleep, and then, the (pending) GIT will wake up the system.
//                    Thus, you can safely assume that GIT is (effectively) triggered only while the system is in Sleep.
//      [4] - MBus message has triggered wakeup (e.g., Flash Auto Boot-up)
//      [3] - One of GPIO_PAD[3:0] has triggered wakeup
//      [2] - XO Timer has triggered wakeup
//      [1] - Wake-up Timer has triggered wakeup
//      [0] - GOC/EP has triggered wakeup
//
//-------------------------------------------------------------------------------------------
// DEBUG Messages
//-------------------------------------------------------------------------------------------
// Valid only when DEVEL is enabled
//
//  ADDR    DATA                                Description
//  -----------------------------------------------------------------------------------------
//
//  -----------------------------------------------------------------------------------------
//  < FSM >
//  -----------------------------------------------------------------------------------------
//  0x70    wakeup_source                       Wakeup Source (See above WAKEUP_SOURCE definition)
//                                                  0x0001: GOC/EP
//                                                  0x0002: PRE Wakeup Timer
//                                                  0x0010: SNT Timer (MBus)
//                                                  0x0108: NFC (GPIO[0])
//  0x71    xt1_state                           XT1 State (See set_system() below)
//  0x72    target                              set_system() is called for target:
//                                                  0x0: XT1_RESET       
//                                                  0x1: XT1_IDLE        
//                                                  0x2: XT1_PEND        
//                                                  0x3: XT1_ACTIVE      
//                                                  0x9: XT1_CRASH       
//  0x73    value                               Function Calls
//                                                  0x0: reset_system_status() is called
//                                                  0x1: update_system_configs() is called and uses the default values
//                                                  0x2: update_system_configs() is called and uses the values read from EEPROM
//                                                  0x3: update_user_configs() is called
//                                                  0x5: update_aes_key() is called
//                                                  0xF: debug_system_configs() is called
//  0x74    value                               Temperature Thresholds (given by user)
//                                                  [31:16] Low Threshold
//                                                  [15: 0] High Threshold
//  0x75    value                               Measurement Configuration
//                                                  [31:16] Start Delay (unit: minutes)
//                                                  [15: 0] Measurement Interval (unit: minutes)
//  0x76    value                               Temperature Calibration Coefficient A gets updated to 'value'
//  0x77    value                               Temperature Calibration Coefficient B gets updated to 'value'
//  0x78    value                               aes_key[0] is set to 'value'
//  0x79    value                               aes_key[1] is set to 'value'
//  0x7A    value                               aes_key[2] is set to 'value'
//  0x7B    value                               aes_key[3] is set to 'value'
//  0x7C    value                               Measurement Configuration
//                                                  [15: 0] num_cons_excursions
//
//  -----------------------------------------------------------------------------------------
//  < XO and SNT Timer - calibrate_snt_timer() >
//  -----------------------------------------------------------------------------------------
//  0x80    xo_val_prev                         Previous session's XO value (xo_val_prev)
//  0x81    xo_val_curr                         Current session's XO value (xo_val_curr)
//  0x82    temp_xo_val                         Latest XO value (temp_xo_val)
//  0x83    value                               Calibration Status
//                                                  0x0: XO has failed before. Will not perform the calibration in this session.
//                                                  0x1: XO was unstable while reading temp_xo_val
//                                                  0x2: XO counter has not incremented (i.e., temp_xo_val <= xo_val_curr)
//                                                  0x3: 'skip_calib' or 'MAIN_CALLED' or 'FLAG_INVLD_XO_PREV' is 1. Will use the previous snt_freq as it is.
//                                                  0x4: Beginning the calibration
//                                                  0x5: Calculated a is not close to 1 (allowed error: 25%)
//                                                  0x6: Calibration has failed. Will restart the XO.
//                                                  0x7: Calibration is done and successful
//                                                  0x8: Skipping the snt_threshold calculation (usually due to a new START/STOP/RESET command)
//                                                  0x9: Calculated snt_threshold was 0, so it is forced to 1.
//                                                  0xA: main() has been directly called, i.e., Sleep has been bypassed before the current active session. 
//                                                          This implies that xo_val_curr is inaccurate.
//                                                          It uses the previous snt_freq to calculate the next snt_threshold.
//                                                  0xB: FLAG_INVLD_XO_PREV is 1. i.e., XO was restarted during the previous wakeup -OR- Sleep has been bypassed before the previous active session.
//                                                          This implies that xo_val_prev is inaccurate.
//                                                          It uses the previous snt_freq to calculate the next snt_threshold.
//  0x84    snt_duration                        SNT counter value that corresponds to the given interval (previous sleep duration)
//  0x85    XO_FREQ                             XO frequency
//  0x86    snt_freq                            SNT frequency that is used for the previous sleep.
//  0x87    delta_xo_val                        xo_val_curr - xo_val_prev
//  0x88    a                                   Calculated a (Custom fixed-point, N=30)
//  0x89    temp_snt_freq                       Newly Calculated SNT frequency
//  0x8A    wakeup_interval_sec                 New wakeup interval (in seconds)
//  0x8B    snt_duration                        Newly calculated snt_duration
//  0x8C    snt_threshold                       Newly calculated snt_threshold
//  0x8D    snt_threshold_prev                  Previous snt_threshold
//  0x8E    snt_threshold                       snt_threshold
//  0x8F    snt_threshold                       Going into sleep with snt_threshold
//  
//  -----------------------------------------------------------------------------------------
//  < Temperature Measurement >
//  -----------------------------------------------------------------------------------------
//  0x90    value                               Measured Temperature
//                                                  [31:16] Raw code (*SNT_TARGET_REG_ADDR)
//                                                  [15: 0] After the conversion (temp.val) "10 x (T + 80)" where T is the actual temperature in celsius degree.
//  0x91    0x00000000                          meas_temp_adc() is called with go_sleep=0
//  0x91    0x00000001                          meas_temp_adc() is called with go_sleep=1
//  0x91    0x00000002                          meas_temp_adc() is called with go_sleep=1 & FLAG_TEMP_MEAS_PEND=0
//  0x91    0x00000003                          meas_temp_adc() is called with go_sleep=1 & FLAG_TEMP_MEAS_PEND=1
//  0x91    0xFFFFFFFF                          Skipped the temperature measurement since meas.valid=1
//  0x92    {0x1, sub_sample_cnt}               Sub-sample count
//  0x92    {0x2, sub_sample_sum}               Current sum of the sub-samples
//  0x92    {0x3, temp_sample_cnt}              Temp sample count (NOTE: Temp Sample ID = temp_sample_cnt - 1)
//  0x93    value                               VBAT and SAR Ratio Information
//                                                  [   28] Critical VBAT (crit_vbat)
//                                                  [   24] Low VBAT (low_vbat)
//                                                  [23:16] ADC Offset (adc_offset)
//                                                  [15: 8] SAR Ratio (meas.sar)
//                                                  [ 7: 0] ADC Reading (meas.adc)
//  0x94    value                               PMU SAR Ratio is changing
//                                                  [15: 8] Previous SAR Ratio
//                                                  [ 7: 0] New SAR Ratio
//  0x95    value                               Temperature Data read from the write buffer
//                                                  [15: 0] Temperature Data read from the write buffer (temp.val)
//  0x96    comp_sample_id                      Compression Sample ID
//  0x97    value                               (Reversed) Raw/Encoded data to be stored
//                                                  [31:24] Number of bits
//                                                  [23: 0] Reversed code
//  0x9A    value                               [DEPRECATED] Measured Temperature (from snt_get_temp_raw())
//                                                  [15: 0] Raw code (*SNT_TARGET_REG_ADDR)
//  0x9B    value                               [DEPRECATED] Measured Temperature (from snt_get_temp())
//                                                  [15: 0] After the conversion (temp.val) "10 x (T + 80)" where T is the actual temperature in celsius degree.
//  0x9C    value                               Averaged sample value
//  0x9D    {0x00, tmp_exc.num_cons_hi}         temp.val exceeds the high threshold. Alarm not yet triggered.
//  0x9D    {0x0F, tmp_exc.num_cons_hi}         temp.val exceeds the high threshold. A new alarm has been triggered.
//  0x9D    {0x10, tmp_exc.num_cons_lo}         temp.val exceeds the low threshold. Alarm not yet triggered.
//  0x9D    {0x1F, tmp_exc.num_cons_lo}         temp.val exceeds the low threshold. A new alarm has been triggered.
//  0x9E    value                               Current alarm status
//                                                  0xFFFFFFFF: High Alarm && Low Alarm
//                                                  0xFFFF0000: High Alarm Only
//                                                  0x0000FFFF: Low Alarm && Low Alarm
//                                                  0x00000000: No Alarm
//
//  <--- Below are valid only when [Raw Sample] is enabled --->
//
//  0x9C    byte_addr                           [Raw Sample] Byte Address
//  0x9D    temp.val                            [Raw Sample] Raw Temperature Data
//  0x9E    sar_adc_addr                        [Raw Sample] Byte Address to store SAR/ADC (or XO FAIL) value
//  0x9F    sar_adc_data                        [Raw Sample] SAR/ADC (or XO FAIL) data
//
//  -----------------------------------------------------------------------------------------
//  < Display >
//  -----------------------------------------------------------------------------------------
//  0xA0    {0x0, pattern}                      Display has changed to 'pattern' (EEPROM Updated)
//  0xA0    {0x1, pattern}                      Display has been refreshed with 'pattern' (EEPROM Not Updated)
//  0xA0    {0x2, pattern}                      Display has changed to 'pattern' but failed to update the EEPROM.
//  0xA0    {0xF, pattern}                      Display has blinked.
//  0xA1    disp_min_since_refresh              Elapsed time (in minutes) since the last E-ink update
//  0xA2    value                               EID duration and Refresh Interval are set
//                                                  [31:16] EID Duration
//                                                  [15: 0] Refresh Interval (minutes)
//  0xA3    0x00000000                          Critical Temperature. Display did not update, although XT1 behaves like it did.
//  0xA4    0x00000001                          [DEPRECATED] eid_update_display() requires a new temperature measurement
//  0xA5    eid_updated_at_low_temp             1 indicates that the display has been updated at a low temperature
//  0xA6    eid_updated_at_crit_temp            1 indicates that the display has a pending update due to a critical temperature
//  0xA7    eid_update_type                     Update type used for the display update
//                                                  0x0: Global Update
//                                                  0x1: Local Update
//  0xA8    0x00000000                          display_low_batt() is called.
//  0xA8    0xFFFFFFFF                          Does not have to display LOW_BATT since it is already on.
//
//  -----------------------------------------------------------------------------------------
//  < NFC and EEPROM >
//  -----------------------------------------------------------------------------------------
//  0xB0    target                              The returned 'target' value from nfc_check_cmd()
//  0xB1    value                               NFC Command Info
//                                                  [23:16]: Command Parameter (cmd_param)
//                                                  [ 4: 0]: Command
//                                                              0x00: NOP         
//                                                              0x01: START       
//                                                              0x02: STOP        
//                                                              0x03: GETSEC        
//                                                              0x04: CONFIG
//                                                              0x07: HRESET      
//                                                              0x08: NEWKEY      
//                                                              0x09: NEWAB
//                                                              0x10: DISPLAY     
//                                                              0x1E: SRESET       
//                                                              0x1F: DEBUG       
//  0xB2    value                               ACK with Data Successful and 'value' is written in EEPROM_ADDR_CMD
//  0xB3    value                               ACK with Data Fail; 'value' was supposed to be written in EEPROM_ADDR_CMD
//
//  --- buffer_update()
//  0xB4    data                                Unencrypted data (qword)
//  0xB5    addr                                buffer_eeprom_addr
//  0xB6    data                                Encrypted data
//  0xB7    CRC                                 buffer_crc32
//
//  --- buffer_commit()
//  0xB8    value                               buffer_commit() result
//                                                  0x1: Successfully committed the buffer
//                                                  0x0: There was no data in the buffer
//
//  --- status
//  0xBA    {0x00, curr_state       }           status.curr_state         (committed)
//          {0x02, config_set       }           status.config_set         (committed)
//          {0x03, memory_full      }           status.memory_full        (committed)
//          {0x04, sample_type      }           status.sample_type        (committed)
//          {0x05, activated        }           status.activated          (committed)
//          {0x06, unread_sample    }           status.unread_sample      (committed)
//          {0x07, crash_triggered  }           status.crash_triggered    (committed)
//          {0x08, fail_id          }           status.fail_id            (committed)
//          {0x10, aes_key_set      }           status.aes_key_set        (updated)
//          {0x11, disp_skipped     }           status.disp_skipped       (updated)
//          {0x12, custom_init      }           status.custom_init        (updated)
//          {0x13, skip_init        }           status.skip_init          (updated)
//
//  -----------------------------------------------------------------------------------------
//  < Math Functions > -> Need to enable DEVEL
//  -----------------------------------------------------------------------------------------
//  NOTE: Integer numbers are 0 fixed-point numbers
//
//  mult()
//  0xC0    num_a (m fixed-point) or num_b (n fixed-point)
//  0xC1    result (m+n fixed-point)            result = num_a * num_b
//
//  mult_b28()
//  0xC2    num_a (m fixed-point) or num_b (N=28 fixed-point)) or n
//  0xC3    result (m fixed-point)              result = bit_truncated(num_a*num_b) + error
//
//  div()
//  0xC6    numer or denom or n                 Three messages. The first one is numer, the second one is denom, the third one is n.
//  0xC7    result (n fixed-point)              result = numer / denom
//
//  0xCA    snt_threshold                       snt_threshold has been updated with snt_read_wup_timer() + snt_duration (xo fail w/ skip_calib)
//  0xCB    snt_threshold                       snt_threshold has been updated with snt_read_wup_timer() + snt_duration (xo pass w/ skip_calib)
//  0xCC    snt_threshold_prev                  snt_threshold_prev in calc_ssls()
//  0xCD    snt_curr_val                        snt_curr_val in calc_ssls()
//  0xCE    snt_delta                           snt_delta in calc_ssls()
//  0xCF    ssls                                ssls calculated in calc_ssls()
//  -----------------------------------------------------------------------------------------
//  < I2C Transaction > - Defined in TMCv2r1.c
//  -----------------------------------------------------------------------------------------
//
//  --- nfc_i2c_byte_write()
//  0xD0    {E2, nb, addr}                      Byte Write Information.
//                                                  [   28] E2
//                                                  [27:24] Number of Bytes (1-4)
//                                                  [23: 0] Byte Address in EEPROM
//  0xD1    data                                Data use for Byte Write
//
//  --- nfc_i2c_byte_read()
//  0xD2    {E2, nb, addr}                      Byte Read Information.
//                                                  [   28] E2
//                                                  [27:24] Number of Bytes (1-4)
//                                                  [23: 0] Byte Address in EEPROM
//  0xD3    data                                Data read using Byte Read
//
//  --- nfc_i2c_word_read()
//  0xD4    {nw, addr}                          Word Read Information.
//                                                  [31:24] Number of Words (1-64)
//                                                  [23: 0] Byte Address in EEPROM
//  0xD5    {ptr}                               Word Read Information.
//                                                  [31: 0] ptr (where the read data is stored)
//  0xD6    value                               Word Read operation return value
//                                                  0x1: Word Read operation has been successfully done
//                                                  0x0: Word Read operation has NAKed. May retry.
//  0xD7    ptr                                 ptr where the 32-bit read data (shown with 0xD8) is stored
//  0xD8    data                                32-bit read data stored at ptr (shown with 0xD7)
//
//  --- Common
//  0xDF    iter                                Iteration number (if previous I2C transcation was NAK'd)
//
//  -----------------------------------------------------------------------------------------
//  < I2C Token > - Defined in TMCv2r1.c
//  -----------------------------------------------------------------------------------------
//
//  --- nfc_i2c_get_token()
//  0xE0    0x01000000                          INFO - NAK'd while trying to get the I2C token. Will Re-try.
//  0xE0    0x01000001                          PASS - Successfully got the I2C token
//  0xE0    0x01000002                          FAIL - Number of Trials exceeds
//  0xE0    0x01000003                          PASS - Token already exists
//
//  --- nfc_i2c_release_token()
//  0xE0    0x02000000                          FAIL - Not Acked while trying to release the I2C token.
//  0xE0    0x02000001                          PASS - Successfully released the I2C token
//  0xE0    0x02000003                          PASS - There was no token
//
//  -----------------------------------------------------------------------------------------
//  < Timeout & Error Handling > - Some of them are defined in TMCv2r1.c
//  -----------------------------------------------------------------------------------------
//  0xEE    value                               SNT Timer Sync Read value 
//  0xEF    __wfi_timeout_id__                  Timeout/Fail occurs
//                                                  0x0: Generic/Unknown
//                                                  0x1: Timed out during a display update (EID)
//                                                  0x2: Timed out while reading SNT timer (WUP)
//                                                  0x3: Timed out while accessing PMU register (PMU)
//                                                  0x4: Timed out during temperature measurement (SNT)
//                                                  0x5: I2C NAK (I2C)
//                                                  0x6: Meta-stability from SNT timer reading (MET)
//  0xEF    0x1010DEAD                          Timeout/Fail occurs during initialization
//  0xEF    0x12C0DEAD                          nfc_i2c_byte() returns NAK; It may retry.
//
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//******************************************************************************************* 
//
//                               THINGS TO BE NOTED
//
//******************************************************************************************* 
// set_halt and IRQ
//-------------------------------------------------------------------------------------------
//
// NOTE: Do NOT enable IRQ_REGn if: 
//                    REGn is a target register that a reply msg writes into 
//              -AND- set_halt_until_mbus_trx() is used
//              -AND- REGn IRQ handler generates an MBus message.
//      Example)
//              PMU Reply message writes into REG0.
//              In main()
//                  *NVIC_ISER = (0x1 << IRQ_REG0);
//                  set_halt_until_mbus_trx();
//                  mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
//                  set_halt_until_mbus_tx();
//              In handler_ext_int_reg0
//                  mbus_write_message32(0x71, 0x3);
//
//              -> The reply msg from PMU writes into REG0, triggering IRQ_REG0, 
//                 which then sends out the msg 0x71, 0x3.
//                 If this happens BEFORE the system releases the halt state, 
//                 the system may still stay in 'set_halt_until_mbus_trx()'
//
//******************************************************************************************* 

//*******************************************************************************************
// HEADER FILES
//*******************************************************************************************

#include "TMCv2r1.h"

//*******************************************************************************************
// DEVEL Mode
//*******************************************************************************************
// Enable 'DEVEL' for the following features:
//      - Send debug messages
//      - Use default values rather than grabbing the values from EEPROM
#define DEVEL
//#define ENABLE_XO_PAD
//#define USE_SHORT_REFRESH
//#define ENABLE_DEBUG_SYSTEM_CONFIG

//*******************************************************************************************
// INITIALIZATION
//*******************************************************************************************
#define INIT_TRIG_ALL   0xF00D
#define INIT_TRIG_NOCAL 0xBEEF
#define INIT_DEBUG_READ 0xDEAD
#define INIT_RESET      0x0000

#define TEMP_CALIB_A_DEFAULT    0x00007E8B  // Default value of temperature Calibration Coefficient a (fixed-point, N=10)
#define TEMP_CALIB_B_DEFAULT    0x005479B9  // Default value of temperature Calibration Coefficient b (fixed-point, N=10)

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1
#define FLAG_XO_INITIALIZED     2
#define FLAG_WD_ENABLED         3
#define FLAG_MAIN_CALLED_BY_SNT 4   // Sleep has been bypassed before the current active session. Thus, main() has called from within operation_sleep(). This implies that xo_val_curr is inaccurate.
#define FLAG_INVLD_XO_PREV      5   // Sleep has been bypassed before the previous active session. This implies that xo_val_prev is inaccurate.
#define FLAG_BOOTUP_DONE        6
#define FLAG_MAIN_CALLED_BY_GPO 7   // Same as FLAG_MAIN_CALLED_BY_SNT. The only difference is that it directly calls main() because it detects a GPO pulse before/during the low-power active mode.
#define FLAG_TEMP_MEAS_PEND     8   // Indicates that the system has gone sleep for temperature measurement

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// PRE Clock Generator Frequency
#define CPU_CLK_FREQ    140000  // XT1F#1, SAR_RATIO=0x60 (Sleep Voltages: 4.55V/1.59V/0.79V)

// PRE Clock-based Delay (5 instructions @ CPU_CLK_FREQ)
#define DLY_1S      ((CPU_CLK_FREQ>>3)+(CPU_CLK_FREQ>>4)+(CPU_CLK_FREQ>>5))  // = CPU_CLK_FREQx(1/8 + 1/16 + 1/32) = CPU_CLK_FREQx(35/160) ~= (CPU_CLK_FREQ/5)
#define DLY_1MS     DLY_1S >> 10                                             // = DLY_1S / 1024 ~= DLY_1S / 1000

// XO Frequency
#define XO_FREQ_SEL 0                       // XO Frequency (in kHz) = 2 ^ XO_FREQ_SEL
#define XO_FREQ     1<<(XO_FREQ_SEL+10)     // XO Frequency (in Hz)

// XO Initialization Wait Duration
#define XO_WAIT_A   2*DLY_1S    // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S    // Must be ~1 second delay.

// Pre-defined Wakeup Intervals
#define WAKEUP_INTERVAL_ACTIVE      1   // (Default: 1) Wakeup Interval for XT1_ACTIVE (unit: minutes)
#ifdef USE_SHORT_REFRESH
    #define WAKEUP_INTERVAL_IDLE    2   // (Default: 720) Wakeup Interval for XT1_IDLE (unit: minutes)
#else
    #define WAKEUP_INTERVAL_IDLE    720    // (Default: 720) Wakeup Interval for XT1_IDLE (unit: minutes)
#endif

// XO Failure Type
#define XO_FAIL_STOP        (0x1<<0)
#define XO_FAIL_UNSTABLE    (0x1<<1)
#define XO_FAIL_MAX_ERR     (0x1<<2)


//*******************************************************************************************
// WAKEUP SOURCE DEFINITIONS
//*******************************************************************************************
#define WAKEUP_BY_GOCEP     get_bit(wakeup_source, 0)
#define WAKEUP_BY_WUPT      get_bit(wakeup_source, 1)
#define WAKEUP_BY_XOT       get_bit(wakeup_source, 2)
#define WAKEUP_BY_GPIO      get_bit(wakeup_source, 3)
#define WAKEUP_BY_MBUS      get_bit(wakeup_source, 4)
#define WAKEUP_BY_GIT       get_bit(wakeup_source, 5)
#define WAKEUP_BY_GPIO0     (WAKEUP_BY_GPIO && get_bit(wakeup_source, 8))
#define WAKEUP_BY_GPIO1     (WAKEUP_BY_GPIO && get_bit(wakeup_source, 9))
#define WAKEUP_BY_GPIO2     (WAKEUP_BY_GPIO && get_bit(wakeup_source, 10))
#define WAKEUP_BY_GPIO3     (WAKEUP_BY_GPIO && get_bit(wakeup_source, 11))
#define WAKEUP_BY_SNT       WAKEUP_BY_MBUS
#define WAKEUP_BY_NFC       WAKEUP_BY_GPIO0
#define MAIN_CALLED         (WAKEUP_BY_MBUS && get_bit(wakeup_source, 7))

//*******************************************************************************************
// FSM STATES
//*******************************************************************************************

// XT1 States
#define XT1_RESET       0x0     // set_system() automatically calls XT1_IDLE once XT1_RESET is completed.
#define XT1_IDLE        0x1
#define XT1_PEND        0x2
#define XT1_ACTIVE      0x3
#define XT1_CRASH       0x9     // Used for set_system()

// XT1 Commands
#define CMD_NOP         0x00
#define CMD_START       0x01
#define CMD_STOP        0x02
#define CMD_GETSEC      0x03
#define CMD_CONFIG      0x04
#define CMD_HRESET      0x07
#define CMD_NEWKEY      0x08
#define CMD_NEWAB       0x09
#define CMD_DISPLAY     0x10
#define CMD_SRESET      0x1E
#define CMD_DEBUG       0x1F

// XT1 Constants
#define ACK 1
#define ERR 0

// Temperature Sample Type
#define SAMPLE_RAW      1
#define SAMPLE_NORMAL   0

//*******************************************************************************************
// EID LAYER CONFIGURATION
//*******************************************************************************************

// Segment ID : See 'External Connections' for details
#define SEG_PLAY        0
#define SEG_TICK        1
#define SEG_LOWBATT     2
#define SEG_BACKSLASH   3
#define SEG_SLASH       4
#define SEG_PLUS        5
#define SEG_MINUS       6

// Display Patterns: See 'External Connections' for details
#define DISP_PLAY       (0x1 << SEG_PLAY     )
#define DISP_TICK       (0x1 << SEG_TICK     )
#define DISP_LOWBATT    (0x1 << SEG_LOWBATT  )
#define DISP_BACKSLASH  (0x1 << SEG_BACKSLASH)
#define DISP_SLASH      (0x1 << SEG_SLASH    )
#define DISP_PLUS       (0x1 << SEG_PLUS     )
#define DISP_MINUS      (0x1 << SEG_MINUS    )

// Display Presets
#define DISP_NONE           (0)
#define DISP_CHECK          (DISP_TICK  | DISP_SLASH)
#define DISP_CROSS          (DISP_SLASH | DISP_BACKSLASH)
#define DISP_ALL            (0x7F)

// Update Types
#define EID_GLOBAL      0
#define EID_LOCAL       1

//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************

//-------------------------------------------------------------------------------------------
// System Configuration Variables in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in update_system_configs() function.
//-------------------------------------------------------------------------------------------

//--- VBAT and ADC

    //----------------------------------------------------
    //  ADC Temperature Region Definition
    //----------------------------------------------------
    //  Z = Temperature Measurement (raw code)
    //
    //  Temp Measurement        Offset added to ADC threshold values
    //----------------------------------------------------
    //  ADC_T1 < Z              ADC_OFFSET_R1 
    //  ADC_T2 < Z <= ADC_T1    ADC_OFFSET_R2
    //  ADC_T3 < Z <= ADC_T2    ADC_OFFSET_R3
    //  ADC_T4 < Z <= ADC_T3    ADC_OFFSET_R4
    //           Z <= ADC_T4    ADC_OFFSET_R5
    //----------------------------------------------------

union eeprom_adc_th_t {
    struct {
        unsigned t1 :   16; // [15: 0] Temperature Threshold (Raw Code) for ADC. See the table above.
        unsigned t2 :   16; // [31:16] Temperature Threshold (Raw Code) for ADC. See the table above.
        unsigned t3 :   16; // [47:32] Temperature Threshold (Raw Code) for ADC. See the table above.
        unsigned t4 :   16; // [63:48] Temperature Threshold (Raw Code) for ADC. See the table above.
    };
    struct {
        unsigned lower  :   32;
        unsigned upper  :   32;
    };
    uint64_t value;
};
volatile union eeprom_adc_th_t eeprom_adc_th;

union eeprom_adc_t {
    struct {
        unsigned offset_r1  :   8;  // [ 7: 0] ADC offset. 2's complement. See the table above.
        unsigned offset_r2  :   8;  // [15: 8] ADC offset. 2's complement. See the table above.
        unsigned offset_r3  :   8;  // [23:16] ADC offset. 2's complement. See the table above.
        unsigned offset_r4  :   8;  // [31:24] ADC offset. 2's complement. See the table above.
        unsigned offset_r5  :   8;  // [39:32] ADC offset. 2's complement. See the table above.
        unsigned low_vbat   :   8;  // [47:40] ADC code threshold to turn on the Low VBAT indicator.
        unsigned crit_vbat  :   8;  // [55:48] ADC code threshold to trigger the EID crash handler.
        unsigned dummy      :   8;  // [63:56]
    };
    struct {
        unsigned lower  :   32;
        unsigned upper  :   32;
    };
    uint64_t value;
};
volatile union eeprom_adc_t eeprom_adc;


//--- EID Configurations
union eeprom_eid_threshold_t {
    struct {
        unsigned high   :   16; // [15: 0] Temperature Threshold "10x(T+80)" to determin HIGH and MID temperature for EID
        unsigned low    :   16; // [31:16] Temperature Threshold "10x(T+80)" to determin MID and LOW temperature for EID
    };
    uint32_t value;
};
volatile union eeprom_eid_threshold_t eeprom_eid_threshold;
    
union eeprom_eid_duration_t {
    struct {
        unsigned high   :   16; // [15: 0] EID duration (ECTR_PULSE_WIDTH) for High Temperature
        unsigned mid    :   16; // [31:16] EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
        unsigned low    :   16; // [47:32] EID duration (ECTR_PULSE_WIDTH) for Low Temperature
        unsigned dummy  :   16; // [63:48]
    };
    struct {
        unsigned lower  :   32;
        unsigned upper  :   32;
    };
    uint64_t value;
};
volatile union eeprom_eid_duration_t eeprom_eid_duration;

union eeprom_eid_rfrsh_int_hr_t {
    struct {
        unsigned high   :   8;  // [ 7: 0] EID Refresh interval for High Temperature (unit: hr). '0' means 'do not refresh'.
        unsigned mid    :   8;  // [15: 8] EID Refresh interval for Mid Temperature (unit: hr). '0' means 'do not refresh'.
        unsigned low    :   8;  // [23:16] EID Refresh interval for Low Temperature (unit: hr). '0' meas 'do not refresh'.
        unsigned dummy  :   8;  // [31:24] 
    };
    uint32_t value;
};
volatile union eeprom_eid_rfrsh_int_hr_t eeprom_eid_rfrsh_int_hr;

//--- Other Configurations
union eeprom_misc_config_t {
    struct {
        unsigned pmu_num_cons_meas          :   4;  // [3:0] PMU_NUM_CONS_MEAS
                                                    //          Number of consecutive measurements required to trigger Low VBAT warning/Crash Handler.
        unsigned eid_crit_temp_threshold    :   1;  // [  4] EID_CRIT_TEMP_THRESHOLD
                                                    //          Display does not update when temperature is lower than EID_CRIT_TEMP_THRESHOLD
                                                    //          0x1: -20C 
                                                    //          0x0: -25C
        unsigned eid_disable_local_update   :   1;  // [  5] EID_DISABLE_LOCAL_UPDATE
                                                    //          0x1: Use GLOBAL_UPDATE for all temperatures.
                                                    //          0x0: Use LOCAL_UPDATE below EEPROM_ADDR_EID_LOW_TEMP_THRESHOLD. Use GLOBAL_UPDATE otherwise.
        unsigned pmu_adc_sar_margin         :   1;  // [  6] PMU_ADC_SAR_MARGIN
                                                    //          0x1: 9%
                                                    //          0x0: 8% (default)
        unsigned pmu_adc_sar_hysteresis     :   1;  // [  7] PMU_ADC_SAR_HYSTERESIS
                                                    //          0x1: 1 (default)
                                                    //          0x0: 0
        unsigned ignore_first_sample_exc    :   1;  // [  8] IGNORE_FIRST_SAMPLE_EXCURSION
                                                    //          0x1: The first sample is NOT counted for NUM_CONS_EXCURSIONS
                                                    //          0x0: The first sample is counted for NUM_CONS_EXCURSIONS
    };
    uint32_t value;
};
volatile union eeprom_misc_config_t eeprom_misc_config;
        

//--- Temperature Calibration Coefficients
union eeprom_temp_calib_t {
    struct {
        unsigned a  :   32; // Temperature Calibration Coefficient a
        unsigned b  :   32; // Temperature Calibration Coefficient b
    };
    uint64_t value;
};
volatile union eeprom_temp_calib_t eeprom_temp_calib;


//-------------------------------------------------------------------------------------------
// User Configuration Variables in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in update_user_configs() function.
//-------------------------------------------------------------------------------------------

union eeprom_user_config_t {
    struct {
        //--- VBAT and ADC
        unsigned high_temp_threshold   :    16; // [15: 0] Threshold for High Temperature
        unsigned low_temp_threshold    :    16; // [31:16] Threshold for Low Temperature
        unsigned temp_meas_interval    :    16; // [47:32] Period of Temperature Measurement
        unsigned temp_meas_start_delay :    16; // [63:48] Start Delay before starting temperature measurement
    };
    struct {
        unsigned lower  :   32;
        unsigned upper  :   32;
    };
    uint64_t value;
};
volatile union eeprom_user_config_t eeprom_user_config;

volatile uint32_t num_cons_excursions;      // Number of excursions required to trigger an alarm

union excursion_t {
    struct {
        unsigned num_cons_hi    : 15;   // [14: 0] Current number of consecutive high excursions
        unsigned alarm_hi       :  1;   // [   15] High Excursion alarm
        unsigned num_cons_lo    : 15;   // [30:16] Current number of consecutive low excursions
        unsigned alarm_lo       :  1;   // [   31] Low Excursion alarm
    };
    struct {
        unsigned hi :   16;
        unsigned lo :   16;
    };
    uint32_t value;
};
volatile union excursion_t tmp_exc; // Updated whenever the measurement is done (at user-specified measurement interval)


//-------------------------------------------------------------------------------------------
// Handshake in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in nfc_check_cmd() function.
//-------------------------------------------------------------------------------------------

volatile uint32_t cmd;              // Command
volatile uint32_t cmd_param;        // Command Parameter

//-------------------------------------------------------------------------------------------
// System Status in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: Most of these variables are reset in reset_system_status() function.
// NOTE: Sample ID starts from 0
//-------------------------------------------------------------------------------------------
volatile uint32_t sub_sample_cnt;                   // Sub-sample count (Sample measured every minute)
volatile uint32_t temp_sample_cnt;                  // Temp sample count ('sample average' calculated at user-specified interval)
volatile uint32_t eeprom_sample_cnt;                // EEPROM sample count ('sample average' stored into EEPROM)
volatile uint32_t sub_sample_sum;                   // Sum of the sub-samples
volatile uint32_t num_calib_xo_fails;               // Number of Calibration fails (XO timeout/unstable)
volatile uint32_t num_calib_max_err_fails;          // Number of Calibration fails (MAX_CHANGE error)

// System Status/State (See comment on EEPROM_ADDR_SYSTEM_STATE and EEPROM_ADDR_FAIL)
union status_t {
    struct {
        //--- Stored in EEPROM (SYSTEM_STATE)
        unsigned curr_state         :   2;  // [ 1: 0] Current System State
        unsigned config_set         :   1;  // [    2] Valid User Configs set using CONFIG command
        unsigned memory_full        :   1;  // [    3] Memory (EEPROM) Full
        unsigned sample_type        :   1;  // [    4] Sample Type (1: Raw, 0: Normal)
        unsigned activated          :   1;  // [    5] System Activated
        unsigned unread_sample      :   1;  // [    6] Unread Samples
        unsigned crash_triggered    :   1;  // [    7] Crash Handler Triggered
        //--- Stored in EEPROM (TIMEOUT/FAIL)
        unsigned fail_id            :   8;  // [15: 8] Timeout/Fail ID
        //--- TMC-internal
        unsigned aes_key_set        :   1;  // [   16] 1 indicates it has a valid AES key from NEWKEY command
        unsigned disp_skipped       :   1;  // [   17] 1 indicates it has skipped a display update (e.g., when start_delay=0 it skips the 'play-only' pattern.)
        unsigned custom_init        :   1;  // [   18] 1 indicates it uses a custom system init values, rather than the default values.
        unsigned skip_init          :   1;  // [   19] 1 indicates it needs to skip loading the init values from EEPROM.
        unsigned dummy0             :  12;  // [31:20] 
    };
    struct {
        unsigned eeprom_state   :   8;
        unsigned eeprom_fail    :   8;
        unsigned internal       :  16;
    };
    uint32_t value;
};

volatile union status_t status;


//-------------------------------------------------------------------------------------------
// XO/SNT Counters and Calibration
//-------------------------------------------------------------------------------------------

volatile uint32_t xo_val_curr_tmp;                  // XO counter value read in the beginning of main() (tentative)
volatile uint32_t xo_val_curr;                      // XO counter value read in the beginning of main() (committed)
volatile uint32_t xo_val_prev;                      // XO counter value from the previous active session
volatile uint32_t snt_freq;                         // SNT Timer Frequency (Hz)
volatile uint32_t snt_duration;                     // SNT counter value that corresponds to the given interval
volatile uint32_t snt_threshold;                    // SNT Timer Threshold to wake up the system
volatile uint32_t snt_threshold_prev;               // Previous SNT Timer Threshold
volatile uint32_t wakeup_interval;                  // Wakeup Period (unit: minutes)
volatile uint32_t snt_skip_calib;                   // If 1, it skips the SNT calibration and uses the previous snt_freq to calculate the next snt_threshold.
volatile uint32_t calib_status;                     // Calibration Status
                                                    // [2]: 1 if the SNT or XO counter has an incorrect value (>6.25% (=1/16) error than expected)
                                                    // [1]: 1 if XO is unstable (i.e., asynchronous read glitches do not disappear quickly)
                                                    // [0]: 1 if there has been a XO timeout (i.e., XO has stopped)
volatile uint32_t ssls;                             // [15: 0] Seconds Since the Last Sample (Max 65535 seconds = 18.2 hours)

//-------------------------------------------------------------------------------------------
// Data Compression & Encryption
//-------------------------------------------------------------------------------------------
#define COMP_SECTION_SIZE       128
#define COMP_UNCOMP_NUM_BITS    11
#define COMP_OFFSET_K           (273-80)
volatile uint32_t comp_sample_id;
volatile uint32_t comp_prev_sample;
volatile uint32_t comp_bit_pos;             // Bit position; 0 means Byte#128[0]. Last bit position is 64511 (i.e., Byte#8192[7])
                                            // NOTE: qword (Quardword = 128 bits)
                                            //          qword_id=n indicates Byte#(16n+128)~Byte(16n+143), for n=0, ..., 503 (integer)
                                            //          qword_id=comp_bit_pos>>7
volatile uint32_t qword[4];                 // Current (incomplete) Quadword

volatile uint32_t aes_key_valid;            // 1 indicates *(AES_KEY_n) is valid for the current wakeup session (It is NOT for the aes_key[4])
volatile uint32_t aes_key[4];               // Clone of the AES Key written to EEPROM (EEPROM is reset after the cloning is done)

volatile uint32_t crc32;                    // Adler-32 CRC Value

//-------------------------------------------------------------------------------------------
// Other Global Variables
//-------------------------------------------------------------------------------------------

//--- Wakeups
volatile uint32_t wakeup_source;            // Wakeup Source. Updated each time the system wakes up. See 'WAKEUP_SOURCE definition'.
volatile uint32_t pretend_wakeup_by_snt;    // 1 indicates that the current wakeup must be handled as if it was triggered by the SNT timer, 
                                            // even thought it was not. e.g. start_delay=0 or direct main() call.

//--- System State
volatile uint32_t xt1_state;                // TMP state

//--- Temperatre & ADC
union temp_t {
    struct {
        unsigned val        : 16;   // [ 0:15] Latest temp measurement: 10x(T+80)
        unsigned raw        : 16;   // [31:16] Latest temp measurement: Raw code
    };
    uint32_t value;
};

volatile union temp_t temp;

union meas_t {
    struct {
        unsigned adc        : 8;    // [ 7: 0] Latest PMU ADC Reading
        unsigned sar        : 8;    // [15: 8] Suggested new SAR Ratio
        unsigned low_vbat   : 1;    // [   16] Low VBAT Detected
        unsigned crit_vbat  : 1;    // [   17] Critical VBAT Detected
        unsigned valid      : 1;    // [   18] 1 indicates temp and meas have been updated during the current active session.
    };
    uint32_t value;
};

volatile union meas_t meas;

volatile uint32_t sar_adc_addr;             // For debugging (temp sensor calibration)
volatile uint32_t sar_adc_data;             // For debugging (temp sensor calibration)
                                            // [   15] XO Fail
                                            // [   14] Max Error Fail
                                            // [13: 8] SAR_RATIO - 64
                                            // [ 7: 0] ADC Value

union override_t {
    struct {
        unsigned sar_margin :  4;   // [ 3: 0] SAR Margin
        unsigned dummy      : 27;   // [30: 4] Dummy
        unsigned enable     :  1;   // [   31] Override enable
    };
    uint32_t value;
};

volatile union override_t override;

//--- Display
volatile uint32_t disp_refresh_interval;    // (Unit: min) Display Refresh Interval. 0 means 'do not refresh'.
volatile uint32_t disp_min_since_refresh;   // (Unit: min) Minutes since the last Display Refresh.
volatile uint32_t eid_duration;             // EID active CP duration 
volatile uint32_t eid_disp_before_user_mod; // [ 31] Display has been modified by user using DISPLAY command
                                            // [6:0] Display pattern before being modified by DISPLAY command. Valid only if bit[31]=1.
volatile uint32_t eid_updated_at_low_temp;  // Becomes 1 when the display gets updated at a temperature lower than eeprom_eid_threshold.low.
volatile uint32_t eid_updated_at_crit_temp; // Becomes 1 when the display has a pending update due to a critical temperature.
volatile uint32_t eid_crit_temp_threshold;  // Display does not update when temperature is lower than EID_CRIT_TEMP_THRESHOLD
volatile uint32_t eid_update_type;          // EID Update Type (0: Global Update, 1: Local Update)

//--- Write Buffer
union buffer_t {
    struct {
        unsigned sample_count   :   23; // [22: 0] Sample Count
        unsigned last_qword_id  :    9; // [31:23] Last Qword ID
        unsigned adc            :    8; // [39:32] ADC Value
        unsigned sar            :    8; // [47:40] SAR Ratio
        unsigned eeprom_addr    :   16; // [63:48]
    };
    struct {
        unsigned count          :   32; // [31: 0] Last Qword ID and Sample Count
        unsigned pmu            :   16; // [47:40] SAR and ADC
        unsigned eeprom_info    :   16; // [63:48]
    };
    uint64_t value;
};

volatile union buffer_t buffer;

volatile uint32_t buffer_valid;     // Becomes 1 when updating buffer_data and buffer_crc32; Becomes 0 after writing buffer_data and buffer_crc32 in EEPROM.
volatile uint32_t buffer_data[4];
volatile uint32_t buffer_crc32;


//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep/IRQ Handling
static void operation_sleep (uint32_t check_snt);
static void operation_sleep_snt_timer(uint32_t check_snt);
static void operation_dead(void);
static void operation_init(void);
void disable_all_irq_except_timer32(void);
void fail_handler(uint32_t id);

//--- FSM
static void set_system(uint32_t target);
static void reset_system_status(uint32_t target);
static void commit_status(void);
static void meas_temp_adc (uint32_t go_sleep);
static void snt_operation (void);
static void display_low_batt (void);

//--- XO & SNT Timers
static uint32_t get_xo_cnt(void);
static void calibrate_snt_timer(uint32_t skip_calib);
static uint32_t calc_ssls(void);

//--- E-Ink Display
static uint32_t eid_get_curr_seg(void);
static void eid_update_configs(void);
static void eid_update_display(uint32_t seg);
static void eid_blink_display(void);

//--- NFC
static uint32_t nfc_set_ack(uint32_t ack);
static uint32_t nfc_set_ack_with_data(uint32_t data);
static uint32_t nfc_check_cmd(void);

//--- EEPROM Variables
static void update_system_configs(uint32_t use_default);
static void update_temp_coefficients(void);
static void update_user_configs(void);
static void update_aes_key(void);
#ifdef ENABLE_DEBUG_SYSTEM_CONFIG
static void debug_system_configs(void);
#endif

//--- Write Buffer
static void buffer_init  (void);
static void buffer_update(uint32_t addr, uint32_t commit_crc);
static void buffer_commit(void);

//--- Data Compression
uint32_t tcomp_encode (uint32_t value);
uint32_t reverse_bits (uint32_t bit_stream, uint32_t num_bits);

//--- 128-bit qword Handling
void set_qword (uint32_t pattern);
void sub_qword (uint32_t msb, uint32_t lsb, uint32_t value);

//--- AES & CRC
void aes_encrypt(uint32_t* pt);
void aes_encrypt_eeprom(uint32_t addr);
uint32_t calc_crc32_128(uint32_t* src, uint32_t crc_prev);

//*******************************************************************************************
// FUNCTIONS IMPLEMENTATION
//*******************************************************************************************

//-------------------------------------------------------------------
// Initialization/Sleep Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: operation_sleep
// Args    : check_snt  - Check the current SNT counter value
//                        and decide whether to bypass the sleep
// Description:
//           Sends the MBus Sleep message
//           If check_snt=1, it first checks the SNT counter value.
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep (uint32_t check_snt) {

    // Reset GOC_DATA_IRQ
    //*GOC_DATA_IRQ = 0; // GOC IRQ is not used.

    // NOTE: EID will be stopped upon sleep message.

    // Power off NFC
    nfc_power_off();

    // Make sure we have done the PMU adjustmenet
    meas_temp_adc(/*go_sleep*/0);
    if (temp.val > 600) {
        if (meas.low_vbat) display_low_batt();
    }

    if (check_snt) {
        // Just in case - check the current SNT counter value.
        // NOTE: SNT counter is 32-bit long.
        //       With SNT Freq=1500Hz, it expires in 2863312 sec = 47722 min = 795 hrs = 33 days
        //-----------------------------------------------------------------
        //  t: snt_threshold
        //  c: snt_val
        //  dist_min = 10s (if high_power_history=1)
        //             4s  (if high_power_history=0)
        //  dist_max = >24 hrs (=86400sec)
        //
        // CASE A)  th > c
        //
        //          0              c    th                      MAX
        //          |--------------|----|------------------------|      if th - c < dist_min: it is near the wakeup
        //
        //          0  c                                     th MAX
        //          |--|-------------------------------------|---|      if th - c > dist_max: it has already passed the threshold.
        //
        // CASE B)  th < c
        //
        //          0              th   c                       MAX
        //          |--------------|----|------------------------|      if th + A > dist_max: it has already passed the threshold
        //                               <-----------A---------->
        //
        //          0  th                                  c    MAX
        //          |--|-----------------------------------|-----|      if th + A < dist_min: it is near the wakeup
        //                                                  <-A->
        //
        //-----------------------------------------------------------------
        uint32_t dist_min = get_high_power_history()  
                            ? (snt_freq<<3)+(snt_freq<<1) // 10s
                            : (snt_freq<<2);              // 4s
        uint32_t dist_max = (snt_freq<<18);               // 262144s (= 4369min = 73hrs); Also see calc_ssls()
        uint32_t snt_val = snt_read_wup_timer();
        uint32_t dist = snt_threshold - snt_val;

        //uint32_t dist;
        //if (snt_threshold >= snt_val) { dist = snt_threshold - snt_val;                    } 
        //else                          { dist = snt_threshold + (0xFFFFFFFF - snt_val) + 1; }

        if ((dist < dist_min) || (dist > dist_max)) {
            // Call main()
            set_flag(FLAG_MAIN_CALLED_BY_SNT, 1);
            *NVIC_ICPR = 0xFFFFFFFF; // For safety, disable/clear all pending IRQs.
            main();
            return;
        }
    }

    // Perform the low-power active mode if needed
    if (get_high_power_history()) {
        // Low-Power Active mode: 18~20uA for ~6 seconds
        *REG_MBUS_WD = 1680000; // 6s with 280kHz;
        halt_cpu();
    }
    // Go to sleep
    else {
        mbus_sleep_all(); 
        while(1);
    }
}

//-------------------------------------------------------------------
// Function: operation_sleep_snt_timer
// Args    : check_snt  - Check the current SNT counter value
//                        and decide whether to bypass the sleep
// Description:
//           Goes into sleep with SNT Timer enabled
//           Auto Reset is disabled.
//           SNT threshold is set to snt_threshold.
//           If check_snt=1, it first checks the SNT counter value.
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep_snt_timer(uint32_t check_snt) {
    if (!snt_set_wup_timer(/*auto_reset*/0, /*threshold*/snt_threshold)) {
        __wfi_id__ = FAIL_ID_GEN;
        fail_handler(/*id*/__wfi_id__);
    }
    #ifdef DEVEL
        mbus_write_message32(0x8F, snt_threshold);
    #endif
    operation_sleep(/*check_snt*/check_snt);
}

//-------------------------------------------------------------------
// Function: operation_dead
// Args    : None
// Description:
//           Turns off everything,
//           then goes into sleep indefinitely.
//           NOTE: EID CPs are automatically reset upon sleep message.
// Return  : None
//-------------------------------------------------------------------
static void operation_dead(void) {

    // Turn off NFC 
    nfc_power_off();
    
    // Turn off temperature sensor
    snt_temp_sensor_reset();
    snt_temp_sensor_power_off();

    // Disable SNT timer, XO timer
    snt_stop_timer();
    snt_disable_wup_timer();
    xo_stop();

    // For safetly, disable all IRQs.
    *NVIC_ICER = 0xFFFFFFFF;

    mbus_sleep_all();  // This will turn off any ongoing EID CP operation
    while(1);
}

//-------------------------------------------------------------------
// Function: operation_init
// Args    : None
// Description:
//           Initializes the system
// Return  : None
//-------------------------------------------------------------------
static void operation_init (void) {

    uint32_t i;

    if (!get_flag(FLAG_ENUMERATED)) {

        // Very first initialization
        status.value = 0;

        //-------------------------------------------------
        // PRE Tuning
        //-------------------------------------------------

        //--- Set CPU & MBus Clock Speeds      Default
        *REG_CLKGEN_TUNE =    (0x0 << 21)   // 1'h0     CLK_GEN_HIGH_FREQ
                            | (0x3 << 18)   // 3'h3     CLK_GEN_DIV_CORE	#Default CPU Freq = 96kHz (pre-pex simulation, TT, 25C)
                            | (0x2 << 15)   // 3'h2     CLK_GEN_DIV_MBC	    #Default MBus Freq = 193kHz (pre-pex simulation, TT, 25C)
                            | (0x1 << 13)   // 2'h1     CLK_GEN_RING
                            | (0x0 << 12)   // 1'h0     RESERVED
                            | (0x0 << 10)   // 2'h1     GOC_CLK_GEN_SEL_DIV
                            | (0x5 <<  7)   // 3'h7     GOC_CLK_GEN_SEL_FREQ
                            | (0x0 <<  6)   // 1'h0     GOC_ONECLK_MODE
                            | (0x0 <<  4)   // 2'h0     GOC_SEL_DLY
                            | (0xF <<  0);  // 4'h8     GOC_SEL

        //--- Pending Wakeup Handling          Default
        *REG_SYS_CONF =       (0x0 << 9)    // 1'h1     NO_SLEEP_WITH_PEND_IRQ  #If 1, it replaces the sleep message with a selective wakeup message if there is a pending M0 IRQ. If 0, it goes to sleep even when there is a pending M0 IRQ.
                            | (0x0 << 8)    // 1'h0     ENABLE_SOFT_RESET	#If 1, Soft Reset will occur when there is an MBus Memory Bulk Write message received and the MEM Start Address is 0x2000
                            | (0x1 << 7)    // 1'h1     PUF_CHIP_ID_SLEEP
                            | (0x1 << 6)    // 1'h1     PUF_CHIP_ID_ISOLATE
                            | (0x8 << 0);   // 5'h1E    WAKEUP_ON_PEND_REQ	#[4]: GIT (PRE_E Only), [3]: GPIO (PRE only), [2]: XO TIMER (PRE only), [1]: WUP TIMER, [0]: GOC/EP

        //-------------------------------------------------
        // Enumeration
        //-------------------------------------------------
        set_halt_until_mbus_trx();
        mbus_enumerate(EID_ADDR);
        mbus_enumerate(SNT_ADDR);
        mbus_enumerate(PMU_ADDR);
        set_halt_until_mbus_tx();

        //-------------------------------------------------
        // Target Register Index
        //-------------------------------------------------
        mbus_remote_register_write(PMU_ADDR, 0x52, (0x10 << 8) | PMU_TARGET_REG_IDX);
        mbus_remote_register_write(SNT_ADDR, 0x07, (0x10 << 8) | SNT_TARGET_REG_IDX);
        mbus_remote_register_write(EID_ADDR, 0x05, (0x1 << 16) | (0x10 << 8) | EID_TARGET_REG_IDX);

        //-------------------------------------------------
        // PMU Settings
        //-------------------------------------------------
        pmu_init();

        //-------------------------------------------------
        // SNT Settings
        //-------------------------------------------------
        snt_init();

        // Update the flag
        set_flag(FLAG_ENUMERATED, 1);

        // Turn on the SNT timer clock
        snt_start_timer(/*wait_time*/DLY_1S);

        // Start the SNT counter
        snt_enable_wup_timer(/*auto_reset*/0);

        //-------------------------------------------------
        // XO Settings
        //-------------------------------------------------
        //--- XO Frequency
        *REG_XO_CONF2 =             // Default  // Description
            //-----------------------------------------------------------------------------------------------------------
            ( (0x1         << 20)   // (3'h1) XO_SEL_DLY	        #Adjusts pulse delay
            | (0x1         << 18)   // (2'h1) XO_SEL_CLK_SCN	    #Selects division ratio for SCN CLK
            | (XO_FREQ_SEL << 15)   // (3'h1) XO_SEL_CLK_OUT_DIV	#Selects division ratio for the XO CLK output. XO Freq = 2^XO_SEL_CLK_OUT_DIV (kHz)
            | (0x1         << 14)   // (1'h1) XO_SEL_LDO            #Selects LDO output as an input to SCN
            | (0x0         << 13)   // (1'h0) XO_SEL_0P6            #Selects V0P6 as an input to SCN
            | (0x0         << 10)   // (3'h0) XO_I_AMP	            #Adjusts VREF body bias buffer current
            | (0x0         <<  3)   // (7'h0) XO_VREF_TUNEB 	    #Adjust VREF level and TC
            | (0x0         <<  0)   // (3'h0) XO_SEL_VREF 	        #Selects VREF output from its diode stack
            );

        xo_stop();  // Default value of XO_START_UP is wrong in RegFile, so need to override it.

        //--- Start XO clock
        xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);
        // Update the flag
        set_flag(FLAG_XO_INITIALIZED, 1);

        //--- Configure and Start the XO Counter
        set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0, /*auto_reset*/0);
        start_xo_cnt();

    #ifdef ENABLE_XO_PAD
        start_xo_cout();
    #endif

        //-------------------------------------------------
        // Go to Sleep
        //-------------------------------------------------
        // Go to sleep
        set_wakeup_timer (/*timestamp*/10, /*irq_en*/0x1, /*reset*/0x1); // About 2 seconds
        mbus_sleep_all();
        while(1);
    }
    else if (!get_flag(FLAG_INITIALIZED)) {
        
      //// Set the Active floor setting to the minimum (b/c RAT is enabled at this point)
      //pmu_set_active_min();

        // Disable the PRC wakeup timer
        *REG_WUPT_CONFIG = *REG_WUPT_CONFIG & 0xFF3FFFFF; // WUP_ENABLE=0, WUP_WREQ_EN=0

        //-------------------------------------------------
        // NFC 
        //-------------------------------------------------
        nfc_init();

        // Set the Hardware/Firmware ID
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HW_ID, /*data*/HARDWARE_ID, /*nb*/4);
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_FW_ID, /*data*/FIRMWARE_ID, /*nb*/2);
        // Sub-Versions
        nfc_i2c_byte_write(/*e2*/0, /*addr*/6, /*data*/0x0000, /*nb*/2);

        // Set default values for CALIB and AES_KEY
        eeprom_temp_calib.a = TEMP_CALIB_A_DEFAULT;
        eeprom_temp_calib.b = TEMP_CALIB_B_DEFAULT;
        for (i=0; i<4; i++) *(aes_key+i) = 0x0;

        // Initialize the EEPROM variables
        update_system_configs(/*use_default*/1);

        // Turn off the NFC
        nfc_power_off();

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
	    eid_init();

        // Enable EID Crash Hander
        eid_enable_crash_handler();
        set_flag(FLAG_WD_ENABLED, 1); 

        //-------------------------------------------------
        // System Reset
        //-------------------------------------------------
        // Set system
        cmd = CMD_NOP;      // need to initialize here since it is used in reset_system_status()
        if (status.eeprom_fail!=0) fail_handler(/*id*/FAIL_ID_INIT); // If there has been a timeout/failure during initialization
        else status.value = 0;   // will be committed in set_system()
        set_system(/*target*/XT1_RESET);

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);

        //-------------------------------------------------
        // Go to Sleep for 10s.
        //-------------------------------------------------
        // this provides the information for the very first SNT calibration
        snt_freq = 1400;
        snt_duration = 30000;
        snt_threshold_prev = snt_read_wup_timer();
        snt_threshold = snt_threshold_prev + snt_duration;
        xo_val_curr = get_xo_cnt();
        operation_sleep_snt_timer(/*check_snt*/0);
    }
}


//-------------------------------------------------------------------
// FSM
//-------------------------------------------------------------------

static void set_system(uint32_t target) {

    //------------------------------------------
    // COMMON FOR ALL TARGETS
    //------------------------------------------
    #ifdef DEVEL
        mbus_write_message32(0x72, target);
    #endif

    //------------------------------------------
    // XT1_CRASH
    //------------------------------------------
    if (target==XT1_CRASH) {
        // Update the System State
        status.crash_triggered = 1;
        commit_status();
        // Turn off NFC
        nfc_power_off();
        // Update the state
        xt1_state = XT1_CRASH;
        return;
    }

    //--------------------------------------
    // XT1_RESET
    //--------------------------------------
    else if (target==XT1_RESET) {

        reset_system_status(target);

        // Reset some variables
        disp_refresh_interval    = 720;    // Every 12 hrs
        eid_duration             = 100;
        eid_disp_before_user_mod = 0;
        eid_updated_at_low_temp  = 0;
        eid_updated_at_crit_temp = 0;
        eid_update_type          = EID_GLOBAL;
        meas.valid               = 0;
        override.enable          = 0;
        ssls                     = 0;

        // Display all segments
        eid_update_display(/*seg*/DISP_ALL);

        // Update/Reset System Configuration
        if (!status.skip_init) update_system_configs(/*use_default*/0);

        // Reset the System State in EEPROM
        status.eeprom_state = 0; // curr_state=XT1_RESET
        status.eeprom_fail = 0;
        commit_status();

        // Update the state
        xt1_state = XT1_RESET;
    }

    //--------------------------------------
    // XT1_PEND
    //--------------------------------------
    else if (target==XT1_PEND) {

        reset_system_status(target);

        // If Raw Sample mode, use the pre-defined configuration
        if (status.sample_type == SAMPLE_RAW) {
            eeprom_user_config.high_temp_threshold   = 1800;   // 100C
            eeprom_user_config.low_temp_threshold    = 400;    // -40C
            eeprom_user_config.temp_meas_interval    = 1;      // 1 min
            eeprom_user_config.temp_meas_start_delay = 0;      // 0 min
            num_cons_excursions = 1;
        }

        // Display Play Sign 
        // If start_delay=0, the display does not need to be updated here. 
        if (eeprom_user_config.temp_meas_start_delay!=0) 
            eid_update_display(/*seg*/DISP_PLAY);
        else {
            status.disp_skipped = 1;
            #ifdef DEVEL
                mbus_write_message32(0xBA, (0x11 << 24) | 0x1 /*status.disp_skipped*/);
            #endif
        }

        // Start Delay
        wakeup_interval = eeprom_user_config.temp_meas_start_delay;

        // Set the System State
        status.curr_state = XT1_PEND;
        status.activated = 1;
        status.memory_full = 0;
        status.unread_sample = 0;
        commit_status();

        // Update the state
        xt1_state = XT1_PEND;
    }

    //------------------------------------------
    // XT1_IDLE
    //------------------------------------------
    else if (target==XT1_IDLE) {

        // Reset variables
        wakeup_interval   = WAKEUP_INTERVAL_IDLE;

        sub_sample_cnt = 0;
        sub_sample_sum = 0;

        // During Boot-Up
        if (!get_flag(FLAG_BOOTUP_DONE))
            eid_update_display(/*seg*/DISP_CHECK|DISP_PLUS|DISP_MINUS|DISP_LOWBATT);
        // If coming from XT1_RESET: Display Check only
        else if (xt1_state==XT1_RESET)
            // If correctly initialized, display Check Only
            if (status.custom_init)
                eid_update_display(/*seg*/DISP_CHECK);
            // If using the default values, display 'backslash' and 'tick'
            else                           
                eid_update_display(/*seg*/DISP_BACKSLASH|DISP_TICK);
        // If coming from XT1_PEND: Display Check only. Preserve Low-BATT indicator.
        else if (xt1_state==XT1_PEND)
            eid_update_display(/*seg*/(eid_get_curr_seg() & DISP_LOWBATT) | DISP_CHECK);
        // If coming from other states: Remove the Play sign
        else
            eid_update_display(/*seg*/eid_get_curr_seg() & ~DISP_PLAY);

        // Reset the System State
        status.curr_state = XT1_IDLE;
        commit_status();

        // Update the state
        xt1_state = XT1_IDLE;
    }

    //------------------------------------------
    // XT1_ACTIVE
    //------------------------------------------
    else if (target==XT1_ACTIVE) {
        wakeup_interval = WAKEUP_INTERVAL_ACTIVE;

        // Reset the System State
        status.curr_state = XT1_ACTIVE;
        commit_status();

        // Update the state
        xt1_state = XT1_ACTIVE;
    }

    // If XT1_RESET, it also needs to go to XT1_IDLE
    if (xt1_state==XT1_RESET) set_system(/*target*/XT1_IDLE);
}

static void reset_system_status(uint32_t target) {

    #ifdef DEVEL
        mbus_write_message32(0x73, 0x0);
    #endif

    // Critical Variables
    temp.value          = (2534 << 16) | (1000); // Assume 20C by default
    snt_threshold_prev  = 0;
    snt_threshold       = 0;

    // Data Compression Variables
    comp_sample_id   = 0;
    comp_prev_sample = 0;
    comp_bit_pos     = 0;
    crc32            = 1;
    set_qword(/*pattern*/0xFFFFFFFF);  // qword = all 1s.

    // Sub-sample counts
    sub_sample_cnt    = 0;
    sub_sample_sum    = 0;

    // Reset the Sample Count
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SAMPLE_COUNT, /*data*/0, /*nb*/4);
    temp_sample_cnt   = 0;
    eeprom_sample_cnt = 0;

    // Reset NUM_XO/CALIB_MAX_ERR
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, /*data*/0, /*nb*/2); // EEPROM_ADDR_NUM_CALIB_XO_FAILS, EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS
    num_calib_xo_fails      = 0;
    num_calib_max_err_fails = 0;

    // Consecutive Excursions
    tmp_exc.value = 0;

    // Reset the Write Buffer
    buffer_init();

    // Other System Status
    status.disp_skipped = 0;
    #ifdef DEVEL
        mbus_write_message32(0xBA, (0x11 << 24) | 0x0 /*status.disp_skipped*/);
    #endif
    if (target==XT1_RESET) {
        // custom_init & skip_init
        if ((cmd==CMD_SRESET) && (get_bit(cmd_param, 0))) {
            status.skip_init = 1; 
            #ifdef DEVEL
                mbus_write_message32(0xBA, (0x13 << 24) | 0x1 /*status.skip_init*/);
            #endif
        }
        else {
            status.skip_init = 0; 
            status.custom_init = 0;
            #ifdef DEVEL
                mbus_write_message32(0xBA, (0x12 << 24) | 0x0 /*status.custom_init*/);
                mbus_write_message32(0xBA, (0x13 << 24) | 0x0 /*status.skip_init*/);
            #endif
        }
    }

    // Following variables are not set/reset here
    //-----------------------------------------------------------
    // EEPROM_ADDR_SYSTEM_STATE (status)
    // EEPROM_ADDR_DISPLAY
    // EEPROM_ADDR_LAST_ADC
    // EEPROM_ADDR_LAST_SAR

}

static void commit_status(void) {
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SYSTEM_STATE, /*data*/status.eeprom_state, /*nb*/1);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_FAIL, /*data*/status.eeprom_fail, /*nb*/1);
    #ifdef DEVEL
        mbus_write_message32(0xBA, (0x00 << 24) | status.curr_state);
        mbus_write_message32(0xBA, (0x02 << 24) | status.config_set);
        mbus_write_message32(0xBA, (0x03 << 24) | status.memory_full);
        mbus_write_message32(0xBA, (0x04 << 24) | status.sample_type);
        mbus_write_message32(0xBA, (0x05 << 24) | status.activated);
        mbus_write_message32(0xBA, (0x06 << 24) | status.unread_sample);
        mbus_write_message32(0xBA, (0x07 << 24) | status.crash_triggered);
        mbus_write_message32(0xBA, (0x08 << 24) | status.fail_id);
    #endif
}

static void meas_temp_adc (uint32_t go_sleep) {
    #ifdef DEVEL
        mbus_write_message32(0x91, go_sleep);
    #endif

    if (!meas.valid) {

        // Read ADC first, in case go_sleep=1 (i.e., before goes into sleep for temp measurement)
        meas.adc = pmu_read_adc();

        if (go_sleep) {
            if (!get_flag(FLAG_TEMP_MEAS_PEND)) {

                #ifdef DEVEL
                    mbus_write_message32(0x91, 0x00000002);
                #endif

                //// Higher Floor Setting for Sleep
                //pmu_temp_sleep_floor();

                // Turn on SNT Temperature Sensor
                snt_temp_sensor_power_on(/*sel_ldo*/1);
                snt_temp_sensor_reset();

                // Set flag
                set_flag(FLAG_TEMP_MEAS_PEND, 1);

                // Release the reset for the Temp Sensor
                snt_temp_sensor_start();

                // go sleep
                mbus_sleep_all(); 
                while(1);
            }
            else {

                #ifdef DEVEL
                    mbus_write_message32(0x91, 0x00000003);
                #endif

                //// Get back to the original Floor Setting
                //pmu_normal_sleep_floor();

                set_flag(FLAG_TEMP_MEAS_PEND, 0);
            }
        }
        else {
            // Enable REG IRQ
            enable_reg_irq(SNT_TARGET_REG_IDX); // REG1

            // Turn on SNT Temperature Sensor
            snt_temp_sensor_power_on(/*sel_ldo*/1);
            snt_temp_sensor_reset();

            // Release the reset for the Temp Sensor
            start_timeout32_check(/*id*/FAIL_ID_SNT, /*val*/TIMEOUT_TH<<2); // ~2s
            snt_temp_sensor_start();

            WFI();
        }

        // Raw value
        temp.raw = (*SNT_TARGET_REG_ADDR) & 0xFFFF;

        // NOTE: temp.val is "10 x (T + 80)" where T is the actual temperature in celsius degree
        if (temp.raw < 240) { // if the raw code is low enough, skip the temp conversion process.
            temp.val = 500; // -30C
        }
        else {
            temp.val = tconv(   /* dout */ temp.raw,
                                /*   a  */ eeprom_temp_calib.a, 
                                /*   b  */ eeprom_temp_calib.b, 
                                /*offset*/ COMP_OFFSET_K);
        }

        // Turn off the temperature sensor
        snt_temp_sensor_reset();
        snt_temp_sensor_power_off();

        // PMU adjustment is done only after initialization (FLAG_INITIALIZED=1)
        if (get_flag(FLAG_INITIALIZED)) {
            // Determine ADC Offset, ADC Low VBAT threshold, and ADC Critical VBAT threshold
//            uint32_t adc_offset = 0;
            uint32_t adc_offset;
            if      (temp.val > eeprom_adc_th.t1) { adc_offset = eeprom_adc.offset_r1; }
            else if (temp.val > eeprom_adc_th.t2) { adc_offset = eeprom_adc.offset_r2; }
            else if (temp.val > eeprom_adc_th.t3) { adc_offset = eeprom_adc.offset_r3; }
            else if (temp.val > eeprom_adc_th.t4) { adc_offset = eeprom_adc.offset_r4; }
            else                                  { adc_offset = eeprom_adc.offset_r5; }
    
            // VBAT Measurement and SAR_RATIO Adjustment
            // meas.adc has been udpated above.
            meas.sar = pmu_calc_new_sar_ratio(  /*meas.adc*/      meas.adc,
                                               /*offset*/       adc_offset, 
                                               /*sel_margin*/   /*override.enable ? 
                                                                    override.sar_margin : 
                                                                    eeprom_misc_config.pmu_adc_sar_margin,*/
                                                                4,  // 15% margin
                                               /*hysteresis*/   eeprom_misc_config.pmu_adc_sar_hysteresis
                                            );
    
            meas.low_vbat  = pmu_check_low_vbat (    /*meas.adc*/         meas.adc,
                                                    /*offset*/          adc_offset, 
                                                    /*threshold*/       eeprom_adc.low_vbat,  
                                                    /*num_cons_meas*/   eeprom_misc_config.pmu_num_cons_meas
                                                    );
    
            meas.crit_vbat = pmu_check_crit_vbat(    /*meas.adc*/         meas.adc,
                                                    /*offset*/          adc_offset, 
                                                    /*threshold*/       eeprom_adc.crit_vbat, 
                                                    /*num_cons_meas*/   eeprom_misc_config.pmu_num_cons_meas
                                                   );
    
            #ifdef DEVEL
                mbus_write_message32(0x90, temp.value);
                mbus_write_message32(0x93, (meas.crit_vbat<<28)|(meas.low_vbat<<24)|(adc_offset<<16)|(meas.sar<<8)|(meas.adc));
            #endif
    
            // Change the SAR ratio
            pmu_set_sar_ratio(meas.sar);
    
            meas.valid = 1;
    
            /////////////////////////////////////////////////////////////////
            // EID Crash Handler
            //---------------------------------------------------------------
            //--- If VBAT is too low, trigger the EID Watchdog (System Crash)
            if (meas.crit_vbat) {
    
                // Set system
                set_system(/*target*/XT1_CRASH);
    
                // Trigger the Crash Handler
                eid_trigger_crash();
    
                // Go to indefinite sleep
                snt_set_wup_timer(/*auto_reset*/1, /*threshold*/0);
                operation_sleep(/*check_snt*/0);
            }
            /////////////////////////////////////////////////////////////////

        }
        else {
            // During boot-up, set some default values.
            meas.valid = 1;
            meas.low_vbat = 0;
            meas.crit_vbat = 0;
        }

        // Temp-releated settings
        eid_update_configs();
    }
    #ifdef DEVEL
    else {
        mbus_write_message32(0x91, 0xFFFFFFFF);
    }
    #endif

}

static void snt_operation (void) {

    // Meas temperature & ADC
    meas_temp_adc(/*go_sleep*/1);

    // Do nothing if temp.val < -20C
    if (temp.val > 600) {

        // Read the current display
        uint32_t curr_seg = eid_get_curr_seg();
        uint32_t new_seg = curr_seg;

        // XT_ACTIVE (Sample Average & Buffer Write)
        if (xt1_state == XT1_ACTIVE) {

            // Increment the sub-sample counter
            sub_sample_cnt++;
            sub_sample_sum += temp.val;

            #ifdef DEVEL
                mbus_write_message32(0x92, (0x1<<28)|sub_sample_cnt);
                mbus_write_message32(0x92, (0x2<<28)|sub_sample_sum);
            #endif


            if (// If this is the very first sample (right after the start delay)
                (temp_sample_cnt==0)
                // At User-defined measurement interval (Calculate an Averaged Sample)
               || (sub_sample_cnt==eeprom_user_config.temp_meas_interval)) {

                // Increment the temp sample counter 
                temp_sample_cnt++;

                #ifdef DEVEL
                    mbus_write_message32(0x92, (0x3<<28)|temp_sample_cnt);
                #endif

                // Calculate the average
                temp.val = div(/*numer*/sub_sample_sum, /*denom*/sub_sample_cnt, /*n*/1);
                if (temp.val&0x1) temp.val += 0x2;  // round-up
                temp.val >>= 1;
                sub_sample_cnt = 0;
                sub_sample_sum = 0;

                #ifdef DEVEL
                    if (status.sample_type==SAMPLE_NORMAL) mbus_write_message32(0x9C, temp.val);
                #endif

                // Check Excursions / Alarm
                uint32_t new_alarm = 0;
                if ((temp_sample_cnt>1) || (eeprom_misc_config.ignore_first_sample_exc==0)) {
                    if (temp.val > eeprom_user_config.high_temp_threshold) {
                        tmp_exc.lo = 0;
                        if (!tmp_exc.alarm_hi) {
                            tmp_exc.num_cons_hi++;
                            if (tmp_exc.num_cons_hi>=num_cons_excursions) {
                                tmp_exc.alarm_hi = 1;
                                new_alarm = 1;
                                #ifdef DEVEL
                                    mbus_write_message32(0x9D, (0x0<<28) | (0xF<<24) | tmp_exc.num_cons_hi);
                                #endif
                            }
                            #ifdef DEVEL
                            else {
                                mbus_write_message32(0x9D, (0x0<<28) | (0x0<<24) | tmp_exc.num_cons_hi);
                            }
                            #endif
                        }
                    }
                    else if (temp.val < eeprom_user_config.low_temp_threshold) {
                        tmp_exc.hi = 0;
                        if (!tmp_exc.alarm_lo) {
                            tmp_exc.num_cons_lo++;
                            if (tmp_exc.num_cons_lo>=num_cons_excursions) {
                                tmp_exc.alarm_lo = 1;
                                new_alarm = 1;
                                #ifdef DEVEL
                                    mbus_write_message32(0x9D, (0x1<<28) | (0xF<<24) | tmp_exc.num_cons_lo);
                                #endif
                            }
                            #ifdef DEVEL
                            else {
                                mbus_write_message32(0x9D, (0x1<<28) | (0x0<<24) | tmp_exc.num_cons_lo);
                            }
                            #endif
                        }
                    }
                    else {
                        tmp_exc.value = 0;
                    }
                }

                #ifdef DEVEL
                    mbus_write_message32(0x95, temp.val);
                    mbus_write_message32(0x96, comp_sample_id);
                    if (tmp_exc.alarm_hi && tmp_exc.alarm_lo) {
                        mbus_write_message32(0x9E, 0xFFFFFFFF);
                    }
                    else if (tmp_exc.alarm_hi) {
                        mbus_write_message32(0x9E, 0xFFFF0000);
                    }
                    else if (tmp_exc.alarm_lo) {
                        mbus_write_message32(0x9E, 0x0000FFFF);
                    }
                    else {
                        mbus_write_message32(0x9E, 0x00000000);
                    }
                #endif

                //--------------------------------------------------------------------------------
                // RAW DATA MODE
                //--------------------------------------------------------------------------------

                // sample_type: SAMPLE_RAW
                if (status.sample_type==SAMPLE_RAW) {

                        // Store ADC Reading and SAR Ratio
                        nfc_i2c_byte_write(/*e2*/0, 
                            /*addr*/ EEPROM_ADDR_LAST_ADC,      // EEPROM_ADDR_LAST_ADC, EEPROM_ADDR_LAST_SAR
                            /*data*/ (meas.sar<<8)|meas.adc,
                            /* nb */ 2
                            );

                        uint32_t byte_addr = (eeprom_sample_cnt<<1)+EEPROM_ADDR_DATA_RESET_VALUE;

                        // Store the Sample Count
                        nfc_i2c_byte_write(/*e2*/0, 
                            /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, 
                            /*data*/ (eeprom_sample_cnt+1),
                            /* nb */ 4
                            );

                        // Store the raw data
                        //  Max Sample Count = (8192 - 128) / 2 = 4032
                        //  However, we split this into two and store the raw data into the first half,
                        //  and SAR/ADC values in to the second half.
                        //  i.e., max sample count = 4032 / 2 = 2016
                        //      Byte# 128 - Byte#4159: Raw Data      (each sample is 16-bit)
                        //      Byte#4160 - Byte#8192: SAR/ADC Value (each sample is 16-bit)
                        if (eeprom_sample_cnt<2016) {
                            nfc_i2c_byte_write( /*e2*/   0, 
                                                /*addr*/ byte_addr, 
                                                /*data*/ temp.raw,
                                                /*nb*/   2);

                        // Store ADC & SAR
                        sar_adc_addr = byte_addr + (2016<<1);
                        sar_adc_data = ((meas.sar - 64)<<8) | (meas.adc&0xFF);

                        nfc_i2c_byte_write( /*e2*/   0,
                                            /*addr*/ sar_adc_addr,  // Starting from Byte#4160
                                            /*data*/ sar_adc_data,  // See Description
                                            /*nb*/   2);

                        #ifdef DEVEL
                            mbus_write_message32(0x9C, byte_addr);
                            mbus_write_message32(0x9D, temp.raw);
                            mbus_write_message32(0x9E, sar_adc_addr);
                            mbus_write_message32(0x9F, sar_adc_data);
                        #endif

                    }

                    eeprom_sample_cnt++;
                }

                //--------------------------------------------------------------------------------
                // CONVERTED & COMPRESSED DATA MODE
                //--------------------------------------------------------------------------------

                // sample_type: SAMPLE_NORMAL
                else {

                    // Store ADC Reading and SAR_RATIO
                    buffer.adc = meas.adc;
                    buffer.sar = meas.sar;

                    uint32_t num_bits;
                    uint32_t reverse_code;
                    //--- Store the Uncompressed Data (128*N-th samples)
                    if (comp_sample_id==0) {
                        num_bits = COMP_UNCOMP_NUM_BITS;
                        reverse_code = reverse_bits(/*bit_stream*/temp.val, /*num_bits*/COMP_UNCOMP_NUM_BITS);
                    }
                    //--- Calculate the Delta
                    else {
                        uint32_t comp_result = tcomp_encode(/*value*/temp.val - comp_prev_sample);
                        num_bits = comp_result >> 24;
                        reverse_code = comp_result&0xFFFFF;
                    }
                    //--- Store the current temperature in comp_prev_sample
                    comp_prev_sample = temp.val;

                    #ifdef DEVEL
                        mbus_write_message32(0x97, (num_bits<<24)|reverse_code);
                    #endif

                    // Find out the byte address of comp_bit_pos
                    uint32_t bit_pos_start = comp_bit_pos;
                    uint32_t bit_pos_end   = bit_pos_start + num_bits - 1;
                    uint32_t eeprom_avail  = (bit_pos_end < 64512); // NOTE: 64512 = (8192 - 128) * 8
                    uint32_t last_qword_id = eeprom_avail ? (bit_pos_end>>7) : 503;

                    // Store the Sample Count & Last Qword ID
                    eeprom_sample_cnt++;

                    buffer.sample_count = eeprom_sample_cnt;
                    buffer.last_qword_id = last_qword_id;

                    #ifdef DEVEL
                        mbus_write_message32(0xC8, (bit_pos_start<<16)|(bit_pos_end&0xFFFF));
                        mbus_write_message32(0xC9, (last_qword_id<<24)|(eeprom_sample_cnt&0xFFFFFF));
                    #endif

                    // MEMORY IS AVAILABLE
                    if (eeprom_avail) {

                        // CASE I ) Entire bits go into the same qword (bit_pos_start>>7 == bit_pos_end>>7)
                        if ((bit_pos_start>>7)==(bit_pos_end>>7)) {
                            uint32_t end_at_bndry = ((bit_pos_end&0x7F)==0x7F);

                            sub_qword(/*msb*/bit_pos_end&0x7F, /*lsb*/bit_pos_start&0x7F, /*value*/reverse_code);
                            buffer_update(  /*addr*/ ((bit_pos_start>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                            /*commit_crc*/ end_at_bndry
                                            );

                            if (end_at_bndry) {
                                buffer_commit();
                                set_qword(/*pattern*/0xFFFFFFFF);
                            }
                        }

                        // CASE II) Bits go across different qwords (bit_pos_start>>7 != bit_pos_end>>7)
                        else {
                            sub_qword(/*msb*/127, /*lsb*/bit_pos_start&0x7F, /*value*/reverse_code);
                            buffer_update(  /*addr*/ ((bit_pos_start>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                            /*commit_crc*/ 1
                                            );

                            buffer_commit();
                            set_qword(/*pattern*/0xFFFFFFFF);

                            sub_qword(/*msb*/bit_pos_end&0x7F, /*lsb*/0, /*value*/reverse_code>>(128-(bit_pos_start&0x7F)));
                            buffer_update(  /*addr*/ ((bit_pos_end>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                            /*commit_crc*/ 0    // Assuming that 'reverse_code' is less than 128 bits.
                                            );
                        }

                        comp_sample_id++;
                        if (comp_sample_id==COMP_SECTION_SIZE) comp_sample_id = 0;
                        comp_bit_pos = bit_pos_end + 1;

                        // Dump the buffer when a new alarm occurs
                        if (new_alarm) buffer_commit();
                    }
                    // MEMORY GETS FULL (Roll-over is disabled)
                    else if (comp_bit_pos != 65536) {
                        comp_bit_pos = 65536;
                        // Buffer Commit
                        buffer_commit();
                        // Update Memory Full Flag
                        status.memory_full = 1;
                        commit_status();
                    }

                }

                /////////////////////////////////////////////////////////////////
                // e-Ink Display
                //---------------------------------------------------------------
                // NOTE: 'Play' is common in all cases
                //
                //  Current     > High      Normal      < Low Threshold
                //  -------------------------------------------------------------
                //  None        Cross/+     Check       Cross/-
                //  Check       Cross/+     Check       Cross/-
                //  Cross/+     Cross/+     Cross/+     Cross/+/-
                //  Cross/-     Cross/+/-   Cross/-     Cross/-
                //  Cross/+/-   Cross/+/-   Cross/+/-   Cross/+/-
                //
                //---------------------------------------------------------------
                // NOTE: Actual update happens in 'Write into EEPROM/Display"

                // 'initialize' new_seg to play sign only. 
                // This is to reset new_seg with start_delay=0. Otherwise, new_seg may retain previous 'cross' mark or the broken sign (i.e., status.custom_init=0), if any.
                if (status.disp_skipped) {
                    curr_seg = DISP_PLAY;
                    new_seg = DISP_PLAY;
                    status.disp_skipped = 0;
                    #ifdef DEVEL
                        mbus_write_message32(0xBA, (0x11 << 24) | 0x0 /*status.disp_skipped*/);
                    #endif
                }

                //--- High Temperature Alarm
                if (tmp_exc.alarm_hi) {
                    new_seg &= ~DISP_CHECK;     // Remove Check
                    new_seg |= DISP_CROSS;      // Add Cross
                    new_seg |= DISP_PLUS;       // Add Plus
                }
                //--- Low Temperature Alarm
                else if (tmp_exc.alarm_lo) {
                    new_seg &= ~DISP_CHECK;     // Remove Check
                    new_seg |= DISP_CROSS;      // Add Cross
                    new_seg |= DISP_MINUS;      // Add Minus
                }
                //--- Normal Temperature
                else if (!get_bit(curr_seg, SEG_PLUS) && !get_bit(curr_seg, SEG_MINUS)) {
                    new_seg |= DISP_CHECK;      // Add Check
                }

            }
        }

        //--- Show Low VBATT Indicator (if needed)
        if (!get_bit(new_seg, SEG_LOWBATT) && meas.low_vbat) {
            new_seg = new_seg | DISP_LOWBATT;
        }

        /////////////////////////////////////////////////////////////////
        // Update/Refresh the e-Ink Display if needed

        //--- If display has been changed by DISPLAY
        if (get_bit(eid_disp_before_user_mod, 31)) eid_update_display(/*seg*/new_seg);
        //--- Update 
        else if (new_seg!=curr_seg) eid_update_display(/*seg*/new_seg);
        //--- Refresh
        else {
           //--- During Bootup, display the check mark to confirm that the bootup is done.
           if (!get_flag(FLAG_BOOTUP_DONE)) {
                // If correctly initialized, display Check Only
                if (status.custom_init)
                    eid_update_display(/*seg*/DISP_CHECK);
                // If using the default values, display 'backslash' and 'tick'
                else                           
                    eid_update_display(/*seg*/DISP_BACKSLASH|DISP_TICK);
                // Update the flag
                set_flag(FLAG_BOOTUP_DONE, 1);
           }
           else if (// Display has a pending update due to a critical temperature
                    (eid_updated_at_crit_temp && (temp.val > eid_crit_temp_threshold))
                    // Display has been updated at a low temperature
                 || (eid_updated_at_low_temp && (temp.val > eeprom_eid_threshold.low))
                    // User-configured Refresh interval
                 || ((disp_refresh_interval != 0) && ((disp_min_since_refresh + wakeup_interval) > disp_refresh_interval)) 
                 ){
               eid_update_display(/*seg*/eid_get_curr_seg());
           }
        }
    }
}

static void display_low_batt(void) {
    #ifdef DEVEL
        mbus_write_message32(0xA8, 0x00000000);
    #endif

    if (!get_bit(eid_get_current_display(), SEG_LOWBATT)) {
        eid_update_display(eid_get_current_display() | DISP_LOWBATT);
    }
    #ifdef DEVEL
    else {
        mbus_write_message32(0xA8, 0xFFFFFFFF);
    }
    #endif
}


//-------------------------------------------------------------------
// XO & SNT Timers
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: get_xo_cnt
// Args    : None
// Description:
//              Keep doing the asynchronous read.
//              Return the last value if two consecutive values are the same.
// Return  : XO counter value if no error
//           0 if #tries exceeds 5
//           0 if XO has not been initialized
//-------------------------------------------------------------------
static uint32_t get_xo_cnt(void) {
    if(get_flag(FLAG_XO_INITIALIZED)) {
        //----------------------------------------
        // GETTING THE SAME VALUES TWICE CONSECUTIVELY
        //----------------------------------------

        uint32_t temp = *XOT_VAL_ASYNC;
        uint32_t temp_prev;
        uint32_t num_same_reads = 1;
        uint32_t num_try = 0;

        while ((num_same_reads < 2) && (num_try<5)) {
            temp_prev = temp;
            temp = *XOT_VAL_ASYNC;
            if (temp==temp_prev) num_same_reads++;
            else num_same_reads=1;
            num_try++;
        }

        if (num_try >= 5) {
            calib_status |= XO_FAIL_UNSTABLE;
            return 0;
        }
        else {
            return temp;
        }
    }
    else {
        return 0;
    }
}

static void calibrate_snt_timer(uint32_t skip_calib) {

    uint32_t temp_xo_val;
    uint32_t temp_snt_freq;
    uint32_t fail = 0;
    uint32_t a = (0x1 << 28);   // Default: 1 (this is the ideal case; i.e., no error)

    // Make sure you have a valid temperature measurement
    meas_temp_adc(/*go_sleep*/0);

    // Fail check is done only if temp > -20C
    if (temp.val >= 600) {

        // Committ xo_val_curr_tmp
        xo_val_curr = xo_val_curr_tmp;

        // XO malfunction during sleep 
        if (xo_val_curr <= xo_val_prev) {
            calib_status |= XO_FAIL_STOP;
        }

        // FAIL: XO has failed before
        if (calib_status!=0) {
            fail = 1;
            #ifdef DEVEL
                mbus_write_message32(0x83, 0x0);
            #endif
        }
        // XO has been fine so far
        else {

            // Read the current counter value
            temp_xo_val = get_xo_cnt();

            #ifdef DEVEL
                mbus_write_message32(0x80, xo_val_prev);
                mbus_write_message32(0x81, xo_val_curr);
                mbus_write_message32(0x82, temp_xo_val);
            #endif

            // FAIL: XO is unstable (calib_status is updated within get_xo_cnt())
            if (calib_status!=0) {
                fail = 1;
                #ifdef DEVEL
                    mbus_write_message32(0x83, 0x1);
                #endif
            }
            // XO is running
            else {
        
                // FAIL: XO counter has not incremented
                if (temp_xo_val <= xo_val_curr) {
                    fail = 1;
                    calib_status |= XO_FAIL_STOP;
                    #ifdef DEVEL
                        mbus_write_message32(0x83, 0x2);
                    #endif
                }
                else {
                    if (skip_calib||MAIN_CALLED||get_flag(FLAG_INVLD_XO_PREV)) {
                        temp_snt_freq = snt_freq;
                        #ifdef DEVEL
                            mbus_write_message32(0x83, 0x3);
                        #endif
                    }
                    else {
                        #ifdef DEVEL
                            mbus_write_message32(0x83, 0x4);
                        #endif

                        // Calculate delta_xo
                        uint32_t delta_xo = xo_val_curr - xo_val_prev;

                        //uint32_t delta_xo;
                        //if (xo_val_curr > xo_val_prev) delta_xo = xo_val_curr - xo_val_prev;
                        //else delta_xo = xo_val_curr + (0xFFFFFFFF - xo_val_prev) + 1;

                        // Calculate 'A' (n=30 fixed-ratio)
                        a = mult_b28(/*num_a*/ div(/*numer*/snt_duration, /*denom*/delta_xo, /*n*/28),
                                     /*num_b*/ div(/*numer*/XO_FREQ,      /*denom*/snt_freq, /*n*/28)
                                    );

                        // Calculate the new SNT frequency
                        temp_snt_freq = mult_b28(/*num_a*/snt_freq, /*num_b*/a);

                        // 'A' must be close to 1. (Allowed Range: 0.75 < A < 1.25)
                        //      delta_xo + 25% results in a = 0.8       (0x0CCCCCCC with N=28)
                        //      delta_xo - 25% results in a = 1.3333333 (0x15555555 with N=28)
                        if ( (a>0x0CCCCCCC) && (a<0x15555555) ) {
                            // Update SNT frequency
                            snt_freq = temp_snt_freq;
                        }
                        // FAIL: Something is wrong if 'A' is not close to 1
                        else {
                            fail = 1;
                            calib_status |= XO_FAIL_MAX_ERR;
                            #ifdef DEVEL
                                mbus_write_message32(0x83, 0x5);
                            #endif
                        }

                        #ifdef DEVEL
                            mbus_write_message32(0x84, snt_duration);
                            mbus_write_message32(0x85, XO_FREQ);
                            mbus_write_message32(0x86, snt_freq);
                            mbus_write_message32(0x87, delta_xo);
                            mbus_write_message32(0x88, a);
                            mbus_write_message32(0x89, temp_snt_freq);
                        #endif
                    }
                }
            }
        }

    }

    // Get wakeup_interval in seconds
    uint32_t wakeup_interval_sec = (wakeup_interval << 6) - (wakeup_interval << 2);
    #ifdef DEVEL
        mbus_write_message32(0x8A, wakeup_interval_sec);
    #endif

    // Update the previous snt_threshold
    snt_threshold_prev = snt_threshold;

    // If there was a failure: Use previous numbers as they are.
    if (fail) {
        // Update the next duration
        snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq);

        #ifdef DEVEL
            mbus_write_message32(0x83, 0x6);
        #endif
        // Log the fail information in EEPROM
        if ((calib_status&XO_FAIL_MAX_ERR)!=0) {
            num_calib_max_err_fails++;
        }
        if (((calib_status&XO_FAIL_UNSTABLE)!=0)||((calib_status&XO_FAIL_STOP)!=0)) {
            num_calib_xo_fails++;
        }
        nfc_i2c_byte_write(/*e2*/0, 
                            /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, // EEPROM_ADDR_NUM_CALIB_XO_FAILS, EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS
                            /*data*/((num_calib_max_err_fails&0xFF)<<8)|(num_calib_xo_fails&0xFF), 
                            /*nb*/2);

        if (xt1_state==XT1_ACTIVE) {
            if (status.sample_type==SAMPLE_RAW) {
                if (sar_adc_addr < 8192) {
                    uint32_t data = ((((calib_status&XO_FAIL_UNSTABLE)!=0)||((calib_status&XO_FAIL_STOP)!=0)) << 15)
                                  | (((calib_status&XO_FAIL_MAX_ERR)!=0) << 14)
                                  | (sar_adc_data&0x3FFF);

                    nfc_i2c_byte_write( /*e2*/   0,
                                        /*addr*/ sar_adc_addr,
                                        /*data*/ data,
                                        /*nb*/   2);
                    #ifdef DEVEL
                        mbus_write_message32(0x9E, sar_adc_addr);
                        mbus_write_message32(0x9F, data);
                    #endif
                }
            }
        }

        // Restart the XO
        nfc_power_off(); // Turn off the NFC
        restart_xo(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

    #ifdef ENABLE_XO_PAD
        start_xo_cout();
    #endif

        // Calculate the next threshold
        if (skip_calib) {
            snt_threshold_prev = snt_read_wup_timer();
            snt_threshold      = snt_threshold_prev + snt_duration;
            #ifdef DEVEL
                mbus_write_message32(0xCA, snt_threshold);
            #endif
        }
        else {
            snt_threshold += snt_duration;
        }

        // Update xo_val_curr. 
        // You need this because the next wakeup compares the next xo_val_curr and this xo_val_curr, and restart_xo makes the xo counter 0.
        // Still, this xo_val_curr is invalid, and should NOT be used for calibration at the next wakeup.
        xo_val_curr = get_xo_cnt();
        set_flag(FLAG_INVLD_XO_PREV, 1);
    }
    // Skip Calibration
    else if (skip_calib) {
        #ifdef DEVEL
            mbus_write_message32(0x83, 0x8);
        #endif

        // Update the next duration
        snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq);

        // Calculate the next threshold
        snt_threshold_prev = snt_read_wup_timer();
        snt_threshold      = snt_threshold_prev + snt_duration;
        #ifdef DEVEL
            mbus_write_message32(0xCB, snt_threshold);
        #endif

        // Update xo_val_curr
        xo_val_curr = get_xo_cnt();
        set_flag(FLAG_INVLD_XO_PREV, 0);

        #ifdef DEVEL
            mbus_write_message32(0x81, xo_val_curr);
        #endif
    }
    // MAIN_CALLED
    else if (MAIN_CALLED||get_flag(FLAG_INVLD_XO_PREV)) {
        #ifdef DEVEL
            if (MAIN_CALLED) mbus_write_message32(0x83, 0xA);
            else             mbus_write_message32(0x83, 0xB);
        #endif

        // Update the next duration
        snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq);

        // Calculate the next threshold
        snt_threshold += snt_duration;

        if (MAIN_CALLED) 
            set_flag(FLAG_INVLD_XO_PREV, 1);
        else 
            set_flag(FLAG_INVLD_XO_PREV, 0);
    }
    // PASS
    else {

        #ifdef DEVEL
            mbus_write_message32(0x83, 0x7);
        #endif
        // Update next duration
        //--- if A >= 1
        if (get_bits(/*var*/a, /*msb*/31, /*lsb*/28)>=1) {
            snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq)
                         + mult_b28(/*num_a*/snt_duration, /*num_b*/a&0xEFFFFFFF);
        }
        //--- if A < 1
        else {
            snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq)
                            - mult_b28(/*num_a*/snt_duration, /*num_b*/(~a)&0x0FFFFFFF) 
                            - (snt_duration>>28);
        }

        // Calculate the next threshold
        snt_threshold += snt_duration;

    }

    if(snt_threshold==0) {
        #ifdef DEVEL
            mbus_write_message32(0x83, 0x9);
        #endif
        snt_threshold = 1; // SNT Timer does not expire at cnt=0.
    }

    #ifdef DEVEL
        mbus_write_message32(0x8B, snt_duration);
        mbus_write_message32(0x8D, snt_threshold_prev);
        mbus_write_message32(0x8C, snt_threshold);
    #endif
}

static uint32_t calc_ssls(void) {

    uint32_t snt_curr_val = snt_read_wup_timer();
    uint32_t snt_delta;
    uint32_t result;

    // CASE I) snt_curr_val > snt_threshold_prev
    if (snt_curr_val > snt_threshold_prev) {
        snt_delta = snt_curr_val - snt_threshold_prev;
    }
    // CASE II) snt_curr_val <= snt_threshold_prev
    else {
        // CASE II-A) Normal Counter Roll-Over i.e., (snt_threshold - snt_curr_val) is very large
        if (snt_threshold_prev - snt_curr_val > (snt_freq << 18)) { // Corresponds to ~73hrs; See operation_sleep().
            //snt_delta = snt_curr_val + (0xFFFFFFFF - snt_threshold_prev) + 1;
            snt_delta = snt_curr_val - snt_threshold_prev;
        }
        // CASE II-B) Sleep before the previous wakeup was bypassed i.e., (snt_threshold - snt_curr_val) is very small, like, < 10 seconds
        else {
            // Report SSLS=0
            snt_delta = 0;
        }
    }

    result = (sub_sample_cnt << 6) - (sub_sample_cnt << 2)                    // Full-minutes (converted to seconds) from the last 'sample'
             + (div(/*numer*/snt_delta, /*denom*/snt_freq, /*n*/0)&0xFFFF);  // Seconds from the last 'wake-up'

    #ifdef DEVEL
        mbus_write_message32(0xCC, snt_threshold_prev);
        mbus_write_message32(0xCD, snt_curr_val);
        mbus_write_message32(0xCE, snt_delta);
        mbus_write_message32(0xCF, result);
    #endif

    return result;
}

//-------------------------------------------------------------------
// E-Ink Display
//-------------------------------------------------------------------

static uint32_t eid_get_curr_seg(void) {
    if (get_bit(eid_disp_before_user_mod, 31))
        return eid_disp_before_user_mod&0x1FF;
    else
        return eid_get_current_display();
}

static void eid_update_configs(void) {
    uint32_t new_val;
    eid_update_type = EID_GLOBAL;
    if (temp.val > eeprom_eid_threshold.high) {
        new_val = eeprom_eid_rfrsh_int_hr.high;
        eid_duration = eeprom_eid_duration.high;
    }
    else if (temp.val > eeprom_eid_threshold.low) { 
        new_val = eeprom_eid_rfrsh_int_hr.mid;
        eid_duration = eeprom_eid_duration.mid;
    }
    else {
        new_val = eeprom_eid_rfrsh_int_hr.low;
        eid_duration = eeprom_eid_duration.low;
        if (!eeprom_misc_config.eid_disable_local_update)
            eid_update_type = EID_LOCAL;
    }

    // Set the refresh interval
    #ifdef USE_SHORT_REFRESH
        disp_refresh_interval = new_val; // Dummy line to avoid the warning 'new_val set but not used'
        //disp_refresh_interval = (WAKEUP_INTERVAL_IDLE<<1);
        disp_refresh_interval = WAKEUP_INTERVAL_IDLE;
    #else
        disp_refresh_interval = (new_val << 6) - (new_val << 2); // convert hr to min
    #endif

    // Set the duration
    eid_set_duration(eid_duration);

    #ifdef DEVEL
        mbus_write_message32(0xA2, (eid_duration<<16)|disp_refresh_interval);
    #endif
}

static void eid_update_display(uint32_t seg) {

    // Make sure you have a valid temperature measurement
    meas_temp_adc(/*go_sleep*/0);

    if (temp.val > 600) { 

        // Enable low-power active
        set_high_power_history(1);

        // Turn off NFC, if on, to save power
        nfc_power_off();

        // Get the current display
        uint32_t prev_seg = eid_get_current_display();

        // E-ink Update
        // Update the flag if it is a critical temperature
        if (temp.val > eid_crit_temp_threshold) {
            if (eid_update_type == EID_GLOBAL) {
                #ifdef DEVEL
                    mbus_write_message32(0xA7, 0x0);
                #endif
                eid_update_global(seg);
            }
            else { 
                #ifdef DEVEL
                    mbus_write_message32(0xA7, 0x1);
                #endif
                eid_update_local(seg);
            }
            eid_updated_at_crit_temp = 0;
        }
        else {
            __eid_current_display__ = seg;
            eid_updated_at_crit_temp = 1;
            #ifdef DEVEL
                mbus_write_message32(0xA3, 0x00000000);
            #endif
        }

        // Update the flag if it is a low temperature
        if (temp.val > eeprom_eid_threshold.low)
            eid_updated_at_low_temp = 0;
        else
            eid_updated_at_low_temp = 1;

        // Reset the refresh counter
        disp_min_since_refresh = 0;

        #ifdef DEVEL
            mbus_write_message32(0xA5, eid_updated_at_low_temp);
            mbus_write_message32(0xA6, eid_updated_at_crit_temp);
        #endif

        // Reset eid_disp_before_user_mod if needed
        if (cmd!=CMD_DISPLAY) eid_disp_before_user_mod = 0;

        // Update EEPROM if it is different from the previous status (prev_seg)
        if (seg != prev_seg) {
            // Try up to 30 times - Consistency between the EEPROM log and the actual display is important!
            if(nfc_i2c_get_token(/*num_try*/30)) {
                // EEPROM Update
                nfc_i2c_byte_write(/*e2*/0,
                    /*addr*/ EEPROM_ADDR_DISPLAY,
                    /*data*/ seg,
                    /* nb */ 1
                );
                #ifdef DEVEL
                    mbus_write_message32(0xA0, 
                        (0x0            <<28) |
                        (get_bit(seg, 6)<<24) |
                        (get_bit(seg, 5)<<20) |
                        (get_bit(seg, 4)<<16) |
                        (get_bit(seg, 3)<<12) |
                        (get_bit(seg, 2)<< 8) |
                        (get_bit(seg, 1)<< 4) |
                        (get_bit(seg, 0)<< 0)
                    );
                #endif
            }
            #ifdef DEVEL
            else {
                mbus_write_message32(0xA0, 
                    (0x2            <<28) |
                    (get_bit(seg, 6)<<24) |
                    (get_bit(seg, 5)<<20) |
                    (get_bit(seg, 4)<<16) |
                    (get_bit(seg, 3)<<12) |
                    (get_bit(seg, 2)<< 8) |
                    (get_bit(seg, 1)<< 4) |
                    (get_bit(seg, 0)<< 0)
                );
            }
            #endif
        }
        else {
            #ifdef DEVEL
                mbus_write_message32(0xA0, 
                    (0x1            <<28) |
                    (get_bit(seg, 6)<<24) |
                    (get_bit(seg, 5)<<20) |
                    (get_bit(seg, 4)<<16) |
                    (get_bit(seg, 3)<<12) |
                    (get_bit(seg, 2)<< 8) |
                    (get_bit(seg, 1)<< 4) |
                    (get_bit(seg, 0)<< 0)
                );
            #endif
        }
    }
}

static void eid_blink_display(void) {
    uint32_t curr_disp = eid_get_current_display();
    eid_update_display(curr_disp);
    #ifdef DEVEL
        mbus_write_message32(0xA0, (0xF<<28) | (curr_disp&0xFFFFFF));
    #endif
}


//-------------------------------------------------------------------
// NFC
//-------------------------------------------------------------------

uint32_t nfc_set_ack(uint32_t ack) {
    uint32_t cmd_raw;
    // ACK or ERR
    if (ack) cmd_raw = (0x1<<6)|cmd;
    else     cmd_raw = (0x1<<5)|cmd;

    // Checksum
    uint32_t cmd_chksum = (cmd_raw+cmd_param)&0xFF;

    uint32_t num_try=10;
    while (num_try!=0) {
        nfc_i2c_start();
        if (nfc_i2c_byte_imm(0xA6)) {
            nfc_i2c_byte(EEPROM_ADDR_CMD >> 8);
            nfc_i2c_byte(EEPROM_ADDR_CMD);
            nfc_i2c_byte(cmd_raw);
            nfc_i2c_byte(cmd_param);
            nfc_i2c_byte(cmd_chksum);
            nfc_i2c_stop();
            nfc_i2c_polling();
            return 1;
        }
        nfc_i2c_stop();
        num_try--;
    }
    return 0;
}

uint32_t nfc_set_ack_with_data(uint32_t data) {
    data = (data<<16)|(cmd_param<<8)|(0x1<<6)|cmd;
    uint32_t num_try=10;
    while (num_try!=0) {
        nfc_i2c_start();
        if (nfc_i2c_byte_imm(0xA6)) {
            nfc_i2c_byte(EEPROM_ADDR_CMD >> 8);
            nfc_i2c_byte(EEPROM_ADDR_CMD);
            nfc_i2c_byte((data>> 0)&0xFF);  // Byte 92
            nfc_i2c_byte((data>> 8)&0xFF);  // Byte 93
            nfc_i2c_byte((data>>16)&0xFF);  // Byte 94
            nfc_i2c_byte((data>>24)&0xFF);  // Byte 95
            nfc_i2c_stop();
            nfc_i2c_polling();
            #ifdef DEVEL
                mbus_write_message32(0xB2, data);
            #endif
            return 1;
        }
        nfc_i2c_stop();
        num_try--;
    }
    #ifdef DEVEL
        mbus_write_message32(0xB3, data);
    #endif
    return 0;
}

uint32_t nfc_check_cmd(void) {

    //---------------------------------------------------------------
    // Read the command from EEPROM
    //---------------------------------------------------------------
    uint32_t cmd_raw;
    uint32_t cmd_chksum;
    uint32_t num_try=10;
    while (num_try!=0) {
        nfc_i2c_start();
        if (nfc_i2c_byte_imm(0xA6)) {
            nfc_i2c_byte(EEPROM_ADDR_CMD >> 8);
            nfc_i2c_byte(EEPROM_ADDR_CMD);
            nfc_i2c_start();
            nfc_i2c_byte(0xA7);
            cmd_raw    = nfc_i2c_rd(1); // CMD
            cmd_param  = nfc_i2c_rd(1); // CMD_PARAM
            cmd_chksum = nfc_i2c_rd(0); // CMD_CHKSUM
            nfc_i2c_stop();
            break;
        }
        nfc_i2c_stop();
        num_try--;
    }

    //--- Command Read Fail: Return with 0xDEAD
    if (num_try==0) return 0xDEAD;

    // REQ=cmd_raw[7], ACK=cmd_raw[6], ERR=cmd_raw[5], CMD=cmd_raw[4:0]
    cmd = cmd_raw&0x1F;

    //--- Check Checksum
    if (cmd_chksum == ((cmd_raw+cmd_param)&0xFF)) {

        //--- Invalid Handshaking (REQ=0 or ACK=1 or ERR=1)
        if ((cmd_raw&0xE0)!=0x80) return 0xDEAD;

        //---------------------------------------------------------------
        // Normal Handshaking (REQ=1 AND ACK=0 AND ERR=0)
        //---------------------------------------------------------------

        #ifdef DEVEL
            mbus_write_message32(0xB1, (cmd_param<<16)|cmd);
        #endif

        //-----------------------------------------------------------
        // CMD: SOFT_RESET
        //-----------------------------------------------------------
        if (cmd == CMD_SRESET) {
            nfc_set_ack(/*ack*/ACK);
            return XT1_RESET;
        }
        //-----------------------------------------------------------
        // CMD: START
        //-----------------------------------------------------------
        else if (cmd == CMD_START) {
            status.sample_type = get_bit(cmd_param, 1); // will be committed in set_system()

            // START Successful
            if (// Raw Sample
                (status.sample_type==SAMPLE_RAW)
                // Normal Sample requires both AES KEY and CONFIG to be valid
                || ( status.aes_key_set && status.config_set )
                ) {

                nfc_set_ack(/*ack*/ACK);
                return XT1_PEND;
            }

            // START Fail
            // Normale Sample requested, but AES key is not set -OR- CONFIG has not executed yet
            else {
                nfc_set_ack(/*ack*/ERR);
                return 0xDEAD;
            }

        }
        //-----------------------------------------------------------
        // CMD: STOP
        //-----------------------------------------------------------
        else if (cmd == CMD_STOP) {

            // STOP Successful
            if (((xt1_state==XT1_PEND) || (xt1_state==XT1_ACTIVE)) && (      // Must be in PEND or ACTIVE state - AND
                    (status.sample_type==SAMPLE_RAW) || (status.aes_key_set) // Must be the Raw Sample -OR- AES KEY is set
                )) {
                // Dump pending data, if any.
                if (status.sample_type==SAMPLE_NORMAL) 
                    buffer_commit();
                // SSLS and ACK
                ssls = calc_ssls();
                nfc_set_ack_with_data(/*data*/ssls);

                return XT1_IDLE;
            }

            // STOP Fail
            else {
                nfc_set_ack(/*ack*/ERR);
                return 0xDEAD;
            }
        }
        //-----------------------------------------------------------
        // CMD: GETSEC
        //-----------------------------------------------------------
        else if (cmd == CMD_GETSEC) {
            // Unread_sample flag
            if (status.unread_sample) {
                status.unread_sample = 0;
                commit_status();
            }

            // Unit is currently running
            if ((xt1_state==XT1_PEND) || (xt1_state==XT1_ACTIVE)) {
                // Dump pending data, if any.
                buffer_commit();
                // SSLS and ACK
                nfc_set_ack_with_data(/*data*/calc_ssls());
            }
            // Unit has been stopped
            else 
                nfc_set_ack_with_data(/*data*/ssls);

            return 0xF00D;
        }
        //-----------------------------------------------------------
        // CMD: NEWKEY
        //-----------------------------------------------------------
        else if (cmd == CMD_NEWKEY) {
            // Key has not been set
            if (!status.aes_key_set) {
                nfc_set_ack(/*ack*/ACK);
                return 0xF00D;
            }
            // Key has been already set
            else {
                nfc_set_ack(/*ack*/ERR);
                return 0xDEAD;
            }
        }
        //-----------------------------------------------------------
        // CMD: CONFIG
        //-----------------------------------------------------------
        else if (cmd == CMD_CONFIG) {

            // CONFIG Successful
            if (status.aes_key_set) {
                nfc_set_ack(/*ack*/ACK);
                return 0xF00D;
            }
            // CONFIG Fail
            else {
                nfc_set_ack(/*ack*/ERR);
                return 0xDEAD;
            }
        }
        //-----------------------------------------------------------
        // CMD: NOP, HARD_RESET, DISPLAY
        //-----------------------------------------------------------
        else if (  (cmd == CMD_NOP)
                || (cmd == CMD_NEWAB)
                || (cmd == CMD_HRESET)
                || (cmd == CMD_DISPLAY)
                || (cmd == CMD_DEBUG)) {
            nfc_set_ack(/*ack*/ACK); 
            return 0xF00D;
        } 
        //-----------------------------------------------------------
        // CMD: Invalid
        //-----------------------------------------------------------
        else { 
            nfc_set_ack(/*ack*/ERR); 
            return 0xDEAD;
        }


    }
    // Checksum Error
    else {
        nfc_set_ack(/*ack*/ERR); 
        return 0xDEAD;
    }

}


//-------------------------------------------------------------------
// EEPROM Variables
//-------------------------------------------------------------------

static void update_system_configs(uint32_t use_default) {

    uint32_t init_status;

    // Check INIT_STATUS
    if (use_default==0) {
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_INIT_STATUS, /*nw*/1, /*ptr*/&init_status);
        init_status &= 0xFFFF;
        if ((init_status!=INIT_TRIG_ALL) && (init_status!=INIT_TRIG_NOCAL))
            use_default = 1;
    }

    // Use Default values
    if (use_default) {

        #ifdef DEVEL
            mbus_write_message32(0x73, 0x1);
        #endif

        //--- VBAT and ADC
        //eeprom_adc_th.t* = 0;     // t1 ~ t4
        eeprom_adc_th.value = 0;

        //eeprom_adc.offset_r* = 0; // r1 ~ r5
        //eeprom_adc.low_vbat  = 105;
        //eeprom_adc.crit_vbat = 95;
        eeprom_adc.value = 0x005F690000000000;

        //--- EID Configurations 
        //eeprom_eid_threshold.high = 950; // 15C
        //eeprom_eid_threshold.low  = 750; // -5C
        eeprom_eid_threshold.value = (750 << 16) | (950 << 0);

        //eeprom_eid_duration.high        = 60 ; // 0.5s
        //eeprom_eid_duration.mid         = 500; // 4s
        //eeprom_eid_duration.low         = 500; // 4s
        eeprom_eid_duration.value = 0x000001F401F4003C;

        //eeprom_eid_rfrsh_int_hr.high    = 12 ;
        //eeprom_eid_rfrsh_int_hr.mid     = 24 ;
        //eeprom_eid_rfrsh_int_hr.low     = 0  ;
        eeprom_eid_rfrsh_int_hr.value = (0 << 16) | (24 << 8) | (12 << 0);

        //--- Other Configurations
        //eeprom_misc_config.ignore_first_sample_exc  =  1; // [8]
        //eeprom_misc_config.pmu_adc_sar_hysteresis   =  1; // [7]
        //eeprom_misc_config.pmu_adc_sar_margin       =  0; // [6]
        //eeprom_misc_config.eid_disable_local_update =  1; // [5]
        //eeprom_misc_config.eid_crit_temp_threshold  =  0; // [4]
        //eeprom_misc_config.pmu_num_cons_meas        =  5; // [3:0]
        eeprom_misc_config.value = (1 << 8) | (1 << 7) | (0 << 6) | (1 << 5) | (0 << 4) | (5 << 0);

        // Update the flag
        status.custom_init = 0;
        #ifdef DEVEL
            mbus_write_message32(0xBA, (0x12 << 24) | 0x0 /*status.custom_init*/);
        #endif
    }

    // Get the real values from EEPROM
    else {

        #ifdef DEVEL
            mbus_write_message32(0x73, 0x2);
        #endif

        //--- VBAT and ADC
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T1,        /*nw*/2, /*ptr*/ (volatile uint32_t *)&eeprom_adc_th.value);
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R1, /*nw*/2, /*ptr*/ (volatile uint32_t *)&eeprom_adc.value);

        //--- EID Configurations
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD, /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_eid_threshold.value);
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_HIGH,       /*nw*/2, /*ptr*/ (volatile uint32_t *)&eeprom_eid_duration.value);
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_RFRSH_INT_HR_HIGH,   /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_eid_rfrsh_int_hr.value);

        //--- Other Configurations
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_MISC_CONFIG,   /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_misc_config.value);

        //--- Update a and b if needed.
        if (init_status == INIT_TRIG_ALL) update_temp_coefficients();

        // Reset INIT_STATUS
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_INIT_STATUS, /*data*/INIT_RESET, /*nb*/2);

        // Update the flag
        status.custom_init = 1;
        #ifdef DEVEL
            mbus_write_message32(0xBA, (0x12 << 24) | 0x1 /*status.custom_init*/);
        #endif
    }

    // EID_CRIT_TEMP_THRESHOLD Configuration using 10x(T+80)
    eid_crit_temp_threshold = eeprom_misc_config.eid_crit_temp_threshold ?
                                600 :   // 0x1: -20C
                                550 ;   // 0x0: -25C
}

static void update_temp_coefficients (void) {
    // Read the temperature coefficients
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_A_WRITEONLY, /*nw*/2, /*ptr*/ (volatile uint32_t *)&eeprom_temp_calib.value);

    // Write the calibration info in the designated place
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_A, /*data*/eeprom_temp_calib.a, /*nb*/4);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_B, /*data*/eeprom_temp_calib.b, /*nb*/4);

    #ifdef DEVEL
        mbus_write_message32(0x76, eeprom_temp_calib.a);
        mbus_write_message32(0x77, eeprom_temp_calib.b);
    #endif
}

#ifdef ENABLE_DEBUG_SYSTEM_CONFIG
static void debug_system_configs(void) {

    #ifdef DEVEL
        mbus_write_message32(0x73, 0xF);
    #endif

    //--- VBAT and ADC
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T1,
        /*data*/  eeprom_adc_th.lower,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T3,
        /*data*/  eeprom_adc_th.upper,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R1,
        /*data*/  eeprom_adc.lower,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R5,
        /*data*/  eeprom_adc.upper,
        /* nb */  3);

    //--- EID Configurations
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD,
        /*data*/  eeprom_eid_threshold.value,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_HIGH,
        /*data*/  eeprom_eid_duration.lower,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_LOW,
        /*data*/  eeprom_eid_duration.upper,
        /* nb */  2);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_EID_RFRSH_INT_HR_HIGH,
        /*data*/  eeprom_eid_rfrsh_int_hr.value,
        /* nb */  3);

    //--- Other Configurations
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_MISC_CONFIG,
        /*data*/  eeprom_misc_config.value,
        /* nb */  2);

    //--- Temperature Coefficients
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_A_WRITEONLY, 
        /*data*/  eeprom_temp_calib.a, 
        /* nb */  4);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_B_WRITEONLY, 
        /*data*/  eeprom_temp_calib.b, 
        /* nb */  4);

    // Reset INIT_STATUS
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_INIT_STATUS, /*data*/INIT_DEBUG_READ, /*nb*/2);

}
#endif

static void update_user_configs(void) {
    #ifdef DEVEL
        mbus_write_message32(0x73, 0x3);
    #endif

    // Read the user configuration
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_TEMP_THRESHOLD, /*nw*/2, /*ptr*/ (volatile uint32_t *)&eeprom_user_config.value);
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CONS_EXCURSIONS, /*nw*/1, /*ptr*/ &num_cons_excursions);
    num_cons_excursions &= 0xFFFF;  // num_cons_excursions is 2-byte-long

    // Set Flag
    status.config_set = 1;
    commit_status();

    #ifdef DEVEL
        mbus_write_message32(0x74, eeprom_user_config.lower);
        mbus_write_message32(0x75, eeprom_user_config.upper);
        mbus_write_message32(0x7C, num_cons_excursions);
    #endif

}

static void update_aes_key(void) {
    #ifdef DEVEL
        mbus_write_message32(0x73, 0x5);
    #endif

    // Read the key and store in aes_key
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_AES_KEY, /*nw*/4, /*ptr*/aes_key);

    // Erase the key in EEPROM
    nfc_i2c_word_pattern_write(/*e2*/0, /*addr*/EEPROM_ADDR_AES_KEY, /*data*/0x0, /*nw*/4);

    // Set Flag
    status.aes_key_set = 1;

    #ifdef DEVEL
        mbus_write_message32(0xBA, (0x10 << 24) | status.aes_key_set);
        mbus_write_message32(0x78, aes_key[0]);
        mbus_write_message32(0x79, aes_key[1]);
        mbus_write_message32(0x7A, aes_key[2]);
        mbus_write_message32(0x7B, aes_key[3]);
    #endif
}


//-------------------------------------------------------------------
// Write Buffer
//-------------------------------------------------------------------

static void buffer_init(void) {
    buffer_valid = 0;
}

static void buffer_update (uint32_t addr, uint32_t commit_crc) {

    uint32_t text[4];
    uint32_t i;

    // Make a clone
    for (i=0; i<4; i++) {
        *(text+i) = *(qword+i);
        #ifdef DEVEL
            mbus_write_message32(0xB4, *(text+i));
        #endif
    }

    // Encrypt
    aes_encrypt(/*pt*/text);

    // Save into the Buffer
    buffer.eeprom_addr = addr;
    #ifdef DEVEL
        mbus_write_message32(0xB5, addr);
    #endif
    for(i=0; i<4; i++) {
        buffer_data[i] = text[i];
        #ifdef DEVEL
            mbus_write_message32(0xB6, text[i]);
        #endif
    }

    // Calculate CRC
    buffer_crc32 = calc_crc32_128(/*src*/text, /*crc_prev*/crc32);
    #ifdef DEVEL
        mbus_write_message32(0xB7, buffer_crc32);
    #endif

    // Commit CRC
    if (commit_crc) crc32 = buffer_crc32;

    // Set the Buffer Valid bit
    buffer_valid = 1;

    // Update unread_sample status
    if (!status.unread_sample) {
        status.unread_sample = 1;
        commit_status();
    }
}

static void buffer_commit(void) {

    // Sample Count
    nfc_i2c_byte_write(/*e2*/ 0, /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, /*data*/ buffer.count, /*nb*/ 4);
    // Last ADC/SAR
    nfc_i2c_byte_write(/*e2*/ 0, /*addr*/ EEPROM_ADDR_LAST_ADC,     /*data*/ buffer.pmu,   /*nb*/ 2);

    if (buffer_valid) {
        // Data
        nfc_i2c_word_write(/*e2*/ 0, /*addr*/ buffer.eeprom_addr,       /*data*/ (uint32_t *) buffer_data,  /*nw*/ 4);
        // CRC
        nfc_i2c_byte_write(/*e2*/ 0, /*addr*/ EEPROM_ADDR_CRC,          /*data*/ buffer_crc32, /*nb*/ 4);
        // Now the buffer is empty
        buffer_valid = 0;
        #ifdef DEVEL
            mbus_write_message32(0xB8, 0x1);
        #endif
    }
    #ifdef DEVEL
    else {
        mbus_write_message32(0xB8, 0x0);
    }
    #endif

    return;
}

//-------------------------------------------------------------------
// Data Compression
//-------------------------------------------------------------------

//-----------------------------------------------
// HUFFMAN TABLE
//-----------------------------------------------
//  value       Encoded Bit             Num Bits 
//  Range       Stream                  Used
//-----------------------------------------------
//  -2047~-9    {10000, |value|&0x7FF}  16       
//  -8          10001                   5        
//  -7          10010                   5        
//  -6          10011                   5        
//  -5          10100                   5        
//  -4          10101                   5        
//  -3          10110                   5        
//  -2          10111                   5        
//  -1          010                     3        
//  0           00                      2        
//  +1          011                     3        
//  +2          11000                   5        
//  +3          11001                   5        
//  +4          11010                   5        
//  +5          11011                   5        
//  +6          11100                   5        
//  +7          11101                   5        
//  +8          11110                   5        
//  +9~+2047    {11111, value&0x7FF}    16       
//-----------------------------------------------

// return value:
// [31:24] Number of valid bits in the Encoded code
// [17: 0] Encoded code (bit reversed)

uint32_t tcomp_encode (uint32_t value) {
    uint32_t num_bits;
    uint32_t code;

    uint32_t sign = (value >> 31);  // 0: positive, 1: negative
    // Make 'value' an absolute number
    if (sign) value = (0xFFFFFFFF - value) + 1;

    // value=0: code=00
    if (value == 0) {
        num_bits = 2; code = 0x0;
    }
    // value=1: code=010(-) or 011(+)
    else if (value == 1) {
        num_bits = 3; code = sign ? 0x2 : 0x3;
    }
    // value=2~8:
    else if (value < 9) {
        num_bits = 5; 
        code = 0x8 | (value - 2);
        if (sign) code = ~code; // Bit negation for negative numbers.
        else      code |= 0x10; // For positive numbers
    }
    // value > 8
    else {
        num_bits = 16;
        if (sign) code = (0x10 << 11) | (value & 0x7FF);
        else      code = (0x1F << 11) | (value & 0x7FF);
    }

    return (num_bits<<24) 
            | reverse_bits(/*bit_stream*/code, /*num_bits*/num_bits);
}

uint32_t reverse_bits (uint32_t bit_stream, uint32_t num_bits) {
    uint32_t rev_bit_stream = 0;
    uint32_t idx;
    for (idx=0; idx<num_bits; idx++) {
        rev_bit_stream <<= 1;
        rev_bit_stream |= (bit_stream&0x1);
        bit_stream >>= 1;
    }
    return rev_bit_stream;
}


//-------------------------------------------------------------------
// 128-bit qword Handling
//-------------------------------------------------------------------

void set_qword (uint32_t pattern) {
    uint32_t i;
    for (i=0; i<4; i++) *(qword+i) = pattern;
}

void sub_qword (uint32_t msb, uint32_t lsb, uint32_t value) {
// Valid Range: msb= 0 - 127
//              lsb= 0 - 127
//
    // CASE I) msb and lsb belong to the same word
    if ((msb>>5)==(lsb>>5)) {
        *(qword+(lsb>>5)) = set_bits(/*var*/*(qword+(lsb>>5)), /*msb_idx*/msb&0x1F, /*lsb_idx*/lsb&0x1F, /*value*/value);
    }
    // CASE II) msb and lsb belong to the different word
    else {
        *(qword+(lsb>>5)) = set_bits(/*var*/*(qword+(lsb>>5)), /*msb_idx*/31, /*lsb_idx*/lsb&0x1F, /*value*/value);
        *(qword+(msb>>5)) = set_bits(/*var*/*(qword+(msb>>5)), /*msb_idx*/msb&0x1F, /*lsb_idx*/0, /*value*/value>>(32-(lsb&0x1F)));
    }
}

//-------------------------------------------------------------------
// AES & CRC
//-------------------------------------------------------------------

void aes_encrypt (uint32_t* pt) {

    // Enable AES IRQ
    *NVIC_ISER = (0x1 << IRQ_AES);

    uint32_t i;

    // Set the key if needed
    if (!aes_key_valid) {
        for (i=0; i<4; i++) {
            *(AES_KEY_0+i) = *(aes_key+i);
        }
        aes_key_valid = 1;
    }

    // Set Plain Text
    for (i=0; i<4; i++) {
        *(AES_TEXT_0+i) = *(pt+i);
    }

    // Perform AES
    *AES_GO = 1;
    WFI();

    // Update
    for (i=0; i<4; i++) {
        *(pt+i) = *(AES_TEXT_0+i);
    }

}

void aes_encrypt_eeprom(uint32_t addr) {

    uint32_t i;
    uint32_t text[4];

    // Read the raw data
    nfc_i2c_word_read(/*e2*/0, /*addr*/addr, /*nw*/4, /*ptr*/text);

    // Encrypt
    aes_encrypt(/*pt*/text);

    // Write the encrypted data back into EEPROM
    for (i=0; i<4; i++)
        nfc_i2c_byte_write(/*e2*/0, /*addr*/addr+(i<<2), /*data*/*(text+i), /*nb*/4);

}

uint32_t calc_crc32_128(uint32_t* src, uint32_t crc_prev) {
    uint32_t a = crc_prev&0xFFFF;
    uint32_t b = crc_prev>>16;
    uint32_t word, i, j;

    for(i=0; i<4; i++) {
        word = *(src+i);
        for(j=0; j<4; j++) {
            a += (word&0xFF); while(a > 65520) a -= 65521;
            b += a;           while(b > 65520) b -= 65521;
            word >>= 8;
        }
    }

    return (b<<16)|a;
}

//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

void disable_all_irq_except_timer32(void) {
    *NVIC_ICPR = 0xFFFFFFFF;
    *NVIC_ICER = ~(1 << IRQ_TIMER32);
}

void fail_handler(uint32_t id) {
//    uint32_t commit_result = get_flag(FLAG_INITIALIZED);
//
//    if (id==FAIL_ID_INIT) {
//        #ifdef DEVEL
//            mbus_write_message32(0xEF, FAIL_ID_INIT);
//        #endif
//        id = status.fail_id;
//        commit_result = 1;
//    }
//
//    #ifdef DEVEL
//        mbus_write_message32(0xEF, id);
//    #endif
//    uint32_t disp_pattern;
//    status.fail_id = id;
//
//    // Timeout during display update
//    if (id==FAIL_ID_EID) {
//        disp_pattern = 0;   // No display update if the timeout is from EID
//    }
//    // Timeout during SNT Timer read
//    else if (id==FAIL_ID_WUP) {
//        disp_pattern = DISP_BACKSLASH|DISP_MINUS;
//    }
//    // Timeout during PMU register access
//    else if (id==FAIL_ID_PMU) {
//        disp_pattern = DISP_BACKSLASH|DISP_PLAY;
//    }
//    // Timeout during SNT temperature measurement
//    else if (id==FAIL_ID_SNT) {
//        disp_pattern = DISP_BACKSLASH|DISP_PLUS;
//    }
//    // I2C NAK Failure
//    else if (id==FAIL_ID_I2C) {
//        disp_pattern = DISP_BACKSLASH;
//    }
//    // Meta-Stability (SNT Timer Reading)
//    else if (id==FAIL_ID_MET) {
//        disp_pattern = DISP_BACKSLASH|DISP_PLUS|DISP_MINUS;
//    }
//    // Generic/Unknown Timeout
//    //  NOTE: It may indicate that the SNT Threshold Update has failed.
//    else {
//        #ifdef DEVEL
//            mbus_write_message32(0xEF, 0x00000000);
//        #endif
//        disp_pattern = DISP_BACKSLASH|DISP_PLUS|DISP_MINUS|DISP_PLAY;
//    }
//
//    if (commit_result) {
//        // Commit the Status
//        if (id!=FAIL_ID_I2C) commit_status();
//
//        // Display
//        if (id!=FAIL_ID_EID) eid_update_display(/*seg*/disp_pattern);
//
//        // Turn off everything; Stay in sleep indefinitely.
        operation_dead();
//    }
}

//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_wfi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

// TIMER32 - Timeout Check
void handler_ext_int_timer32 (void) {
    disable_all_irq_except_timer32();
    *TIMER32_STAT = 0x0;
    *TIMER32_GO = 0x0;
    fail_handler(/*id*/__wfi_id__);
}
void handler_ext_int_wfi (void) {
    // REG0: PMU
    // REG1: SNT (Temperature Sensor)
    // REG2: EID Display Update
    // REG5: SNT (Wakeup Timer)
    // AES : AES
    disable_all_irq_except_timer32();
    *TIMER32_GO = 0;
}
//// PMU (PMU_TARGET_REG_IDX)
//void handler_ext_int_reg0 (void) {
//    disable_all_irq_except_timer32();
//}
//// SNT (Temperature Sensor) (SNT_TARGET_REG_IDX)
//void handler_ext_int_reg1 (void) {
//    disable_all_irq_except_timer32();
//}
////  EID (EID_TARGET_REG_IDX)
//void handler_ext_int_reg2 (void) {
//    disable_all_irq_except_timer32();
//}
//// SNT (Timer Access) (WP1_TARGET_REG_IDX)
//void handler_ext_int_reg5 (void) {
//    disable_all_irq_except_timer32();
//}
// I2C ACK Failure Handling (I2C_TARGET_REG_IDX)
void handler_ext_int_reg7 (void) {
    disable_all_irq_except_timer32();
    *TIMER32_GO = 0;
    __wfi_id__ = FAIL_ID_I2C;
    fail_handler(/*id*/__wfi_id__);
}
//// AES
//void handler_ext_int_aes (void) { 
//    disable_all_irq_except_timer32();
//}


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

    // Set PRE Watchdog
    *TIMERWD_GO  = 0;
    *TIMERWD_CNT = 16800000; // ~120s @ 140kHz
    *TIMERWD_GO  = 1;

    // Disable MBus Watchdog
    *REG_MBUS_WD = 0;

    // Reset variables
    set_high_power_history(0);
    
    // Enable IRQs for timeout check
    *NVIC_IPR0 = (0x3 << 30); // Lowest Priority for TIMER32 IRQ.
    *NVIC_IPR3 = (0x3 << 30); // Lowest Priority for REG7 IRQ.
    *NVIC_ISER = (0x1 << IRQ_TIMER32);

    if (!get_flag(FLAG_TEMP_MEAS_PEND)) {

        // Read the XO value for the calibration later
        snt_skip_calib  = 0;    // By default, it does the normal calibration
        calib_status    = 0;    // Reset Calibration status
        xo_val_prev     = xo_val_curr;
        xo_val_curr_tmp = get_xo_cnt(); // It returns 0 if XO has not been initialized yet

        // Reset variables
        meas.valid = 0;
        aes_key_valid = 0;
        pretend_wakeup_by_snt = 0;

        // Get the info on who woke up the system, then reset WAKEUP_SOURCE register.
        if (get_flag(FLAG_MAIN_CALLED_BY_SNT)) {
            wakeup_source = 0x90; // Treat it as if the SNT timer has woken up the system. Set bit[7] as well. See WAKEUP_SOURCE.
            pretend_wakeup_by_snt = 1;
            set_flag(FLAG_MAIN_CALLED_BY_SNT, 0);
        }
        else {
            wakeup_source = *SREG_WAKEUP_SOURCE;
        }
        *SCTR_REG_CLR_WUP_SOURCE = 1;

        #ifdef DEVEL
            mbus_write_message32(0x70, wakeup_source);
            mbus_write_message32(0x71, xt1_state);
        #endif

        // If this is the very first wakeup, initialize the system
        if (!get_flag(FLAG_INITIALIZED)) operation_init();

        // Check-in the EID Watchdog if it is enabled
        if (get_flag(FLAG_WD_ENABLED)) eid_check_in();

        //-----------------------------------------
        // NFC HANDSHAKE
        //-----------------------------------------
        if (WAKEUP_BY_NFC) {

            if (nfc_i2c_get_token(30)) {

                uint32_t target = nfc_check_cmd();

                nfc_i2c_release_token();

                #ifdef DEVEL
                    mbus_write_message32(0xB0, target);
                #endif

                // Fail Handling
                if (target==0xDEAD) {
                    operation_sleep(/*check_snt*/1);
                }

                // Commands requiring further operation
                else if (target==0xF00D) {

                    // Command: GETSEC
                    if (cmd==CMD_GETSEC) {
                        // Nothing to do here.
                    }
                    // Command: CONFIG
                    else if (cmd==CMD_CONFIG) {
                        update_user_configs();
                        aes_encrypt_eeprom(/*addr*/EEPROM_ADDR_HIGH_TEMP_THRESHOLD);
                        eid_blink_display();
                    }
                    // Command: HARD_RESET
                    else if (cmd==CMD_HRESET) {
                        eid_update_display(/*seg*/DISP_PLUS|DISP_MINUS);
                        config_timerwd(/*cnt*/10);
                        while(1);
                    }
                    // Command: NEWKEY
                    else if (cmd==CMD_NEWKEY) {
                        update_aes_key();
                        eid_blink_display();
                    }
                    // Command: NEWAB
                    else if (cmd==CMD_NEWAB) {
                        update_temp_coefficients();
                        eid_blink_display();
                    }
                    // Command: DISPLAY
                    else if (cmd==CMD_DISPLAY) {
                        if (!get_bit(eid_disp_before_user_mod, 31))
                            eid_disp_before_user_mod = (0x1<<31)|eid_get_current_display();
                        eid_update_display(/*seg*/cmd_param&0x7F);
                        cmd = CMD_NOP;
                    }
                    // Command: DEBUG
                    else if (cmd==CMD_DEBUG) {
                        override.enable = get_bit(cmd_param, 7);
                        override.sar_margin = cmd_param&0xF;

                        #ifdef ENABLE_DEBUG_SYSTEM_CONFIG
                        if (cmd_param==0x00) debug_system_configs();
                        #endif

                        eid_blink_display();
                    }
                    // Command: NOP
                    else if (cmd==CMD_NOP) {
                        eid_blink_display();
                    }
                    // Command: Invalid
                    else {}

                    // Go to Sleep
                    operation_sleep(/*check_snt*/1);
                }

                // Normal User commands (START, STOP, SRESET)
                else {
                    set_system(/*target*/target);
                    snt_skip_calib = 1;

                    if (cmd==CMD_START) {
                        if (status.sample_type==SAMPLE_NORMAL)
                            aes_encrypt_eeprom(/*addr*/EEPROM_ADDR_START_TIME);

                        // if start_delay=0, treat it as if waken up by SNT
                        if (eeprom_user_config.temp_meas_start_delay==0) {
                            wakeup_source |= (0x1 << 4);
                            pretend_wakeup_by_snt = 1;
                        }
                    }
                    else if (cmd==CMD_STOP) {
                        if (status.sample_type==SAMPLE_NORMAL)
                            aes_encrypt_eeprom(/*addr*/EEPROM_ADDR_STOP_TIME);
                    }
                }
            }
        }

        //-----------------------------------------
        // WAKEUP BY SNT
        //-----------------------------------------
        if (WAKEUP_BY_SNT) {
            // If this is the expected wakeup by SNT
            // --- CASE 1) pretend_wakeup_by_snt = 1
            // --- CASE 2) SNT Counter Value - snt_threshold < 32 sec
            if (  pretend_wakeup_by_snt
               || ((snt_read_wup_timer()-snt_threshold)<(snt_freq<<5))
               ) {

                // Counter for E-Ink Refresh 
                disp_min_since_refresh += wakeup_interval;

                #ifdef DEVEL
                    mbus_write_message32(0xA1, disp_min_since_refresh);
                #endif

                if (xt1_state==XT1_PEND) set_system(/*target*/XT1_ACTIVE);

                // Bookkeeping/Temp Measurement
                snt_operation();
            }
            // If this is an accidental/unintended wakeup, quietly go to sleep
            else {
                #ifdef DEVEL
                    mbus_write_message32(0x8E, snt_threshold);
                #endif
                operation_sleep(/*check_snt*/1);
            }

        }

        //-----------------------------------------
        // Other Wakeup Source - GOC/EP or a Glitch
        //-----------------------------------------
        if (!WAKEUP_BY_NFC && !WAKEUP_BY_SNT) {
            if (WAKEUP_BY_GOCEP) {
                //uint32_t goc_head = (*REG0 >> 16) & 0xFF;
                ////uint32_t goc_data = *REG0 & 0xFFFF;
                uint32_t orig_disp;
                //if (goc_head==0x00) {
                    orig_disp = eid_get_current_display();
                    eid_update_display(/*seg*/DISP_ALL);
                    eid_update_display(/*seg*/orig_disp);
                //}
            }
            operation_sleep(/*check_snt*/1);
        }

    }
    // If waken up by the temp sensor, resume snt_operation()
    else {
        snt_operation();
    }

    // SNT Calibration
    calibrate_snt_timer(/*skip_calib*/snt_skip_calib);
    operation_sleep_snt_timer(/*check_snt*/0);

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
