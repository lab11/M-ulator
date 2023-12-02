//*******************************************************************************************
// XT1 (TMCv1) Debug
// design_tb_0
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jun 24 2021 - First commit 
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
// SNT Temperature Sensor Raw Code
//-------------------------------------------------------------------------------------------
//
//  Based on Seokhyeon's Temperature Calibration
//
//  Temp(C) Raw Code
//  --------------------
//  -20	    348
//  -10	    603
//    0	    1007
//   10	    1625
//   20	    2534
//   30	    3847
//   45	    6848
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
// Total Time Estimation
//-------------------------------------------------------------------------------------------
//
//  TOTAL_TIME (min) = TEMP_MEAS_START_DELAY + [(SAMPLE_COUNT - 1) x TEMP_MEAS_INTERVAL]
//                     + TE_LAST_SAMPLE + (0.5 x TE_LAST_SAMPLE_RES)
//
//  Error is within +/-(0.5 x TE_LAST_SAMPLE_RES) + (up to 1 min)
//
//  The "up to 1 min" is from the NFC START command to the following system wakeup.
//
//  Alse see the Memory Map.
//
//-------------------------------------------------------------------------------------------
// DEBUG Messages
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

#include "TMCv1.h"

//*******************************************************************************************
// DEVEL Mode
//*******************************************************************************************
// Enable 'DEVEL' for the following features:
//      - Use of GOC to do the NFC commands 
//          [FIXME] This must be disabled in the final version.
//                   There is a risk when main() is called inside update_snt_timer_threshold() inside handler_ext_int_gocep().
//                   Since the ISR calls main, the same IRQ may not be called again properly afterwards.
//      - Send debug messages
//      - Use default values rather than grabbing the values from EEPROM
#define DEVEL

//*******************************************************************************************
// HARDWARE/FIRMWARE ID
//*******************************************************************************************

#define HARDWARE_ID    0x01000000  // XT1 Hardware ID
#define FIRMWARE_ID    0x58540001  // XT1 Firmware Version 1

//*******************************************************************************************
// KEYS
//*******************************************************************************************

#define GOC_ACTIVATE_KEY    0x16002351  // Activate the system. Once executed, the system starts the 1-min wakeup/sleep operation.
#define GOC_START_KEY       0xFEEDC0DE  // Start the temperature measurement after updating the settings.
#define GOC_STOP_KEY        0xDEADBEEF  // Stop the ongoing temperature measurement.
#define GOC_RESET_KEY       0x19821229  // Reset the system and put it into 'Activated' state.
#define GOC_ACT_START_KEY   0x16002329  // Activate & Start the System. 
#define GOC_DISPLAY_KEY     0xD1500000  // GOC Display Key; [6:0] used for segment pattern.
#define GOC_IMM_DISPLAY_KEY 0xD1590000  // GOC Display Key (immediate); [6:0] used for segment pattern.

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED     0
#define FLAG_INITIALIZED    1
#define FLAG_ACTIVATED      2
#define FLAG_STARTED        3
#define FLAG_WD_ENABLED     4
#define FLAG_SLEEP_BYPASS   5

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
//#define XO_WAIT_A   2*DLY_1S  // Must be ~1 second delay.
//#define XO_WAIT_B   2*DLY_1S  // Must be ~1 second delay.
#define XO_WAIT_A   1000
#define XO_WAIT_B   1000

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
#define SLEEP_BYPASSED      (WAKEUP_BY_MBUS && get_bit(wakeup_source, 7))

//*******************************************************************************************
// FSM STATES
//*******************************************************************************************

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
volatile uint32_t num_calib_xo_fails;               // Number of Calibration fails (XO timeout)
volatile uint32_t num_calib_max_err_fails;          // Number of Calibration fails (MAX_CHANGE error)
volatile uint32_t system_state;                     // System State (See comment on EEPROM_ADDR_SYSTEM_STATE)

//--- User Commands
volatile uint32_t eeprom_high_temp_threshold;       // Threshold for High Temperature
volatile uint32_t eeprom_low_temp_threshold;        // Threshold for Low Temperature
volatile uint32_t eeprom_temp_meas_interval;        // Period of Temperature Measurement (unit: snt_timer_1min)
volatile uint32_t eeprom_temp_meas_start_delay;     // Start Delay before starting temperature measurement (unit: snt_timer_1min)
volatile uint32_t eeprom_temp_meas_config;          // Misc. Configurations (See memory map)

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
volatile uint32_t eeprom_pmu_num_cons_meas;         // Number of consecutive measurements required to change SAR ratio or trigger Low VBAT warning/Crash Handler.
volatile uint32_t eeprom_timer_calib_interval;      // Period of SNT Timer Calibration. 0 means 'do not calibrate'. (unit:snt_timer_1min).
volatile uint32_t eeprom_te_last_sample_res;        // Resolution of TE_LAST_SAMPLE (unit: minute). See comment on EEPROM_ADDR_TE_LAST_SAMPLE.

//-------------------------------------------------------------------------------------------
// Other Global Variables
//-------------------------------------------------------------------------------------------

//--- General 
volatile uint32_t wakeup_source;            // Wakeup Source. Updated each time the system wakes up. See 'WAKEUP_SOURCE definition'.
volatile uint32_t wakeup_count;             // Wakeup Count. Incrementing each time the system wakes up. It does not increment if the system wakes up to resume the SNT temperature measurement.

//--- SNT & Temperature Measurement
volatile uint32_t tmp_state;                // TMP state
volatile uint32_t snt_running;              // Indicates whether the SNT temp sensor is running.
volatile uint32_t eeprom_addr;              // EEPROM byte address for temperature data storage
volatile uint32_t snt_temp_val;             // Latest temp measurement (raw code)

//--- SNT & Timer
volatile uint32_t xo_timer_threshold;      // SNT Timer Threshold to wake up the system
volatile uint32_t snt_timer_1min;           // SNT Timer tick count corresponding to 1 minute

//--- Display
volatile uint32_t disp_refresh_interval;     // Period of Display Refresh. 0 means 'do not refresh'. (unit: snt_timer_1min)

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
int main(void); // Declaration here needed because update_snt_timer_threshold() may call main().

//--- Initialization/Sleep Functions
static void operation_sleep (void);
static void operation_sleep_snt_timer(uint32_t auto_reset, uint32_t timestamp);
static void operation_init (void);

//--- Application Specific
static void eid_update_with_eeprom(uint32_t seg);
static void set_system_state(uint32_t msb, uint32_t lsb, uint32_t val);
static void snt_operation (void);
static void nfc_set_ack(uint32_t cmd);
static void nfc_set_err(uint32_t cmd);
static void nfc_check_cmd(void);
static void calibrate_snt_timer(void);
static void set_system(uint32_t target, uint32_t reset_cmd, uint32_t reset_start_info);
static void update_system_configs(uint32_t use_default);
static void update_snt_timer_threshold(void);

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

    //if (get_flag(FLAG_ACTIVATED)) {
    //    // Low-Power Active mode: 18~20uA for ~6 seconds
    //    *REG_MBUS_WD = 1680000; // 6s with 280kHz;
    //    halt_cpu();
    //}
    //else {
        // Go to sleep immediately if not yet Activated
        mbus_sleep_all(); 
        while(1);
    //}
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
        //pmu_init();

        ////-------------------------------------------------
        //// SNT Settings
        ////-------------------------------------------------
        //snt_init();

        // Update the flag
        set_flag(FLAG_ENUMERATED, 1);

        //// Turn on the SNT timer 
        //snt_start_timer(/*wait_time*/2*DLY_1S);

        // Go to sleep
        set_wakeup_timer (/*timestamp*/10, /*irq_en*/0x1, /*reset*/0x1); // About 2 seconds
        mbus_sleep_all();
        while(1);
    }
    else if (!get_flag(FLAG_INITIALIZED)) {
        
        // Set the Active floor setting to the minimum (b/c RAT is enabled at this point)
        //pmu_set_active_min();

        // Disable the PRC wakeup timer
        *REG_WUPT_CONFIG = *REG_WUPT_CONFIG & 0xFF3FFFFF; // WUP_ENABLE=0, WUP_WREQ_EN=0

        ////-------------------------------------------------
        //// NFC 
        ////-------------------------------------------------
        //nfc_init();

        //// Set the Hardware/Firmware ID
        //nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_HW_ID, /*data*/HARDWARE_ID, /*nb*/4);
        //nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_FW_ID, /*data*/FIRMWARE_ID, /*nb*/4);

        //// Initialize the EEPROM variables
        //update_system_configs(/*use_default*/1);

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
	    //eid_init(/*ring*/0, /*te_div*/3, /*fd_div*/3, /*seg_div*/3);

        ////-------------------------------------------------
        //// System Reset
        ////-------------------------------------------------
        //// Set system
        //set_system  (   /*target*/          0x0,    /*IDLE*/
        //                /*reset_cmd*/       0x1,
        //                /*reset_start_info*/0x1
        //            );

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);

        ////-------------------------------------------------
        //// Indefinite Sleep
        ////-------------------------------------------------
        //operation_sleep();

        //-------------------------------------------------
        // Start XO Timer
        //-------------------------------------------------

        xo_timer_threshold   = 0;

        // XO Frequency
        *REG_XO_CONF2 =             // Default  // Description
            //-----------------------------------------------------------------------------------------------------------
            ( (0x2 << 13)           // 2'h2     // XO_INJ	            #Adjusts injection period
            | (0x1 << 10)           // 3'h1     // XO_SEL_DLY	        #Adjusts pulse delay
            | (0x1 <<  8)           // 2'h1     // XO_SEL_CLK_SCN	    #Selects division ratio for SCN CLK
            | (0x4 <<  5)           // 3'h1     // XO_SEL_CLK_OUT_DIV   #Selects division ratio for the XO CLK output
            | (0x1 <<  4)           // 1'h1     // XO_SEL_LDO	        #Selects LDO output as an input to SCN
            | (0x0 <<  3)           // 1'h0     // XO_SEL_0P6	        #Selects V0P6 as an input to SCN
            | (0x0 <<  0)           // 3'h0     // XO_I_AMP	            #Adjusts VREF body bias buffer current
            );

        // Start XO clock (High Power Mode)
        xo_start_high_power(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B, /*start_cnt*/0);

        // Configure the XO Counter as needed (See 'Use as a Timer in Active Mode' in Section 18.4. XO Timer)
        set_xo_timer(   /*mode*/      0,
                        /*threshold*/ 0,
                        /*wreq_en*/   0,
                        /*irq_en*/    0
                    );

        // Start the XO counter
        start_xo_cnt();
        //start_xo_cout();
        //delay(100);
        //stop_xo_cout();

    }
}

//-------------------------------------------------------------------
// Application-Specific Functions
//-------------------------------------------------------------------

static void eid_update_with_eeprom(uint32_t seg) {

    // Turn off NFC, if on, to save power
    nfc_power_off();

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
    if(nfc_i2c_get_token(/*num_try*/30)) {
        // EEPROM Update
        nfc_i2c_byte_write(/*e2*/0,
            /*addr*/ EEPROM_ADDR_DISPLAY,
            /*data*/ seg,
            /* nb */ 1
        );
    }

    #ifdef DEVEL
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
    if(nfc_i2c_get_token(/*num_try*/30)) {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SYSTEM_STATE, /*data*/system_state, /*nb*/2);
    }
}

static void snt_operation (void) {

    snt_running = 1;

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

    // Get the measurement
    snt_temp_val = *SNT_TARGET_REG_ADDR;
    temp_sample_count++;

    // Assert temp sensor isolation & turn off temp sensor power
    snt_temp_sensor_power_off();
    snt_ldo_power_off();

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

    // Change the SAR ratio
    if (pmu_sar_ratio != pmu_get_sar_ratio()) 
        pmu_set_sar_ratio(pmu_sar_ratio);

    // Write data into the write buffer
    if(!buffer_add_item((pmu_sar_ratio<<24)|(pmu_adc_vbat_val<<16)|(snt_temp_val&0xFFFF))) {
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
    uint32_t new_seg;

    //--- Add Play
    new_seg = curr_seg | DISP_PLAY;

    //--- Show Plus (if needed)
    if (!get_bit(new_seg, SEG_PLUS) && (snt_temp_val > eeprom_high_temp_threshold)) {
        new_seg = new_seg & ~DISP_CHECK;    // Remove the Check
        new_seg = new_seg | DISP_CROSS;     // Add the Cross
        new_seg = new_seg | DISP_PLUS;      // Add Plus
    }

    //--- Show Minus (if needed)
    if (!get_bit(new_seg, SEG_MINUS) && (snt_temp_val < eeprom_low_temp_threshold)) {
        new_seg = new_seg & ~DISP_CHECK;    // Remove the Check
        new_seg = new_seg | DISP_CROSS;     // Add the Cross
        new_seg = new_seg | DISP_MINUS;     // Add Minus
    }

    //--- Show Low VBATT Indicator (if needed)
    if (!get_bit(new_seg, SEG_LOWBATT) && low_vbat) {
        new_seg = new_seg | DISP_LOWBATT;
    }


    /////////////////////////////////////////////////////////////////
    // Write into EEPROM/Display
    //---------------------------------------------------------------

    if (nfc_i2c_get_token(/*num_try*/1)) {
        // Store the Measured Temp data
        while (num_buffer_items!=0) {

            eeprom_sample_count++;

            // Store the Sample Count
            nfc_i2c_byte_write(/*e2*/0, 
                /*addr*/ EEPROM_ADDR_SAMPLE_COUNT, 
                /*data*/ eeprom_sample_count,
                /* nb */ 4
                );

            // Read the buffer
            uint32_t buf_val = buffer_get_item();

            // Store SAR_RATIO and VBAT ADC Reading
            nfc_i2c_byte_write(/*e2*/0, 
                /*addr*/ EEPROM_ADDR_VBAT_INFO, 
                /*data*/ get_bits(buf_val, 31, 16),
                /* nb */ 2
                );

            // Store the Temperature Data
            if (eeprom_addr != 0) { // eeprom_addr=0 indicates the EEPROM is full -AND- the roll-over is not allowed.
                nfc_i2c_byte_write(/*e2*/0, 
                    /*addr*/ eeprom_addr, 
                    /*data*/ buf_val&0xFFFF,
                    /* nb */ 2
                    );
                eeprom_addr += 2;
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

    }

    // Update the e-Ink Display if needed
    if (new_seg!=curr_seg) eid_update_with_eeprom(new_seg);

    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    // EID Crash Handler
    //---------------------------------------------------------------
    //--- If VBAT is too low, trigger the EID Watchdog (System Crash)
    if (crit_vbat) {

        // Set system
        set_system  (   /*target*/          0x9,    /*Crash*/
                        /*reset_cmd*/       0x0,    /*Not Used*/
                        /*reset_start_info*/0x0     /*Not Used*/
                    );

        // Trigger the Crash Handler
        eid_trigger_crash();
        while(1);
    }
    /////////////////////////////////////////////////////////////////

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

void nfc_set_ack(uint32_t cmd) {
    if (nfc_i2c_get_token(/*num_try*/30)) {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD, /*data*/ (0x1<<6)|(cmd&0x1F), /*nb*/ 1);
        #ifdef DEVEL
            mbus_write_message32(0x86, 0x1);
        #endif
    }
}
void nfc_set_err(uint32_t cmd) {
    if (nfc_i2c_get_token(/*num_try*/30)) {
        nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD, /*data*/ (0x1<<5)|(cmd&0x1F), /*nb*/ 1);
        #ifdef DEVEL
            mbus_write_message32(0x86, 0xDEADBEEF);
        #endif
    }
}
void nfc_check_cmd(void) {

    // Get the token
    if (nfc_i2c_get_token(/*num_try*/1)) {

        // Read the command from EEPROM
        uint32_t cmd_raw = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_CMD, /*nb*/1);

        //---------------------------------------------------------------
        // Normal Handshaking (REQ=1 AND ACK=0 AND ERR=0)
        //---------------------------------------------------------------
        // REQ=cmd_raw[7], ACK=cmd_raw[6], ERR=cmd_raw[5], CMD=cmd_raw[4:0]
        if ((cmd_raw&0xE0)==0x80) {  // If REQ=1, ACK=0, ERR=0

            // Extract the command
            uint32_t cmd = get_bits(cmd_raw, 4, 0);

            #ifdef DEVEL
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

                // Set system
                set_system  (   /*target*/          0x2,    /*STARTED*/
                                /*reset_cmd*/       0x0,
                                /*reset_start_info*/0x0
                            );

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
                    #ifdef DEVEL
                        mbus_write_message32(0x83, 0x00000001);
                    #endif

                    // Set system
                    set_system  (   /*target*/          0x3,    /*STOPPED*/
                                    /*reset_cmd*/       0x0,    /*Not Used*/
                                    /*reset_start_info*/0x0     /*Not Used*/
                                );

                    // ACK
                    nfc_set_ack(cmd);
                }
            }
            //-----------------------------------------------------------
            // CMD: DISPLAY
            //-----------------------------------------------------------
            else if (cmd == 0x10) {

                // Get the parameter
                uint32_t param = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_CMD_PARAM, /*nb*/1);

                #ifdef DEVEL
                    mbus_write_message32(0x83, 0xD159|(param&0x7F));
                #endif

                // Update the display
                eid_update_with_eeprom(param&0x7F);

                // ACK
                nfc_set_ack(cmd);
                
            }
            //-----------------------------------------------------------
            // CMD: RESET
            //-----------------------------------------------------------
            else if (cmd == 0x1E) {
                #ifdef DEVEL
                    mbus_write_message32(0x83, 0x0000000B);
                #endif

                // Set system
                set_system  (   /*target*/          0x1,    /*ACTIVATED*/
                                /*reset_cmd*/       0x0,
                                /*reset_start_info*/0x1
                            );

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
        //else {}

        // Turn off NFC
        nfc_power_off();
    }
}

// SNT Timer Calibration
static void calibrate_snt_timer(void) {

    uint32_t a, b, new_val;

    // Turn off NFC, if on, to save power
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
    xo_start_high_power(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B, /*start_cnt*/0);

    // Configure the XO Counter as needed (See 'Use as a Timer in Active Mode' in Section 18.4. XO Timer)
    set_xo_timer(   /*mode*/      0,
                    /*threshold*/ xo_threshold,
                    /*wreq_en*/   0,
                    /*irq_en*/    1
                );

    // Time-out Check
    set_timeout32_check(45*CPU_CLK_FREQ);

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
    if (!stop_timeout32_check(0x0)) {

        // FAIL TYPE I - XO does not start
        #ifdef DEVEL
            mbus_write_message32(0x87, snt_timer_1min);
            mbus_write_message32(0x87, 0xFFFFFFFF);
            mbus_write_message32(0x88, 0x3);
        #endif

        // ERROR: XO failure
        num_calib_xo_fails++;
        if (nfc_i2c_get_token(/*num_try*/1)) {
            nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, /*data*/num_calib_xo_fails, /*nb*/1);
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
            #ifdef DEVEL
                mbus_write_message32(0x87, snt_timer_1min);
                mbus_write_message32(0x87, new_val);
                mbus_write_message32(0x88, 0x1);
            #endif

            // ERROR: Calibration Max Error
            num_calib_max_err_fails++;
            if (nfc_i2c_get_token(/*num_try*/1)) {
                nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_MAX_ERR_FAILS, /*data*/num_calib_max_err_fails, /*nb*/1);
            }
        }
        else {
            // SUCCESS: Update snt_timer_1min
            #ifdef DEVEL
                mbus_write_message32(0x87, snt_timer_1min);
                mbus_write_message32(0x87, new_val);
                mbus_write_message32(0x88, 0x2);
            #endif

            // Update snt_timer_1min
            snt_timer_1min = new_val;
        }
    }

}

static void set_system(uint32_t target, uint32_t reset_cmd, uint32_t reset_start_info) {
    //------------------------------------------
    // target:  0x0     - Go back to System IDLE
    //          0x1     - Go back to System ACTIVATED
    //          0x2     - Go back to System STARTED
    //          0x3     - Go back to System STOPPED; it ignores 'reset_cmd' and 'reset_start_info'.
    //          0x9     - Prepare for EID crash handler; it ignores 'reset_cmd' and 'reset_start_info'.
    //------------------------------------------

    //------------------------------------------
    // COMMON FOR ALL TARGETS
    //------------------------------------------
    // MBus Halt Setting
    set_halt_until_mbus_tx();

    // Just in case the SNT is running
    if (snt_running) {
        snt_running = 0;
        snt_temp_sensor_reset();
        snt_temp_sensor_power_off();
        snt_ldo_power_off();
    }

    // Reset tmp_state
    tmp_state = TMP_IDLE;

    // EID Crash Handler
    if (target == 0x9) {
        // Turn off NFC
        nfc_power_off();
        // Update the System State
        set_system_state(/*msb*/15, /*lsb*/15, /*val*/0x1);
        return;
    }

    //------------------------------------------
    // FOR TARGETS OTHER THAN 'STOPPED'
    //------------------------------------------
    if (target != 0x3) {
        // Reset the Sample Count
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SAMPLE_COUNT, /*data*/0x0, /*nb*/4);
        temp_sample_count   = 0;
        eeprom_sample_count = 0;

        // Reset NUM_XO/CALIB_MAX_ERR
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_NUM_CALIB_XO_FAILS, /*data*/0x0, /*nb*/2);
        num_calib_xo_fails      = 0;
        num_calib_max_err_fails = 0;

        // Reset the Command
        if (reset_cmd) {
            nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_CMD, /*data*/0x0, /*nb*/1);
        }

        // Reset Start Time and User IDs
        if (reset_start_info) {
            nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_START_TIME,    /*data*/0x0, /*nb*/4);
            nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_START_USER_ID, /*data*/0x0, /*nb*/4);
        }

        // Reset Stop Time and User IDs
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_STOP_TIME,    /*data*/0x0, /*nb*/4);
        nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_STOP_USER_ID, /*data*/0x0, /*nb*/4);

        // Counter for User Parameters
        cnt_temp_meas_start_delay       = 2;
        cnt_temp_meas_interval          = 1;
        cnt_disp_refresh_interval       = 1;
        cnt_timer_calib_interval        = 1;
        cnt_te_last_sample              = 0;
        cnt_te_last_sample_res          = 0;

        // Global Variables
        wakeup_count = 0;

        snt_running  = 0;
        eeprom_addr  = EEPROM_ADDR_DATA_RESET_VALUE;
        snt_temp_val = 2534; // Assume 20C by default

        disp_refresh_interval = 1440;    // 24 hours
        xo_timer_threshold   = 0;

        pmu_adc_vbat_val = 0;   // not necessary
        pmu_sar_ratio = 0;      // not necessary

        // Disable SNT wakeup counter so that it gets reset to 0
        snt_disable_wup_timer();

        // Reset the Write Buffer
        buffer_init();
    }

    //------------------------------------------
    // FOR 'IDLE' TARGET
    //------------------------------------------
    if (target == 0x0) {
        // Set Flags
        set_flag(FLAG_ACTIVATED, 0); 
        set_flag(FLAG_STARTED,   0);
        // Reset the System State
        set_system_state(/*msb*/15, /*lsb*/0, /*val*/0);
        // Display all segments
        eid_update_with_eeprom(DISP_ALL);
    }
    //------------------------------------------
    // FOR 'ACTIVATED' TARGET
    //------------------------------------------
    else if (target == 0x1) {
        // Set Flags
        set_flag(FLAG_ACTIVATED, 1); 
        set_flag(FLAG_STARTED,   0);
        // Reset the System State
        set_system_state(/*msb*/15, /*lsb*/0, /*val*/1);
        // Enable the timer again
        snt_enable_wup_timer(/*auto_reset*/0);
        // Display the Check mark
        eid_update_with_eeprom(DISP_CHECK);
    }
    //------------------------------------------
    // FOR 'STARTED' TARGET
    //------------------------------------------
    else if (target == 0x2) {
        // Set Flags
        set_flag(FLAG_ACTIVATED, 1); 
        set_flag(FLAG_STARTED,   1);
        // Reset the System State
        set_system_state(/*msb*/15, /*lsb*/0, /*val*/2);
        // Enable the timer again
        snt_enable_wup_timer(/*auto_reset*/0);
        // Display the Play mark
        eid_update_with_eeprom(DISP_PLAY);

        // Update the parameters
        uint32_t temp;

        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_HIGH_TEMP_THRESHOLD, /*nb*/4);
        eeprom_high_temp_threshold = get_bits(temp, 15, 0);
        eeprom_low_temp_threshold  = get_bits(temp, 31, 16);

        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_MEAS_INTERVAL, /*nb*/4);
        eeprom_temp_meas_interval    = get_bits(temp, 15, 0);
        eeprom_temp_meas_start_delay = get_bits(temp, 31, 16);

        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TEMP_MEAS_CONFIG, /*nb*/2);
        eeprom_temp_meas_config      = get_bits(temp, 15, 0);

        // Update the System Config Variables
        update_system_configs(/*use_default*/0);
    }
    //------------------------------------------
    // FOR 'STOPPED' TARGET
    //------------------------------------------
    else if (target == 0x3) {
        // Set Flags
        set_flag(FLAG_ACTIVATED, 1); 
        set_flag(FLAG_STARTED,   0);
        // Reset the System State
        set_system_state(/*msb*/15, /*lsb*/0, /*val*/3);
        // Remove the 'Play' sign
        eid_update_with_eeprom(eid_get_current_display() & ~DISP_PLAY);
        // Go back to the default refresh rate
        cnt_disp_refresh_interval = 1;
        disp_refresh_interval     = 1440;   // 24 hours
    }

}

static void update_system_configs(uint32_t use_default) {

#ifdef DEVEL
    use_default = 1;
#endif

    // Use Default values
    if (use_default) {
        //--- VBAT and ADC
        eeprom_adc_t1           = 2534; // ( 2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
        eeprom_adc_t2           = 2534; // ( 2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
        eeprom_adc_t3           = 2534; // ( 2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
        eeprom_adc_t4           = 2534; // ( 2534 ) Temperature Threshold (Raw Code) for ADC. See the table above.
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
        
        //--- EID Configurations                    Recommended Value
        eeprom_eid_high_temp_threshold  = 2000; // ( 2000 (=15C)  ) Temperature Threshold (Raw Code) to determine HIGH and MID temperature for EID
        eeprom_eid_low_temp_threshold   = 800 ; // ( 800  (=-5C)  ) Temperature Threshold (Raw Code) to determine MID and LOW temperature for EID
        eeprom_eid_duration_high        = 60  ; // ( 60   (=0.5s) ) EID duration (ECTR_PULSE_WIDTH) for High Temperature
        eeprom_eid_fe_duration_high     = 60  ; // ( 60   (=0.5s) ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for High Temperature. '0' makes it skip 'Field Erase'
        eeprom_eid_rfrsh_int_hr_high    = 12  ; // ( 12   (=12hr) ) EID Refresh interval for High Temperature (unit: hr). '0' means 'do not refresh'.
        eeprom_eid_duration_mid         = 500 ; // ( 500  (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Mid Temperature
        eeprom_eid_fe_duration_mid      = 500 ; // ( 500  (=4s)   ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Mid Temperature. '0' makes it skip 'Field Erase'
        eeprom_eid_rfrsh_int_hr_mid     = 24  ; // ( 24   (=24hr) ) EID Refresh interval for Mid Temperature (unit: hr). '0' means 'do not refresh'.
        eeprom_eid_duration_low         = 500 ; // ( 500  (=4s)   ) EID duration (ECTR_PULSE_WIDTH) for Low Temperature
        eeprom_eid_fe_duration_low      = 500 ; // ( 500  (=4s)   ) EID 'Field Erase' duration (ECTR_PULSE_WIDTH) for Low Temperature. '0' makes it skip 'Field Erase'
        eeprom_eid_rfrsh_int_hr_low     = 0   ; // ( 0    (=none) ) EID Refresh interval for Low Temperature (unit: hr). '0' means 'do not refresh'.

        //--- Other Configurations
        eeprom_pmu_num_cons_meas        =  5;   // (            5 ) Number of consecutive measurements required to change SAR ratio or trigger Low VBAT warning/Crash Handler.
        eeprom_timer_calib_interval     = 10;   // (           10 ) Timer Calibration Interval. '0' means 'do not calibrate'. (unit: minute)
        eeprom_te_last_sample_res       =  5;   // (            5 ) Resolution of TE_LAST_SAMPLE (unit: minute). See comment on EEPROM_ADDR_TE_LAST_SAMPLE.
    }
    // Get the real values from EEPROM
    else {

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

        //--- Calibration Data
        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_SNT_CALIB_CONFIG, /*nb*/3);
        eeprom_snt_calib_config         = get_bits(temp,  7,  0);
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

        //--- Other Configurations
        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_PMU_NUM_CONS_MEAS, /*nb*/1);
        eeprom_pmu_num_cons_meas        = get_bits(temp, 7,  0);

        temp = nfc_i2c_byte_read(/*e2*/0, /*addr*/EEPROM_ADDR_TIMER_CALIB_INTERVAL, /*nb*/4);
        eeprom_timer_calib_interval     = get_bits(temp, 15,  0);
        eeprom_te_last_sample_res       = get_bits(temp, 31, 16);
    }

    // Update SNT timer variables
    snt_timer_1min = (eeprom_snt_base_freq << 6) - (eeprom_snt_base_freq << 2);

}

static void update_snt_timer_threshold(void) {

    // Update xo_timer_threshold only if it has been woken up by the SNT timer
    if (WAKEUP_BY_SNT) {
        xo_timer_threshold = xo_timer_threshold + snt_timer_1min;
        if(xo_timer_threshold==0) xo_timer_threshold = 1; // SNT Timer does not expire at cnt=0.
    }

    // Just in case - check the current SNT counter value.
    //-----------------------------------------------------------------
    //  t: xo_timer_threshold
    //  c: curr_snt_val
    //
    // CASE A)  th > c
    //
    //          0              c    th                      MAX
    //          |--------------|----|------------------------|      if th - c < 2s: it is near the wakeup
    //
    //          0  c                                     th MAX
    //          |--|-------------------------------------|---|      if th - c > 4m: it has already passed the threshold.
    //
    // CASE B)  th < c
    //
    //          0              th   c                       MAX
    //          |--------------|----|------------------------|      if th + A > 4m: it has already passed the threshold
    //                               <-----------A---------->
    //
    //          0  th                                  c    MAX
    //          |--|-----------------------------------|-----|      if th + A < 2s: it is neaer the wakeup
    //                                                  <-A->
    //
    //-----------------------------------------------------------------
    uint32_t dist_min = (snt_timer_1min>>3)+(snt_timer_1min>>5);    // 1/8 + 1/32 = 5/32 (= 0.15625); 9.375 sec
    uint32_t dist_max = (snt_timer_1min<<2);                        // 4 mins
    uint32_t curr_snt_val = snt_read_wup_timer();
    uint32_t distance;

    if (xo_timer_threshold >= curr_snt_val) { distance = xo_timer_threshold - curr_snt_val;                } 
    else                                     { distance = xo_timer_threshold + (0xFFFFFFFF - curr_snt_val); }

    if ((distance < dist_min) || (distance > dist_max)) {
        #ifdef DEVEL
            mbus_write_message32(0x84, 0x0);
        #endif
        set_flag(FLAG_SLEEP_BYPASS, 1);
        // For safely, disable/clear all IRQs.
        *NVIC_ICER = 0xFFFFFFFF;
        *NVIC_ICPR = 0xFFFFFFFF;
        // Call main()
        main();
        return;
    }

    #ifdef DEVEL
        mbus_write_message32(0x84, xo_timer_threshold);
    #endif
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
void handler_ext_int_reg1     (void) { 
    *NVIC_ICPR = (0x1 << IRQ_REG1);       
    // Used for SNT Temp Sensor Measurement in snt_operation()
}
void handler_ext_int_reg2     (void) { *NVIC_ICPR = (0x1 << IRQ_REG2);       }
void handler_ext_int_reg3     (void) { *NVIC_ICPR = (0x1 << IRQ_REG3);       }
void handler_ext_int_reg4     (void) { *NVIC_ICPR = (0x1 << IRQ_REG4);       }
void handler_ext_int_reg5     (void) { *NVIC_ICPR = (0x1 << IRQ_REG5);       }
void handler_ext_int_reg6     (void) { *NVIC_ICPR = (0x1 << IRQ_REG6);       }
void handler_ext_int_reg7     (void) { *NVIC_ICPR = (0x1 << IRQ_REG7);       }
void handler_ext_int_mbusmem  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);   }
void handler_ext_int_mbusrx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);    }
void handler_ext_int_mbustx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);    }
void handler_ext_int_mbusfwd  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);   }

void handler_ext_int_gocep    (void) { 
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);      

    uint32_t goc_raw = *GOC_DATA_IRQ;
    *GOC_DATA_IRQ   = 0;

    // Activating System
    if ((goc_raw==GOC_ACTIVATE_KEY) || (goc_raw==GOC_ACT_START_KEY)) { 
        if (!get_flag(FLAG_ACTIVATED)) {
            // Enable EID Crash Hander
            eid_enable_crash_handler();
            set_flag(FLAG_WD_ENABLED, 1); 

            // Set system
            set_system  (   /*target*/          0x1,    /*ACTIVATED*/
                            /*reset_cmd*/       0x1,
                            /*reset_start_info*/0x1
                        );

            if (goc_raw==GOC_ACT_START_KEY) { 
                goc_raw = GOC_START_KEY;
            }
        }
    }

    // Manually change the display
    else if ((goc_raw&0xFFFF0000)==(GOC_IMM_DISPLAY_KEY&0xFFFF0000)) {
        eid_update_with_eeprom(goc_raw&0x7F);
    }

#ifdef DEVEL
    //---------------------------------------------------------
    // GOC NFC Emulation 
    //---------------------------------------------------------
    // IMPORTANT: It is assumed that 
    //              1) System is in ACTIVATED state
    //              2) there is no RF acitivity while executing
    //---------------------------------------------------------

    if (get_flag(FLAG_ACTIVATED)) {
        if ((goc_raw==GOC_START_KEY) || (goc_raw==GOC_STOP_KEY) || (goc_raw==GOC_RESET_KEY)
            || ((goc_raw&0xFFFF0000)==(GOC_DISPLAY_KEY&0xFFFF0000))) {

            if (nfc_i2c_get_token(/*num_try*/1)) {

                // Start the temperature measurement
                if (goc_raw==GOC_START_KEY) 
                    nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD, /*data*/ (0x1<<7)|(0x01<<0), /*nb*/ 1);
                // Stop the temperature measurement
                else if (goc_raw==GOC_STOP_KEY) 
                    nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD, /*data*/ (0x1<<7)|(0x02<<0), /*nb*/ 1);
                // Reset the system
                else if (goc_raw==GOC_RESET_KEY) 
                    nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD, /*data*/ (0x1<<7)|(0x1E<<0), /*nb*/ 1);
                // Manipulate Display
                else if ((goc_raw&0xFFFF0000)==(GOC_DISPLAY_KEY&0xFFFF0000)) {
                    nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD, /*data*/ (0x1<<7)|(0x10<<0), /*nb*/ 1);
                    nfc_i2c_byte_write(/*e2*/0, /*addr*/ EEPROM_ADDR_CMD_PARAM, /*data*/ goc_raw&0x7F, /*nb*/ 1);
                }

                update_snt_timer_threshold();
                operation_sleep_snt_timer(/*auto_reset*/0, /*threshold*/xo_timer_threshold);
            }
        }
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

int main(void) {

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;
    
    wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1;

    #ifdef DEVEL
        mbus_write_message32(0x80, wakeup_source);
    #endif

    // Check-in the EID Watchdog if it is enabled
    if (get_flag(FLAG_WD_ENABLED) && !snt_running) eid_check_in();

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_TIMER32) | (0x1 << IRQ_XOT);

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    //// Handle GOC triggers
    //if (get_flag(FLAG_INITIALIZED) && WAKEUP_BY_GOCEP) {
    //    handler_ext_int_gocep();
    //}

    // Wakeup Count (increment only when SNT_IDLE)
    if (!snt_running) {
        wakeup_count++;
        #ifdef DEVEL
            mbus_write_message32(0x81, wakeup_count);
        #endif
    }

    ////--------------------------------------------------------------------------
    //// System Activated
    ////--------------------------------------------------------------------------
    //if (get_flag(FLAG_ACTIVATED)) {

    //    // Only if it has been waken up by SNT
    //    if (WAKEUP_BY_SNT) {

    //        #ifdef DEVEL
    //            mbus_write_message32(0x82, (0x00 << 24) | tmp_state);
    //            mbus_write_message32(0x82, (0x01 << 24) | cnt_temp_meas_start_delay);
    //            mbus_write_message32(0x82, (0x02 << 24) | cnt_temp_meas_interval);
    //            mbus_write_message32(0x82, (0x03 << 24) | cnt_disp_refresh_interval);
    //            mbus_write_message32(0x82, (0x04 << 24) | cnt_timer_calib_interval);
    //            mbus_write_message32(0x82, (0x0E << 24) | eeprom_sample_count);
    //            mbus_write_message32(0x82, (0x0F << 24) | temp_sample_count);
    //        #endif

    //        //----------------------------------------------------------------------
    //        // Checking NFC Activity
    //        //----------------------------------------------------------------------
    //        if (!snt_running) { nfc_check_cmd(); }

    //        //----------------------------------------------------------------------
    //        // Temperature Measurement FSM
    //        //----------------------------------------------------------------------
    //        if (tmp_state==TMP_IDLE) {
    //            if (get_flag(FLAG_STARTED)) {

    //                #ifdef DEVEL
    //                    eeprom_high_temp_threshold   = 700;
    //                    eeprom_low_temp_threshold    = 450;
    //                    eeprom_temp_meas_interval    = 2;
    //                    eeprom_temp_meas_start_delay = 1;
    //                    eeprom_temp_meas_config      = (1 << 0);

    //                    mbus_write_message32(0x83, 0x00000002);
    //                    mbus_write_message32(0x83, (0x01 << 24) | eeprom_temp_meas_start_delay);
    //                    mbus_write_message32(0x83, (0x02 << 24) | eeprom_temp_meas_interval);
    //                    mbus_write_message32(0x83, (0x03 << 24) | eeprom_timer_calib_interval);
    //                    mbus_write_message32(0x83, (0x04 << 24) | eeprom_te_last_sample_res);
    //                    mbus_write_message32(0x83, (0x05 << 24) | eeprom_temp_meas_config);
    //                    mbus_write_message32(0x83, (0x06 << 24) | eeprom_high_temp_threshold);
    //                    mbus_write_message32(0x83, (0x07 << 24) | eeprom_low_temp_threshold);
    //                #endif

    //                // Decide where to go
    //                if (eeprom_temp_meas_start_delay==1) {
    //                    tmp_state = TMP_ACTIVE;
    //                    cnt_temp_meas_interval = eeprom_temp_meas_interval;
    //                }
    //                else tmp_state = TMP_PENDING;

    //                // Calibrate the SNT timer
    //                cnt_timer_calib_interval = eeprom_timer_calib_interval;

    //            }
    //            #ifdef DEVEL
    //            else {
    //                mbus_write_message32(0x83, 0x00000003);
    //            }
    //            #endif
    //        }
    //        else if (tmp_state==TMP_PENDING) {
    //            if (cnt_temp_meas_start_delay == eeprom_temp_meas_start_delay) {
    //                #ifdef DEVEL
    //                    mbus_write_message32(0x83, 0x00000004);
    //                #endif
    //                tmp_state = TMP_ACTIVE;
    //                cnt_temp_meas_interval = eeprom_temp_meas_interval;
    //            }
    //            else {
    //                cnt_temp_meas_start_delay++;
    //                #ifdef DEVEL
    //                    mbus_write_message32(0x83, 0x00000005);
    //                #endif
    //            }
    //        }
    //        else if (tmp_state==TMP_ACTIVE) {
    //            if (cnt_temp_meas_interval == eeprom_temp_meas_interval) {
    //                #ifdef DEVEL
    //                    mbus_write_message32(0x83, 0x00000006);
    //                #endif
    //                snt_operation();
    //                cnt_temp_meas_interval = 0;
    //                cnt_te_last_sample     = 0;
    //                cnt_te_last_sample_res = 0;
    //            }
    //            #ifdef DEVEL
    //            else {
    //                mbus_write_message32(0x83, 0x00000007);
    //            }
    //            #endif

    //            // Write TE_LAST_SAMPLE
    //            if (  (cnt_te_last_sample_res == 0) 
    //               || (cnt_te_last_sample_res == eeprom_te_last_sample_res)) {
    //                if(nfc_i2c_get_token(/*num_try*/30)) {
    //                    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_TE_LAST_SAMPLE, /*data*/cnt_te_last_sample, /*nb*/2);
    //                }
    //                cnt_te_last_sample_res = 0;
    //            }

    //            cnt_temp_meas_interval++;
    //            cnt_te_last_sample++;
    //            cnt_te_last_sample_res++;

    //        }

    //        //----------------------------------------------------------------------
    //        // Refresh Display
    //        //----------------------------------------------------------------------
    //        if (disp_refresh_interval != 0) {
    //            if (cnt_disp_refresh_interval == disp_refresh_interval) {
    //                #ifdef DEVEL
    //                    mbus_write_message32(0x83, 0x00000009);
    //                #endif
    //                nfc_power_off(); // Turn off NFC, if on, to save power
    //                eid_update(eid_get_current_display());
    //                cnt_disp_refresh_interval = 0;
    //            }
    //            cnt_disp_refresh_interval++;
    //        }
    //        else {
    //            cnt_disp_refresh_interval = 1;
    //        }

    //        //----------------------------------------------------------------------
    //        // SNT Timer Calibration (Only when the system has been started)
    //        //----------------------------------------------------------------------
    //        if ((get_flag(FLAG_STARTED)) && (eeprom_timer_calib_interval != 0)) {
    //            if (cnt_timer_calib_interval == eeprom_timer_calib_interval) {
    //                #ifdef DEVEL
    //                    mbus_write_message32(0x83, 0x0000000A);
    //                #endif
    //                calibrate_snt_timer();
    //                cnt_timer_calib_interval = 0;
    //            }
    //            cnt_timer_calib_interval++;
    //        }
    //        else {
    //            cnt_timer_calib_interval = 1;
    //        }
    //    } // WAKEUP_BY_SNT

    //    //----------------------------------------------------------------------
    //    // Go to sleep
    //    //----------------------------------------------------------------------
    //    update_snt_timer_threshold();
    //    operation_sleep_snt_timer(/*auto_reset*/0, /*threshold*/xo_timer_threshold);

    //} // ACTIVATED

    //xo_timer_threshold += 1000;

    //*REG_XOT_CONFIGU = xo_timer_threshold >> 16;
    //*REG_XOT_CONFIG  = (0xE << 20) | (xo_timer_threshold & 0xFFFF);


    //--------------------------------------------------------------------------
    // Invalid Operation - Go to Sleep
    //--------------------------------------------------------------------------
    operation_sleep();
    while(1) asm("nop");
    return 1;
}

