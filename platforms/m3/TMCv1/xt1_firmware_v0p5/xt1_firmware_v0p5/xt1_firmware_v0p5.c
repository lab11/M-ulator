//*******************************************************************************************
// XT1 (TMCv1) FIRMWARE
// Version 0.5
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jun 24 2021 - First commit 
//  Jan 21 2022 - Version 0.1
//                  - For Long-term testing with SNT Timer calibration
//  Jan 24 2022 - Version 0.2
//                  - snt_init()
//                      Changed TSNS_SEL_CONV_TIME from 0xA to 0x7
//                  - operation_init()
//                      Now it writes HW ID (0x01000000) into EEPROM (WRITE_HW_ID switch). 
//                      Remove this for the final version.
//                  - Removed GOC_ACTIVATE_ALSO_STARTS switch
//                  - Removed SKIP_SNT_CALIB switch
//  Jan 26 2022 - Version 0.3
//                  - Added back GOC_ACTIVATE_ALSO_STARTS switch (for Zhiyoong's long term testing)
//                  - xo_start()
//                      Added the following configuration.
//                          [NOTE] xo_control is an instance of pre_r19_t
//                          xo_control.XO_SEL_VLDO     = 0x2; // Default value: 3'h0
//                          xo_control.XO_SEL_vV0P6    = 0x0; // Default value: 2'h2
//                  - calibrate_snt_timer()
//                      Changed the timeout from 30sec to 45sec
//                      Now it has THREE (3) failure typs during calibration.
//                          CLIB_XO_FAILS:      Most likely that the XO does not start from the beginning.
//                          CLIB_MAX_ERR_FAILS: Calibration results go out of the maximum accepted error.
//                          CLIB_TIMEOUT_FAILS: XO does not reach the threshold within the ~45sec timeout.
//                      It stores num_calib_pass in EEPROM_ADDR_NUM_I2C_FAILS (for debugging).
//                      It stores the last calibrated result (snt_timer_1min) in EEPROM_ADDR_START_TIME (for debugging).
//                      Now it turns off the NFC, if it was on, before starting the calibration, to save power.
//                  - snt_operation()
//                      Added ALSO_STORE_VBAT switch to save the current SAR ratio, VBAT, and temperature data into EEPROM.
//  Jan 27 2022 - Version 0.4
//                  - ALSO_STORE_VBAT switch is disabled by default.
//                  - Replaced EEPROM_ADDR_NUM_I2C_FAILS with EEPROM_ADDR_BUF_OVERRUN_FAIL
//                          We cannot write into EEPROM anyway, if there is an I2C Fail (i.e., I2C ACK timeout)
//                  - Added EEPROM_ADDR_TEMP_MEAS_CONFIG
//                          [15: 1] Reserved
//                          [0]     EN_ROLL_OVER     If 1 (default), it overwrites the oldest measurement data when the #sample exceeds the maximum allowed in the EEPROM capacity. 
//                                                   If 0, it stops storing temperature measurements once the EEPROM becomes full. #sample @ EEPROM_ADDR_SAMPLE_COUNT keeps increasing.
//                  - Removed EEPROM_ADDR_SAR_RATIO
//                  - If disp_refresh_interval=0, it does not perform the display refresh.
//                  - If eeprom_timer_calib_interval=0, it does not perform the timer calibration.
//                  - Two different sample counting:
//                          temp_sample_count  : Number of actual temperature measurements that have been done so far.
//                          eeprom_sample_count: Number of temperature measurements that have been written into EEPROM.
//                  - Implemented the write buffer
//                          buffer_init(void);
//                          buffer_inc_ptr(uint32_t ptr);
//                          buffer_add_item(uint32_t item);
//                          buffer_get_item(uint32_t item);
//                  - Change the reset values of the following variables. This is to remove the unwanted 'offset' in the beginning.
//                          cnt_temp_meas_start_delay = 0 -> 1
//                          cnt_temp_meas_interval    = 0 -> 1
//                          cnt_disp_refresh_interval = 0 -> 1
//                          cnt_timer_calib_interval  = 0 -> 1
//                  - calibrate_snt_timer()
//                      Removed the debugging feature (i.e., storing num_calib_pass and snt_timer_1min in EEPROM)
//                      Removed TIMEOUT_FAILS. Instead, it is not considered a XO FAILS.
//                      Added CALIB_TIMEOUT_USING_SNT switch to enable the timeout check using the SNT timer.
//                  - snt_operation()
//                      eid_trigger_crash() is triggered *after* writing temp data into EEPROM.
//                      Added two temperature thresholds for adc_low_vbat and adc_crit_vbat
//                      Implement the buffer overrun error
//                      Simplified the FSM states. Now snt_state has only two available states: SNT_IDLE and SNT_TEMP_READ.
//                  - eid_update_write_eeprom()
//                      It tries to the I2C token with max_num_try = 30
//                  - pmu_adc_read_and_sar_ratio_adjustment()
//                      Updated ADC threshold and corresponding SAR ratios based on the recent measurement of XT1#4.
//                      Added debug messages to notify whether the SAR ratio has been changed or not.
//                  - update_eeprom_variables()
//                      For USE_DEFAULT_VALUE, change the default values as shown below:
//                          eeprom_snt_calib_config = ((1 << 7) | (2 << 5) | (4 << 0)) -> ((1 << 7) | (2 << 5) | (2 << 0))
//                  - TMCv1.h/TMCv1.c
//                      Now it always does nfc_i2c_polling(). Removed __NFC_DO_POLLING__ switch.
//                      Implemented I2C token. Functions below are newly added:
//                          nfc_i2c_get_token()
//                          nfc_i2c_release_token()
//                          nfc_i2c_get_timeout()
//                          nfc_i2c_wait_for_ack()
//                      Now functions below return a value based on whether it has been correctly acked or not.
//                          nfc_i2c_present_password()
//                          nfc_i2c_update_password()
//                          nfc_i2c_byte_write()
//                          nfc_i2c_byte_read()
//                          nfc_i2c_seq_byte_write()
//                          nfc_i2c_word_write()
//                          nfc_i2c_word_write()
//                  - PREv22E.h/PREv22E.c
//                      Now stop_timeout32_check() returns 1 if there was no timeout, or returns 0 if there was a timeout.
//  Feb 03 2022 - Version 0.5
//                  - It performs the SNT timer calibration right after it detects the START command. 
//                      Previously it was done in TMP_PENDING state, but now it is done *before* going into TMP_PENDING.
//                  - For ADC, now it has four temperature thresholds (ADC_T1 ~ ADC_T4) and five different offsets (ADC_OFFSET_R1 ~ ADC_OFFSET_R5)
//                      to be added to various ADC thresholds, such as SAR ratio adjustment, ADC Low VBAT, ADC Critical VBAT.
//                  - Default (base) value of ADC Low VBAT and ADC Critical VBAT are also read from EEPROM.
//                  - It writes the system state. See comment on EEPROM_ADDR_SYSTEM_STATE
//                      NOTE: EEPROM_ADDR_BUF_OVERRUN_FAIL has been removed. Now the buffer overrun fail is logged in EEPROM_ADDR_SYSTEM_STATE.
//                  - Now the eeprom_timer_calib_interval is within the 'Other Configurations' section, rather than 'User Command'
//                  - Added 'TE_LAST_SAMPLE' tracking. It shows how many minutes have elapsed since the last temperature measurement.
//                      This helps post-calibrate the time on the app. 
//                      TE_LAST_SAMPLE resolution is set using TE_LAST_SAMPLE_RES variable.
//                  - Changed the code so that it measures the temperature once the 'start delay' has passed.
//                      Previously, it starts the 'temp_meas_interval' counting once the 'start delay' has passed.
//                  - snt_operation()
//                          Added operartion_back_to_default() before calling eid_trigger_crash(),
//                              so that it turns off the NFC and temp sensors if needed.
//                          Now, the Low BATT indicator does not go OFF once turned on.
//                  - calibrate_snt_timer()
//                          Reduced the timeout duration from 45s to 30s when checked using the SNT timer (i.e., CALIB_TIMEOUT_USING_SNT enabled)
//                  - Change the reset values of the following variables. This is to remove the unwanted 'offset' in the beginning.
//                          cnt_temp_meas_start_delay = 1 -> 2
//                      Also, if the start delay is 1 (i.e., eeprom_temp_meas_start_delay=), tmp_state is going from TMP_IDLE to TMP_ACTIVE, skipping TMP_PENDING.
//                      Thus, now the 'start delay=N' means an N-minute delay, not an (N+1)-minute delay.
//-------------------------------------------------------------------------------------------
// IMPORTANT
//  - This program assumes that GOC/EP uses RUN_CPU (not GEN_IRQ) only in its Control section.
//-------------------------------------------------------------------------------------------
//
// TMCv1 SUB-LAYER CONNECTION:
//      PREv22E -> SNTv5 -> EIDv1 -> MRRv11A -> MEMv3 -> PMUv13
//
//-------------------------------------------------------------------------------------------
// Short Address (Defined in TMCv1.h)
//-------------------------------------------------------------------------------------------
//      PRE: 0x1
//      SNT: 0x2
//      EID: 0x3
//      MRR: 0x4
//      MEM: 0x5
//      PMU: 0x6
//
//-------------------------------------------------------------------------------------------
// NOTE: GIT can be triggered by either VGOC or EDI.
//-------------------------------------------------------------------------------------------
//
// External Connections
//
//  XT1         NFC (ST25DVxxx)     XT1     e-Ink Display           Display    
//  ---------------------------     ------------------------                       
//  GPIO[0]     GPO                 SEG[0]  Play                       [==]                 .
//  GPIO[1]     SCL                 SEG[1]  Tick                                            .
//  GPIO[2]     SDA                 SEG[2]  Low Battery               \\  //                .
//  VPG2_OUT    VCC                 SEG[3]  Back Slash          |\     \\//    +            .
//                                  SEG[4]  Slash               |/     //\\    -            .
//                                  SEG[5]  Plus                    \\//  \\                .
//                                  SEG[6]  Minus                                
//
//-------------------------------------------------------------------------------------------
// General Operation 
//-------------------------------------------------------------------------------------------
//
//  * INITIALIZED: 
//      You have run the very first programming, and it has gone through enumeration and initializations. 
//      The SNT timer runs at this point. The system is in indefinite sleep. 
//      The display lit all the segments.
//
//  * ACTIVATED: 
//      You do a GOC to *activate* the system. Now the system wakes up every 1 min, checks NFC, 
//      as well as some housekeeping (display refresh and timer calibration when needed..) 
//      The display becomes all white.
//
//          GOC_ACTIVATE_KEY    0x16002351  // Activate the system. Once executed, the system starts the 1-min wakeup/sleep operation.
//
//  * STARTED: 
//      You do a GOC to "start" the temperature measurement. 
//      The display immediately shows the 'triangle'. 
//      Once actual temp measurement starts after the eeprom_temp_meas_start_delay, 
//      the display shows the 'triangle' as well as the 'check mark' or 'plus/minus' mark, depending on the measurement. 
//      It also displays the 'low batt' if the VBAT ADC reading indicates so.
//
//          GOC_QUICK_START_KEY 0xDECAFBAD  // Start the temperature measurement without changing the existing settings.
//          GOC_START_KEY       0xFEEDC0DE  // Start the temperature measurement after updating the settings.
//
//  * STOPPED: 
//      You can stop the system by sending GOC_STOP_KEY.
//      Once triggered, the system stops measuring the temperatuares but keeps doing the housekeeping.
//
//          GOC_STOP_KEY        0xDEADBEEF  // Stop the ongoing temperature measurement.
//
//-------------------------------------------------------------------------------------------
// SNT Temperature Sensor Raw Code
//-------------------------------------------------------------------------------------------
//
//  Temp(C)     Raw Code (Estimated value from Long-Term XT1 testing)
//  --------------------
//  -40            50
//  -30           100
//  -20           180
//  -10           250
//    0           450
//   10           700
//   25          1350
//   35          2000
//   45          2950
//   55          4200
//
//-------------------------------------------------------------------------------------------
// FLAG Register
//-------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------
// WAKEUP_SOURCE (wakeup_source) definition
//-------------------------------------------------------------------------------------------
//
//  Use get_bit(wakeup_source, n) to get the current value, where n is 0-5, 8-11 as shown below.
//
//  [31:12] - Reserved (NOTE: wakeup_source[31] is used to detect the NFC GPO activity.)
//     [11] - GPIO_PAD[3] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [10] - GPIO_PAD[2] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [ 9] - GPIO_PAD[1] has triggered wakeup (valid only when wakeup_source[3]=1)
//     [ 8] - GPIO_PAD[0] has triggered wakeup (valid only when wakeup_source[3]=1)
//  [ 7: 6] - Reserved
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
// I2C Token
//-------------------------------------------------------------------------------------------
//
// I2C - RF Conflict Scenario
//
//      1) Initialization (operation_init)
//          This does not require the I2C token. 
//          User needs to make sure that there is no RF activity during system initialization.
//
//      2) GOC/EP (handler_ext_int_gocep)
//          This is for system activation, or system debugging.
//          This does not require the I2C token. 
//          User needs to make sure that there is no RF activity during this operation.
//
//      3) Checking NFC Commands (nfc_check_cmd)
//          If it fails to get the token,
//          it just skips checking the NFC command.
//          This command will be re-checked at the next wakeup.
//
//      4) Logging Timer Calibration Results (calibrate_snt_timer)
//          If it fails to get the token,
//          it does NOT update the failure counts in EEPROM.
//          The failure counts are updated to the correct numbers at the next successful attempt.
//
//      5) Saving Temperature Data (snt_operation)
//          If it fails to get the token,
//          it first attempts to use the write buffer.
//          If there is a buffer overrun, then it sets a flag. User will lose the temperature data.
//
//      6) E-ink Update
//          It keeps trying to get the token until success.
//
//-------------------------------------------------------------------------------------------
// DEBUG Messages
//-------------------------------------------------------------------------------------------
// Valid only when DEBUG is enabled
//
//  ADDR    DATA                                Description
//  -----------------------------------------------------------------------------------------
//  0x80    wakeup_source                       Wakeup Source (See above WAKEUP_SOURCE definition)
//
//  0x81    wakeup_count                        Wakeup Count (only when snt_state = SNT_IDLE)
//
//  0x82    {0x00, tmp_state}                   Temp Meas State (only when system is activated)
//                                                      0x0 TMP_IDLE        
//                                                      0x1 TMP_PENDING     
//                                                      0x2 TMP_ACTIVE      
//                                                      0x3 TMP_DONE        
//
//  0x82    {0x01, cnt_temp_meas_start_delay}   Counter for Start Delay
//  0x82    {0x02, cnt_temp_meas_interval}      Counter for Temp Meas Period
//  0x82    {0x03, cnt_disp_refresh_interval}   Counter for Display Refresh
//  0x82    {0x04, cnt_timer_calib_interval}    Counter for Timer Calibration
//  0x82    {0x0E, eeprom_sample_count}         Current Number of Temp Meas (saved into EEPROM)
//  0x82    {0x0F, temp_sample_count}           Current Number of Temp Meas
//
//  0x82    {0x10, snt_state}                   SNT State (in snt_operation())
//                                                      0x0 SNT_IDLE        
//                                                      0x1 SNT_LDO         // NOT USED
//                                                      0x2 SNT_TEMP_START  // NOT USED
//                                                      0x3 SNT_TEMP_READ   
//
//  0x83    0x00000000                          NFC activity detected (GPO=1) (only when snt_state=SNT_IDLE)
//  0x83    0x00000001                          Temp Meas forced to stop by user
//  0x83    0x00000002                          Temp Meas started and now it becomes TMP_PENDING
//
//  0x83    {0x01, eeprom_temp_meas_start_delay}    Start Delay
//  0x83    {0x02, eeprom_temp_meas_interval}       Temp Meas Period
//  0x83    {0x03, eeprom_timer_calib_interval}     Timer Calibration
//  0x83    {0x04, eeprom_te_last_sample_res}       Resolution of TE_LAST_SAMPLE.
//  0x83    {0x05, eeprom_temp_meas_config}         Misc. Configurations
//  0x83    {0x06, eeprom_high_temp_threshold}      High Temp Threshold
//  0x83    {0x07, eeprom_low_temp_threshold}       Low Temp Threshold
//
//  0x83    0x00000003                          Temp Meas stays in IDLE (Has not started by user)
//  0x83    0x00000004                          Temp Meas Start Delay has met. Now it becomes TMP_ACTIVE.
//  0x83    0x00000005                          Temp Meas Start Delay has not met. It is still in TMP_PENDING.
//  0x83    0x00000006                          Temp Meas performs the temp measurement in this active session (TMP_ACTIVE)
//  0x83    0x00000007                          Temp Meas skips this active session (TMP_ACTIVE)
//  0x83    0x00000008                          Temp Meas has performed the given number of temp measurements. Now it goes back to TMP_IDLE.
//  0x83    0x00000009                          Refreshing Eink display
//  0x83    0x0000000A                          Calibrating the SNT timer (using XO)
//  0x83    0x0000000B                          CONFIG command detected
//
//  0x84    snt_timer_threshold                 New threshold value for the SNT timer
//
//  0x85    0x00000000                          NFC Command (NOP) Detected
//  0x85    0x00000001                          NFC Command (START) Detected
//  0x85    0x00000002                          NFC Command (STOP) Detected
//  0x85    others                              NFC Command (Invalid) Detected
//
//  0x86    0x00000001                          NFC Command ACKed (ACK Bit Set)
//  0x86    0xDEADBEEF                          NFC Command ERRORed (ERR Bit Set)
//
//  0x87    old_val (snt_timer_1min)            The old_val value before an SNT Calibration
//  0x87    new_val                             The new_val value after an SNT Calibration
//      NOTE: The first 0x87 is the old_val, and the second 0x87 is the new_val.
//  0x88    0x00000000                          XO timeout during SNT timer calibration
//  0x88    0x00000001                          SNT calibration result is out of expectation.
//  0x88    0x00000002                          SNT calibration successful
//  0x88    0x00000003                          XO does not start
//
//  0x90    0xaabbccdd                          PMU ADC reading and SAR ratio values (aa: Given offset, bb: ADC output code, cc: previous SAR ratio, dd: new SAR ratio)
//  0x91    0xaabbccdd                          PMU ADC reading and SAR ratio values (aa: Given offset, bb: ADC output code, cc=dd: current SAR ratio)
//
//  0xA0    0x0abcdefg                          E-ink display has been updated.
//                                                  a: Minus
//                                                  b: Plus
//                                                  c: Slash
//                                                  d: Back Slash
//                                                  e: Low Battery
//                                                  f: Tick
//                                                  g: Play
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
// EEPROM in ST25DV64K using I2C interface
//-------------------------------------------------------------------------------------------
//  Memory Capacity: 64 kbits = 8kB = 2k words = 2k pages (1 page = 1 word)
//  
//  A Sequential Write can write up to 256 bytes (= 64 words = 64 pages)
//  2k / 64 = 32 Sequential Writes are needed to overwrite the entire EEPROM.
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
//              -> The reply msg from PMU writes into REG0, triggering IRQ_REG0, which then sends out the msg 0x71, 0x3.
//                 If this happens BEFORE the system releases the halt state, the system may still stay in 'set_halt_until_mbus_trx()'
//
//******************************************************************************************* 

#include "ST25DV64K.h"
#include "TMCv1.h"

//*******************************************************************************************
// DEBUGGING
//*******************************************************************************************
#define DEVEL                   // Enable this to enable the use of GOC to Start/Stop the system with customized settings.
#define GOCEP_RUN_CPU_ONLY      // Enable this when you cannot do 'GEN_IRQ' in GOC/EP Header (e.g., the current m3_ice script)
#define DEBUG                   // Send debug messages
#define USE_DEFAULT_VALUE       // Use default values rather than grabbing the values from EEPROM
#define GOC_ACTIVATE_ALSO_STARTS    // Enable this to make 'GOC Activate' also starts the temperature measurements
//#define ALSO_STORE_VBAT       // Enable this to store SAR ratio, VBAT, and temperature data into EEPROM.

#define CALIB_TIMEOUT_USING_SNT // Check the timeout during the calibration using the SNT timer, rather than the TIMER32

//*******************************************************************************************
// GROUP ID
//*******************************************************************************************

#define FIRMWARE_ID    0x58540001  // XT Firmware Version 0.1

//*******************************************************************************************
// KEYS
//*******************************************************************************************

#define GOC_ACTIVATE_KEY    0x16002351  // Activate the system. Once executed, the system starts the 1-min wakeup/sleep operation.
#define GOC_QUICK_START_KEY 0xDECAFBAD  // Start the temperature measurement without changing the existing settings.
#define GOC_START_KEY       0xFEEDC0DE  // Start the temperature measurement after updating the settings.
#define GOC_STOP_KEY        0xDEADBEEF  // Stop the ongoing temperature measurement.

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED     0
#define FLAG_INITIALIZED    1
#define FLAG_ACTIVATED      2
#define FLAG_STARTED        3
#define FLAG_WD_ENABLED     4

//*******************************************************************************************
// WRITE BUFFER CONFIGURATIONS
//*******************************************************************************************
#define BUFFER_SIZE 10      // Buffer Size (Max number of items that can be saved in the buffer)

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// PRE Clock Generator Frequency
#define CPU_CLK_FREQ    140000  // XT1F#1, SAR_RATIO=0x60 (Sleep Voltages: 4.55V/1.59V/0.79V)

// PRE Clock-based Delay (5 instructions @ CPU_CLK_FREQ)
#define DLY_1S      ((CPU_CLK_FREQ>>3)+(CPU_CLK_FREQ>>4)+(CPU_CLK_FREQ>>5))  // = CPU_CLK_FREQx(1/8 + 1/16 + 1/32) = CPU_CLK_FREQx(35/160) ~= (CPU_CLK_FREQ/5)
#define DLY_1MS     DLY_1S >> 10                                             // = DLY_1S / 1024 ~= DLY_1S / 1000

// XO Initialization Wait Duration
#define XO_WAIT_A   2*DLY_1S  // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S  // Must be ~1 second delay.

//*******************************************************************************************
// SNT LAYER CONFIGURATION
//*******************************************************************************************

// SNT states
#define SNT_IDLE        0x0
#define SNT_LDO         0x1 // NOT USED
#define SNT_TEMP_START  0x2 // NOT USED
#define SNT_TEMP_READ   0x3

// Temp Meas states
#define TMP_IDLE        0x0
#define TMP_PENDING     0x1
#define TMP_ACTIVE      0x2
#define TMP_DONE        0x3 // NOT USED

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
#define DISP_NORMAL         (DISP_CHECK)
#define DISP_HIGH_TEMP      (DISP_PLUS)
#define DISP_LOW_TEMP       (DISP_MINUS)
#define DISP_LOW_VBAT       (DISP_LOWBATT)
#define DISP_ALL            (0x7F)

//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************

//-------------------------------------------------------------------------------------------
// EEPROM Variables
//-------------------------------------------------------------------------------------------
// NOTE: eeprom_* variables are copied from EEPROM whenever needed.

//--- System Status
volatile uint32_t temp_sample_count;                // Counter for sample count (# actual temperature measurements)
volatile uint32_t eeprom_sample_count;              // Counter for sample count (# temperature measurements stored into EEPROM)
volatile uint32_t num_calib_pass;                   // Number of Calibration passes
volatile uint32_t num_calib_xo_fails;               // Number of Calibration fails (XO timeout)
volatile uint32_t num_calib_max_err_fails;          // Number of Calibration fails (MAX_CHANGE error)
volatile uint32_t system_state;                     // System State (See comment on EEPROM_ADDR_SYSTEM_STATE)

//--- User Commands
volatile uint32_t eeprom_high_temp_threshold;       // (Default:  300) Threshold for High Temperature
volatile uint32_t eeprom_low_temp_threshold;        // (Default:  100) Threshold for Low Temperature
volatile uint32_t eeprom_temp_meas_interval;        // (Default:   15) Period of Temperature Measurement (unit: snt_timer_1min)
volatile uint32_t eeprom_temp_meas_start_delay;     // (Default:   30) Start Delay before starting temperature measurement (unit: snt_timer_1min)
volatile uint32_t eeprom_temp_meas_config;          // Misc. Configurations
                                                    // [15: 1] Reserved
                                                    // [0]     EN_ROLL_OVER     If 1, it overwrites the oldest measurement data when the #sample exceeds the maximum allowed in the EEPROM capacity. 
                                                    //                          If 0, it stops storing temperature measurements once the EEPROM becomes full. #sample @ EEPROM_ADDR_SAMPLE_COUNT keeps increasing.

//--- Counter for User Parameters
volatile uint32_t cnt_temp_meas_start_delay;        // Counter for eeprom_temp_meas_start_delay
volatile uint32_t cnt_temp_meas_interval;           // Counter for eeprom_temp_meas_interval
volatile uint32_t cnt_disp_refresh_interval;        // Counter for disp_refresh_interval
volatile uint32_t cnt_timer_calib_interval;         // Counter for eeprom_timer_calib_interval
volatile uint32_t cnt_te_last_sample;               // Counter for eeprom_te_last_sample
volatile uint32_t cnt_te_last_sample_res;           // Counter for eeprom_te_last_sample_res

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

volatile uint32_t eeprom_vbat_info;                 // VBAT + calibration data
volatile uint32_t eeprom_adc_t1;                    //  ( 1250 ) Temperature Threshold (Raw Code) for ADC. See the table above.
volatile uint32_t eeprom_adc_t2;                    //  ( 1250 ) Temperature Threshold (Raw Code) for ADC. See the table above.
volatile uint32_t eeprom_adc_t3;                    //  ( 1250 ) Temperature Threshold (Raw Code) for ADC. See the table above.
volatile uint32_t eeprom_adc_t4;                    //  ( 1250 ) Temperature Threshold (Raw Code) for ADC. See the table above.
volatile uint32_t eeprom_adc_offset_r1;             //  (    0 ) ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_offset_r2;             //  (    0 ) ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_offset_r3;             //  (    0 ) ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_offset_r4;             //  (    0 ) ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_offset_r5;             //  (    0 ) ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_low_vbat;              //  (  105 ) ADC code threshold to turn on the Low VBAT indicator.
volatile uint32_t eeprom_adc_crit_vbat;             //  (   95 ) ADC code threshold to trigger the EID crash handler.

//--- Calibration Data
volatile uint32_t eeprom_snt_calib_config;          // [7]: Calibration Duration (0: 7.5sec, 1: 15sec); [6:5]: XO Frequency (0: 4kHz, 1: 8kHz, 2: 16kHz, 3: 32kHz); [4:3]: Reserved; [2:0]: Max Error allowed (# bit shift from SNT_BASE_FREQ)
volatile uint32_t eeprom_snt_base_freq;             // SNT Timer Base Frequency in Hz (integer value only)

//--- EID Configurations
volatile uint32_t eeprom_eid_high_temp_threshold;   // Temperature Threshold (Raw Code) to determin HIGH and MID temperature for EID
volatile uint32_t eeprom_eid_low_temp_threshold;    // Temperature Threshold (Raw Code) to determin MID and LOW temperature for EID
volatile uint32_t eeprom_eid_duration_high;         // EID duration (ECTR_PULSE_WIDTH) for High Temperature
volatile uint32_t eeprom_eid_fe_duration_high;      // EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for High Temperature. '0' makes it skip 'Field Erase'
volatile uint32_t eeprom_eid_rfrsh_int_hr_high;     // EID Refresh interval for High Temperature (unit: hr). '0' means 'do not refresh'.
volatile uint32_t eeprom_eid_duration_mid;          // EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
volatile uint32_t eeprom_eid_fe_duration_mid;       // EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Mid Temperature. '0' makes it skip 'Field Erase'
volatile uint32_t eeprom_eid_rfrsh_int_hr_mid;      // EID Refresh interval for Mid Temperature (unit: hr). '0' means 'do not refresh'.
volatile uint32_t eeprom_eid_duration_low;          // EID duration (ECTR_PULSE_WIDTH) for Low Temperature
volatile uint32_t eeprom_eid_fe_duration_low;       // EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Low Temperature. '0' makes it skip 'Field Erase'
volatile uint32_t eeprom_eid_rfrsh_int_hr_low;      // EID Refresh interval for Low Temperature (unit: hr). '0' meas 'do not refresh'.

//--- Other Configurations
volatile uint32_t eeprom_timer_calib_interval;      // Period of SNT Timer Calibration. 0 means 'do not calibrate'. (unit:snt_timer_1min).
volatile uint32_t eeprom_te_last_sample_res;        // Resolution of TE_LAST_SAMPLE (unit: minute). See comment on EEPROM_ADDR_TE_LAST_SAMPLE.

//-------------------------------------------------------------------------------------------
// Other Global Variables
//-------------------------------------------------------------------------------------------

//--- General 
volatile uint32_t wakeup_source;            // Wakeup Source. Updated each time the system wakes up. See 'WAKEUP_SOURCE definition'.
volatile uint32_t wakeup_count;             // Wakeup Count. Incrementing each time the system wakes up. It does not increment if the system wakes up to resume the SNT temperature measurement.

//--- SNT & Temperature Measurement
volatile uint32_t snt_state;                // SNT state
volatile uint32_t tmp_state;                // TMP state
volatile uint32_t snt_running;              // Indicates whether the SNT temp sensor is running.
volatile uint32_t eeprom_addr;              // EEPROM byte address for temperature data storage
volatile uint32_t snt_temp_val;             // Latest temp measurement (raw code)

//--- SNT & Timer
volatile uint32_t snt_timer_threshold;      // SNT Timer Threshold to wake up the system
volatile uint32_t snt_timer_1min;           // SNT Timer tick count corresponding to 1 minute

//--- Display
volatile uint32_t disp_refresh_interval;     // Period of Display Refresh. 0 means 'do not refresh'. (unit: snt_timer_1min)

//--- Write Buffer
volatile uint32_t num_buffer_items;         // Number of items in the buffer
volatile uint32_t buffer_next_ptr;          // Pointer where the next item will be inserted
volatile uint32_t buffer_first_ptr;         // Pointer of the first item to be read
volatile uint32_t buffer_overrun;           // Buffer Overrun Status
volatile uint32_t buffer_missing_sample_id; // ID of the missing sample in case of the buffer overrun. It keeps the very first missing smaple ID only.

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Initialization/Sleep Functions
static void operation_sleep (void);
static void operation_sleep_snt_timer(uint32_t auto_reset, uint32_t timestamp);
static void operation_back_to_default(void);
static void operation_init (void);

//--- Application Specific
static void eid_update_with_eeprom(uint32_t seg);
static void set_system_state(uint32_t msb, uint32_t lsb, uint32_t val);
static void snt_operation (void);
static void nfc_set_ack(uint32_t cmd);
static void nfc_set_err(uint32_t cmd);
static void nfc_check_cmd(void);
static void calibrate_snt_timer(void);
static void reset_eeprom(uint32_t fw_id);
static void update_eeprom_variables(void);

//--- Write Buffer
static void buffer_init(void);
static uint32_t buffer_inc_ptr(uint32_t ptr);
static uint32_t buffer_add_item(uint32_t item);
static uint32_t buffer_get_item(void);


//*******************************************************************************************
// FUNCTIONS IMPLEMENTATION
//*******************************************************************************************

//-------------------------------------------------------------------
// Initialization/Sleep Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: operation_sleep
// Args    : None
// Description:
//           Sends the MBus Sleep message
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep (void) {
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // FIXME: Stop any ongoing EID operation (but how...?)

    // Power off NFC
    nfc_power_off();

    // Clear all pending IRQs; otherwise, PREv22E replaces the sleep msg with a selective wakeup msg
    *NVIC_ICER = 0xFFFFFFFF;

    // Send a sleep msg
    mbus_sleep_all();
    while(1);
}

//-------------------------------------------------------------------
// Function: operation_sleep_snt_timer
// Args    : auto_reset - 1: Auto Reset enabled.
//                              SNT Wakeup counter automatically gets reset to 0 upon system going into sleep
//                        0: Auto Reset disabled.
//           timestamp - Time stamp for the SNT Timer
// Description:
//           Goes into sleep with SNT Timer enabled
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep_snt_timer(uint32_t auto_reset, uint32_t timestamp){
    snt_set_wup_timer(auto_reset, timestamp);
    operation_sleep();
}

//-------------------------------------------------------------------
// Function: operation_back_to_default
// Args    : None
// Description:
//           Reset everything to its default.
// Return  : None
//-------------------------------------------------------------------
static void operation_back_to_default(void){
    set_halt_until_mbus_tx();
    if (snt_running) {
        snt_running = 0;
        snt_state = SNT_IDLE;
        snt_temp_sensor_reset();
        snt_temp_sensor_power_off();
        snt_ldo_power_off();
    }
    nfc_power_off();
    tmp_state = TMP_IDLE;
}

//-------------------------------------------------------------------
// Function: operation_init
// Args    : None
// Description:
//           Initializes the system
// Return  : None
//-------------------------------------------------------------------
static void operation_init (void) {

    if (!get_flag(FLAG_ENUMERATED)) {

        //-------------------------------------------------
        // EEPROM Variables (w/ their suggested default value)
        //-------------------------------------------------

        //--- System Status
        temp_sample_count               = 0;
        eeprom_sample_count             = 0;
        num_calib_pass                  = 0;
        num_calib_xo_fails              = 0;
        num_calib_max_err_fails         = 0;
        system_state                    = 0;

        //--- Counter for User Parameters
        cnt_temp_meas_start_delay       = 2;
        cnt_temp_meas_interval          = 1;
        cnt_disp_refresh_interval       = 1;
        cnt_timer_calib_interval        = 1;
        cnt_te_last_sample              = 0;
        cnt_te_last_sample_res          = 0;

        //--- User Parameters
        eeprom_high_temp_threshold      = 700;
        eeprom_low_temp_threshold       = 450;
        eeprom_temp_meas_interval       = 15;
        eeprom_temp_meas_start_delay    = 30;
        eeprom_temp_meas_config         = (1 << 0);

        //-------------------------------------------------
        // Global Variables
        //-------------------------------------------------

        wakeup_count = 0;

        snt_state    = SNT_IDLE;
        tmp_state    = TMP_IDLE;
        snt_running  = 0;
        eeprom_addr  = EEPROM_ADDR_DATA_RESET_VALUE;
        snt_temp_val = 1350; // Assume 25C by default

        snt_timer_threshold   = 0;
        snt_timer_1min        = 0; // will be updated again in update_eeprom_variables()

        disp_refresh_interval = 240;

        //-------------------------------------------------
        // PRE Tuning
        //-------------------------------------------------
        pre_r0B.as_int = PRE_R0B_DEFAULT_AS_INT;
        //--- Set CPU & MBus Clock Speeds      Default
        pre_r0B.CLK_GEN_RING         = 0x1; // 0x1
        pre_r0B.CLK_GEN_DIV_MBC      = 0x2; // 0x2  // XT1F#1: 280kHz with SAR_RATIO=0x4C @ VBAT=2.8V
        pre_r0B.CLK_GEN_DIV_CORE     = 0x3; // 0x3  // XT1F#1: 140kHz with SAR_RATIO=0x4C @ VBAT=2.8V
        pre_r0B.GOC_CLK_GEN_SEL_FREQ = 0x5; // 0x7
        pre_r0B.GOC_CLK_GEN_SEL_DIV  = 0x0; // 0x1
        pre_r0B.GOC_SEL              = 0xF; // 0x8
        *REG_CLKGEN_TUNE = pre_r0B.as_int;

        //-------------------------------------------------
        // Enumeration
        //-------------------------------------------------
        set_halt_until_mbus_trx();
        mbus_enumerate(SNT_ADDR);
        mbus_enumerate(EID_ADDR);
        mbus_enumerate(MRR_ADDR);
        mbus_enumerate(MEM_ADDR);
        mbus_enumerate(PMU_ADDR);
        set_halt_until_mbus_tx();

        //-------------------------------------------------
        // Target Register Index
        //-------------------------------------------------
        mbus_remote_register_write(PMU_ADDR, 0x52, (0x10 << 8) | PMU_TARGET_REG_IDX);
        mbus_remote_register_write(SNT_ADDR, 0x07, (0x10 << 8) | SNT_TARGET_REG_IDX);
        mbus_remote_register_write(EID_ADDR, 0x05, (0x1 << 16) | (0x10 << 8) | EID_TARGET_REG_IDX);
        mbus_remote_register_write(MRR_ADDR, 0x1E, (0x10 << 8) | MRR_TARGET_REG_IDX); // FSM_IRQ_REPLY_PACKET
        mbus_remote_register_write(MRR_ADDR, 0x23, (0x10 << 8) | MRR_TARGET_REG_IDX); // TRX_IRQ_REPLY_PACKET

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

        // Turn on the SNT timer 
        snt_start_timer(/*wait_time*/2*DLY_1S);

        // Go to sleep
        set_wakeup_timer (/*timestamp*/10, /*irq_en*/0x1, /*reset*/0x1); // About 2 seconds
        mbus_sleep_all();
        while(1);
    }
    else if (!get_flag(FLAG_INITIALIZED)) {
        
        // Set the Active floor setting to the minimum (b/c RAT is enabled at this point)
        pmu_set_active_min();

        //-------------------------------------------------
        // NFC 
        //-------------------------------------------------
        nfc_init();

    #ifdef WRITE_HW_ID
        // Set the Hardware ID
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HW_ID, /*data*/0x00000001, /*nb*/4);
    #endif

        // Reset status in EEPROM
        reset_eeprom(FIRMWARE_ID);

        // Initialize the EEPROM variables
        update_eeprom_variables();

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
	    eid_init(/*ring*/0, /*te_div*/3, /*fd_div*/3, /*seg_div*/3);
        // Update the display
        eid_update_with_eeprom(DISP_ALL);

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);

        //-------------------------------------------------
        // Write Buffer Settings
        //-------------------------------------------------
        buffer_init();

        //-------------------------------------------------
        // Indefinite Sleep
        //-------------------------------------------------
        operation_sleep();
    }
}

//-------------------------------------------------------------------
// Application-Specific Functions
//-------------------------------------------------------------------

static void eid_update_with_eeprom(uint32_t seg) {

    // Update duration based on lastest temperature measurement
    if (snt_temp_val > eeprom_eid_high_temp_threshold) {
        eid_set_duration(eeprom_eid_duration_high);
        eid_set_fe_duration(eeprom_eid_fe_duration_high);
    }
    else if (snt_temp_val > eeprom_eid_low_temp_threshold) {
        eid_set_duration(eeprom_eid_duration_mid);
        eid_set_fe_duration(eeprom_eid_fe_duration_mid);
    }
    else {
        eid_set_duration(eeprom_eid_duration_low);
        eid_set_fe_duration(eeprom_eid_fe_duration_low);
    }

    // E-ink Update
    eid_update(seg);

    // Reset the refresh counter
    cnt_disp_refresh_interval = 1;

    // Try up to 30 times - Consistency between the EEPROM log and the actual display is important!
    if(nfc_i2c_get_token(30)) {
        // EEPROM Update
        nfc_i2c_byte_write(/*e2*/0,
            /*addr*/ EEPROM_ADDR_DISPLAY,
            /*data*/ seg,
            /* nb */ 1
        );
        nfc_i2c_release_token();
    }

    #ifdef DEBUG
        mbus_write_message32(0xA0, 
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

static void set_system_state(uint32_t msb, uint32_t lsb, uint32_t val) {
    system_state = set_bits(/*original_var*/system_state, /*msb_idx*/msb, /*lsb_idx*/lsb, /*value*/val);
    if(nfc_i2c_get_token(30)) {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SYSTEM_STATE, /*data*/system_state, /*nb*/2);
        nfc_i2c_release_token();
    }
}

static void snt_operation (void) {

    #ifdef DEBUG
        mbus_write_message32(0x82, (0x10 << 24) | snt_state);
    #endif

    if (snt_state == SNT_IDLE) {
        snt_running = 1;
        snt_state = SNT_TEMP_READ;

        // Turn on SNT LDO VREF.
        snt_ldo_vref_on();

        // Turn on SNT LDO
        snt_ldo_power_on();

        // Turn on SNT Temperature Sensor
        snt_temp_sensor_power_on();

        // Reset the Temp Sensor
        snt_temp_sensor_reset();

        // Release the reset for the Temp Sensor
        snt_temp_sensor_start();

        // Go to sleep during measurement
        operation_sleep();
    }
    else if (snt_state == SNT_TEMP_READ) {

        snt_temp_val = *SNT_TARGET_REG_ADDR;
        temp_sample_count++;

        // Assert temp sensor isolation & turn off temp sensor power
        snt_temp_sensor_power_off();
        snt_ldo_power_off();
        snt_state = SNT_IDLE;

        // Determine ADC Offset, ADC Low VBAT threshold, and ADC Critical VBAT threshold
        uint32_t adc_offset, adc_low_vbat, adc_crit_vbat;
        if      (snt_temp_val > eeprom_adc_t1) { adc_offset = eeprom_adc_offset_r1; }
        else if (snt_temp_val > eeprom_adc_t2) { adc_offset = eeprom_adc_offset_r2; }
        else if (snt_temp_val > eeprom_adc_t3) { adc_offset = eeprom_adc_offset_r3; }
        else if (snt_temp_val > eeprom_adc_t4) { adc_offset = eeprom_adc_offset_r4; }
        else                                   { adc_offset = eeprom_adc_offset_r5; }
        adc_low_vbat  = eeprom_adc_low_vbat  + adc_offset;
        adc_crit_vbat = eeprom_adc_crit_vbat + adc_offset;

        // VBAT Measurement and SAR_RATIO Adjustment
        uint32_t pmu_adc_vbat_val = pmu_adc_read_and_sar_ratio_adjustment(/*offset*/adc_offset);

        // Write data into the write buffer
        if(!buffer_add_item((pmu_get_sar_ratio()<<24)|(pmu_adc_vbat_val<<16)|(snt_temp_val&0xFFFF))) {
            // ERROR: Buffer Overrun
            if (!buffer_overrun) {
                buffer_overrun = 1;
                // Store the sample ID. Sample ID starts from 0.
                buffer_missing_sample_id = temp_sample_count - 1;
            }
        }

        /////////////////////////////////////////////////////////////////
        // e-Ink Display
        //---------------------------------------------------------------
        // NOTE: Actual update happens in 'Write into EEPROM/Display"

        //--- Read the current display
        uint32_t curr_seg = eid_get_current_display();
        uint32_t new_seg = curr_seg;

        //--- If the current display does not show temp excursions (i.e., no HIGH nor LOW TEMP)
        if (!get_bit(curr_seg, SEG_PLUS) && !get_bit(curr_seg, SEG_MINUS)) {
            if       (snt_temp_val > eeprom_high_temp_threshold) new_seg = DISP_PLAY | DISP_CROSS | DISP_HIGH_TEMP;
            else if  (snt_temp_val < eeprom_low_temp_threshold ) new_seg = DISP_PLAY | DISP_CROSS | DISP_LOW_TEMP;
            else                                                 new_seg = DISP_PLAY | DISP_NORMAL;
        }
        //--- If the current display shows HIGH TEMP
        else if (get_bit(curr_seg, SEG_PLUS)) {
            if (snt_temp_val < eeprom_low_temp_threshold ) new_seg = set_bit(new_seg, SEG_MINUS, 1);
        }
        //--- If the current display shows LOW TEMP
        if (get_bit(curr_seg, SEG_MINUS)) {
            if (snt_temp_val > eeprom_high_temp_threshold) new_seg = set_bit(new_seg, SEG_PLUS, 1);
        }

        //--- Update the Battery Indicator
        if (!get_bit(curr_seg, SEG_LOWBATT) && (pmu_adc_vbat_val<adc_low_vbat)) {
            new_seg = set_bit(new_seg, SEG_LOWBATT, 1);
        }


        /////////////////////////////////////////////////////////////////
        // Write into EEPROM/Display
        //---------------------------------------------------------------

        if (nfc_i2c_get_token(1)) {
            // Store the Measured Temp data
            while (num_buffer_items!=0) {

                eeprom_sample_count++;

                nfc_i2c_byte_write(/*e2*/0, 
                    /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, 
                    /*data*/ eeprom_sample_count,
                    /* nb */ 4
                    );

                if (eeprom_addr != 0) { // eeprom_addr=0 indicates the EEPROM is full -AND- the roll-over is not allowed.
                #ifdef ALSO_STORE_VBAT
                    nfc_i2c_byte_write(/*e2*/0, 
                        /*addr*/ eeprom_addr, 
                        /*data*/ buffer_get_item(),
                        /* nb */ 4
                        );
                    eeprom_addr += 4;
                #else
                    nfc_i2c_byte_write(/*e2*/0, 
                        /*addr*/ eeprom_addr, 
                        /*data*/ buffer_get_item()&0xFFFF,
                        /* nb */ 2
                        );
                    eeprom_addr += 2;
                #endif
                }

                if (eeprom_addr==EEPROM_NUM_BYTES) {
                    // Roll-over Allowed
                    if (get_bit(eeprom_temp_meas_config, 0)) {
                        eeprom_addr = EEPROM_ADDR_DATA_RESET_VALUE;
                    }
                    // Roll-over NOT Allowed
                    else {
                        eeprom_addr = 0;
                    }
                }
            }

            // If buffer overrun, set the failure flag
            if (buffer_overrun) {
                set_system_state(/*msb*/14, /*lsb*/14, /*val*/0x1);
            }

            // NOTE: Need to postpone 'nfc_i2c_release_token' after 'eid_trigger_crash' 
            // since eid_update_with_eeprom and the System State update require the token anyway.
        }

        // Update the e-Ink Display if needed
        if (new_seg!=curr_seg) eid_update_with_eeprom(new_seg);

        /////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////
        // EID Crash Handler
        //---------------------------------------------------------------
        //--- If VBAT is too low, trigger the EID Watchdog (System Crash)
        if (pmu_adc_vbat_val < adc_crit_vbat) {
            // Update the System State
            set_system_state(/*msb*/15, /*lsb*/15, /*val*/0x1);
            // Back to Default
            operation_back_to_default();
            // Trigger the Crash Handler
            eid_trigger_crash();
            while(1);
        }
        /////////////////////////////////////////////////////////////////

        // Release the I2C token
        nfc_i2c_release_token();

        /////////////////////////////////////////////////////////////////
        // Update Display Refresh Interval
        //---------------------------------------------------------------
        //--- Get the new value
        uint32_t new_val;
        if      (snt_temp_val > eeprom_eid_high_temp_threshold) new_val = eeprom_eid_rfrsh_int_hr_high;
        else if (snt_temp_val > eeprom_eid_low_temp_threshold)  new_val = eeprom_eid_rfrsh_int_hr_mid;
        else                                                    new_val = eeprom_eid_rfrsh_int_hr_low;
        new_val = (new_val << 6) - (new_val << 2); // convert hr to min

        //--- if new_val < disp_refresh_interval, do refresh display now.
        if (new_val < disp_refresh_interval) {
            if (cnt_disp_refresh_interval > new_val) cnt_disp_refresh_interval = new_val;
        }
        disp_refresh_interval = new_val;
        /////////////////////////////////////////////////////////////////

        snt_running = 0;
    }
}

void nfc_set_ack(uint32_t cmd) {
    nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD, /*data*/ (0x1<<6)|(cmd&0x1F), /*nb*/ 1);
    #ifdef DEBUG
        mbus_write_message32(0x86, 0x1);
    #endif
}
void nfc_set_err(uint32_t cmd) {
    nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD, /*data*/ (0x1<<5)|(cmd&0x1F), /*nb*/ 1);
    #ifdef DEBUG
        mbus_write_message32(0x86, 0xDEADBEEF);
    #endif
}
void nfc_check_cmd(void) {

    // Get the token
    if (nfc_i2c_get_token(1)) {

        // Read the command from EEPROM
        uint32_t cmd_raw = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_CMD, /*nb*/1);

        //---------------------------------------------------------------
        // Normal Handshaking (REQ=1 AND ACK=0 AND ERR=0)
        //---------------------------------------------------------------
        // REQ=cmd_raw[7], ACK=cmd_raw[6], ERR=cmd_raw[5], CMD=cmd_raw[4:0]
        if ((cmd_raw&0xE0)==0x80) {  // If REQ=1, ACK=0, ERR=0

            // Extract the command
            uint32_t cmd = get_bits(cmd_raw, 4, 0);

            #ifdef DEBUG
                mbus_write_message32(0x85, cmd);
            #endif

            //-----------------------------------------------------------
            // CMD: NOP
            //-----------------------------------------------------------
            if (cmd == 0x00) { nfc_set_ack(cmd); } 
            //-----------------------------------------------------------
            // CMD: START
            //-----------------------------------------------------------
            else if (cmd == 0x01) {

                // If this is a re-start
                if (get_flag(FLAG_STARTED)) { operation_back_to_default(); }

                // Update the parameters
                #ifdef USE_DEFAULT_VALUE
                    eeprom_temp_meas_start_delay = 1;
                    eeprom_temp_meas_interval    = 1;
                    eeprom_temp_meas_config      = (1 << 0);
                    eeprom_high_temp_threshold   = 700;
                    eeprom_low_temp_threshold    = 450;
                #else
                    uint32_t temp;

                    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_TEMP_THRESHOLD, /*nb*/4);
                    eeprom_high_temp_threshold = get_bits(temp, 15, 0);
                    eeprom_low_temp_threshold  = get_bits(temp, 31, 16);

                    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_MEAS_INTERVAL, /*nb*/4);
                    eeprom_temp_meas_interval    = get_bits(temp, 15, 0);
                    eeprom_temp_meas_start_delay = get_bits(temp, 31, 16);

                    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_MEAS_CONFIG, /*nb*/2);
                    eeprom_temp_meas_config      = get_bits(temp, 15, 0);
                #endif

                // Start the system
                set_flag(FLAG_STARTED, 1);

                // Update the System State
                set_system_state(/*msb*/1, /*lsb*/0, /*val*/0x2);

                // ACK
                nfc_set_ack(cmd);
            }
            //-----------------------------------------------------------
            // CMD: STOP
            //-----------------------------------------------------------
            else if (cmd == 0x02) {

                // If system is not in STARTED, set ERR.
                if (!get_flag(FLAG_STARTED)) { nfc_set_err(cmd); }
                // Stop the system
                else {
                    #ifdef DEBUG
                        mbus_write_message32(0x83, 0x00000001);
                    #endif
                    operation_back_to_default();
                    // Remove the 'Play' sign
                    eid_update_with_eeprom(eid_get_current_display() & ~DISP_PLAY);
                    // Reset FLAG_STARTED
                    set_flag(FLAG_STARTED, 0);

                    // Update the System State
                    set_system_state(/*msb*/1, /*lsb*/0, /*val*/0x3);

                    // ACK
                    nfc_set_ack(cmd);
                }
            }
            //-----------------------------------------------------------
            // CMD: UPDATE
            //-----------------------------------------------------------
            else if (cmd == 0x03) {

                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x0000000B);
                #endif

                // Update the EEPROM variables
                update_eeprom_variables();

                if (get_flag(FLAG_STARTED)) {
                    // Stop the system
                    operation_back_to_default();

                    // Remove the 'Play' sign
                    eid_update_with_eeprom(eid_get_current_display() & ~DISP_PLAY);
                    // Reset FLAG_STARTED
                    set_flag(FLAG_STARTED, 0);
                    
                    // Update the System State
                    set_system_state(/*msb*/1, /*lsb*/0, /*val*/0x3);
                }

                // ACK
                nfc_set_ack(cmd);

            }
            //-----------------------------------------------------------
            // CMD: Invalid
            //-----------------------------------------------------------
            else { nfc_set_err(cmd); }

        }
        //---------------------------------------------------------------
        // Invalid Handshaking (REQ=0 OR ACK=1 OR ERR=1)
        // NOTE: This just means there was no user (NFC) activity.
        //       Check this later when we implement the real GPIO IRQ.
        //---------------------------------------------------------------
        else {}

        // Release the token
        nfc_i2c_release_token();

        // Turn off NFC (This also resets RF_SLEEP, so the RF becomes enabled again next time NFC turns on)
        nfc_power_off();
    }
}

// SNT Timer Calibration
static void calibrate_snt_timer(void) {

    uint32_t a, b, new_val;

    // Turn of NFC, if on, to save power
    nfc_power_off();

    //------------------------------------------------------------------
    // xo_freq = 1024Hz << (sel_freq+2);
    //------------------------------------------------------------------
    // 15 sec     (sel_duration=1): threshold = (xo_freq << 4) - xo_freq
    // 7.5 sec    (sel_duration=0): threshold = (xo_freq << 3) - (xo_freq >> 1)
    //------------------------------------------------------------------

    uint32_t sel_duration = get_bit(eeprom_snt_calib_config, 7);
    uint32_t sel_freq     = get_bits(eeprom_snt_calib_config, 6, 5);
    uint32_t max_change   = eeprom_snt_base_freq >> get_bits(eeprom_snt_calib_config, 2, 0);
    uint32_t xo_freq      = 1024 << (sel_freq + 2);
    uint32_t xo_threshold = (xo_freq << (sel_duration+3)) - (xo_freq >> (1-sel_duration));

    // XO Frequency
    *REG_XO_CONF2 =             // Default  // Description
        //-----------------------------------------------------------------------------------------------------------
        ( (0x2 << 13)           // 2'h2     // XO_INJ	            #Adjusts injection period
        | (0x1 << 10)           // 3'h1     // XO_SEL_DLY	        #Adjusts pulse delay
        | (0x1 <<  8)           // 2'h1     // XO_SEL_CLK_SCN	    #Selects division ratio for SCN CLK
        | ((sel_freq+2) << 5)   // 3'h1     // XO_SEL_CLK_OUT_DIV   #Selects division ratio for the XO CLK output
        | (0x1 <<  4)           // 1'h1     // XO_SEL_LDO	        #Selects LDO output as an input to SCN
        | (0x0 <<  3)           // 1'h0     // XO_SEL_0P6	        #Selects V0P6 as an input to SCN
        | (0x0 <<  0)           // 3'h0     // XO_I_AMP	            #Adjusts VREF body bias buffer current
        );

    // Start XO clock (High Power Mode)
    xo_start_high_power(XO_WAIT_A, XO_WAIT_B);

    // Configure the XO Counter as needed (See 'Use as a Timer in Active Mode' in Section 18.4. XO Timer)
    set_xo_timer(   /*mode*/      0,
                    /*threshold*/ xo_threshold,
                    /*wreq_en*/   0,
                    /*irq_en*/    1
                );

    // Time-out Check
#ifdef CALIB_TIMEOUT_USING_SNT
    *NVIC_ISER = (0x1 << IRQ_REG7);
    // threshold for the timeout
    uint32_t snt_timeout_threshold = snt_timer_threshold+(snt_timer_1min>>1);
    if(snt_timeout_threshold==0) snt_timeout_threshold = 1; // SNT Timer does not expire at cnt=0.
    set_timeout_snt_check(/*threshold*/snt_timeout_threshold);
#else
    set_timeout32_check(45*CPU_CLK_FREQ);
#endif

    // Start the XO counter
    start_xo_cnt();

    // Read SNT counter
    a = snt_read_wup_timer();

    // Wait for WFI
    WFI();

    // Read SNT counter
    b = snt_read_wup_timer();

    // Disable XO Driver & XO Counter
    xo_stop();

    // Time-out
#ifdef CALIB_TIMEOUT_USING_SNT
    if (!stop_timeout_snt_check(0x0)) {
#else
    if (!stop_timeout32_check(0x0)) {
#endif

        // FAIL TYPE I - XO does not start
        #ifdef DEBUG
            mbus_write_message32(0x87, snt_timer_1min);
            mbus_write_message32(0x87, 0xFFFFFFFF);
            mbus_write_message32(0x88, 0x3);
        #endif

        // ERROR: XO failure
        num_calib_xo_fails++;
        if (nfc_i2c_get_token(1)) {
            nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, /*data*/num_calib_xo_fails, /*nb*/1);
            nfc_i2c_release_token();
        }

    }
    // Successful
    else {

        // Calculate the new snt_timer_1min candidate
        if (b > a) new_val = (b - a) << (3 - sel_duration);
        else       new_val = ((0xFFFFFFFF - a) + b) << (3 - sel_duration);

        uint32_t upper_limit = ((eeprom_snt_base_freq + max_change)<<6)-((eeprom_snt_base_freq + max_change)<<2);
        uint32_t lower_limit = ((eeprom_snt_base_freq - max_change)<<6)-((eeprom_snt_base_freq - max_change)<<2);

        if ((new_val > upper_limit) || (new_val < lower_limit)) {
            // ERROR: Calibration Result Out Of Expectation
            #ifdef DEBUG
                mbus_write_message32(0x87, snt_timer_1min);
                mbus_write_message32(0x87, new_val);
                mbus_write_message32(0x88, 0x1);
            #endif

            // ERROR: Calibration Max Error
            num_calib_max_err_fails++;
            if (nfc_i2c_get_token(1)) {
                nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS, /*data*/num_calib_max_err_fails, /*nb*/1);
                nfc_i2c_release_token();
            }
        }
        else {
            // SUCCESS: Update snt_timer_1min
            #ifdef DEBUG
                mbus_write_message32(0x87, snt_timer_1min);
                mbus_write_message32(0x87, new_val);
                mbus_write_message32(0x88, 0x2);
            #endif

            // Update snt_timer_1min
            num_calib_pass++;

            snt_timer_1min = new_val;
        }
    }

}

static void reset_eeprom(uint32_t fw_id) {
    // Set the Firmware ID
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_FW_ID, /*data*/fw_id, /*nb*/4);
    // Reset the Sample Count
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SAMPLE_COUNT, /*data*/0x0, /*nb*/4);
    // Reset NUM_XO/CALIB_MAX_ERR/BUF_OVERRUN
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, /*data*/0x0, /*nb*/2);
    // Reset the Command
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_CMD, /*data*/0x0, /*nb*/1);
    // Reset Start/Stop Time and User IDs
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_START_TIME, /*data*/0x0, /*nb*/16);
    // Reset the System State
    set_system_state(/*msb*/15, /*lsb*/0, /*val*/0);
}

static void update_eeprom_variables(void) {

#ifdef USE_DEFAULT_VALUE
    //--- VBAT and ADC
    eeprom_vbat_info        = 0;    // VBAT + calibration data
    eeprom_adc_t1           = 1250; // ( 1250 ) Temperature Threshold (Raw Code) for ADC. See the table above.
    eeprom_adc_t2           = 1250; // ( 1250 ) Temperature Threshold (Raw Code) for ADC. See the table above.
    eeprom_adc_t3           = 1250; // ( 1250 ) Temperature Threshold (Raw Code) for ADC. See the table above.
    eeprom_adc_t4           = 1250; // ( 1250 ) Temperature Threshold (Raw Code) for ADC. See the table above.
    eeprom_adc_offset_r1    = 0;    // (    0 ) ADC offset to be used in Region 1. 2's complement. See the table above.
    eeprom_adc_offset_r2    = 0;    // (    0 ) ADC offset to be used in Region 2. 2's complement. See the table above.
    eeprom_adc_offset_r3    = 0;    // (    0 ) ADC offset to be used in Region 3. 2's complement. See the table above.
    eeprom_adc_offset_r4    = 0;    // (    0 ) ADC offset to be used in Region 4. 2's complement. See the table above.
    eeprom_adc_offset_r5    = 0;    // (    0 ) ADC offset to be used in Region 5. 2's complement. See the table above.
    eeprom_adc_low_vbat     = 105;  // (  105 ) ADC code threshold to turn on the Low VBAT indicator.
    eeprom_adc_crit_vbat    = 95;   // (   95 ) ADC code threshold to trigger the EID crash handler.

    //--- Calibration Data
    eeprom_snt_calib_config         = ((1 << 7) | (2 << 5) | (2 << 0)); // [7]: Calibration Duration (0: 7.5sec, 1: 15sec); [6:5]: XO Frequency (0: 4kHz, 1: 8kHz, 2: 16kHz, 3: 32kHz); [4:3]: Reserved; [2:0]: Max Error allowed (# bit shift from SNT_BASE_FREQ)
    eeprom_snt_base_freq            = 1500; // SNT Timer Base Frequency in Hz (integer value only)
    
    //--- EID Configurations
    eeprom_eid_high_temp_threshold  = 1250; // ( 1250 (=20C)  ) Temperature Threshold (Raw Code) to determin HIGH and MID temperature for EID
    eeprom_eid_low_temp_threshold   = 700;  // ( 700  (=10C)  ) Temperature Threshold (Raw Code) to determin MID and LOW temperature for EID
    eeprom_eid_duration_high        = 30;   // ( 30   (=0.25s)) EID duration (ECTR_PULSE_WIDTH) for High Temperature
    eeprom_eid_fe_duration_high     = 15;   // ( 15   (=0.12s)) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for High Temperature. '0' makes it skip 'Field Erase'
    eeprom_eid_rfrsh_int_hr_high    = 2;    // ( 2    (=2hr)  ) EID Refresh interval for High Temperature (unit: hr). '0' makes it skip the refresh.
    eeprom_eid_duration_mid         = 250;  // ( 250  (=2s)   ) EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
    eeprom_eid_fe_duration_mid      = 125;  // ( 125  (=1s)   ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Mid Temperature. '0' makes it skip 'Field Erase'
    eeprom_eid_rfrsh_int_hr_mid     = 4;    // ( 4    (=4hr)  ) EID Refresh interval for Mid Temperature (unit: hr). '0' makes it skip the refresh.
    eeprom_eid_duration_low         = 500;  // ( 500  (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Low Temperature
    eeprom_eid_fe_duration_low      = 500;  // ( 500  (=4s)   ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Low Temperature. '0' makes it skip 'Field Erase'
    eeprom_eid_rfrsh_int_hr_low     = 8;    // ( 8    (=8hr)  ) EID Refresh interval for Low Temperature (unit: hr). '0' makes it skip the refresh.

    //--- Other Configurations
    eeprom_timer_calib_interval     = 10;   // (           10 ) Timer Calibration Interval. '0' means 'do not calibrate'. (unit: minute)
    eeprom_te_last_sample_res       =  5;   // (            5 ) Resolution of TE_LAST_SAMPLE (unit: minute). See comment on EEPROM_ADDR_TE_LAST_SAMPLE.

#else
    uint32_t temp;

    //--- VBAT and ADC
    eeprom_vbat_info = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_VBAT_INFO, /*nb*/2);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T1, /*nb*/4);
    eeprom_adc_t1  = get_bits(temp, 15,  0);
    eeprom_adc_t2  = get_bits(temp, 31, 16);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_T3, /*nb*/4);
    eeprom_adc_t3  = get_bits(temp, 15,  0);
    eeprom_adc_t4  = get_bits(temp, 31, 16);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R1, /*nb*/4);
    eeprom_adc_offset_r1    = get_bits(temp,  7,  0);
    eeprom_adc_offset_r2    = get_bits(temp, 15,  8);
    eeprom_adc_offset_r3    = get_bits(temp, 23, 16);
    eeprom_adc_offset_r4    = get_bits(temp, 31, 24);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_ADC_OFFSET_R5, /*nb*/3);
    eeprom_adc_offset_r5    = get_bits(temp,  7,  0);
    eeprom_adc_low_vbat     = get_bits(temp, 15,  8);
    eeprom_adc_crit_vbat    = get_bits(temp, 23, 16);

    //--- Calibration Data
    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_SNT_CALIB_CONFIG, /*nb*/3);
    eeprom_snt_calib_config         = get_bits(temp,  7,  0);,
    eeprom_snt_base_freq            = get_bits(temp, 23,  8);
    
    //--- EID Configurations
    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD, /*nb*/4);
    eeprom_eid_high_temp_threshold  = get_bits(temp, 15,  0);
    eeprom_eid_low_temp_threshold   = get_bits(temp, 31, 16);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_HIGH, /*nb*/4);
    eeprom_eid_duration_high        = get_bits(temp, 15,  0);
    eeprom_eid_fe_duration_high     = get_bits(temp, 31, 16);
    eeprom_eid_rfrsh_int_hr_high    = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_RFRSH_INT_HR_HIGH, /*nb*/1);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_MID, /*nb*/4);
    eeprom_eid_duration_mid         = get_bits(temp, 15,  0);
    eeprom_eid_fe_duration_mid      = get_bits(temp, 31, 16);
    eeprom_eid_rfrsh_int_hr_mid     = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_RFRSH_INT_HR_MID, /*nb*/1);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_LOW, /*nb*/4);
    eeprom_eid_duration_low         = get_bits(temp, 15,  0);
    eeprom_eid_fe_duration_low      = get_bits(temp, 31, 16);
    eeprom_eid_rfrsh_int_hr_low     = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_RFRSH_INT_HR_LOW, /*nb*/1);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TIMER_CALIB_INTERVAL, /*nb*/4);
    eeprom_timer_calib_interval     = get_bits(temp, 15,  0);
    eeprom_te_last_sample_res       = get_bits(temp, 31, 16);

#endif

    // Update SNT timer variables
    snt_timer_1min = (eeprom_snt_base_freq << 6) - (eeprom_snt_base_freq << 2);

}

static void buffer_init(void) {
    num_buffer_items = 0;
    buffer_next_ptr  = 0;
    buffer_first_ptr = 0;
    buffer_overrun   = 0;
    buffer_missing_sample_id = 0;
}

static uint32_t buffer_inc_ptr(uint32_t ptr) {
    if (ptr == (BUFFER_SIZE-1)) return 0;
    else return (ptr+1);
}

static uint32_t buffer_add_item(uint32_t item) {
    if (num_buffer_items < BUFFER_SIZE) {
        // Write into the memory
        mbus_copy_mem_from_local_to_remote_bulk(
            /*remote_prefix*/           MEM_ADDR,
            /*remote_memory_address*/   (uint32_t *) (buffer_next_ptr << 2),
            /*local_address*/           &item,
            /*length-1*/                0
            );
        // Increment the pointer
        buffer_next_ptr = buffer_inc_ptr(buffer_next_ptr);
        // Increment num_buffer_items
        num_buffer_items++;
        // Success
        return 1;
    }
    // Buffer Full error
    else {
        // Fail
        return 0;
    }
}

static uint32_t buffer_get_item(void) {
    if (num_buffer_items != 0) {
        // Read from the memory
        uint32_t temp;
        set_halt_until_mbus_trx();
        mbus_copy_mem_from_remote_to_any_bulk(
            /*source_prefix*/           MEM_ADDR,
            /*source_mem_addr*/         (uint32_t *) (buffer_first_ptr << 2),
            /*destination_prefix*/      PRE_ADDR,
            /*distination_mem_addr*/    &temp,
            /*length-1*/                0
            );
        set_halt_until_mbus_tx();
        // Increment the pointer
        buffer_first_ptr = buffer_inc_ptr(buffer_first_ptr);
        // Decrement num_buffer_items
        num_buffer_items--;
        // Return
        return temp;
    }
    // No item error
    else {
        return 0xFFFFFFFF;
    }
}


//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32  (void) {
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    __wfi_timeout_flag__ = 1; // Declared in PREv22E.h
    set_halt_until_mbus_tx();
    }
void handler_ext_int_timer16  (void) { *NVIC_ICPR = (0x1 << IRQ_TIMER16);    }
void handler_ext_int_reg0     (void) { *NVIC_ICPR = (0x1 << IRQ_REG0);       }
void handler_ext_int_reg1     (void) { *NVIC_ICPR = (0x1 << IRQ_REG1);       }
void handler_ext_int_reg2     (void) { *NVIC_ICPR = (0x1 << IRQ_REG2);       }
void handler_ext_int_reg3     (void) { *NVIC_ICPR = (0x1 << IRQ_REG3);       }
void handler_ext_int_reg4     (void) { *NVIC_ICPR = (0x1 << IRQ_REG4);       }
void handler_ext_int_reg5     (void) { *NVIC_ICPR = (0x1 << IRQ_REG5);       }
void handler_ext_int_reg6     (void) { *NVIC_ICPR = (0x1 << IRQ_REG6);       }
void handler_ext_int_reg7     (void) { 
    *NVIC_ICPR = (0x1 << IRQ_REG7);       
#ifdef CALIB_TIMEOUT_USING_SNT
    __wfi_timeout_flag__ = 1; // Declared in PREv22E.h
#endif
}
void handler_ext_int_mbusmem  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);   }
void handler_ext_int_mbusrx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);    }
void handler_ext_int_mbustx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);    }
void handler_ext_int_mbusfwd  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);   }

void handler_ext_int_gocep    (void) { 
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);      

    uint32_t goc_raw = *GOC_DATA_IRQ;
    *GOC_DATA_IRQ   = 0;

    // Activating System
    if (goc_raw==GOC_ACTIVATE_KEY) { 
        if (!get_flag(FLAG_ACTIVATED)) {
            // Enable EID Crash Hander
            eid_enable_crash_handler();

            // Update the flags
            set_flag(FLAG_ACTIVATED, 1); 
            set_flag(FLAG_WD_ENABLED, 1); 

            // Update the System State
            set_system_state(/*msb*/1, /*lsb*/0, /*val*/0x1);

            // Clear the display
            eid_update_with_eeprom(DISP_NONE);

        #ifdef GOC_ACTIVATE_ALSO_STARTS
            #ifdef USE_DEFAULT_VALUE
                eeprom_temp_meas_start_delay = 1;
                eeprom_temp_meas_interval    = 1;
                eeprom_temp_meas_config      = (1 << 0);
                eeprom_high_temp_threshold   = 700;
                eeprom_low_temp_threshold    = 450;
            #else
                eeprom_temp_meas_start_delay = *(GOC_DATA_IRQ+1);
                eeprom_temp_meas_interval    = *(GOC_DATA_IRQ+2);
                eeprom_temp_meas_config      = *(GOC_DATA_IRQ+3);
                eeprom_high_temp_threshold   = *(GOC_DATA_IRQ+4);
                eeprom_low_temp_threshold    = *(GOC_DATA_IRQ+5);
            #endif

            set_flag(FLAG_STARTED, 1);

            // Update the System State
            set_system_state(/*msb*/1, /*lsb*/0, /*val*/0x2);

        #endif
        }
    }
#ifdef DEVEL
    // Quick Start the temperature measurement
    else if (goc_raw==GOC_QUICK_START_KEY) { 
        if (!get_flag(FLAG_STARTED)) {
            set_flag(FLAG_STARTED, 1);

            // Update the System State
            set_system_state(/*msb*/1, /*lsb*/0, /*val*/0x2);

        }
    }
    // Start the temperature measurement
    else if (goc_raw==GOC_START_KEY) { 
        if (!get_flag(FLAG_STARTED)) {

            #ifdef USE_DEFAULT_VALUE
                eeprom_temp_meas_start_delay = 1;
                eeprom_temp_meas_interval    = 1;
                eeprom_temp_meas_config      = (1 << 0);
                eeprom_high_temp_threshold   = 700;
                eeprom_low_temp_threshold    = 450;
            #else
                eeprom_temp_meas_start_delay = *(GOC_DATA_IRQ+1);
                eeprom_temp_meas_interval    = *(GOC_DATA_IRQ+2);
                eeprom_temp_meas_config      = *(GOC_DATA_IRQ+3);
                eeprom_high_temp_threshold   = *(GOC_DATA_IRQ+4);
                eeprom_low_temp_threshold    = *(GOC_DATA_IRQ+5);
            #endif

            set_flag(FLAG_STARTED, 1);

            // Update the System State
            set_system_state(/*msb*/1, /*lsb*/0, /*val*/0x2);

        }
    }
    // Stop the ongoing temperature measurement
    else if (goc_raw==GOC_STOP_KEY) { 
        #ifdef DEBUG
            mbus_write_message32(0x83, 0x00000001);
        #endif
        operation_back_to_default();
        // Remove the 'Play' sign
        eid_update_with_eeprom(eid_get_current_display() & ~DISP_PLAY);
        // Reset FLAG_STARTED
        set_flag(FLAG_STARTED, 0);
        // Update the System State
        set_system_state(/*msb*/1, /*lsb*/0, /*val*/0x3);
    }
#endif
}

void handler_ext_int_softreset(void) { *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); }
void handler_ext_int_wakeup   (void) { *NVIC_ICPR = (0x1 << IRQ_WAKEUP);     }
void handler_ext_int_aes      (void) { *NVIC_ICPR = (0x1 << IRQ_AES);        }
void handler_ext_int_gpio     (void) { *NVIC_ICPR = (0x1 << IRQ_GPIO);       }
void handler_ext_int_spi      (void) { *NVIC_ICPR = (0x1 << IRQ_SPI);        }
void handler_ext_int_xot      (void) { *NVIC_ICPR = (0x1 << IRQ_XOT);        }


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;
    
    // Get the info on who woke up the system, then reset WAKEUP_SOURCE register.
    wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1;

    #ifdef DEBUG
        mbus_write_message32(0x80, wakeup_source);
    #endif

    // Check-in the EID Watchdog if it is enabled (STATE 6)
    if (get_flag(FLAG_WD_ENABLED) && !snt_running) eid_check_in();

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_TIMER32) | (0x1 << IRQ_XOT);
    #ifndef GOCEP_RUN_CPU_ONLY
        *NVIC_ISER = (0x1 << IRQ_GOCEP);
    #endif

    // If this is the very first wakeup, initialize the system (STATE 1)
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    #ifdef GOCEP_RUN_CPU_ONLY
        if (get_flag(FLAG_INITIALIZED) && get_bit(wakeup_source, 0)) {
            handler_ext_int_gocep();
        }
    #endif

    // Wakeup Count (increment only when SNT_IDLE)
    if (!snt_running) {
        wakeup_count++;
        #ifdef DEBUG
            mbus_write_message32(0x81, wakeup_count);
        #endif
    }

    //--------------------------------------------------------------------------
    // System Activated
    //--------------------------------------------------------------------------
    if (get_flag(FLAG_ACTIVATED)) {

        #ifdef DEBUG
            mbus_write_message32(0x82, (0x00 << 24) | tmp_state);
            mbus_write_message32(0x82, (0x01 << 24) | cnt_temp_meas_start_delay);
            mbus_write_message32(0x82, (0x02 << 24) | cnt_temp_meas_interval);
            mbus_write_message32(0x82, (0x03 << 24) | cnt_disp_refresh_interval);
            mbus_write_message32(0x82, (0x04 << 24) | cnt_timer_calib_interval);
            mbus_write_message32(0x82, (0x0E << 24) | eeprom_sample_count);
            mbus_write_message32(0x82, (0x0F << 24) | temp_sample_count);
        #endif

        //----------------------------------------------------------------------
        // Checking NFC Activity
        //----------------------------------------------------------------------
        if (!snt_running) { nfc_check_cmd(); }

        //----------------------------------------------------------------------
        // Temperature Measurement FSM
        //----------------------------------------------------------------------
        if (tmp_state==TMP_IDLE) {
            if (get_flag(FLAG_STARTED)) {
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000002);
                    mbus_write_message32(0x83, (0x01 << 24) | eeprom_temp_meas_start_delay);
                    mbus_write_message32(0x83, (0x02 << 24) | eeprom_temp_meas_interval);
                    mbus_write_message32(0x83, (0x03 << 24) | eeprom_timer_calib_interval);
                    mbus_write_message32(0x83, (0x04 << 24) | eeprom_te_last_sample_res);
                    mbus_write_message32(0x83, (0x05 << 24) | eeprom_temp_meas_config);
                    mbus_write_message32(0x83, (0x06 << 24) | eeprom_high_temp_threshold);
                    mbus_write_message32(0x83, (0x07 << 24) | eeprom_low_temp_threshold);
                #endif
                if (eeprom_temp_meas_start_delay==1) {
                    tmp_state = TMP_ACTIVE;
                    cnt_temp_meas_interval = eeprom_temp_meas_interval;
                }
                else tmp_state = TMP_PENDING;
                // Reset counters
                cnt_temp_meas_start_delay   = 2;
                cnt_temp_meas_interval      = 1;    // maybe redundant
                temp_sample_count           = 0;
                eeprom_sample_count         = 0;
                cnt_te_last_sample          = 0;    // maybe redundant
                cnt_te_last_sample_res      = 0;    // maybe redundant
                // Display 
                eid_update_with_eeprom(DISP_PLAY);
                // Calibrate the SNT timer
                cnt_timer_calib_interval = eeprom_timer_calib_interval;
            }
            #ifdef DEBUG
            else {
                mbus_write_message32(0x83, 0x00000003);
            }
            #endif
        }
        else if (tmp_state==TMP_PENDING) {
            if (cnt_temp_meas_start_delay == eeprom_temp_meas_start_delay) {
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000004);
                #endif
                tmp_state = TMP_ACTIVE;
                cnt_temp_meas_interval = eeprom_temp_meas_interval;
            }
            else {
                cnt_temp_meas_start_delay++;
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000005);
                #endif
            }
        }
        else if (tmp_state==TMP_ACTIVE) {
            if (cnt_temp_meas_interval == eeprom_temp_meas_interval) {
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000006);
                #endif
                snt_operation();
                cnt_temp_meas_interval = 0;
                cnt_te_last_sample     = 0;
                cnt_te_last_sample_res = 0;
            }
            #ifdef DEBUG
            else {
                mbus_write_message32(0x83, 0x00000007);
            }
            #endif

            // Write TE_LAST_SAMPLE
            if (  (cnt_te_last_sample_res == 0) 
               || (cnt_te_last_sample_res == eeprom_te_last_sample_res)) {
                if(nfc_i2c_get_token(30)) {
                    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TE_LAST_SAMPLE, /*data*/cnt_te_last_sample, /*nb*/2);
                    nfc_i2c_release_token();
                }
                cnt_te_last_sample_res = 0;
            }

            cnt_temp_meas_interval++;
            cnt_te_last_sample++;
            cnt_te_last_sample_res++;

        }

        //----------------------------------------------------------------------
        // Refresh Display
        //----------------------------------------------------------------------
        if (disp_refresh_interval != 0) {
            if (cnt_disp_refresh_interval == disp_refresh_interval) {
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000009);
                #endif
                eid_update(eid_get_current_display());
                cnt_disp_refresh_interval = 0;
            }
            cnt_disp_refresh_interval++;
        }
        else {
            cnt_disp_refresh_interval = 1;
        }

        //----------------------------------------------------------------------
        // SNT Timer Calibration
        //----------------------------------------------------------------------
        if (eeprom_timer_calib_interval != 0) {
            if (cnt_timer_calib_interval == eeprom_timer_calib_interval) {
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x0000000A);
                #endif
                calibrate_snt_timer();
                cnt_timer_calib_interval = 0;
            }
            cnt_timer_calib_interval++;
        }
        else {
            cnt_timer_calib_interval = 1;
        }

        //----------------------------------------------------------------------
        // Go to sleep
        //----------------------------------------------------------------------
        snt_timer_threshold = snt_timer_threshold + snt_timer_1min;
        if(snt_timer_threshold==0) snt_timer_threshold = 1; // SNT Timer does not expire at cnt=0.
        #ifdef DEBUG
            mbus_write_message32(0x84, snt_timer_threshold);
        #endif
        operation_sleep_snt_timer(/*auto_reset*/0, /*threshold*/snt_timer_threshold);
    }


    //--------------------------------------------------------------------------
    // Invalid Operation - Go to Sleep
    //--------------------------------------------------------------------------
    operation_sleep();
    while(1) asm("nop");
    return 1;
}
