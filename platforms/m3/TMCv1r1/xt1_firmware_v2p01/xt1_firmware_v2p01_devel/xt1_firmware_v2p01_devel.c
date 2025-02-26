//*******************************************************************************************
// XT1 (TMCv1r1) FIRMWARE
// Version 2.01 (devel)
// id   _devel   _std    description
//  0   4070    3755    Original
//  1   3957    3642    Add EXTRA_STAT to make HIGHEST/LOWEST_TEMP and HIGH/LOW_SAMPLE_0-3 optional
//  2   3965    3645    Add eeprom_user_config_t
//  3   3964    3645    Add eeprom_adc_th_t and eeprom_adc_t (UINT64_C to cast constants)
//  4   3886    3567    Using upper/lower in eeprom_adc_th_t and eeprom_adc_t.
//  5   3815    3494    Add eeprom_eid_threshold_t, eeprom_eid_duration_t, eeprom_eid_rfrsh_int_hr_t, eeprom_misc_config_t, eeprom_temp_calib_t
//  6   3791    3475    Add upper/lower in eeprom_user_config_t
//  7   3728    3419    Replace nfc_i2c_byte_read with nfc_i2c_word_read
//  8   3622    3314    TMCv1r1: Add __USE_HCODE__ for PMU & NFC variables
//                               gpio_set_dir_with_mask()    -> gpio_set_dir()
//                               gpio_write_data_with_mask() -> gpio_write_data()
//                               Add __i2c_password_t__
//  9   3520    3212    TMCv1r1: Add __USE_HCODE__ for EID
//                              Removed args from eid_init()
// 10   3634    3324    Enable EXTRA_STAT
// 11   3646    3337    Add excursion_status_t for EXTRA_STAT
//                      Add 'use_default' arg in update_system_config() again.
// ref  3487    3319    Firmware 1.15
//------------------------
#define HARDWARE_ID 0x01005843  // XT1r1 Hardware ID
#define FIRMWARE_ID 0x0201      // [15:8] Integer part, [7:0]: Non-Integer part
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Mar 28 2022 - Version 1.00
//                  - Hard-forked from xt1_firmware_v0p99
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
//                                                  0xF: debug_system_config()() is called
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
//  0x87    delta_xo_val                        (xo_val_curr - xo_val_prev) or (xo_val_curr + (0xFFFFFFFF - xo_val_prev) + 1)
//  0x88    a                                   Calculated a (Custom fixed-point, N=30)
//  0x89    temp_snt_freq                       Newly Calculated SNT frequency
//  0x8A    wakeup_interval_sec                 New wakeup interval (in seconds)
//  0x8B    snt_duration                        Newly calculated snt_duration
//  0x8C    snt_threshold                       Newly calculated snt_threshold
//  0x8D    snt_threshold_prev                  Previous snt_threshold
//  0x8F    snt_threshold                       Going into sleep with snt_threshold
//  
//  -----------------------------------------------------------------------------------------
//  < Temperature Measurement >
//  -----------------------------------------------------------------------------------------
//  0x90    value                               Measured Temperature (EEPROM will NOT be written in this session)
//                                                  [31:16] Raw code (*SNT_TARGET_REG_ADDR)
//                                                  [15: 0] After the conversion (snt_temp_val) "10 x (T + 80)" where T is the actual temperature in celsius degree.
//  0x91    value                               Measured Temperature (EEPROM WILL be written in this session)
//                                                  [31:16] Raw code (*SNT_TARGET_REG_ADDR)
//                                                  [15: 0] After the conversion (snt_temp_val) "10 x (T + 80)" where T is the actual temperature in celsius degree.
//  0x92    temp_sample_count                   Temperature Sample ID
//  0x93    value                               VBAT and SAR Ratio Information
//                                                  [   28] Critical VBAT (crit_vbat)
//                                                  [   24] Low VBAT (low_vbat)
//                                                  [23:16] ADC Offset (adc_offset)
//                                                  [15: 8] SAR Ratio (pmu_sar_ratio)
//                                                  [ 7: 0] ADC Reading (pmu_adc_vbat_val)
//  0x94    value                               PMU SAR Ratio is changing
//                                                  [15: 8] Previous SAR Ratio
//                                                  [ 7: 0] New SAR Ratio
//  0x95    value                               Temperature Data read from the write buffer
//                                                  [15: 0] Temperature Data read from the write buffer (buf_temp_val)
//  0x96    comp_sample_id                      Compression Sample ID
//  0x97    value                               (Reversed) Raw/Encoded data to be stored
//                                                  [31:24] Number of bits
//                                                  [23: 0] Reversed code
//  0x9A    value                               Measured Temperature (from snt_get_temp_raw())
//                                                  [15: 0] Raw code (*SNT_TARGET_REG_ADDR)
//  0x9B    value                               Measured Temperature (from snt_get_temp())
//                                                  [15: 0] After the conversion (snt_temp_val) "10 x (T + 80)" where T is the actual temperature in celsius degree.
//  0x9C    byte_addr                           [Raw Sample] Byte Address
//  0x9D    buf_temp_val                        [Raw Sample] Raw Temperature Data
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
//  0xA4    0x00000001                          eid_update_display() requires a new temperature measurement
//  0xA5    eid_updated_at_low_temp             1 indicates that the display has been updated at a low temperature
//  0xA6    eid_updated_at_crit_temp            1 indicates that the display has a pending update due to a critical temperature
//  0xA7    eid_update_type                     Update type used for the display update
//                                                  0x0: Global Update
//                                                  0x1: Local Update
//
//  -----------------------------------------------------------------------------------------
//  < NFC and EEPROM >
//  -----------------------------------------------------------------------------------------
//  0xB0    cnt_update_eeprom                   Counter value that tracks when to write into EEPROM. This session does NOT write into EEPROM.
//  0xB1    cnt_update_eeprom                   Counter value that tracks when to write into EEPROM. This session DOES write into EEPROM.
//  0xB2    target                              The returned 'target' value from nfc_check_cmd()
//  0xB3    value                               NFC Command Info
//                                                  [23:16]: Command Parameter (cmd_param)
//                                                  [ 4: 0]: Command
//                                                              0x00: NOP         
//                                                              0x01: START       
//                                                              0x02: STOP        
//                                                              0x03: GETSEC        
//                                                              0x07: HRESET      
//                                                              0x10: DISPLAY     
//                                                              0x1E: RESET       
//  0xB4    value                               ACK with Data Successful and 'value' is written in EEPROM_ADDR_CMD
//  0xB5    value                               ACK with Data Fail; 'value' was supposed to be written in EEPROM_ADDR_CMD
//
//  -----------------------------------------------------------------------------------------
//  < Math Functions >
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
//  -----------------------------------------------------------------------------------------
//  < I2C Transaction > - Defined in TMCv1r1.c
//  -----------------------------------------------------------------------------------------
//  0xD0    {E2, nb, addr}                      Byte Write Information.
//                                                  [   28] E2
//                                                  [27:24] Number of Bytes (1-4)
//                                                  [23: 0] Byte Address in EEPROM
//  0xD1    data                                Data use for Byte Write
//  0xD2    {E2, nb, addr}                      Byte Read Information.
//                                                  [   28] E2
//                                                  [27:24] Number of Bytes (1-4)
//                                                  [23: 0] Byte Address in EEPROM
//  0xD3    data                                Data read using Byte Read
//
//  0xD4    {nw, addr}                          Word Read Information.
//                                                  [31:24] Number of Words (1-64)
//                                                  [23: 0] Byte Address in EEPROM
//  0xD5    {ptr}                               Word Read Information.
//                                                  [31: 0] ptr (where the read data is stored)
//  0xD6    0x1                                 Word Read operation has been successfully done
//
//
//  -----------------------------------------------------------------------------------------
//  < I2C Token > - Defined in TMCv1r1.c
//  -----------------------------------------------------------------------------------------
//  0xE0    0x01000000                          FAIL - Timeout while trying to get the I2C token
//  0xE0    0x01000001                          PASS - Successfully got the I2C token
//  0xE0    0x01000002                          FAIL - Number of Trials exceeds
//  0xE0    0x01000003                          PASS - Token already exists
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

#include "TMCv1r1.h"

//*******************************************************************************************
// DEVEL Mode
//*******************************************************************************************
// Enable 'DEVEL' for the following features:
//      - Send debug messages
//      - Use default values rather than grabbing the values from EEPROM
#define DEVEL
#ifdef DEVEL
    //#define DEVEL_SHORT_REFRESH
    //#define DEVEL_ENABLE_XO_PAD
#endif

//*******************************************************************************************
// OPTIONAL FEATURES
//*******************************************************************************************
// EXTRA_STAT: If enabled, it stores the following stats:
//                  HIGHEST_TEMP, LOWEST_TEMP, HIGH_SAMPLE_0-3, LOW_SAMPLE_0-3
#define EXTRA_STAT

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
#define FLAG_ENUMERATED     0
#define FLAG_INITIALIZED    1
#define FLAG_XO_INITIALIZED 2
#define FLAG_WD_ENABLED     3
#define FLAG_MAIN_CALLED    4   // Sleep has been bypassed before the current active session. Thus, main() has called from within operation_sleep(). This implies that xo_val_curr is inaccurate.
#define FLAG_INVLD_XO_PREV  5   // Sleep has been bypassed before the previous active session. This implies that xo_val_prev is inaccurate.
#define FLAG_BOOTUP_DONE    6
#define FLAG_AES_KEY_SET    7
#define FLAG_CONFIG_SET     8
#define FLAG_USE_DEFAULT    9   // Using default values for the initialization parameters, rather than the real values read from EEPROM. See update_system_configs();

//*******************************************************************************************
// WRITE BUFFER CONFIGURATIONS
//*******************************************************************************************
#define EEPROM_UPDATE_INTERVAL  1   // Every n measurement(s), XT1 writes the current/previous measurements into EEPROM
#define BUFFER_SIZE             10  // Buffer Size; It must be larger than EEPROM_UPDATE_INTERVAL

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
#ifdef DEVEL_SHORT_REFRESH
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
#define CMD_DISPLAY     0x10
#define CMD_SRESET      0x1E
#define CMD_DEBUG       0x1F

// XT1 Constants
#define ACK 1
#define ERR 0

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
                                                    //          Number of consecutive measurements required to change SAR ratio or trigger Low VBAT warning/Crash Handler.
        unsigned eid_crit_temp_threshold    :   1;  // [  4] EID_CRIT_TEMP_THRESHOLD
                                                    //          Display does not update when temperature is lower than EID_CRIT_TEMP_THRESHOLD
                                                    //          0x1: -20C; 0x0: -25C
        unsigned eid_disable_local_update   :   1;  // [  5] EID_DISABLE_LOCAL_UPDATE
                                                    //          0x1: Use GLOBAL_UPDATE for all temperatures.
                                                    //          0x0: Use LOCAL_UPDATE below EEPROM_ADDR_EID_LOW_TEMP_THRESHOLD. Use GLOBAL_UPDATE otherwise.
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


//-------------------------------------------------------------------------------------------
// Handshake in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in nfc_check_cmd() function.
//-------------------------------------------------------------------------------------------

volatile uint32_t cmd;              // Command
volatile uint32_t cmd_param;        // Command Parameter
volatile uint32_t sample_type;      // If 1, store 16-bit Raw value.                                                                                                                              
                                    // If 0, store converted & compressed values.                                                                                                                 


//-------------------------------------------------------------------------------------------
// System Status in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: Most of these variables are reset in reset_system_status() function.
//-------------------------------------------------------------------------------------------

volatile uint32_t temp_sample_count;                // Counter for sample count (# actual temperature measurements)
volatile uint32_t eeprom_sample_count;              // Counter for sample count (# temperature measurements stored into EEPROM)
volatile uint32_t num_calib_xo_fails;               // Number of Calibration fails (XO timeout/unstable)
volatile uint32_t num_calib_max_err_fails;          // Number of Calibration fails (MAX_CHANGE error)
volatile uint32_t system_state;                     // System State (See comment on EEPROM_ADDR_SYSTEM_STATE)

#ifdef EXTRA_STAT
union eeprom_peak_temp_t {
    struct {
        unsigned max    :   16; // [15: 0] Highest Temperature observed
        unsigned min    :   16; // [31:16] Lowest Temperature observed
    };
    uint32_t value;
};
volatile union eeprom_peak_temp_t eeprom_peak_temp;

union excursion_status_t {
    struct {
        unsigned sid        :   17; // [16: 0] Sample ID
        unsigned num        :   7;  // [23:17] Number of consecutive samples
        unsigned addr       :   7;  // [30:24] Last updated EEPROM Byte Address
        unsigned ongoing    :   1;  // [   31] If 1, it indicates that the current excursion is still ongoing.
                                    //         If 0, it indicates that the streak of the previous excursion has ended.
    };
    uint32_t value;
};
volatile union excursion_status_t high_ex_status;
volatile union excursion_status_t low_ex_status;

volatile uint32_t high_sample_id_0; // 1st sample ID that violates the high threshold (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t high_sample_id_1; // 2nd sample ID that violates the high threshold (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t high_sample_id_2; // 3rd sample ID that violates the high threshold (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t low_sample_id_0;  // 1st sample ID that violates the low threshold  (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t low_sample_id_1;  // 2nd sample ID that violates the low threshold  (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t low_sample_id_2;  // 3rd sample ID that violates the low threshold  (valid only if bit[23]=1; bit[22:0] is the sample ID.)
#endif


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

//--- SNT & Temperature Measurement
volatile uint32_t xt1_state;                // TMP state
volatile uint32_t snt_temp_raw;             // Latest temp measurement: Raw code
volatile uint32_t snt_temp_val;             // Latest temp measurement: 10x(T+80)
volatile uint32_t snt_temp_valid;           // 1 indicates that the snt_temp_raw has been measured in the current wakeup session.
volatile uint32_t cnt_update_eeprom;        // Counter to track when to write the measurement into EEPROM.

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
volatile uint32_t num_buffer_items;         // Number of items in the buffer
volatile uint32_t buffer_next_ptr;          // Pointer where the next item will be inserted
volatile uint32_t buffer_first_ptr;         // Pointer of the first item to be read
volatile uint32_t buffer_overrun;           // Buffer Overrun Status
volatile uint32_t buffer_missing_sample_id; // ID of the missing sample in case of the buffer overrun. It keeps the very first missing smaple ID only.

//--- PMU ADC and SAR Ratio
volatile uint32_t pmu_adc_vbat_val;         // Latest PMU ADC Reading
volatile uint32_t pmu_sar_ratio;            // Suggested new SAR Ratio

volatile uint32_t sar_adc_addr;             // For debugging (temp sensor calibration)
volatile uint32_t sar_adc_data;             // For debugging (temp sensor calibration)
                                            // [   15] XO Fail
                                            // [   14] Max Error Fail
                                            // [10: 8] 0x1: SAR_RATIO=0x48
                                            //         0x2: SAR_RATIO=0x4C
                                            //         0x3: SAR_RATIO=0x54
                                            //         0x4: SAR_RATIO=0x60
                                            // [ 7: 0] ADC Value

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep Functions
static void operation_sleep (uint32_t check_snt);
static void operation_sleep_snt_timer(uint32_t check_snt);
static void operation_init (void);

//--- FSM
static void set_system(uint32_t target);
static void reset_system_status(void);
static void set_system_state(uint32_t msb, uint32_t lsb, uint32_t val);
static uint32_t snt_get_temp_raw (void);
static uint32_t snt_get_temp (uint32_t temp_raw);
static void snt_operation (uint32_t update_eeprom);

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
static void debug_system_configs(void);
static void update_user_configs(void);
static void update_aes_key(void);

//--- Write Buffer
static void buffer_init(void);
static uint32_t buffer_inc_ptr(uint32_t ptr);
static uint32_t buffer_add_item(uint32_t item);
static uint32_t buffer_get_item(void);

//--- Data Compression
uint32_t tcomp_encode (uint32_t value);
uint32_t reverse_bits (uint32_t bit_stream, uint32_t num_bits);

//--- 128-bit qword Handling
void set_qword (uint32_t pattern);
void sub_qword (uint32_t msb, uint32_t lsb, uint32_t value);
void copy_qword (uint32_t* targ);

//--- AES & CRC
void aes_encrypt(uint32_t* pt);
void aes_encrypt_eeprom(uint32_t addr);
uint32_t calc_crc32_128(uint32_t* src, uint32_t crc_prev);
void save_qword (uint32_t addr, uint32_t commit_crc);

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

    // FIXME: Stop any ongoing EID operation (but how...?)

    // Power off NFC
    nfc_power_off();

    // Clear all pending IRQs; otherwise, PREv22E replaces the sleep msg with a selective wakeup msg
    *NVIC_ICER = 0xFFFFFFFF;

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
        uint32_t dist;

        if (snt_threshold >= snt_val) { dist = snt_threshold - snt_val;                    } 
        else                          { dist = snt_threshold + (0xFFFFFFFF - snt_val) + 1; }

        if ((dist < dist_min) || (dist > dist_max)) {
            set_flag(FLAG_MAIN_CALLED, 1);
            // For safely, disable/clear all pending IRQs.
            *NVIC_ICPR = 0xFFFFFFFF;
            // Call main()
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
    snt_set_wup_timer(/*auto_reset*/0, /*threshold*/snt_threshold);
    #ifdef DEVEL
        mbus_write_message32(0x8F, snt_threshold);
    #endif
    operation_sleep(/*check_snt*/check_snt);
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
        *REG_SYS_CONF =       (0x0 << 8)    // 1'h0     ENABLE_SOFT_RESET	#If 1, Soft Reset will occur when there is an MBus Memory Bulk Write message received and the MEM Start Address is 0x2000
                            | (0x1 << 7)    // 1'h1     PUF_CHIP_ID_SLEEP
                            | (0x1 << 6)    // 1'h1     PUF_CHIP_ID_ISOLATE
                            | (0x8 << 0);   // 5'h1E    WAKEUP_ON_PEND_REQ	#[4]: GIT (PRE_E Only), [3]: GPIO (PRE only), [2]: XO TIMER (PRE only), [1]: WUP TIMER, [0]: GOC/EP

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

        // Turn on the SNT timer clock
        snt_start_timer(/*wait_time*/2*DLY_1S);

        // Start the SNT counter
        snt_enable_wup_timer(/*auto_reset*/0);

        //-------------------------------------------------
        // XO Settings
        //-------------------------------------------------
        //--- XO Frequency
        *REG_XO_CONF2 =             // Default  // Description
            //-----------------------------------------------------------------------------------------------------------
            ( (0x2          << 13)  // 2'h2     // XO_INJ	            #Adjusts injection period
            | (0x1          << 10)  // 3'h1     // XO_SEL_DLY	        #Adjusts pulse delay
            | (0x1          <<  8)  // 2'h1     // XO_SEL_CLK_SCN	    #Selects division ratio for SCN CLK
            | (XO_FREQ_SEL  <<  5)  // 3'h1     // XO_SEL_CLK_OUT_DIV   #Selects division ratio for the XO CLK output; freq = (2^XO_SEL_CLK_OUT_DIV) in kHz.
            | (0x1          <<  4)  // 1'h1     // XO_SEL_LDO	        #Selects LDO output as an input to SCN
            | (0x0          <<  3)  // 1'h0     // XO_SEL_0P6	        #Selects V0P6 as an input to SCN
            | (0x0          <<  0)  // 3'h0     // XO_I_AMP	            #Adjusts VREF body bias buffer current
            );

        xo_stop();  // Default value of XO_START_UP is wrong in RegFile, so need to override it.

        //--- Start XO clock
        xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B, /*start_cnt*/0);
        // Update the flag
        set_flag(FLAG_XO_INITIALIZED, 1);

        //--- Configure and Start the XO Counter
        set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0);
        start_xo_cnt();

    #ifdef DEVEL_ENABLE_XO_PAD
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

        // Set default values for CALIB and AES_KEY
        eeprom_temp_calib.a = TEMP_CALIB_A_DEFAULT;
        eeprom_temp_calib.b = TEMP_CALIB_B_DEFAULT;
        for (i=0; i<4; i++) *(aes_key+i) = 0x0;

        // Initialize the EEPROM variables
        update_system_configs(/*use_default*/1);

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
        set_system(/*target*/XT1_RESET);

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);

        //-------------------------------------------------
        // Go to Sleep for 10s.
        //-------------------------------------------------
        // this provides the information for the very first SNT calibration
        snt_freq = 1500;
        snt_duration = 15000;
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
    // MBus Halt Setting
    set_halt_until_mbus_tx();

    #ifdef DEVEL
        mbus_write_message32(0x72, target);
    #endif

    //------------------------------------------
    // XT1_CRASH
    //------------------------------------------
    if (target==XT1_CRASH) {
        // Update the System State
        set_system_state(/*msb*/7, /*lsb*/7, /*val*/0x1);
        // Turn off NFC
        nfc_power_off();
        // Update the state
        xt1_state = XT1_CRASH;
        return;
    }

    //------------------------------------------
    // XT1_RESET and XT1_PEND
    //------------------------------------------
    else if ((target==XT1_RESET) || (target==XT1_PEND)) {

        // Critical Variables
        snt_temp_raw        = 2534; // Assume 20C by default
        snt_temp_val        = 1000; // Assume 20C by default
        snt_threshold_prev  = 0;
        snt_threshold       = 0;

        // Data Compression Variables
        comp_sample_id   = 0;
        comp_prev_sample = 0;
        comp_bit_pos     = 0;
        crc32            = 1;
        set_qword(/*pattern*/0xFFFFFFFF);  // qword = all 1s.

        // Reset the Write Buffer
        buffer_init();

        //--------------------------------------
        // XT1_RESET
        //--------------------------------------
        if (target==XT1_RESET) {

            // Reset some variables
            cmd                      = CMD_NOP;
            sample_type              = 0;
            disp_refresh_interval    = 1440;    // Every 24 hrs
            eid_duration             = 100;
            eid_disp_before_user_mod = 0;
            eid_updated_at_low_temp  = 0;
            eid_updated_at_crit_temp = 0;
            eid_update_type          = EID_GLOBAL;
            snt_temp_valid           = 0;
            ssls                     = 0;

            // Display all segments
            eid_update_display(/*seg*/DISP_ALL);

            // Reset System Status Variables
            reset_system_status();

            // Update/Reset System Configuration
            update_system_configs(/*use_default*/0);

            // Reset the System State
            system_state = 0;
            set_system_state(/*msb*/7, /*lsb*/0, /*val*/XT1_RESET);

            // Update the state
            xt1_state = XT1_RESET;
        }
        //--------------------------------------
        // XT1_PEND
        //--------------------------------------
        else if (target==XT1_PEND) {

            // Display Play Sign
            eid_update_display(/*seg*/DISP_PLAY);

            // Reset System Status Variables
            reset_system_status();

            // Start Delay
            wakeup_interval = eeprom_user_config.temp_meas_start_delay;

            // Set the System State
            set_system_state(/*msb*/5, /*lsb*/0, /*val*/(0<<5)|(sample_type<<4)|(0<<3)|XT1_PEND);

            // Update the state
            xt1_state = XT1_PEND;
        }
    }

    //------------------------------------------
    // XT1_IDLE
    //------------------------------------------
    else if (target==XT1_IDLE) {

        // Reset variables
        wakeup_interval   = WAKEUP_INTERVAL_IDLE;
        cnt_update_eeprom = 0;

        // During Boot-Up
        if (!get_flag(FLAG_BOOTUP_DONE))
            eid_update_display(/*seg*/DISP_CHECK|DISP_PLUS|DISP_MINUS|DISP_LOWBATT);
        // If coming from XT1_RESET: Display Check only
        else if (xt1_state==XT1_RESET)
            // If correctly initialized, display Check Only
            if (!get_flag(FLAG_USE_DEFAULT))
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
        set_system_state(/*msb*/1, /*lsb*/0, /*val*/XT1_IDLE);

        // Update the state
        xt1_state = XT1_IDLE;
    }

    //------------------------------------------
    // XT1_ACTIVE
    //------------------------------------------
    else if (target==XT1_ACTIVE) {
        wakeup_interval = eeprom_user_config.temp_meas_interval;

        // Reset the System State
        set_system_state(/*msb*/1, /*lsb*/0, /*val*/XT1_ACTIVE);

        // Update the state
        xt1_state = XT1_ACTIVE;
    }

    // If XT1_RESET, it also needs to go to XT1_IDLE
    if (xt1_state==XT1_RESET) set_system(/*target*/XT1_IDLE);
}

static void reset_system_status(void) {

    #ifdef DEVEL
        mbus_write_message32(0x73, 0x0);
    #endif

    // Reset the Sample Count
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SAMPLE_COUNT, /*data*/0, /*nb*/4);
    temp_sample_count   = 0;
    eeprom_sample_count = 0;

    // Reset NUM_XO/CALIB_MAX_ERR
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, /*data*/0, /*nb*/2); // EEPROM_ADDR_NUM_CALIB_XO_FAILS, EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS
    num_calib_xo_fails      = 0;
    num_calib_max_err_fails = 0;

    // Following variables are not set/reset here
    //-----------------------------------------------------------
    // EEPROM_ADDR_SYSTEM_STATE (system_state)
    // EEPROM_ADDR_DISPLAY
    // EEPROM_ADDR_LAST_ADC
    // EEPROM_ADDR_LAST_SAR

    #ifdef EXTRA_STAT
        // Reset Measurement Snapshot
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HIGHEST_TEMP,  /*data*/0, /*nb*/4); // EEPROM_ADDR_HIGHEST_TEMP, EEPROM_ADDR_LOWEST_TEMP
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_SAMPLE_0, /*data*/0, /*nb*/3);
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_SAMPLE_1, /*data*/0, /*nb*/3);
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_SAMPLE_2, /*data*/0, /*nb*/3);
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_LOW_SAMPLE_0,  /*data*/0, /*nb*/3);
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_LOW_SAMPLE_1,  /*data*/0, /*nb*/3);
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_LOW_SAMPLE_2,  /*data*/0, /*nb*/3);
        eeprom_peak_temp.value = (/*min*/0xFFFF << 16) | (/*max*/0x0 << 0);
        high_ex_status.value  = (/*ongoing*/0 << 31) | (/*addr*/(EEPROM_ADDR_HIGH_SAMPLE_0 - 3) << 24) | (/*num*/0 << 17) | (/*sid*/0 << 0);
        low_ex_status.value   = (/*ongoing*/0 << 31) | (/*addr*/(EEPROM_ADDR_LOW_SAMPLE_0  - 3) << 24) | (/*num*/0 << 17) | (/*sid*/0 << 0);
        high_sample_id_0      = 0;
        high_sample_id_1      = 0;
        high_sample_id_2      = 0;
        low_sample_id_0       = 0;
        low_sample_id_1       = 0;
        low_sample_id_2       = 0;
    #endif
}

static void set_system_state(uint32_t msb, uint32_t lsb, uint32_t val) {
    system_state = set_bits(/*var*/system_state, /*msb_idx*/msb, /*lsb_idx*/lsb, /*value*/val);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SYSTEM_STATE, /*data*/system_state, /*nb*/1);
}

static uint32_t snt_get_temp_raw (void) {

    // Enable REG IRQ
    *NVIC_ISER = (0x1 << (SNT_TARGET_REG_IDX+8));

    // Turn on SNT Temperature Sensor
    snt_temp_sensor_power_on();

    // Reset the Temp Sensor
    snt_temp_sensor_reset();

    // Release the reset for the Temp Sensor
    snt_temp_sensor_start();

    // Wait
    WFI();

    // Update snt_temp_valid
    snt_temp_valid = 1;

    #ifdef DEVEL
        mbus_write_message32(0x9A, *SNT_TARGET_REG_ADDR);
    #endif

    // Return the measurement
    return *SNT_TARGET_REG_ADDR;
}

static uint32_t snt_get_temp (uint32_t temp_raw) {
    // NOTE: snt_temp_val is "10 x (T + 80)" where T is the actual temperature in celsius degree
    uint32_t result = tconv(/* dout */ temp_raw,
                            /*   a  */ eeprom_temp_calib.a, 
                            /*   b  */ eeprom_temp_calib.b, 
                            /*offset*/ COMP_OFFSET_K);

    #ifdef DEVEL
        mbus_write_message32(0x9B, result);
    #endif

    return result;
}

static void snt_operation (uint32_t update_eeprom) {

    // Get the measurement
    snt_temp_raw = snt_get_temp_raw();

    // Convert the raw data into temperature
    snt_temp_val = snt_get_temp(snt_temp_raw);

    // Increment the sample count
    temp_sample_count++;

    // Assert temp sensor isolation & turn off temp sensor power
    snt_temp_sensor_reset();
    snt_temp_sensor_power_off();

    #ifdef DEVEL
        mbus_write_message32(0x90|update_eeprom, (snt_temp_raw<<16)|snt_temp_val);
    #endif

    // Determine ADC Offset, ADC Low VBAT threshold, and ADC Critical VBAT threshold
    uint32_t adc_offset;
    if      (snt_temp_val > eeprom_adc_th.t1) { adc_offset = eeprom_adc.offset_r1; }
    else if (snt_temp_val > eeprom_adc_th.t2) { adc_offset = eeprom_adc.offset_r2; }
    else if (snt_temp_val > eeprom_adc_th.t3) { adc_offset = eeprom_adc.offset_r3; }
    else if (snt_temp_val > eeprom_adc_th.t4) { adc_offset = eeprom_adc.offset_r4; }
    else                                   { adc_offset = eeprom_adc.offset_r5; }

    // VBAT Measurement and SAR_RATIO Adjustment
    pmu_adc_vbat_val = pmu_read_adc();
    pmu_sar_ratio    = pmu_calc_new_sar_ratio(  /*adc_val*/         pmu_adc_vbat_val, 
                                                /*offset*/          adc_offset, 
                                                /*num_cons_meas*/   eeprom_misc_config.pmu_num_cons_meas
                                            );

    uint32_t low_vbat  = pmu_check_low_vbat (   /*adc_val*/         pmu_adc_vbat_val, 
                                                /*offset*/          adc_offset, 
                                                /*threshold*/       eeprom_adc.low_vbat,  
                                                /*num_cons_meas*/   eeprom_misc_config.pmu_num_cons_meas
                                            );

    uint32_t crit_vbat = pmu_check_crit_vbat(   /*adc_val*/         pmu_adc_vbat_val, 
                                                /*offset*/          adc_offset, 
                                                /*threshold*/       eeprom_adc.crit_vbat, 
                                                /*num_cons_meas*/   eeprom_misc_config.pmu_num_cons_meas
                                            );

    #ifdef DEVEL
        mbus_write_message32(0x92, temp_sample_count);
        mbus_write_message32(0x93, (crit_vbat<<28)|(low_vbat<<24)|(adc_offset<<16)|(pmu_sar_ratio<<8)|(pmu_adc_vbat_val));
    #endif

    // Change the SAR ratio
    pmu_set_sar_ratio(pmu_sar_ratio);

    // Adjust e-Ink Settings: Refresh Interval and Active Duration
    eid_update_configs();

    // Write data into the write buffer
    if (xt1_state == XT1_ACTIVE) {
        if(!buffer_add_item((pmu_sar_ratio<<24)|(pmu_adc_vbat_val<<16)|((sample_type?snt_temp_raw:snt_temp_val)&0xFFFF))) {
            // ERROR: Buffer Overrun
            if (!buffer_overrun) {
                buffer_overrun = 1;
                // Store the sample ID. Sample ID starts from 0.
                buffer_missing_sample_id = temp_sample_count - 1;
            }
        }
    }

    //--- Read the current display
    uint32_t curr_seg = eid_get_curr_seg();
    uint32_t new_seg;

    // XT1_ACTIVE
    if (xt1_state == XT1_ACTIVE) {

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

        //--- Add Play
        new_seg = curr_seg | DISP_PLAY;

        //--- High Temperature
        if (snt_temp_val > eeprom_user_config.high_temp_threshold) {
            new_seg = new_seg & ~DISP_CHECK;    // Remove Check
            new_seg = new_seg | DISP_CROSS;     // Add Cross
            new_seg = new_seg | DISP_PLUS;      // Add Plus
        }
        //--- Low Temperature
        else if (snt_temp_val < eeprom_user_config.low_temp_threshold) {
            new_seg = new_seg & ~DISP_CHECK;    // Remove Check
            new_seg = new_seg | DISP_CROSS;     // Add Cross
            new_seg = new_seg | DISP_MINUS;     // Add Minus
        }
        //--- Normal Temperature
        else if (!get_bit(curr_seg, SEG_PLUS) && !get_bit(curr_seg, SEG_MINUS)) {
            new_seg = new_seg | DISP_CHECK;     // Add Check
        }
    }
    // XT1_IDLE 
    else {
        new_seg = curr_seg;
    }

    //--- Show Low VBATT Indicator (if needed)
    if (!get_bit(new_seg, SEG_LOWBATT) && low_vbat) {
        new_seg = new_seg | DISP_LOWBATT;
    }


    /////////////////////////////////////////////////////////////////
    // Write into EEPROM/Display
    //---------------------------------------------------------------

    if (update_eeprom) {
        // Store the Measured Temp data
        while (num_buffer_items!=0) {

            // Read the buffer
            uint32_t buf_val = buffer_get_item();
            uint32_t buf_temp_val = buf_val&0xFFFF;

            // Store ADC Reading and SAR_RATIO
            nfc_i2c_byte_write(/*e2*/0, 
                /*addr*/ EEPROM_ADDR_LAST_ADC,      // EEPROM_ADDR_LAST_ADC, EEPROM_ADDR_LAST_SAR
                /*data*/ get_bits(buf_val, 31, 16),
                /* nb */ 2
                );


            //--------------------------------------------------------------------------------

            #ifdef DEVEL
                mbus_write_message32(0x95, buf_temp_val);
                mbus_write_message32(0x96, comp_sample_id);
            #endif

            // STORE THE RAW DATA
            if (sample_type) {

                    uint32_t byte_addr = (eeprom_sample_count<<1)+EEPROM_ADDR_DATA_RESET_VALUE;

                    // Store the Sample Count
                    nfc_i2c_byte_write(/*e2*/0, 
                        /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, 
                        /*data*/ (eeprom_sample_count+1),
                        /* nb */ 4
                        );

                    // Store the raw data
                    //  Max Sample Count = (8192 - 128) / 2 = 4032
                    //  However, we split this into two and store the raw data into the first half,
                    //  and SAR/ADC values in to the second half.
                    //  i.e., max sample count = 4032 / 2 = 2016
                    //      Byte# 128 - Byte#4159: Raw Data      (each sample is 16-bit)
                    //      Byte#4160 - Byte#8192: SAR/ADC Value (each sample is 16-bit)
                    if (eeprom_sample_count<2016) {
                        nfc_i2c_byte_write( /*e2*/   0, 
                                            /*addr*/ byte_addr, 
                                        /*data*/ buf_temp_val,
                                            /*nb*/   2);

                    // Store ADC & SAR
                    uint32_t sar_temp = get_bits(buf_val, 31, 24);
                    if      (sar_temp==0x48) buf_val = set_bits(buf_val, 31, 24, 0x1);
                    else if (sar_temp==0x4C) buf_val = set_bits(buf_val, 31, 24, 0x2);
                    else if (sar_temp==0x54) buf_val = set_bits(buf_val, 31, 24, 0x3);
                    else if (sar_temp==0x60) buf_val = set_bits(buf_val, 31, 24, 0x4);

                    sar_adc_addr = byte_addr + (2016<<1);
                    sar_adc_data = buf_val >> 16;

                    nfc_i2c_byte_write( /*e2*/   0,
                                        /*addr*/ sar_adc_addr,  // Starting from Byte#4160
                                        /*data*/ sar_adc_data,  // See Description
                                        /*nb*/   2);

                    #ifdef DEVEL
                        mbus_write_message32(0x9C, byte_addr);
                        mbus_write_message32(0x9D, buf_temp_val);
                        mbus_write_message32(0x9E, sar_adc_addr);
                        mbus_write_message32(0x9F, sar_adc_data);
                    #endif

                }

                    // Update Valid Sample Bit
                    if (eeprom_sample_count==0) {
                        set_system_state(/*msb*/5, /*lsb*/5, /*val*/0x1);
                    }

                    eeprom_sample_count++;

            }
            // STORE THE CONVERTED & COMPRESSED DATA
            else {

                uint32_t num_bits;
                uint32_t reverse_code;
                //--- Store the Uncompressed Data (128*N-th samples)
                if (comp_sample_id==0) {
                    num_bits = COMP_UNCOMP_NUM_BITS;
                    reverse_code = reverse_bits(/*bit_stream*/buf_temp_val, /*num_bits*/COMP_UNCOMP_NUM_BITS);
                }
                //--- Calculate the Delta
                else {
                    uint32_t comp_result = tcomp_encode(/*value*/buf_temp_val - comp_prev_sample);
                    num_bits = comp_result >> 24;
                    reverse_code = comp_result&0xFFFFF;
                }
                //--- Store the current temperature in comp_prev_sample
                comp_prev_sample = buf_temp_val;

                #ifdef DEVEL
                    mbus_write_message32(0x97, (num_bits<<24)|reverse_code);
                #endif

                // Find out the byte address of comp_bit_pos
                uint32_t bit_pos_start = comp_bit_pos;
                uint32_t bit_pos_end   = bit_pos_start + num_bits - 1;
                uint32_t mem_avail     = (bit_pos_end < 64512); // NOTE: 64512 = (8192 - 128) * 8
                uint32_t last_qword_id = mem_avail ? (bit_pos_end>>7) : 503;

                // Store the Sample Count
                eeprom_sample_count++;

                nfc_i2c_byte_write(/*e2*/0, 
                    /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, 
                    /*data*/ (last_qword_id<<23)|(eeprom_sample_count&0x7FFFFF),
                    /* nb */ 4
                    );

                // Update Valid Sample Bit
                if (eeprom_sample_count==1) {
                    set_system_state(/*msb*/5, /*lsb*/5, /*val*/0x1);
                }

                // MEMORY IS AVAILABLE
                if (mem_avail) {

                    // CASE I ) Entire bits go into the same qword (bit_pos_start>>7 == bit_pos_end>>7)
                    if ((bit_pos_start>>7)==(bit_pos_end>>7)) {
                        uint32_t end_at_bndry = ((bit_pos_end&0x7F)==0x7F);

                        sub_qword(/*msb*/bit_pos_end&0x7F, /*lsb*/bit_pos_start&0x7F, /*value*/reverse_code);
                        save_qword( /*addr*/ ((bit_pos_start>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                    /*commit_crc*/ end_at_bndry
                                    );

                        if (end_at_bndry) set_qword(/*pattern*/0xFFFFFFFF);
                    }
                    // CASE II) Bits go across different qwords (bit_pos_start>>7 != bit_pos_end>>7)
                    else {
                        sub_qword(/*msb*/127, /*lsb*/bit_pos_start&0x7F, /*value*/reverse_code);
                        save_qword( /*addr*/ ((bit_pos_start>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                    /*commit_crc*/ 1
                                    );

                        set_qword(/*pattern*/0xFFFFFFFF);

                        sub_qword(/*msb*/bit_pos_end&0x7F, /*lsb*/0, /*value*/reverse_code>>(128-(bit_pos_start&0x7F)));
                        save_qword( /*addr*/ ((bit_pos_end>>7)<<4) + EEPROM_ADDR_DATA_RESET_VALUE,
                                    /*commit_crc*/ 0    // Assuming that 'reverse_code' is less than 128 bits.
                                    );
                    }

                    comp_sample_id++;
                    if (comp_sample_id==COMP_SECTION_SIZE) comp_sample_id = 0;
                    comp_bit_pos = bit_pos_end + 1;

                    //----------------------------------------------------------------------------

                #ifdef EXTRA_STAT
                    // Update Measurement Snapshot
                    if (buf_temp_val > eeprom_peak_temp.max) {
                        eeprom_peak_temp.max = buf_temp_val;
                        nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_HIGHEST_TEMP, /*data*/ eeprom_peak_temp.max, /*nb*/ 2);
                    }
                    if (buf_temp_val < eeprom_peak_temp.min) {
                        eeprom_peak_temp.min = buf_temp_val;
                        nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_LOWEST_TEMP, /*data*/ eeprom_peak_temp.min, /*nb*/ 2);
                    }

                    //--- Update the streaks
                    uint32_t curr_addr, curr_num;
                    if (buf_temp_val > eeprom_user_config.high_temp_threshold) {
                        // End low_temp streak, if any.
                        low_ex_status.ongoing = 0;
                        // Get the current status
                        curr_addr = high_ex_status.addr;
                        curr_num  = high_ex_status.num;
                        // If there the current streak is ongoing
                        if (high_ex_status.ongoing) {
                            if (curr_num != 0x3F) {
                                curr_num++;
                                high_ex_status.num = curr_num;
                                nfc_i2c_byte_write(/*e2*/0, /*addr*/ curr_addr, /*data*/ high_ex_status.value, /*nb*/ 3);
                            }
                        }
                        // If this is a start of a new streak
                        else {
                            if (curr_addr < EEPROM_ADDR_HIGH_SAMPLE_2) {
                                curr_addr += 3;
                                high_ex_status.value = (/*ongoing*/1 << 31) | (/*addr*/curr_addr << 24) | (/*num*/1 << 17) | (/*sid*/eeprom_sample_count - 1);
                                nfc_i2c_byte_write(/*e2*/0, /*addr*/ curr_addr, /*data*/ high_ex_status.value, /*nb*/ 3);
                            }
                        }
                    }
                    else if (buf_temp_val < eeprom_user_config.low_temp_threshold) {
                        // End high_temp streak, if any.
                        high_ex_status.ongoing = 0;
                        // Get the current status
                        curr_addr = low_ex_status.addr;
                        curr_num  = low_ex_status.num;
                        // If there the current streak is ongoing
                        if (low_ex_status.ongoing) {
                            if (curr_num != 0x3F) {
                                curr_num++;
                                low_ex_status.num = curr_num;
                                nfc_i2c_byte_write(/*e2*/0, /*addr*/ curr_addr, /*data*/ low_ex_status.value, /*nb*/ 3);
                            }
                        }
                        // If this is a start of a new streak
                        else {
                            if (curr_addr < EEPROM_ADDR_LOW_SAMPLE_2) {
                                curr_addr += 3;
                                low_ex_status.value = (/*ongoing*/1 << 31) | (/*addr*/curr_addr << 24) | (/*num*/1 << 17) | (/*sid*/eeprom_sample_count - 1);
                                nfc_i2c_byte_write(/*e2*/0, /*addr*/ curr_addr, /*data*/ low_ex_status.value, /*nb*/ 3);
                            }
                        }
                    }
                    else {
                        high_ex_status.ongoing = 0;
                        low_ex_status.ongoing = 0;
                    }
                #endif

                // MEMORY IS FULL
                }
                else { // Roll-over disabled
                    comp_bit_pos = 65536;
                    // Update Memory Full Flag
                    set_system_state(/*msb*/3, /*lsb*/3, /*val*/0x1);

                }
            }

            //--------------------------------------------------------------------------------
        }

        // If buffer overrun, set the failure flag
        if (buffer_overrun) {
            set_system_state(/*msb*/6, /*lsb*/6, /*val*/0x1);
        }

    }

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
            if (!get_flag(FLAG_USE_DEFAULT))
                eid_update_display(/*seg*/DISP_CHECK);
            // If using the default values, display 'backslash' and 'tick'
            else                           
                eid_update_display(/*seg*/DISP_BACKSLASH|DISP_TICK);
            // Update the flag
            set_flag(FLAG_BOOTUP_DONE, 1);
       }
       else if (// Display has a pending update due to a critical temperature
                (eid_updated_at_crit_temp && (snt_temp_val > eid_crit_temp_threshold))
                // Display has been updated at a low temperature
             || (eid_updated_at_low_temp && (snt_temp_val > eeprom_eid_threshold.low))
                // User-configured Refresh interval
             || ((disp_refresh_interval != 0) && ((disp_min_since_refresh + wakeup_interval) > disp_refresh_interval)) 
             ){
           eid_update_display(/*seg*/eid_get_curr_seg());
       }
    }

    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    // EID Crash Handler
    //---------------------------------------------------------------
    //--- If VBAT is too low, trigger the EID Watchdog (System Crash)
    if (crit_vbat) {

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

        uint32_t temp = (*REG_XOT_VAL_U << 16) | (*REG_XOT_VAL_L & 0xFFFF);
        uint32_t temp_prev = temp - 1;  // Make it different from temp
        uint32_t num_same_reads = 1;
        uint32_t num_try = 0;

        while ((num_same_reads < 2) && (num_try<5)) {
            temp_prev = temp;
            temp = (*REG_XOT_VAL_U << 16) | (*REG_XOT_VAL_L & 0xFFFF);
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
                    uint32_t delta_xo;
                    if (xo_val_curr > xo_val_prev) delta_xo = xo_val_curr - xo_val_prev;
                    else delta_xo = xo_val_curr + (0xFFFFFFFF - xo_val_prev) + 1;

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

        if (sample_type) {
            if (xt1_state==XT1_ACTIVE) {
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
        restart_xo(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

        // Calculate the next threshold
        if (skip_calib) {
            snt_threshold = snt_read_wup_timer() + snt_duration;
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
        snt_threshold = snt_read_wup_timer() + snt_duration;

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
        mbus_write_message32(0x8C, snt_threshold);
    #endif

}

static uint32_t calc_ssls(void) {

    uint32_t snt_curr_val = snt_read_wup_timer();
    uint32_t snt_delta;

    // CASE I) snt_curr_val > snt_threshold_prev
    if (snt_curr_val > snt_threshold_prev) {
        snt_delta = snt_curr_val - snt_threshold_prev;
    }
    // CASE II) snt_curr_val <= snt_threshold_prev
    else {
        // CASE II-A) Normal Counter Roll-Over i.e., (snt_threshold - snt_curr_val) is very large
        if (snt_threshold_prev - snt_curr_val > (snt_freq << 18)) { // Corresponds to ~73hrs; See operation_sleep().
            snt_delta = snt_curr_val + (0xFFFFFFFF - snt_threshold_prev) + 1;
        }
        // CASE II-B) Sleep before the previous wakeup was bypassed i.e., (snt_threshold - snt_curr_val) is very small, like, < 10 seconds
        else {
            // Report SSLS=0
            return 0;
        }
    }

    // For CASE I and CASE II-A, divide snt_delta by snt_freq to get SSLS
    return div(/*numer*/snt_delta, /*denom*/snt_freq, /*n*/0)&0xFFFF;
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
    if (snt_temp_val > eeprom_eid_threshold.high) {
        new_val = eeprom_eid_rfrsh_int_hr.high;
        eid_duration = eeprom_eid_duration.high;
    }
    else if (snt_temp_val > eeprom_eid_threshold.low) { 
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
    #ifdef DEVEL_SHORT_REFRESH
        disp_refresh_interval = new_val; // Dummy line to avoid the warning 'new_val set but not used'
        disp_refresh_interval = (WAKEUP_INTERVAL_IDLE<<1);
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

    // Enable low-power active
    set_high_power_history(1);

    // Turn off NFC, if on, to save power
    nfc_power_off();

    // Get the current display
    uint32_t prev_seg = eid_get_current_display();

    // Measure the temperature and re-configure, if needed.
    if (!snt_temp_valid) {
        #ifdef DEVEL
            mbus_write_message32(0xA4, 0x1);
        #endif
        snt_temp_raw = snt_get_temp_raw();
        snt_temp_val = snt_get_temp(snt_temp_raw);
        eid_update_configs();
    }

    // E-ink Update
    // Update the flag if it is a critical temperature
    if (snt_temp_val > eid_crit_temp_threshold) {
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
    if (snt_temp_val > eeprom_eid_threshold.low)
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
                mbus_write_message32(0xB4, data);
            #endif
            return 1;
        }
        nfc_i2c_stop();
        num_try--;
    }
    #ifdef DEVEL
        mbus_write_message32(0xB5, data);
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
            mbus_write_message32(0xB3, (cmd_param<<16)|cmd);
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
            // AES key is not set -OR- CONFIG has not executed yet
            if (!get_flag(FLAG_AES_KEY_SET) || !get_flag(FLAG_CONFIG_SET)) {
                nfc_set_ack(/*ack*/ERR);
                return 0xDEAD;
            }
            // Good to go
            else {
                sample_type = get_bit(cmd_param, 1);
                nfc_set_ack(/*ack*/ACK);
                return XT1_PEND;
            }
        }
        //-----------------------------------------------------------
        // CMD: STOP
        //-----------------------------------------------------------
        else if (cmd == CMD_STOP) {
            // AES key is not set -OR- XT1 is NOT running
            if (!get_flag(FLAG_AES_KEY_SET) || ((xt1_state!=XT1_PEND) && (xt1_state!=XT1_ACTIVE))) {
                nfc_set_ack(/*ack*/ERR);
                return 0xDEAD;
            }
            // Key is set -AND- XT1 is running
            else {
                nfc_set_ack(/*ack*/ACK);
                ssls = calc_ssls();
                return XT1_IDLE;
            }
        }
        //-----------------------------------------------------------
        // CMD: GETSEC
        //-----------------------------------------------------------
        else if (cmd == CMD_GETSEC) {
            // Unit is currently running
            if ((xt1_state==XT1_PEND) || (xt1_state==XT1_ACTIVE))
                nfc_set_ack_with_data(/*data*/calc_ssls());
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
            if (!get_flag(FLAG_AES_KEY_SET)) {
                set_flag(FLAG_AES_KEY_SET, 1);
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
        // CMD: NOP, HARD_RESET, DISPLAY
        //-----------------------------------------------------------
        else if (  (cmd == CMD_NOP)
                || (cmd == CMD_CONFIG)
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
        //eeprom_misc_config.eid_disable_local_update =  0;
        //eeprom_misc_config.eid_crit_temp_threshold  =  0;
        //eeprom_misc_config.pmu_num_cons_meas        =  5;
        eeprom_misc_config.value = (0 << 5) | (0 << 4) | (5 << 0);

        // Update the flag
        set_flag(FLAG_USE_DEFAULT, 1);
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

        if (init_status == INIT_TRIG_ALL) {

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

        // Reset INIT_STATUS
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_INIT_STATUS, /*data*/INIT_RESET, /*nb*/2);

        // Update the flag
        set_flag(FLAG_USE_DEFAULT, 0);
    }

    // EID_CRIT_TEMP_THRESHOLD Configuration using 10x(T+80)
    eid_crit_temp_threshold = eeprom_misc_config.eid_crit_temp_threshold ?
                                600 :   // 0x1: -20C
                                550 ;   // 0x0: -25C

    #ifdef DEVEL
        mbus_write_message32(0xE1, eeprom_eid_duration.high);
        mbus_write_message32(0xE2, eeprom_eid_duration.mid);
        mbus_write_message32(0xE3, eeprom_eid_duration.low);
        mbus_write_message32(0xE4, (uint32_t) &eeprom_eid_duration.value);
        mbus_write_message32(0xE5, (uint32_t) &eeprom_eid_duration);
    #endif
}

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
        /* nb */  1);

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

static void update_user_configs(void) {
    #ifdef DEVEL
        mbus_write_message32(0x73, 0x3);
    #endif

    // Read the user configuration
    nfc_i2c_word_read(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_TEMP_THRESHOLD, /*nw*/2, /*ptr*/ (volatile uint32_t *)&eeprom_user_config.value);

    // Set Flag
    set_flag(FLAG_CONFIG_SET, 1);

    #ifdef DEVEL
        mbus_write_message32(0x74, eeprom_user_config.lower);
        mbus_write_message32(0x75, eeprom_user_config.upper);
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
    set_flag(FLAG_AES_KEY_SET, 1);

    #ifdef DEVEL
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

//-------------------------------------------------------------------
// Data Compression
//-------------------------------------------------------------------

//-----------------------------------------------
//  value       Encoded Bit             Num Bits
//  Range       Stream                  Used
//-----------------------------------------------
//  0           0                       1
//  +1          10                      2
//  -1          110                     3
//  +2          11100                   5
//  -2          11101                   5
//  -16~+15     {11110, value&0x1F}     10
//  -64~+63     {111110, value&0x7F}    13
//  -2047~+2048 {111111, value&0xFFF}   18
//-----------------------------------------------

// return value:
// [31:24] Number of valid bits in the Encoded code
// [17: 0] Encoded code (bit reversed)

uint32_t tcomp_encode (uint32_t value) {
    uint32_t num_bits;
    uint32_t code;
    // value=0: code=0
    if (value == 0) {
        num_bits = 1; code = 0x0;
    }
    // value=+1: code=10
    else if (value == 0x00000001) {
        num_bits = 2; code = 0x2;
    }
    // value=-1: code=110
    else if (value == 0xFFFFFFFF) {
        num_bits = 3; code = 0x6;
    }
    // value=+2: code=11100
    else if (value == 0x00000002) {
        num_bits = 5; code = 0x1C;
    }
    // value=-2: code=11101
    else if (value == 0xFFFFFFFE) {
        num_bits = 5; code = 0x1D;
    }
    // value=-16~+15: code=11110+value(5bits)
    else if ((value < 16) || (value > (0xFFFFFFFF-16))) {
        num_bits = 10; code = (0x1E<<5) | (value&0x1F);
    }
    // value=-64~+63: code=111110+value(7bits)
    else if ((value < 64) || (value > (0xFFFFFFFF-64))) {
        num_bits = 13; code = (0x3E<<7) | (value&0x7F);
    }
    // value=-2048~+2047: code=111111+value(12bits)
    else {
        num_bits = 18; code = (0x3F<<12) | (value&0xFFF);
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

void copy_qword (uint32_t* targ) {
    uint32_t i;
    for (i=0; i<4; i++) *(targ+i) = *(qword+i);
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

void save_qword (uint32_t addr, uint32_t commit_crc) {

    uint32_t text[4];

    // Make a clone
    copy_qword(/*targ*/text);

    // Encrypt
    aes_encrypt(/*pt*/text);

    // Save into EEPROM
    nfc_i2c_word_write( /* e2 */ 0,
                        /*addr*/ addr,
                        /*data*/ text,
                        /* nw */ 4
                        );
    // Calculate CRC
    uint32_t new_crc32 = calc_crc32_128(/*src*/text, /*crc_prev*/crc32);

    // Save CRC
    nfc_i2c_byte_write( /* e2 */ 0,
                        /*addr*/ EEPROM_ADDR_CRC,
                        /*data*/ new_crc32,
                        /* nb */ 4
                        );

    // Commit CRC
    if (commit_crc) crc32 = new_crc32;

}

//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

//void handler_ext_int_wakeup   (void) { *NVIC_ICPR = (0x1 << IRQ_WAKEUP);     }
//void handler_ext_int_softreset(void) { *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); }
//void handler_ext_int_gocep    (void) { *NVIC_ICPR = (0x1 << IRQ_GOCEP);      }
void handler_ext_int_timer32  (void) {
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    __wfi_timeout_flag__ = 1; // Declared in PREv22E.h
    set_halt_until_mbus_tx();
    }
//void handler_ext_int_timer16  (void) { *NVIC_ICPR = (0x1 << IRQ_TIMER16);    }
//void handler_ext_int_mbustx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);    }
//void handler_ext_int_mbusrx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);    }
//void handler_ext_int_mbusfwd  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);   }
//void handler_ext_int_reg0     (void) { *NVIC_ICPR = (0x1 << IRQ_REG0);       }
void handler_ext_int_reg1     (void) { *NVIC_ICPR = (0x1 << IRQ_REG1);       }
//void handler_ext_int_reg2     (void) { *NVIC_ICPR = (0x1 << IRQ_REG2);       }
//void handler_ext_int_reg3     (void) { *NVIC_ICPR = (0x1 << IRQ_REG3);       }
//void handler_ext_int_reg4     (void) { *NVIC_ICPR = (0x1 << IRQ_REG4);       }
//void handler_ext_int_reg5     (void) { *NVIC_ICPR = (0x1 << IRQ_REG5);       }
//void handler_ext_int_reg6     (void) { *NVIC_ICPR = (0x1 << IRQ_REG6);       }
//void handler_ext_int_reg7     (void) { *NVIC_ICPR = (0x1 << IRQ_REG7);       }
//void handler_ext_int_mbusmem  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);   }
void handler_ext_int_aes      (void) { *NVIC_ICPR = (0x1 << IRQ_AES);        }
//void handler_ext_int_gpio     (void) { *NVIC_ICPR = (0x1 << IRQ_GPIO);       }
//void handler_ext_int_spi      (void) { *NVIC_ICPR = (0x1 << IRQ_SPI);        }
//void handler_ext_int_xot      (void) { *NVIC_ICPR = (0x1 << IRQ_XOT);        }


//********************************************************************
// MAIN function starts here             
//********************************************************************

int main(void) {

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;

    // Read the XO value for the calibration later
    snt_skip_calib  = 0;    // By default, it does the normal calibration
    calib_status    = 0;    // Reset Calibration status
    xo_val_prev     = xo_val_curr;
    xo_val_curr_tmp = get_xo_cnt(); // It returns 0 if XO has not been initialized yet

    // Reset variables
    set_high_power_history(0);
    snt_temp_valid = 0;
    aes_key_valid = 0;
    
    // Get the info on who woke up the system, then reset WAKEUP_SOURCE register.
    if (get_flag(FLAG_MAIN_CALLED)) {
        wakeup_source = 0x90; // Treat it as if the SNT timer has woken up the system. Set bit[7] as well. See WAKEUP_SOURCE.
        set_flag(FLAG_MAIN_CALLED, 0);
    }
    else {
        wakeup_source = *SREG_WAKEUP_SOURCE;
    }
    *SCTR_REG_CLR_WUP_SOURCE = 1;

    #ifdef DEVEL
        mbus_write_message32(0x70, wakeup_source);
        mbus_write_message32(0x71, xt1_state);
    #endif

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_TIMER32);

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    // Check-in the EID Watchdog if it is enabled
    if (get_flag(FLAG_WD_ENABLED)) eid_check_in();

    //-----------------------------------------
    // NFC HANDSHAKE
    //-----------------------------------------
    if (WAKEUP_BY_NFC) {

        uint32_t target = nfc_check_cmd();

        #ifdef DEVEL
            mbus_write_message32(0xB2, target);
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
            // Command: DISPLAY
            else if (cmd==CMD_DISPLAY) {
                if (!get_bit(eid_disp_before_user_mod, 31))
                    eid_disp_before_user_mod = (0x1<<31)|eid_get_current_display();
                eid_update_display(/*seg*/cmd_param&0x7F);
                cmd = CMD_NOP;
            }
            // Command: DEBUG
            else if (cmd==CMD_DEBUG) {
                if (cmd_param==0x00) {
                    debug_system_configs();
                }
            }
            // Command: NOP
            else {}

            // Go to Sleep
            operation_sleep(/*check_snt*/1);
        }

        // Normal User commands (START, STOP, SRESET)
        else {
            set_system(/*target*/target);
            snt_skip_calib = 1;

            if (cmd==CMD_START) {
                aes_encrypt_eeprom(/*addr*/EEPROM_ADDR_START_TIME);

                // if start_delay=0, treat it as if waken up by SNT
                if (eeprom_user_config.temp_meas_start_delay==0) {
                    set_bit(/*var*/wakeup_source, /*idx*/4, /*val*/1);
                }
            }
            else if (cmd==CMD_STOP) {
                aes_encrypt_eeprom(/*addr*/EEPROM_ADDR_STOP_TIME);
            }
        }

    }

    //-----------------------------------------
    // WAKEUP BY SNT
    //-----------------------------------------
    if (WAKEUP_BY_SNT) {

        // Counter for E-Ink Refresh 
        disp_min_since_refresh += wakeup_interval;

        #ifdef DEVEL
            mbus_write_message32(0xA1, disp_min_since_refresh);
        #endif

        uint32_t update_eeprom = 0;

        if ((xt1_state==XT1_PEND)||(xt1_state==XT1_ACTIVE)) {
            cnt_update_eeprom++;
            update_eeprom = nfc_i2c_get_token(/*num_try*/(xt1_state==XT1_PEND)?30:1) 
                            && (cnt_update_eeprom >= EEPROM_UPDATE_INTERVAL);
            #ifdef DEVEL
                mbus_write_message32(0xB0|update_eeprom, cnt_update_eeprom);
            #endif
            if (update_eeprom) cnt_update_eeprom = 0;

            if (xt1_state==XT1_PEND) set_system(/*target*/XT1_ACTIVE);
        }

        // Bookkeeping/Temp Measurement
        snt_operation(/*update_eeprom*/update_eeprom);

    }

    //-----------------------------------------
    // Other Wakeup Source - Probably a Glitch
    //-----------------------------------------
    if (!WAKEUP_BY_NFC && !WAKEUP_BY_SNT) {
        operation_sleep(/*check_snt*/1);
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
