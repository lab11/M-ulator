//*******************************************************************************************
// XT1 (TMCv1) FIRMWARE
// Version alpha-0.1
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
//      Once actual temp measurement starts after the start_delay, 
//      the display shows the 'triangle' as well as the 'check mark' or 'plus/minus' mark, depending on the measurement. 
//      It also displays the 'low batt' if the VBAT ADC reading indicates so.
//
//          GOC_QUICK_START_KEY 0xDECAFBAD  // Start the temperature measurement without changing the existing settings.
//          GOC_START_KEY       0xFEEDC0DE  // Start the temperature measurement after updating the settings.
//
//  * STOPPED: 
//      You can stop the system by sending GOC_STOP_KEY.
//      Once triggered, the system goes back to 'ACTIVATED' state.
//
//          GOC_STOP_KEY        0xDEADBEEF  // Stop the ongoing temperature measurement.
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
//  0x82    {0x01, curr_start_delay}            Counter for Start Delay
//  0x82    {0x02, curr_period_temp_meas}       Counter for Temp Meas Period
//  0x82    {0x03, curr_period_refresh_disp}    Counter for Display Refresh
//  0x82    {0x04, curr_period_timer_calib}     Counter for Timer Calibration
//  0x82    {0x05, curr_num_temp_meas_iter}     Current Number of Temp Meas (Iteration)
//  0x82    {0x06, curr_num_temp_meas}          Current Number of Temp Meas
//
//  0x82    {0x10, snt_state}                   SNT State (in snt_operation())
//                                                      0x0 SNT_IDLE        
//                                                      0x1 SNT_LDO         
//                                                      0x2 SNT_TEMP_START  
//                                                      0x3 SNT_TEMP_READ   
//
//  0x83    0x00000000                          NFC activity detected (GPO=1) (only when snt_state=SNT_IDLE)
//  0x83    0x00000001                          Temp Meas forced to stop by user
//  0x83    0x00000002                          Temp Meas started and now it becomes TMP_PENDING
//
//  0x83    {0x01, start_delay}                 Start Delay
//  0x83    {0x02, period_temp_meas}            Temp Meas Period
//  0x83    {0x03, period_refresh_disp}         Display Refresh
//  0x83    {0x04, period_timer_calib}          Timer Calibration
//  0x83    {0x05, num_temp_meas_iter}          Number of Temp Meas (Iteration)
//  0x83    {0x06, num_temp_meas}               Number of Temp Meas
//  0x83    {0x07, high_temp_threshold}         High Temp Threshold
//  0x83    {0x08, low_temp_threshold}          Low Temp Threshold
//
//  0x83    0x00000003                          Temp Meas stays in IDLE (Has not started by user)
//  0x83    0x00000004                          Temp Meas Start Delay has met. Now it becomes TMP_ACTIVE.
//  0x83    0x00000005                          Temp Meas Start Delay has not met. It is still in TMP_PENDING.
//  0x83    0x00000006                          Temp Meas performs the temp measurement in this active session (TMP_ACTIVE)
//  0x83    0x00000007                          Temp Meas skips this active session (TMP_ACTIVE)
//  0x83    0x00000008                          Temp Meas has performed the given number of temp measurements. Now it goes back to TMP_IDLE.
//  0x83    0x00000009                          Refreshing Eink display
//  0x83    0x0000000A                          Calibrating the SNT timer (using XO)
//
//  0x84    snt_timer_threshold                 New threshold value for the SNT timer
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

//*******************************************************************************************
// DEBUGGING
//*******************************************************************************************
#define DEVEL                   // Enable this to shorten long delays/wait times. FLAG_STARTED also becomes 1 when FLAG_ACTIVATED becomes 1.
#define GOCEP_RUN_CPU_ONLY      // Enable this when you cannot do 'GEN_IRQ' in GOC/EP Header (e.g., the current m3_ice script)
#define DEBUG                   // Send debug messages

//*******************************************************************************************
// KEYS
//*******************************************************************************************
//[NOTE] [31:24]=0xFF triggers the Demo Display

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
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// PRE Clock-based Delay (5 instructions @ 100kHz w/ 2x margin)
#define DLY_1MS     40
#define DLY_1S      40000

// XO Initialization Wait Duration
#define XO_WAIT_A   20000   // Must be ~1 second delay. Delay for XO Start-Up. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#define XO_WAIT_B   20000   // Must be ~1 second delay. Delay for VLDO & IBIAS Generation. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).

// XO Counter Value per Specific Time Durations
#define XOT_1SEC    2048        // By default, the XO clock frequency is 2kHz. Frequency = 2 ^ XO_SEL_CLK_OUT_DIV (kHz).
#define XOT_1MIN    60*XOT_1SEC
#define XOT_1HR     60*XOT_1MIN
#define XOT_1DAY    24*XOT_1HR

// SNT Wakeup Counter Value per Specific Time Durations
#define SNT_1SEC    1494        // Measured with XT1#3, VBAT=2.6V, RT
#define SNT_1MIN    60*SNT_1SEC
#define SNT_1HR     60*SNT_1MIN
#define SNT_1DAY    24*SNT_1HR


//*******************************************************************************************
// SNT LAYER CONFIGURATION
//*******************************************************************************************

// SNT states
#define SNT_IDLE        0x0
#define SNT_LDO         0x1
#define SNT_TEMP_START  0x2
#define SNT_TEMP_READ   0x3

// Temp Meas states
#define TMP_IDLE        0x0
#define TMP_PENDING     0x1
#define TMP_ACTIVE      0x2
#define TMP_DONE        0x3

//*******************************************************************************************
// NFC CONFIGURATION
//*******************************************************************************************
//[NOTE] All EEPROM_*_ADDR are 'byte' addresses.

#define EEPROM_FLAG_ADDR        0x00    // [DO NOT CHANGE] Flag Address in EEPROM. Must be the same as __NFC_FLAG_ADDR__
#define EEPROM_SYS_START_ADDR   0x04    // Start address of EEPROM where the system info is stored.
#define EEPROM_TMP_START_ADDR   0x80    // Start address of EEPROM where the first temperature data is stored. 
#define EEPROM_NUM_BYTES        8192    // Number of bytes in EEPROM

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
#define DISP_ALL            0x7F

//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************
//--- User Parameters
volatile uint32_t start_delay;              // (Default:   30) Start Delay before starting temperature measurement (unit: snt_timer_sleep_duration)
volatile uint32_t period_temp_meas;         // (Default:   15) Period of Temperature Measurement (unit: snt_timer_sleep_duration)
volatile uint32_t period_refresh_disp;      // (Default: 1440) Period of Display Refresh (unit: snt_timer_sleep_duration)
volatile uint32_t period_timer_calib;       // (Default:    1) Period of SNT Timer Calibration (unit:snt_timer_sleep_duration)
volatile uint32_t num_temp_meas_iter;       // (Default:    1) Measure temperature 'num_temp_meas_iter'-times to increment curr_num_temp_meas
volatile uint32_t num_temp_meas;            // (Default:    0) Number of temperature measurements set by user
volatile uint32_t high_temp_threshold;      // (Default:  300) Threshold for High Temperature
volatile uint32_t low_temp_threshold;       // (Default:  100) Threshold for Low Temperature

//--- Tracking the user parameters
volatile uint32_t curr_start_delay;         // Counter for start_delay
volatile uint32_t curr_period_temp_meas;    // Counter for period_temp_meas
volatile uint32_t curr_period_refresh_disp; // Counter for period_refresh_disp
volatile uint32_t curr_period_timer_calib;  // Counter for period_timer_calib
volatile uint32_t curr_num_temp_meas_iter;  // Counter for num_temp_meas_iter
volatile uint32_t curr_num_temp_meas;       // Counter for num_temp_meas

//--- General 
volatile uint32_t wakeup_source;            // Wakeup Source. Updated each time the system wakes up. See 'WAKEUP_SOURCE definition'.
volatile uint32_t wakeup_count;             // Wakeup Count. Incrementing each time the system wakes up. It does not increment if the system wakes up to resume the SNT temperature measurement.

//--- SNT & Timer
volatile uint32_t snt_timer_threshold;      // SNT Timer Threshold to wake up the system
volatile uint32_t snt_timer_sleep_duration; // SNT Timer tick count corresponding to the sleep duration (default: SNT_1MIN)

//--- SNT & Temperature Measurement
volatile uint32_t snt_state;                // SNT state
volatile uint32_t tmp_state;                // TMP state
volatile uint32_t snt_running;              // Indicates whether the SNT temp sensor is running.
volatile uint32_t eeprom_addr;              // EEPROM byte address for temperature data storage

#ifdef DEVEL
//--- Used for DEVEL
volatile uint32_t devel_var;
#endif

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//-- Initialization/Sleep Functions
static void operation_sleep (void);
static void operation_sleep_snt_timer(uint32_t auto_reset, uint32_t timestamp);
static void operation_back_to_default(void);
static void operation_init (void);

//-- Application Specific
static uint32_t snt_operation (void);

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
    tmp_state = TMP_IDLE;
    nfc_power_off();
    #ifdef DEVEL
        devel_var = 0;
        *GOC_DATA_IRQ = 0;
    #endif
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
        wakeup_count            = 0;
        snt_running             = 0;
        snt_state               = SNT_IDLE;
        tmp_state               = TMP_IDLE;
        eeprom_addr             = EEPROM_TMP_START_ADDR;
        snt_timer_threshold      = 0;
        snt_timer_sleep_duration = SNT_1MIN;

        // User Parameter Initialization
        #ifdef DEVEL
            start_delay         = 1;
            period_temp_meas    = 1;
            period_refresh_disp = 60;
            period_timer_calib  = 1;
            num_temp_meas_iter  = 1;
            num_temp_meas       = 0;
            high_temp_threshold = 300;
            low_temp_threshold  = 100;
        #else
            start_delay         = 30;
            period_temp_meas    = 15;
            period_refresh_disp = 1440;
            period_timer_calib  = 1;
            num_temp_meas_iter  = 1;
            num_temp_meas       = 0;
            high_temp_threshold = 300;
            low_temp_threshold  = 100;
        #endif

        curr_start_delay            = 0;
        curr_period_temp_meas       = 0;
        curr_period_refresh_disp    = 0;
        curr_period_timer_calib     = 0;
        curr_num_temp_meas_iter     = 0;
        curr_num_temp_meas          = 0;

        #ifdef DEVEL
            devel_var = 0;
        #endif

        //-------------------------------------------------
        // PRE Tuning
        //-------------------------------------------------
        pre_r0B.as_int = PRE_R0B_DEFAULT_AS_INT;
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
        pmu_init();

        //-------------------------------------------------
        // NFC 
        //-------------------------------------------------
        nfc_init();

        // Flag Initialization
        reset_flag();

        //-------------------------------------------------
        // SNT Settings
        //-------------------------------------------------
        snt_init();

        // Update the flag
        set_flag(FLAG_ENUMERATED, 1);

        // Start turning on the SNT timer 
        // NOTE: TMC goes into sleep at the end of snt_start_timer()
        snt_start_timer();
    }
    else if (!get_flag(FLAG_INITIALIZED)) {
        
        // Finish setting up the SNT timer
        snt_start_timer();

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
        eid_init();
        eid_update(DISP_ALL);

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);

        //-------------------------------------------------
        // Indefinite Sleep
        //-------------------------------------------------
        operation_sleep();
    }
}

//-------------------------------------------------------------------
// Application-Specific Functions
//-------------------------------------------------------------------

static uint32_t snt_operation (void) {

    #ifdef DEBUG
        mbus_write_message32(0x82, (0x10 << 24) | snt_state);
    #endif

    if (snt_state == SNT_IDLE) {
        snt_running = 1;

        snt_state = SNT_LDO;

        //--- Update the Display
        //eid_update(DISP_RUNNING | DISP_NORMAL | DISP_PLUS | DISP_MINUS);

        // Turn on SNT LDO VREF.
        snt_ldo_vref_on();
    }
    else if (snt_state == SNT_LDO) {
        snt_state = SNT_TEMP_START;

        // Turn on SNT LDO
        snt_ldo_power_on();

        // Turn on SNT Temperature Sensor
        snt_temp_sensor_power_on();
    }
    else if (snt_state == SNT_TEMP_START){
        snt_state = SNT_TEMP_READ;

        // Reset the Temp Sensor
        snt_temp_sensor_reset();

        // Release the reset for the Temp Sensor
        snt_temp_sensor_start();

        // Go to sleep during measurement
        operation_sleep();
    }
    else if (snt_state == SNT_TEMP_READ) {

        uint32_t snt_temp_val = *SNT_TARGET_REG_ADDR;
        curr_num_temp_meas_iter++;

        // Option to take multiple measurements
        if (curr_num_temp_meas_iter < num_temp_meas_iter) {
            snt_temp_sensor_reset();
            snt_state = SNT_TEMP_START;
        }
        // All measurements are done
        else {
            curr_num_temp_meas_iter = 0;
            curr_num_temp_meas++;

            // VBAT Measurement and SAR_RATIO Adjustment
            uint32_t pmu_adc_vbat_val = pmu_adc_read_and_sar_ratio_adjustment();

            // If VBAT is too low, trigger the EID Watchdog (System Crash)
            if (pmu_adc_vbat_val > pmu_get_adc_crit_val()) {
                eid_trigger_crash();
                while(1);
            }
            
            // Assert temp sensor isolation & turn off temp sensor power
            snt_temp_sensor_power_off();
            snt_ldo_power_off();
            snt_state = SNT_IDLE;

            /////////////////////////////////////////////////////////////////
            // Store the Measured data
            //---------------------------------------------------------------
            nfc_i2c_byte_write(/*e2*/0, 
                /*addr*/ EEPROM_SYS_START_ADDR, 
                /*data*/ (0<<31) | (curr_num_temp_meas&0x7FFFFFFF),
                /* nb */ 4
                );
            nfc_i2c_byte_write(/*e2*/0, 
                /*addr*/ eeprom_addr, 
                /*data*/ (0<<31) | ((curr_num_temp_meas&0x7F)<<24) | (snt_temp_val&0xFFFFFF),
                /* nb */ 4
                );
            eeprom_addr += 4;
            if (eeprom_addr==EEPROM_NUM_BYTES) eeprom_addr = EEPROM_TMP_START_ADDR; // roll-over
            /////////////////////////////////////////////////////////////////

            /////////////////////////////////////////////////////////////////
            // Update e-Ink
            //---------------------------------------------------------------
            //--- Default Display
            uint32_t seg_display = DISP_RUNNING | DISP_NORMAL;

            //--- Check temperature
            if (snt_temp_val > high_temp_threshold)
                seg_display = DISP_RUNNING | DISP_XMARK | DISP_HIGH_TEMP;
            else if  (snt_temp_val < low_temp_threshold)
                seg_display = DISP_RUNNING | DISP_XMARK | DISP_LOW_TEMP;

            //--- Check VBAT
            if (pmu_adc_vbat_val > pmu_get_adc_low_val()) seg_display |= DISP_LOW_VBAT;

            //--- Update the Display if needed
            #ifdef DEVEL
                if (devel_var == 0x2) {
                    eid_update(seg_display);
                }
                else if (devel_var == 0x0) {
                    if (seg_display!=eid_get_current_display()) eid_update(seg_display);
                }
            #else
                if (seg_display!=eid_get_current_display()) eid_update(seg_display);
            #endif
            /////////////////////////////////////////////////////////////////

            snt_running = 0;

            #ifdef DEVEL
                // Temp Meas routine is done.
                if (devel_var==0) {return 1;}
            #else
                return 1;
            #endif

        }
    }
    return 0; // Temp Meas routine is still running.
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

    uint32_t goc_raw;

    // If this function is called because there has been an NFC activity
    if (get_bit(wakeup_source, 31)) {
        goc_raw = 0;

    }
    // If this is called by normal GOC/EP interrupt
    else {
        *NVIC_ICPR      = (0x1 << IRQ_GOCEP);      
        goc_raw         = *GOC_DATA_IRQ;
        //*GOC_DATA_IRQ   = 0;
    }

#ifdef DEVEL
    uint32_t goc_data;
    uint32_t i;
    // Demo Display
    if (((goc_raw>>24)&0xFF)==0xFF) { eid_update(goc_raw&0x1FF);}
    // Power Measurement (goc_raw[31:24] = 0xFF)
    else if (((goc_raw>>24)&0xFF)==0xAA) { 
        goc_data = goc_raw&0xFFFFFF;

        // Keep reading the first 4 bytes from EEPROM (goc_raw[23:0] = 0x000000)
        if (goc_data == 0x000000) {
            uint32_t temp=nfc_i2c_byte_read(/*e2*/0, /*addr*/0, /*nb*/4);
        }
        // Temp Meas + Write EEPROM (goc_raw[23:0] = 0x000001)
        else if (goc_data == 0x000001) {
            devel_var = 0x1;
            operation_sleep_snt_timer(/*auto_reset*/1, /*threshold*/SNT_1SEC<<1);
        }
        // Temp Meas + Write EEPROM + Write Eink (goc_raw[23:0] = 0x000002)
        else if (goc_data == 0x000002) {
            devel_var = 0x2;
            operation_sleep_snt_timer(/*auto_reset*/1, /*threshold*/SNT_1SEC<<1);
        }
    }

#endif

    // Activating System
    if (goc_raw==GOC_ACTIVATE_KEY) { 
        if (!get_flag(FLAG_ACTIVATED)) {
            // Enable EID Crash Hander
            eid_enable_crash_handler();

            // Update the flags
            set_flag(FLAG_ACTIVATED, 1); 
            set_flag(FLAG_WD_ENABLED, 1); 
            #ifdef DEVEL
                set_flag(FLAG_STARTED, 1);
            #endif
            nfc_power_off();

            // Clear the display
            eid_update(DISP_NONE);
        }
    }
    // Quick Start the temperature measurement
    else if (goc_raw==GOC_QUICK_START_KEY) { 
        if (!get_flag(FLAG_STARTED)) {
            set_flag(FLAG_STARTED, 1);
        }
    }
    // Start the temperature measurement
    else if (goc_raw==GOC_START_KEY) { 
        if (!get_flag(FLAG_STARTED)) {

            start_delay         = *(GOC_DATA_IRQ+1);
            period_temp_meas    = *(GOC_DATA_IRQ+2);
            period_refresh_disp = *(GOC_DATA_IRQ+3);
            period_timer_calib  = *(GOC_DATA_IRQ+4);
            num_temp_meas_iter  = *(GOC_DATA_IRQ+5);
            num_temp_meas       = *(GOC_DATA_IRQ+6);
            high_temp_threshold = *(GOC_DATA_IRQ+7);
            low_temp_threshold  = *(GOC_DATA_IRQ+8);

            set_flag(FLAG_STARTED, 1);
        }
    }
    // Stop the ongoing temperature measurement
    else if (goc_raw==GOC_STOP_KEY) { 
        if (get_flag(FLAG_STARTED)) {
            set_flag(FLAG_STARTED, 0);
        }
    }

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

    // Disable PRE Watchdog Timers (CPU watchdog and MBus watchdog)
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;
    
    // Get the info on who woke up the system, then reset WAKEUP_SOURCE register.
    wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1;

    #ifdef DEVEL
    if ((devel_var==0x1)||(devel_var==0x2)) {
        if (*GOC_DATA_IRQ!=0) operation_back_to_default();
    }
    #endif

    #ifdef DEBUG
        mbus_write_message32(0x80, wakeup_source);
    #endif

    // Check-in the EID Watchdog if it is enabled (STATE 6)
    if (get_flag(FLAG_WD_ENABLED) && (snt_state==SNT_IDLE)) eid_check_in();

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_TIMER32);
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
    if (snt_state==SNT_IDLE) {
        wakeup_count++;
        #ifdef DEBUG
            mbus_write_message32(0x81, wakeup_count);
        #endif
    }

    //--------------------------------------------------------------------------
    // DEVEL Mode
    //--------------------------------------------------------------------------
    #ifdef DEVEL
    if ((devel_var==0x1)||(devel_var==0x2)) {
        snt_disable_wup_timer();
        while(*GOC_DATA_IRQ==0) { snt_operation(); }
    }
    #endif


    //--------------------------------------------------------------------------
    // System Activated
    //--------------------------------------------------------------------------
    if (get_flag(FLAG_ACTIVATED)) {

        #ifdef DEBUG
            mbus_write_message32(0x82, (0x00 << 24) | tmp_state);
            mbus_write_message32(0x82, (0x01 << 24) | curr_start_delay);
            mbus_write_message32(0x82, (0x02 << 24) | curr_period_temp_meas);
            mbus_write_message32(0x82, (0x03 << 24) | curr_period_refresh_disp);
            mbus_write_message32(0x82, (0x04 << 24) | curr_period_timer_calib);
            mbus_write_message32(0x82, (0x05 << 24) | curr_num_temp_meas_iter);
            mbus_write_message32(0x82, (0x06 << 24) | curr_num_temp_meas);
        #endif

        //----------------------------------------------------------------------
        // NFC activity detected (GPO=1)
        //----------------------------------------------------------------------
        if ((*GPIO_DATA&0x1)&&(snt_state==SNT_IDLE)) {
            #ifdef DEBUG
                mbus_write_message32(0x83, 0x00000000);
            #endif
            // Act as if the NFC has waken up the system, then call GOC/EP handler.
            wakeup_source = set_bit(wakeup_source, 31, 1);
            handler_ext_int_gocep();
        }

        //----------------------------------------------------------------------
        // Temperature Measurement FSM
        //----------------------------------------------------------------------
        //-- If forced to stop
        if (!get_flag(FLAG_STARTED)) {
            #ifdef DEBUG
                mbus_write_message32(0x83, 0x00000001);
            #endif
            operation_back_to_default();
        }
            
        //-- FSM implementation
        if (tmp_state==TMP_IDLE) {
            if (get_flag(FLAG_STARTED)) {
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000002);
                    mbus_write_message32(0x83, (0x01 << 24) | start_delay);
                    mbus_write_message32(0x83, (0x02 << 24) | period_temp_meas);
                    mbus_write_message32(0x83, (0x03 << 24) | period_refresh_disp);
                    mbus_write_message32(0x83, (0x04 << 24) | period_timer_calib);
                    mbus_write_message32(0x83, (0x05 << 24) | num_temp_meas_iter);
                    mbus_write_message32(0x83, (0x06 << 24) | num_temp_meas);
                    mbus_write_message32(0x83, (0x07 << 24) | high_temp_threshold);
                    mbus_write_message32(0x83, (0x08 << 24) | low_temp_threshold);
                #endif
                tmp_state = TMP_PENDING;
                // Reset counters
                curr_start_delay        = 0;
                curr_period_temp_meas   = 0;
                curr_num_temp_meas_iter = 0;
                curr_num_temp_meas      = 0;
                // Display 
                eid_update(DISP_RUNNING);
            }
            #ifdef DEBUG
            else {
                mbus_write_message32(0x83, 0x00000003);
            }
            #endif
        }
        else if (tmp_state==TMP_PENDING) {
            if (curr_start_delay == start_delay) {
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000004);
                #endif
                tmp_state = TMP_ACTIVE;
            }
            else {
                curr_start_delay++;
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000005);
                #endif
            }
        }
        else if (tmp_state==TMP_ACTIVE) {
            if (curr_period_temp_meas == period_temp_meas) {
                #ifdef DEBUG
                    mbus_write_message32(0x83, 0x00000006);
                #endif
                while(!snt_operation());
                curr_period_temp_meas = 0;
                if ((num_temp_meas!=0) && (curr_num_temp_meas==num_temp_meas)) {
                    tmp_state = TMP_DONE;
                }
            }
            #ifdef DEBUG
            else {
                mbus_write_message32(0x83, 0x00000007);
            }
            #endif
            curr_period_temp_meas++;

        }
        else if (tmp_state==TMP_DONE) {
            #ifdef DEBUG
                mbus_write_message32(0x83, 0x00000008);
            #endif
            set_flag(FLAG_STARTED, 0);
            tmp_state = TMP_IDLE;
        }

        //----------------------------------------------------------------------
        // Refresh Display
        //----------------------------------------------------------------------
        if (curr_period_refresh_disp == period_refresh_disp) {
            #ifdef DEBUG
                mbus_write_message32(0x83, 0x00000009);
            #endif
            eid_update(eid_get_current_display());
            curr_period_refresh_disp = 0;
        }
        curr_period_refresh_disp++;

        //----------------------------------------------------------------------
        // SNT Timer Calibration
        //----------------------------------------------------------------------
        if (curr_period_timer_calib == period_timer_calib) {
            #ifdef DEBUG
                mbus_write_message32(0x83, 0x0000000A);
            #endif
            curr_period_timer_calib = 0;
            // NOTE:
            // XO & SNT Calibration
            // 37000 SNT Timer
            // 25 seconds XO calibration every measurement
        }
        curr_period_timer_calib++;

        //----------------------------------------------------------------------
        // Go to sleep
        //----------------------------------------------------------------------
        snt_timer_threshold = snt_timer_threshold + snt_timer_sleep_duration;
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
