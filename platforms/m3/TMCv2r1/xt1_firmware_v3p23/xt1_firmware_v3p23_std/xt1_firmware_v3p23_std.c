//*******************************************************************************************
// XT1 (TMCv2r1) FIRMWARE
// Version 3.23 (standard)
//------------------------
#define HARDWARE_ID 0x01005843  // XT1r1 Hardware ID
#define FIRMWARE_ID 0x0317      // [15:8] Integer part, [7:0]: Non-Integer part
//-------------------------------------------------------------------------------------------
// SNT Timer Read Time-Out (snt_sync_read_wup_timer() in snt_read_wup_timer())
//
// Note that when the SNT Timer restarts, 
//  snt_read_wup_timer() may return ~0-ish value,
//  and __snt_timer_restarted__ variable is set 1.
//
//  * operation_init()
//      -> It sets snt_threshold based on the returned value.
//          So 'check_restart=0' will do the job.
//  * calibrate_snt_timer()
//      -> It sets snt_threshold based on the returned value.
//          So 'check_restart=0' will do the job.
//  * check_snt_wakeup_near_threshold()
//      -> If restarted, if sets snt_threshold=snt_threshold_prev=0.
//      As a result, it always returns 1 (i.e., treating it as a valid wakeup)
//  * operation_sleep()
//      - Only when 'check_snt=1'
//      - This happens when the unit woke up by NFC or a glitch.
//      -> If restarted, if sets snt_threshold=snt_threshold_prev=0.
//      As a result, it will directly call main()
//  * calc_ssls()
//      -> If restarted, if sets snt_threshold=snt_threshold_prev=0.
//      As a result, SSLS is always 0. And at the end, it will directly call main()
//-------------------------------------------------------------------------------------------
//
// SNT Threshold Update Time-Out (snt_set_wup_timer())
//  * operation_init()
//      operation_sleep_snt_timer()
//          snt_set_wup_timer()
//              snt_set_timer_threshold()
//
//      -> Alt. Threshold = snt_duration
//
//  * meas_temp_adc() (when crit_vbat is detected)
//      snt_set_wup_timer()
//          snt_set_timer_threshold()
//
//      -> Alt. Threshold = 0
//
//  * main() (At the end...)
//      operation_sleep_snt_timer()
//          snt_set_wup_timer()
//              snt_set_timer_threshold()
//
//      -> Alt. Threshold = snt_duration
//
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
// XT1 Display Failure Pattern
//-------------------------------------------------------------------------------------------
//
//  Display Patterns
//  -----------------------------------------
//  TYPE     WUP PMU SNT I2C MET WTH GEN INIT
//  -----------------------------------------
//  PLAY             v   v   v   v    
//  TICK                                 v
//  LOWBAT   v   v   v   v   v   v   v
//  BSLASH   v   v   v   v   v   v   v   v
//  SLASH                            
//  PLUS     v   v           v   v    
//  MINUS        v       v       v    
//  -----------------------------------------
//  
//  Description
//  --------------------------------------------------------------
//  WUP : Timeout while reading the SNT timer value
//  PMU : Timeout while accessing PMU registers
//  SNT : Timeout during SNT temperature measurement
//  I2C : I2C NAK Failure
//  MET : Meta-Stability (SNT timer reading is fluctuating)
//  WTH : Timeout while updating the SNT timer threshold
//  GEN : Generic/Unknown Error
//  INIT: TMC-INIT is not done correctly (handled in set_system())
//  --------------------------------------------------------------
//
//  NOTE: 'DEVEL' flavor ignores all failures except INIT.
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
//  0x76    value                               [DEBUG_TEMP_CALIB] Temperature Calibration Coefficient A gets updated to 'value'
//  0x77    value                               [DEBUG_TEMP_CALIB] Temperature Calibration Coefficient B gets updated to 'value'
//  0x78    value                               [DEBUG_AES_KEY] aes_key[0] is set to 'value'
//  0x79    value                               [DEBUG_AES_KEY] aes_key[1] is set to 'value'
//  0x7A    value                               [DEBUG_AES_KEY] aes_key[2] is set to 'value'
//  0x7B    value                               [DEBUG_AES_KEY] aes_key[3] is set to 'value'
//  0x7C    value                               num_cons_excursions
//  0x7D    value                               start_delay_cnt
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
//                                                  0x3: 'skip_calib' or 'MAIN_CALLED_BY_SNT' or 'FLAG_INVLD_XO_PREV' is 1. Will use the previous snt_freq as it is.
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
//                                                  [31:16] Raw code (temp.raw)
//                                                  [15: 0] Translated Temperature (temp.val)
//  0x91    value                               meas_temp_adc(), snt_operation() information
//                                                  0x00000000: meas_temp_adc() is called with go_sleep=0
//                                                  0x00000001: meas_temp_adc() is called with go_sleep=1
//                                                  0x00000002: Skipped SAR adjustment due to hibernation
//                                                  0x00000004: Beginning of the new hibernation
//                                                  0x00000005: Hibernation continues
//                                                  0x00000006: new_alarm_hbr=1 and new_alarm=1 right after a hibernation
//                                                  0x00000007: First sample after a hibernation.
//                                                  0xFFFFFFFF: Skipped the temperature measurement since meas.valid=1
//  0x92    {0x1, sub_meas_cnt}                 Sub-measurement count (incrementing every minute)
//  0x92    {0x2, sub_meas_sum}                 Current sum of the sub-measurements
//  0x92    {0x3, meas_cnt}                     Measurement Count
//  0x92    {0x4, hbr_meas_cnt}                 Hibernation Meas Count
//  0x93    value                               VBAT and SAR Ratio Information
//                                                  [   28] Critical VBAT (crit_vbat)
//                                                  [   24] Low VBAT (low_vbat)
//                                                  [23:16] ADC Offset (adc_offset)
//                                                  [15: 8] SAR Ratio (meas.sar)
//                                                  [ 7: 0] ADC Reading (meas.adc)
//  0x94    value                               PMU SAR Ratio is changing
//                                                  [15: 8] Previous SAR Ratio
//                                                  [ 7: 0] New SAR Ratio
//  0x96    comp_sample_id                      Compression Sample ID
//  0x97    num_bits                            Encoded data to be stored (num_bits)
//  0x98    reverse_code[63:32]                 (Reversed) Encoded data to be stored (reverse_code[63:32])
//  0x99    reverse_code[31:0]                  (Reversed) Encoded data to be stored (reverse_code[31:0])
//  0x9C    value                               Averaged Measurement value (temp.val)
//  0x9D    {0x00, tmp_exc.num_cons_hi}         temp.val exceeds the high threshold. Alarm not yet triggered.
//  0x9D    {0x0F, tmp_exc.num_cons_hi}         temp.val exceeds the high threshold. A new alarm has been triggered.
//  0x9D    {0x10, tmp_exc.num_cons_lo}         temp.val exceeds the low threshold. Alarm not yet triggered.
//  0x9D    {0x20, tmp_exc.num_cons_lo}         tmp_exc.num_cons_lo has been incremented due to hibernation
//  0x9D    {0x1F, tmp_exc.num_cons_lo}         temp.val exceeds the low threshold. A new alarm has been triggered.
//  0x9E    value                               Current alarm status
//                                                  0xFFFFFFFF: High Alarm && Low Alarm
//                                                  0xFFFF0000: High Alarm Only
//                                                  0x0000FFFF: Low Alarm && Low Alarm
//                                                  0x00000000: No Alarm
//  0x9F    hbr_temp_threshold_raw              Temperature Thresholds for Hibernation (Raw)
//
//  <--- Below are valid only when [Raw Sample] is enabled --->
//
//  0x9C    byte_addr                           [DEBUG_RAW; Raw Sample] Byte Address
//  0x9D    temp.val                            [DEBUG_RAW; Raw Sample] Raw Temperature Data
//  0x9E    sar_adc_addr                        [DEBUG_RAW; Raw Sample] Byte Address to store SAR/ADC (or XO FAIL) value
//  0x9F    sar_adc_data                        [DEBUG_RAW; Raw Sample] SAR/ADC (or XO FAIL) data
//
//  -----------------------------------------------------------------------------------------
//  < Display >
//  -----------------------------------------------------------------------------------------
//  0xA0    {0x0, seg}                          Display has changed to 'seg' (EEPROM Updated)
//  0xA0    {0x1, 0x0}                          Display has been refreshed (EEPROM Not Updated)
//  0xA0    {0x2, seg}                          Display has changed to 'seg' but failed to update the EEPROM.
//  0xA0    {0xF, seg}                          Display has blinked.
//  0xA1    disp_min_since_refresh              Elapsed time (in minutes) since the last E-ink update
//  0xA2    value                               EID duration and Refresh Interval are set
//                                                  [31:16] EID Duration
//                                                  [15: 0] Refresh Interval (minutes)
//  0xA3    0x00000000                          Critical Temperature or hibernation. Display did not update, although XT1 behaves like it did.
//  0xA5    eid_updated_at_low_temp             1 indicates that the display has been updated at a low temperature
//  0xA6    eid_updated_at_crit_temp            1 indicates that the display has a pending update due to a critical temperature
//  0xA7    eid_update_type                     Update type used for the display update
//                                                  0x0: Global Update
//                                                  0x1: Local Update
//  0xA8    0x00000000                          display_low_batt() is called.
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
//  --- data_buffer_update()
//  0xB4    data                                Unencrypted data (qword)
//  0xB5    addr                                dat_buf_addr.addr
//  0xB6    data                                Encrypted data
//  0xB7    CRC                                 dat_buf_crc.crc32
//
//  --- all_buffer_commit()
//  0xB8    value                               all_buffer_commit() is called
//                                                  [ 8]: cnt_buf_meas.valid
//                                                  [ 4]: pmu_buf.valid
//                                                  [ 0]: dat_buf_addr.valid
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
//          {0x14, pattern          }           hibernation status have been set
//                                                  [12]: status.hbr_temp
//                                                  [ 8]: status.hbr_flag
//                                                  [ 4]: status.hibernating
//                                                  [ 0]: status.nfc_out_temp
//          {0x15, commit_status_pend}          status.commit_status_pend
//  0xBB    value                               Bit Positions
//                                                  [31:16] bit_pos_start
//                                                  [15: 0] bit_pos_end
//  0xBC    value                               Misc Info
//                                                  [31:24] last_qword_id
//                                                  [23: 0] sample_cnt
//  -----------------------------------------------------------------------------------------
//  < Math Functions > -> Need to enable DEBUG_MATH
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
//  0xD5    {ptr}                               [DEPRECATED] Word Read Information.
//                                                  [31: 0] ptr (where the read data is stored)
//  0xD6    value                               Word Read operation return value
//                                                  0x1: Word Read operation has been successfully done
//                                                  0x0: Word Read operation has NAKed. May retry.
//  0xD7    ptr                                 [DEPRECATED] ptr where the 32-bit read data (shown with 0xD8) is stored
//  0xD8    data                                32-bit read data stored at ptr (shown with 0xD7)
//
//  --- Common
//  0xDF    iter                                [DEPRECATED] Iteration number (if previous I2C transcation was NAK'd)
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
//  0xE1    {0x1, iter}                         eid_enable_cp_ck() does not receive a reply from EID at iter-th attempt (iter=0, 1, 2, 3, 4)
//                                                  iter=4 indicates that it will trigger FAIL_ID_EID failure handling.
//  0xE1    {0x2, iter}                         snt_set_timer_threshold() has received 0xDEAD from SNT at iter-th attempt (iter=0, 1, 2, 3, 4)
//  0xE1    {0x3, iter}                         snt_set_timer_threshold() does not receive a reply from SNT at iter-th attempt (iter=0, 1, 2, 3, 4)
//                                                  iter=4 indicates that it will trigger FAIL_ID_WTH failure handling.
//  0xE1    {0x4, iter}                         snt_sync_read_wup_timer()() has received 0xAD00DEAD from SNT at iter-th attempt (iter=0, 1, 2, 3, 4)
//  0xE1    {0x5, iter}                         snt_sync_read_wup_timer()() does not receive a reply from SNT at iter-th attempt (iter=0, 1, 2, 3, 4)
//                                                  iter=4 indicates that it will trigger FAIL_ID_WUP failure handling.
//  0xEE    value                               SNT Timer Sync Read value 
//  0xEF    __wfi_timeout_id__                  Timeout/Fail occurs
//                                                  0x0: [DEPRECATED] Generic/Unknown
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
//#define DEVEL
#define ENABLE_GOC_CRC                    // Enable GOC CRC Calculation
#define ENABLE_RAW                        // Enable support for SAMPLE_RAW
//#define ENABLE_XO_PAD
//#define ENABLE_EID_WATCHDOG               // Enable EID Watchdog
//#define USE_SHORT_REFRESH
//#define ENABLE_DEBUG_SYSTEM_CONFIG
//#define DEBUG_TEMP_CALIB                  // Display when eeprom_temp_calib changes
//#define DEBUG_AES_KEY                     // Display when aes_key[3:0] changes
//#define DEBUG_RAW                         // Display Raw Sample information (ENABLE_RAW must be enabled)
#define WAKEUP_INTERVAL_IN_MINUTES          // The unit of Wakeup Intervals is 'minutes'. If undefined, the unit is 'seconds'

//#define SKIP_SAR_IF_HIBERNATION           // Skip SAR adjustment if in hibernation
#define EID_USE_GLOBAL_UPDATE_ONLY          // Use GLOBAL_UPDATE for all temperatures. If disabled, use LOCAL_UPDATE below EEPROM_ADDR_EID_LOW_TEMP_THRESHOLD. Use GLOBAL_UPDATE otherwise.
#define IGNORE_FIRST_SAMPLE_EXCURSION       // The first sample is NOT counted for NUM_CONS_EXCURSIONS
#define EID_CRIT_TEMP_THRESHOLD         25  // Display does not update when temperature is lower than (-1)*EID_CRIT_TEMP_THRESHOLD

#define EID_REFRESH_INT_MIN_HIGH     12*60  //  EID Refresh interval for High Temperature (unit: min). '0' means 'do not refresh'.
#define EID_REFRESH_INT_MIN_MID      24*60  //  EID Refresh interval for Mid Temperature (unit: min). '0' means 'do not refresh'.
#define EID_REFRESH_INT_MIN_LOW       0*60  //  EID Refresh interval for Low Temperature (unit: min). '0' means 'do not refresh'.

//*******************************************************************************************
// INITIALIZATION
//*******************************************************************************************
#define INIT_TRIG_ALL   0xF00D
#define INIT_TRIG_NOCAL 0xBEEF
#define INIT_DEBUG_READ 0xDEAD
#define INIT_RESET      0x0000

#define TEMP_CALIB_A_DEFAULT    0x00007E8B  // Default value of temperature Calibration Coefficient a (fixed-point, N=10)
#define TEMP_CALIB_B_DEFAULT    0x005479B9  // Default value of temperature Calibration Coefficient b (fixed-point, N=10)

#define HBR_TEMP_THRESHOLD_RAW_DEFAULT  0   // Default value of hbr_temp_threshold_raw. 0 means 'hibernation disabled'

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1
#define FLAG_XO_INITIALIZED     2
#define FLAG_WD_ENABLED         3   // Only if ENABLE_EID_WATCHDOG is enabled.
#define FLAG_MAIN_CALLED_BY_SNT 4   // Sleep has been bypassed before the current active session. Thus, main() has called from within operation_sleep(). This implies that xo_val_curr is inaccurate.
#define FLAG_INVLD_XO_PREV      5   // Sleep has been bypassed before the previous active session. This implies that xo_val_prev is inaccurate.
#define FLAG_BOOTUP_DONE        6
#define FLAG_MAIN_CALLED_BY_NFC 7   // Same as FLAG_MAIN_CALLED_BY_SNT. The only difference is that it directly calls main() because it detects a GPO pulse during the 1st wakeup.
#define FLAG_TEMP_MEAS_PEND     8   // Indicates that the system has gone sleep for temperature measurement

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// XO Frequency
#define XO_FREQ_SEL 0                       // XO Frequency (in kHz) = 2 ^ XO_FREQ_SEL
#define XO_FREQ     1<<(XO_FREQ_SEL+10)     // XO Frequency (in Hz)

// XO Initialization Wait Duration
#define XO_WAIT_A   2*DLY_1S    // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S    // Must be ~1 second delay.

#ifdef WAKEUP_INTERVAL_IN_MINUTES // Intervals are given in minutes
    // Pre-defined Wakeup Intervals
    #define WAKEUP_INTERVAL_ACTIVE      1   // (Default: 1) Wakeup Interval for XT1_ACTIVE (unit: minutes)
    #ifdef USE_SHORT_REFRESH
        #define WAKEUP_INTERVAL_IDLE    5   // (Default: 720) Wakeup Interval for XT1_IDLE (unit: minutes)
    #else
        #define WAKEUP_INTERVAL_IDLE    720    // (Default: 720) Wakeup Interval for XT1_IDLE (unit: minutes)
    #endif
#else // Intervals are given in seconds
    #define WAKEUP_INTERVAL_IDLE_IN_SECONDS 30
    // Pre-defined Wakeup Intervals (unit: seconds)
    #define WAKEUP_INTERVAL_ACTIVE      60
    #define WAKEUP_INTERVAL_IDLE        WAKEUP_INTERVAL_IDLE_IN_SECONDS
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
#define WAKEUP_BY_NFC       WAKEUP_BY_GPIO1
#define MAIN_CALLED_BY_SNT  (WAKEUP_BY_SNT && get_bit(wakeup_source, 7))
#define MAIN_CALLED_BY_NFC  (WAKEUP_BY_NFC && get_bit(wakeup_source, 7))

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
//#define CMD_DISPLAY   0x10  // DEPRECATED
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

#ifndef DEVEL
union eeprom_adc_th_t {
    // eeprom_adc_th_0
    struct {
        unsigned t1 :   16; // [15: 0] Temperature Threshold (Raw Code) for ADC. See the table above.
        unsigned t2 :   16; // [31:16] Temperature Threshold (Raw Code) for ADC. See the table above.
    };
    // eeprom_adc_th_1
    struct {
        unsigned t3 :   16; // [15: 0] Temperature Threshold (Raw Code) for ADC. See the table above.
        unsigned t4 :   16; // [31:16] Temperature Threshold (Raw Code) for ADC. See the table above.
    };
    uint32_t value;
};
volatile union eeprom_adc_th_t eeprom_adc_th_0;
volatile union eeprom_adc_th_t eeprom_adc_th_1;
#endif

union eeprom_adc_t {
    // eeprom_adc_0
    struct {
        unsigned offset_r1  :   8;  // [ 7: 0] ADC offset. 2's complement. See the table above.
        unsigned offset_r2  :   8;  // [15: 8] ADC offset. 2's complement. See the table above.
        unsigned offset_r3  :   8;  // [23:16] ADC offset. 2's complement. See the table above.
        unsigned offset_r4  :   8;  // [31:24] ADC offset. 2's complement. See the table above.
    };
    // eeprom_adc_1
    struct {
        unsigned offset_r5  :   8;  // [ 7: 0] ADC offset. 2's complement. See the table above.
        unsigned low_vbat   :   8;  // [15: 8] ADC code threshold to turn on the Low VBAT indicator.
        unsigned crit_vbat  :   8;  // [23:16] ADC code threshold to trigger the EID crash handler.
        unsigned dummy      :   8;  // [31:24]
    };
    uint32_t value;
};
volatile union eeprom_adc_t eeprom_adc_0;
volatile union eeprom_adc_t eeprom_adc_1;


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
    // eeprom_eid_duration_0
    struct {
        unsigned high   :   16; // [15: 0] EID duration (ECTR_PULSE_WIDTH) for High Temperature
        unsigned mid    :   16; // [31:16] EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
    };
    // eeprom_eid_duration_1
    struct {
        unsigned low    :   16; // [15: 0] EID duration (ECTR_PULSE_WIDTH) for Low Temperature
        unsigned dummy  :   16; // [31:16]
    };
    uint32_t value;
};
volatile union eeprom_eid_duration_t eeprom_eid_duration_0;
volatile union eeprom_eid_duration_t eeprom_eid_duration_1;

//--- Temperature Calibration Coefficients
volatile uint32_t eeprom_temp_calib_a;  // Temperature Calibration Coefficient a
volatile uint32_t eeprom_temp_calib_b;  // Temperature Calibration Coefficient b

//--- Hibernation Threshold
volatile uint32_t hbr_temp_threshold_raw;   // Hibernation Threshold (Raw) - Read from EEPROM_ADDR_HBR_THRESHOLD_RAW_WRITEONLY

//-------------------------------------------------------------------------------------------
// User Configuration Variables in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in update_user_configs() function.
//-------------------------------------------------------------------------------------------

union eeprom_user_config_t {
    // eeprom_user_config_0
    struct {
        unsigned high_temp_threshold   :    16; // [15: 0] Threshold for High Temperature
        unsigned low_temp_threshold    :    16; // [31:16] Threshold for Low Temperature
    };
    // eeprom_user_config_1
    struct {
        unsigned temp_meas_interval    :    16; // [15: 0] Period of Temperature Measurement
        unsigned temp_meas_start_delay :    16; // [31:16] Start Delay before starting temperature measurement
    };
    uint32_t value;
};
volatile union eeprom_user_config_t eeprom_user_config_0;
volatile union eeprom_user_config_t eeprom_user_config_1;

volatile uint32_t num_cons_excursions; // Number of excursions required to trigger an alarm

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
// System Status
//-------------------------------------------------------------------------------------------
// NOTE: Most of these variables are reset in reset_system_status() function.
// NOTE: Sample ID starts from 0
//-------------------------------------------------------------------------------------------
volatile uint32_t sub_meas_cnt;                     // Sub-measurement count (incrementing every minute)
volatile uint32_t meas_cnt;                         // Measurement Count. Incremending at every measurement interval, regardless of the hibernation status.
volatile uint32_t sample_cnt;                       // Sample Count. Incremending at every measurement interval ONLY IF it is NOT in hibernation.
volatile uint32_t hbr_meas_cnt;                     // Hibernation Measurement count; Incrementing with meas_cnt but only in hibernation.
volatile uint32_t sub_meas_sum;                     // Sum of the sub-measurements
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
        unsigned hbr_temp           :   1;  // [   20] 1 indicates that the temp.raw < hbr_temp_threshold_raw
        unsigned hbr_flag           :   1;  // [   21] 1 indicates that the it has seen a temp below hbr_temp_threshold_raw during the last meas interval
        unsigned hibernating        :   1;  // [   22] 1 indicates it is currently in hibernation
        unsigned nfc_out_temp       :   1;  // [   23] 1 indicates that the current temperature is out of the NFC chip operating range.
        unsigned commit_status_pend :   1;  // [   24] 1 indicates that commit_status() is pending due to lack of valid temp measurement or status.hbr_flag.
        unsigned dummy0             :   7;  // [31:25] 
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
volatile uint32_t start_delay_cnt;                  // Start Delay Counter (incrementing every 1 min)
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

volatile uint32_t crc32;                    // Adler-32 CRC Value (If ENABLE_GOC_CRC, it is also used to check the GOC CRC)

//-------------------------------------------------------------------------------------------
// Write Buffers
//-------------------------------------------------------------------------------------------

//--- Counter Buffer
union cnt_buf_t {
    // cnt_buf_sample: Last Qword ID and Sample Count
    struct {
        unsigned sample_cnt     :   23; // [22: 0] Sample Count
        unsigned last_qword_id  :    9; // [31:23] Last Qword ID
    };
    // cnt_buf_meas: Valid & Measurement Count
    struct {
        unsigned meas_cnt       :   23; // [22: 0] Measurement Count
        unsigned rsvd0          :    8; // [30:23] 
        unsigned valid          :    1; // [21] 
    };
    uint32_t value;
};
volatile union cnt_buf_t cnt_buf_sample;
volatile union cnt_buf_t cnt_buf_meas;

//--- PMU Buffer
union pmu_buf_t {    
    struct {
        unsigned adc    :    8; // [ 7: 0] ADC Value
        unsigned sar    :    8; // [15: 8] SAR Ratio
        unsigned rsvd0  :   15; // [30:16]
        unsigned valid  :    1; // [31]
    };
    struct {
        unsigned sar_adc:   16; // [15: 0] SAR and ADC
        unsigned upper  :   16; // [31:16]
    };
    uint32_t value;
};
volatile union pmu_buf_t pmu_buf;

//--- Data Buffer
union dat_buf_t {
    // dat_buf_crc
    struct {
        unsigned crc32  :   32; // [31: 0] CRC32
    };
    // dat_buf_addr
    struct {
        unsigned addr   :   16; // [15: 0] EEPROM Address
        unsigned rsvd0  :   15; // [30:16]
        unsigned valid  :    1; // [31] Becomes 1 when updating buf_storage and dat_buf_crc.crc32; Becomes 0 after writing buf_storage and dat_buf_crc.crc32 in EEPROM.
    };
    uint32_t value;
};
volatile union dat_buf_t dat_buf_crc;
volatile union dat_buf_t dat_buf_addr;
volatile uint32_t buf_storage[4];

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

#ifdef ENABLE_DEBUG_SYSTEM_CONFIG
union override_t {
    struct {
        unsigned sar_margin :  4;   // [ 3: 0] SAR Margin
        unsigned dummy      : 27;   // [30: 4] Dummy
        unsigned enable     :  1;   // [   31] Override enable
    };
    uint32_t value;
};

volatile union override_t override;
#endif

//--- Display
volatile uint32_t disp_refresh_interval;    // (Unit: min) Display Refresh Interval. 0 means 'do not refresh'.
volatile uint32_t disp_min_since_refresh;   // (Unit: min) Minutes since the last Display Refresh.
volatile uint32_t eid_duration;             // EID active CP duration 
volatile uint32_t eid_updated_at_low_temp;  // Becomes 1 when the display gets updated at a temperature lower than eeprom_eid_threshold.low.
volatile uint32_t eid_updated_at_crit_temp; // Becomes 1 when the display has a pending update due to a critical temperature.
volatile uint32_t eid_crit_temp_threshold;  // Display does not update when temperature is lower than EID_CRIT_TEMP_THRESHOLD
volatile uint32_t eid_update_type;          // EID Update Type (0: Global Update, 1: Local Update)

//--- EID-related variables
//  __eid_current_display__ (Defined in TMCv2r1.h)
//          Indicates the actual pattern displayed on the e-ink.
//          Updated by eid_enable_cp_ck()
//
//  eid_expected_display
//          Indicates the expected display pattern. Updated in eid_update_display()
//          There could be discrepancy between __eid_current_display__ and eid_expected_display
//          if the display update process is skipped due to the EID critical temperature, etc.
volatile uint32_t eid_expected_display;
//
//  eid_seg_in_eeprom
//          EID pattern status saved in EEPROM. 
//          This may not consistent with the actual display pattern on the e-ink, due to hibernation, etc.
volatile uint32_t eid_seg_in_eeprom;        

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep/IRQ Handling
static void operation_sleep (uint32_t check_snt);
static void operation_sleep_snt_timer(void);
static void operation_dead(void);
static void operation_init(void);
void disable_all_irq_except_timer32(void);
void fail_handler(uint32_t id);

//--- FSM
static void set_system(uint32_t target);
static void reset_system_status(uint32_t target);
static void commit_status(uint32_t forced);
static void meas_temp_adc (uint32_t go_sleep);
static void snt_operation (void);
static void display_low_batt (void);

//--- XO & SNT Timers
static uint32_t get_xo_cnt(void);
static void calibrate_snt_timer(uint32_t skip_calib);
static uint32_t calc_ssls(void);
static uint32_t check_snt_wakeup_near_threshold(void);
static uint32_t snt_read_wup_timer_check_restart(void);

//--- E-Ink Display
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
static void data_buffer_update(uint32_t addr, uint32_t commit_crc);
static void all_buffer_commit(void);

//--- Data Compression
uint32_t tcomp_encode (uint32_t delta);
uint32_t reverse_bits (uint32_t bit_stream, uint32_t num_bits);
void reverse_bits_ext (uint32_t* ptr, uint32_t code, uint32_t count, uint32_t value);

//--- 128-bit qword Handling
void set_qword (uint32_t pattern);
void sub_qword (uint32_t msb, uint32_t lsb, uint32_t* ptr);

//--- AES & CRC
void aes_encrypt(uint32_t* pt);
void aes_encrypt_eeprom(uint32_t addr);
uint32_t calc_crc32(uint32_t* src, uint32_t crc_prev, uint32_t nw);

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
    if (WAKEUP_BY_NFC) {
        meas_temp_adc(/*go_sleep*/0);
        if (meas.low_vbat) display_low_batt();
    }

    // Execute commit_status() if there is any pending status
    if (status.commit_status_pend) {
        commit_status(/*forced*/0);
        nfc_power_off();
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
        uint32_t snt_val = snt_read_wup_timer_check_restart();
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

    // Directly call main() if there is a pending NFC command coming in during the 1st wakeup.
    if (get_flag(FLAG_MAIN_CALLED_BY_NFC)) {
        *NVIC_ICPR = 0xFFFFFFFF; // For safety, disable/clear all pending IRQs.
        main();
        return;
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
// Args    : None
// Description:
//           Goes into sleep with SNT Timer enabled
//           Auto Reset is disabled.
//           SNT threshold is set to snt_threshold.
//           If check_snt=1, it first checks the SNT counter value.
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep_snt_timer(void) {
    //NOTE: If it restarts the SNT timer (due to timeouts, etc..)
    //      it sets snt_threshold to a new value so that XT1 wakes up in 1min.
    snt_threshold = snt_set_wup_timer(/*auto_reset*/0, /*threshold*/snt_threshold, /*altval*/snt_duration);
    #ifdef DEVEL
        mbus_write_message32(0x8F, snt_threshold);
    #endif
    operation_sleep(/*check_snt*/0);
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
    
    // Reset & Turn off the temperature sensor
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

    #ifdef ENABLE_GOC_CRC
        crc32  = calc_crc32(/*src*/(uint32_t*)0x0 , /*crc_prev*/1, /*nw*/(((*PTR_LAST_MEM_ADDR)>>2)+1));
    #endif

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

        // Update) the flag
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
        
        // Set the Active floor setting to the minimum (b/c RAT is enabled at this point)
        pmu_set_active_min();

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
        //nfc_i2c_byte_write(/*e2*/0, /*addr*/6, /*data*/0x0100, /*nb*/2);
        
    #ifdef ENABLE_GOC_CRC
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_CRC, /*data*/crc32, /*nb*/4);
    #endif

        // Set default values for CALIB and AES_KEY
        eeprom_temp_calib_a = TEMP_CALIB_A_DEFAULT;
        eeprom_temp_calib_b = TEMP_CALIB_B_DEFAULT;
        for (i=0; i<4; i++) *(aes_key+i) = 0x0;

        // Set default values for Hibernation Threshold
        hbr_temp_threshold_raw = HBR_TEMP_THRESHOLD_RAW_DEFAULT;

        // Initialize the EEPROM variables
        update_system_configs(/*use_default*/1);

        // Reset EEPROM debug sections
        eeprom_init();

        // Turn off the NFC
        nfc_power_off();

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
	    eid_init();

    #ifdef ENABLE_EID_WATCHDOG
        // Enable EID Crash Hander
        eid_enable_crash_handler();
        set_flag(FLAG_WD_ENABLED, 1); 
    #endif

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
        snt_threshold_prev = snt_read_wup_timer(/*check_restart*/0);
        snt_threshold = snt_threshold_prev + snt_duration;
        xo_val_curr = get_xo_cnt();
        operation_sleep_snt_timer();
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
        commit_status(/*forced*/1); // FORCED
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

        // Display all segments
        eid_update_display(/*seg*/DISP_ALL);

        // Update/Reset System Configuration
        if (!status.skip_init) update_system_configs(/*use_default*/0);

        // Reset the System State in EEPROM
        status.eeprom_state = 0; // curr_state=XT1_RESET
        status.eeprom_fail = 0;
        commit_status(/*forced*/1); // it is guaranteed that it is not in hibernation.

        __wfi_id__=FAIL_ID_NONE; // Let's make it match status.eeprom_fail.

        // Update the state
        xt1_state = XT1_RESET;
    }

    //--------------------------------------
    // XT1_PEND
    //--------------------------------------
    else if (target==XT1_PEND) {

        reset_system_status(target);

        // If Raw Sample mode, use the pre-defined configuration
    #ifdef ENABLE_RAW
        if (status.sample_type == SAMPLE_RAW) {
            eeprom_user_config_0.high_temp_threshold   = 1800;   // 100C
            eeprom_user_config_0.low_temp_threshold    = 400;    // -40C
            eeprom_user_config_1.temp_meas_interval    = 1;      // 1 min
            eeprom_user_config_1.temp_meas_start_delay = 0;      // 0 min
            num_cons_excursions   = 1;
        }
    #endif

        // Display Play Sign 
        // If start_delay=0, the display does not need to be updated here. 
        if (eeprom_user_config_1.temp_meas_start_delay!=0) 
            eid_update_display(/*seg*/DISP_PLAY);
        else {
            status.disp_skipped = 1;
            #ifdef DEVEL
                mbus_write_message32(0xBA, (0x11 << 24) | 0x1 /*status.disp_skipped*/);
            #endif
        }

        // Start Delay
        wakeup_interval = WAKEUP_INTERVAL_ACTIVE;

        // Set the System State
        status.curr_state = XT1_PEND;
        status.activated = 1;
        status.memory_full = 0;
        status.unread_sample = 0;
        commit_status(/*forced*/1); // it is guaranteed that it is not in hibernation.

        // Update the state
        xt1_state = XT1_PEND;
    }

    //------------------------------------------
    // XT1_IDLE
    //------------------------------------------
    else if (target==XT1_IDLE) {

        // Reset variables
        wakeup_interval   = WAKEUP_INTERVAL_IDLE;

        sub_meas_cnt = 0;
        sub_meas_sum = 0;

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
            eid_update_display(/*seg*/(eid_expected_display & DISP_LOWBATT) | DISP_CHECK);
        // If coming from other states: Remove the Play sign
        else
            eid_update_display(/*seg*/eid_expected_display & ~DISP_PLAY);

        // Hiberation Status
        status.hbr_temp     = 0;
        status.hbr_flag     = 0;
        status.hibernating  = 0;
        status.nfc_out_temp = 0;
        #ifdef DEVEL
            mbus_write_message32(0xBA, (0x14 << 24) | 0x0); // hibernation status
        #endif

        // Reset the System State
        status.curr_state = XT1_IDLE;
        commit_status(/*forced*/1); // it is guaranteed that it is not in hibernation.

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
        commit_status(/*forced*/0);

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

    // Sub-measurements
    sub_meas_cnt    = 0;
    sub_meas_sum    = 0;

    // Reset the Sample Count
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SAMPLE_COUNT, /*data*/0, /*nb*/4);
    meas_cnt    = 0;
    sample_cnt  = 0;
    hbr_meas_cnt= 0;

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
    status.hbr_temp     = 0;
    status.hbr_flag     = 0;
    status.hibernating  = 0;
    status.nfc_out_temp = 0;
    #ifdef DEVEL
        mbus_write_message32(0xBA, (0x11 << 24) | 0x0 /*status.disp_skipped*/);
        mbus_write_message32(0xBA, (0x14 << 24) | 0x0); // hibernation status
    #endif

    // XT1_RESET
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

        // Other variables
        disp_refresh_interval    = 720;    // Every 12 hrs
        eid_duration             = 100;
        eid_updated_at_low_temp  = 0;
        eid_updated_at_crit_temp = 0;
        eid_update_type          = EID_GLOBAL;
        eid_seg_in_eeprom        = 0;
        eid_expected_display     = 0;
        meas.valid               = 0;
        ssls                     = 0;
        #ifdef ENABLE_DEBUG_SYSTEM_CONFIG
            override.enable          = 0;
        #endif
    }

    // Following variables are not set/reset here
    //-----------------------------------------------------------
    // EEPROM_ADDR_SYSTEM_STATE (status)
    // EEPROM_ADDR_DISPLAY
    // EEPROM_ADDR_LAST_ADC
    // EEPROM_ADDR_LAST_SAR

}

static void commit_status(uint32_t forced) {
    //----------------------------------------------
    // If forced = 1
    //      It commits the status regardless of meas.valid or hibernation status.
    //          User needs to make sure that the unit is NOT in hibernation or below nfc_out_temp.
    // If forced = 0
    //      It commits the status ONLY IF meas.valid=1 and NOT in hibernation status.
    //      Otherwise, it sets status.commit_status_pend=1 and returns.
    //----------------------------------------------
    if (forced || (meas.valid && !status.hbr_flag) ) {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SYSTEM_STATE, /*data*/status.eeprom_state, /*nb*/1);
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_FAIL, /*data*/status.eeprom_fail, /*nb*/1);
        status.commit_status_pend = 0;
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
    else {
        // operation_sleep() checks status.commit_status_pend, and if it is 1, then it calls commit_status().
        status.commit_status_pend = 1;
    }
    #ifdef DEVEL
        mbus_write_message32(0xBA, (0x15 << 24) | status.commit_status_pend);
    #endif
}

static void meas_temp_adc (uint32_t go_sleep) {
    #ifdef DEVEL
        mbus_write_message32(0x91, go_sleep);
    #endif

    if (!meas.valid) {

        if (go_sleep) {
            if (!get_flag(FLAG_TEMP_MEAS_PEND)) {

                meas.adc = pmu_read_adc();

                // Higher Floor Setting for Sleep
                pmu_sleep_floor(/*type*/1);

                // Reset the register
                *SNT_TARGET_REG_ADDR = 0xFFFFFF;

                // Turn on SNT Temperature Sensor
                snt_temp_sensor_power_on();

                // Set flag
                set_flag(FLAG_TEMP_MEAS_PEND, 1);

                // Release the reset for the Temp Sensor
                snt_temp_sensor_start();

                // go sleep
                mbus_sleep_all(); 
                while(1);
            }
            else {

                // Get back to the original Floor Setting
                pmu_sleep_floor(/*type*/0);

                set_flag(FLAG_TEMP_MEAS_PEND, 0);
            }
        }
        else {

            meas.adc = pmu_read_adc();

            // Enable REG IRQ
            enable_reg_irq(SNT_TARGET_REG_IDX); // REG1

            // Turn on SNT Temperature Sensor
            snt_temp_sensor_power_on();

            // Release the reset for the Temp Sensor
            start_timeout32_check(/*id*/FAIL_ID_SNT, /*val*/TIMEOUT_TH<<2); // ~2s

            // Release the reset for the Temp Sensor
            snt_temp_sensor_start();

            WFI();
        }

        // Raw value
        temp.raw = (*SNT_TARGET_REG_ADDR) & 0xFFFF;

        // Reset & Turn off the temperature sensor
        snt_temp_sensor_power_off();

        // Hibernation Control (Set status.hbr_flag)
        // -------------------------------------------------------------------------
        // If in XT1_PEND or XT1_ACTIVE
        #ifdef DEVEL
            mbus_write_message32(0xBA, (0x14 << 24) | (status.hbr_temp << 12) | (status.hbr_flag << 8) | (status.hibernating << 4) | (status.nfc_out_temp << 0) );
            mbus_write_message32(0x9F, hbr_temp_threshold_raw);
        #endif
        if ((xt1_state == XT1_PEND) || (xt1_state == XT1_ACTIVE)) {

            temp.val = 0; // Default Value

            // current temp is below the hibernation threshold
            if (temp.raw < hbr_temp_threshold_raw) {
                status.hbr_temp = 1;
                status.hbr_flag = 1;
            }
            // current temp is high
            else {
                status.hbr_temp = 0;

                // If this is the first sub-measurement, reset hbr_flag.
                if (status.hbr_flag && (sub_meas_cnt==0)) {
                    status.hbr_flag = 0;
                }

                // Translate the temperature if needed
                if (!status.hbr_flag) {
                    temp.val = tconv(   /* dout */ temp.raw,
                                        /*   a  */ eeprom_temp_calib_a, 
                                        /*   b  */ eeprom_temp_calib_b, 
                                        /*offset*/ COMP_OFFSET_K);
                }
            }

            // NFC chip temperature check 
            // NOTE1: __NFC_OP_MIN_TEMP__ must be higher than the hibernation threshold temperature.
            // NOTE2: status.hbr_flag=1 guarantees temp.val=0. 
            //        Otherwise, temp.val should have a reasonable value and should be compared against __NFC_OP_MIN_TEMP__.
            status.nfc_out_temp = (temp.val < __NFC_OP_MIN_TEMP__);

        }
        // If NOT in (XT1_PEND | XT1_ACTIVE)
        else {
            // Translate the temperature
            temp.val = tconv(   /* dout */ temp.raw,
                                /*   a  */ eeprom_temp_calib_a, 
                                /*   b  */ eeprom_temp_calib_b, 
                                /*offset*/ COMP_OFFSET_K);
            status.nfc_out_temp = 0;
        }
        #ifdef DEVEL
            mbus_write_message32(0x90, temp.value);
            mbus_write_message32(0xBA, (0x14 << 24) | (status.hbr_temp << 12) | (status.hbr_flag << 8) | (status.hibernating << 4) | (status.nfc_out_temp << 0) );
        #endif

        // -------------------------------------------------------------------------
        

        // PMU adjustment is done only after initialization (FLAG_INITIALIZED=1)
        if (get_flag(FLAG_INITIALIZED)) {
            // Determine ADC Offset, ADC Low VBAT threshold, and ADC Critical VBAT threshold
        #ifdef DEVEL
            uint32_t adc_offset = 0;
        #else
            uint32_t adc_offset;
            if      (temp.val > eeprom_adc_th_0.t1) { adc_offset = eeprom_adc_0.offset_r1; }
            else if (temp.val > eeprom_adc_th_0.t2) { adc_offset = eeprom_adc_0.offset_r2; }
            else if (temp.val > eeprom_adc_th_1.t3) { adc_offset = eeprom_adc_0.offset_r3; }
            else if (temp.val > eeprom_adc_th_1.t4) { adc_offset = eeprom_adc_0.offset_r4; }
            else                                    { adc_offset = eeprom_adc_1.offset_r5; }
        #endif
    
            // VBAT Measurement and SAR_RATIO Adjustment
        #ifdef SKIP_SAR_IF_HIBERNATION
            if (status.hbr_flag) {
            #ifdef DEVEL
                mbus_write_message32(0x91, 0x00000002);
            #endif
                meas.sar = pmu_get_sar_ratio();
            }
            else {
        #endif
                meas.sar = pmu_calc_new_sar_ratio( /*meas.adc*/     meas.adc,
                                                   /*offset*/       adc_offset
                                                );
        #ifdef SKIP_SAR_IF_HIBERNATION
            }
        #endif
    
            meas.low_vbat  = pmu_check_low_vbat (   /*meas.adc*/        meas.adc,
                                                    /*offset*/          adc_offset, 
                                                    /*threshold*/       eeprom_adc_1.low_vbat
                                                    );
    
            meas.crit_vbat = pmu_check_crit_vbat(   /*meas.adc*/        meas.adc,
                                                    /*offset*/          adc_offset, 
                                                    /*threshold*/       eeprom_adc_1.crit_vbat
                                                   );
    
            #ifdef DEVEL
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
    
                #ifdef ENABLE_EID_WATCHDOG
                    // Trigger the Crash Handler
                    eid_trigger_crash();
                #else
                    // Display the BATT sign only, mimicking the crash handler behavior.
                    eid_update_display(/*seg*/DISP_LOWBATT);
                #endif
    
                // Go to indefinite sleep
                snt_set_wup_timer(/*auto_reset*/1, /*threshold*/0, /*altval*/0);
                operation_sleep(/*check_snt*/0);
            }
            /////////////////////////////////////////////////////////////////

        } // if (get_flag(FLAG_INITIALIZED))

        // During boot-up
        else {
            // During boot-up, set some default values.
            meas.valid = 1;
            meas.low_vbat = 0;
            meas.crit_vbat = 0;
        }

        // Temp-related settings
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

    // Read the current display
    uint32_t curr_seg = eid_expected_display; // Start from eid_expected_display, NOT __eid_current_display__
    uint32_t new_seg = curr_seg;
    uint32_t display_update_allowed = 0;

    // XT_ACTIVE (Sample Average & Buffer Write)
    if (xt1_state == XT1_ACTIVE) {

    #ifdef DEVEL
        mbus_write_message32(0xBA, (0x14 << 24) | (status.hbr_temp << 12) | (status.hbr_flag << 8) | (status.hibernating << 4) | (status.nfc_out_temp << 0) );
    #endif

        // Increment the sub-measurement counter
        sub_meas_cnt++;
        #ifdef DEVEL
            mbus_write_message32(0x92, (0x1<<28)|sub_meas_cnt);
        #endif

        if (!status.hbr_flag) {
            sub_meas_sum += temp.val;
            #ifdef DEVEL
                mbus_write_message32(0x92, (0x2<<28)|sub_meas_sum);
            #endif
        }

        if (// If this is the very first sample (right after the start delay)
            (meas_cnt==0)
            // At User-defined measurement interval (Calculate an Averaged Sample)
           || (sub_meas_cnt==eeprom_user_config_1.temp_meas_interval)) {

            display_update_allowed = 1;

            // Increment the Measurement Count
            meas_cnt++;
            cnt_buf_meas.meas_cnt = meas_cnt;
            cnt_buf_meas.valid = 1;

            #ifdef DEVEL
                mbus_write_message32(0x92, (0x3<<28)|meas_cnt);
            #endif

            // If hbr_temp has been seen
            if (status.hbr_flag) {

                // Beginning of the hibernation
                if (!status.hibernating) {
                    status.hibernating = 1;
                    hbr_meas_cnt = 1;
                    #ifdef DEVEL
                        mbus_write_message32(0x91, 0x00000004);
                        mbus_write_message32(0x92, (0x4<<28)|hbr_meas_cnt);
                        mbus_write_message32(0xBA, (0x14 << 24) | (status.hbr_temp << 12) | (status.hbr_flag << 8) | (status.hibernating << 4) | (status.nfc_out_temp << 0) );
                    #endif

                }
                // Keep hibernating
                else {
                    hbr_meas_cnt++;
                    #ifdef DEVEL
                        mbus_write_message32(0x91, 0x00000005);
                        mbus_write_message32(0x92, (0x4<<28)|hbr_meas_cnt);
                    #endif
                }

                // Hibernation is considered a low excursion
                if ((eeprom_user_config_0.low_temp_threshold!=0) // If low-excursion check is enabled
                    && (tmp_exc.num_cons_lo!=0x7FFF) // prevent roll-over
                ){
                    tmp_exc.num_cons_lo++;
                    #ifdef DEVEL
                        mbus_write_message32(0x9D, (0x2<<28) | (0x0<<24) | tmp_exc.num_cons_lo);
                    #endif
                }

                // Reset sub-measurement stat
                sub_meas_cnt = 0;
                sub_meas_sum = 0;

            } // if (status.hbr_flag)

            // If hbr_temp has NOT been seen
            else {

                // Calculate the average
                temp.val = div(/*numer*/sub_meas_sum, /*denom*/sub_meas_cnt, /*n*/1);
                if (temp.val&0x1) temp.val += 0x2;  // round-up
                temp.val >>= 1;
                sub_meas_cnt = 0;
                sub_meas_sum = 0;

                #ifdef DEVEL
                    if (status.sample_type==SAMPLE_NORMAL) mbus_write_message32(0x9C, temp.val);
                #endif

                // Check Excursions / Alarm
                uint32_t new_alarm = 0;

                // If this is the one right after a hibernation: check the low excursion.
                //  By definition, it is guaranteed that meas_cnt > 1
                uint32_t new_alarm_hbr = 0;
                if ((status.hibernating) 
                    && (tmp_exc.num_cons_lo>=num_cons_excursions)
                ){
                    new_alarm_hbr = 1; // to update the display
                    new_alarm     = 1; // to dump the buffer
                    #ifdef DEVEL
                        mbus_write_message32(0x91, 0x00000006);
                    #endif
                }

                // High/Low excursion checks
            #ifdef IGNORE_FIRST_SAMPLE_EXCURSION
                if (meas_cnt>1) {
            #endif
                    if (temp.val > eeprom_user_config_0.high_temp_threshold) {
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
                    else if (temp.val < eeprom_user_config_0.low_temp_threshold) {
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
            #ifdef IGNORE_FIRST_SAMPLE_EXCURSION
                }
            #endif

                #ifdef DEVEL
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


            #ifdef ENABLE_RAW
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

                        uint32_t byte_addr = (sample_cnt<<1)+EEPROM_ADDR_DATA_RESET_VALUE;

                        // Store the Sample Count
                        nfc_i2c_byte_write(/*e2*/0, 
                            /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, 
                            /*data*/ (sample_cnt+1),
                            /* nb */ 4
                            );

                        // Store the raw data
                        //  Max Sample Count = (8192 - 128) / 2 = 4032
                        //  However, we split this into two and store the raw data into the first half,
                        //  and SAR/ADC values in to the second half.
                        //  i.e., max sample count = 4032 / 2 = 2016
                        //      Byte# 128 - Byte#4159: Raw Data      (each sample is 16-bit)
                        //      Byte#4160 - Byte#8192: SAR/ADC Value (each sample is 16-bit)
                        if (sample_cnt<2016) {
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

                        #ifdef DEBUG_RAW
                            mbus_write_message32(0x9C, byte_addr);
                            mbus_write_message32(0x9D, temp.raw);
                            mbus_write_message32(0x9E, sar_adc_addr);
                            mbus_write_message32(0x9F, sar_adc_data);
                        #endif

                    }

                    sample_cnt++;
                }

                else {
            #endif

                //--------------------------------------------------------------------------------
                // CONVERTED & COMPRESSED DATA MODE
                //--------------------------------------------------------------------------------

                // sample_type: SAMPLE_NORMAL

                    // Store ADC Reading and SAR_RATIO
                    pmu_buf.adc = meas.adc;
                    pmu_buf.sar = meas.sar;
                    pmu_buf.valid = 1;

                    uint32_t num_bits;
                    uint32_t reverse_code[2];   // reverse_code[0] = bit[31:0]
                                                // reverse_code[1] = bit[63:32]

                    //--- Store the Uncompressed Data (128*N-th samples)
                    if (comp_sample_id==0) {
                        if (status.hibernating) {
                            num_bits = COMP_UNCOMP_NUM_BITS + 23 + COMP_UNCOMP_NUM_BITS; // hbr_meas_cnt is 23-bit wide.
                            reverse_bits_ext(/*ptr*/reverse_code, /*code*/0x0, /*count*/hbr_meas_cnt, /*value*/temp.val);
                        }
                        else {
                            num_bits = COMP_UNCOMP_NUM_BITS;
                            reverse_code[0] = reverse_bits(/*bit_stream*/temp.val, /*num_bits*/COMP_UNCOMP_NUM_BITS);
                            reverse_code[1] = 0;
                        }
                    }
                    //--- For non-128*N-th samples)
                    else {
                        if (status.hibernating) {
                            num_bits = 6 + 23 + COMP_UNCOMP_NUM_BITS; // hbr_meas_cnt is 23-bit wide.
                            reverse_bits_ext(/*ptr*/reverse_code, /*code*/0x20, /*count*/hbr_meas_cnt, /*value*/temp.val);
                        }
                        else {
                            uint32_t comp_result = tcomp_encode(/*delta*/temp.val - comp_prev_sample);
                            num_bits = comp_result >> 24;
                            reverse_code[0] = comp_result&0xFFFFFF;
                            reverse_code[1] = 0;
                        }
                    }
                    //--- Store the current temperature in comp_prev_sample
                    comp_prev_sample = temp.val;

                    #ifdef DEVEL
                        mbus_write_message32(0x97, num_bits);
                        mbus_write_message32(0x98, reverse_code[1]);
                        mbus_write_message32(0x99, reverse_code[0]);
                    #endif

                    // Find out the byte address of comp_bit_pos
                    uint32_t bit_pos_start = comp_bit_pos;
                    uint32_t bit_pos_end   = bit_pos_start + num_bits - 1;
                    uint32_t eeprom_avail  = (bit_pos_end < 64512); // NOTE: 64512 = (8192 - 128) * 8
                    uint32_t last_qword_id = eeprom_avail ? (bit_pos_end>>7) : 503;

                    // Store the Sample Count & Last Qword ID
                    sample_cnt++;
                    cnt_buf_sample.sample_cnt    = sample_cnt;
                    cnt_buf_sample.last_qword_id = last_qword_id;

                    #ifdef DEVEL
                        mbus_write_message32(0xBB, (bit_pos_start<<16)|(bit_pos_end&0xFFFF));
                        mbus_write_message32(0xBC, (last_qword_id<<24)|(sample_cnt&0xFFFFFF));
                    #endif

                    // MEMORY IS AVAILABLE
                    if (eeprom_avail) {

                        // CASE I ) Entire bits go into the same qword (bit_pos_start>>7 == bit_pos_end>>7)
                        if ((bit_pos_start>>7)==(bit_pos_end>>7)) {
                            uint32_t end_at_bndry = ((bit_pos_end&0x7F)==0x7F);

                            sub_qword(/*msb*/bit_pos_end&0x7F, /*lsb*/bit_pos_start&0x7F, /*ptr*/reverse_code);
                            data_buffer_update(  /*addr*/ ((bit_pos_start>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                            /*commit_crc*/ end_at_bndry
                                            );

                            if (end_at_bndry) {
                                all_buffer_commit();
                                set_qword(/*pattern*/0xFFFFFFFF);
                            }
                        }

                        // CASE II) Bits go across different qwords (bit_pos_start>>7 != bit_pos_end>>7)
                        else {
                            sub_qword(/*msb*/127, /*lsb*/bit_pos_start&0x7F, /*ptr*/reverse_code);
                            data_buffer_update(  /*addr*/ ((bit_pos_start>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                            /*commit_crc*/ 1
                                            );

                            all_buffer_commit();

                            set_qword(/*pattern*/0xFFFFFFFF);

                            uint32_t shift_amt = 128-(bit_pos_start&0x7F);
                            if (shift_amt > 31) {
                                reverse_code[0] = reverse_code[1]>>(shift_amt-32);
                                reverse_code[1] = 0;
                            }
                            else {
                                reverse_code[0] = (reverse_code[1]<<(32-shift_amt)) | (reverse_code[0]>>shift_amt);
                                reverse_code[1] = reverse_code[1]>>shift_amt;

                            }
                            sub_qword(/*msb*/bit_pos_end&0x7F, /*lsb*/0, /*ptr*/reverse_code);
                            data_buffer_update(  /*addr*/ ((bit_pos_end>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                            /*commit_crc*/ 0    // Assuming that 'reverse_code' is less than 128 bits.
                                            );
                        }

                        comp_sample_id++;
                        if (comp_sample_id==COMP_SECTION_SIZE) comp_sample_id = 0;
                        comp_bit_pos = bit_pos_end + 1;

                        // Dump the buffer when a new alarm occurs
                        if (new_alarm) all_buffer_commit();
                    }
                    // MEMORY GETS FULL (Roll-over is disabled)
                    else if (comp_bit_pos != 65536) {
                        comp_bit_pos = 65536;
                        // Buffer Commit
                        all_buffer_commit();
                        // Update Memory Full Flag
                        status.memory_full = 1;
                        commit_status(/*forced*/1); // it is guranteed that it is not in hibernation.
                    }

            #ifdef ENABLE_RAW
                }
            #endif

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
                    curr_seg = DISP_PLAY; // pretend that the display is showing the play-sign only
                    new_seg = DISP_PLAY;
                    status.disp_skipped = 0;
                    #ifdef DEVEL
                        mbus_write_message32(0xBA, (0x11 << 24) | 0x0 /*status.disp_skipped*/);
                    #endif
                }

                if (new_alarm_hbr || tmp_exc.alarm_hi || tmp_exc.alarm_lo) {
                    //--- High Temperature Alarm
                    if (tmp_exc.alarm_hi) {
                        new_seg &= ~DISP_CHECK;     // Remove Check
                        new_seg |= DISP_CROSS;      // Add Cross
                        new_seg |= DISP_PLUS;       // Add Plus
                    }
                    //--- Low Temperature Alarm
                    if (tmp_exc.alarm_lo || new_alarm_hbr ) {
                        new_seg &= ~DISP_CHECK;     // Remove Check
                        new_seg |= DISP_CROSS;      // Add Cross
                        new_seg |= DISP_MINUS;      // Add Minus
                    }
                }
                //--- Normal Temperature
                else if (!get_bit(curr_seg, SEG_PLUS) && !get_bit(curr_seg, SEG_MINUS)) {
                    new_seg |= DISP_CHECK;      // Add Check
                }

                // Reset status.hibernating
                #ifdef DEVEL
                    if (status.hibernating) mbus_write_message32(0x91, 0x00000007);
                #endif
                status.hibernating = 0;

            } // if (!status.hbr_flag)

        } // if (@ user_defined_interval)

    } // if (xt1_state == XT1_ACTIVE)

    //--- Show Low VBATT Indicator (if needed)
    //      It does not check the low batt while in XT1_PEND
    if ((xt1_state != XT1_PEND) && !get_bit(new_seg, SEG_LOWBATT) && meas.low_vbat) {
        new_seg = new_seg | DISP_LOWBATT;
    }

    /////////////////////////////////////////////////////////////////
    // Update/Refresh the e-Ink Display if needed
    // NOTE: If status.hbr_flag=1, then eid_update_display() updates flags only; 

    //--- Update
    if (new_seg!=__eid_current_display__) {
        // If in XT1_ACTIVE, the display can be changed only at measurement intervals
        if (xt1_state == XT1_ACTIVE) {
            if (display_update_allowed) eid_update_display(/*seg*/new_seg);
        }
        // If NOT in XT1_ACTIVE, the display can be updated anytime the system wakes up
        else {
            eid_update_display(/*seg*/new_seg);
        }
    }
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
#ifdef WAKEUP_INTERVAL_IN_MINUTES // Normal Operation: Intervals are given in minutes
       else if (// Display has a pending update due to a critical temperature
                (eid_updated_at_crit_temp && (temp.val > eid_crit_temp_threshold))
                // Display has been updated at a low temperature
             || (eid_updated_at_low_temp && (temp.val > eeprom_eid_threshold.low))
                // User-configured Refresh interval
             || ((disp_refresh_interval != 0) && ((disp_min_since_refresh + wakeup_interval) > disp_refresh_interval)) 
             ){
#else // For debugging: Intervals are given in seconds
        else { // force refresh
#endif
           eid_update_display(/*seg*/eid_expected_display);
       }
    }
}

// NOTE: If status.hbr_flag=1, then eid_update_display() within display_low_batt() sets eid_updated_at_crit_temp=1
static void display_low_batt(void) {
    #ifdef DEVEL
        mbus_write_message32(0xA8, 0x00000000);
    #endif

    if (!get_bit(eid_expected_display, SEG_LOWBATT)) {
        eid_update_display(eid_expected_display | DISP_LOWBATT);
    }
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

    // Fail check
    if (!status.hbr_flag) {

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
                    if (skip_calib||MAIN_CALLED_BY_SNT||get_flag(FLAG_INVLD_XO_PREV)) {
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
#ifdef WAKEUP_INTERVAL_IN_MINUTES // Normal Operation: Intervals are given in minutes
    uint32_t wakeup_interval_sec = (wakeup_interval << 6) - (wakeup_interval << 2);
#else // For debugging: Intervals are given in seconds
    uint32_t wakeup_interval_sec = wakeup_interval;
#endif
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
            
    #ifdef ENABLE_RAW
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
    #endif

        // Restart the XO
        nfc_power_off(); // Turn off the NFC
        restart_xo(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

    #ifdef ENABLE_XO_PAD
        start_xo_cout();
    #endif

        // Calculate the next threshold
        if (skip_calib) {
            snt_threshold_prev = snt_read_wup_timer(/*check_restart*/0);
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
        snt_threshold_prev = snt_read_wup_timer(/*check_restart*/0);
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
    // MAIN_CALLED_BY_SNT
    else if (MAIN_CALLED_BY_SNT||get_flag(FLAG_INVLD_XO_PREV)) {
        #ifdef DEVEL
            if (MAIN_CALLED_BY_SNT) mbus_write_message32(0x83, 0xA);
            else             mbus_write_message32(0x83, 0xB);
        #endif

        // Update the next duration
        snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq);

        // Calculate the next threshold
        snt_threshold += snt_duration;

        if (MAIN_CALLED_BY_SNT) 
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

    uint32_t snt_curr_val = snt_read_wup_timer_check_restart();
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

    result = (sub_meas_cnt << 6) - (sub_meas_cnt << 2)                       // Full-minutes (converted to seconds) from the last 'measurement'
             + (div(/*numer*/snt_delta, /*denom*/snt_freq, /*n*/0)&0xFFFF);  // Seconds from the last 'wake-up'

    #ifdef DEVEL
        mbus_write_message32(0xCC, snt_threshold_prev);
        mbus_write_message32(0xCD, snt_curr_val);
        mbus_write_message32(0xCE, snt_delta);
        mbus_write_message32(0xCF, result);
    #endif

    return result;
}

static uint32_t check_snt_wakeup_near_threshold(void) {

    // NOTE: We do not need to check the 'early' wakeup,
    //      as it should be correctly handled by the variable 'pretend_wakeup_by_snt'
    //      So I'm commenting out a section below.
    //-----------------------------------------------------------------------------------
    //          uint32_t early_limit = snt_threshold - (snt_freq << 3); // 8s
    //          uint32_t late_limit  = snt_threshold + (snt_freq << 5); // 32s
    //          uint32_t curr_val = snt_read_wup_timer();
    //          // CASE I (early_limit < late_limit)
    //          //
    //          //          0         early  th          late                  MAX
    //          //          |-----------|----|--------------|---------------------|
    //          //                       <=====ALLOWED=====>
    //          //                                            
    //          if (early_limit < late_limit) {
    //              return ((curr_val > early_limit) && (curr_val < late_limit));
    //          }
    //          // CASE II (early_limit >= late_limit)
    //          //
    //          //          0  th           late                           early MAX
    //          //          |--|--------------|-------------------------------|---|
    //          //          ===ALLOWED=======>                                 <==
    //          //                                            
    //          //          0       late                            early  th    MAX
    //          //          |---------|-------------------------------|----|------|
    //          //          =========>                                 <===ALLOWED
    //          //                                            
    //          else {
    //              return ((curr_val > early_limit) || (curr_val < late_limit));
    //          }
    
    // NOTE: Below is the traditional approach used in, for example, 3.08.
    return ((snt_read_wup_timer_check_restart()-snt_threshold)<(snt_freq<<5));
}

static uint32_t snt_read_wup_timer_check_restart(void) {
    uint32_t snt_val = snt_read_wup_timer(/*check_restart*/1);
    if (__snt_timer_restarted__) {
        __snt_timer_restarted__ = 0;
        snt_threshold      = 0;
        snt_threshold_prev = 0;
    }
    return snt_val;
}

//-------------------------------------------------------------------
// E-Ink Display
//-------------------------------------------------------------------

static void eid_update_configs(void) {
    eid_update_type = EID_GLOBAL;
    if (temp.val > eeprom_eid_threshold.high) {
        disp_refresh_interval = EID_REFRESH_INT_MIN_HIGH;
        eid_duration = eeprom_eid_duration_0.high;
    }
    else if (temp.val > eeprom_eid_threshold.low) { 
        disp_refresh_interval = EID_REFRESH_INT_MIN_MID;
        eid_duration = eeprom_eid_duration_0.mid;
    }
    else {
        disp_refresh_interval = EID_REFRESH_INT_MIN_LOW;
        eid_duration = eeprom_eid_duration_1.low;
        #ifndef EID_USE_GLOBAL_UPDATE_ONLY
            eid_update_type = EID_LOCAL;
        #endif
    }

    // Set the refresh interval
    #ifdef USE_SHORT_REFRESH
        disp_refresh_interval = WAKEUP_INTERVAL_IDLE;
    #endif

    // Set the duration
    eid_set_duration(eid_duration);

    #ifdef DEVEL
        mbus_write_message32(0xA2, (eid_duration<<16)|disp_refresh_interval);
    #endif
}

static void eid_update_display(uint32_t seg) {

    eid_expected_display = seg;

    // Make sure you have a valid temperature measurement
    if (__wfi_id__!=FAIL_ID_SNT) meas_temp_adc(/*go_sleep*/0);

    // Turn off NFC, if on, to save power
    nfc_power_off();

    // E-ink Update
    if ((temp.val > eid_crit_temp_threshold) && !status.hbr_flag) {
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

        // Reset the refresh counter
        disp_min_since_refresh = 0;

        // Enable low-power active
        set_high_power_history(1);
    }
    else {
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

    #ifdef DEVEL
        mbus_write_message32(0xA5, eid_updated_at_low_temp);
        mbus_write_message32(0xA6, eid_updated_at_crit_temp);
    #endif

    // Update EEPROM if it is different from the previous status (eid_seg_in_eeprom)
    if ((__wfi_id__!=FAIL_ID_I2C) && (eid_expected_display != eid_seg_in_eeprom) && !status.hbr_flag) {
        // Try up to 30 times - Consistency between the EEPROM log and the actual display is important!
        if(nfc_i2c_get_token(/*num_try*/30)) {
            // EEPROM Update
            nfc_i2c_byte_write(/*e2*/0,
                /*addr*/ EEPROM_ADDR_DISPLAY,
                /*data*/ eid_expected_display,
                /* nb */ 1
            );
            eid_seg_in_eeprom = eid_expected_display;
            #ifdef DEVEL
                mbus_write_message32(0xA0, (0x0<<28) | eid_expected_display);
            #endif
        }
        #ifdef DEVEL
        else {
                mbus_write_message32(0xA0, (0x2<<28) | eid_expected_display);
        }
        #endif
    }
    #ifdef DEVEL
    else {
        mbus_write_message32(0xA0, 0x1 << 28);
    }
    #endif
}

static void eid_blink_display(void) {
    eid_update_display(eid_expected_display);
    #ifdef DEVEL
        mbus_write_message32(0xA0, (0xF<<28) | eid_expected_display);
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
            if (// Normal Sample requires both AES KEY and CONFIG to be valid
                ( status.aes_key_set && status.config_set )
                #ifdef ENABLE_RAW
                    // Raw Sample
                    || (status.sample_type==SAMPLE_RAW)
                #endif
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
                    (status.aes_key_set) // Must be the Raw Sample -OR- AES KEY is set
                    #ifdef ENABLE_RAW
                        || (status.sample_type==SAMPLE_RAW)
                    #endif
                )) {
                // Dump pending data, if any.
                if (status.sample_type==SAMPLE_NORMAL) 
                    all_buffer_commit();
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
                commit_status(/*forced*/1); // It is guaranteed that it is not in hibernation.
            }

            // Unit is currently running
            if ((xt1_state==XT1_PEND) || (xt1_state==XT1_ACTIVE)) {
                // Dump pending data, if any.
                all_buffer_commit();
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
        // CMD: NOP, NEWAB, HRESET
        //-----------------------------------------------------------
        else if (  (cmd == CMD_NOP)
                || (cmd == CMD_NEWAB)
                || (cmd == CMD_HRESET)
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
        #ifndef DEVEL
            //eeprom_adc_th_0.t* = 0;
            //eeprom_adc_th_1.t* = 0;
            eeprom_adc_th_0.value = 0;
            eeprom_adc_th_1.value = 0;
        #endif

        //eeprom_adc_*.offset_r* = 0; // r1 ~ r5
        //eeprom_adc_1.low_vbat  = 105;
        //eeprom_adc_1.crit_vbat = 95;
        eeprom_adc_0.value = 0x00000000;
        eeprom_adc_1.value = 0x005F6900;

        //--- EID Configurations 
        //eeprom_eid_threshold.high = 950; // 15C
        //eeprom_eid_threshold.low  = 750; // -5C
        eeprom_eid_threshold.value = (750 << 16) | (950 << 0);

        //eeprom_eid_duration_0.high        = 60 ; // 0.5s
        //eeprom_eid_duration_0.mid         = 500; // 4s
        //eeprom_eid_duration_1.low         = 500; // 4s
        eeprom_eid_duration_0.value = 0x01F4003C;
        eeprom_eid_duration_1.value = 0x000001F4;

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
        #ifndef DEVEL
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T1,        /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_adc_th_0.value);
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T3,        /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_adc_th_1.value);
        #endif
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R1, /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_adc_0.value);
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R5, /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_adc_1.value);

        //--- EID Configurations
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD, /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_eid_threshold.value);
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_HIGH,       /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_eid_duration_0.value);
        nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_LOW,        /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_eid_duration_1.value);

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
    eid_crit_temp_threshold = ((80-EID_CRIT_TEMP_THRESHOLD)<<3) + ((80-EID_CRIT_TEMP_THRESHOLD)<<1);
}

static void update_temp_coefficients (void) {
    // Read the temperature coefficients
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_A_WRITEONLY, /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_temp_calib_a);
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_B_WRITEONLY, /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_temp_calib_b);

    // Read the hibernation threshold
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_HBR_THRESHOLD_RAW_WRITEONLY, /*nw*/1, /*ptr*/ &hbr_temp_threshold_raw);
    hbr_temp_threshold_raw &= 0xFFFF;

    // Write the calibration info in the designated place
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_A, /*data*/eeprom_temp_calib_a, /*nb*/4);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_B, /*data*/eeprom_temp_calib_b, /*nb*/4);

    // Write the Hibernation Threshold in the designated place
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HBR_THRESHOLD_RAW, /*data*/hbr_temp_threshold_raw, /*nb*/2);

    #ifdef DEBUG_TEMP_CALIB
        mbus_write_message32(0x76, eeprom_temp_calib_a);
        mbus_write_message32(0x77, eeprom_temp_calib_b);
    #endif

    #ifdef DEVEL
        mbus_write_message32(0x9F, hbr_temp_threshold_raw);
    #endif
}

#ifdef ENABLE_DEBUG_SYSTEM_CONFIG
static void debug_system_configs(void) {

    #ifdef DEVEL
        mbus_write_message32(0x73, 0xF);
    #endif

    //--- VBAT and ADC
    #ifndef DEVEL
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T1,
        /*data*/  eeprom_adc_th_0.value,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T3,
        /*data*/  eeprom_adc_th_1.value,
        /* nb */  4);
    #endif

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R1,
        /*data*/  eeprom_adc_0.value,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R5,
        /*data*/  eeprom_adc_1.value,
        /* nb */  3);

    //--- EID Configurations
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD,
        /*data*/  eeprom_eid_threshold.value,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_HIGH,
        /*data*/  eeprom_eid_duration_0.value,
        /* nb */  4);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_LOW,
        /*data*/  eeprom_eid_duration_1.value,
        /* nb */  2);

    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_EID_RFRSH_INT_HR_HIGH,
        /*data*/  eeprom_eid_rfrsh_int_hr.value,
        /* nb */  3);

    //--- Temperature Coefficients
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_A_WRITEONLY, 
        /*data*/  eeprom_temp_calib_a, 
        /* nb */  4);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_B_WRITEONLY, 
        /*data*/  eeprom_temp_calib_b, 
        /* nb */  4);

    //--- Hibernation Threshold
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HBR_THRESHOLD_RAW_WRITEONLY, 
        /*data*/  hbr_temp_threshold_raw, 
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
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_TEMP_THRESHOLD, /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_user_config_0.value);
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_MEAS_INTERVAL,  /*nw*/1, /*ptr*/ (volatile uint32_t *)&eeprom_user_config_1.value);
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CONS_EXCURSIONS, /*nw*/1, /*ptr*/ &num_cons_excursions);

    // Set Flag
    status.config_set = 1;
    commit_status(/*forced*/1); // it is guranteed that it is not in hibernation.

    #ifdef DEVEL
        mbus_write_message32(0x74, eeprom_user_config_0.value);
        mbus_write_message32(0x75, eeprom_user_config_1.value);
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
    #endif
    #ifdef DEBUG_AES_KEY
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
    cnt_buf_meas.value = 0;
    pmu_buf.value = 0;
    dat_buf_addr.value = 0;
}

static void data_buffer_update (uint32_t addr, uint32_t commit_crc) {

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
    dat_buf_addr.addr = addr;
    #ifdef DEVEL
        mbus_write_message32(0xB5, addr);
    #endif
    for(i=0; i<4; i++) {
        buf_storage[i] = text[i];
        #ifdef DEVEL
            mbus_write_message32(0xB6, text[i]);
        #endif
    }

    // Calculate CRC (4 words = 128 bits)
    dat_buf_crc.crc32 = calc_crc32(/*src*/text, /*crc_prev*/crc32, /*nw*/4);
    #ifdef DEVEL
        mbus_write_message32(0xB7, dat_buf_crc.crc32);
    #endif

    // Commit CRC
    if (commit_crc) crc32 = dat_buf_crc.crc32;

    // Set the Buffer Valid bit
    dat_buf_addr.valid = 1;

    // Update unread_sample status
    if (!status.unread_sample) {
        status.unread_sample = 1;
        commit_status(/*forced*/1); // it is guranteed that it is not in hibernation.
    }
}

static void all_buffer_commit(void) {

    #ifdef DEVEL
        mbus_write_message32(0xB8, (cnt_buf_meas.valid<<8) | (pmu_buf.valid<<4) | (dat_buf_addr.valid<<0));
    #endif

    // Counter Buffer
    if (cnt_buf_meas.valid) {
        // Last Qword ID & Sample Count
        nfc_i2c_byte_write(/*e2*/ 0, /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, /*data*/ cnt_buf_sample.value, /*nb*/ 4);
        // Measurement Count
        nfc_i2c_byte_write(/*e2*/ 0, /*addr*/ EEPROM_ADDR_MEAS_COUNT,   /*data*/ cnt_buf_meas.meas_cnt,    /*nb*/ 3);
        // Invalidate the buffer
        cnt_buf_meas.valid = 0;
    }

    // PMU Buffer
    if (pmu_buf.valid) {
        // Last ADC/SAR
        nfc_i2c_byte_write(/*e2*/ 0, /*addr*/ EEPROM_ADDR_LAST_ADC, /*data*/ pmu_buf.sar_adc, /*nb*/ 2);
        // Invalidate the buffer
        pmu_buf.valid = 0;
    }

    // Data Buffer
    if (dat_buf_addr.valid) {
        // Data
        nfc_i2c_word_write(/*e2*/ 0, /*addr*/ dat_buf_addr.addr,  /*data*/ (uint32_t *) buf_storage, /*nw*/ 4);
        // CRC
        nfc_i2c_byte_write(/*e2*/ 0, /*addr*/ EEPROM_ADDR_CRC,  /*data*/ dat_buf_crc.crc32, /*nb*/ 4);
        // Now the buffer is empty
        dat_buf_addr.valid = 0;
    }

    return;
}

//-------------------------------------------------------------------
// Data Compression
//-------------------------------------------------------------------
//  Glossary
//
//      value (11 bits): 
//          A translated temperature, "10 x (T + 80)", 
//              where 'T' is the measured temperature in C. 
//          e.g., if TMC measures 20C (T=20), then the value is 1000 (=10 x (20 + 80))
//
//      sample_id (23 bits): 
//          A 23-bit-long Sample ID. 
//          It starts from 0, and increments by 1 for each measured sample.
//
//      delta (variable length): 
//          Difference between the current value and the previous value. 
//          delta@(sample_id=n) = value@(sample_id=n) - value@(sample_id=n-1). 
//          e.g., delta=1 means the current value is 1 higher than the previous value.
//
//      count (23 bits): 
//          A value of the hibernation counter: TMC goes into hibernation once it measures 
//              a temperature that is lower than a given threshold. During hibernation, 
//              it increments the hibernation count (count) at the given interval. 
//              It starts from 1.
//
//  Storing measured samples
//
//      If sample_id == 128xN (N=0, 1, 2, ..):
//          In general, the full 11-bit value is stored. 
//              e.g., if the measured temperature is 20C, 
//                  then it stores 11'b011_1110_1000.
//          If it is the first measured sample following a hibernation, 
//              it stores 11'b000_0000_0000, followed by count and value. 
//              The sample_id still increments by 1.
//              e.g., if TMC measures 20C (i.e., value=1000) right after 
//                  the hibernation with count=10, and if it is the 128N-th sample, 
//                  then it stores 
//                  {11'b000_0000_0000, 23'b000_0000_0000_0000_0000_1010, 11'b011_1110_1000}
//
//      If sample_id != 128xN (N=0, 1, 2, ..):
//          In general, it stores a "Encoded Bit Stream" shown in the table below.
//          If it is the first measured sample following a hibernation, 
//              it stores 6'b10_0000, followed by count and value. 
//              The sample_id still increments by 1.
//              e.g., if TMC measures 20C (i.e., value=1000) right after the 
//                  hibernation with count=10, and if it is NOT the 128N-th sample, 
//                  then it stores 
//                  {6'b10_0000, 23'b000_0000_0000_0000_0000_1010, 11'b011_1110_1000}
//
//
//-----------------------------------------------
//  For                Encoded Bit
//  sample_id!=128N    Stream                  
//-----------------------------------------------
//  First sample
//  after  hibernation {100000, 23-bit count, 11-bit value}      
//
//  delta=-2047~-9     {100001, 11-bit |delta|}
//  delta=-8           10001
//  delta=-7           10010
//  delta=-6           10011
//  delta=-5           10100
//  delta=-4           10101
//  delta=-3           10110
//  delta=-2           10111
//  delta=-1           010
//  delta= 0           00
//  delta=+1           011
//  delta=+2           11000
//  delta=+3           11001
//  delta=+4           11010
//  delta=+5           11011
//  delta=+6           11100
//  delta=+7           11101
//  delta=+8           11110
//  delta=+9~+2047     {11111, 11-bit delta}
//-----------------------------------------------

// return value:
// [31:24] Number of valid bits in the Encoded code
// [23: 0] Encoded code (bit reversed)
uint32_t tcomp_encode (uint32_t delta) {
    uint32_t num_bits;
    uint32_t code;

    uint32_t sign = (delta >> 31);  // 0: positive, 1: negative
    // Make 'delta' an absolute number
    if (sign) delta = (0xFFFFFFFF - delta) + 1;

    // delta=0: code=00
    if (delta == 0) {
        num_bits = 2; code = 0x0;
    }
    // delta=1: code=010(-) or 011(+)
    else if (delta == 1) {
        num_bits = 3; code = sign ? 0x2 : 0x3;
    }
    // delta=2~8:
    else if (delta < 9) {
        num_bits = 5; 
        code = 0x8 | (delta - 2);
        if (sign) code = ~code; // Bit negation for negative numbers.
        else      code |= 0x10; // For positive numbers
    }
    // delta > 8
    else {
        // negative
        if (sign) {
            num_bits = 17;
            code = (0x21 << 11) | (delta & 0x7FF);
        }
        // positive
        else {
            num_bits = 16;
            code = (0x1F << 11) | (delta & 0x7FF);
        }
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


void reverse_bits_ext (uint32_t* ptr, uint32_t code, uint32_t count, uint32_t value) {

    uint32_t reversed_value = reverse_bits(/*bit_stream*/value, /*num_bits*/11);
    uint32_t reversed_count = reverse_bits(/*bit_stream*/count, /*num_bits*/23);

    // For 128xN-th sample (code = 11'b000_0000_0000)
    //      result[44:34] = (reversed)value[10:0]
    //      result[33:11] = (reversed)count[22:0]
    //      result[10:0]  = (reversed)code[10:0]
    //      -------------------------------------
    //      upper[12: 2] = (reversed)value[10:0]
    //      upper[ 1: 0] = (reversed)count[22:21]
    //      lower[31:11] = (reversed)count[20:0]
    //      lower[10: 0] = (reversed)code[10:0]
    if (code == 0x0) {
        *(ptr+1) = (reversed_value <<  2) | get_bits(/*variable*/reversed_count, /*msb_idx*/22, /*lsb_idx*/21);
        *(ptr+0) = (reversed_count << 11) | /*code*/0x000;
    }
    // For non-128xN-th sample (code = 6'b10_0000)
    //      result[39:29] = (reversed)value[10:0]
    //      result[28:6]  = (reversed)count[22:0]
    //      result[5:0]   = (reversed)code[5:0]
    //      -------------------------------------
    //      upper[ 7: 0] = (reversed)value[10:3]
    //      lower[31:29] = (reversed)value[2:0]
    //      lower[28: 6] = (reversed)count[22:0]
    //      lower[ 5: 0] = (reversed)code[5:0]
    else {
        *(ptr+1) = get_bits(/*variable*/reversed_value, /*msb_idx*/10, /*lsb_idx*/3);
        *(ptr+0) = (reversed_value << 29) | (reversed_count << 6) | /*code*/0x01;
    }
}

//-------------------------------------------------------------------
// 128-bit qword Handling
//-------------------------------------------------------------------

void set_qword (uint32_t pattern) {
    uint32_t i;
    for (i=0; i<4; i++) *(qword+i) = pattern;
}

void sub_qword (uint32_t msb, uint32_t lsb, uint32_t* ptr) {
// Valid Range: msb= 0 - 127
//              lsb= 0 - 127
//
    uint32_t num_bits = msb - lsb + 1;
    uint32_t wid = (lsb>>5);    // Word ID
    uint32_t lsb0 = lsb&0x1F;   // LSB within a word
    uint32_t nb;                // Num Bits to Write
    uint32_t val_l = *(ptr+0);
    uint32_t val_u = *(ptr+1);

    while (num_bits>0) {
        nb = (num_bits > (32 - lsb0)) ? (32 - lsb0) : num_bits;
        *(qword + wid) = set_bits(/*var*/*(qword+wid), /*msb_idx*/lsb0 + nb - 1, /*lsb_idx*/lsb0, /*val_l*/val_l);
        wid++;
        val_l = (val_u << (32-nb)) | (val_l >> nb);
        val_u = val_u >> nb;
        num_bits -= nb;
        lsb0 = 0;
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

uint32_t calc_crc32(uint32_t* src, uint32_t crc_prev, uint32_t nw) {
    uint32_t a = crc_prev&0xFFFF;
    uint32_t b = crc_prev>>16;
    uint32_t word, i, j;

    for(i=0; i<nw; i++) {
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

    // No fail check
    if (id == FAIL_ID_SOFT) return;

    // Commit result?
    uint32_t commit_result = get_flag(FLAG_INITIALIZED);

    if (id==FAIL_ID_INIT) {
        #ifdef DEVEL
            mbus_write_message32(0xEF, FAIL_ID_INIT);
        #endif
        id = status.fail_id;
        commit_result = 1;
    }

    #ifdef DEVEL
        mbus_write_message32(0xEF, id);
    #endif
    uint32_t disp_pattern;
    status.fail_id |= id;

#ifndef DEVEL

    // Timeout during display update
    if (id==FAIL_ID_EID) {
        disp_pattern = 0;   // No display update if the timeout is from EID
    }
    // Timeout during SNT Timer read
    else if (id==FAIL_ID_WUP) {
        disp_pattern = DISP_LOWBATT|DISP_BACKSLASH|DISP_PLUS;
    }
    // Timeout during PMU register access
    else if (id==FAIL_ID_PMU) {
        disp_pattern = DISP_LOWBATT|DISP_BACKSLASH|DISP_PLUS|DISP_MINUS;
    }
    // Timeout during SNT temperature measurement
    else if (id==FAIL_ID_SNT) {
        disp_pattern = DISP_LOWBATT|DISP_BACKSLASH|DISP_PLAY;
    }
    // I2C NAK Failure
    else if (id==FAIL_ID_I2C) {
        disp_pattern = DISP_LOWBATT|DISP_BACKSLASH|DISP_PLAY|DISP_MINUS;
    }
    // Meta-Stability (SNT Timer Reading)
    else if (id==FAIL_ID_MET) {
        disp_pattern = DISP_LOWBATT|DISP_BACKSLASH|DISP_PLAY|DISP_PLUS;
    }
    // Timeout during SNT Timer Threshold Update
    else if (id==FAIL_ID_WTH) {
        disp_pattern = DISP_LOWBATT|DISP_BACKSLASH|DISP_PLAY|DISP_PLUS|DISP_MINUS;
    }
    // Generic/Unknown Timeout
    else {
 //       #ifdef DEVEL
 //           mbus_write_message32(0xEF, 0x00000000);
 //       #endif
        disp_pattern = DISP_LOWBATT|DISP_BACKSLASH;
    }
#else
        disp_pattern = DISP_LOWBATT|DISP_BACKSLASH;
#endif

    if (commit_result) {
        // Commit the Status
        if (id!=FAIL_ID_I2C) commit_status(/*forced*/1); // FORCED

        // Display
        if (id!=FAIL_ID_EID) eid_update_display(/*seg*/disp_pattern);

        // Turn off everything; Stay in sleep indefinitely.
        operation_dead();
    }
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
    // If __wfi_id__=FAIL_ID_SOFT, fail_handler() immediately returns here. 
    // Change it to FAIL_ID_PEND to notify the caller.
    __wfi_id__ = FAIL_ID_PEND;
}
void handler_ext_int_wfi (void) {
    // REG0: PMU
    // REG1: SNT (Temperature Sensor)
    // REG2: EID Display Update
    // REG5: SNT (Wakeup Timer)
    // AES : AES
    disable_all_irq_except_timer32();
    *TIMER32_GO = 0;
    __wfi_id__ = FAIL_ID_NONE;
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

    // Get the info on who woke up the system, then reset WAKEUP_SOURCE register.
    pretend_wakeup_by_snt = 0;
    if (get_flag(FLAG_MAIN_CALLED_BY_SNT)) {
        wakeup_source = 0x90; // Treat it as if the SNT timer has woken up the system. Set bit[7] as well. See WAKEUP_SOURCE.
        pretend_wakeup_by_snt = 1;
        set_flag(FLAG_MAIN_CALLED_BY_SNT, 0);
    }
    else if (get_flag(FLAG_MAIN_CALLED_BY_NFC)) {
        wakeup_source = 0xF80; // Treat it as if the NFC has woken up the system. DEVEL and STD versions have different NFC connections, so just set wakeup_source[11:8]=0xF. Also set bit[7] as well. See WAKEUP_SOURCE.
        set_flag(FLAG_MAIN_CALLED_BY_NFC, 0);
    }
    else {
        wakeup_source = *SREG_WAKEUP_SOURCE;
    }
    //*SCTR_REG_CLR_WUP_SOURCE = 1; // OBSOLETE

    #ifdef DEVEL
        mbus_write_message32(0x70, wakeup_source);
        mbus_write_message32(0x71, xt1_state);
    #endif

    if (!get_flag(FLAG_TEMP_MEAS_PEND)) {

        // Read the XO value for the calibration later
        snt_skip_calib  = 0;    // By default, it does the normal calibration
        calib_status    = 0;    // Reset Calibration status
        xo_val_prev     = xo_val_curr;
        xo_val_curr_tmp = get_xo_cnt(); // It returns 0 if XO has not been initialized yet

        // Reset variables
        meas.valid = 0;
        aes_key_valid = 0;

        // If this is the very first wakeup, initialize the system
        if (!get_flag(FLAG_INITIALIZED)) operation_init();

        #ifdef ENABLE_EID_WATCHDOG
            // Check-in the EID Watchdog if it is enabled
            if (get_flag(FLAG_WD_ENABLED)) eid_check_in();
        #endif

        //-----------------------------------------
        // NFC HANDSHAKE
        //-----------------------------------------
        if (WAKEUP_BY_NFC) {
            if (status.nfc_out_temp) {
                operation_sleep(/*check_snt*/1);
            } 
            else {
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
                    #ifdef ENABLE_DEBUG_SYSTEM_CONFIG
                        // Command: DEBUG
                        else if (cmd==CMD_DEBUG) {
                            override.enable = get_bit(cmd_param, 7);
                            override.sar_margin = cmd_param&0xF;
                            if (cmd_param==0x00) debug_system_configs();
                            eid_blink_display();
                        }
                    #endif
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
                            if (eeprom_user_config_1.temp_meas_start_delay==0) {
                                wakeup_source |= (0x1 << 4);
                                pretend_wakeup_by_snt = 1;
                                // start_delay_cnt needs to be set to 0
                                // since "if (WAKEUP_BY_SNT)" is called immediately
                                start_delay_cnt = 0;
                            }
                            else {
                                // start_delay_cnt needs to be set to 1
                                // since "if (WAKEUP_BY_SNT)" is called after 1min
                                start_delay_cnt = 1;
                            }
                        }
                        else if (cmd==CMD_STOP) {
                            if (status.sample_type==SAMPLE_NORMAL)
                                aes_encrypt_eeprom(/*addr*/EEPROM_ADDR_STOP_TIME);
                        }
                    }
                } // if (nfc_i2c_get_token(30))
            } // if (!status.nfc_out_temp)
        } // if (WAKEUP_BY_NFC)

        //-----------------------------------------
        // WAKEUP BY SNT
        //-----------------------------------------
        if (WAKEUP_BY_SNT) {
            // If this is the expected wakeup by SNT
            if (  pretend_wakeup_by_snt || check_snt_wakeup_near_threshold()) {

                // Counter for E-Ink Refresh 
                disp_min_since_refresh += wakeup_interval;

                #ifdef DEVEL
                    mbus_write_message32(0xA1, disp_min_since_refresh);
                #endif

                if (xt1_state==XT1_PEND) {
                    #ifdef DEVEL
                        mbus_write_message32(0x7D, start_delay_cnt);
                    #endif

                    if (start_delay_cnt==eeprom_user_config_1.temp_meas_start_delay) 
                        set_system(/*target*/XT1_ACTIVE);
                    else
                        start_delay_cnt++;
                }

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
            operation_sleep(/*check_snt*/1);
        }

    }
    // 2nd Wakeup: If waken up by the temp sensor, resume snt_operation()
    else {
        // if waken up by something other than SNT, wait until the temp sensor has finished.
        if (!WAKEUP_BY_SNT) {
            // Set a flag so that operation_sleep() calls main() directly without going into sleep.
            set_flag(FLAG_MAIN_CALLED_BY_NFC, 1);
            // Wait until the temp sensor returns a result
            while(*SNT_TARGET_REG_ADDR==0xFFFFFF)
                delay(DLY_1MS<<8);  // ~256ms delay
        }

        // Resume snt_operation()
        snt_operation();
    }

    // SNT Calibration
    calibrate_snt_timer(/*skip_calib*/snt_skip_calib);
    operation_sleep_snt_timer();

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
