//*******************************************************************************************
// ZHIYOONG: THINGS TO DO
//-------------------------------------------------------------------------------------------
// * operation_init() calls pmu_init() that initializes the PMU setting. 
//      The PMU configuration used here is very similar to what was used in the previous TS code.
//      Please change pmu_init() based on what you have found during your recent PMU testing.
//      Currently, pmu_init() is commented out.
// * Ideally, pmu_* functions should reside in TMCv1.h and TMCv1.c.
//      However they are still included in this file with 'static' keyword.
//      This is because I think the PMU settings may not be determined for the real use yet - 
//      Once we figure out what to do, we may want to move the functions into TMCv1.h/TMCv1.c
//      and remove the 'static' keyword.
//*******************************************************************************************

//*******************************************************************************************
// XT1 (TMCv1) FIRMWARE
// Version alpha-0.1
// NOTE: Major portion of this code is based on TSstack_ondemand_v2.0 (pre_v20e)
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
//  GPIO[0]     GPO                 SEG[0]  Triangle                   [==]                 .
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
//  STATE 1) First Programming to load this binary onto TMC's SRAM
//              TMC executes operation_init(). In the middle of operation_init(), TMC also
//              enables the SNT timer, which involves a brief (2~3 seconds) sleep duration.
//              TMC wakes up again and finishes the remaining initialization sequence,
//              and then goes into sleep for a long duration (XOT_SLEEP_DURATION_LONG).
//              FLAG_ENUMERATED set to 1 in operation_init() before it goes into the brief sleep duration.
//              FLAG_INITIALIZED set to 1 in operation_init() after the initialization sequence is fully done
//              (i.e., before going into the sleep for a long duration (XOT_SLEEP_DURATION_LONG)
//
//  STATE 2) User needs to put a sticker on the GOC diode.
//
//  STATE 3) After XOT_SLEEP_DURATION_LONG from Step 1, TMC wakes up and enables GIT (FLAG_GIT_ENABLED set to 1)
//
//  STATE 4) Then TMC periodically wakes up and goes into sleep (Sleep duration: XOT_SLEEP_DURATION)
//              while measuring ambient temperature and adjusting PMU as needed.
//              This continues until the user removes the sticker.
//
//  STATE 5) Once the user removes the sticker, TMC resets all operation. (FLAG_GIT_TRIGGERED set to 1)
//              TMC enables the EID crash detector & handler (FLAG_WD_ENABLED set to 1)
//
//  STATE 6) Then it periodically wakes up and goes into sleep (Sleep duration: XOT_SLEEP_DURATION)
//              while measuring ambient temperature and adjusting PMU as needed.
//              TMC also stores the temperature measurement in the NFC's EEPROM.
//
//-------------------------------------------------------------------------------------------
// FLAG Register
//-------------------------------------------------------------------------------------------
//
//  FLAG_ENUMERATED
//      Set to 1 when enumeration and other initialization is done in operation_init() in STATE 1.
//
//  FLAG_INITIALIZED
//      Set to 1 when the initialization is fully done, including the SNT timer, 
//      in operation_init() in STATE 1.
//
//  FLAG_GIT_ENABLED
//      Set to 1 when the GIT becomes enabled in STATE 3.
//
//  FLAG_GIT_TRIGGERED
//      Set to 1 when the GIT gets triggered (i.e., the user removes the sticker) in STATE 5.
//
//  FLAG_WD_ENABLED
//      Set to 1 when TMC enables the EID crash detector & handler in STATE 5.
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
// < UPDATE HISTORY >
//  Jun 24 2021 -   First commit 
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//******************************************************************************************* 
// THINGS TO DO
//******************************************************************************************* 
// - Add a feature where the user writes the current wall time through GOC or NFC
//      (e.g., when the user reads out using NFC)
// - TS 2.0 has TEMP_CALIB_A = 240000, while TS 1.4 has TEMP_CALIB_A=24000.
//      According to the comment, 24000 is correct?
// - Define the HIGH/LOW threshold for temperatures to update E-ink Display.
//      snt_high_temp_threshold
//      snt_low_temp_threshold
// - The original code has read_data_batadc_diff variable, but it is not used anywhere.
// - The original code uses temp_storage_latest, but it seems redundant.
// - There is pmu_reg_write(0x45) in the PMU initialization in the original code.
//      But we may not need this. Bit[7:0] is only for 'vbat_read_only'
//      which is not enabled at all.
// - The original code has timeout check with TIMER32 using wfi_timeout_flag variable.
//      But it does not make sense because TIMER32 does not run in Sleep.
//      Think about how to implement the timeout (using XO timer?? - is this possible?)
// - In handler_ext_int_wakeup, if the system is waken up by an MBus message,
//      it may be good to add some signature check to tell whether the wakeup 
//      is due to a glitch on the wire.
// - In snt_operation(), add code to store the temp & VBAT data with timestamp
// - Is it possible to get a GOC/EP to go into sleep while the system is doing the display update?
//      If yes, how can we ensure that we stop the EID operataion before going into sleep?
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

#include "TMCv1.h"

#define DLY_1S  40000    // 5 instructions @ 100kHz w/ 2x margin
#define DLY_1MS 40    // 5 instructions @ 100kHz w/ 2x margin

//*******************************************************************************************
// DEBUGGING
//*******************************************************************************************
#define DEBUG                   // Send debug MBus messages (Disable this for real use)
#define FAIL_MBUS_ADDR  0xEF    // fail(): In case of failure, it sends an MBus message containing the failure code to this MBus Address.

#define NO_PMU
//#define SIMPLE_WAKEUP_SLEEP     // If enabled, it does not execute snt_operation(). It just does simple wakeup/sleep sequence.

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED     0
#define FLAG_INITIALIZED    1
#define FLAG_GIT_ENABLED    2
#define FLAG_GIT_TRIGGERED  3
#define FLAG_WD_ENABLED     4
//#define FLAG_UPDATE_EEPROM  // If enabled, store the flag in the first page in EEPROM

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

#define USE_SNT_TIMER   // If enabled, it relies on the SNT timer for wakeup/sleep

// XO Initialization Wait Duration
#define XO_WAIT_A  20000    // Must be ~1 second delay. Delay for XO Start-Up. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#define XO_WAIT_B  20000    // Must be ~1 second delay. Delay for VLDO & IBIAS Generation. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).

// XO Counter Value per Specific Time Durations
#define XOT_1SEC    2048        // By default, the XO clock frequency is 2kHz. Frequency = 2 ^ XO_SEL_CLK_OUT_DIV (kHz).
#define XOT_1MIN    60*XOT_1SEC
#define XOT_1HR     60*XOT_1MIN
#define XOT_1DAY    24*XOT_1HR

// Sleep Duration 
#define XOT_SLEEP_DURATION_LONG      10*XOT_1SEC     // The long sleep duration during which the user must put on a GIT sticker
#define XOT_SLEEP_DURATION_PREGIT    10*XOT_1SEC     // The sleep duration before activating the system
#define XOT_SLEEP_DURATION           10*XOT_1SEC     // The sleep duration after the system activation

// SNT Wakeup Counter Value per Specific Time Durations
#define SNT_1SEC    1494        // Measured with XT1#3, VBAT=2.6V, RT
#define SNT_1MIN    60*SNT_1SEC
#define SNT_1HR     60*SNT_1MIN
#define SNT_1DAY    24*SNT_1HR

// Sleep Duration 
#define SNT_SLEEP_DURATION_LONG      10*SNT_1SEC     // The long sleep duration during which the user must put on a GIT sticker
#define SNT_SLEEP_DURATION_PREGIT    10*SNT_1SEC     // The sleep duration before activating the system
#define SNT_SLEEP_DURATION           10*SNT_1SEC     // The sleep duration after the system activation

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


//*******************************************************************************************
// PMU/SNT LAYER CONFIGURATION
//*******************************************************************************************

// PMU Behavior
#define PMU_CHECK_WRITE         // If enabled, PMU replies with an actual read-out data from the analog block, rather than just forwarding the write data.
#define PMU_ADC_PERIOD      1   // Period of ADC VBAT measurement (Default: 500)
#define PMU_TICK_ADC_RESET  2   // Pulse Width of PMU's ADC_RESET signal (Default: 50)
#define PMU_TICK_ADC_CLK    2   // Pulse Width of PMU's ADC_CLK signal (Default: 2)

// PMU Reg Write Timeout
#define PMU_TIMEOUT 300000  // LSB corresponds to ~10us, assuming ~100kHz CPU clock.

// System Operation Mode
#define ACTIVE      0x1
#define SLEEP       0x0

// SNT states
#define SNT_IDLE        0x0
#define SNT_LDO         0x1
#define SNT_TEMP_START  0x2
#define SNT_TEMP_READ   0x6

// PMU Temperatures
#define PMU_10C 0x0
#define PMU_20C 0x1
#define PMU_25C 0x2
#define PMU_35C 0x3
#define PMU_55C 0x4
#define PMU_75C 0x5
#define PMU_95C 0x6

// Number of SNT Temp Sensor Measurements during snt_operation()'s full cycle.
#define SNT_NUM_TEMP_MEAS   1

//-------------------------------------------------------------------
// PMU Floor Loop-Up Tables
//-------------------------------------------------------------------

// ACTIVE
volatile union pmu_floor pmu_floor_active[] = {
    //  Temp       idx     R    L    BASE  L_SAR
    /*PMU_10C */ /* 0*/  {{0xF, 0x7, 0x0F, 0xE}},
    /*PMU_20C */ /* 1*/  {{0xF, 0x7, 0x0F, 0xE}},
    /*PMU_25C */ /* 2*/  {{0xF, 0x3, 0x0F, 0x7}},
    /*PMU_35C */ /* 3*/  {{0xF, 0x2, 0x0F, 0x4}},
    /*PMU_55C */ /* 4*/  {{0x6, 0x1, 0x0F, 0x2}},
    /*PMU_75C */ /* 5*/  {{0x6, 0x1, 0x0F, 0x2}},
    /*PMU_95C */ /* 6*/  {{0x6, 0x1, 0x0F, 0x2}},
    };

// SLEEP
volatile union pmu_floor pmu_floor_sleep[] = {
    //  Temp       idx     R    L    BASE  L_SAR
    /*PMU_10C */ /* 0*/  {{0xF, 0x1, 0x01, 0x2}},
    /*PMU_20C */ /* 1*/  {{0xF, 0x1, 0x01, 0x2}},
    /*PMU_25C */ /* 2*/  {{0xF, 0x0, 0x01, 0x1}},
    /*PMU_35C */ /* 3*/  {{0xF, 0x0, 0x01, 0x1}},
    /*PMU_55C */ /* 4*/  {{0x6, 0x0, 0x01, 0x1}},
    /*PMU_75C */ /* 5*/  {{0x6, 0x0, 0x01, 0x1}},
    /*PMU_95C */ /* 6*/  {{0x6, 0x0, 0x01, 0x1}},
    };

// SLEEP (TEMP SENSOR)
volatile union pmu_floor pmu_floor_sleep_tsnt[] = {
    //  Temp Range        idx     R    L    BASE  L_SAR
    /*       ~PMU_10C*/ /* 0*/  {{0xF, 0xA, 0x07, 0xF}},
    /*PMU_20C~PMU_25C*/ /* 1*/  {{0xF, 0xA, 0x05, 0xF}},
    /*PMU_35C~PMU_55C*/ /* 2*/  {{0xA, 0xA, 0x05, 0xF}},
    /*PMU_75C~       */ /* 3*/  {{0x5, 0xA, 0x05, 0xF}},
    };

// SLEEP (LOW)
volatile union pmu_floor pmu_floor_sleep_low[] = {
    //  idx     R    L    BASE  L_SAR
      /* 0*/  {{0xF, 0x0, 0x01, 0x1}},
      };

// SLEEP (RADIO)
volatile union pmu_floor pmu_floor_sleep_radio[] = {
    //  idx     R    L    BASE  L_SAR
      /* 0*/  {{0xF, 0xA, 0x05, 0xF}},
      };

//-------------------------------------------------------------------
// Temperature Sensor Threshold for PMU Floor Setting
//-------------------------------------------------------------------
volatile uint32_t PMU_10C_threshold_snt = 600;      // Around 10C
volatile uint32_t PMU_20C_threshold_snt = 1000;     // Around 20C
volatile uint32_t PMU_35C_threshold_snt = 2000;     // Around 35C
volatile uint32_t PMU_55C_threshold_snt = 3200;     // Around 55C
volatile uint32_t PMU_75C_threshold_snt = 7000;     // Around 75C
volatile uint32_t PMU_95C_threshold_snt = 12000;    // Around 95C

//-------------------------------------------------------------------
// Temperature Sensor Calibration Coefficients
//-------------------------------------------------------------------
// TEMP_CALIB_A: A is the slope, typical value is around 24.000, stored as A*1000
// TEMP_CALIB_B: B is the offset, typical value is around -3750.000, stored as -(B-3400)*100 due to limitations on radio bitfield
//-------------------------------------------------------------------
volatile uint32_t TEMP_CALIB_A = 240000;
volatile uint32_t TEMP_CALIB_B = 20000; 

//-------------------------------------------------------------------
// PMU State (Also see TMCv1.h)
//-------------------------------------------------------------------
volatile union pmu_state pmu_desired_state_sleep;   // Register 0x3B
volatile union pmu_state pmu_desired_state_active;  // Register 0x3C
volatile union pmu_state pmu_stall_state_sleep;     // Register 0x39
volatile union pmu_state pmu_stall_state_active;    // Register 0x3A

// PMU VSOLAR SHORT BEHAVIOR
// --------------------------------------
// PMU_VSOLAR_SHORT    | EN_VSOLAR_SHORT
// Value               | Behavior
// --------------------------------------
//   0x08 (default)    | 0 until POR activates; then 1 until V0P6 output turns on; then follows vbat_high_latched.
//   0x0A              | Always 0
//   0x0B              | Always 1
//   0x0C              | vbat_high_latched     (comparator output from PMU)
//   0x18              | vbat_high_lat_latched (latched comparator output from PMU)
// --------------------------------------
#define PMU_VSOLAR_SHORT        0x08    // (Default 5'h08) See table above
#define PMU_VSOLAR_CLAMP_TOP    0x0     // (Default 3'h0) Decrease clamp threshold voltage
#define PMU_VSOLAR_CLAMP_BOTTOM 0x1     // (Default 3'h1) Increase clamp threshold voltage


//*******************************************************************************************
// EID LAYER CONFIGURATION
//*******************************************************************************************

// Display Patterns: See 'External Connections' for details
#define DISP_TRIANGLE   (0x1 << 0)
#define DISP_TICK       (0x1 << 1)
#define DISP_LOWBATT    (0x1 << 2)
#define DISP_BACKSLASH  (0x1 << 3)
#define DISP_SLASH      (0x1 << 4)
#define DISP_PLUS       (0x1 << 5)
#define DISP_MINUS      (0x1 << 6)

// Display Presets
#define DISP_NONE           0
#define DISP_CHECK          DISP_TICK  | DISP_SLASH
#define DISP_XMARK          DISP_SLASH | DISP_BACKSLASH
#define DISP_RUNNING        DISP_TRIANGLE
#define DISP_NORMAL         DISP_CHECK
#define DISP_HIGH_TEMP      DISP_PLUS
#define DISP_LOW_TEMP       DISP_MINUS
#define DISP_LOW_VBAT       DISP_LOWBATT

//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************
//--- General 
volatile uint32_t wakeup_source;            // Wakeup Source. Updated each time the system wakes up. See 'WAKEUP_SOURCE definition'.
volatile uint32_t wakeup_count;             // Wakeup Count. Incrementing each time the system wakes up.
volatile uint32_t wakeup_timestamp;         // XOT counter value upon wake-up. Updated only if the system is waken up by the XO Timer.
volatile uint32_t wakeup_timestamp_rollover;// Not Used.
//--- SNT & Temperature Measurement
volatile uint32_t snt_state;                // SNT state: SNT_IDLE, SNT_LDO, SNT_TEMP_START, SNT_READ
volatile uint32_t meas_count;               // Meas count. Number of temperature measurements performed during the snt_operation()'s full cycle.
volatile uint32_t exec_count;               // Exec count. Incrementing each time snt_operation() completes the full cycle.
volatile uint32_t snt_running;              // Indicates whether the SNT temp sensor is running.
volatile uint32_t read_data_temp;           // SNT Temperature Measurement Value (24-bit)
volatile uint32_t snt_high_temp_threshold;  // Threshold for High Temperature (used for display)
volatile uint32_t snt_low_temp_threshold;   // Threshold for Low Temperature (used for display)
volatile uint32_t nfc_temp_addr;            // EEPROM byte address for temperature data storage
//--- PMU & ADC (VBAT) Measurement
volatile uint32_t pmu_setting_state;        // Current PMU setting state. e.g., PMU_10C, PMU_20C, ...
volatile uint32_t read_data_batadc;         // PMU ADC value for VBAT measurement (8-bit)
volatile uint32_t read_data_batadc_diff;    // Not Used
volatile uint32_t pmu_adc_3p0_val;          // PMU ADC value at VBAT = 3.0V
volatile uint32_t pmu_adc_low_val;          // Threshold for Low VBAT (used for display)
volatile uint32_t pmu_adc_crit_val;         // Threshold for Critical VBAT (used to trigger the EID crash detector & handler)
//--- Register Files
volatile pre_r0B_t pre_r0B = PRE_R0B_DEFAULT;
volatile snt_r00_t snt_r00 = SNT_R00_DEFAULT;
volatile snt_r01_t snt_r01 = SNT_R01_DEFAULT;
volatile snt_r03_t snt_r03 = SNT_R03_DEFAULT;

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//-- Initialization/Sleep Functions
static void operation_sleep (void);
static void operation_prepare_sleep_notimer(void);
static void operation_sleep_notimer(void);
static void operation_sleep_xo_timer(uint32_t timestamp);
static void operation_sleep_snt_timer(uint32_t auto_reset, uint32_t timestamp);
static void operation_back_to_default(void);
static void operation_init (void);

//-- Application Specific

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
// Function: operation_prepare_sleep_notimer
// Args    : None
// Description:
//           Turns off SNT Temp Sensor and LDO.
//           Set PMU to room temperature setting
// Return  : None
//-------------------------------------------------------------------
static void operation_prepare_sleep_notimer(void){
    // Disable Wakeup Request
    xot_disable_wreq();
    // Make sure LDO is off
    if (snt_running){
        snt_running = 0;
        snt_temp_sensor_power_off();
        snt_ldo_power_off();
    }
    // Set PMU to room temp setting
    if (pmu_setting_state != PMU_25C){
        pmu_setting_state = PMU_25C;
        pmu_set_active_temp_based();
        pmu_set_sleep_temp_based();
    }
}

//-------------------------------------------------------------------
// Function: operation_sleep_notimer
// Args    : None
// Description:
//           Goes into sleep without a timer (i.e., indefinite sleep)
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep_notimer(void){
    operation_prepare_sleep_notimer();
    operation_sleep();
}

//-------------------------------------------------------------------
// Function: operation_sleep_xo_timer
// Args    : timestamp - Time stamp for the XO Timer
// Description:
//           Goes into sleep with XO Timer enabled
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep_xo_timer(uint32_t timestamp){
    xot_enable(timestamp);
    operation_sleep();
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
//           This is supposed to be executed when GOC/EP triggers
//           while the system is in Active.
//           It replaces operation_goc_trigger_init() in the TS code.
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
        // Initialize Global Variables
        // NOTE: Need to explicitly initialize global variables if the reset value is 0
        //-------------------------------------------------
        exec_count = 0;
        wakeup_count = 0;
        pmu_adc_3p0_val = 0x62;
        read_data_batadc = pmu_adc_3p0_val + 8;   // Fresh VBAT = ~2.80V
        pmu_adc_low_val  = pmu_adc_3p0_val + 53;  // VBAT = ~2.00V
        pmu_adc_crit_val = pmu_adc_3p0_val + 60;  // VBAT = ~1.90V
        snt_running = 0;
        snt_state = SNT_IDLE;
        read_data_temp = 150; // From SNSv10 (does this need an initial value?)
        snt_high_temp_threshold = 300;
        snt_low_temp_threshold = 100;
        nfc_temp_addr = 0x4;    // First 4-bytes (1 word) is used for flag storage
        wakeup_timestamp = 0;
        wakeup_timestamp_rollover = 0;


        //-------------------------------------------------
        // PRE Tuning
        //-------------------------------------------------
        //--- Set CPU & MBus Clock Speeds      Default
        pre_r0B.CLK_GEN_RING         = 0x1; // 0x1
        pre_r0B.CLK_GEN_DIV_MBC      = 0x1; // 0x2
        pre_r0B.CLK_GEN_DIV_CORE     = 0x2; // 0x3
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

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
        //eid_init();
        //eid_update(DISP_RUNNING);

        //-------------------------------------------------
        // NFC 
        //-------------------------------------------------
        nfc_init();

        //-------------------------------------------------
        // SNT Settings
        //-------------------------------------------------
        //snt_init();

        // Update the flag
        set_flag(FLAG_ENUMERATED, 1);
        #ifdef FLAG_UPDATE_EEPROM
            nfc_i2c_set_flag(FLAG_ENUMERATED, 1);
        #endif

//        // Start turning on the SNT timer 
//        // NOTE: TMC goes into sleep at the end of snt_start_timer()
//        snt_start_timer();
//    }
//    else if (!get_flag(FLAG_INITIALIZED)) {
//        
//        // Finish setting up the SNT timer
//        snt_start_timer();

        //-------------------------------------------------
        // XO Driver
        //-------------------------------------------------
        xo_start(XO_WAIT_A, XO_WAIT_B);
        *XOT_START_COUT = 1;

        //-------------------------------------------------
        // MRR Settings
        //-------------------------------------------------
        // NOTE: XT1 does not use MRR

//        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);
//        #ifdef FLAG_UPDATE_EEPROM
//            nfc_i2c_set_flag(FLAG_INITIALIZED, 1);
//        #endif

        //-------------------------------------------------
        // Sleep
        //-------------------------------------------------
        // It must go into sleep with a long 'sleep' duration
        // to give user enough time to put a "sticker" over the GOC solar cell.
//        #ifdef USE_SNT_TIMER
//            operation_sleep_snt_timer(/*auto_reset*/1, /*threshold*/SNT_SLEEP_DURATION_LONG);
//        #else
//            reset_xo_cnt(); // Make counter value = 0
//            operation_sleep_xo_timer(XOT_SLEEP_DURATION_LONG);
//        #endif

    }
}

//-------------------------------------------------------------------
// Application-Specific Functions
//-------------------------------------------------------------------


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
void handler_ext_int_reg7     (void) { *NVIC_ICPR = (0x1 << IRQ_REG7);       }
void handler_ext_int_mbusmem  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);   }
void handler_ext_int_mbusrx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);    }
void handler_ext_int_mbustx   (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);    }
void handler_ext_int_mbusfwd  (void) { *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);   }

void handler_ext_int_gocep    (void) { 
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);      

    uint32_t goc_raw    = *GOC_DATA_IRQ;
    uint32_t goc_header = (goc_raw & 0xFF000000) >> 24;
    uint32_t goc_data   = (goc_raw & 0x00FFFFFF);
    uint32_t i;

    mbus_write_message32(0x80, goc_raw);
    mbus_write_message32(0x81, *(GOC_DATA_IRQ+1));
    mbus_write_message32(0x82, goc_header);
    mbus_write_message32(0x83, goc_data);

    //--------------------------------------------------
    //
    // *GOC_DATA_IRQ = 0x0000000n (n = 0x0 ~ 0xF)
    //
    // [Description] 
    //      Various shortcuts
    //
    //      n = 0x0: Write all 0 to the entire EEPROM
    //      n = 0x1: Write all 0 to the entire EEPROM (except the first 1 word)
    //      n = 0x2: Write a word 'data @ GOC_DATA_IRQ+2' to 'addr @ GOC_DATA_IRQ+1'
    //      n = 0x3: Read a word from 'addr @ GOC_DATA_IRQ+1'
    //      n = 0x4: Reserved
    //      n = 0x5: Reserved
    //      n = 0x6: Reserved
    //      n = 0x7: Reserved
    //      n = 0x8: Reserved
    //      n = 0x9: Reserved
    //      n = 0xA: Reserved
    //      n = 0xB: Reserved
    //      n = 0xC: Reserved
    //      n = 0xD: Reserved
    //      n = 0xE: Write all 1 to the entire EEPROM (except the first 1 word)
    //      n = 0xF: Write all 1 to the entire EEPROM
    //--------------------------------------------------

    if (goc_header == 0x00) {
        // Write All0 to the entire EEPROM
        if (goc_data == 0x0) {
            for (i=0; i<8192; i=i+256) {
                nfc_i2c_seq_word_pattern_write(/*e2*/0, /*addr*/i, /*data*/0x00000000, /*len*/64);
            }
        }
        // Write All0 to the entire EEPROM (except the first 1 word)
        else if (goc_data == 0x1) {
            nfc_i2c_seq_word_pattern_write(/*e2*/0, /*addr*/4, /*data*/0x00000000, /*len*/63);
            for (i=256; i<8192; i=i+256) {
                nfc_i2c_seq_word_pattern_write(/*e2*/0, /*addr*/i, /*data*/0x00000000, /*len*/64);
            }
        }
        // Write a word 'data @ GOC_DATA_IRQ+2' to 'addr @ GOC_DATA_IRQ+1'
        else if (goc_data == 0x2) {
            nfc_i2c_word_write(/*e2*/0, *(GOC_DATA_IRQ+1), *(GOC_DATA_IRQ+2));
        }
        // Read a word from 'addr @ GOC_DATA_IRQ+1'
        else if (goc_data == 0x3) {
            mbus_write_message32(0x80, nfc_i2c_word_read(/*e2*/0, *(GOC_DATA_IRQ+1)));
        }
        // Toggle SCL (GPIO[1]) for the specified number at (GOC_DATA_IRQ+1)
        else if (goc_data == 0x4) {
            gpio_set_dir_with_mask   (__I2C_MASK__,(1<<__NFC_SCL__)|(0<<__NFC_SDA__)); // SCL(output), SDA(input)
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SCL__)|(0<<__NFC_SDA__)); // SCL=0
            //set_cps(0x1 << 2);
            unfreeze_gpio_out();
            for (i=0; i<*(GOC_DATA_IRQ+1); i++) {
                *GPIO_DATA = 0x02;  // SCL = 1
                *GPIO_DATA = 0x00;  // SCL = 0
            }
            // update the retentive memory
            *R_GPIO_DIR  = 0x02;
            *R_GPIO_DATA = 0x00;
            freeze_gpio_out();
            //set_cps(0x0 << 2);
        }
        // Toggle SDA (GPIO[2]) for the specified number at (GOC_DATA_IRQ+1)
        else if (goc_data == 0x5) {
            gpio_set_dir_with_mask   (__I2C_MASK__,(0<<__NFC_SCL__)|(1<<__NFC_SDA__)); // SCL(input), SDA(output)
            gpio_write_data_with_mask(__I2C_MASK__,(0<<__NFC_SCL__)|(0<<__NFC_SDA__)); // SDA=0
            //set_cps(0x1 << 2);
            unfreeze_gpio_out();
            for (i=0; i<*(GOC_DATA_IRQ+1); i++) {
                *GPIO_DATA = 0x04;  // SDA = 1
                *GPIO_DATA = 0x00;  // SDA = 0
            }
            // update the retentive memory
            *R_GPIO_DIR  = 0x04;
            *R_GPIO_DATA = 0x00;
            freeze_gpio_out();
            //set_cps(0x0 << 2);
        }
        // Toggle GPO (GPIO[0]) for the specified number at (GOC_DATA_IRQ+1)
        else if (goc_data == 0x6) {
            gpio_set_dir_with_mask   (__GPO_MASK__,(1<<__NFC_GPO__)); // GPO(output)
            gpio_write_data_with_mask(__GPO_MASK__,(0<<__NFC_GPO__)); // GPO=0
            //set_cps(0x1 << 2);
            unfreeze_gpio_out();
            for (i=0; i<*(GOC_DATA_IRQ+1); i++) {
                *GPIO_DATA = 0x01;  // GPO = 1
                *GPIO_DATA = 0x00;  // GPO = 0
            }
            gpio_set_dir_with_mask   (__GPO_MASK__,(0<<__NFC_GPO__)); // GPO(input)
            // update the retentive memory
            *R_GPIO_DIR  = 0x00;
            *R_GPIO_DATA = 0x00;
            freeze_gpio_out();
            //set_cps(0x0 << 2);
        }
        // Turn on NFC
        else if (goc_data == 0x7) {
            nfc_power_on();
            delay(10000);
        }
        // Re-start XO
        else if (goc_data == 0x8) {
            xo_stop();
            xo_start(XO_WAIT_A, XO_WAIT_B);
        }
        // Enable XO_CLK output
        else if (goc_data == 0x9) {
            *XOT_START_COUT = 1;
        }
        // Write All1 to the entire EEPROM (except the first 1 word)
        else if (goc_data == 0xE) {
            nfc_i2c_seq_word_pattern_write(/*e2*/0, /*addr*/4, /*data*/0xFFFFFFFF, /*len*/63);
            for (i=256; i<8192; i=i+256) {
                nfc_i2c_seq_word_pattern_write(/*e2*/0, /*addr*/i, /*data*/0xFFFFFFFF, /*len*/64);
            }
        }
        // Write All1 to the entire EEPROM
        else if (goc_data == 0xF) {
            for (i=0; i<8192; i=i+256) {
                nfc_i2c_seq_word_pattern_write(/*e2*/0, /*addr*/i, /*data*/0xFFFFFFFF, /*len*/64);
            }
        }
        nfc_power_off();
    }
    
//    #ifdef USE_SNT_TIMER
//        operation_sleep_snt_timer(/*auto_reset*/1, /*threshold*/SNT_SLEEP_DURATION_LONG);
//    #else
//        reset_xo_cnt(); // Make counter value = 0
//        operation_sleep_xo_timer(XOT_SLEEP_DURATION_LONG);
//    #endif
}

void handler_ext_int_softreset(void) { *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); }
void handler_ext_int_wakeup   (void) { *NVIC_ICPR = (0x1 << IRQ_WAKEUP);     }
void handler_ext_int_aes      (void) { *NVIC_ICPR = (0x1 << IRQ_AES);        }
void handler_ext_int_spi      (void) { *NVIC_ICPR = (0x1 << IRQ_SPI);        }
void handler_ext_int_gpio     (void) { *NVIC_ICPR = (0x1 << IRQ_GPIO);       }

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    wakeup_count++;

    #ifdef DEBUG
        mbus_write_message32(0x70, wakeup_count);
    #endif

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;
    
    // Get the info on who woke up the system, then reset WAKEUP_SOURCE register.
    wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1;

    // Check-in the EID Watchdog if it is enabled (STATE 6)
    if (get_flag(FLAG_WD_ENABLED)) eid_check_in();

    // For safety, disable Wakeup Timer's WREQ.
    xot_disable_wreq();

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_TIMER32)|(0x1 << IRQ_GOCEP);

    // If this is the very first wakeup, initialize the system (STATE 1)
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    //--------------------------------------------------------------------------
    // If woken up by GIT (i.e., User removes the sticker!), trigger STEP 5.
    //--------------------------------------------------------------------------
    if (get_bit(wakeup_source, 5)) {
        #ifdef DEBUG
            mbus_write_message32(0x72, 0x4);
        #endif
        // Post-GIT Initialization
        //postgit_init();
    }

    //--------------------------------------------------------------------------
    // If woken up by a XO wakeup timer
    //--------------------------------------------------------------------------
    else if (get_bit(wakeup_source, 2)) {
        #ifdef DEBUG
            mbus_write_message32(0x72, 0x5);
        #endif

        // Save the timestamp
        if (wakeup_timestamp==0) wakeup_timestamp = *XOT_VAL;

        // Enable GIT, if GIT is not yet enabled (STATE 3)
        if (!get_flag(FLAG_GIT_ENABLED)) {
            // Enable GIT (GOC Instant Trigger)
            *REG_GOC_CONFIG = set_bit(*REG_GOC_CONFIG, 16, 1);
            set_flag(FLAG_GIT_ENABLED, 1);
            #ifdef FLAG_UPDATE_EEPROM
                nfc_i2c_set_flag(FLAG_GIT_ENABLED, 1);
            #endif
            // Start Temp/VBAT measurement
            snt_running = 1;
            meas_count = 0;
        }
    }

    //--------------------------------------------------------------------------
    // If woken up by NFC (GPIO[0]) [NOTE] Currently the GPIO[0] wakeup is not supported
    //--------------------------------------------------------------------------
    else if (get_bit(wakeup_source, 3)) {

        //if (get_bit(wakeup_source, 8)) {
        //    #ifdef DEBUG
        //        mbus_write_message32(0x72, 0x6);
        //    #endif
        //    // Handle the NFC event
        //    //do_nfc();
        //}

    }

    //--------------------------------------------------------------------------
    // If woken up by an MBus Message (i.e., SNT Temp Sensor)
    //--------------------------------------------------------------------------
    else if (get_bit(wakeup_source, 4)) { 
        #ifdef DEBUG
            mbus_write_message32(0x72, 0x9);
        #endif
    }

    //--------------------------------------------------------------------------
    // If woken up by GOC/EP
    //--------------------------------------------------------------------------
    else if (get_bit(wakeup_source, 0)) { 
        #ifdef DEBUG
            mbus_write_message32(0x72, 0xA);
        #endif
        handler_ext_int_gocep();
    }

    //--------------------------------------------------------------------------
    // OTHER OPERATIONS
    //--------------------------------------------------------------------------

//    #ifdef SIMPLE_WAKEUP_SLEEP
//        operation_sleep_snt_timer(/*auto_reset*/1, /*threshold*/SNT_SLEEP_DURATION_LONG);
//    #else
//        // If SNT is running
//        while (snt_running) snt_operation();
//    #endif
    mbus_sleep_all();

    //---------------------------------------------
    // Never Quit (should not stay here for an extended duration)
    //---------------------------------------------
    while(1) asm("nop");
    return 1;
}
