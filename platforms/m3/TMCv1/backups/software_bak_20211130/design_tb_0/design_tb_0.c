//#define PMU_TEST
//#define XO_TEST
//#define EID_TEST
//#define GIT_TEST    // GIT also enables the EID Watchdog
//#define GIT_DEMO  // Need EID_TEST and GIT_TEST. Displays the check mark ten times and then makes the EID watchdog expire.
#define NFC_TEST
//*******************************************************************************************
// XT1 (TMCv1) FIRMWARE
// Version alpha-0.1
//-------------------------------------------------------------------------------------------
// TMCv1 SUB-LAYER CONNECTION:
//      PREv22E -> SNTv5 -> EIDv1 -> MRRv11A -> MEMv3 -> PMUv13
//-------------------------------------------------------------------------------------------
// External Connections
//
//  XT1         NFC (ST25DVxxx)     XT1     e-Ink Display
//  ---------------------------     ------------------------
//  GPIO[0]     GPO                 SEG[0]  Triangle
//  GPIO[1]     SCL                 SEG[1]  Tick
//  GPIO[2]     SDA                 SEG[2]  Low Battery
//  VPG2_OUT    VCC                 SEG[3]  Back Slash
//                                  SEG[4]  Slash
//                                  SEG[5]  Plus
//                                  SEG[6]  Minus
//
//  NOTE: In the e-Ink display, Top side is where the SEG[2] is located.
//        SEG[0] (Triangle) points the right. SEG[1] and SEG[4] makes the 'check' mark.
//-------------------------------------------------------------------------------------------
//
// Use of Vector Table
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
// - Currently, use of 'nfc_power_on()' makes VBAT > 100uA. Need to figure out why.
// - We may not need the code to handle "If woken up by NFC". 
//      The NFC chip is supposed to handle the phone communication by its own.
//      TMCv1 only has to store measurement inside the NFC chip periodically.
//      The 'GPO' event should 'reset' the 'pointer' only - for now.
// - Is it possible to get a GOC/EP to go into sleep while the system is doing the display update?
//      If yes, how can we ensure that we stop the EID operataion before going into sleep?
//******************************************************************************************* 

//******************************************************************************************* 
//
//                               THINGS TO BE NOTED
//
//******************************************************************************************* 
// I2C with NFC (ST25DVnnK)
//-------------------------------------------------------------------------------------------
//
//  Device Select Code (DSC)
//      [7:4] = 4'b1010
//      [3]   = E2 (0: Access to user memory, dynamic registers or Mailbox; 1: Access to system area)
//      [2:1] = 2'b11
//      [0]   = RW (0: Write; 1: Read)
//
//  I2C Write Operation
//
//  I2C Read Operation
//
//  I2C Present Password
//
//      Start 
//      -> 0xAE         // Device Select code with E2=1, RW=0
//      -> 0x09 -> 0x00 // Password Address
//      -> PW[63:56] -> PW[55:48] -> PW[47:40] -> PW[39:32] -> PW[31:24] -> PW[23:16] -> PW[15:8] -> PRW[7:0]
//      -> 0x09         // Validation Code
//      -> PW[63:56] -> PW[55:48] -> PW[47:40] -> PW[39:32] -> PW[31:24] -> PW[23:16] -> PW[15:8] -> PRW[7:0] 
//      -> Stop
//      
//  I2C Write Password
//
//      Start 
//      -> 0xAE         // Device Select code with E2=1, RW=0
//      -> 0x09 -> 0x00 // Password Address
//      -> PW[63:56] -> PW[55:48] -> PW[47:40] -> PW[39:32] -> PW[31:24] -> PW[23:16] -> PW[15:8] -> PRW[7:0]
//      -> 0x07         // Validation Code
//      -> PW[63:56] -> PW[55:48] -> PW[47:40] -> PW[39:32] -> PW[31:24] -> PW[23:16] -> PW[15:8] -> PRW[7:0] 
//      -> Stop
//      
//
//
//******************************************************************************************* 
// NFC (ST25DVnnK)
//-------------------------------------------------------------------------------------------
//
//
//  System Configuration Memory Map - Static Registers (Table 11) - I2C Access
//  - located in the system configuration area memory (EEPROM nonvolatile registers)
//  - I2C security session must first be open, by presenting a valid I2C password.
//  
//      Name            DevSel  Addr        Type    Description
//      -----------------------------------------------------------------------------------------
//      GPO             E2=1    0x0000      RW      Enable/Disable ITs on GPO
//      IT_TIME         E2=1    0x0001      RW      Interruption pulse duration
//      UID             E2=1    0x0018      RO      Unique identifier, 8 bytes
//                              - 0x001F
//      IC_REF          E2=1    0x0017      RO      IC reference value
//      IC_REV          E2=1    0x0020      RO      IC revision
//      I2C_PWD         E2=1    0x0900      RW      I2C security session password, 8 bytes
//                              - 0x0907
//
//      [GPO]
//          bit default name            function
//          -------------------------------------------------------------------------------------------------
//          [7] 1'b1    GPO_EN          1: GPO output is enabled. GPO outputs enabled interrupts.
//          [6] 1'b0    RF_WRITE_EN     1: A pulse is emitted on GPO at completion of valid RF write operation in EEPROM.
//          [5] 1'b0    RF_GET_MSG_EN   1: A pulse is emitted on GPO at completion of valid RF read message command if end of messeage has been reached.
//          [4] 1'b0    RF_PUT_MSG_EN   1: A pulse is emitted on GPO at completion of valid RF write message command.
//          [3] 1'b1    FIELD_CHANGE_EN 1: A pulse is emitted on GPO, when RF field appears or disappears.
//          [2] 1'b0    RF_INTERRUPT_EN 1: GPO output level is controlled by Manage GPO Command (pulse)
//          [1] 1'b0    RF_ACTIVITY_EN  1: GPO output level changes from RF command EOF to response EOF.
//          [0] 1'b0    RF_USER_EN      1: GPO output level is controlled by Manage GPO Command (set/reset)
//      
//      [IT_TIME]
//          bit     default name        function
//          -------------------------------------------------------------------------------------------------
//          [7:3]   5'b0    RFU         -
//          [2:0]   3'b011  IT_TIME     Pulse duration = 301us - IT_TIME x 37.65us (+/- 2us)
//
//  
//  Dynamic Configuration Memory Map - Dynamic Registers (Table 12) - I2C Access
//  
//      Name            DevSel  Addr        Type    Description
//      -----------------------------------------------------------------------------------------
//      GPO_CTRL_Dyn    E2=0    0x2000      RW      GPO control
//      I2C_SSO_Dyn     E2=0    0x2004      RO      I2C security session status
//      IT_STS_Dyn      E2=0    0x2005      RO      Interruption Status
//
//      [GPO_CTRL_Dyn] - Same as GPO; See above.
//
// GPO Wakeup (Assuming GPO_EN=1)
//
//      FIELD_CHANGE:
//           - When RF field appears or disappears, GPO emits a pulse of duration IT_TIME.
//           - In case of RF field disappears, the pulse is emitted only if VCC(=VNFC) power supply is present.
//           - If RF_SLEEP=1, the field change is not reported on GPO.
//
//

//******************************************************************************************* 
// EID Charge Pump 
//-------------------------------------------------------------------------------------------
//
// VIN=0: Use VBAT as the charge pump input (recommended when VBAT is low)
// VIN=1: Use VSS  as the charge pump input (recommended when VBAT is high) 
//       [WARNING: VIN=1 may affect the EID timer when used with TMR_SEL_LDO=1]
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
// XO Configuration
//-------------------------------------------------------------------------------------------
//
//  *REG_XO_CONF2 = ( 0x0
//      | (0x2 << 13)     // XO_INJ	            (2'h2) #Adjusts injection period
//      | (0x1 << 10)     // XO_SEL_DLY	        (3'h1) #Adjusts pulse delay
//      | (0x1 << 8 )     // XO_SEL_CLK_SCN	    (2'h1) #Selects division ratio for SCN CLK
//      | (0x1 << 5 )     // XO_SEL_CLK_OUT_DIV (3'h1) #Selects division ratio for the XO CLK output
//                        //    Frequency = 2 ^ (10 + XO_SEL_CLK_OUT_DIV) (Hz) for XO_SEL_CLK_OUT_DIV=1, 2, 3, 4, 5. 
//      | (0x1 << 4 )     // XO_SEL_LDO	        (1'h1) #Selects LDO output as an input to SCN
//      | (0x0 << 3 )     // XO_SEL_0P6	        (1'h0) #Selects V0P6 as an input to SCN
//      | (0x0 << 0 )     // XO_I_AMP	        (3'h0) #Adjusts VREF body bias buffer current
//      );
//
//******************************************************************************************* 

#include "TMCv1.h"

#define DLY_1S  40000    // 5 instructions @ 100kHz w/ 2x margin
#define DLY_1MS 40    // 5 instructions @ 100kHz w/ 2x margin

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
// Simulation for Debug
//#define SIM         // Enable this for verilog simulations. It enables 1) ARB Debug Messages, 2) Short delays. 

// Failure Codes
#define FAIL_MBUS_ADDR  0xE0    // In case of failure, it sends an MBus message containing the failure code to this MBus Address.
#define FCODE_I2C_ACK_TIMEOUT     6   // Timeout during I2C ACK

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

// EEPROM Write Time (Tw): Must be >5ms; See datasheet
#define NFC_TW  10*DLY_1MS

// ACK Timeout
#define I2C_ACK_TIMEOUT 50000   // Checked using TIMER32

//*******************************************************************************************
// XO AND SLEEP DURATIONS
//*******************************************************************************************

// XO Initialization Wait Duration
#ifndef SIM
    #define XO_WAIT_A  20000    // Must be ~1 second delay. Delay for XO Start-Up. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
    #define XO_WAIT_B  20000    // Must be ~1 second delay. Delay for VLDO & IBIAS Generation. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#else
    #define XO_WAIT_A  100      // Short delay for simulation
    #define XO_WAIT_B  100      // Short delay for simulation
#endif

// XO Counter Value per Specific Time Durations
#define XOT_1SEC    2048
#define XOT_1MIN    60*XOT_1SEC
#define XOT_1HR     60*XOT_1MIN
#define XOT_1DAY    24*XOT_1HR

// Sleep Duration 
#ifndef SIM
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

#define DISP_CHECK      DISP_TICK | DISP_SLASH
#define DISP_XMARK      DISP_SLASH | DISP_BACKSLASH

// dummy - remove later
#define DISP_INITIALIZED    0
#define DISP_NORMAL    0
#define DISP_HIGH_TEMP    0
#define DISP_LOW_TEMP    0
#define DISP_LOW_VBAT    0

// LDO for Timer
//-----------------------------------------------
#define EID_TMR_SEL_LDO     1 // 1: EID Timer uses the regulated VBAT from LDO; 0: EID Timer uses V1P2 (i.e., the LDO is bypassed).

// Timer Start-up Delay
//-----------------------------------------------
// NOTE: In silicon, most chips are fine with EID_TMR_INIT_DELAY=60000. 
//       However, in some cases (e.g., XT1B#3), it must be 120000 to make a 4-second delay.
#ifndef SIM
    #define EID_TMR_INIT_DELAY  120000   // Must be >3 seconds delay. Delay between TMR_SELF_EN=1 and TMR_EN_OSC=0. LSB corresponds to ~50us, assuming ~100kHz CPU clock and 5 cycles per delay(1).
#else
    #define EID_TMR_INIT_DELAY  100     // Short delay for simulation
#endif

// Charge Pump Behavior (Normal Use)
//-----------------------------------------------
#define EID_CP_VIN             0 // Charge pumps's VIN selection; Use 0 if VBAT is low (< ~2.5V); Use 1 if VBAT is high (> ~2.5V).
#define EID_CP_CLEAR_FD        1 // If 1, it makes FD white after writing SEG(s).

// Charge Pumps Tuning (Normal Use)
//-----------------------------------------------
#define EID_CP_CLK_SEL_RING    0x0     // (Default: 0x0; Max: 0x3) Selects # of rings. 0x0: 11 stages; 0x3: 5 stages.
#define EID_CP_CLK_SEL_TE_DIV  0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Top Electrode (TE) charge pump. Divide the core clock by 2^EID_CP_CLK_SEL_TE_DIV.
#define EID_CP_CLK_SEL_FD_DIV  0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Field (FD) charge pump. Divide the core clock by 2^EID_CP_CLK_SEL_FD_DIV.
#define EID_CP_CLK_SEL_SEG_DIV 0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Segment (SEG) charge pumps. Divide the core clock by 2^EID_CP_CLK_SEL_SEG_DIV.

// Charge Pump Activation Duration (Normal Use)
//-----------------------------------------------
// NOTE: In silicon, the FSM clock speed (=TMR clock frequency / 16) is 120~125Hz at RT, which means, LSB corresponds to ~8ms.
#ifndef SIM
    #define EID_CP_PULSE_WIDTH 250   // (Default: 400; Max: 65535) Duration of Charge Pump Activation.
#else
    #define EID_CP_PULSE_WIDTH 40      // Short delay for simulation
#endif

//--- Crash Handling
//--------------------------------------------------------
//
// X: CRSH_CP_IDLE_WIDTH
// Y: CRSH_CP_PULSE_WIDTH
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

// CP Clock setting for Crash handling
//-----------------------------------------------
#define CRSH_CP_CLK_SEL_RING       0x0     // (Default: 0x0; Max: 0x3) Selects # of rings. 0x0: 11 stages; 0x3: 5 stages.
#define CRSH_CP_CLK_SEL_TE_DIV     0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Top Electrode (TE) charge pump.
#define CRSH_CP_CLK_SEL_FD_DIV     0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Field (FD) charge pump.
#define CRSH_CP_CLK_SEL_SEG_DIV    0x0     // (Default: 0x0; Max: 0x3) Selects clock division ratio for Segment (SEG) charge pumps.

// Watchdog Threshold for Crash Detection
//-----------------------------------------------
// NOTE: In silicon, the FSM clock speed (=TMR clock frequency / 16) is 120~125Hz at RT, which means, LSB corresponds to ~8ms.
#define CRSH_WD_THRSHLD     2500  // (Default: 360,000, Max:16,777,215) Crash Watchdog Threshold.

// Charge Pump Activation/Idle Duration (for Crash handling)
//-----------------------------------------------
// NOTE: In silicon, the FSM clock speed (=TMR clock frequency / 16) is 120~125Hz at RT, which means, LSB corresponds to ~8ms.
#ifndef SIM
    #define CRSH_CP_IDLE_WIDTH     250     // (Default: 400; Max: 65535) Duration of Charge Pump Activation.
    #define CRSH_CP_PULSE_WIDTH    250     // (Default: 400; Max: 65535) Duration of Charge Pump Activation.
#else
    #define CRSH_CP_IDLE_WIDTH     40      // Short delay for simulation
    #define CRSH_CP_PULSE_WIDTH    40      // Short delay for simulation
#endif
#define CRSH_SEL_SEQ        0x7     // See above waveform. Each bit in [2:0] enables/disables Seq C, Seq B, Seq A, respectively. 
                                    // If 1, the corresponding sequence is enabled. If 0, the corresponding sequence is skipped.

//--- Sequence A (valid only if CRSH_SEL_SEQ[0]=1)
//-----------------------------------------------
#define CRSH_SEQA_VIN       0x0     // Use 0 if VBAT is low; Use 1 if VBAT is high.
#define CRSH_SEQA_CK_TE     0x0     // CK Value for Top Electrode (TE)
#define CRSH_SEQA_CK_FD     0x1     // CK Value for Field (FD)
#define CRSH_SEQA_CK_SEG    0x1FF   // CK Value for Segments (SEG[8:0])

//--- Sequence B (valid only if CRSH_SEL_SEQ[1]=1)
//-----------------------------------------------
#define CRSH_SEQB_VIN       0x0     // Use 0 if VBAT is low; Use 1 if VBAT is high.
#define CRSH_SEQB_CK_TE     0x1     // CK Value for Top Electrode (TE)
#define CRSH_SEQB_CK_FD     0x0     // CK Value for Field (FD)
#define CRSH_SEQB_CK_SEG    0x000   // CK Value for Segments (SEG[8:0])

//--- Sequence C (valid only if CRSH_SEL_SEQ[2]=1)
//-----------------------------------------------
#define CRSH_SEQC_VIN       0x0     // Use 0 if VBAT is low; Use 1 if VBAT is high.
#define CRSH_SEQC_CK_TE     0x0     // CK Value for Top Electrode (TE)
#define CRSH_SEQC_CK_FD     0x0     // CK Value for Field (FD)
#define CRSH_SEQC_CK_SEG    0x018   // CK Value for Segments (SEG[8:0])

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
volatile uint32_t wakeup_source;
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

volatile uint32_t eid_clk_sel_ring;
volatile uint32_t eid_clk_sel_te_div;
volatile uint32_t eid_clk_sel_fd_div;
volatile uint32_t eid_clk_sel_seg_div;
volatile uint32_t eid_tmr_sel_ldo;
volatile uint32_t eid_clear_fd;
volatile uint32_t eid_vin;

volatile uint32_t i2c_password_upper;   // Bit[63:32] of the I2C password
volatile uint32_t i2c_password_lower;   // Bit[31: 0] of the I2C password

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
static void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data);
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
static void eid_enable_cp_ck(uint32_t te, uint32_t fd, uint32_t seg);
static void eid_all_black();
static void eid_all_white();
static void eid_seg_black(uint32_t seg);
static void eid_seg_white(uint32_t seg);
static void eid_update(uint32_t seg);
static void eid_check_in(void);
static void eid_trigger_crash(void);
static void eid_init(void);

//-- NFC Functions
static void nfc_power_on(void);
static void nfc_power_off(void);
static void nfc_init(void);
static void nfc_i2c_start(void);
static void nfc_i2c_stop(void);
static void nfc_i2c_byte(uint8_t byte);
static void nfc_i2c_password(uint32_t mode);
static void nfc_i2c_present_password(void);
static void nfc_i2c_update_password(void);
static uint8_t nfc_i2c_rd(uint8_t ack);
static uint8_t nfc_i2c_byte_read(uint32_t e2, uint32_t addr);
static void nfc_i2c_byte_write(uint32_t e2, uint32_t addr, uint8_t data);
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
    start_xo_cnt();
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
// Description:
//          pmu_reg_write_core() writes reg_data in PMU's reg_addr
// Return  : None
//-------------------------------------------------------------------
static void pmu_reg_write_core (uint32_t reg_addr, uint32_t reg_data) {
    set_halt_until_mbus_trx();
    mbus_remote_register_write(PMU_ADDR,reg_addr,reg_data);
    set_halt_until_mbus_tx();
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
    pmu_reg_write_core(reg_addr, reg_data);
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
    pmu_reg_write_core(0x00, reg_addr);
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
        | (1   << 11)   // 2'h0     // Clock Ring Tuning
        | (3   << 8 )   // 3'h0     // Clock Divider Tuning for SAR Charge Pump Pull-Up
        | (4   << 5 )   // 3'h0     // Clock Divider Tuning for UPC Charge Pump Pull-Up
        | (3   << 2 )   // 3'h0     // Clock Divider Tuning for DNC Charge Pump Pull-Up
        | (3   << 0 )   // 2'h0     // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up
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
    //pmu_set_active_temp_based();
    //pmu_set_sleep_temp_based();

    pmu_reg_write(0x16,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0 << 19)     // 1'h0     // Enable PFM even during periodic reset
        | (0 << 18)     // 1'h0     // Enable PFM even when VREF is not used as reference
        | (0 << 17)     // 1'h0     // Enable PFM
        | (3 << 14)     // 3'h3     // Comparator clock division ratio
        | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (0x8 << 9)      // 4'h8     // Frequency multiplier R
        | (0x8 << 5)  // 4'h8     // Frequency multiplier L (actually L+1)
        | (0x0F)        // 5'h0F    // Floor frequency base (0-63)
    ));

    //---------------------------------------------------------------------------------------
    // UPC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x18,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (3 << 14)     // 2'h0     // Desired Vout/Vin ratio
        | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (0x8 << 9)      // 4'h8     // Frequency multiplier R
        | (0x4 << 5)      // 4'h4     // Frequency multiplier L (actually L+1)
        | (0x08)        // 5'h08    // Floor frequency base (0-63)
    ));


    //---------------------------------------------------------------------------------------
    // DNC_TRIM_V3_[ACTIVE|SLEEP]
    //---------------------------------------------------------------------------------------
    pmu_reg_write(0x1A,  // Default  // Description
    //---------------------------------------------------------------------------------------
        ( (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
        | (0x8 << 9)      // 4'h8     // Frequency multiplier R
        | (0x4 << 5)      // 4'h4     // Frequency multiplier L (actually L+1)
        | (0x08)        // 5'h08    // Floor frequency base (0-63)
    ));

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
    // CTRL_DESIRED_STATE_SLEEP / Turn off ADC
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

    // Turn on SAR_EN_ACC_VOUT
    pmu_reg_write(0x0F, 0x00B204); 

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
    delay(EID_TMR_INIT_DELAY); // Wait for >3s
    eid_r01.TMR_EN_OSC = 0;
    mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    eid_r01.TMR_SEL_LDO = eid_tmr_sel_ldo; // 1: use VBAT; 0: use V1P2
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
// Args    : te  - If 1, Top Electrode (TE) charge pump output goes high (~15V).
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
static void eid_enable_cp_ck(uint32_t te, uint32_t fd, uint32_t seg) {
    uint32_t cp_ck = ((te << 10) | (fd << 9) | (seg << 0)) & 0x7FF;
    uint32_t cp_pd = (~cp_ck) & 0x7FF;

    // Make PG_DIODE=0
    eid_r02.ECP_PG_DIODE = 0;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);

    // Enable charge pumps
    eid_r09.ECTR_RESETB_CP = 1;
    eid_r09.ECTR_VIN_CP    = eid_vin;
    eid_r09.ECTR_EN_CP_PD  = cp_pd;
    eid_r09.ECTR_EN_CP_CK  = cp_ck;
    set_halt_until_mbus_trx();
    mbus_remote_register_write(EID_ADDR,0x09,eid_r09.as_int);
    set_halt_until_mbus_tx();

    // Make FD White
    if (eid_clear_fd && (seg != 0)) {
        cp_ck = ((0x1 << 10) | (0x0 << 9) | (seg << 0)) & 0x7FF;
        cp_pd = (~cp_ck) & 0x7FF;
        eid_set_pulse_width(EID_CP_PULSE_WIDTH>>1); // Use a half duration to prevent over-erase.
        eid_r09.ECTR_EN_CP_PD  = cp_pd;
        eid_r09.ECTR_EN_CP_CK  = cp_ck;
        set_halt_until_mbus_trx();
        mbus_remote_register_write(EID_ADDR,0x09,eid_r09.as_int);
        set_halt_until_mbus_tx();
        eid_set_pulse_width(EID_CP_PULSE_WIDTH);
    }

    // Make PG_DIODE=1
    eid_r02.ECP_PG_DIODE = 1;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);
}

//-------------------------------------------------------------------
// Function: eid_all_black
// Args    : None
// Description:
//           Make the entire display black
// Return  : None
//-------------------------------------------------------------------
static void eid_all_black(void) { 
    eid_enable_cp_ck(0x0, 0x1, 0x1FF); 
}

//-------------------------------------------------------------------
// Function: eid_all_white
// Args    : None
// Description:
//           Make the entire display white
// Return  : None
//-------------------------------------------------------------------
static void eid_all_white(void) { 
    eid_enable_cp_ck(0x1, 0x0, 0x000); 
}

//-------------------------------------------------------------------
// Function: eid_seg_black
// Args    : seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] black.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the selected segment(s) black.
//           NOTE: Field (FD) must be white first.
// Return  : None
//-------------------------------------------------------------------
static void eid_seg_black(uint32_t seg) { 
    eid_enable_cp_ck(0x0, 0x0, seg);
}

//-------------------------------------------------------------------
// Function: eid_seg_white
// Args    : seg - For n=0,1,...,8:
//                 If bit[n]=1, make SEG[n] white.
//                 If bit[n]=0, leave SEG[n] unchanged.
// Description:
//           Make the selected segment(s) white.
//           NOTE: Field (FD) must be black first.
// Return  : None
//-------------------------------------------------------------------
static void eid_seg_white(uint32_t seg) { 
    eid_enable_cp_ck(0x1, 0x1, (~seg & 0x1FF));
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
    //eid_vin = (read_data_batadc < eid_vin_threshold) ? 0x0 : 0x1;
    eid_all_white();
    eid_seg_black(seg);
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

    // Charge Pump Tuning; See EID_CP_CLK_SEL_RING, EID_CP_CLK_SEL_TE_DIV, EID_CP_CLK_SEL_FD_DIV, EID_CP_CLK_SEL_SEG_DIV
    eid_r02.ECP_PG_DIODE    = 1;
    eid_r02.ECP_SEL_RING    = eid_clk_sel_ring;
    eid_r02.ECP_SEL_TE_DIV  = eid_clk_sel_te_div;
    eid_r02.ECP_SEL_FD_DIV  = eid_clk_sel_fd_div;
    eid_r02.ECP_SEL_SEG_DIV = eid_clk_sel_seg_div;
    mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);

    // Charge Pump Pulse Width
    eid_set_pulse_width(EID_CP_PULSE_WIDTH);

    // Configuration for Crash Behavior (See comment section in the top)
    eid_r10.WCTR_THRESHOLD = CRSH_WD_THRSHLD;
    mbus_remote_register_write(EID_ADDR,0x10,eid_r10.as_int);

    eid_r11.WCTR_CR_ECP_SEL_SEG_DIV     = CRSH_CP_CLK_SEL_SEG_DIV;
    eid_r11.WCTR_CR_ECP_SEL_FD_DIV      = CRSH_CP_CLK_SEL_FD_DIV;
    eid_r11.WCTR_CR_ECP_SEL_TE_DIV      = CRSH_CP_CLK_SEL_TE_DIV;
    eid_r11.WCTR_CR_ECP_SEL_RING        = CRSH_CP_CLK_SEL_RING;
    eid_r11.WCTR_CR_ECP_PG_DIODE_C1     = CRSH_PG_DIODE_C1;
    eid_r11.WCTR_CR_ECP_PG_DIODE_C      = CRSH_PG_DIODE_C;
    eid_r11.WCTR_CR_ECP_PG_DIODE_B1     = CRSH_PG_DIODE_B1;
    eid_r11.WCTR_CR_ECP_PG_DIODE_B      = CRSH_PG_DIODE_B;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A1     = CRSH_PG_DIODE_A1;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A      = CRSH_PG_DIODE_A;
    eid_r11.WCTR_CR_ECP_PG_DIODE_A0     = CRSH_PG_DIODE_A0;
    mbus_remote_register_write(EID_ADDR,0x11,eid_r11.as_int);

    eid_r12.WCTR_IDLE_WIDTH = CRSH_CP_IDLE_WIDTH;
    eid_r12.WCTR_SEL_SEQ    = CRSH_SEL_SEQ;
    mbus_remote_register_write(EID_ADDR,0x12,eid_r12.as_int);

    eid_r13.WCTR_PULSE_WIDTH    = CRSH_CP_PULSE_WIDTH;
    mbus_remote_register_write(EID_ADDR,0x13,eid_r13.as_int);

    eid_r15.WCTR_RESETB_CP_A = 1;
    eid_r15.WCTR_VIN_CP_A    = CRSH_SEQA_VIN;
    eid_r15.WCTR_EN_CP_CK_A  =   ((CRSH_SEQA_CK_TE << 10) | (CRSH_SEQA_CK_FD << 9) | (CRSH_SEQA_CK_SEG << 0)) & 0x7FF;
    eid_r15.WCTR_EN_CP_PD_A  = ~(((CRSH_SEQA_CK_TE << 10) | (CRSH_SEQA_CK_FD << 9) | (CRSH_SEQA_CK_SEG << 0)) & 0x7FF) & 0x7FF;
    mbus_remote_register_write(EID_ADDR,0x15,eid_r15.as_int);

    eid_r16.WCTR_RESETB_CP_B = 1;
    eid_r16.WCTR_VIN_CP_B    = CRSH_SEQB_VIN;
    eid_r16.WCTR_EN_CP_CK_B  =   ((CRSH_SEQB_CK_TE << 10) | (CRSH_SEQB_CK_FD << 9) | (CRSH_SEQB_CK_SEG << 0)) & 0x7FF;
    eid_r16.WCTR_EN_CP_PD_B  = ~(((CRSH_SEQB_CK_TE << 10) | (CRSH_SEQB_CK_FD << 9) | (CRSH_SEQB_CK_SEG << 0)) & 0x7FF) & 0x7FF;
    mbus_remote_register_write(EID_ADDR,0x16,eid_r16.as_int);

    eid_r17.WCTR_RESETB_CP_C = 1;
    eid_r17.WCTR_VIN_CP_C    = CRSH_SEQC_VIN;
    eid_r17.WCTR_EN_CP_CK_C  =   ((CRSH_SEQC_CK_TE << 10) | (CRSH_SEQC_CK_FD << 9) | (CRSH_SEQC_CK_SEG << 0)) & 0x7FF;
    eid_r17.WCTR_EN_CP_PD_C  = ~(((CRSH_SEQC_CK_TE << 10) | (CRSH_SEQC_CK_FD << 9) | (CRSH_SEQC_CK_SEG << 0)) & 0x7FF) & 0x7FF;
    mbus_remote_register_write(EID_ADDR,0x17,eid_r17.as_int);
}


//-------------------------------------------------------------------
// NFC Functions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Function: nfc_init
// Args    : None
// Description:
//           Initialization for the NFC chip communication
// Return  : None
//-------------------------------------------------------------------
static void nfc_init(void){

    // Initialize passwords; Factory default: I2C_PWD[63:0]=0x0
    i2c_password_upper = 0x0;
    i2c_password_lower = 0x0;

    // Initialize the direction (*R_GPIO_DIR needs to be explicitly reset)
    gpio_set_dir(0x0);

    // Enable the GPIO pads
    set_gpio_pad_with_mask(I2C_MASK,(1<<NFC_SDA)|(1<<NFC_SCL));
    set_gpio_pad_with_mask(GPO_MASK,(1<<NFC_GPO));

    // At this point, SCL/SDA are set to 'input' and TMC does not driving them.
    // They should be properly configured in nfc_power_on().

    // Config GPO wakeup (NOTE: it does not work in TMCv1/PREv22E)
    config_gpio_posedge_wirq((1<<NFC_GPO));

}

//-------------------------------------------------------------------
// Function: nfc_power_on
// Args    : None
// Description:
//           Initialize NFC GPIO pads and turn on the NFC.
// Return  : None
//-------------------------------------------------------------------
static void nfc_power_on(void) {
    // Set directions (1: output, 0: input); Default direction is 0 (input).
    gpio_set_dir_with_mask(I2C_MASK,(1<<NFC_SCL)|(1<<NFC_SDA));

    // Write SCL=SDA=1
    gpio_write_data_with_mask(I2C_MASK,(1<<NFC_SCL)|(1<<NFC_SDA));

    // Power the NFC chip
    // NOTE: At this point, SCL/SDA are still in 'input' mode as GPIO pads are still frozen.
    //       As VNFC goes high, SDA also goes high by the pull-up resistor. SCL remains low.
    set_cps(0x1 << NFC_CPS);

    // Unfreeze the pads
    unfreeze_gpio_out();
}

//-------------------------------------------------------------------
// Function: nfc_power_off
// Args    : None
// Description:
//           Turn off the NFC and freeze the GPIO.
//           After the execution, SCL/SDA are set to 'input'.
// Return  : None
//-------------------------------------------------------------------
static void nfc_power_off(void) {
    // Power off the NFC chip
    set_cps(0x0 << NFC_CPS);
    // Write SCL=SDA=0
    gpio_write_data_with_mask(I2C_MASK,(0<<NFC_SCL)|(0<<NFC_SDA));
    // Set direction (Direction- 1: output, 0: input)
    gpio_set_dir_with_mask(I2C_MASK,(0<<NFC_SCL)|(0<<NFC_SDA));
    // Freeze the pads
    freeze_gpio_out();
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
// Function: nfc_i2c_byte
// Args    : byte   - Byte to be sent (8 bits)
// Description:
//           Send 'byte' from MSB to LSB, then check ACK.
//           At the end of the execution:
//              SCL(output) = 0
//              SDA(input)  = 1 (held by the pull-up resistor)
// Return  : None
//-------------------------------------------------------------------

static void nfc_i2c_byte(uint8_t byte){
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
    stop_timer32_timeout_check(FCODE_I2C_ACK_TIMEOUT);

    gpio_write_data_with_mask(I2C_MASK,1<<NFC_SCL);
    gpio_write_data_with_mask(I2C_MASK,0<<NFC_SCL);
}

//-------------------------------------------------------------------
// Function: nfc_i2c_password()
// Args    : mode   - 0x09: Present password
//                    0x07: Write password
// Description:
//           If mode=0x09, it present the I2C password to open the I2C security session
//           If mode=0x07, it updates the I2C password.
// Return  : none
//-------------------------------------------------------------------

static void nfc_i2c_password(uint32_t mode) {
    uint32_t i2c_dsc   = 0xAE;      // Device Select Code
    uint32_t i2c_addr  = 0x0900;    // Address
    nfc_i2c_start();
    nfc_i2c_byte(i2c_dsc);
    nfc_i2c_byte(i2c_addr>>8);
    nfc_i2c_byte(i2c_addr);
    nfc_i2c_byte(i2c_password_upper >> 24);
    nfc_i2c_byte(i2c_password_upper >> 16);
    nfc_i2c_byte(i2c_password_upper >>  8);
    nfc_i2c_byte(i2c_password_upper >>  0);
    nfc_i2c_byte(i2c_password_upper >> 24);
    nfc_i2c_byte(i2c_password_upper >> 16);
    nfc_i2c_byte(i2c_password_upper >>  8);
    nfc_i2c_byte(i2c_password_upper >>  0);
    nfc_i2c_byte(mode);
    nfc_i2c_byte(i2c_password_lower >> 24);
    nfc_i2c_byte(i2c_password_lower >> 16);
    nfc_i2c_byte(i2c_password_lower >>  8);
    nfc_i2c_byte(i2c_password_lower >>  0);
    nfc_i2c_byte(i2c_password_lower >> 24);
    nfc_i2c_byte(i2c_password_lower >> 16);
    nfc_i2c_byte(i2c_password_lower >>  8);
    nfc_i2c_byte(i2c_password_lower >>  0);
    nfc_i2c_stop();
}

//-------------------------------------------------------------------
// Function: nfc_i2c_present_password(), nfc_i2c_update_password()
// Args    : none
// Description:
//           These are wrapper functions to easily use nfc_i2c_password()
// Return  : none
//-------------------------------------------------------------------

static void nfc_i2c_present_password(void) { nfc_i2c_password(0x09); }
static void nfc_i2c_update_password(void) { nfc_i2c_password(0x07); }


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
// Function: nfc_i2c_byte_read
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit address
// Description:
//           Read data at the address 'addr'
// Return  : 8-bit read data
//-------------------------------------------------------------------

static uint8_t nfc_i2c_byte_read(uint32_t e2, uint32_t addr){
    uint8_t data;
    nfc_i2c_start();
    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
    nfc_i2c_byte(addr >> 8);
    nfc_i2c_byte(addr);
    nfc_i2c_start();
    nfc_i2c_byte(0xA7 | ((e2&0x1) << 3));
    data = nfc_i2c_rd(0);  // must not acknowledge
    nfc_i2c_stop();
    return data;
}

//-------------------------------------------------------------------
// Function: nfc_i2c_byte_write
// Args    : e2     : Bit[3] in Device Select Code
//           addr   : 16-bit address
//           data   : 8-bit data
// Description:
//           Write data at the address 'addr'
// Return  : None
//-------------------------------------------------------------------

static void nfc_i2c_byte_write(uint32_t e2, uint32_t addr, uint8_t data){
    nfc_i2c_start();
    nfc_i2c_byte(0xA6 | ((e2&0x1) << 3));
    nfc_i2c_byte(addr>>8);
    nfc_i2c_byte(addr);
    nfc_i2c_byte(data);
    nfc_i2c_stop();
}

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_write (NOT VERIFIED)
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
    nfc_i2c_byte(0xA6);
    nfc_i2c_byte(addr>>8);
    nfc_i2c_byte(addr);
    for (i=0; i<len; i++) {
        nfc_i2c_byte(data[i]);
    }
    nfc_i2c_stop();
}

//-------------------------------------------------------------------
// Function: nfc_i2c_seq_word_write (NOT YET COMPLETE)
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

static void nfc_i2c_seq_word_write(uint32_t addr, uint32_t data[], uint32_t len){
    uint32_t i;
    nfc_i2c_start();
    nfc_i2c_byte(0xA6);
    nfc_i2c_byte(addr>>8);
    nfc_i2c_byte(addr);
    for (i=0; i<len; i++) {
        nfc_i2c_byte(data[i]);
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
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;

    // Stop any ongoing EID operation (but how...?)
    //eid_r02.ECP_PG_DIODE = 1;
    //mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);

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

    eid_clk_sel_ring = EID_CP_CLK_SEL_RING;
    eid_clk_sel_te_div = EID_CP_CLK_SEL_TE_DIV;
    eid_clk_sel_fd_div = EID_CP_CLK_SEL_FD_DIV;
    eid_clk_sel_seg_div = EID_CP_CLK_SEL_SEG_DIV;

    eid_clear_fd = EID_CP_CLEAR_FD;
    eid_tmr_sel_ldo = EID_TMR_SEL_LDO;
    eid_vin = EID_CP_VIN;


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
#ifdef PMU_TEST
    pmu_init();
#endif

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
#ifdef EID_TEST
    eid_init();
    eid_update(DISP_TRIANGLE);
#endif

    //-------------------------------------------------
    // XO Driver
    //-------------------------------------------------
#ifdef XO_TEST
    xo_start();
#endif

    //-------------------------------------------------
    // E-Ink Display
    //-------------------------------------------------
    //eid_enable_cp_ck(/*te*/0x1, /*fd*/0x0, /*seg*/0x000); 
    //eid_all_white(0);
    //eid_update(DISP_INITIALIZED);

    //-------------------------------------------------
    // NFC 
    //-------------------------------------------------
    nfc_init();

    //-------------------------------------------------
    // Sleep
    //-------------------------------------------------
    // It must go into sleep with a long 'sleep' duration
    // to give user enough time to put a "sticker" over the GOC solar cell.
    //reset_xo_cnt(); // Make counter value = 0
    //operation_sleep_xo_timer(SLEEP_DURATION_LONG);
    //operation_sleep_xo_timer(3*XOT_1SEC);

    //operation_sleep();
    //while(1);
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
    set_flag(FLAG_WD_ENABLED, 1);

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
    mbus_write_message32(0x71, 0);
    *NVIC_ICPR = (0x1 << IRQ_TIMER32);
    #ifdef SIM
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
    #ifdef SIM
    arb_debug_reg(IRQ_TIMER16, 0x00000000);
    #endif
    }
void handler_ext_int_reg0(void) { // REG0
    mbus_write_message32(0x71, 3);
    *NVIC_ICPR = (0x1 << IRQ_REG0);
    #ifdef SIM
    arb_debug_reg(IRQ_REG0, 0x00000000);
    #endif
}
void handler_ext_int_reg1(void) { // REG1
    mbus_write_message32(0x71, 4);
    *NVIC_ICPR = (0x1 << IRQ_REG1);
    #ifdef SIM
    arb_debug_reg(IRQ_REG1, 0x00000000);
    #endif
}
void handler_ext_int_reg2(void) { // REG2
    mbus_write_message32(0x71, 5);
    *NVIC_ICPR = (0x1 << IRQ_REG2);
    #ifdef SIM
    arb_debug_reg(IRQ_REG2, 0x00000000);
    #endif
}
void handler_ext_int_reg3(void) { // REG3
    mbus_write_message32(0x71, 6);
    *NVIC_ICPR = (0x1 << IRQ_REG3);
    #ifdef SIM
    arb_debug_reg(IRQ_REG3, 0x00000000);
    #endif
}
void handler_ext_int_reg4(void) { // REG4
    mbus_write_message32(0x71, 7);
    *NVIC_ICPR = (0x1 << IRQ_REG4);
    #ifdef SIM
    arb_debug_reg(IRQ_REG4, 0x00000000);
    #endif
}
void handler_ext_int_reg5(void) { // REG5
    mbus_write_message32(0x71, 8);
    *NVIC_ICPR = (0x1 << IRQ_REG5);
    #ifdef SIM
    arb_debug_reg(IRQ_REG5, 0x00000000);
    #endif
}
void handler_ext_int_reg6(void) { // REG6
    mbus_write_message32(0x71, 9);
    *NVIC_ICPR = (0x1 << IRQ_REG6);
    #ifdef SIM
    arb_debug_reg(IRQ_REG6, 0x00000000);
    #endif
}
void handler_ext_int_reg7(void) { // REG7
    mbus_write_message32(0x71, 0xA);
    *NVIC_ICPR = (0x1 << IRQ_REG7);
    #ifdef SIM
    arb_debug_reg(IRQ_REG7, 0x00000000);
    #endif
}
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    mbus_write_message32(0x71, 0xB);
    *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM);
    #ifdef SIM
    arb_debug_reg(IRQ_MBUS_MEM, 0x00000000);
    #endif
}
void handler_ext_int_mbusrx(void) { // MBUS_RX
    mbus_write_message32(0x71, 0xC);
    *NVIC_ICPR = (0x1 << IRQ_MBUS_RX);
    #ifdef SIM
    arb_debug_reg(IRQ_MBUS_RX, 0x00000000);
    #endif
}
void handler_ext_int_mbustx(void) { // MBUS_TX
    mbus_write_message32(0x71, 0xD);
    *NVIC_ICPR = (0x1 << IRQ_MBUS_TX);
    #ifdef SIM
    arb_debug_reg(IRQ_MBUS_TX, 0x00000000);
    #endif
}
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    mbus_write_message32(0x71, 0xE);
    *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD);
    #ifdef SIM
    arb_debug_reg(IRQ_MBUS_FWD, 0x00000000);
    #endif
}
void handler_ext_int_gocep(void) { // GOCEP
    mbus_write_message32(0x71, 0xF);
    *NVIC_ICPR = (0x1 << IRQ_GOCEP);
    #ifdef SIM
    arb_debug_reg(IRQ_GOCEP, 0x00000000);
    #endif

    uint32_t i, j;
    uint32_t goc_raw    = *GOC_DATA_IRQ;
    uint32_t goc_header = (goc_raw & 0xFF000000) >> 24;

    // Notification
    mbus_write_message32(0x72, goc_raw);

#ifdef NFC_TEST
    //--------------------------------------------------//
    // NFC
    //--------------------------------------------------//

    //--------------------------------------------------
    //
    // *GOC_DATA_IRQ = 0x0000000n (n = 0x0 ~ 0xF)
    //
    // [Description] 
    //      Various shortcuts (Use Shortcut buttons on LabView)
    //
    // [Arguments]
    //      n = 0x0: Power on NFC
    //      n = 0x1: I2C Present Password
    //      n = 0x2: I2C Update Password (*REG0: Upper 32-bit of the New PW; *REG1: Lower 32-bit of the New PW)
    //      n = 0x3: Read UID
    //      n = 0x4: Read IC_REV
    //      n = 0x5: Read MEM_SIZE
    //      n = 0x6:
    //      n = 0x7:
    //      n = 0x8:
    //      n = 0x9:
    //      n = 0xA:
    //      n = 0xB:
    //      n = 0xC:
    //      n = 0xD:
    //      n = 0xE:
    //      n = 0xF: Power off NFC and go to sleep
    //
    //--------------------------------------------------
    //
    // *GOC_DATA_IRQ = 0x01nnaaaa
    //
    // [Description] 
    //      Write into User Memory (EEPROM)
    //      Data must reside in *(GOC_DATA_IRQ+1) ~ *(GOC_DATA_IRQ+1+nn) 
    //
    // [Arguments]
    //      nn: Number of Words - 1 (nn = 0x00 ~ 0x09)
    //      aa: I2C Byte Address (aa= 0x0000 ~ 0x1FFC for ST25DV64K)
    //
    //--------------------------------------------------
    //
    // *GOC_DATA_IRQ = 0x02nnaaaa
    //
    // [Description] 
    //      Read from User Memory (EEPROM)
    //
    // [Arguments]
    //      nn: Number of Words - 1 (nn = 0x00 ~ 0x09)
    //      aa: I2C Byte Address (aa= 0x0000 ~ 0x1FFC for ST25DV64K)
    //
    //--------------------------------------------------

    if (goc_header == 0x00) {

        uint32_t goc_data   = (goc_raw & 0x00FFFFFF);

        // Enable NFC and stay active (goc_data=0xC) -OR- go sleep (goc_data=0xE)
        if (goc_data == 0x0) {
            nfc_power_on();
        }
        // Send I2C Present Password Command
        else if (goc_data == 0x1) {
            nfc_i2c_present_password();
        }
        // Send I2C Write Password Command
        // NOTE: REG0 must have the upper 32-bit of the new password.
        //       REG1 must have the lower 32-bit of the new password.
        else if (goc_data == 0x2) {
            i2c_password_upper = *REG0;
            i2c_password_lower = *REG1;
            nfc_i2c_update_password();
        }
        // Read UID
        // For ST25DV64K, the returned data must be 0xE00227xx_xxxxxxxx, 
        // where xx_xxxxxxxx is a 40-bit unique serial number.
        else if (goc_data == 0x3) {
            uint32_t upper, lower;

            upper = nfc_i2c_byte_read(/*E2*/1, /*ADDR*/0x001F); // Byte 7
            upper = (upper << 8) | nfc_i2c_byte_read(/*E2*/1, /*ADDR*/0x001E); // Byte 6
            upper = (upper << 8) | nfc_i2c_byte_read(/*E2*/1, /*ADDR*/0x001D); // Byte 5
            upper = (upper << 8) | nfc_i2c_byte_read(/*E2*/1, /*ADDR*/0x001C); // Byte 4

            lower = nfc_i2c_byte_read(/*E2*/1, /*ADDR*/0x001B); // Byte 3
            lower = (lower << 8) | nfc_i2c_byte_read(/*E2*/1, /*ADDR*/0x001A); // Byte 2
            lower = (lower << 8) | nfc_i2c_byte_read(/*E2*/1, /*ADDR*/0x0019); // Byte 1
            lower = (lower << 8) | nfc_i2c_byte_read(/*E2*/1, /*ADDR*/0x0018); // Byte 0

            mbus_write_message32(0x80, upper);
            mbus_write_message32(0x81, lower);

        }
        // Read IC_REV
        // For the specific ST25DV64K being tested, the returned data is 0x12.
        else if (goc_data == 0x4) {
            uint32_t data = nfc_i2c_byte_read(/*E2*/0x1, /*ADDR*/0x0020);
            mbus_write_message32(0x80, data);
        }
        // Read MEM_SIZE
        // For ST25DV64K, the returned data must be 0x07FF.
        else if (goc_data == 0x5) {
            uint32_t msb = nfc_i2c_byte_read(/*E2*/0x1, /*ADDR*/0x0015); // MSB Bytes
            uint32_t lsb = nfc_i2c_byte_read(/*E2*/0x1, /*ADDR*/0x0014); // LSB Bytes
            mbus_write_message32(0x80, (msb << 8) | lsb);
        }
        // Go to sleep (also turns off NFC)
        else if (goc_data == 0xF) {
            operation_sleep();
        }
    }

    else if (goc_header == 0x01) {
        uint32_t num_words_1 = (goc_raw >> 16) & 0xFF;
        uint32_t addr = goc_raw & 0xFFFF;
        uint32_t data;

        for (i=1; i<(num_words_1+2); i++) {
            data = *(GOC_DATA_IRQ + i);
            for (j=0; j<4; j++) {
                nfc_i2c_byte_write(/*E2*/0, addr, (data & 0xFF));
                // Each Byte Write must be followed by a delay of >5ms (tw)
                // Sequential Write should be much faster (See datasheet)
                delay(NFC_TW);
                addr++;
                data = data >> 8;
            }
        }
    }

    else if (goc_header == 0x02) {
        uint32_t num_words_1 = (goc_raw >> 16) & 0xFF;
        uint32_t addr = goc_raw & 0xFFFF;
        uint32_t data;
        for (i=0; i<(num_words_1+1); i++) {
            data = 0;
            for (j=0; j<4; j++) {
                data = (data >> 8) | (nfc_i2c_byte_read(/*E2*/0, addr) << 24);
                addr++;
            }
            mbus_write_message32((0x80 | i), data);
        }
    }
#endif


#ifdef PMU_TEST
    //--------------------------------------------------//
    // PMU
    //--------------------------------------------------//

    //--------------------------
    // Shorcuts
    //--------------------------
    if (goc_header == 0x00) {

        // GOC_DATA = 0x0, 0x1, 0x2, 0x3
        // Description: Update CP_CONFIG[11:10] = SAR_V1P2_DIV_SEL[1:0] (Default: 0x0)
        if (goc_data < 0x4) {
            // Read Reg#72 (CP_CONFIG)
            uint32_t orig = pmu_reg_read(72);
            // Update the value and re-write
            pmu_reg_write(72, (orig & 0xFFFFF3FF) | ((goc_data & 0x3) << 10));
        }
        // GOC_DATA = 0xA
        // Description: Make all Sleep Floors Default
        else if (goc_data == 0xA) {
            //---------------------------------------------------------------------------------------
            // SAR_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x15,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (0 << 19)     // 1'h0     // Enable PFM even during periodic reset
                | (0 << 18)     // 1'h0     // Enable PFM even when VREF is not used as reference
                | (0 << 17)     // 1'h0     // Enable PFM
                | (3 << 14)     // 3'h3     // Comparator clock division ratio
                | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x2 << 9)    // 4'h2     // Frequency multiplier R
                | (0x2 << 5)    // 4'h2     // Frequency multiplier L (actually L+1)
                | (0x06)        // 5'h06    // Floor frequency base (0-63)
            ));
        
            //---------------------------------------------------------------------------------------
            // UPC_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x17,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (3 << 14)     // 2'h0     // Desired Vout/Vin ratio
                | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x1 << 9)    // 4'h1     // Frequency multiplier R
                | (0x0 << 5)    // 4'h0     // Frequency multiplier L (actually L+1)
                | (0x04)        // 5'h04    // Floor frequency base (0-63)
            ));
        
        
            //---------------------------------------------------------------------------------------
            // DNC_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x19,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x2 << 9)    // 4'h2     // Frequency multiplier R
                | (0x2 << 5)    // 4'h2     // Frequency multiplier L (actually L+1)
                | (0x02)        // 5'h02    // Floor frequency base (0-63)
            ));
        }
        // GOC_DATA = 0xE
        // Description: Make all Sleep Floors 1/0/1
        else if (goc_data == 0xE) {
            //---------------------------------------------------------------------------------------
            // SAR_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x15,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (0 << 19)     // 1'h0     // Enable PFM even during periodic reset
                | (0 << 18)     // 1'h0     // Enable PFM even when VREF is not used as reference
                | (0 << 17)     // 1'h0     // Enable PFM
                | (3 << 14)     // 3'h3     // Comparator clock division ratio
                | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x1 << 9)    // 4'h2     // Frequency multiplier R
                | (0x0 << 5)    // 4'h2     // Frequency multiplier L (actually L+1)
                | (0x01)        // 5'h06    // Floor frequency base (0-63)
            ));
        
            //---------------------------------------------------------------------------------------
            // UPC_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x17,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (3 << 14)     // 2'h0     // Desired Vout/Vin ratio
                | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x1 << 9)    // 4'h1     // Frequency multiplier R
                | (0x0 << 5)    // 4'h0     // Frequency multiplier L (actually L+1)
                | (0x01)        // 5'h04    // Floor frequency base (0-63)
            ));
        
        
            //---------------------------------------------------------------------------------------
            // DNC_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x19,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x1 << 9)    // 4'h2     // Frequency multiplier R
                | (0x0 << 5)    // 4'h2     // Frequency multiplier L (actually L+1)
                | (0x01)        // 5'h02    // Floor frequency base (0-63)
            ));
        }
        // GOC_DATA = 0xF
        // Description: Make all Sleep Floors 0/0/0
        else if (goc_data == 0xF) {
            //---------------------------------------------------------------------------------------
            // SAR_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x15,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (0 << 19)     // 1'h0     // Enable PFM even during periodic reset
                | (0 << 18)     // 1'h0     // Enable PFM even when VREF is not used as reference
                | (0 << 17)     // 1'h0     // Enable PFM
                | (3 << 14)     // 3'h3     // Comparator clock division ratio
                | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x0 << 9)    // 4'h2     // Frequency multiplier R
                | (0x0 << 5)    // 4'h2     // Frequency multiplier L (actually L+1)
                | (0x00)        // 5'h06    // Floor frequency base (0-63)
            ));
        
            //---------------------------------------------------------------------------------------
            // UPC_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x17,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (3 << 14)     // 2'h0     // Desired Vout/Vin ratio
                | (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x0 << 9)    // 4'h1     // Frequency multiplier R
                | (0x0 << 5)    // 4'h0     // Frequency multiplier L (actually L+1)
                | (0x00)        // 5'h04    // Floor frequency base (0-63)
            ));
        
        
            //---------------------------------------------------------------------------------------
            // DNC_TRIM_V3_SLEEP
            //---------------------------------------------------------------------------------------
            pmu_reg_write(0x19,  // Default  // Description
            //---------------------------------------------------------------------------------------
                ( (0 << 13)     // 1'h0     // Makes the converter clock 2x slower
                | (0x0 << 9)    // 4'h2     // Frequency multiplier R
                | (0x0 << 5)    // 4'h2     // Frequency multiplier L (actually L+1)
                | (0x00)        // 5'h02    // Floor frequency base (0-63)
            ));
        }
    }
    //--------------------------
    // General Register Write
    //--------------------------
    // goc_header = 0x0C: REFERENCE_TRIM (Default: 7'b001_1000 (0x000018))
    //--------------------
    // goc_header = 0x0F: SAR_TRIM_SLEEP (Default: 17'b0 1010 0010 0000 0100 (0x00A204): Override SAR_EN_ACC_VOUT = 0
    //                                             17'b0 1011 0010 0000 0100 (0x00B204): Override SAR_EN_ACC_VOUT = 1
    //--------------------------
    // goc_header = 0x3B: CTRL_DESIRED_STATE_SLEEP
    //                    --------------------------------------------
    //                     Turn off ADC Offset    : 1 0111 1110 1111 1111 1111 (0x17EFFF)
    //                     Turn off ADC completely: 1 0111 1100 0111 1111 1111 (0x17C7FF)
    //                    --------------------------------------------
    //                     [20] horizon                     1       0
    //                     [19] vbat_read_only              0       0
    //                     [18] vdd_0p6_turned_on           1       1
    //                     [17] vdd_1p2_turned_on           1       1
    //                     [16] vdd_3p6_turned_on           1       1
    //                     [15] dnc_stabilized              1       1
    //                     [14] dnc_on                      1       1
    //                     [13] sar_ratio_adjusted          1       0
    //                     [12] adc_adjusted                1       0
    //                     [11] adc_output_ready            1       1
    //                     [10] refgen_on                   1       1
    //                     [9]  upc_stabilized              1       1
    //                     [8]  upc_on                      1       1
    //                     [7]  control_supply_switched     1       1
    //                     [6]  clock_supply_switched       1       1
    //                     [5]  sar_ratio_roughly_adjusted  1       1
    //                     [4]  sar_stabilized              1       1
    //                     [3]  sar_reset                   1       1
    //                     [2]  wait_for_cap_charge         1       1
    //                     [1]  wait_for_clock_cycles       1       1
    //                     [0]  sar_on                      1       1
    //                    --------------------------------------------
    //--------------------------
    // goc_header = 0x48: CP_CONFIG (Default: 0000 0011 1111) (0x00003F / 0x00043F / 0x00083F / 0x000C3F)
    //                    --------------------------------------------
    //                      [11:10] SAR_V1P2_DIV_SEL[1:0] (Default 2'b00)
    //                      [9:7]   SAR/UPC/DNC_CMP_EN_OVERRIDE (Default 3'b000)
    //                      [6:4]   SAR_CP_CAP (Default: 0x3)
    //                      [3:2]   UPC_CP_CAP (Default: 0x3)
    //                      [1:0]   DNC_CP_CAP (Default: 0x3)
    //--------------------------
    else if (goc_header < 73) {
        pmu_reg_write(goc_header, goc_data);
    }
    // goc_header = 0x73
    // Description: Update REFERENCE_TRIM more easily
    else if (goc_header == 0x73) {
        // Reg0x0C: REFERENCE_TRIM (Default: 7'b001_1000 (0x000018))
        //  goc_data[9:8] is written to REFERENCE_TRIM[6:5]
        //  goc_data[4:0] is written to REFERENCE_TRIM[4:0]
        pmu_reg_write(12, ((goc_data >> 3) & 0x00000060) | (goc_data & 0x1F));
    }
    // goc_header = 0x74
    // Description: Update SAR_V1P2_DIV_SEL more easily
    //  goc_data[1:0] is written to SAR_V1P2_DIV_SEL[1:0]
    // Reg0x48: CP_CONFIG (Default: 0000 0011 1111) (0x00003F / 0x00043F / 0x00083F / 0x000C3F)
    //                    --------------------------------------------
    //                      [11:10] SAR_V1P2_DIV_SEL[1:0] (Default 2'b00)
    //                      [9:7]   SAR/UPC/DNC_CMP_EN_OVERRIDE (Default 3'b000)
    //                      [6:4]   SAR_CP_CAP (Default: 0x3)
    //                      [3:2]   UPC_CP_CAP (Default: 0x3)
    //                      [1:0]   DNC_CP_CAP (Default: 0x3)
    else if (goc_header == 0x74) {
        pmu_reg_write(0x48, ((goc_data & 0x3) << 10) | 0x3F);
    }
    //--------------------------
    // General Register Read
    //--------------------------
    else if (goc_header == 0xFF) {
        pmu_reg_read(goc_data);
    }
#endif

#ifdef XO_TEST
    //--------------------------------------------------//
    // XO
    //--------------------------------------------------//
    if (goc_header == 0x00) {
        if (goc_data == 0x1) { 
            *XOT_STOP_COUT = 1;
            xo_stop();
            //---------------------------
            xo_start();
            *XOT_START_COUT = 1;
            operation_sleep();
            while(1);
        }
    }
#endif

#ifdef EID_TEST
    //--------------------------------------------------//
    // e-Ink Display
    // NOTE: There are total 9 SEG CPs in EIDv1. 
    //       e-Ink display has 7 segments.
    //--------------------------------------------------//
    if (goc_header == 0x00) {
        /*[LLL]*/ if      (goc_data == 0x0) { eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x000); }
        /*[LLH]*/ else if (goc_data == 0x1) { eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x1FF); }
        /*[LHL]*/ else if (goc_data == 0x2) { eid_enable_cp_ck(/*te*/0, /*fd*/1, /*seg*/0x000); }
        /*[LHH]*/ else if (goc_data == 0x3) { eid_enable_cp_ck(/*te*/0, /*fd*/1, /*seg*/0x1FF); }
        /*[HLL]*/ else if (goc_data == 0x4) { eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x000); }
        /*[HLH]*/ else if (goc_data == 0x5) { eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x1FF); }
        /*[HHL]*/ else if (goc_data == 0x6) { eid_enable_cp_ck(/*te*/1, /*fd*/1, /*seg*/0x000); }
        /*[HHH]*/ else if (goc_data == 0x7) { eid_enable_cp_ck(/*te*/1, /*fd*/1, /*seg*/0x1FF); }
        /*Demo Mode*/
        else if (goc_data == 0xF) {
            while(1) {
                // Triangle
                eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x000); 
                eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x001); 
                // X-mark
                eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x000); 
                eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x01A); 
                // Low Battery
                eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x000); 
                eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x004); 
                // Plus Sign
                eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x000); 
                eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x020); 
                // Minus Sign
                eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x000); 
                eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x040); 
                // non-X
                eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x000); 
                eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x065); 
                // only-X
                eid_enable_cp_ck(/*te*/1, /*fd*/0, /*seg*/0x000); 
                eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x01A); 
            }
        }
        /*Individual Segment*/
        else if (goc_data > 0x7) {
            eid_enable_cp_ck(/*te*/0, /*fd*/0, /*seg*/0x1 << (goc_data - 8)); 
        }
    }
    // Direct Control
    else if (goc_header == 0x01) {
        uint32_t old_eid_vin = eid_vin;
        eid_vin = (goc_data >> 11) & 0x1;
        eid_enable_cp_ck(   /*te*/ ((goc_data >> 10) & 0x1),
                            /*fd*/ ((goc_data >>  9) & 0x1),
                            /*seg*/((goc_data >>  0) & 0x1FF)
                        ); 
        eid_vin = old_eid_vin;
    }
    // Pulse Width Tuning
    else if (goc_header == 0x02) {
        eid_set_pulse_width(goc_data & 0xFFFF);
    }
    // Clock Tuning
    else if (goc_header == 0x03) {
        eid_clk_sel_ring    = (goc_data >> 6) & 0x3;
        eid_clk_sel_te_div  = (goc_data >> 4) & 0x3;
        eid_clk_sel_fd_div  = (goc_data >> 2) & 0x3;
        eid_clk_sel_seg_div = (goc_data >> 0) & 0x3;
        eid_r02.ECP_SEL_RING    = eid_clk_sel_ring;
        eid_r02.ECP_SEL_TE_DIV  = eid_clk_sel_te_div;
        eid_r02.ECP_SEL_FD_DIV  = eid_clk_sel_fd_div;
        eid_r02.ECP_SEL_SEG_DIV = eid_clk_sel_seg_div;
        mbus_remote_register_write(EID_ADDR,0x02,eid_r02.as_int);
    }
    // Enable/Disable ClearFD feature
    else if (goc_header == 0x04) {
        eid_clear_fd = goc_data & 0x1;
    }
    // VIN Selection
    else if (goc_header == 0x05) {
        eid_vin = goc_data & 0x1;
    }
    // TMR_SEL_LDO Selection (1: Use VBAT, 0: User V1P2)
    else if (goc_header == 0x06) {
        eid_r01.TMR_SEL_LDO = goc_data & 0x1;
        mbus_remote_register_write(EID_ADDR,0x01,eid_r01.as_int);
    }
    // TMR_EN_CLK_OUT 
    else if (goc_header == 0x07) {
        eid_r00.TMR_EN_CLK_OUT = goc_data & 0x1;
        mbus_remote_register_write(EID_ADDR,0x00,eid_r00.as_int);
    }
    // Generic MBus Message
    else {
        mbus_remote_register_write(EID_ADDR,goc_header,goc_data);
        delay(2*DLY_1S);
    }
#endif
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    mbus_write_message32(0x71, 0x10);
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET);
    #ifdef SIM
    arb_debug_reg(IRQ_SOFT_RESET, 0x00000000);
    #endif
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    mbus_write_message32(0x71, 0x11);
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP);
    #ifdef SIM
    arb_debug_reg(IRQ_WAKEUP, (0x10 << 24) | wakeup_source);
    #endif
}
void handler_ext_int_aes(void) { // AES
    mbus_write_message32(0x71, 0x12);
    *NVIC_ICPR = (0x1 << IRQ_AES);
    #ifdef SIM
    arb_debug_reg(IRQ_SPI, 0x00000000);
    #endif
}
void handler_ext_int_spi(void) { // SPI
    mbus_write_message32(0x71, 0x13);
    *NVIC_ICPR = (0x1 << IRQ_SPI);
    #ifdef SIM
    arb_debug_reg(IRQ_SPI, 0x00000000);
    #endif
}
void handler_ext_int_gpio(void) { // GPIO
    mbus_write_message32(0x71, 0x14);
    *NVIC_ICPR = (0x1 << IRQ_GPIO);
    #ifdef SIM
    arb_debug_reg(IRQ_GPIO, 0x00000000);
    #endif
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    wakeup_count++;
    mbus_write_message32(0x70, wakeup_count);

    // Disable Watchdog Timers
    *TIMERWD_GO  = 0;   // Disable the CPU watchdog
    *REG_MBUS_WD = 0;   // Disable the MBus watchdog by setting Threshold=0.
    
    // Get the info on who woke up the system
    wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1; // reset WAKEUP_SOURCE register

    // EID Watchdog Check-In
#ifdef GIT_TEST 
    if (get_flag(FLAG_WD_ENABLED)) eid_check_in();
#endif

    // For safety, disable Wakeup Timer's WREQ.
#ifdef XO_TEST
    xot_disable_wreq();
#endif

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_GOCEP) | (0x1 << IRQ_TIMER32);

    // If this is the very first wakeup, initialize the system and go back to sleep
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    // Unfreeze GPIO
    //nfc_power_on();

    // Enable GIT, if GIT is not yet enabled 
    // NOTE: This must be done after the very first wakeup by the wakeup timer in real usage,
    //       to provide sufficiet time for the user to put a "sticker" on the photo diode.
#ifdef GIT_TEST
    if (!get_flag(FLAG_GIT_ENABLED)) {
        *REG_GOC_CONFIG = set_bit(*REG_GOC_CONFIG, 16, 1);
        set_flag(FLAG_GIT_ENABLED, 1);
    }
#endif

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
    //              NOTE: If GIT is triggered while the system is in active, the GIT is NOT immediately handled.
    //                    Instead, it waits until the system goes in sleep, and then, the (pending) GIT will wake up the system.
    //                    Thus, you can safely assume that GIT is (effectively) triggered only while the system is in Sleep.
    //      [4] - MBus message has triggered wakeup (e.g., Flash Auto Boot-up)
    //      [3] - One of GPIO_PAD[3:0] has triggered wakeup
    //      [2] - XO Timer has triggered wakeup
    //      [1] - Wake-up Timer has triggered wakeup
    //      [0] - GOC/EP has triggered wakeup
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // If woken up by GIT
    //--------------------------------------------------------------------------
#ifdef GIT_TEST
    if (get_bit(wakeup_source, 5)) {
        mbus_write_message32(0x72, 0x4);
        set_flag(FLAG_GIT_TRIGGERED, 1);
        // Reset everything back to default.
        //operation_back_to_default(); // Enable this in the final firmware.
        // Post-GIT Initialization
        postgit_init();
        // Start Temp/VBAT measurement
        snt_running = 1;
        meas_count = 0;
    }
#endif
//    //--------------------------------------------------------------------------
//    // If woken up by a XO wakeup timer
//    //--------------------------------------------------------------------------
//    else if (get_bit(wakeup_source, 2)) {
//        mbus_write_message32(0x72, 0x5);
//
//        if (wakeup_timestamp==0) wakeup_timestamp = *XOT_VAL;
//
//        //// If GIT is not yet enabled (i.e., the very first wakeup by the wakeup timer)
//        //if (!get_flag(FLAG_GIT_ENABLED)) {
//        //    // Enable GIT (GOC Instant Trigger)
//        //    *REG_GOC_CONFIG = set_bit(*REG_GOC_CONFIG, 16, 1);
//        //    set_flag(FLAG_GIT_ENABLED, 1);
//        //    // Start Temp/VBAT measurement
//        //    snt_running = 1;
//        //    meas_count = 0;
//        //}
//    }
    //--------------------------------------------------------------------------
    // If woken up by NFC (GPIO[0])
    //--------------------------------------------------------------------------
    // NOTE: See note in 'THINGS TO DO'
    if (get_bit(wakeup_source, 3)) {
        if (get_bit(wakeup_source, 8)) {
            mbus_write_message32(0x72, 0x6);
            // Handle the NFC event
            //do_nfc();
        }
        // ERROR: other than GPIO[0] woke up the system
        else {
            mbus_write_message32(0x72, 0x7);
        }
    }
//    //--------------------------------------------------------------------------
//    // If woken up by a wakeup timer [ERROR]
//    //--------------------------------------------------------------------------
//    else if (get_bit(wakeup_source, 1)) {
//        mbus_write_message32(0x72, 0x8);
//    }
//    //--------------------------------------------------------------------------
//    // If woken up by an MBus Message
//    //--------------------------------------------------------------------------
//    else if (get_bit(wakeup_source, 4)) { 
//        mbus_write_message32(0x72, 0x9);
//    }
//    //--------------------------------------------------------------------------
//    // If woken up by GOC/EP
//    //--------------------------------------------------------------------------
//    else if (get_bit(wakeup_source, 0)) { 
//        mbus_write_message32(0x72, 0xA);
//        // GOC/EP shall be handled by the GOC/EP IRQ Handler.
//    }
//
//    //--------------------------------------------------------------------------
//    // OTHER OPERATIONS
//    //--------------------------------------------------------------------------
//    mbus_write_message32(0x72, 0xB);
//
//    //// If SNT is running
//    //while (snt_running) snt_operation();
//
//    reset_xo_cnt(); // Make counter value = 0
//    operation_sleep_xo_timer(3*XOT_1SEC);


// Some GIT Demo
#ifdef GIT_DEMO
    if (get_flag(FLAG_GIT_TRIGGERED)) {
        eid_update(DISP_CHECK);
        if (exec_count < 10) set_wakeup_timer(10, 1, 1);
    }
#endif

    //delay(100000);

    //set_wakeup_timer(20, 1, 1);
    //operation_sleep();

    // Never Quit (should not stay here for an extended duration)
    while(1) asm("nop");
    return 1;
}
