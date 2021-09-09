//*******************************************************************************************
// XT1 (TMCv1) FIRMWARE
// Version alpha-0.1
//-------------------------------------------------------------------------------------------
// TMCv1 SUB-LAYER CONNECTION:
//      PREv22E -> SNTv5 -> EIDv1 -> MRRv11A -> MEMv3 -> PMUv13
//-------------------------------------------------------------------------------------------
// External Connections
//
//  XT1         NFC (ST25DVxxx)
//  ---------------------------
//  GPIO[0]     GPO
//  GPIO[1]     SCL
//  GPIO[2]     SDA
//  VPG2_OUT    VCC
//      
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
// - Any code to handle general tasks in handler_ext_int_gocep?
// - In snt_operation(), add code to store the temp & VBAT data with timestamp
// - The flag bit @ FLAG_USE_REAL_TIME must be set to 0. PREv22E (in TMCv1) automatically
//      resets the XO counter to 0 upon wakeup. This needs to be changed in the next version.
//******************************************************************************************* 

#include "TMCv1.h"

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
#define USE_SHORT_DELAY     // Use short delays for quick simulations. Comment it out for real silicon testing.

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

// SNT states
#define SNT_IDLE        0x0
#define SNT_LDO         0x1
#define SNT_TEMP_START  0x2
#define SNT_TEMP_READ   0x6

// System Operation Mode
#define ACTIVE      0x1
#define SLEEP       0x0

//*******************************************************************************************
// E-INK DISPLAY PATTERN
//-------------------------------------------------------------------------------------------
// The display will become all-white before displaying patterns specified.
// For n=0,...,8, Bit[n]=1 makes SEG[n] black.
//*******************************************************************************************
#define DISP_INITIALIZED 0x001  // After intialization (At the end of operation_init())
#define DISP_NORMAL      0x002  // Normal Operation (after GIT)
#define DISP_LOW_VBAT    0x003  // Low VBAT Level (just a warning)
#define DISP_LOW_TEMP    0x004  // Low Temperature Detected
#define DISP_HIGH_TEMP   0x005  // High Temperature Detected

//*******************************************************************************************
// NFC CONFIGURATIONS
//*******************************************************************************************

// COTS Power Switch
#define NFC_CPS 2

// GPIO Interface
#define NFC_GPO 0
#define NFC_SCL 1
#define NFC_SDA 2
#define I2C_MASK (1<<NFC_SDA)|(1<<NFC_SCL)
#define I2C_SCL_MASK (1<<NFC_SCL)
#define I2C_SDA_MASK (1<<NFC_SDA)
#define GPO_MASK (1<<NFC_GPO)

// ACK Timeout
#define I2C_ACK_TIMEOUT 50000   // Checked using TIMER32

// GPO Edge Sensitivity (At least one of the below must be enabled; Both can be enabled.)
#define USE_GPO_POSEDGE
//#define USE_GPO_NEGEDGE

//*******************************************************************************************
// XO AND SLEEP DURATIONS
//*******************************************************************************************

// XO Initialization Wait Duration
#ifndef USE_SHORT_DELAY
    #define XO_WAIT_A  20000    // Must be ~1 second delay. Delay for XO Start-Up. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
    #define XO_WAIT_B  20000    // Must be ~1 second delay. Delay for VLDO & IBIAS Generation. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#else
    #define XO_WAIT_A  100      // Short delay for simulation
    #define XO_WAIT_B  100      // Short delay for simulation
#endif

// XO Counter Value per Specific Time Durations
#define XOT_1SEC    32765
#define XOT_1MIN    60*XOT_1SEC
#define XOT_1HR     60*XOT_1MIN
#define XOT_1DAY    24*XOT_1HR

// Sleep Duration 
#ifndef USE_SHORT_DELAY
    #define SLEEP_DURATION_LONG      1*XOT_1HR      // The long sleep duration during which the user must put on a GIT sticker
    #define SLEEP_DURATION_PREGIT   10*XOT_1MIN     // The sleep duration before activating the system
    #define SLEEP_DURATION          10*XOT_1MIN     // The sleep duration after the system activation
#else
    #define SLEEP_DURATION_LONG      1*XOT_1SEC     // The long sleep duration during which the user must put on a GIT sticker
    #define SLEEP_DURATION_PREGIT    1*XOT_1SEC     // The sleep duration before activating the system
    #define SLEEP_DURATION           1*XOT_1SEC     // The sleep duration after the system activation
#endif


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
// EID LAYER CONFIGURATION
//*******************************************************************************************

// Timer Start-up Delay
#ifndef USE_SHORT_DELAY
    #define EID_TIMER_WAIT  60000   // Must be >3 seconds delay. Delay between TMR_SELF_EN=1 and TMR_EN_OSC=0. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#else
    #define EID_TIMER_WAIT  100     // Short delay for simulation
#endif

// Charge Pumps Tuning
#define EID_SEL_RING    0x0     // (Default: 0x0; Max: 0x3) Selects # of rings. 0x0: 11 stages; 0x3: 5 stages.
#define EID_SEL_TE_DIV  0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Top Electrode (TE) charge pump.
#define EID_SEL_FD_DIV  0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Field (FD) charge pump.
#define EID_SEL_SEG_DIV 0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Segment (SEG) charge pumps.

// Charge Pump Activation Duration (Normal Use)
#ifndef USE_SHORT_DELAY
    #define EID_PULSE_WIDTH 400     // (Default: 400; Max: 65535) Duration of Charge Pump Activation. LSB corresponds to ~5ms, assuming a 3kHz clock (i.e., LSB = clock_period * 16)
#else
    #define EID_PULSE_WIDTH 40      // Short delay for simulation
#endif

//--- Crash Handling
//--------------------------------------------------------
//
// X: CRSH_IDLE_WIDTH
// Y: CRSH_PULSE_WIDTH
//
//    crash detected
//           v
//                  <-Seq A->       <-Seq B->       <-Seq C->
//  VOUT_*   _______|*******|_______|*******|_______|*******|_______________
// Duration  <--X--> <--Y--> <--X--> <--Y--> <--X--> <--Y--> <--X-->
// dead      _______________________________________________________|*******
// PG_DIODE  <--A0-> <--A--> <--A1-> <--B--> <--B1-> <--C--> <--C1->|*******
//
// PG_DIODE  *******|_______________________________________________|*******
// (intended)
//
//////////////////////////////////////////////////////////
#define CRSH_WD_THRSHLD     360000  // (Default: 360,000, Max:16,777,215) Crash Watchdog Threshold. LSB corresponds to ~5ms, assuming a 3kHz clock (i.e., LSB = clock_period * 16)
#define CRSH_SEL_RING       0x0     // (Default: 0x0; Max: 0x3) Selects # of rings. 0x0: 11 stages; 0x3: 5 stages.
#define CRSH_SEL_TE_DIV     0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Top Electrode (TE) charge pump.
#define CRSH_SEL_FD_DIV     0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Field (FD) charge pump.
#define CRSH_SEL_SEG_DIV    0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Segment (SEG) charge pumps.

#ifndef USE_SHORT_DELAY
    #define CRSH_IDLE_WIDTH     400     // (Default: 400; Max: 65535) Duration of Charge Pump Activation. LSB corresponds to ~5ms, assuming a 3kHz clock (i.e., LSB = clock_period * 16)
    #define CRSH_PULSE_WIDTH    400     // (Default: 400; Max: 65535) Duration of Charge Pump Activation. LSB corresponds to ~5ms, assuming a 3kHz clock (i.e., LSB = clock_period * 16)
#else
    #define CRSH_IDLE_WIDTH     40      // Short delay for simulation
    #define CRSH_PULSE_WIDTH    40      // Short delay for simulation
#endif
#define CRSH_SEL_SEQ        0x1     // See above waveform. Each bit in [2:0] enables/disables Seq C, Seq B, Seq A, respectively. 
                                    // If 1, the corresponding sequence is enabled. If 0, the corresponding sequence is skipped.

//--- Sequence A (valid only if CRSH_SEL_SEQ[0]=1)
#define CRSH_SEQA_VIN       0x0     // Use 0 if VBAT is high; Use 1 if VBAT is low.
#define CRSH_SEQA_CK_TE     0x0     // CK Value for Top Electrode (TE)
#define CRSH_SEQA_CK_FD     0x0     // CK Value for Field (FD)
#define CRSH_SEQA_CK_SEG    0x000   // CK Value for Segments (SEG[8:0])

//--- Sequence B (valid only if CRSH_SEL_SEQ[1]=1)
#define CRSH_SEQB_VIN       0x0     // Use 0 if VBAT is high; Use 1 if VBAT is low.
#define CRSH_SEQB_CK_TE     0x0     // CK Value for Top Electrode (TE)
#define CRSH_SEQB_CK_FD     0x0     // CK Value for Field (FD)
#define CRSH_SEQB_CK_SEG    0x000   // CK Value for Segments (SEG[8:0])

//--- Sequence C (valid only if CRSH_SEL_SEQ[2]=1)
#define CRSH_SEQC_VIN       0x0     // Use 0 if VBAT is high; Use 1 if VBAT is low.
#define CRSH_SEQC_CK_TE     0x0     // CK Value for Top Electrode (TE)
#define CRSH_SEQC_CK_FD     0x0     // CK Value for Field (FD)
#define CRSH_SEQC_CK_SEG    0x000   // CK Value for Segments (SEG[8:0])

//--------- <DO NOT CHANGE> ------------------------------------------------------------------
#define CRSH_PG_DIODE_A0    0x1
#define CRSH_PG_DIODE_A     0x0
#define CRSH_PG_DIODE_A1    0x0
#define CRSH_PG_DIODE_B     0x0
#define CRSH_PG_DIODE_B1    0x0
#define CRSH_PG_DIODE_C     0x0
#define CRSH_PG_DIODE_C1    0x0
//--------------------------------------------------------------------------------------------


//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************
volatile uint32_t state;
volatile uint32_t snt_state;
volatile uint32_t wfi_timeout_flag;
volatile uint32_t exec_count;
volatile uint32_t wakeup_count;
volatile uint32_t exec_count_irq;
volatile uint32_t pmu_setting_state;
volatile uint32_t snt_running;
volatile uint32_t meas_count;
volatile uint32_t read_data_temp;
volatile uint32_t snt_high_temp_threshold;
volatile uint32_t snt_low_temp_threshold;
volatile uint32_t read_data_batadc;
volatile uint32_t read_data_batadc_diff;
volatile uint32_t pmu_adc_3p0_val;
volatile uint32_t pmu_adc_low_val;      // Low VBAT level which displays the warning
volatile uint32_t pmu_adc_crit_val;     // Critical VBAT level which triggers the EID Watchdog
volatile uint32_t eid_vin_threshold;    // EID's VIN threshold. PMU ADC values @ VBAT = 2.5V
volatile uint32_t wakeup_timestamp;
volatile uint32_t wakeup_timestamp_rollover;
volatile uint32_t realtime_offset; // Difference btw the wall time and XO Timer counter value. Need to be updated by user. 

volatile pre_r0B_t pre_r0B = PRE_R0B_DEFAULT;
volatile snt_r00_t snt_r00 = SNT_R00_DEFAULT;
volatile snt_r01_t snt_r01 = SNT_R01_DEFAULT;
volatile snt_r03_t snt_r03 = SNT_R03_DEFAULT;
volatile eid_r00_t eid_r00 = EID_R00_DEFAULT;
volatile eid_r01_t eid_r01 = EID_R01_DEFAULT;
volatile eid_r02_t eid_r02 = EID_R02_DEFAULT;
volatile eid_r07_t eid_r07 = EID_R07_DEFAULT;
volatile eid_r08_t eid_r08 = EID_R08_DEFAULT;
volatile eid_r09_t eid_r09 = EID_R09_DEFAULT;
volatile eid_r10_t eid_r10 = EID_R10_DEFAULT;
volatile eid_r11_t eid_r11 = EID_R11_DEFAULT;
volatile eid_r12_t eid_r12 = EID_R12_DEFAULT;
volatile eid_r13_t eid_r13 = EID_R13_DEFAULT;
volatile eid_r14_t eid_r14 = EID_R14_DEFAULT;
volatile eid_r15_t eid_r15 = EID_R15_DEFAULT;
volatile eid_r16_t eid_r16 = EID_R16_DEFAULT;
volatile eid_r17_t eid_r17 = EID_R17_DEFAULT;

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//-- PRE Functions
static void set_timer32_timeout(uint32_t val);
static void stop_timer32_timeout_check(uint32_t code);
static void xot_enable(uint32_t timestamp);
static void xot_disable_wreq(void);
static void xo_start(void);
static void xo_stop(void);

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

//-- SNT Functions
static void snt_temp_sensor_power_on(void);
static void snt_temp_sensor_power_off(void);
static void snt_temp_sensor_start(void);
static void snt_temp_sensor_reset(void);
static void snt_ldo_power_off(void);
static void snt_ldo_vref_on(void);
static void snt_ldo_power_on(void);

//-- EID Functions
static void eid_enable_timer(void);
static void eid_set_pulse_width(uint32_t pulse_width);
static void eid_enable_cp_ck(uint32_t vin, uint32_t te, uint32_t fd, uint32_t seg);
static void eid_all_black(uint32_t vin);
static void eid_all_white(uint32_t vin);
static void eid_seg_black(uint32_t vin, uint32_t seg);
static void eid_seg_white(uint32_t vin, uint32_t seg);
static void eid_update(uint32_t seg);
static void eid_check_in(void);
static void eid_trigger_crash(void);
static void eid_init(void);

//-- NFC Functions
static void nfc_unfreeze_gpio(void);
static void nfc_init(void);
static void nfc_i2c_start(void);
static void nfc_i2c_stop(void);
static void nfc_i2c_cmd(uint8_t byte);
static uint8_t nfc_i2c_rd(uint8_t ack);
static void nfc_i2c_byte_write(uint32_t addr, uint8_t data);
static void nfc_i2c_seq_write(uint32_t addr, uint32_t data[], uint32_t len);
static void nfc_i2c_seq_word_write(uint32_t addr, uint32_t data[], uint32_t len);

//-- Initialization/Sleep Functions
static void operation_sleep (void);
static void operation_prepare_sleep_notimer(void);
static void operation_sleep_notimer(void);
static void operation_sleep_xo_timer(uint32_t timestamp);
static void operation_back_to_default(void);
static void operation_init (void);

//-- Application Specific
static void snt_operation (void);
static void postgit_init(void);
static void do_nfc(void);
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
// Function: xot_enable
// Args    : timestamp - Timestamp value at which the XO timer generates 
//                       a wakeup request
// Description:
//           Changes the XO Timer's threshold value using timestamp.
//           This function assumes that XO Driver and XO Timer have 
//           been already properly started.
// Return  : None
//-------------------------------------------------------------------
static void xot_enable (uint32_t timestamp) {
    *REG_XOT_CONFIGU = (timestamp >> 16) & 0xFFFF;
    *REG_XOT_CONFIG  = (timestamp & 0x0000FFFF) | 0x00A00000; // [23] XOT_ENABLE = 1; [21] XOT_WREQ_EN = 1;
}

//-------------------------------------------------------------------
// Function: xot_disable_wreq
// Args    : None
// Description:
//           Make XOT_WREQ_EN = 0, which prevents XO Wakeup Timer
//           from generating a wakeup request.
//           The XO Wakeup Timer running status is NOT affected 
//           by this function.
// Return  : None
//-------------------------------------------------------------------
static void xot_disable_wreq (void) {
    *REG_XOT_CONFIG  = *REG_XOT_CONFIG & 0xFFDFFFFF; // [21] XOT_WREQ_EN = 0;
}

//-------------------------------------------------------------------
// Function: xo_start
// Args    : None
// Description:
//           Start the XO Driver and the XO Wakeup Timer
// Return  : None
//-------------------------------------------------------------------
static void xo_start( void ) {

    //--------------------------------------------------------------------------
    // XO Driver (XO_DRV_V3_TSMC180) Start-Up Sequence
    //--------------------------------------------------------------------------
    // RESETn       __|*********************************************************
    // PGb_StartUp  __|***************************|_____________________________
    // START_UP     **************************|_________________________________
    // ISOL_CLK_HP  **********|_________________|*******************************
    // ISOL_CLK_LP  ******************|_________________________________________
    //                |<--A-->|<--B-->|<--C-->|.|.|<-- Low Power Operation -->
    //--------------------------------------------------------------------------
    // A: ~1s  (XO Start-Up): NOTE: You may need more time here due to the weak power-gate switch.
    // B: ~1s  (VLDO & IBIAS generation)
    // C: <1ms (SCN Output Generation)
    // .(dot): minimum delay
    //--------------------------------------------------------------------------

    pre_r19_t xo_control = PRE_R19_DEFAULT; // REG_XO_CONF1

    xo_control.XO_RESETn       = 1;
    xo_control.XO_PGb_START_UP = 1;
    *REG_XO_CONF1 = xo_control.as_int;

    delay(XO_WAIT_A); // Delay A (~1s; XO Start-Up)

    xo_control.XO_ISOL_CLK_HP = 0;
    *REG_XO_CONF1 = xo_control.as_int;
    
    delay(XO_WAIT_B); // Delay B (~1s; VLDO & IBIAS generation)

    xo_control.XO_ISOL_CLK_LP = 0;
    *REG_XO_CONF1 = xo_control.as_int;
    
    delay(100); // Delay C (~1ms; SCN Output Generation)

    xo_control.XO_START_UP = 0;
    *REG_XO_CONF1 = xo_control.as_int;
    
    xo_control.XO_ISOL_CLK_HP = 1;
    *REG_XO_CONF1 = xo_control.as_int;

    xo_control.XO_PGb_START_UP = 0;
    *REG_XO_CONF1 = xo_control.as_int;

    delay(100); // Dummy Delay

    // Start XO Wakeup Timer
    enable_xo_timer();
    start_xo_cnt();
}

//-------------------------------------------------------------------
// Function: xo_stop
// Args    : None
// Description:
//           Stop the XO Driver and the XO Wakeup Timer
// Return  : None
//-------------------------------------------------------------------
static void xo_stop( void ) {
    // Stop the XO Driver
    pre_r19_t xo_control;
    xo_control.as_int = *REG_XO_CONF1;

    xo_control.XO_ISOL_CLK_LP = 1;
    *REG_XO_CONF1 = xo_control.as_int;

    xo_control.XO_RESETn   = 0;
    xo_control.XO_START_UP = 1;
    *REG_XO_CONF1 = xo_control.as_int;

    // Stop the XO Wakeup Timer
    disable_xo_timer();
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

    #ifdef PMU_CHECK_WRITE // See PMU Behavior section at the top)
        //[5]: PMU_CHECK_WRITE(1'h0); [4]: PMU_IRQ_EN(1'h1); [3:2]: LC_CLK_DIV(2'h3); [1:0]: LC_CLK_RING(2'h1)
        mbus_remote_register_write(PMU_ADDR, 0x51, (0x1 << 5) | (0x1 << 4) | (0x3 << 2) | (0x1 << 0));
    #endif

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
// SNT Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: snt_temp_sensor_power_on
// Args    : None
// Description:
//           Power on SNT Temp Sensor
// Return  : None
//-------------------------------------------------------------------
static void snt_temp_sensor_power_on(void){
    // Turn on digital block
    snt_r01.TSNS_SEL_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
    // Turn on analog block
    snt_r01.TSNS_EN_SENSOR_LDO = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
    // Delay (~2ms @ 100kHz clock speed)
    delay(200);
    // Release isolation
    snt_r01.TSNS_ISOLATE = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
    // Delay (~2ms @ 100kHz clock speed)
    delay(200);
}

//-------------------------------------------------------------------
// Function: snt_temp_sensor_power_off
// Args    : None
// Description:
//           Turns off SNT Temp Sensor
// Return  : None
//-------------------------------------------------------------------
static void snt_temp_sensor_power_off(void){
    snt_r01.TSNS_RESETn         = 0;
    snt_r01.TSNS_SEL_LDO        = 0;
    snt_r01.TSNS_EN_SENSOR_LDO  = 0;
    snt_r01.TSNS_ISOLATE        = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

//-------------------------------------------------------------------
// Function: snt_temp_sensor_start
// Args    : None
// Description:
//           Release the reset of SNT Temp Sensor
// Return  : None
//-------------------------------------------------------------------
static void snt_temp_sensor_start(void){
    snt_r01.TSNS_RESETn = 1;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

//-------------------------------------------------------------------
// Function: snt_temp_sensor_reset
// Args    : None
// Description:
//           Assert the reset of SNT Temp Sensor
// Return  : None
//-------------------------------------------------------------------
static void snt_temp_sensor_reset(void){
    snt_r01.TSNS_RESETn = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);
}

//-------------------------------------------------------------------
// Function: snt_ldo_power_off
// Args    : None
// Description:
//           Turns off SNT LDO
// Return  : None
//-------------------------------------------------------------------
static void snt_ldo_power_off(void){
    snt_r00.LDO_EN_VREF   = 0;
    snt_r00.LDO_EN_IREF   = 0;
    snt_r00.LDO_EN_LDO    = 0;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
}

//-------------------------------------------------------------------
// Function: snt_ldo_vref_on
// Args    : None
// Description:
//           Turns on SNT VREF
// Return  : None
//-------------------------------------------------------------------
static void snt_ldo_vref_on(void){
    snt_r00.LDO_EN_VREF = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
    // Delay (~50ms @ 100kHz clock speed); NOTE: Start-Up Time of VREF @ TT, 1.1V, 27C is ~40ms.
    delay(5000);
}

//-------------------------------------------------------------------
// Function: snt_ldo_power_on
// Args    : None
// Description:
//           Turns on SNT LDO
// Return  : None
//-------------------------------------------------------------------
static void snt_ldo_power_on(void){
    snt_r00.LDO_EN_IREF = 1;
    snt_r00.LDO_EN_LDO  = 1;
    mbus_remote_register_write(SNT_ADDR,0x00,snt_r00.as_int);
}


//-------------------------------------------------------------------
// EID Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: eid_enable_timer
// Args    : None
// Description:
//           Enable the EID timer
// Return  : None
//-------------------------------------------------------------------
static void eid_enable_timer(void){
    eid_r01.TMR_SELF_EN = 0;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_EN_OSC = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_RESETB = 1;
    eid_r01.TMR_RESETB_DIV = 1;
    eid_r01.TMR_RESETB_DCDC = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_EN_SELF_CLK = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_SELF_EN = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    delay(EID_TIMER_WAIT); // Wait for >3s
    eid_r01.TMR_EN_OSC = 0;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_SEL_LDO = 1;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_ISOL_CLK = 0;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r00.TMR_EN_CLK_DIV = 1;
    mbus_remote_register_write(EID_ADDR,0x00,eid_r00.as_int);
}

//-------------------------------------------------------------------
// Function: eid_set_pulse_width
// Args    : pulse_width - Duration of Charge Pump Activation 
//                          (Default: 400; Max: 65535)
//                          LSB corresponds to ~5ms, assuming a 3kHz clock (i.e., LSB = clock_period * 16)
// Description:
//           Set the duration of the charge pump activation 
// Return  : None
//-------------------------------------------------------------------
static void eid_set_pulse_width(uint32_t pulse_width){
    eid_r07.ECTR_PULSE_WIDTH = pulse_width;
    mbus_remote_register_write(EID_ADDR,0x07,eid_r07.as_int);
}

//-------------------------------------------------------------------
// Function: eid_enable_cp_ck
// Args    : vin - Use 0 if VBAT is high; 
//                 Use 1 if VBAT is low
//           te  - If 1, Top Electrode (TE) charge pump output goes high (~15V).
//                 If 0, Top Electrode (TE) charge pump output remains low at 0V.
//           fd  - If 1, Field (FD) charge pump output goes high (~15V).
//                 If 0, Field (FD) charge pump output remains low at 0V.
//           seg - For n=0,1,...,8:
//                 If bit[n]=1, SEG[n] charge pump output goes high (~15V).
//                 If bit[n]=0, SEG[n] charge pump output remains low at 0V.
// Description:
//           Enable the clock for the specified charge pumps
//              to make the outputs go high (~15V)
// Return  : None
//-------------------------------------------------------------------
static void eid_enable_cp_ck(uint32_t vin, uint32_t te, uint32_t fd, uint32_t seg) {
    uint32_t cp_ck = ((te << 10) | (fd << 9) | (seg << 0)) & 0x1FF;
    uint32_t cp_pd = (~cp_ck) & 0x1FF;

    // Make PG_DIODE=0
    eid_r02.ECP_PG_DIODE = 0;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);

    // Enable charge pumps
    eid_r09.ECTR_RESETB_CP = 1;
    eid_r09.ECTR_VIN_CP    = vin;
    eid_r09.ECTR_EN_CP_PD  = cp_pd;
    eid_r09.ECTR_EN_CP_CK  = cp_ck;
    set_halt_until_mbus_trx();
    mbus_remote_register_write(EID_ADDR,0x09,eid_r09.as_int);
    set_halt_until_mbus_tx();

    // Make PG_DIODE=1
    eid_r02.ECP_PG_DIODE = 1;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);
}

//-------------------------------------------------------------------
// Function: eid_all_black
// Args    : vin - Use 0 if VBAT is high; 
//                 Use 1 if VBAT is low
// Description:
//           Make the entire display black
// Return  : None
//-------------------------------------------------------------------
static void eid_all_black(uint32_t vin) { 
    eid_enable_cp_ck(vin, 0x0, 0x1, 0x1FF); 
}

//-------------------------------------------------------------------
// Function: eid_all_white
// Args    : vin - Use 0 if VBAT is high; 
//                 Use 1 if VBAT is low
// Description:
//           Make the entire display white
// Return  : None
//-------------------------------------------------------------------
static void eid_all_white(uint32_t vin) { 
    eid_enable_cp_ck(vin, 0x1, 0x0, 0x000); 
}

//-------------------------------------------------------------------
// Function: eid_seg_black
// Args    : vin - Use 0 if VBAT is high; 
//                 Use 1 if VBAT is low
//           seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] black.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the selected segment(s) black.
//           NOTE: Field (FD) must be white first.
// Return  : None
//-------------------------------------------------------------------
static void eid_seg_black(uint32_t vin, uint32_t seg) { 
    eid_enable_cp_ck(vin, 0x0, 0x0, seg);
}

//-------------------------------------------------------------------
// Function: eid_seg_white
// Args    : vin - Use 0 if VBAT is high; 
//                 Use 1 if VBAT is low
//           seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] white.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the selected segment(s) white.
//           NOTE: Field (FD) must be black first.
// Return  : None
//-------------------------------------------------------------------
static void eid_seg_white(uint32_t vin, uint32_t seg) { 
    eid_enable_cp_ck(vin, 0x1, 0x1, (~seg & 0x1FF));
}

//-------------------------------------------------------------------
// Function: eid_update
// Args    : seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] black.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the entire display white,
//           and then make the selected segment(s) black.
// Return  : None
//-------------------------------------------------------------------
static void eid_update(uint32_t seg) { 
    // vin = 0 if VBAT is high (> ~2.5V)
    //       1 if VBAT is low  (< ~2.5V)
    uint32_t vin = (read_data_batadc < eid_vin_threshold) ? 0x0 : 0x1;
    eid_all_white(vin);
    eid_seg_black(vin, seg);
}

//-------------------------------------------------------------------
// Function: eid_check_in
// Args    : None
// Description:
//           EID Watchdog Check-In.
//           Must be done before the watchdog expires.
// Return  : None
//-------------------------------------------------------------------
static void eid_check_in() {
    mbus_remote_register_write(EID_ADDR,0x18,0x000000);
}

//-------------------------------------------------------------------
// Function: eid_trigger_crash
// Args    : None
// Description:
//           Trigger the crash behavior in EID
// Return  : None
//-------------------------------------------------------------------
static void eid_trigger_crash() {
    mbus_remote_register_write(EID_ADDR,0x19,0x00DEAD);
}

//-------------------------------------------------------------------
// Function: eid_init
// Args    : None
// Description:
//           Initializes the EID layer
// Return  : None
//-------------------------------------------------------------------
static void eid_init(void){
    // Start EID Timer 
    eid_enable_timer();

    // Charge Pump Tuning
    eid_r02.ECP_PG_DIODE    = 1;
    eid_r02.ECP_SEL_RING    = EID_SEL_RING;
    eid_r02.ECP_SEL_TE_DIV  = EID_SEL_TE_DIV;
    eid_r02.ECP_SEL_FD_DIV  = EID_SEL_FD_DIV;
    eid_r02.ECP_SEL_SEG_DIV = EID_SEL_SEG_DIV;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);

    // Charge Pump Pulse Width
    eid_set_pulse_width(EID_PULSE_WIDTH);

    // Configuration for Crash Behavior (See comment section in the top)
    eid_r10.WCTR_THRESHOLD = CRSH_WD_THRSHLD;
    mbus_remote_register_write(EID_ADDR,0x10,eid_r10.as_int);

    eid_r11.WCTR_CR_ECP_SEL_SEG_DIV     = CRSH_SEL_SEG_DIV;
    eid_r11.WCTR_CR_ECP_SEL_FD_DIV      = CRSH_SEL_FD_DIV;
    eid_r11.WCTR_CR_ECP_SEL_TE_DIV      = CRSH_SEL_TE_DIV;
    eid_r11.WCTR_CR_ECP_SEL_RING        = CRSH_SEL_RING;
    eid_r11.WCTR_CR_ECP_PG_DIODE_C1     = CRSH_PG_DIODE_C1;
    eid_r11.WCTR_CR_ECP_PG_DIODE_C      = CRSH_PG_DIODE_C;
    eid_r11.WCTR_CR_ECP_PG_DIODE_B1     = CRSH_PG_DIODE_B1;
    eid_r11.WCTR_CR_ECP_PG_DIODE_B      = CRSH_PG_DIODE_B;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A1     = CRSH_PG_DIODE_A1;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A      = CRSH_PG_DIODE_A;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A0     = CRSH_PG_DIODE_A0;
    mbus_remote_register_write(EID_ADDR,0x11,eid_r11.as_int);

    eid_r12.WCTR_IDLE_WIDTH = CRSH_IDLE_WIDTH;
    eid_r12.WCTR_SEL_SEQ    = CRSH_SEL_SEQ;
    mbus_remote_register_write(EID_ADDR,0x12,eid_r12.as_int);

    eid_r13.WCTR_PULSE_WIDTH    = CRSH_PULSE_WIDTH;
    mbus_remote_register_write(EID_ADDR,0x13,eid_r13.as_int);

    eid_r15.WCTR_RESETB_CP_A = 1;
    eid_r15.WCTR_VIN_CP_A    = CRSH_SEQA_VIN;
    eid_r15.WCTR_EN_CP_CK_A  =   ((CRSH_SEQA_CK_TE << 10) | (CRSH_SEQA_CK_FD << 9) | (CRSH_SEQA_CK_SEG << 0)) & 0x1FF;
    eid_r15.WCTR_EN_CP_PD_A  = ~(((CRSH_SEQA_CK_TE << 10) | (CRSH_SEQA_CK_FD << 9) | (CRSH_SEQA_CK_SEG << 0)) & 0x1FF) & 0x1FF;
    mbus_remote_register_write(EID_ADDR,0x15,eid_r15.as_int);

    eid_r16.WCTR_RESETB_CP_B = 1;
    eid_r16.WCTR_VIN_CP_B    = CRSH_SEQB_VIN;
    eid_r16.WCTR_EN_CP_CK_B  =   ((CRSH_SEQB_CK_TE << 10) | (CRSH_SEQB_CK_FD << 9) | (CRSH_SEQB_CK_SEG << 0)) & 0x1FF;
    eid_r16.WCTR_EN_CP_PD_B  = ~(((CRSH_SEQB_CK_TE << 10) | (CRSH_SEQB_CK_FD << 9) | (CRSH_SEQB_CK_SEG << 0)) & 0x1FF) & 0x1FF;
    mbus_remote_register_write(EID_ADDR,0x16,eid_r16.as_int);

    eid_r17.WCTR_RESETB_CP_C = 1;
    eid_r17.WCTR_VIN_CP_C    = CRSH_SEQC_VIN;
    eid_r17.WCTR_EN_CP_CK_C  =   ((CRSH_SEQC_CK_TE << 10) | (CRSH_SEQC_CK_FD << 9) | (CRSH_SEQC_CK_SEG << 0)) & 0x1FF;
    eid_r17.WCTR_EN_CP_PD_C  = ~(((CRSH_SEQC_CK_TE << 10) | (CRSH_SEQC_CK_FD << 9) | (CRSH_SEQC_CK_SEG << 0)) & 0x1FF) & 0x1FF;
    mbus_remote_register_write(EID_ADDR,0x17,eid_r17.as_int);
}


//-------------------------------------------------------------------
// NFC Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: nfc_unfreeze_gpio
// Args    : None
// Description:
//           Initialize NFC GPIO pads and unfreeze.
// Return  : None
//-------------------------------------------------------------------
static void nfc_unfreeze_gpio(void) {
    // Write SCL=SDA=1
    gpio_write_data_with_mask(I2C_MASK,(1<<NFC_SCL)|(1<<NFC_SDA));
    // Set directions
    gpio_set_dir_with_mask(I2C_MASK,(1<<NFC_SCL)|(1<<NFC_SDA));
    gpio_set_dir_with_mask(GPO_MASK,(0<<NFC_GPO));
    // Unfreeze the pads
    unfreeze_gpio_out();
}

//-------------------------------------------------------------------
// Function: nfc_init
// Args    : None
// Description:
//           Initialization for the NFC chip communication
// Return  : None
//-------------------------------------------------------------------
static void nfc_init(void){

    // Config GPO wakeup
    #ifdef USE_GPO_POSEDGE
        config_gpio_posedge_wirq((1<<NFC_GPO));
    #endif
    #ifdef USE_GPO_NEGEDGE
        config_gpio_negedge_wirq((1<<NFC_GPO));
    #endif

    // Enable the GPIO pads
    set_gpio_pad_with_mask(I2C_MASK,(1<<NFC_SDA)|(1<<NFC_SCL));
    set_gpio_pad_with_mask(GPO_MASK,(1<<NFC_GPO));

    // Unfreeze GPIO
    nfc_unfreeze_gpio();

}

//-------------------------------------------------------------------
// Function: nfc_i2c_start
// Args    : None
// Description:
//           I2C Start Sequence
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(output) = 0
// Return  : None
// Waveform:
//          If starting from SCL=SDA=1
//              SCL  ********|______________
//              SDA  ****|__________________
//          If starting from SCL(output)=0, SDA(input)=1
//              SCL  __|**********|________   
//              SDA  **********|___________   
//-------------------------------------------------------------------

static void nfc_i2c_start(void) {
    gpio_write_data_with_mask(I2C_MASK,(1<<NFC_SDA)|(1<<NFC_SCL));
    gpio_set_dir_with_mask(I2C_MASK,(1<<NFC_SDA)|(1<<NFC_SCL));
    gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA)|(1<<NFC_SCL));
    gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA)|(0<<NFC_SCL));
}

//-------------------------------------------------------------------
// Function: nfc_i2c_stop
// Args    : None
// Description:
//           I2C Stop Sequence
//           At the end of the execution:
//              SCL(output) = 1
//              SDA(output) = 1
// Return  : None
// Waveform:
//          If starting from SCL(output)=0, SDA(output)=0
//              SCL  ____|******************
//              SDA  ________|**************
//          If starting from SCL(output)=0, SDA(input)=1
//              SCL  _______|***************
//              SDA  **|________|***********
//-------------------------------------------------------------------

static void nfc_i2c_stop(void) {
    gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA)|(0<<NFC_SCL));
    gpio_set_dir_with_mask(I2C_MASK,(1<<NFC_SDA)|(1<<NFC_SCL));
    gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA)|(1<<NFC_SCL));
    gpio_write_data_with_mask(I2C_MASK,(1<<NFC_SDA)|(1<<NFC_SCL));
}

//-------------------------------------------------------------------
// Function: nfc_i2c_cmd
// Args    : byte   - Byte to be sent (8 bits)
// Description:
//           Send 'byte' from MSB to LSB, then check ACK.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
// Return  : None
//-------------------------------------------------------------------

static void nfc_i2c_cmd(uint8_t byte){
    uint32_t i;

    // Direction: SCL (output), SDA (output)
    gpio_set_dir_with_mask(I2C_MASK,(1<<NFC_SDA)|(1<<NFC_SCL));

    // Send byte[7:1]
    for (i=7; i>0; i--) {
        gpio_write_data_with_mask(I2C_MASK,(((byte>>i)&0x1)<<NFC_SDA)|(0<<NFC_SCL));
        gpio_write_data_with_mask(I2C_MASK,(((byte>>i)&0x1)<<NFC_SDA)|(1<<NFC_SCL));
        gpio_write_data_with_mask(I2C_MASK,(((byte>>i)&0x1)<<NFC_SDA)|(0<<NFC_SCL));
    }
    // Send byte[0]
    if (byte&0x1) {
        // Direction: SCL (output), SDA (input)
        gpio_set_dir_with_mask(I2C_MASK,(0<<NFC_SDA)|(1<<NFC_SCL));
        gpio_write_data_with_mask(I2C_MASK,0<<NFC_SCL);
        gpio_write_data_with_mask(I2C_MASK,1<<NFC_SCL);
        gpio_write_data_with_mask(I2C_MASK,0<<NFC_SCL);
    }
    else {
        gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA)|(0<<NFC_SCL));
        gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA)|(1<<NFC_SCL));
        gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA)|(0<<NFC_SCL));
        // Direction: SCL (output), SDA (input)
        gpio_set_dir_with_mask(I2C_MASK,(0<<NFC_SDA)|(1<<NFC_SCL));
    }
    
    //Wait for ACK
    set_timer32_timeout(I2C_ACK_TIMEOUT);
    while((*GPIO_DATA>>NFC_SDA)&0x1);
    stop_timer32_timeout_check(FCODE_6);

    gpio_write_data_with_mask(I2C_MASK,1<<NFC_SCL);
    gpio_write_data_with_mask(I2C_MASK,0<<NFC_SCL);
}

//-------------------------------------------------------------------
// Function: nfc_i2c_rd
// Args    : ack    - 1: Ack
//                    0: No Ack
// Description:
//           Provide 8 SCL cycles and read SDA at every posedge.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
// Return  : 8-bit read data (uint8_t)
//-------------------------------------------------------------------

static uint8_t nfc_i2c_rd(uint8_t ack){
    uint8_t data = 0;

    // Direction: SCL (output), SDA (input)
    gpio_set_dir_with_mask(I2C_MASK,(0<<NFC_SDA)|(1<<NFC_SCL));

    // Read byte[7:0]
    uint32_t i = 7;
    while (1) {
        gpio_write_data_with_mask(I2C_MASK,0<<NFC_SCL);
        gpio_write_data_with_mask(I2C_MASK,1<<NFC_SCL);
        data = data | (((*GPIO_DATA>>NFC_SDA)&0x1)<<i);
        if (i==0) break;
        else i--;
    }
    gpio_write_data_with_mask(I2C_MASK,0<<NFC_SCL);

    // Acknowledge
    if (ack) {
        gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA) | (0<<NFC_SCL));
        gpio_set_dir_with_mask(I2C_MASK,(1<<NFC_SDA) | (1<<NFC_SCL));
        gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA) | (1<<NFC_SCL));
        gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SDA) | (0<<NFC_SCL));
        gpio_set_dir_with_mask(I2C_MASK,(0<<NFC_SDA) | (1<<NFC_SCL));
    }
    else{
        gpio_set_dir_with_mask(I2C_MASK,(0<<NFC_SDA) | (1<<NFC_SCL));
        gpio_write_data_with_mask(I2C_MASK,1<<NFC_SCL);
        gpio_write_data_with_mask(I2C_MASK,0<<NFC_SCL);
    }

    return data;
}

//-------------------------------------------------------------------
// Function: nfc_i2c_byte_write
// Args    : addr   : 16-bit address
//           data   : 8-bit data
// Description:
//           Write data at the address 'addr'
// Return  : None
//-------------------------------------------------------------------

static void nfc_i2c_byte_write(uint32_t addr, uint8_t data){
    nfc_i2c_start();
    nfc_i2c_cmd(0xA6);
    nfc_i2c_cmd(addr>>8);
    nfc_i2c_cmd(addr);
    nfc_i2c_cmd(data);
    nfc_i2c_stop();
}

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_write
// Args    : addr   : 16-bit address
//           data   : Memory address of the data array,
//                    where each element is a byte
//                    Order of TX:
//                      data[0] -> data[1] -> data[2] -> ... -> data[len-1]
//           len    : Number of bytes to be sent
// Description:
//           Write data starting at the address 'addr'
// Return  : None
//-------------------------------------------------------------------

static void nfc_i2c_seq_write(uint32_t addr, uint32_t data[], uint32_t len){
    uint32_t i;
    nfc_i2c_start();
    nfc_i2c_cmd(0xA6);
    nfc_i2c_cmd(addr>>8);
    nfc_i2c_cmd(addr);
    for (i=0; i<len; i++) {
        nfc_i2c_cmd(data[i]);
    }
    nfc_i2c_stop();
}

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_word_write
// Args    : addr   : 16-bit address
//           data   : Memory address of the data array,
//                    where each element is a word (4 bytes)
//                    Order of TX:
//                      data[0][31:24] -> data[0][23:16] -> data[0][15:8] -> data[0][7:0] ->
//                      data[1][31:24] -> data[1][23:16] -> data[1][15:8] -> data[1][7:0] ->
//                      ...
//                      data[len-1][31:24] -> data[len-][23:16] -> data[len-1][15:8] -> data[len-1][7:0] ->
//           len    : Number of words (4-bytes) to be sent
// Description:
//           Write data starting at the address 'addr'
// Return  : None
//-------------------------------------------------------------------

// FIXME: START FROM HERE

static void nfc_i2c_seq_word_write(uint32_t addr, uint32_t data[], uint32_t len){
    uint32_t i;
    nfc_i2c_start();
    nfc_i2c_cmd(0xA6);
    nfc_i2c_cmd(addr>>8);
    nfc_i2c_cmd(addr);
    for (i=0; i<len; i++) {
        nfc_i2c_cmd(data[i]);
    }
    nfc_i2c_stop();
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
    exec_count_irq = 0; // Make sure the irq counter is reset    
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
    eid_vin_threshold = pmu_adc_3p0_val + 21; // VBAT = ~2.55V
    snt_running = 0;
    snt_state = SNT_IDLE;
    read_data_temp = 150; // From SNSv10
    snt_high_temp_threshold = 300;
    snt_low_temp_threshold = 100;
    wakeup_timestamp = 0;
    wakeup_timestamp_rollover = 0;
    realtime_offset = 0;    

    eid_r00.as_int = EID_R00_DEFAULT_AS_INT;


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

    // Temp Sensor Initialization
    snt_r01.TSNS_RESETn     = 0;
    snt_r01.TSNS_EN_IRQ     = 1;
    snt_r01.TSNS_BURST_MODE = 0;
    snt_r01.TSNS_CONT_MODE  = 0;
    mbus_remote_register_write(SNT_ADDR,0x01,snt_r01.as_int);

    // Set temp sensor conversion time
    snt_r03.TSNS_SEL_STB_TIME  = 0x1; // Default: 0x1
    snt_r03.TSNS_SEL_CONV_TIME = 0x6; // Default: 0x6
    mbus_remote_register_write(SNT_ADDR,0x03,snt_r03.as_int);

    //-------------------------------------------------
    // MRR Settings
    //-------------------------------------------------
    // NOTE: XT1 does not use MRR

    //-------------------------------------------------
    // EID Settings
    //-------------------------------------------------
    // NOTE: eid_init() does not start the EID. It just configures its register file.
    eid_init();

    //-------------------------------------------------
    // XO Driver
    //-------------------------------------------------
    xo_start();

    //-------------------------------------------------
    // E-Ink Display
    //-------------------------------------------------
    uint32_t i, s;
    for(i=0; i<5; i++) {
        for(s=0; s<7; s++) {
            eid_update(1<<s);
            delay(200000);
        }
    }

    operation_sleep();
    while(1);





    eid_update(DISP_INITIALIZED);

    //-------------------------------------------------
    // NFC 
    //-------------------------------------------------
    nfc_init();

    //-------------------------------------------------
    // Sleep
    //-------------------------------------------------
    // It must go into sleep with a long 'sleep' duration
    // to give user enough time to put a "sticker" over the GOC solar cell.
    reset_xo_cnt(); // Make counter value = 0
    operation_sleep_xo_timer(SLEEP_DURATION_LONG);
}

//-------------------------------------------------------------------
// Application-Specific Functions
//-------------------------------------------------------------------

// pregit Implementation
static void snt_operation (void) {

    if (snt_state == SNT_IDLE) {
        snt_state = SNT_LDO;

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

        // Set PMU Floor for Temp Sensor Operation
        pmu_set_sleep_tsns();

        // Reset the Temp Sensor
        snt_temp_sensor_reset();

        // Release the reset for the Temp Sensor
        snt_temp_sensor_start();

        // Go to sleep during measurement
        operation_sleep();
    }
    else if (snt_state == SNT_TEMP_READ) {

        read_data_temp = *SNT_TARGET_REG_ADDR;
        meas_count++;

        // Option to take multiple measurements
        if (meas_count < SNT_NUM_TEMP_MEAS) {
            snt_temp_sensor_reset();
            snt_state = SNT_TEMP_START;
        }
        // All measurements are done
        else {
            meas_count = 0;

            // VBAT Measurement and SAR_RATIO Adjustment
            pmu_adc_read_and_sar_ratio_adjustment();

            // If VBAT is too low, trigger the EID Watchdog (System Crash)
            if (read_data_batadc < pmu_adc_crit_val) {
                eid_trigger_crash();
                while(1);
            }
            
            // Change PMU based on temp
            uint32_t pmu_setting_prev = pmu_setting_state;
            if      (read_data_temp > PMU_95C_threshold_snt){ pmu_setting_state = PMU_95C; }
            else if (read_data_temp > PMU_75C_threshold_snt){ pmu_setting_state = PMU_75C; }
            else if (read_data_temp > PMU_55C_threshold_snt){ pmu_setting_state = PMU_55C; }
            else if (read_data_temp > PMU_35C_threshold_snt){ pmu_setting_state = PMU_35C; }
            else if (read_data_temp < PMU_10C_threshold_snt){ pmu_setting_state = PMU_10C; }
            else if (read_data_temp < PMU_20C_threshold_snt){ pmu_setting_state = PMU_20C; }
            else                                            { pmu_setting_state = PMU_25C; }

            // Always restore sleep setting from higher pmu meas setting
            pmu_set_sleep_temp_based();

            if (pmu_setting_prev != pmu_setting_state){
                pmu_set_active_temp_based();
            }

            // Assert temp sensor isolation & turn off temp sensor power
            snt_temp_sensor_power_off();
            snt_ldo_power_off();
            snt_state = SNT_IDLE;

            // Store the Temp & VBAT Data with timestamp
            // FIXME: TBD

            // Update e-Ink as necessary
            //--- Default Display
            uint32_t seg_display;
            if (!get_flag(FLAG_GIT_TRIGGERED)) 
                seg_display = DISP_INITIALIZED;
            else
                seg_display = DISP_NORMAL;

            //--- Check temperature
            if (read_data_temp > snt_high_temp_threshold) 
                seg_display |= DISP_HIGH_TEMP;
            else if  (read_data_temp < snt_low_temp_threshold)  
                seg_display |= DISP_LOW_TEMP;

            //--- Check VBAT
            if (read_data_batadc > pmu_adc_low_val) seg_display |= DISP_LOW_VBAT;

            //--- Update the Display
            eid_update(seg_display);

            // Track execution count
            exec_count++;

            // Before GIT
            if (!get_flag(FLAG_GIT_TRIGGERED)) {
                reset_xo_cnt();
                operation_sleep_xo_timer(SLEEP_DURATION_PREGIT);
            }
            // After GIT
            else {
                if (!get_flag(FLAG_USE_REAL_TIME)) {
                    reset_xo_cnt();
                    operation_sleep_xo_timer(SLEEP_DURATION);
                }
                else {
                    uint32_t prev_wakeup_timestamp = wakeup_timestamp;
                    wakeup_timestamp += SLEEP_DURATION;
                    if (wakeup_timestamp < prev_wakeup_timestamp) wakeup_timestamp_rollover++;
                    operation_sleep_xo_timer(wakeup_timestamp);
                }
            }
        }
    }
}

// Post-GIT Initialization
static void postgit_init(void) {
    // Probably redundant, but for safety...
    wakeup_timestamp_rollover = 0;

    // Enable EID Watchdog
    eid_r00.EN_WATCHDOG = 1;
    mbus_remote_register_write(EID_ADDR,0x00,eid_r00.as_int);

    // Any more initialization?

}

// NFC Event handling
static void do_nfc(void) {

}

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
    *NVIC_ICPR = (0x1 << IRQ_TIMER16);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_TIMER16, 0x00000000);
    #endif
    }
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG0, 0x00000000);
    #endif
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG1, 0x00000000);
    #endif
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG2, 0x00000000);
    #endif
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG3, 0x00000000);
    #endif
}
void handler_ext_int_reg4(void) { // REG4
    *NVIC_ICPR = (0x1 << IRQ_REG4);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG4, 0x00000000);
    #endif
}
void handler_ext_int_reg5(void) { // REG5
    *NVIC_ICPR = (0x1 << IRQ_REG5);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG5, 0x00000000);
    #endif
}
void handler_ext_int_reg6(void) { // REG6
    *NVIC_ICPR = (0x1 << IRQ_REG6);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG6, 0x00000000);
    #endif
}
void handler_ext_int_reg7(void) { // REG7
    *NVIC_ICPR = (0x1 << IRQ_REG7);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_REG7, 0x00000000);
    #endif
}
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_MBUS_MEM, 0x00000000);
    #endif
}
void handler_ext_int_mbusrx(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_MBUS_RX, 0x00000000);
    #endif
}
void handler_ext_int_mbustx(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_MBUS_TX, 0x00000000);
    #endif
}
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_MBUS_FWD, 0x00000000);
    #endif
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_GOCEP, 0x00000000);
    #endif
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_SOFT_RESET, 0x00000000);
    #endif
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);
    uint32_t wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1; // reset WAKEUP_SOURCE register
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_WAKEUP, (0x10 << 24) | wakeup_source);
    #endif

    // For safety, disable Wakeup Timer's WREQ.
    xot_disable_wreq();

    // EID Watchdog Check-In
    if (get_flag(FLAG_WD_ENABLED)) eid_check_in();

    // Unfreeze NFC GPIO
    nfc_unfreeze_gpio();

    //--------------------------------------------------------------------------
    // WAKEUP_SOURCE (wakeup_source) definition
    //--------------------------------------------------------------------------
    //  [31:12] - Reserved
    //     [11] - GPIO_PAD[3] has triggered wakeup (valid only when wakeup_source[3]=1)
    //     [10] - GPIO_PAD[2] has triggered wakeup (valid only when wakeup_source[3]=1)
    //     [ 9] - GPIO_PAD[1] has triggered wakeup (valid only when wakeup_source[3]=1)
    //     [ 8] - GPIO_PAD[0] has triggered wakeup (valid only when wakeup_source[3]=1)
    //  [ 7: 6] - Reserved
    //      [5] - GIT (GOC Instant Trigger) has triggered wakeup
    //      [4] - MBus message has triggered wakeup (e.g., Flash Auto Boot-up)
    //      [3] - One of GPIO_PAD[3:0] has triggered wakeup
    //      [2] - XO Timer has triggered wakeup
    //      [1] - Wake-up Timer has triggered wakeup
    //      [0] - GOC/EP has triggered wakeup
    //--------------------------------------------------------------------------

    // If woken up by GIT
    // NOTE: If GIT is triggered while the system is in active, the GIT is NOT immediately handled.
    //       Instead, it waits until the system goes in sleep, and then, the (pending) GIT will wake up the system.
    //       Thus, you can safely assume that GIT is (effectively) triggered only while the system is in Sleep.
    if (get_bit(wakeup_source, 5)) {
        set_flag(FLAG_GIT_TRIGGERED, 1);
        // Reset everything back to default.
        operation_back_to_default();
        // Post-GIT Initialization
        postgit_init();
        // Start Temp/VBAT measurement
        snt_running = 1;
        meas_count = 0;
    }
    // If woken up by a XO wakeup timer
    else if (get_bit(wakeup_source, 2)) {

        if (wakeup_timestamp==0) wakeup_timestamp = *XOT_VAL;

        // If GIT is not yet enabled (i.e., the very first wakeup by the wakeup timer)
        if (!get_flag(FLAG_GIT_ENABLED)) {
            // Enable GIT (GOC Instant Trigger)
            *REG_GOC_CONFIG = set_bits(*REG_GOC_CONFIG, 16, 16, 1);
            set_flag(FLAG_GIT_ENABLED, 1);
            // Start Temp/VBAT measurement
            snt_running = 1;
            meas_count = 0;
        }
    }
    // If woken up by NFC (GPIO[0])
    else if (get_bit(wakeup_source, 3) && get_bit(wakeup_source, 8)) {
        // Handle the NFC event
        do_nfc();
    }
    // If woken up by an MBus Message
    else if (get_bit(wakeup_source, 4)) { }

    // If SNT is running
    while (snt_running) snt_operation();

    // NOTE: If the wakeup source is GOC/EP (i.e., wakeup_source[0] == 1)
    //       it will be handled by handler_ext_int_gocep()

}
void handler_ext_int_aes(void) { // AES
    *NVIC_ICPR = (0x1 << IRQ_AES);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_SPI, 0x00000000);
    #endif
}
void handler_ext_int_spi(void) { // SPI
    *NVIC_ICPR = (0x1 << IRQ_SPI);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_SPI, 0x00000000);
    #endif
}
void handler_ext_int_gpio(void) { // GPIO
    *NVIC_ICPR = (0x1 << IRQ_GPIO);
    #ifdef ARB_DEBUG
    arb_debug_reg(IRQ_GPIO, 0x00000000);
    #endif
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_WAKEUP) | (0x1 << IRQ_GOCEP) | (0x1 << IRQ_TIMER32) 
               | (0x1 << IRQ_REG0  ) | (0x1 << IRQ_REG1 ) 
               | (0x1 << IRQ_REG2  ) | (0x1 << IRQ_REG3 );

    // Initialization Sequence
    if (!get_flag(FLAG_INITIALIZED)) { 
        operation_init();
    }

    // Never Quit (should not stay here for an extended duration)
    while(1) asm("nop");
    return 1;

//    // Sleep/Wakeup OR Terminate operation
//    if (cyc_num == 999) *REG_CHIP_ID = 0xFFFFFF; // This will stop the verilog sim.
//    else {
//        cyc_num++;
//        set_wakeup_timer(5, 1, 1);
//        mbus_sleep_all();
//    }

//    *REG_CHIP_ID = 0xFFFFFF; // This will stop the verilog sim
//
//    // Never Quit (should not come here)
//    while(1) asm("nop;"); 
//    return 1;
}
