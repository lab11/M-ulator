//*******************************************************************************************
// XT1 (TMCv1r1) FIRMWARE
// Version 2.08 (encryption)
//------------------------
#define HARDWARE_ID 0x01005843  // XT1r1 Hardware ID
#define FIRMWARE_ID 0x0208      // [15:8] Integer part, [7:0]: Non-Integer part
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
#include "sample_data_2"    // Has sample_data[] and sample_size

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
#define FLAG_WD_ENABLED     3
#define FLAG_MAIN_CALLED    4   // Sleep has been bypassed before the current active session. Thus, main() has called from within operation_sleep(). This implies that xo_val_curr is inaccurate.
#define FLAG_INVLD_XO_PREV  5   // Sleep has been bypassed before the previous active session. This implies that xo_val_prev is inaccurate.
#define FLAG_BOOTUP_DONE    6
#define FLAG_AES_KEY_SET    7

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
#define CMD_HRESET      0x07
#define CMD_NEWKEY      0x08
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

// Update Types
#define EID_GLOBAL      0
#define EID_LOCAL       1

//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************

//-------------------------------------------------------------------------------------------
// System Configuration Variables in EEPROM
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// User Configuration Variables in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in update_user_configs() function.
//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
// Handshake in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: These variables are updated in nfc_check_cmd() function.
//-------------------------------------------------------------------------------------------

volatile uint32_t cmd;              // Command
volatile uint32_t cmd_param;        // Command Parameter

//-------------------------------------------------------------------------------------------
// System Status in EEPROM
//-------------------------------------------------------------------------------------------
// NOTE: Most of these variables are reset in reset_system_status() function.
//-------------------------------------------------------------------------------------------

volatile uint32_t temp_sample_count;                // Counter for sample count (# actual temperature measurements)
volatile uint32_t eeprom_sample_count;              // Counter for sample count (# temperature measurements stored into EEPROM)
volatile uint32_t system_state;                     // System State (See comment on EEPROM_ADDR_SYSTEM_STATE)


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



//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep Functions
static void operation_sleep (void);
static void operation_init (void);

//--- FSM
static void set_system(uint32_t target);
static void set_system_state(uint32_t msb, uint32_t lsb, uint32_t val);
static void snt_operation (void);

//--- E-Ink Display
static void eid_update_display(uint32_t seg);

//--- NFC
static uint32_t nfc_set_ack(uint32_t ack);
static uint32_t nfc_check_cmd(void);

//--- Data Compression
uint32_t tcomp_encode (uint32_t value);
uint32_t reverse_bits (uint32_t bit_stream, uint32_t num_bits);

//--- 128-bit qword Handling
void set_qword (uint32_t pattern);
void sub_qword (uint32_t msb, uint32_t lsb, uint32_t value);
void copy_qword (uint32_t* targ);

//--- AES & CRC
void aes_encrypt_128(uint32_t* pt);
void aes_encrypt_eeprom(uint32_t addr_begin, uint32_t addr_end);
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
// Description:
//           Sends the MBus Sleep message
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep (void) {

    // Reset GOC_DATA_IRQ
    //*GOC_DATA_IRQ = 0; // GOC IRQ is not used.

    // FIXME: Stop any ongoing EID operation (but how...?)

    // Power off NFC
    nfc_power_off();

    // Clear all pending IRQs; otherwise, PREv22E replaces the sleep msg with a selective wakeup msg
    *NVIC_ICER = 0xFFFFFFFF;

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

        // Update the flag
        set_flag(FLAG_ENUMERATED, 1);

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

        //-------------------------------------------------
        // EID Settings
        //-------------------------------------------------
	    //eid_init(/*ring*/0, /*te_div*/3, /*fd_div*/3, /*seg_div*/3);
	    eid_init();

        //-------------------------------------------------
        // System Reset
        //-------------------------------------------------
        // Set system
        set_system(/*target*/XT1_RESET);

        // Update the flag
        set_flag(FLAG_INITIALIZED, 1);
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
    // XT1_RESET 
    //------------------------------------------
    if (target==XT1_RESET) {

        // Critical Variables
        snt_temp_val        = 1000; // Assume 20C by default

        // Data Compression Variables
        comp_sample_id   = 0;
        comp_prev_sample = 0;
        comp_bit_pos     = 0;
        crc32            = 1;
        set_qword(/*pattern*/0xFFFFFFFF);  // qword = all 1s.

        //--------------------------------------
        // XT1_RESET
        //--------------------------------------
        if (target==XT1_RESET) {

            // Reset some variables
            cmd                      = CMD_NOP;

            // Display all segments
            eid_update_display(/*seg*/DISP_ALL);

            // Reset the System State
            system_state = 0;
            set_system_state(/*msb*/7, /*lsb*/0, /*val*/XT1_RESET);

            // Update the state
            xt1_state = XT1_RESET;
        }
    }

    //------------------------------------------
    // XT1_IDLE
    //------------------------------------------
    else if (target==XT1_IDLE) {

        // During Boot-Up
        if (!get_flag(FLAG_BOOTUP_DONE))
            eid_update_display(/*seg*/DISP_CHECK|DISP_PLUS|DISP_MINUS|DISP_LOWBATT);
        // If coming from XT1_RESET: Display Check only
        else if (xt1_state==XT1_RESET)
            eid_update_display(/*seg*/DISP_CHECK);

        // Reset the System State
        set_system_state(/*msb*/1, /*lsb*/0, /*val*/XT1_IDLE);

        // Update the state
        xt1_state = XT1_IDLE;
    }


    // If XT1_RESET, it also needs to go to XT1_IDLE
    if (xt1_state==XT1_RESET) set_system(/*target*/XT1_IDLE);
}

static void set_system_state(uint32_t msb, uint32_t lsb, uint32_t val) {
    system_state = set_bits(/*original_var*/system_state, /*msb_idx*/msb, /*lsb_idx*/lsb, /*value*/val);
    nfc_i2c_byte_write(/*e2*/0, /*addr*/EEPROM_ADDR_SYSTEM_STATE, /*data*/system_state, /*nb*/1);
}

static void snt_operation (void) {

    uint32_t sample_id;
    for (sample_id=0; sample_id<sample_size; sample_id++) {

        #ifdef DEVEL
            mbus_write_message32(0x88, sample_id);
        #endif

        // Increment the sample count
        temp_sample_count++;

        /////////////////////////////////////////////////////////////////
        // Write into EEPROM/Display
        //---------------------------------------------------------------

        // Read the sample
        uint32_t buf_temp_val = sample_data[sample_id];

        //--------------------------------------------------------------------------------

        #ifdef DEVEL
            mbus_write_message32(0x95, buf_temp_val);
            mbus_write_message32(0x96, comp_sample_id);
        #endif

        // STORE THE CONVERTED & COMPRESSED DATA
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


        // MEMORY IS FULL
        }
        else { // Roll-over disabled
            comp_bit_pos = 65536;
            // Update Memory Full Flag
            set_system_state(/*msb*/3, /*lsb*/3, /*val*/0x1);

        }

        //--------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////
    }

}


//-------------------------------------------------------------------
// XO & SNT Timers
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// E-Ink Display
//-------------------------------------------------------------------

static void eid_update_display(uint32_t seg) {

    // Turn off NFC, if on, to save power
    nfc_power_off();

    // Get the current display
    uint32_t prev_seg = eid_get_current_display();

    // E-ink Update
    eid_update_global(seg);

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
    uint32_t cmd_raw;
    // ACK or ERR
    if (ack) cmd_raw = (0x1<<6)|cmd;
    else     cmd_raw = (0x1<<5)|cmd;

    uint32_t num_try=10;
    while (num_try!=0) {
        nfc_i2c_start();
        if (nfc_i2c_byte_imm(0xA6)) {
            nfc_i2c_byte(EEPROM_ADDR_CMD >> 8);
            nfc_i2c_byte(EEPROM_ADDR_CMD);
            nfc_i2c_byte(cmd_raw);
            nfc_i2c_byte(cmd_param);
            nfc_i2c_stop();
            nfc_i2c_polling();
            return 1;
        }
        nfc_i2c_stop();
        num_try--;
    }
    return 0;
}

uint32_t nfc_check_cmd(void) {

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
            cmd_raw    = nfc_i2c_rd(1); // CMD
            cmd_param  = nfc_i2c_rd(0); // CMD_PARAM
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

    //--- Invalid Handshaking (REQ=0 or ACK=1 or ERR=1)
    if ((cmd_raw&0xE0)!=0x80) return 0xDEAD;

    //---------------------------------------------------------------
    // Normal Handshaking (REQ=1 AND ACK=0 AND ERR=0)
    //---------------------------------------------------------------

    #ifdef DEVEL
        mbus_write_message32(0xB3, (cmd_param<<16)|cmd);
    #endif

    //-----------------------------------------------------------
    // CMD: NOP, HARD_RESET, DISPLAY
    //-----------------------------------------------------------
    if (cmd == CMD_HRESET) {
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
    #ifdef DEVEL
        mbus_write_message32(0x85, 0);
    #endif

    uint32_t i;
    for (i=0; i<4; i++) *(qword+i) = pattern;

    #ifdef DEVEL
        mbus_write_message32(0x80, qword[0]);
        mbus_write_message32(0x81, qword[1]);
        mbus_write_message32(0x82, qword[2]);
        mbus_write_message32(0x83, qword[3]);
    #endif

}

void sub_qword (uint32_t msb, uint32_t lsb, uint32_t value) {
// Valid Range: msb= 0 - 127
//              lsb= 0 - 127
//
    #ifdef DEVEL
        mbus_write_message32(0x85, 1);
        mbus_write_message32(0x84, (msb<<16) | lsb);
        mbus_write_message32(0x84, value);
    #endif

    // CASE I) msb and lsb belong to the same word
    if ((msb>>5)==(lsb>>5)) {
        *(qword+(lsb>>5)) = set_bits(/*original_var*/*(qword+(lsb>>5)), /*msb_idx*/msb&0x1F, /*lsb_idx*/lsb&0x1F, /*value*/value);
    }
    // CASE II) msb and lsb belong to the different word
    else {
        *(qword+(lsb>>5)) = set_bits(/*original_var*/*(qword+(lsb>>5)), /*msb_idx*/31, /*lsb_idx*/lsb&0x1F, /*value*/value);
        *(qword+(msb>>5)) = set_bits(/*original_var*/*(qword+(msb>>5)), /*msb_idx*/msb&0x1F, /*lsb_idx*/0, /*value*/value>>(32-(lsb&0x1F)));
    }

    #ifdef DEVEL
        mbus_write_message32(0x80, qword[0]);
        mbus_write_message32(0x81, qword[1]);
        mbus_write_message32(0x82, qword[2]);
        mbus_write_message32(0x83, qword[3]);
    #endif

}

void copy_qword (uint32_t* targ) {
    #ifdef DEVEL
        mbus_write_message32(0x85, 2);
    #endif
    uint32_t i;
    for (i=0; i<4; i++) *(targ+i) = *(qword+i);
}


//-------------------------------------------------------------------
// AES & CRC
//-------------------------------------------------------------------

void aes_encrypt_128 (uint32_t* pt) {

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

    #ifdef DEVEL
        mbus_write_message32(0x85, 3);
    #endif

    // Set Plain Text
    for (i=0; i<4; i++) {
        *(AES_TEXT_0+i) = *(pt+i);
    #ifdef DEVEL
        mbus_write_message32(0x80+i, *(pt+i));
    #endif
    }

    // Perform AES
    *AES_GO = 1;
    WFI();

    #ifdef DEVEL
        mbus_write_message32(0x85, 4);
    #endif

    // Update
    for (i=0; i<4; i++) {
        *(pt+i) = *(AES_TEXT_0+i);
    #ifdef DEVEL
        mbus_write_message32(0x80+i, *(pt+i));
    #endif
    }

}

void aes_encrypt_eeprom(uint32_t addr_begin, uint32_t addr_end) {

    uint32_t addr, i;
    uint32_t text[4];

    // Initialize the address
    addr = addr_begin;

    // For each 128-bits (16 bytes)
    while (addr < addr_end) {
        // Read the raw data
        for (i=0; i<4; i++) {
            text[i] = nfc_i2c_byte_read(/*e2*/0, /*addr*/addr+(i<<2), /*nb*/4);
        }

        // Encrypt
        aes_encrypt_128(/*pt*/text);

        // Write the encrypted data back into EEPROM
        for (i=0; i<4; i++) {
            nfc_i2c_byte_write(/*e2*/0, /*addr*/addr+(i<<2), /*data*/*(text+i), /*nb*/4);
        }

        // Increment addr
        addr += 16; // 16 bytes = 128 bits
    }
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
    aes_encrypt_128(/*pt*/text);

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

    // Reset variables
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

    mbus_write_message32(0x70, sample_data[0]);
    #ifdef DEVEL
        mbus_write_message32(0x70, wakeup_source);
        mbus_write_message32(0x71, xt1_state);
    #endif

    // Enable IRQs
    *NVIC_ISER = (0x1 << IRQ_TIMER32);

    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

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
            operation_sleep();
        }

        // Commands requiring further operation
        else if (target==0xF00D) {

            // Command: HARD_RESET
            if (cmd==CMD_HRESET) {
                eid_update_display(/*seg*/DISP_PLUS|DISP_MINUS);
                config_timerwd(/*cnt*/10);
                while(1);
            }
            // Command: NOP
            else {}

            // Go to Sleep
            operation_sleep();
        }

    }
    //-----------------------------------------
    // Test
    //-----------------------------------------
    else {
        temp_sample_count = 0;
        eeprom_sample_count = 0;

        //// NIST Test Vector tv[7];
        //aes_key[3] = 0x10a58869;
        //aes_key[2] = 0xd74be5a3;
        //aes_key[1] = 0x74cf867c;
        //aes_key[0] = 0xfb473859;

        //// NIST Test Vector tv[27];
        //aes_key[3] = 0xfebd9a24;
        //aes_key[2] = 0xd8b65c1c;
        //aes_key[1] = 0x787d50a4;
        //aes_key[0] = 0xed3619a9;

        //// NIST Test Vector tv[156];
        //aes_key[3] = 0x80000000;
        //aes_key[2] = 0x00000000;
        //aes_key[1] = 0x00000000;
        //aes_key[0] = 0x00000000;

        // NIST Test Vector tv[283];
        aes_key[3] = 0xffffffff;
        aes_key[2] = 0xffffffff;
        aes_key[1] = 0xffffffff;
        aes_key[0] = 0xffffffff;


        eid_update_display(DISP_ALL);

        // Bookkeeping/Temp Measurement
        snt_operation();

        eid_update_display(DISP_CHECK);

    }

    operation_sleep();

    //--------------------------------------------------------------------------
    // Dummy Buffer
    //--------------------------------------------------------------------------
    while(1) asm("nop");
    return 1;
}
