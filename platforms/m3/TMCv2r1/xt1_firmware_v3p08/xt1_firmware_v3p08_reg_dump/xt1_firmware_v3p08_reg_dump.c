//*******************************************************************************************
// XT1 (TMCv2r1) FIRMWARE
// Version 3.08 (standard)
//------------------------
#define HARDWARE_ID 0x01005843  // XT1r1 Hardware ID
#define FIRMWARE_ID 0x0308      // [15:8] Integer part, [7:0]: Non-Integer part
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
//#define ENABLE_XO_PAD
//#define USE_SHORT_REFRESH
//#define ENABLE_DEBUG_SYSTEM_CONFIG
//#define DEBUG_TEMP_CALIB                  // Display when eeprom_temp_calib changes
//#define DEBUG_AES_KEY                     // Display when aes_key[3:0] changes
//#define DEBUG_RAW                         // Display Raw Sample information

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
//#define FLAG_INITIALIZED        23
#define FLAG_INITIALIZED        22

#define FLAG_ENUMERATED         0
//#define FLAG_INITIALIZED        1
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
    #define WAKEUP_INTERVAL_IDLE    5   // (Default: 720) Wakeup Interval for XT1_IDLE (unit: minutes)
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
#define WAKEUP_BY_NFC       WAKEUP_BY_GPIO1
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

volatile uint32_t state;
volatile uint32_t gidx;

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

volatile uint32_t crc32;                    // Adler-32 CRC Value

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
static void operation_dead(void);
static void operation_init(void);
void disable_all_irq_except_timer32(void);
void fail_handler(uint32_t id);

//--- FSM
static void set_system(uint32_t target);
static void reset_system_status(uint32_t target);
static void commit_status(uint32_t forced);
static void meas_temp_adc (uint32_t go_sleep);

//--- XO & SNT Timers

//--- E-Ink Display
static void eid_update_configs(void);
static void eid_update_display(uint32_t seg);

//--- NFC

//--- EEPROM Variables
static void update_system_configs(uint32_t use_default);
static void update_temp_coefficients(void);
#ifdef ENABLE_DEBUG_SYSTEM_CONFIG
static void debug_system_configs(void);
#endif

//--- Write Buffer
static void buffer_init  (void);

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

    // Power off NFC
    nfc_power_off();

    mbus_sleep_all(); 
    while(1);
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

    state = 0;
    gidx  = 0;

    // Initialize the status
    __nfc_on__ = 0x0;
    __nfc_i2c_token__ = 0x0;
    __nfc_i2c_nak__ = 0x0;

//    // Initialize EEPROM
//    for (gidx=0; gidx<280; gidx=gidx+4) {
//        nfc_i2c_byte_write(/*e2*/0, /*addr*/gidx, /*data*/0x00000000, /*nb*/4);
//    }

    // Initialize EEPROM (Total 73 blocks)
    // Block 70 - 142 (Byte 280 - 571)
    for (gidx=0; gidx<74; gidx++) {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/280+(gidx<<2), /*data*/0x00000000, /*nb*/4);
    }

    set_flag(FLAG_INITIALIZED, 1);
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
            eeprom_user_config_0.high_temp_threshold   = 1800;   // 100C
            eeprom_user_config_0.low_temp_threshold    = 400;    // -40C
            eeprom_user_config_1.temp_meas_interval    = 1;      // 1 min
            eeprom_user_config_1.temp_meas_start_delay = 0;      // 0 min
            num_cons_excursions   = 1;
        }

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
    
                // Trigger the Crash Handler
                eid_trigger_crash();
    
                // Go to indefinite sleep
                snt_set_wup_timer(/*auto_reset*/1, /*threshold*/0);
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



//-------------------------------------------------------------------
// XO & SNT Timers
//-------------------------------------------------------------------




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
    meas_temp_adc(/*go_sleep*/0);

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
    if ((eid_expected_display != eid_seg_in_eeprom) && !status.hbr_flag) {
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



//-------------------------------------------------------------------
// NFC
//-------------------------------------------------------------------





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



//-------------------------------------------------------------------
// Write Buffer
//-------------------------------------------------------------------

static void buffer_init(void) {
    cnt_buf_meas.valid = 0;
    pmu_buf.valid = 0;
    dat_buf_addr.valid = 0;
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

// FAIL_ID_SNT: Temp Sensor Timeout (while staying in Active)
// FAIL_ID_WUP: SNT Timer Read Timeout (snt_sync_read_wup_timer)
void fail_handler(uint32_t id) {

    if (id==FAIL_ID_SNT) {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/572, /*data*/0xDEAD7777, /*nb*/4);
    }
    else if (id==FAIL_ID_WUP) {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/572, /*data*/0xDEAD8888, /*nb*/4);
    }
    else {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/572, /*data*/(0xDEAD<<16)|id, /*nb*/4);
    }

    nfc_power_off();
    mbus_sleep_all(); 
    while(1);
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

    uint32_t data;

    // Set PRE Watchdog
    *TIMERWD_GO  = 0;

    // Disable MBus Watchdog
    *REG_MBUS_WD = 0;

    // Enable IRQs for timeout check
    *NVIC_IPR0 = (0x3 << 30); // Lowest Priority for TIMER32 IRQ.
    *NVIC_IPR3 = (0x3 << 30); // Lowest Priority for REG7 IRQ.
    *NVIC_ISER = (0x1 << IRQ_TIMER32);

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    // Marker
    nfc_i2c_byte_write(/*e2*/0, /*addr*/280, /*data*/0x99999999, /*nb*/4);

    // Dump SNT Registers (33 Registers)
    // EEPROM Block 71 - 103 (Byte: 284 - 415)
    // SNT Register N is at EEPROM Block (N+71)
    for(gidx=0; gidx<33; gidx++) {
        set_halt_until_mbus_trx();
        mbus_copy_registers_from_remote_to_local(
            /*remote_prefix*/       SNT_ADDR,
            /*remote_reg_start*/    gidx,
            /*local_reg_start*/     0x03,
            /*length_minus_one*/    0
        );
        set_halt_until_mbus_tx();
        nfc_i2c_byte_write(/*e2*/0, /*addr*/284+(gidx<<2), /*data*/*REG3, /*nb*/4);
        delay(10000);
    }

    // Marker
    nfc_i2c_byte_write(/*e2*/0, /*addr*/416, /*data*/0xAAAAAAAA, /*nb*/4);

    // Dump EID Registers (33 Registers)
    // EEPROM Block 105 - 137 (Byte: 420 - 551)
    // EID Register N is at EEPROM Block (N+105)
    for(gidx=0; gidx<33; gidx++) {
        set_halt_until_mbus_trx();
        mbus_copy_registers_from_remote_to_local(
            /*remote_prefix*/       EID_ADDR,
            /*remote_reg_start*/    gidx,
            /*local_reg_start*/     0x03,
            /*length_minus_one*/    0
        );
        set_halt_until_mbus_tx();
        nfc_i2c_byte_write(/*e2*/0, /*addr*/420+(gidx<<2), /*data*/*REG3, /*nb*/4);
        delay(10000);
    }

    // Marker
    nfc_i2c_byte_write(/*e2*/0, /*addr*/552, /*data*/0xBBBBBBBB, /*nb*/4);

    // Dump PMU (MBus) Registers (3 Registers)
    // EEPROM Block 139 - 141 (Byte: 556 - 567)
    // PMU Register N+81 is at EEPROM Block (N+139)
    // PMU Register N is at EEPROM Block (N+58)
    for(gidx=0; gidx<3; gidx++) {
        set_halt_until_mbus_trx();
        mbus_copy_registers_from_remote_to_local(
            /*remote_prefix*/       PMU_ADDR,
            /*remote_reg_start*/    (gidx+81),
            /*local_reg_start*/     0x03,
            /*length_minus_one*/    0
        );
        set_halt_until_mbus_tx();
        nfc_i2c_byte_write(/*e2*/0, /*addr*/556+(gidx<<2), /*data*/*REG3, /*nb*/4);
        delay(10000);
    }

    // Marker
    nfc_i2c_byte_write(/*e2*/0, /*addr*/568, /*data*/0xCCCCCCCC, /*nb*/4);

    // End of Testing
    nfc_power_off();
    mbus_sleep_all(); 
    while(1);
   
    return 1;
}
