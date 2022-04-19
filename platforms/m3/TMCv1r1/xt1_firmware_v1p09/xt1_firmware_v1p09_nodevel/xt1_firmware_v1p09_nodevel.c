//*******************************************************************************************
// XT1 (TMCv1r1) FIRMWARE
// Version 1.09 (no_devel)
//------------------------
#define HARDWARE_ID 0x01005843  // XT1r1 Hardware ID
#define FIRMWARE_ID 0x0109      // [15:8] Integer part, [7:0]: Non-Integer part
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
//                                                  0x1: update_system_configs() is called with use_default=1
//                                                  0x2: update_system_configs() is called with use_default=0
//                                                  0x3: update_user_configs() is called
//  0x74    value                               Temperature Thresholds (given by user)
//                                                  [31:16] High Threshold
//                                                  [15: 0] Low Threshold
//  0x75    value                               Measurement Configuration
//                                                  [31:16] Start Delay (unit: minutes)
//                                                  [15: 0] Measurement Interval (unit: minutes)
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
//                                                  0x3: 'restart' or 'SLEEP_BYPASSED' or 'FLAG_SLEEP_BYPASSED' is 1. Will use the previous snt_freq as it is.
//                                                  0x4: Beginning the calibration
//                                                  0x5: Calculated a is not close to 1 (allowed error: 25%)
//                                                  0x6: Calibration has failed. Will restart the XO.
//                                                  0x7: Calibration is done and successful
//                                                  0x8: Restarting the snt_threshold calculation (usually due to a new START/STOP/RESET command)
//                                                  0x9: Calculated snt_threshold was 0, so it is forced to 1.
//                                                  0xA: Sleep has been bypassed before the current active session. 
//                                                          This implies that xo_val_curr is inaccurate.
//                                                          It uses the previous snt_freq to calculate the next snt_threshold.
//                                                  0xB: Sleep has been bypassed before the previous active session.
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
//  0x98    value                               Information on the EEPROM address to store the compressed data
//                                                  [31:24] bit_offset
//                                                  [23: 0] byte_addr
//  0x9A    value                               Measured Temperature (from snt_get_temp_raw())
//                                                  [15: 0] Raw code (*SNT_TARGET_REG_ADDR)
//  0x9B    value                               Measured Temperature (from snt_get_temp())
//                                                  [15: 0] After the conversion (snt_temp_val) "10 x (T + 80)" where T is the actual temperature in celsius degree.
//
//
//  -----------------------------------------------------------------------------------------
//  < Display >
//  -----------------------------------------------------------------------------------------
//  0xA0    {0x0, pattern}                      Display has changed to 'pattern' (EEPROM Updated)
//  0xA0    {0x1, pattern}                      Display has been refreshed with 'pattern' (EEPROM Not Updated)
//  0xA0    {0x2, pattern}                      Display has changed to 'pattern' but failed to update the EEPROM.
//  0xA1    disp_min_since_refresh              Elapsed time (in minutes) since the last E-ink update
//  0xA2    value                               EID duration and Refresh Interval are set
//                                                  [31:16] EID Duration
//                                                  [15: 0] Refresh Interval (minutes)
//  0xA3    eid_update_type                     EID update type chosen 
//                                                  0: Global Update
//                                                  1: Local Update
//  0xA4    0x00000001                          eid_update_display() requires a new temperature measurement
//  0xA4    0x00000002                          eid_update_display() is updating the display using eid_update_global()
//  0xA4    0x00000003                          eid_update_display() is updating the display using eid_update_local()
//  0xA5    eid_updated_at_low_temp             1 indicates that the display has been updated at a low temperature
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
//#define DEVEL
#ifdef DEVEL
    #define DEVEL_SHORT_REFRESH
    //#define DEVEL_ENABLE_XO_PAD
#endif

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED     0
#define FLAG_INITIALIZED    1
#define FLAG_XO_INITIALIZED 2
#define FLAG_WD_ENABLED     3
#define FLAG_SLEEP_BYPASS   4   // Sleep has been bypassed before the current active session. This implies that xo_val_curr is inaccurate.
#define FLAG_SLEEP_BYPASSED 5   // Sleep has been bypassed before the previous active session. This implies that xo_val_prev is inaccurate.
#define FLAG_BOOTUP_DONE    6

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
#define SLEEP_BYPASSED      (WAKEUP_BY_MBUS && get_bit(wakeup_source, 7))

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
#define CMD_HRESET      0x07
#define CMD_DISPLAY     0x10
#define CMD_RESET       0x1E
#define CMD_INVALID     0xFF

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

//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************

//-------------------------------------------------------------------------------------------
// System Configuration Variables in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in update_system_configs() function.
//-------------------------------------------------------------------------------------------

//--- VBAT and ADC
//
//      //----------------------------------------------------
//      //  ADC Temperature Region Definition
//      //----------------------------------------------------
//      //  Z = Temperature Measurement (raw code)
//      //
//      //  Temp Measurement        Offset added to ADC threshold values
//      //----------------------------------------------------
//      //  ADC_T1 < Z              ADC_OFFSET_R1 
//      //  ADC_T2 < Z <= ADC_T1    ADC_OFFSET_R2
//      //  ADC_T3 < Z <= ADC_T2    ADC_OFFSET_R3
//      //  ADC_T4 < Z <= ADC_T3    ADC_OFFSET_R4
//      //           Z <= ADC_T4    ADC_OFFSET_R5
//      //----------------------------------------------------
//

volatile uint32_t eeprom_adc_t1;                    // Temperature Threshold (Raw Code) for ADC. See the table above.
volatile uint32_t eeprom_adc_t2;                    // Temperature Threshold (Raw Code) for ADC. See the table above.
volatile uint32_t eeprom_adc_t3;                    // Temperature Threshold (Raw Code) for ADC. See the table above.
volatile uint32_t eeprom_adc_t4;                    // Temperature Threshold (Raw Code) for ADC. See the table above.
volatile uint32_t eeprom_adc_offset_r1;             // ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_offset_r2;             // ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_offset_r3;             // ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_offset_r4;             // ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_offset_r5;             // ADC offset. 2's complement. See the table above.
volatile uint32_t eeprom_adc_low_vbat;              // ADC code threshold to turn on the Low VBAT indicator.
volatile uint32_t eeprom_adc_crit_vbat;             // ADC code threshold to trigger the EID crash handler.

//--- EID Configurations
volatile uint32_t eeprom_eid_high_temp_threshold;   // Temperature Threshold (Raw Code) to determin HIGH and MID temperature for EID
volatile uint32_t eeprom_eid_low_temp_threshold;    // Temperature Threshold (Raw Code) to determin MID and LOW temperature for EID
volatile uint32_t eeprom_eid_duration_high;         // EID duration (ECTR_PULSE_WIDTH) for High Temperature
volatile uint32_t eeprom_eid_duration_mid;          // EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
volatile uint32_t eeprom_eid_duration_low;          // EID duration (ECTR_PULSE_WIDTH) for Low Temperature
volatile uint32_t eeprom_eid_rfrsh_int_hr_high;     // EID Refresh interval for High Temperature (unit: hr). '0' means 'do not refresh'.
volatile uint32_t eeprom_eid_rfrsh_int_hr_mid;      // EID Refresh interval for Mid Temperature (unit: hr). '0' means 'do not refresh'.
volatile uint32_t eeprom_eid_rfrsh_int_hr_low;      // EID Refresh interval for Low Temperature (unit: hr). '0' meas 'do not refresh'.

//--- Other Configurations
volatile uint32_t eeprom_eid_update_type;           // Display update type to be used in each temperature zone
                                                    // [2] HIGH Temperature - 0: Global Update, 1: Local Update
                                                    // [1] MID Temperature  - 0: Global Update, 1: Local Update
                                                    // [0] LOW Temperature  - 0: Global Update, 1: Local Update
volatile uint32_t eeprom_pmu_num_cons_meas;         // Number of consecutive measurements required to change SAR ratio or trigger Low VBAT warning/Crash Handler.

//--- Temperature Calibration Coefficients
volatile uint32_t eeprom_temp_calib_a;              // Temperature Calibration Coefficient a
volatile uint32_t eeprom_temp_calib_b;              // Temperature Calibration Coefficient b

//-------------------------------------------------------------------------------------------
// User Configuration Variables in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in update_user_configs() function.
//-------------------------------------------------------------------------------------------

volatile uint32_t eeprom_high_temp_threshold;       // Threshold for High Temperature
volatile uint32_t eeprom_low_temp_threshold;        // Threshold for Low Temperature
volatile uint32_t eeprom_temp_meas_interval;        // Period of Temperature Measurement
volatile uint32_t eeprom_temp_meas_start_delay;     // Start Delay before starting temperature measurement


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
volatile uint32_t highest_temp;                     // Highest Temperature observed
volatile uint32_t lowest_temp;                      // Lowest Temperature observed
volatile uint32_t high_sample_id_status;            // Status for updating high_sample_id_*
volatile uint32_t low_sample_id_status;             // Status for updating low_sample_id_*
                                                    // [   31] If 1, it indicates that the current excursion is still ongoing.
                                                    //         If 0, it indicates that the streak of the previous excursion has ended.
                                                    // [27:24] Last updated EEPROM Byte Address
                                                    // [23:17] Number of consecutive samples
                                                    // [16: 0] Sample ID
volatile uint32_t high_sample_id_0;                 // 1st sample ID that violates the high threshold (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t high_sample_id_1;                 // 2nd sample ID that violates the high threshold (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t high_sample_id_2;                 // 3rd sample ID that violates the high threshold (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t low_sample_id_0;                  // 1st sample ID that violates the low threshold  (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t low_sample_id_1;                  // 2nd sample ID that violates the low threshold  (valid only if bit[23]=1; bit[22:0] is the sample ID.)
volatile uint32_t low_sample_id_2;                  // 3rd sample ID that violates the low threshold  (valid only if bit[23]=1; bit[22:0] is the sample ID.)


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
volatile uint32_t snt_calib_restart;                // If 1, it skips the SNT calibration and uses the previous snt_freq to calculate the next snt_threshold.
volatile uint32_t calib_status;                     // Calibration Status
                                                    // [2]: 1 if the SNT or XO counter has an incorrect value (>6.25% (=1/16) error than expected)
                                                    // [1]: 1 if XO is unstable (i.e., asynchronous read glitches do not disappear quickly)
                                                    // [0]: 1 if there has been a XO timeout (i.e., XO has stopped)
volatile uint32_t system_status_at_stop;            // System Status at Stop
                                                    // [23:17] Last Chunk ID
                                                    // [16: 0] Seconds Since the Last Sample

//-------------------------------------------------------------------------------------------
// Data Compression
//-------------------------------------------------------------------------------------------
#define COMP_SECTION_SIZE       128
#define COMP_UNCOMP_NUM_BITS    11
#define COMP_OFFSET_K           (273-80)
volatile uint32_t comp_sample_id;
volatile uint32_t comp_prev_sample;
volatile uint32_t comp_bit_pos;

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
volatile uint32_t eid_update_type;          // EID update type. 0: Global Update, 1: Local Update
volatile uint32_t eid_duration;             // EID active CP duration 
volatile uint32_t eid_disp_before_user_mod; // [ 31] Display has been modified by user using DISPLAY command
                                            // [6:0] Display pattern before being modified by DISPLAY command. Valid only if bit[31]=1.
volatile uint32_t eid_updated_at_low_temp;  // Becomes 1 when the display gets updated at a temperature lower than eeprom_eid_low_temp_threshold.

//--- Write Buffer
volatile uint32_t num_buffer_items;         // Number of items in the buffer
volatile uint32_t buffer_next_ptr;          // Pointer where the next item will be inserted
volatile uint32_t buffer_first_ptr;         // Pointer of the first item to be read
volatile uint32_t buffer_overrun;           // Buffer Overrun Status
volatile uint32_t buffer_missing_sample_id; // ID of the missing sample in case of the buffer overrun. It keeps the very first missing smaple ID only.

//--- PMU ADC and SAR Ratio
volatile uint32_t pmu_adc_vbat_val;         // Latest PMU ADC Reading
volatile uint32_t pmu_sar_ratio;            // Suggested new SAR Ratio


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
static void calibrate_snt_timer(uint32_t restart);
static uint32_t calc_system_status_at_stop(void);

//--- E-Ink Display
static uint32_t eid_get_curr_seg(void);
static void eid_update_configs(void);
static void eid_update_display(uint32_t seg);

//--- NFC
static uint32_t nfc_set_ack(uint32_t ack);
static uint32_t nfc_set_ack_with_data(uint32_t data);
static uint32_t nfc_check_cmd(void);

//--- EEPROM Variables
static void update_system_configs(uint32_t use_default);
static void update_user_configs(void);

//--- Write Buffer
static void buffer_init(void);
static uint32_t buffer_inc_ptr(uint32_t ptr);
static uint32_t buffer_add_item(uint32_t item);
static uint32_t buffer_get_item(void);

//--- Data Compression
uint32_t tcomp_encode (uint32_t value);
uint32_t reverse_bits (uint32_t bit_stream, uint32_t num_bits);

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
    *GOC_DATA_IRQ = 0;

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
        uint32_t dist_max = (snt_freq<<18);               // 262144s (= 4369min = 73hrs); Also see calc_system_status_at_stop()
        uint32_t snt_val = snt_read_wup_timer();
        uint32_t dist;

        if (snt_threshold >= snt_val) { dist = snt_threshold - snt_val;                    } 
        else                          { dist = snt_threshold + (0xFFFFFFFF - snt_val) + 1; }

        if ((dist < dist_min) || (dist > dist_max)) {
            set_flag(FLAG_SLEEP_BYPASS, 1);
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

        // Initialize the EEPROM variables
        update_system_configs(/*use_default*/1);

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
	    eid_init(/*ring*/0, /*te_div*/3, /*fd_div*/3, /*seg_div*/3);

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
        // Turn off NFC
        nfc_power_off();
        // Update the System State
        set_system_state(/*msb*/7, /*lsb*/7, /*val*/0x1);
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
            eid_update_type          = 0;       // 0: Global Update, 1: Local Update
            eid_duration             = 100;
            eid_disp_before_user_mod = 0;
            eid_updated_at_low_temp  = 0;
            snt_temp_valid           = 0;
            system_status_at_stop    = 0;

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

            // Update the user parameters
            update_user_configs();

            // Start Delay
            wakeup_interval = eeprom_temp_meas_start_delay;

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
            eid_update_display(/*seg*/DISP_CHECK);
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
        wakeup_interval = eeprom_temp_meas_interval;

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
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, /*data*/0, /*nb*/2);
    num_calib_xo_fails      = 0;
    num_calib_max_err_fails = 0;

    // Following variables are not set/reset here
    //-----------------------------------------------------------
    // EEPROM_ADDR_SYSTEM_STATE (system_state)
    // EEPROM_ADDR_DISPLAY
    // EEPROM_ADDR_LAST_ADC
    // EEPROM_ADDR_LAST_SAR

    // Reset Measurement Snapshot
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HIGHEST_TEMP,  /*data*/0, /*nb*/4);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_SAMPLE_0, /*data*/0, /*nb*/3);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_SAMPLE_1, /*data*/0, /*nb*/3);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_SAMPLE_2, /*data*/0, /*nb*/3);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_LOW_SAMPLE_0,  /*data*/0, /*nb*/3);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_LOW_SAMPLE_1,  /*data*/0, /*nb*/3);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_LOW_SAMPLE_2,  /*data*/0, /*nb*/3);
    highest_temp     = 0;
    lowest_temp      = 0xFFFFFFFF;
    high_sample_id_status = (EEPROM_ADDR_HIGH_SAMPLE_0 - 3) << 24;
    low_sample_id_status  = (EEPROM_ADDR_LOW_SAMPLE_0  - 3) << 24;
    high_sample_id_0      = 0;
    high_sample_id_1      = 0;
    high_sample_id_2      = 0;
    low_sample_id_0       = 0;
    low_sample_id_1       = 0;
    low_sample_id_2       = 0;

    // Need to write 1 in the last 17 bits (3 bytes)
    // This is to let the app know where the end is, when the compressed data is stored.
    nfc_i2c_byte_write(/*e2*/0, /*addr*/(EEPROM_NUM_BYTES-3),  /*data*/0xFFFFFF, /*nb*/3);
}

static void set_system_state(uint32_t msb, uint32_t lsb, uint32_t val) {
    system_state = set_bits(/*original_var*/system_state, /*msb_idx*/msb, /*lsb_idx*/lsb, /*value*/val);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SYSTEM_STATE, /*data*/system_state, /*nb*/1);
}

static uint32_t snt_get_temp_raw (void) {

    // Enable REG IRQ
    *NVIC_ISER = (0x1 << (SNT_TARGET_REG_IDX+8));

    // Turn on SNT LDO
    snt_ldo_power_on();

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
                            /*   a  */ eeprom_temp_calib_a, 
                            /*   b  */ eeprom_temp_calib_b, 
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
    snt_ldo_power_off();

    #ifdef DEVEL
        mbus_write_message32(0x90|update_eeprom, (snt_temp_raw<<16)|snt_temp_val);
    #endif

    // Determine ADC Offset, ADC Low VBAT threshold, and ADC Critical VBAT threshold
    uint32_t adc_offset;
    if      (snt_temp_val > eeprom_adc_t1) { adc_offset = eeprom_adc_offset_r1; }
    else if (snt_temp_val > eeprom_adc_t2) { adc_offset = eeprom_adc_offset_r2; }
    else if (snt_temp_val > eeprom_adc_t3) { adc_offset = eeprom_adc_offset_r3; }
    else if (snt_temp_val > eeprom_adc_t4) { adc_offset = eeprom_adc_offset_r4; }
    else                                   { adc_offset = eeprom_adc_offset_r5; }

    // VBAT Measurement and SAR_RATIO Adjustment
    pmu_adc_vbat_val = pmu_read_adc();
    pmu_sar_ratio    = pmu_calc_new_sar_ratio(  /*adc_val*/         pmu_adc_vbat_val, 
                                                /*offset*/          adc_offset, 
                                                /*num_cons_meas*/   eeprom_pmu_num_cons_meas
                                            );

    uint32_t low_vbat  = pmu_check_low_vbat (   /*adc_val*/         pmu_adc_vbat_val, 
                                                /*offset*/          adc_offset, 
                                                /*threshold*/       eeprom_adc_low_vbat,  
                                                /*num_cons_meas*/   eeprom_pmu_num_cons_meas
                                            );

    uint32_t crit_vbat = pmu_check_crit_vbat(   /*adc_val*/         pmu_adc_vbat_val, 
                                                /*offset*/          adc_offset, 
                                                /*threshold*/       eeprom_adc_crit_vbat, 
                                                /*num_cons_meas*/   eeprom_pmu_num_cons_meas
                                            );

    #ifdef DEVEL
        mbus_write_message32(0x92, temp_sample_count);
        mbus_write_message32(0x93, (crit_vbat<<28)|(low_vbat<<24)|(adc_offset<<16)|(pmu_sar_ratio<<8)|(pmu_adc_vbat_val));
    #endif

    // Change the SAR ratio
    if (pmu_sar_ratio != pmu_get_sar_ratio()) {
        #ifdef DEVEL
            mbus_write_message32(0x94, (pmu_get_sar_ratio()<<8)|pmu_sar_ratio);
        #endif
        pmu_set_sar_ratio(pmu_sar_ratio);
    }

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
        if (snt_temp_val > eeprom_high_temp_threshold) {
            new_seg = new_seg & ~DISP_CHECK;    // Remove Check
            new_seg = new_seg | DISP_CROSS;     // Add Cross
            new_seg = new_seg | DISP_PLUS;      // Add Plus
        }
        //--- Low Temperature
        else if (snt_temp_val < eeprom_low_temp_threshold) {
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
                /*addr*/ EEPROM_ADDR_LAST_ADC, 
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
                if (eeprom_sample_count<4032) {
                    nfc_i2c_byte_write( /*e2*/   0, 
                                        /*addr*/ byte_addr, 
                                        /*data*/ buf_temp_val,
                                        /*nb*/   2);
                }

                // Update Valid Sample Bit
                if (eeprom_sample_count==0) {
                    set_system_state(/*msb*/5, /*lsb*/5, /*val*/0x1);
                }

                eeprom_sample_count++;

            }
            // STORE THE CONVERTED & COMPRESSED DATA
            else {

                // Store the Sample Count
                nfc_i2c_byte_write(/*e2*/0, 
                    /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, 
                    /*data*/ (eeprom_sample_count+1),
                    /* nb */ 4
                    );

                // Update Valid Sample Bit
                if (eeprom_sample_count==0) {
                    set_system_state(/*msb*/5, /*lsb*/5, /*val*/0x1);
                }

                eeprom_sample_count++;

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

                // MEMORY IS AVAILABLE
                if (bit_pos_end < 64512) { // NOTE: 64512 = (8192 - 128) * 8

                    uint32_t byte_addr  = (bit_pos_start>>3) + EEPROM_ADDR_DATA_RESET_VALUE;
                    uint32_t bit_offset = bit_pos_start&0x7;
                    uint32_t remaining_bits = num_bits;
                    uint32_t nb = 0;
                    uint32_t wdata = nfc_i2c_byte_read(/*e2*/0, /*addr*/byte_addr, /*nb*/1);
                    uint32_t num_written_bits = 0;

                    //--- Initialize wdata 
                    wdata = set_bits(wdata, 31, bit_offset, 0xFFFFFFFF);

                    #ifdef DEVEL
                        mbus_write_message32(0x98, (bit_offset<<24)|byte_addr);
                    #endif


                    while (remaining_bits>0) {

                        if (remaining_bits <= (8 - bit_offset)) {
                            num_written_bits = remaining_bits;
                        }
                        else {
                            num_written_bits = 8 - bit_offset;
                        }

                        wdata = set_bits(wdata, 
                                        /*msb*/(nb<<3)+bit_offset+num_written_bits-1 , 
                                        /*lsb*/(nb<<3)+bit_offset , 
                                        /*value*/get_bits(reverse_code, 
                                                            /*msb*/(num_written_bits-1), 
                                                            /*lsb*/0
                                                        )
                                        );

                        reverse_code >>= num_written_bits;
                        remaining_bits -= num_written_bits;
                        bit_offset = (bit_offset + num_written_bits)&0x7;
                        nb++;
                    }

                    nfc_i2c_byte_write( /*e2*/   0, 
                                        /*addr*/ byte_addr, 
                                        /*data*/ wdata,
                                        /*nb*/   nb);

                    comp_sample_id++;
                    if (comp_sample_id==COMP_SECTION_SIZE) comp_sample_id = 0;
                    comp_bit_pos = bit_pos_end + 1;


                    // Update Measurement Snapshot
                    if (buf_temp_val > highest_temp) {
                        highest_temp = buf_temp_val;
                        nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_HIGHEST_TEMP, /*data*/ highest_temp, /*nb*/ 2);
                    }
                    if (buf_temp_val < lowest_temp) {
                        lowest_temp = buf_temp_val;
                        nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_LOWEST_TEMP, /*data*/ lowest_temp, /*nb*/ 2);
                    }

                    //--- Update the streaks
                    uint32_t curr_act, curr_addr, curr_num;
                    if (buf_temp_val > eeprom_high_temp_threshold) {
                        // End low_temp streak, if any.
                        low_sample_id_status &= 0x7FFFFFFF;
                        // Get the current status
                        curr_act  = get_bit (high_sample_id_status, 31);
                        curr_addr = get_bits(high_sample_id_status, 30, 24);
                        curr_num  = get_bits(high_sample_id_status, 23, 17);
                        // If there the current streak is ongoing
                        if (curr_act) {
                            if (curr_num != 0x3F) {
                                curr_num++;
                                high_sample_id_status = set_bits(high_sample_id_status, 23, 17, curr_num);
                                nfc_i2c_byte_write(/*e2*/0, /*addr*/ curr_addr, /*data*/ high_sample_id_status, /*nb*/ 3);
                            }
                        }
                        // If this is a start of a new streak
                        else {
                            if (curr_addr < EEPROM_ADDR_HIGH_SAMPLE_2) {
                                curr_addr += 3;
                                high_sample_id_status = (1 << 31) | (curr_addr << 24) | (1 << 17) | (eeprom_sample_count - 1);
                                nfc_i2c_byte_write(/*e2*/0, /*addr*/ curr_addr, /*data*/ high_sample_id_status, /*nb*/ 3);
                            }
                        }
                    }
                    else if (buf_temp_val < eeprom_low_temp_threshold) {
                        // End high_temp streak, if any.
                        high_sample_id_status &= 0x7FFFFFFF;
                        // Get the current status
                        curr_act  = get_bit (low_sample_id_status, 31);
                        curr_addr = get_bits(low_sample_id_status, 30, 24);
                        curr_num  = get_bits(low_sample_id_status, 23, 17);
                        // If there the current streak is ongoing
                        if (curr_act) {
                            if (curr_num != 0x3F) {
                                curr_num++;
                                low_sample_id_status = set_bits(low_sample_id_status, 23, 17, curr_num);
                                nfc_i2c_byte_write(/*e2*/0, /*addr*/ curr_addr, /*data*/ low_sample_id_status, /*nb*/ 3);
                            }
                        }
                        // If this is a start of a new streak
                        else {
                            if (curr_addr < EEPROM_ADDR_LOW_SAMPLE_2) {
                                curr_addr += 3;
                                low_sample_id_status = (1 << 31) | (curr_addr << 24) | (1 << 17) | (eeprom_sample_count - 1);
                                nfc_i2c_byte_write(/*e2*/0, /*addr*/ curr_addr, /*data*/ low_sample_id_status, /*nb*/ 3);
                            }
                        }
                    }
                    else {
                        high_sample_id_status &= 0x7FFFFFFF;
                        low_sample_id_status  &= 0x7FFFFFFF;
                    }

                // MEMORY IS FULL
                }
                else { // Roll-over disabled
                    comp_bit_pos = 0xFFFFFFFF;
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
           eid_update_display(/*seg*/DISP_CHECK);
           set_flag(FLAG_BOOTUP_DONE, 1);
       }
       else if (// Display has been updated at a low temperature
                (eid_updated_at_low_temp && (snt_temp_val > eeprom_eid_low_temp_threshold))
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
        while(1);
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
            calib_status |= 0x2;   // Make calib_status[1]=1 (XO unstable)
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

static void calibrate_snt_timer(uint32_t restart) {

    uint32_t temp_xo_val;
    uint32_t temp_snt_freq;
    uint32_t fail = 0;
    uint32_t a = (0x1 << 28);   // Default: 1 (this is the ideal case; i.e., no error)

    // Committ xo_val_curr_tmp
    xo_val_curr = xo_val_curr_tmp;

    // XO malfunction during sleep 
    // This filters out the 'xo_val_prev => xo_val_curr < temp_xo_val' case. (I know it sounds weird, but it has happend!)
    if (xo_val_curr <= xo_val_prev) {
        calib_status |= (0x1 << 0);
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
                calib_status |= 0x1;   // Make calib_status[0]=1 (XO stopped)
                #ifdef DEVEL
                    mbus_write_message32(0x83, 0x2);
                #endif
            }
            else {
                if (restart||SLEEP_BYPASSED||get_flag(FLAG_SLEEP_BYPASSED)) {
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
                        calib_status |= 0x4;   // Make calib_status[2]=1 (excessive deviation)
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
        if (get_bit(calib_status, 2)) {
            num_calib_max_err_fails++;
        }
        if (get_bit(calib_status, 1)||get_bit(calib_status, 0)) {
            num_calib_xo_fails++;
        }
        nfc_i2c_byte_write(/*e2*/0, 
                            /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, 
                            /*data*/((num_calib_max_err_fails&0xFF)<<8)|(num_calib_xo_fails&0xFF), 
                            /*nb*/2);

        // Restart the XO
        restart_xo(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

        // Calculate the next threshold
        if (restart) {
            snt_threshold = snt_read_wup_timer() + snt_duration;
        }
        else {
            snt_threshold += snt_duration;
        }
    }
    // RESTART
    else if (restart) {
        #ifdef DEVEL
            mbus_write_message32(0x83, 0x8);
        #endif

        // Update the next duration
        snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq);

        // Calculate the next threshold
        snt_threshold = snt_read_wup_timer() + snt_duration;

        // Update xo_val_curr
        xo_val_curr = get_xo_cnt();

        #ifdef DEVEL
            mbus_write_message32(0x81, xo_val_curr);
        #endif
    }
    // SLEEP_BYPASSED
    else if (SLEEP_BYPASSED||get_flag(FLAG_SLEEP_BYPASSED)) {
        #ifdef DEVEL
            if (SLEEP_BYPASSED) mbus_write_message32(0x83, 0xA);
            else                mbus_write_message32(0x83, 0xB);
        #endif

        // Update the next duration
        snt_duration = mult(/*num_a*/wakeup_interval_sec, /*num_b*/snt_freq);

        // Calculate the next threshold
        snt_threshold += snt_duration;

        if (SLEEP_BYPASSED) 
            set_flag(FLAG_SLEEP_BYPASSED, 1);
        else 
            set_flag(FLAG_SLEEP_BYPASSED, 0);
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

static uint32_t calc_system_status_at_stop(void) {

    uint32_t last_chunk_id = (((comp_bit_pos-1)>>3)>>8);

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
            return (last_chunk_id<<17)|0;
        }
    }

    // For CASE I and CASE II-A, divide snt_delta by snt_freq to get SSLS
    uint32_t seconds_since_last_sample = div(/*numer*/snt_delta, /*denom*/snt_freq, /*n*/0)&0x1FFFF;

    // Return the result
    return (last_chunk_id<<17)|seconds_since_last_sample;
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
    if (snt_temp_val > eeprom_eid_high_temp_threshold) {
        new_val = eeprom_eid_rfrsh_int_hr_high;
        eid_duration = eeprom_eid_duration_high;
        eid_update_type = get_bit(eeprom_eid_update_type, 2);
    }
    else if (snt_temp_val > eeprom_eid_low_temp_threshold) { 
        new_val = eeprom_eid_rfrsh_int_hr_mid;
        eid_duration = eeprom_eid_duration_mid;
        eid_update_type = get_bit(eeprom_eid_update_type, 1);
    }
    else {
        new_val = eeprom_eid_rfrsh_int_hr_low;
        eid_duration = eeprom_eid_duration_low;
        eid_update_type = get_bit(eeprom_eid_update_type, 0);
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
        mbus_write_message32(0xA3, eid_update_type);
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
    if (eid_update_type==0) {
        #ifdef DEVEL
            mbus_write_message32(0xA4, 0x2);
        #endif
        eid_update_global(seg);
    }
    else {
        #ifdef DEVEL
            mbus_write_message32(0xA4, 0x3);
        #endif
        eid_update_local(seg);
    }

    // Reset the refresh counter
    disp_min_since_refresh = 0;

    // Update the flag if it is a low temperature
    if (snt_temp_val > eeprom_eid_low_temp_threshold)
        eid_updated_at_low_temp = 0;
    else
        eid_updated_at_low_temp = 1;

    #ifdef DEVEL
        mbus_write_message32(0xA5, eid_updated_at_low_temp);
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


//-------------------------------------------------------------------
// NFC
//-------------------------------------------------------------------

uint32_t nfc_set_ack(uint32_t ack) {
    uint32_t data;
    // ACK or ERR
    if (ack) data = (0x1<<6)|cmd;
    else     data = (0x1<<5)|cmd;

    uint32_t num_try=10;
    while (num_try!=0) {
        nfc_i2c_start();
        if (nfc_i2c_byte_imm(0xA6)) {
            nfc_i2c_byte(EEPROM_ADDR_CMD >> 8);
            nfc_i2c_byte(EEPROM_ADDR_CMD);
            nfc_i2c_byte(data);
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
    data = (data<<8)|(0x1<<6)|cmd;
    uint32_t num_try=10;
    while (num_try!=0) {
        nfc_i2c_start();
        if (nfc_i2c_byte_imm(0xA6)) {
            nfc_i2c_byte(EEPROM_ADDR_CMD >> 8);
            nfc_i2c_byte(EEPROM_ADDR_CMD);
            nfc_i2c_byte((data>> 0)&0xFF);
            nfc_i2c_byte((data>> 8)&0xFF);
            nfc_i2c_byte((data>>16)&0xFF);
            nfc_i2c_byte((data>>24)&0xFF);
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

    // Return State
    uint32_t target = 0;

    //---------------------------------------------------------------
    // Read the command from EEPROM
    //---------------------------------------------------------------
    uint32_t cmd_raw;
    uint32_t num_try=10;
    while (num_try!=0) {
        nfc_i2c_start();
        if (nfc_i2c_byte_imm(0xA6)) {
            nfc_i2c_byte(EEPROM_ADDR_CMD >> 8);
            nfc_i2c_byte(EEPROM_ADDR_CMD);
            nfc_i2c_start();
            nfc_i2c_byte(0xA7);
            cmd_raw  =  nfc_i2c_rd(1);
            cmd_raw |= (nfc_i2c_rd(0) << 8);
            nfc_i2c_stop();
            break;
        }
        nfc_i2c_stop();
        num_try--;
    }

    //--- Command Read Fail: Return with 0xDEAD
    if (num_try==0) return 0xDEAD;

    //--- Invalid Handshaking (REQ=0 or ACK=1 or ERR=1)
    if ((cmd_raw&0xE0)!=0x80) return 0xF00D;

    //---------------------------------------------------------------
    // Normal Handshaking (REQ=1 AND ACK=0 AND ERR=0)
    //---------------------------------------------------------------
    // REQ=cmd_raw[7], ACK=cmd_raw[6], ERR=cmd_raw[5], CMD=cmd_raw[4:0]

    cmd       = get_bits(cmd_raw, 4,  0);
    cmd_param = get_bits(cmd_raw, 15, 8);

    #ifdef DEVEL
        mbus_write_message32(0xB3, (cmd_param<<16)|cmd);
    #endif

    //-----------------------------------------------------------
    // CMD: RESET
    //-----------------------------------------------------------
    if (cmd == CMD_RESET) {
        target = XT1_RESET;
        nfc_set_ack(/*ack*/ACK);
    }
    //-----------------------------------------------------------
    // CMD: START
    //-----------------------------------------------------------
    else if (cmd == CMD_START) {
        target = XT1_PEND;
        sample_type = get_bit(cmd_param, 1);
        nfc_set_ack(/*ack*/ACK);
    }
    //-----------------------------------------------------------
    // CMD: STOP
    //-----------------------------------------------------------
    else if (cmd == CMD_STOP) {
        // XT1 is running
        if ((xt1_state==XT1_PEND) || (xt1_state==XT1_ACTIVE)) {
            target = XT1_IDLE;
            nfc_set_ack(/*ack*/ACK);
            system_status_at_stop = calc_system_status_at_stop();
        }
        // XT1 is NOT running
        else {
            nfc_set_ack(/*ack*/ERR);
        }
    }
    //-----------------------------------------------------------
    // CMD: GETSEC
    //-----------------------------------------------------------
    else if (cmd == CMD_GETSEC) {
        uint32_t data;

        // Unit is currently running
        if ((xt1_state==XT1_PEND) || (xt1_state==XT1_ACTIVE))
            data = calc_system_status_at_stop();
        // Unit has been stopped
        else 
            data = system_status_at_stop;

        nfc_set_ack_with_data(/*data*/data);
    }
    //-----------------------------------------------------------
    // CMD: NOP, HARD_RESET, DISPLAY
    //-----------------------------------------------------------
    else if (  (cmd == CMD_NOP)
            || (cmd == CMD_HRESET)
            || (cmd == CMD_DISPLAY)) { 
        nfc_set_ack(/*ack*/ACK); 
    } 
    //-----------------------------------------------------------
    // CMD: Invalid
    //-----------------------------------------------------------
    else { 
        nfc_set_ack(/*ack*/ERR); 
    }

    // Normal return
    return target;

}


//-------------------------------------------------------------------
// EEPROM Variables
//-------------------------------------------------------------------

static void update_system_configs(uint32_t use_default) {

#ifdef DEVEL
    use_default = 1;
#endif

    // Use Default values
    if (use_default) {
        #ifdef DEVEL
            mbus_write_message32(0x73, 0x1);
        #endif

        //--- VBAT and ADC
        eeprom_adc_t1           = 0;    // (    0 ) Temperature Threshold "10x(T+80)" for ADC.
        eeprom_adc_t2           = 0;    // (    0 ) Temperature Threshold "10x(T+80)" for ADC.
        eeprom_adc_t3           = 0;    // (    0 ) Temperature Threshold "10x(T+80)" for ADC.
        eeprom_adc_t4           = 0;    // (    0 ) Temperature Threshold "10x(T+80)" for ADC.
        eeprom_adc_offset_r1    = 0;    // (    0 ) ADC offset to be used in Region 1. 2's complement.
        eeprom_adc_offset_r2    = 0;    // (    0 ) ADC offset to be used in Region 2. 2's complement.
        eeprom_adc_offset_r3    = 0;    // (    0 ) ADC offset to be used in Region 3. 2's complement.
        eeprom_adc_offset_r4    = 0;    // (    0 ) ADC offset to be used in Region 4. 2's complement.
        eeprom_adc_offset_r5    = 0;    // (    0 ) ADC offset to be used in Region 5. 2's complement.
        eeprom_adc_low_vbat     = 105;  // (  105 ) ADC code threshold to turn on the Low VBAT indicator.
        eeprom_adc_crit_vbat    = 95;   // (   95 ) ADC code threshold to trigger the EID crash handler.

        //--- EID Configurations                    Recommended Value
        eeprom_eid_high_temp_threshold  = 950; // ( 950 (=15C)  ) Temperature Threshold "10x(T+80)" to determine HIGH and MID temperature for EID
        eeprom_eid_low_temp_threshold   = 750; // ( 750 (=-5C)  ) Temperature Threshold "10x(T+80)" to determine MID and LOW temperature for EID
        eeprom_eid_duration_high        = 60 ; // ( 60  (=0.5s) ) EID duration (ECTR_PULSE_WIDTH) for High Temperature
        eeprom_eid_duration_mid         = 500; // ( 500 (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
        eeprom_eid_duration_low         = 500; // ( 500 (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Low Temperature
        eeprom_eid_rfrsh_int_hr_high    = 12 ; // ( 12  (=12hr) ) EID Refresh interval for High Temperature (unit: hr). '0' means 'do not refresh'.
        eeprom_eid_rfrsh_int_hr_mid     = 24 ; // ( 24  (=24hr) ) EID Refresh interval for Mid Temperature (unit: hr). '0' means 'do not refresh'.
        eeprom_eid_rfrsh_int_hr_low     = 0  ; // ( 0   (=none) ) EID Refresh interval for Low Temperature (unit: hr). '0' means 'do not refresh'.

        //--- Other Configurations
        eeprom_eid_update_type          =  1;   // (            1 ) Display update type to be used in each temperature zone.
                                                //                  [2] HIGH Temperature - 0: Global Update, 1: Local Update
                                                //                  [1] MID Temperature  - 0: Global Update, 1: Local Update
                                                //                  [0] LOW Temperature  - 0: Global Update, 1: Local Update
        eeprom_pmu_num_cons_meas        =  5;   // (            5 ) Number of consecutive measurements required to change SAR ratio or trigger Low VBAT warning/Crash Handler.

        //--- Temperature Calibration Coefficients
        //eeprom_temp_calib_a = 0x41FCE21B;   // Temperature Calibration Coefficient a
        //eeprom_temp_calib_b = 0x45A90482;   // Temperature Calibration Coefficient b
        eeprom_temp_calib_a = 0x00007E8B;   // Temperature Calibration Coefficient a (fixed-point, N=10)
        eeprom_temp_calib_b = 0x005479B9;   // Temperature Calibration Coefficient b (fixed-point, N=10)

    }
    // Get the real values from EEPROM
    else {

        #ifdef DEVEL
            mbus_write_message32(0x73, 0x2);
        #endif

        uint32_t temp;

        //--- VBAT and ADC
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

        //--- EID Configurations
        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_HIGH_TEMP_THRESHOLD, /*nb*/4);
        eeprom_eid_high_temp_threshold  = get_bits(temp, 15,  0);
        eeprom_eid_low_temp_threshold   = get_bits(temp, 31, 16);

        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_HIGH, /*nb*/4);
        eeprom_eid_duration_high        = get_bits(temp, 15,  0);
        eeprom_eid_duration_mid         = get_bits(temp, 31, 16);

        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_DURATION_LOW, /*nb*/2);
        eeprom_eid_duration_low         = get_bits(temp, 15,  0);

        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_EID_RFRSH_INT_HR_HIGH, /*nb*/3);
        eeprom_eid_rfrsh_int_hr_high    = get_bits(temp,  7,  0);
        eeprom_eid_rfrsh_int_hr_mid     = get_bits(temp, 15,  8);
        eeprom_eid_rfrsh_int_hr_low     = get_bits(temp, 23, 16);

        //--- Other Configurations
        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_MISC_CONFIG, /*nb*/1);
        eeprom_eid_update_type          = get_bits(temp, 6,  4);
        eeprom_pmu_num_cons_meas        = get_bits(temp, 3,  0);

        //--- Temperature Calibration Coefficients
        eeprom_temp_calib_a = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_A, /*nb*/4);
        eeprom_temp_calib_b = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_CALIB_B, /*nb*/4);
    }

}

static void update_user_configs(void) {
    #ifdef DEVEL
        mbus_write_message32(0x73, 0x3);
    #endif

    uint32_t temp;

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_TEMP_THRESHOLD, /*nb*/4);
    eeprom_high_temp_threshold = get_bits(temp, 15, 0);
    eeprom_low_temp_threshold  = get_bits(temp, 31, 16);

    temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_MEAS_INTERVAL, /*nb*/4);
    eeprom_temp_meas_interval    = get_bits(temp, 15, 0);
    eeprom_temp_meas_start_delay = get_bits(temp, 31, 16);

    #ifdef DEVEL
        mbus_write_message32(0x74, (eeprom_high_temp_threshold<<16)|eeprom_low_temp_threshold);
        mbus_write_message32(0x75, (eeprom_temp_meas_start_delay<<16)|eeprom_temp_meas_interval);
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

//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
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
//void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

//void handler_ext_int_wakeup   (void) { *NVIC_ICPR = (0x1 << IRQ_WAKEUP);     }
//void handler_ext_int_softreset(void) { *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); }
void handler_ext_int_gocep    (void) { 
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);      
    //uint32_t goc_raw = *GOC_DATA_IRQ;
    //*GOC_DATA_IRQ   = 0;
}
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
void handler_ext_int_reg1     (void) { 
    // Used in snt_operation()
    *NVIC_ICPR = (0x1 << IRQ_REG1);       
}
//void handler_ext_int_reg2     (void) { *NVIC_ICPR = (0x1 << IRQ_REG2);       }
//void handler_ext_int_reg3     (void) { *NVIC_ICPR = (0x1 << IRQ_REG3);       }
//void handler_ext_int_reg4     (void) { *NVIC_ICPR = (0x1 << IRQ_REG4);       }
//void handler_ext_int_reg5     (void) { *NVIC_ICPR = (0x1 << IRQ_REG5);       }
//void handler_ext_int_reg6     (void) { *NVIC_ICPR = (0x1 << IRQ_REG6);       }
//void handler_ext_int_reg7     (void) { *NVIC_ICPR = (0x1 << IRQ_REG7);       }
//void handler_ext_int_mbusmem  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);   }
//void handler_ext_int_aes      (void) { *NVIC_ICPR = (0x1 << IRQ_AES);        }
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
    snt_calib_restart = 0;      // By default, it does the normal calibration
    calib_status      = 0;           // Reset Calibration status
    xo_val_prev       = xo_val_curr;
    xo_val_curr_tmp   = get_xo_cnt(); // It returns 0 if XO has not been initialized yet

    // Reset variables
    set_high_power_history(0);
    snt_temp_valid = 0;
    
    // Get the info on who woke up the system, then reset WAKEUP_SOURCE register.
    if (get_flag(FLAG_SLEEP_BYPASS)) {
        wakeup_source = 0x90; // Treat it as if the SNT timer has woken up the system. Set bit[7] as well. See WAKEUP_SOURCE.
        set_flag(FLAG_SLEEP_BYPASS, 0);
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
    // NFC HANDSHAKE
    //-----------------------------------------
    else if (WAKEUP_BY_NFC) {

        uint32_t target = nfc_check_cmd();

        #ifdef DEVEL
            mbus_write_message32(0xB2, target);
        #endif

        // Fail Handling
        if ((target==0xDEAD)     // Command Read Fail
         || (target==0xF00D)     // Invalid Handshaking (REQ=0 or ACK=1 or ERR=1)
        ){
            operation_sleep(/*check_snt*/1);
        }
        // Command Successfully Read
        else {

            // Command: START, STOP, RESET
            if ((cmd==CMD_START)||(cmd==CMD_STOP)||(cmd==CMD_RESET)){
                set_system(/*target*/target);
                snt_calib_restart = 1;   // Skip Calibration
            }
            // Other Commands:
            else {
                // Command: GETSEC
                if (cmd==CMD_GETSEC) {
                    // Nothing to do here.
                }
                // Command: DISPLAY
                else if (cmd==CMD_DISPLAY) {
                    if (!get_bit(eid_disp_before_user_mod, 31))
                        eid_disp_before_user_mod = (0x1<<31)|eid_get_current_display();
                    eid_update_display(/*seg*/cmd_param&0x7F);
                    cmd = CMD_NOP;
                }
                // Command: HARD_RESET
                else if (cmd==CMD_HRESET) {
                    eid_update_display(/*seg*/DISP_PLUS|DISP_MINUS);
                    config_timerwd(/*cnt*/10);
                    while(1);
                }
                // Commands: NOP or INVALID
                else {}

                operation_sleep(/*check_snt*/1);
            }
        }

    }
    //-----------------------------------------
    // GOC/EP FIXME: Do we need this?
    //-----------------------------------------
    //else if (WAKEUP_BY_GOCEP) {}
    //-----------------------------------------
    // Other Wakeup Source - Probably a Glitch
    //-----------------------------------------
    else {
        operation_sleep(/*check_snt*/1);
    }

    // SNT Calibration
    calibrate_snt_timer(/*restart*/snt_calib_restart);
    operation_sleep_snt_timer(/*check_snt*/0);

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
