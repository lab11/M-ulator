//*******************************************************************************************
// XT1 (TMCv1) FIRMWARE
// Version alpha-0.1
//-------------------------------------------------------------------------------------------
// TMCv1 SUB-LAYER CONNECTION:
//      PREv22E -> SNTv5 -> EIDv1 -> MRRv11A -> MEMv3 -> PMUv13
//-------------------------------------------------------------------------------------------
// NOTE: GIT can be triggered by either VGOC or EDI.
//-------------------------------------------------------------------------------------------
// Major portion of this code is based on TSstack_ondemand_v2.0 (pre_v20e)
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jun 24 2021 -   First commit 
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

#include "TMCv1.h"

#define DLY_1S 20000    // 5 instructions @ 100kHz

//*******************************************************************************************
// SHORT ADDRESSES
//*******************************************************************************************
#define PRE_ADDR    0x1
#define SNT_ADDR    0x2
#define EID_ADDR    0x3
#define MRR_ADDR    0x4
#define MEM_ADDR    0x5
#define PMU_ADDR    0x6

//*******************************************************************************************
// DEBUGGING
//*******************************************************************************************
// Debug Switches
#define ARB_DEBUG           // Send out ARB Debug Messages. Only for verilog simulations. You can ignore this in real silicon.
//#define USE_SHORT_DELAY     // Use short delays for quick simulations. Comment it out for real silicon testing.

// Failure Codes
#define FCODE_0     1   // Unexpected State value in Pre-GIT / Wakeup Timer / Wakeup IRQ
#define FCODE_1     2   // Unexpected State value in Post-GIT / Wakeup Timer / Wakeup IRQ
#define FCODE_2     2   // Unexpected State value in Pre-GIT / MBus / Wakeup IRQ
#define FCODE_3     3   // Unexpected State value in Post-GIT / MBus / Wakeup IRQ
#define FCODE_4     4   // Timeout during PMU Register Write
#define FCODE_5     5   // Timeout during PMU Register Read
#define FCODE_6     6   // Timeout during I2C ACK
#define FAIL_MBUS_ADDR  0xE0    // In case of failure, it sends an MBus message containing the failure code to this MBus Address.

//*******************************************************************************************
// FLAG INDEXES
//*******************************************************************************************
#define FLAG_INITIALIZED    0
#define FLAG_GIT_ENABLED    1
#define FLAG_GIT_TRIGGERED  2
#define FLAG_WD_ENABLED     3
#define FLAG_USE_REAL_TIME  4

//*******************************************************************************************
// SYSTEM STATES/MODES
//*******************************************************************************************

// System Operation Mode
#define ACTIVE      0x1
#define SLEEP       0x0

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
#define PMU_ADC_PERIOD      1   // Period of ADC VBAT measurement (Default: 500)
#define PMU_TICK_ADC_RESET  2   // Pulse Width of PMU's ADC_RESET signal (Default: 50)
#define PMU_TICK_ADC_CLK    2   // Pulse Width of PMU's ADC_CLK signal (Default: 2)

// PMU Reg Write Timeout
#define PMU_TIMEOUT 300000  // LSB corresponds to ~10us, assuming ~100kHz CPU clock.

// PMU Temperatures
#define PMU_10C 0x0
#define PMU_20C 0x1
#define PMU_25C 0x2
#define PMU_35C 0x3
#define PMU_55C 0x4
#define PMU_75C 0x5
#define PMU_95C 0x6

// Number of SNT Temp Sensor Measurement
#define SNT_NUM_TEMP_MEAS   1

//-------------------------------------------------------------------
// PMU Floor Loop-Up Tables
//-------------------------------------------------------------------

// ACTIVE
volatile union pmu_floor pmu_floor_active[] = {
    //  Temp       idx     R    L    BASE  L_SAR
    /*PMU_10C */ /* 0*/  {{0xF, 0x7, 0x10, 0xE}},
    /*PMU_20C */ /* 1*/  {{0xF, 0x7, 0x10, 0xE}},
    /*PMU_25C */ /* 2*/  {{0xF, 0x3, 0x10, 0x7}},
    /*PMU_35C */ /* 3*/  {{0xF, 0x2, 0x10, 0x4}},
    /*PMU_55C */ /* 4*/  {{0x6, 0x1, 0x10, 0x2}},
    /*PMU_75C */ /* 5*/  {{0x6, 0x1, 0x10, 0x2}},
    /*PMU_95C */ /* 6*/  {{0x6, 0x1, 0x10, 0x2}},
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
volatile uint32_t PMU_10C_threshold_snt = 600; // Around 10C
volatile uint32_t PMU_20C_threshold_snt = 1000; // Around 20C
volatile uint32_t PMU_35C_threshold_snt = 2000; // Around 35C
volatile uint32_t PMU_55C_threshold_snt = 3200; // Around 55C
volatile uint32_t PMU_75C_threshold_snt = 7000; // Around 75C
volatile uint32_t PMU_95C_threshold_snt = 12000; // Around 95C

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
// GLOBAL VARIABLES
//*******************************************************************************************
volatile uint32_t wakeup_source;
volatile uint32_t state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t wakeup_count;
volatile uint32_t exec_count_irq;
volatile uint32_t pmu_setting_state;
volatile uint32_t meas_count;
volatile uint32_t read_data_temp;
volatile uint32_t read_data_batadc;
volatile uint32_t read_data_batadc_diff;
volatile uint32_t pmu_adc_3p0_val;
volatile uint32_t pmu_adc_low_val;      // Low VBAT level which displays the warning
volatile uint32_t pmu_adc_crit_val;     // Critical VBAT level which triggers the EID Watchdog
volatile uint32_t wakeup_timestamp;
volatile uint32_t wakeup_timestamp_rollover;

volatile pre_r0B_t pre_r0B = PRE_R0B_DEFAULT;

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//-- PRE Functions
static void set_timer32_timeout(uint32_t val);
static void stop_timer32_timeout_check(uint32_t code);

//-- PMU Functions
static void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data, uint32_t fail_code);
static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data);
static uint32_t pmu_reg_read (uint32_t reg_addr);
static void pmu_config_rat(uint8_t rat);
static void pmu_set_floor(uint8_t mode, uint8_t r, uint8_t l, uint8_t base, uint8_t l_sar);
static void pmu_set_active_temp_based(void);
static void pmu_set_sleep_temp_based(void);
static void pmu_set_sleep_low(void);
static void pmu_set_sleep_tsns(void);
static void pmu_set_sar_ratio (uint32_t ratio);
static void pmu_set_adc_period(uint32_t val);
static void pmu_adc_read_and_sar_ratio_adjustment(void);
static void pmu_init(void);

//-- Initialization/Sleep Functions
static void operation_sleep (void);
static void operation_prepare_sleep_notimer(void);
static void operation_sleep_notimer(void);
static void operation_back_to_default(void);
static void operation_init (void);

//-- Application Specific
static void fail (uint32_t code);


//*******************************************************************************************
// FUNCTIONS IMPLEMENTATION
//*******************************************************************************************

//-------------------------------------------------------------------
// PRE Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: set_timer32_timeout
// Args    : var - Threshold
// Description:
//          set_timer32_timeout() initializes and starts the TIMER 32
//          for a time-out check
// Return  : None
//-------------------------------------------------------------------
static void set_timer32_timeout(uint32_t val){
    wfi_timeout_flag = 0;
    config_timer32(/*cmp*/val, /*roi*/1, /*cnt*/0, /*status*/0);
}

//-------------------------------------------------------------------
// Function: stop_timer32_timeout_check
// Args    : code - Error code
// Description:
//          stop_timer32_timeout_check() stops the TIMER32.
//          If there is a timeout, it calls fail().
// Return  : None
//-------------------------------------------------------------------
static void stop_timer32_timeout_check(uint32_t code){
    *TIMER32_GO = 0;
    if (wfi_timeout_flag){
        wfi_timeout_flag = 0;
        fail (code);
    }
}

//-------------------------------------------------------------------
// PMU Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: pmu_reg_write_core
// Args    : reg_addr  - Register Address
//           reg_data  - Register Data
//           fail_code - Fail Code for Time-Out
// Description:
//          pmu_reg_write_core() writes reg_data in PMU's reg_addr
//          It also performs a time-out check.
// Return  : None
//-------------------------------------------------------------------
static void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data, uint32_t fail_code) {
    set_timer32_timeout(PMU_TIMEOUT);
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    WFI();
    stop_timer32_timeout_check(fail_code);
}

//-------------------------------------------------------------------
// Function: pmu_reg_write
// Args    : reg_addr - Register Address
//           reg_data - Register Data
// Description:
//          pmu_reg_write() writes reg_data in PMU's reg_addr
//          It also performs a time-out check.
// Return  : None
//-------------------------------------------------------------------
static void pmu_reg_write (uint32_t reg_addr, uint32_t reg_data) {
    pmu_reg_write_core(reg_addr, reg_data, FCODE_4);
}

//-------------------------------------------------------------------
// Function: pmu_reg_read
// Args    : reg_addr
// Description:
//          pmu_reg_read() read from reg_addr in PMU.
//          It also performs a time-out check.
// Return  : Read Data
//-------------------------------------------------------------------
static uint32_t pmu_reg_read (uint32_t reg_addr) {
    pmu_reg_write_core(0x00, reg_addr, FCODE_5);
    return *PMU_TARGET_REG_ADDR;
}

//-------------------------------------------------------------------
// Function: pmu_config_rat
// Args    : rat    - If 1, it enables RAT. It also enables slow-loop in Sleep. (Slow-loop in Active is OFF).
//                    If 0, it disables RAT. It also disables slow-loop in Sleep and Active.
// Description:
//           Enables/Disables RAT and Slow-Loop in Sleep.
// Return  : None
//-------------------------------------------------------------------
static void pmu_config_rat(uint8_t rat) {

    //---------------------------------------------------------------------------------------
    // RAT_CONFIG
    // NOTE: Unless overridden, changes in RAT_CONFIG[19:13] do not take effect
    //       until the next sleep or wakeup-up transition.
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x47, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (rat << 19)   // 1'h0     // Enable RAT
        | (0   << 18)   // 1'h0     // Enable SAR Slow-Loop in Active (NOTE: When RAT is enabled, SAR Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (0   << 17)   // 1'h0     // Enable UPC Slow-Loop in Active (NOTE: When RAT is enabled, UPC Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (0   << 16)   // 1'h0     // Enable DNC Slow-Loop in Active (NOTE: When RAT is enabled, DNC Slow-Loop in Active is forced OFF, regardless of this setting.)
        | (rat << 15)   // 1'h0     // Enable SAR Slow-Loop in Sleep
        | (rat << 14)   // 1'h0     // Enable UPC Slow-Loop in Sleep
        | (rat << 13)   // 1'h0     // Enable DNC Slow-Loop in Sleep
        | (0   << 11)   // 2'h0     // Clock Ring Tuning
        | (0   << 8 )   // 3'h0     // Clock Divider Tuning for SAR Charge Pump Pull-Up
        | (0   << 5 )   // 3'h0     // Clock Divider Tuning for UPC Charge Pump Pull-Up
        | (0   << 2 )   // 3'h0     // Clock Divider Tuning for DNC Charge Pump Pull-Up
        | (0   << 0 )   // 2'h0     // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up
    ));

}

//-------------------------------------------------------------------
// Function: pmu_set_floor
// Args    : mode   - If 1, change settings for Active registers.
//                    If 0, change settings for Sleep registers.
//           r      - Right-Leg Multiplier
//           l      - Left-Leg Multiplier (for UPC & DNC)
//           base   - Base Value
//           l_sar  - Left-Leg Multiplier (for SAR)
// Description:
//           Set the VCO floor setting
// Return  : None
//-------------------------------------------------------------------
static void pmu_set_floor(uint8_t mode, uint8_t r, uint8_t l, uint8_t base, uint8_t l_sar){

    // Register Addresses
    uint32_t sar = (mode == ACTIVE) ? 0x16 : 0x15;
    uint32_t upc = (mode == ACTIVE) ? 0x18 : 0x17;
    uint32_t dnc = (mode == ACTIVE) ? 0x1A : 0x19;

    //---------------------------------------------------------------------------------------
    // SAR_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(sar,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0 << 19)     // 1'h0     // Enable PFM even during periodic reset
        | (0 << 18)     // 1'h0     // Enable PFM even when VREF is not used as reference
        | (0 << 17)     // 1'h0     // Enable PFM
        | (3 << 14)     // 3'h3     // Comparator clock division ratio
        | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l_sar << 5)  // 4'h8     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(upc,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (3 << 14)     // 2'h0     // Desired Vout/Vin ratio
        | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l << 5)      // 4'h4     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h08    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(dnc,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (r << 9)      // 4'h8     // Frequency multiplier R
        | (l << 5)      // 4'h4     // Frequency multiplier L (actually L+1)
        | (base)        // 5'h08    // Floor frequency base (0-63)
    ));
}

//-------------------------------------------------------------------
// Function: pmu_set_active_temp_based
// Args    : None
// Description:
//           Sets PMU active floors based on temperature
// Return  : None
//-------------------------------------------------------------------
static void pmu_set_active_temp_based(){
    uint32_t index = pmu_setting_state;
    pmu_set_floor(ACTIVE, 
                    pmu_floor_active[index].R,
                    pmu_floor_active[index].L,
                    pmu_floor_active[index].BASE,
                    pmu_floor_active[index].L_SAR
                    );
}

//-------------------------------------------------------------------
// Function: pmu_set_sleep_temp_based
// Args    : None
// Description:
//           Sets PMU sleep floors based on temperature
// Return  : None
//-------------------------------------------------------------------
static void pmu_set_sleep_temp_based(){
    uint32_t index = pmu_setting_state;
    pmu_set_floor(SLEEP, 
                    pmu_floor_sleep[index].R,
                    pmu_floor_sleep[index].L,
                    pmu_floor_sleep[index].BASE,
                    pmu_floor_sleep[index].L_SAR
                    );
}

//-------------------------------------------------------------------
// Function: pmu_set_sleep_low
// Args    : None
// Description:
//           Sets PMU sleep floors to its lowest setting
// Return  : None
//-------------------------------------------------------------------
static void pmu_set_sleep_low(){
    pmu_set_floor(SLEEP, 
                    pmu_floor_sleep_low[0].R,
                    pmu_floor_sleep_low[0].L,
                    pmu_floor_sleep_low[0].BASE,
                    pmu_floor_sleep_low[0].L_SAR
                    );
}

//-------------------------------------------------------------------
// Function: pmu_set_sleep_tsns
// Args    : None
// Description:
//           Sets PMU sleep floors for Temp Sensor operation
// Return  : None
//-------------------------------------------------------------------
static void pmu_set_sleep_tsns(){
    uint32_t index;
    if      (pmu_setting_state <= PMU_10C) index = 0;
    else if (pmu_setting_state <= PMU_25C) index = 1;
    else if (pmu_setting_state <= PMU_55C) index = 2;
    else                                   index = 3;
    pmu_set_floor(SLEEP, 
                    pmu_floor_sleep[index].R,
                    pmu_floor_sleep[index].L,
                    pmu_floor_sleep[index].BASE,
                    pmu_floor_sleep[index].L_SAR
                    );
}

//-------------------------------------------------------------------
// Function: pmu_set_sar_ratio
// Args    : ratio - desired SAR ratio
// Description:
//           Overrides SAR ratio with 'ratio', and then performs
//           sar_reset and sar/upc/dnc_stabilized tasks
// Return  : None
//-------------------------------------------------------------------
static void pmu_set_sar_ratio (uint32_t ratio) {
    //---------------------------------------------------------------------------------------
    // SAR_RATIO_OVERRIDE
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x05, // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0x3 << 14)   // 0x0      // 0x3: Perform sar_reset followed by sar/upc/dnc_stabilized upon writing into this register
                                    // 0x2: Perform sar_reset upon writing into this register
                                    // 0x1: Perform sar/upc/dnc_stabilized upon writing into this register
                                    // 0x0: Do nothing
        | (0x2 << 12)   // 0x0      // 0x3: Let VDD_CLK always connected to VBAT; 
                                    // 0x2: Let VDD_CLK always connected to V1P2;
                                    // 0x0: Let PMU handle.
        | (0x2 << 10)   // 0x2      // 0x3: Enable the periodic SAR reset; 
                                    // 0x2: Disable the periodic SAR reset; 
                                    // 0x0: Let PMU handle.
                                    // This setting is temporarily ignored when [15] is triggered.
        | (0x0 << 8 )   // 0x0      // 0x3: Let the SAR do UP-conversion (use if VBAT < 1.2V)
                                    // 0x2: Let the SAR do DOWN-conversion (use if VBAT > 1.2V)
                                    // 0x0: Let PMU handle.
        | (0x1 << 7 )   // 0x0      // If 1, override SAR ratio with [6:0].
        | (ratio)       // 0x00     // SAR ratio for overriding (valid only when [7]=1)
    ));
}

//-------------------------------------------------------------------
// Function: pmu_set_adc_period
// Args    : val - desired period for ADC VBAT measurement
// Description:
//           Sets ADC VBAT measurement to 'val'.
//           Also sets the pulse width of ADC_RESET and ADC_CLK.
// Return  : None
//-------------------------------------------------------------------
static void pmu_set_adc_period(uint32_t val){
    //---------------------------------------------------
    // TICK_REPEAT_VBAT_ADJUST
    //---------------------------------------------------
    pmu_reg_write(0x36, val); 

    //---------------------------------------------------
    // TICK_ADC_RESET
    //---------------------------------------------------
    pmu_reg_write(0x33, PMU_TICK_ADC_RESET);

    //---------------------------------------------------
    // TICK_ADC_CLK
    //---------------------------------------------------
    pmu_reg_write(0x34, PMU_TICK_ADC_CLK);
}

//-------------------------------------------------------------------
// Function: pmu_adc_read_and_sar_ratio_adjustment
// Args    : None
// Description:
//           Read the latest ADC VBAT measurement from PMU.
//           Then adjust SAR Ratio based on the VBAT measurement.
// Return  : None
//-------------------------------------------------------------------
static void pmu_adc_read_and_sar_ratio_adjustment() {

    // Read the ADC result
    read_data_batadc = pmu_reg_read(0x03) & 0xFF;

    // Adjust SAR RATIO
    if      (read_data_batadc < pmu_adc_3p0_val + 0 ){ pmu_set_sar_ratio(0x3C);} // VBAT > 3.0V
    else if (read_data_batadc < pmu_adc_3p0_val + 4 ){ pmu_set_sar_ratio(0x3F);} // 2.9V < VBAT < 3.0V
    else if (read_data_batadc < pmu_adc_3p0_val + 8 ){ pmu_set_sar_ratio(0x41);} // 2.8V < VBAT < 2.9V
    else if (read_data_batadc < pmu_adc_3p0_val + 12){ pmu_set_sar_ratio(0x43);} // 2.7V < VBAT < 2.8V
    else if (read_data_batadc < pmu_adc_3p0_val + 17){ pmu_set_sar_ratio(0x45);} // 2.6V < VBAT < 2.7V
    else if (read_data_batadc < pmu_adc_3p0_val + 21){ pmu_set_sar_ratio(0x48);} // 2.5V < VBAT < 2.6V
    else if (read_data_batadc < pmu_adc_3p0_val + 27){ pmu_set_sar_ratio(0x4B);} // 2.4V < VBAT < 2.5V
    else if (read_data_batadc < pmu_adc_3p0_val + 32){ pmu_set_sar_ratio(0x4E);} // 2.3V < VBAT < 2.4V
    else if (read_data_batadc < pmu_adc_3p0_val + 39){ pmu_set_sar_ratio(0x51);} // 2.2V < VBAT < 2.3V
    else if (read_data_batadc < pmu_adc_3p0_val + 46){ pmu_set_sar_ratio(0x56);} // 2.1V < VBAT < 2.2V
    else if (read_data_batadc < pmu_adc_3p0_val + 53){ pmu_set_sar_ratio(0x5A);} // 2.0V < VBAT < 2.1V
    else                                             { pmu_set_sar_ratio(0x5F);} // VBAT < 2.0V

}


//-------------------------------------------------------------------
// Function: pmu_init
// Args    : None
// Description:
//           Initializes the PMU's floor settings & SAR ratio
//              for the nominal condition.
//           It also disables PMU's ADC operation for Active mode
//              and sets ADC timing parameters.
// Return  : None
//-------------------------------------------------------------------
static void pmu_init(){

    // Initialize PMU State Variables
    pmu_desired_state_sleep.value  = PMU_DESIRED_STATE_SLEEP_DEFAULT;
    pmu_desired_state_active.value = PMU_DESIRED_STATE_ACTIVE_DEFAULT;
    pmu_stall_state_sleep.value    = PMU_STALL_STATE_SLEEP_DEFAULT;
    pmu_stall_state_active.value   = PMU_STALL_STATE_ACTIVE_DEFAULT;

    // Initialized the temperature to 25C and adjust floors
    pmu_setting_state = PMU_25C;
    pmu_set_active_temp_based();
    pmu_set_sleep_temp_based();

    // Initialize SAR Ratio
    pmu_set_sar_ratio(0x43);

    // Disable ADC in Active
    // PMU ADC will be automatically reset when system wakes up
    //---------------------------------------------------
    // CTRL_DESIRED_STATE_ACTIVE
    //---------------------------------------------------
    pmu_desired_state_active.adc_output_ready   = 0;
    pmu_desired_state_active.adc_adjusted       = 0;
    pmu_desired_state_active.sar_ratio_adjusted = 0;
    pmu_desired_state_active.vbat_read_only     = 0;
    pmu_reg_write(0x3C, pmu_desired_state_active.value);

    // Turn off ADC offset measurement in Sleep
    //---------------------------------------------------
    // CTRL_DESIRED_STATE_SLEEP
    //---------------------------------------------------
    pmu_desired_state_sleep.adc_adjusted       = 0;
    pmu_desired_state_sleep.vbat_read_only     = 0;
    pmu_reg_write(0x3B, pmu_desired_state_sleep.value);

    // Set ADC Period
    pmu_set_adc_period(PMU_ADC_PERIOD);

    // Horizon Behavior
    //---------------------------------------------------------------------------------------
    // HORIZON_CONFIG
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x45, // Default  // Description
    //---------------------------------------------------------------------------------------
        // NOTE: [14:11] is ignored if the corresponding DESIRED=0 -OR- STALL=1.
        //------------------------------------------------------------------------
        ( (0x1 << 14)   // 0x1      // If 1, 'horizon' enqueues 'wait_clock_count' with TICK_REPEAT_VBAT_ADJUST
        | (0x1 << 13)   // 0x1      // If 1, 'horizon' enqueues 'adjust_adc'
        | (0x1 << 12)   // 0x1      // If 1, 'horizon' enqueues 'adjust_sar_ratio'
        | (0x1 << 11)   // 0x1      // If 1, 'horizon' enqueues 'vbat_read_only'
        //------------------------------------------------------------------------
        | (0x0 << 9 )   // 0x0      // 0x0: Disable clock monitoring
                                    // 0x1: Monitoring SAR clock
                                    // 0x2: Monitoring UPC clock
                                    // 0x3: Monitoring DNC clock
        | (0x0  << 8)   // 0x0      // Reserved
        | (0x48 << 0)   // 0x48     // Sets ADC_SAMPLING_BIT in 'vbat_read_only' task
    ));

    // VOLTAGE_CLAMP_TRIM (See PMU VSOLAR SHORT BEHAVIOR section at the top)
    pmu_reg_write(0x0E, (PMU_VSOLAR_SHORT << 8) | (PMU_VSOLAR_CLAMP_BOTTOM << 4) | (PMU_VSOLAR_CLAMP_TOP << 0));

    // Enables RAT (it won't take effect until the next sleep or wakeup-up transition)
    pmu_config_rat(1);
}

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
    *GOC_DATA_IRQ = 0;
    freeze_gpio_out();
    *NVIC_ICER = 0xFFFFFFFF;
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
    //xot_disable_wreq();
    // Make sure LDO is off
    //if (snt_running){
    //    snt_running = 0;
    //    snt_temp_sensor_power_off();
    //    snt_ldo_power_off();
    //}
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
    exec_count_irq = 0; // Make sure the irq counter is reset    
    operation_prepare_sleep_notimer();
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
    //if (snt_running) {
    //    snt_running = 0;
    //    snt_state = SNT_IDLE;
    //    snt_temp_sensor_reset();
    //    snt_temp_sensor_power_off();
    //    snt_ldo_power_off();
    //}
}

//-------------------------------------------------------------------
// Function: operation_init
// Args    : None
// Description:
//           Initializes the system
// Return  : None
//-------------------------------------------------------------------
static void operation_init (void) {

    // Disable Watchdog Timers
    *TIMERWD_GO  = 0;   // Disable the CPU watchdog
    *REG_MBUS_WD = 0;   // Disable the MBus watchdog by setting Threshold=0.

    // Set the flag
    set_flag(FLAG_INITIALIZED, 1);

    //-------------------------------------------------
    // Initialize Global Variables
    // NOTE: Need to explicitly initialize global variables if the reset value is 0
    //-------------------------------------------------
    exec_count = 0;
    wakeup_count = 0;
    exec_count_irq = 0;
    pmu_adc_3p0_val = 0x62;
    read_data_batadc = pmu_adc_3p0_val + 8;   // Fresh VBAT = ~2.80V
    pmu_adc_crit_val = pmu_adc_3p0_val + 60;  // VBAT = ~1.90V
    pmu_adc_low_val  = pmu_adc_3p0_val + 53;  // VBAT = ~2.00V
    read_data_temp = 150; // From SNSv10
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
    pmu_init();

    //-------------------------------------------------
    // SNT Settings
    //-------------------------------------------------

    //// Temp Sensor Initialization
    //snt_r01.TSNS_RESETn     = 0;
    //snt_r01.TSNS_EN_IRQ     = 1;
    //snt_r01.TSNS_BURST_MODE = 0;
    //snt_r01.TSNS_CONT_MODE  = 0;
    //mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    //// Set temp sensor conversion time
    //snt_r03.TSNS_SEL_STB_TIME  = 0x1; // Default: 0x1
    //snt_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    //mbus_remote_register_write(SNT_ADDR,0x03,snt_r03.as_int);

    //-------------------------------------------------
    // MRR Settings
    //-------------------------------------------------
    // NOTE: XT1 does not use MRR

    //-------------------------------------------------
    // EID Settings
    //-------------------------------------------------
    // NOTE: eid_init() does not start the EID. It just configures its register file.
    //eid_init();

    //-------------------------------------------------
    // XO Driver
    //-------------------------------------------------
    //xo_start();

    //-------------------------------------------------
    // E-Ink Display
    //-------------------------------------------------
    //eid_update(DISP_INITIALIZED);

    //-------------------------------------------------
    // NFC 
    //-------------------------------------------------
    //nfc_init();

    //-------------------------------------------------
    // Sleep
    //-------------------------------------------------
    // It must go into sleep with a long 'sleep' duration
    // to give user enough time to put a "sticker" over the GOC solar cell.
    //reset_xo_cnt(); // Make counter value = 0
    //operation_sleep_xo_timer(SLEEP_DURATION_LONG);
}

//-------------------------------------------------------------------
// Application-Specific Functions
//-------------------------------------------------------------------

// FAIL: Unexpected Behavior
static void fail (uint32_t code) {
    mbus_write_message32(FAIL_MBUS_ADDR, code);
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

void handler_ext_int_timer32(void) { // TIMER32
    mbus_write_message32(0x71, 0);
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_TIMER32, 0x00000000);
    #endif
    *REG1 = *TIMER32_CNT;
    *REG2 = *TIMER32_STAT;
    *TIMER32_STAT = 0x0;
    wfi_timeout_flag = 1;
    set_halt_until_mbus_tx();
    }
void handler_ext_int_timer16(void) { // TIMER16
    mbus_write_message32(0x71, 2);
    *NVIC_ICPR = (0x1 << IRQ_TIMER16);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_TIMER16, 0x00000000);
    #endif
    }
void handler_ext_int_reg0(void) { // REG0
    mbus_write_message32(0x71, 3);
    *NVIC_ICPR = (0x1 << IRQ_REG0);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG0, 0x00000000);
    #endif
}
void handler_ext_int_reg1(void) { // REG1
    mbus_write_message32(0x71, 4);
    *NVIC_ICPR = (0x1 << IRQ_REG1);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG1, 0x00000000);
    #endif
}
void handler_ext_int_reg2(void) { // REG2
    mbus_write_message32(0x71, 5);
    *NVIC_ICPR = (0x1 << IRQ_REG2);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG2, 0x00000000);
    #endif
}
void handler_ext_int_reg3(void) { // REG3
    mbus_write_message32(0x71, 6);
    *NVIC_ICPR = (0x1 << IRQ_REG3);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG3, 0x00000000);
    #endif
}
void handler_ext_int_reg4(void) { // REG4
    mbus_write_message32(0x71, 7);
    *NVIC_ICPR = (0x1 << IRQ_REG4);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG4, 0x00000000);
    #endif
}
void handler_ext_int_reg5(void) { // REG5
    mbus_write_message32(0x71, 8);
    *NVIC_ICPR = (0x1 << IRQ_REG5);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG5, 0x00000000);
    #endif
}
void handler_ext_int_reg6(void) { // REG6
    mbus_write_message32(0x71, 9);
    *NVIC_ICPR = (0x1 << IRQ_REG6);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG6, 0x00000000);
    #endif
}
void handler_ext_int_reg7(void) { // REG7
    mbus_write_message32(0x71, 0xA);
    *NVIC_ICPR = (0x1 << IRQ_REG7);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG7, 0x00000000);
    #endif
}
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    mbus_write_message32(0x71, 0xB);
    *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_MBUS_MEM, 0x00000000);
    #endif
}
void handler_ext_int_mbusrx(void) { // MBUS_RX
    mbus_write_message32(0x71, 0xC);
    *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_MBUS_RX, 0x00000000);
    #endif
}
void handler_ext_int_mbustx(void) { // MBUS_TX
    mbus_write_message32(0x71, 0xD);
    *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_MBUS_TX, 0x00000000);
    #endif
}
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    mbus_write_message32(0x71, 0xE);
    *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_MBUS_FWD, 0x00000000);
    #endif
}
void handler_ext_int_gocep(void) { // GOCEP
    mbus_write_message32(0x71, 0xF);
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_GOCEP, 0x00000000);
    #endif

    if (*GOC_DATA_IRQ == 0x0) {
//        xo_start();
//        *XOT_START_COUT = 1;
//        operation_sleep();
//        while(1);
    }
    else if (*GOC_DATA_IRQ == 0x1) {
//        *XOT_STOP_COUT = 1;
//        xo_stop();
//        operation_sleep();
//        while(1);
    }
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    mbus_write_message32(0x71, 0x10);
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_SOFT_RESET, 0x00000000);
    #endif
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    mbus_write_message32(0x71, 0x11);
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_WAKEUP, (0x10 << 24) | wakeup_source);
    #endif
}
void handler_ext_int_aes(void) { // AES
    mbus_write_message32(0x71, 0x12);
    *NVIC_ICPR = (0x1 << IRQ_AES);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_SPI, 0x00000000);
    #endif
}
void handler_ext_int_spi(void) { // SPI
    mbus_write_message32(0x71, 0x13);
    *NVIC_ICPR = (0x1 << IRQ_SPI);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_SPI, 0x00000000);
    #endif
}
void handler_ext_int_gpio(void) { // GPIO
    mbus_write_message32(0x71, 0x14);
    *NVIC_ICPR = (0x1 << IRQ_GPIO);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_GPIO, 0x00000000);
    #endif
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    mbus_write_message32(0x70, 1);
    
    // Get the info on who woke up the system
    wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1; // reset WAKEUP_SOURCE register

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_GOCEP) | (0x1 << IRQ_TIMER32) | (0x1 << IRQ_REG0  ) | (0x1 << IRQ_REG1 ) | (0x1 << IRQ_REG2  ) | (0x1 << IRQ_REG3 );

    // If this is the very first wakeup, initialize the system and go back to sleep
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    // Go to Sleep
    set_wakeup_timer(/*timestamp*/ 10, /*irq_en*/ 1, /*reset*/ 1);
    operation_sleep();

    // Never Quit (should not stay here for an extended duration)
    while(1) asm("nop");
    return 1;
}
