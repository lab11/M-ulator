//*******************************************************************************************
// CISv4Br1 Initial Bringup
//-------------------------------------------------------------------------------------------
// PREv23E -> RDCv4 -> SNTv6r1 -> External -> PMUv13r2
//-------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Jan 05 2023 - Version 3.00
//                  - Hard-forked & modified from xt1_firmware_v2p10
//  See Google Doc for detailed update history.
//-------------------------------------------------------------------------------------------
//
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
//  0x50    wakeup_source                       Wakeup Source (See above WAKEUP_SOURCE definition)
//                                                  0x0001: GOC/EP
//                                                  0x0002: PRE Wakeup Timer
//                                                  0x0010: SNT Timer (MBus)
//                                                  0x0108: NFC (GPIO[0])
//
//  0x51    value                               Measurement Count
//
//  -----------------------------------------------------------------------------------------
//  < SNT >
//  -----------------------------------------------------------------------------------------
//  0x80    value                               Temperature Measurement (Raw Data)
//  0x8E    value                               SNT Timer Sync Read value 
//  0x8F    snt_threshold                       Going into sleep with snt_threshold
//
//  -----------------------------------------------------------------------------------------
//  < RDC >
//  -----------------------------------------------------------------------------------------
//  0x91    value                               1st Pressure Measurement (Raw Data)
//  0x92    value                               2nd Pressure Measurement (Raw Data)
//  0x93    value                               3rd Pressure Measurement (Raw Data)
//  0x94    value                               4th Pressure Measurement (Raw Data)
//
//  -----------------------------------------------------------------------------------------
//  < PMU >
//  -----------------------------------------------------------------------------------------
//  0xA4    value                               PMU SAR Ratio is changing
//                                                  [15: 8] Previous SAR Ratio
//                                                  [ 7: 0] New SAR Ratio
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

#include "CISv4Br1.h"

//*******************************************************************************************
// DEVEL Mode
//*******************************************************************************************
// Enable 'DEVEL' for the following features:
//      - Send debug messages
//      - Use default values rather than grabbing the values from EEPROM
#define DEVEL
//#define ENABLE_GOC_CRC                    // Enable GOC CRC Calculation
//#define ENABLE_RAW                        // Enable support for SAMPLE_RAW
//#define ENABLE_XO_PAD
//#define ENABLE_EID_WATCHDOG               // Enable EID Watchdog
//#define USE_SHORT_REFRESH
//#define ENABLE_DEBUG_SYSTEM_CONFIG
//#define DEBUG_TEMP_CALIB                  // Display when eeprom_temp_calib changes
//#define DEBUG_AES_KEY                     // Display when aes_key[3:0] changes
//#define DEBUG_RAW                         // Display Raw Sample information (ENABLE_RAW must be enabled)
#define WAKEUP_INTERVAL_IN_MINUTES          // The unit of Wakeup Intervals is 'minutes'. If undefined, the unit is 'seconds'

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
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1
#define FLAG_XO_INITIALIZED     2
#define FLAG_WD_ENABLED         3   // Only if ENABLE_EID_WATCHDOG is enabled.
#define FLAG_MAIN_CALLED_BY_SNT 4   // Sleep has been bypassed before the current active session. Thus, main() has called from within operation_sleep(). This implies that xo_val_curr is inaccurate.
#define FLAG_INVLD_XO_PREV      5   // Sleep has been bypassed before the previous active session. This implies that xo_val_prev is inaccurate.
#define FLAG_BOOTUP_DONE        6
#define FLAG_MAIN_CALLED_BY_NFC 7   // Same as FLAG_MAIN_CALLED_BY_SNT. The only difference is that it directly calls main() because it detects a GPO pulse during the 1st wakeup.
#define FLAG_TEMP_MEAS_PEND     8   // Indicates that the system has gone sleep for temperature measurement

//*******************************************************************************************
// XO, SNT WAKEUP TIMER AND SLEEP DURATIONS
//*******************************************************************************************

// XO Frequency
#define XO_FREQ_SEL 0                       // XO Frequency (in kHz) = 2 ^ XO_FREQ_SEL
#define XO_FREQ     1<<(XO_FREQ_SEL+10)     // XO Frequency (in Hz)

// XO Initialization Wait Duration
#define XO_WAIT_A   2*DLY_1S    // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S    // Must be ~1 second delay.

#ifdef WAKEUP_INTERVAL_IN_MINUTES // Intervals are given in minutes
    // Pre-defined Wakeup Intervals
    #define WAKEUP_INTERVAL_ACTIVE      1   // (Default: 1) Wakeup Interval for XT1_ACTIVE (unit: minutes)
    #ifdef USE_SHORT_REFRESH
        #define WAKEUP_INTERVAL_IDLE    5   // (Default: 720) Wakeup Interval for XT1_IDLE (unit: minutes)
    #else
        #define WAKEUP_INTERVAL_IDLE    720    // (Default: 720) Wakeup Interval for XT1_IDLE (unit: minutes)
    #endif
#else // Intervals are given in seconds
    #define WAKEUP_INTERVAL_IDLE_IN_SECONDS 30
    // Pre-defined Wakeup Intervals (unit: seconds)
    #define WAKEUP_INTERVAL_ACTIVE      60
    #define WAKEUP_INTERVAL_IDLE        WAKEUP_INTERVAL_IDLE_IN_SECONDS
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
#define MAIN_CALLED_BY_SNT  (WAKEUP_BY_SNT && get_bit(wakeup_source, 7))
#define MAIN_CALLED_BY_NFC  (WAKEUP_BY_NFC && get_bit(wakeup_source, 7))

//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************

volatile uint32_t meas_count;

//-------------------------------------------------------------------------------------------
// XO/SNT Counters and Calibration
//-------------------------------------------------------------------------------------------

volatile uint32_t snt_freq;                         // SNT Timer Frequency (Hz)
volatile uint32_t snt_duration;                     // SNT counter value that corresponds to the given interval
volatile uint32_t snt_threshold;                    // SNT Timer Threshold to wake up the system

//-------------------------------------------------------------------------------------------
// Other Global Variables
//-------------------------------------------------------------------------------------------

//--- Wakeups
volatile uint32_t wakeup_source;            // Wakeup Source. Updated each time the system wakes up. See 'WAKEUP_SOURCE definition'.

//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep/IRQ Handling
static void operation_sleep (void);
static void operation_sleep_snt_timer(void);
static void operation_init(void);

//--- SNT Temperature Measurement
static void meas_temp (void);

//--- RDC Temperature Measurement
static void meas_rdc (void);

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
    mbus_sleep_all(); 
    while(1);
}

//-------------------------------------------------------------------
// Function: operation_sleep_snt_timer
// Args    : None
// Description:
//           Goes into sleep with SNT Timer enabled
//           Auto Reset is disabled.
//           snt_threshold is set to snt_threshold + snt_duration.
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep_snt_timer(void) {
    snt_threshold = snt_set_wup_timer(/*auto_reset*/0, /*threshold*/snt_threshold + snt_duration);
    mbus_write_message32(0x8F, snt_threshold);
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
        *REG_SYS_CONF =       (0x0 << 9)    // 1'h1     NO_SLEEP_WITH_PEND_IRQ  #If 1, it replaces the sleep message with a selective wakeup message if there is a pending M0 IRQ. If 0, it goes to sleep even when there is a pending M0 IRQ.
                            | (0x0 << 8)    // 1'h0     ENABLE_SOFT_RESET	#If 1, Soft Reset will occur when there is an MBus Memory Bulk Write message received and the MEM Start Address is 0x2000
                            | (0x1 << 7)    // 1'h1     PUF_CHIP_ID_SLEEP
                            | (0x1 << 6)    // 1'h1     PUF_CHIP_ID_ISOLATE
                            | (0x8 << 0);   // 5'h1E    WAKEUP_ON_PEND_REQ	#[4]: GIT (PRE_E Only), [3]: GPIO (PRE only), [2]: XO TIMER (PRE only), [1]: WUP TIMER, [0]: GOC/EP

        //-------------------------------------------------
        // Enumeration
        //-------------------------------------------------
        set_halt_until_mbus_trx();
        mbus_enumerate(RDC_ADDR);
        mbus_enumerate(SNT_ADDR);
        mbus_enumerate(PMU_ADDR);
        set_halt_until_mbus_tx();

        //-------------------------------------------------
        // Target Register Index
        //-------------------------------------------------
        mbus_remote_register_write(PMU_ADDR, 0x52, (0x10 << 8) | PMU_TARGET_REG_IDX);
        mbus_remote_register_write(SNT_ADDR, 0x07, (0x10 << 8) | SNT_TARGET_REG_IDX);
        mbus_remote_register_write(RDC_ADDR, 0x01, (0x10 << 8) | RDC_TARGET_REG_IDX);

        //-------------------------------------------------
        // PMU Settings
        //-------------------------------------------------
        pmu_init();

        //-------------------------------------------------
        // RDC Settings
        //-------------------------------------------------
        rdc_init();

        //-------------------------------------------------
        // SNT Settings
        //-------------------------------------------------
        snt_init();

        // Turn on the SNT timer clock
        snt_start_timer(/*wait_time*/DLY_1S);

        // Start the SNT counter
        snt_enable_wup_timer(/*auto_reset*/0);

        //-------------------------------------------------
        // Go to Sleep
        //-------------------------------------------------
        // Update the flag
        set_flag(FLAG_ENUMERATED, 1);

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

        // Reset meas_count
        meas_count = 0;

        // Initial SNT Frequency = 1400Hz
        snt_freq = 1400;
        // Sleep Duration = 60s; snt_duration = 60 x snt_freq
        snt_duration =    (snt_freq << 5)   // 32
                        + (snt_freq << 4)   // 16
                        + (snt_freq << 3)   // 8
                        + (snt_freq << 2);  // 4
        // Initial SNT Threshold
        snt_threshold = snt_read_wup_timer();

        //-------------------------------------------------
        // End of Initialization
        //-------------------------------------------------
        set_flag(FLAG_INITIALIZED, 1);

        //-------------------------------------------------
        // Go to sleep
        //-------------------------------------------------
        operation_sleep_snt_timer();
    }
}


//-------------------------------------------------------------------
// SNT Temperature Measurement
//-------------------------------------------------------------------

static void meas_temp (void) {

    // Enable REG IRQ
    enable_reg_irq(SNT_TARGET_REG_IDX); // REG1

    // Turn on SNT Temperature Sensor
    snt_temp_sensor_power_on();

    // Release the reset for the Temp Sensor
    snt_temp_sensor_start();

    WFI();

    // Reset & Turn off the temperature sensor
    snt_temp_sensor_power_off();

    // Result
    mbus_write_message32(0x80, (*SNT_TARGET_REG_ADDR) & 0xFFFF);

}

//-------------------------------------------------------------------
// RDC Pressure Measurement
//-------------------------------------------------------------------

static void meas_rdc (void) {

    uint32_t read_data_rdc;
    uint32_t read_data_rdc2;
    uint32_t read_data_rdc3;
    uint32_t read_data_rdc4;

	rdc_power_on();

	rdc_r25.RDC_OFFSET_P_LC = 0x1;
	rdc_r25.RDC_OFFSET_PB_LC = (~0x1)&0x1F;
	mbus_remote_register_write(RDC_ADDR,0x25,rdc_r25.as_int);

	// 1st Measure
	read_data_rdc = rdc_start_meas();

	rdc_r25.RDC_OFFSET_P_LC = 0xB;
	rdc_r25.RDC_OFFSET_PB_LC = (~0xB)&0x1F;
	mbus_remote_register_write(RDC_ADDR,0x25,rdc_r25.as_int);

	// 2nd Measure
	read_data_rdc2 = rdc_start_meas();

	rdc_r25.RDC_OFFSET_P_LC = 0x15;
	rdc_r25.RDC_OFFSET_PB_LC = (~0x15)&0x1F;
	mbus_remote_register_write(RDC_ADDR,0x25,rdc_r25.as_int);

	// 3rd Measure
	read_data_rdc3 = rdc_start_meas();

	rdc_r25.RDC_OFFSET_P_LC = 0x1F;
	rdc_r25.RDC_OFFSET_PB_LC = 0x0;
	mbus_remote_register_write(RDC_ADDR,0x25,rdc_r25.as_int);

	// 4th Measure
	read_data_rdc4 = rdc_start_meas();

	rdc_power_off();

    // Result
    mbus_write_message32(0x91, read_data_rdc);
    mbus_write_message32(0x92, read_data_rdc2);
    mbus_write_message32(0x93, read_data_rdc3);
    mbus_write_message32(0x93, read_data_rdc4);
}

//*******************************************************************************************
// INTERRUPT HANDLERS
//*******************************************************************************************

//void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_aes      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));
//void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_reg0 (void) { *NVIC_ICER = 0xFFFFFFFF; }
void handler_ext_int_reg1 (void) { *NVIC_ICER = 0xFFFFFFFF; }
void handler_ext_int_reg2 (void) { *NVIC_ICER = 0xFFFFFFFF; }
void handler_ext_int_reg3 (void) { *NVIC_ICER = 0xFFFFFFFF; }
void handler_ext_int_reg4 (void) { *NVIC_ICER = 0xFFFFFFFF; }
void handler_ext_int_reg5 (void) { *NVIC_ICER = 0xFFFFFFFF; }


//********************************************************************
// MAIN function starts here             
//********************************************************************


int main(void) {

    // Disable PRE Watchdog
    *TIMERWD_GO  = 0;

    // Disable MBus Watchdog
    *REG_MBUS_WD = 0;

    // Wakeup Source
    wakeup_source = *SREG_WAKEUP_SOURCE;
    #ifdef DEVEL
        mbus_write_message32(0x50, wakeup_source);
    #endif

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    // Measurement Count
    mbus_write_message32(0x51, meas_count);

    // Temperature Measurement
    meas_temp();

    // Pressure Measurement
    meas_rdc();

    // Go to Sleep
    operation_sleep_snt_timer();

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}

