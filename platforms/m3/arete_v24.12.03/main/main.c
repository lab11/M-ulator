//*******************************************************************************************
// ARETE FIRMWARE
// Version 24.12.03 (devel)
//-------------------------------------------------------------------------------------------
// SUB-LAYER CONNECTION:
//-------------------------------------------------------------------------------------------
//
//      PREv23E -> ADOv1A -> MRMv1L -> PMUv13r1
//
//-------------------------------------------------------------------------------------------
// DESCRIPTION
//-------------------------------------------------------------------------------------------
//
//  - This firmware is based on GOC/EP Triggers and their IRQ handlings.
//
//      - In the GOC/EP HEADER, CONTROL[7:6] must be set to 2'b11, which means 'GOCEP_IRQ'.
//      - Write up to 3 words starting from Memory Address 0x00000008. 
//          These are stored in the goc_head, goc_data_0, goc_data_1 variables as shown below.
//
//              goc_head   = MEM[0x00000008] 
//              goc_data_0 = MEM[0x0000000C] 
//              goc_data_1 = MEM[0x00000010] 
//
//      =====================================================================================
//          goc_head        goc_data_0  goc_data_1  Description
//      [31:16]  [15:0]     [31:0]      [31:0]
//      =====================================================================================
//
//      -------------------------------------------------------------------------------------
//       START RECORDING
//
//          These triggers start audio recording using the given 'Record Duration', 
//          'Initial Delay', and 'MRAM Offset'.
//      -------------------------------------------------------------------------------------
//
//      0x0001      a       -           -           Record Duration: 'a' seconds (Max: 1536 seconds)
//                                                  Initial Delay: 0
//                                                  MRAM Offset: 0
//
//      0x0002      a       b           -           Record Duration: 'a' seconds (Max: 1536 seconds)
//                                                  Initial Delay: 'b' seconds (Max: 4194303 seconds)
//                                                  MRAM Offset: 0 
//
//      0x0003      a       b           c           Record Duration: 'a' seconds (Max: 1536 seconds)
//                                                  Initial Delay: 'b' seconds (Max: 4194303 seconds)
//                                                  MRAM Offset: 'c' seconds (Max: 1536 seconds)
//
//      -------------------------------------------------------------------------------------
//       READ OUT THE RECORDING
//          
//          These triggers read out the Recording from the MRAM and send MBus message(s).
//          If the 'Record Duration' is longer than 8 seconds, it sends out multiple MBus messages. 
//          Each MBus message contains up to 8-second-long recording.
//      -------------------------------------------------------------------------------------
//
//      0x0008      a       b           -           Record Duration: 'a' seconds (Max: 1536 seconds)
//                                                  MBus Destination Address: b (=goc_data_0[7:0])
//                                                  MRAM Offset: 0 
//
//      0x0009      a       b           c           Record Duration: 'a' seconds (Max: 1536 seconds)
//                                                  MBus Destination Address: b (=goc_data_0[7:0])
//                                                  MRAM Offset: 'c' seconds (Max: 1536 seconds)
//
//      -------------------------------------------------------------------------------------
//       PMU CONTROL
//      -------------------------------------------------------------------------------------
//
//      0x0100      a       b           -           Write into a PMU register. Reg#(a) = b.
//      0x0101      a       -           -           Read a PMU register Reg#(a), and send the MBus message to PRE.
//      0x0102      a       -           -           Set SAR Ratio. SAR Ratio = a.
//      0x0103      a       b           -           Set Active Floor setting.
//                                                      R     = b[31:24]
//                                                      UPC L = b[23:16]
//                                                      SAR L = b[15:8]
//                                                      DNC L = b[7:0]
//                                                      BASE  = a
//      0x0104      a       b           -           Set Sleep Floor setting.
//                                                      R     = b[31:24]
//                                                      UPC L = b[23:16]
//                                                      SAR L = b[15:8]
//                                                      DNC L = b[7:0]
//                                                      BASE  = a
//
//      -------------------------------------------------------------------------------------
//       ADO CONTROL
//      -------------------------------------------------------------------------------------
//
//      0x0201      a       -           -           Execute ado_ldo_set_mode(a)
//      0x0202      a       -           -           Execute ado_safr_set_mode(a)
//      0x0203      a       -           -           Execute ado_amp_set_mode(a)
//      0x0204      a       -           -           Execute ado_adc_set_mode(a)
//      0x0205      a       -           -           Execute ado_cp_set_mode(a)
//      0x0206      a       -           -           Set REC_PGA_GCON = a
//      0x0207      a       -           -           Set DIV1 = a
//      0x0208      a       -           -           Run the ADC for 'a' seconds (Max: 1536 seconds), then go to sleep.
//
//      -------------------------------------------------------------------------------------
//       MRM CONTROL
//      -------------------------------------------------------------------------------------
//
//      0x0300      a       b           -           Write into a MRM register. Reg#(a) = b.
//      0x0301      a       -           -           Read a MRM register Reg#(a), and send the MBus message to PRE's Reg#0x07.
//      0x0302      a       b           -           Write into a MRM register, Reg#(a) = b, then wait for a reply.
//
//      -------------------------------------------------------------------------------------
//       PRE CONTROL
//      -------------------------------------------------------------------------------------
//
//      0x0400      a       b           -           Write into a PRE register. Reg#(a) = b.
//
//      0x0401      0x0000  -           -           Stop XO
//      0x0401      0x0001  -           -           Start XO (free running)
//      0x0401      0x0002  b           -           Set XO timer threshold to 'b' and go to sleep
//      0x0401      0x0003  -           -           Enable XO clock pad output
//      0x0401      0x0004  -           -           Disable XO clock pad output
//
//      0x0402      0x0000  b           -           Stay in active for delay(b) then go to sleep.
//
//
//-------------------------------------------------------------------------------------------
// Use of Cortex-M0 Vector Table
//-------------------------------------------------------------------------------------------
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
//  0xE0    goc_head                            goc_head   (MEM[0x00000008])
//  0xE1    goc_data_0                          goc_data_0 (MEM[0x0000000C])
//  0xE2    goc_data_1                          goc_data_1 (MEM[0x00000010])
//  0xEF    wakeup_source                       Wakeup Source (See above WAKEUP_SOURCE definition)
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

#include "ARETE.h"

//*******************************************************************************************
// DEVEL Mode
//*******************************************************************************************
// Enable 'DEVEL' for the following features:
//      - Send debug messages
//      - Use default values rather than grabbing the values from EEPROM
#define DEVEL
//#define ENABLE_XO_PAD     // Enable XO clock output for debugging

#define XO_ALWAYS_RUNNING   // Enable this to make the XO always run. 
                            // Disable this to make the XO run only during the Init Delay.

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1
#define FLAG_PEND_INIT_DELAY    2

//*******************************************************************************************
// MRM CONFIGURATIONS
//*******************************************************************************************
// MRMv1L's Clock Generator Tuning. The fast clock must be ~50MHz. See the comments section of mrm_init() in MRMv1L.h.
#define MRM_CLK_GEN_S           51

//*******************************************************************************************
// XO CONFIGURATIONS
//*******************************************************************************************

// XO Frequency
#define XO_FREQ_SEL 0                       // XO Frequency (in kHz) = 2 ^ XO_FREQ_SEL
#define XO_FREQ     1<<(XO_FREQ_SEL+10)     // XO Frequency (in Hz)

// XO Initialization Wait Duration
#define XO_WAIT_A   2*DLY_1S    // Must be ~1 second delay.
#define XO_WAIT_B   2*DLY_1S    // Must be ~1 second delay.

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

//*******************************************************************************************
// GLOBAL VARIABLES
//*******************************************************************************************

volatile uint32_t wakeup_source;
volatile uint32_t goc_head;
volatile uint32_t goc_data_0;
volatile uint32_t goc_data_1;
volatile uint32_t prev_goc_head;
volatile uint32_t prev_goc_data_0;
volatile uint32_t prev_goc_data_1;



//*******************************************************************************************
// FUNCTIONS DECLARATIONS
//*******************************************************************************************

//--- Main
int main(void);

//--- Initialization/Sleep/IRQ Handling
static void operation_init(void);
static void operation_sleep(void);

//-------------------------------------------------------------------
// Function: operation_init
// Args    : None
// Description:
//           Initializes the system
// Return  : None
//-------------------------------------------------------------------
static void operation_init(void){

    //-------------------------------------------------
    // PRE Tuning
    //-------------------------------------------------

    //--- Set CPU & MBus Clock Speeds      Default
    *REG_CLKGEN_TUNE =    (0x0 << 21)   // 1'h0     CLK_GEN_HIGH_FREQ
                        | (0x3 << 18)   // 3'h3     CLK_GEN_DIV_CORE    #Default CPU Freq = 96kHz (pre-pex simulation, TT, 25C)
                        | (0x2 << 15)   // 3'h2     CLK_GEN_DIV_MBC     #Default MBus Freq = 193kHz (pre-pex simulation, TT, 25C)
                        | (0x1 << 13)   // 2'h1     CLK_GEN_RING
                        | (0x0 << 12)   // 1'h0     RESERVED
                        | (0x0 << 10)   // 2'h1     GOC_CLK_GEN_SEL_DIV
                        | (0x5 <<  7)   // 3'h7     GOC_CLK_GEN_SEL_FREQ
                        | (0x0 <<  6)   // 1'h0     GOC_ONECLK_MODE
                        | (0x0 <<  4)   // 2'h0     GOC_SEL_DLY
                        | (0xF <<  0);  // 4'h8     GOC_SEL

    //--- Pending Wakeup Handling          Default
    *REG_SYS_CONF =       (0x0 << 9)    // 1'h1     NO_SLEEP_WITH_PEND_IRQ  #If 1, it replaces the sleep message with a selective wakeup message if there is a pending M0 IRQ. If 0, it goes to sleep even when there is a pending M0 IRQ.
                        | (0x0 << 8)    // 1'h0     ENABLE_SOFT_RESET   #If 1, Soft Reset will occur when there is an MBus Memory Bulk Write message received and the MEM Start Address is 0x2000
                        | (0x1 << 7)    // 1'h1     PUF_CHIP_ID_SLEEP
                        | (0x1 << 6)    // 1'h1     PUF_CHIP_ID_ISOLATE
                        | (0x8 << 0);   // 5'h1E    WAKEUP_ON_PEND_REQ  #[4]: GIT (PRE_E Only), [3]: GPIO (PRE only), [2]: XO TIMER (PRE only), [1]: WUP TIMER, [0]: GOC/EP

    //-------------------------------------------------
    // Enumeration
    //-------------------------------------------------
    set_halt_until_mbus_trx();
    mbus_enumerate(ADO_ADDR);
    mbus_enumerate(MRM_ADDR);
    mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();

    // Update the flag
    set_flag(FLAG_ENUMERATED, 1);

    //-------------------------------------------------
    // PMU Settings
    //-------------------------------------------------
    pmu_init(/*irq_reg_idx*/PMU_TARGET_REG_IDX);

    //-------------------------------------------------
    // MRM Settings
    //-------------------------------------------------
    mrm_init(/*irq_reg_idx*/MRM_TARGET_REG_IDX, /*clk_gen_s*/MRM_CLK_GEN_S);

    //-------------------------------------------------
    // ADO Settings
    //-------------------------------------------------
    ado_init();

    //-------------------------------------------------
    // XO Settings
    //-------------------------------------------------
    //--- XO Frequency
    *REG_XO_CONF2 =             // Default  // Description
        //-----------------------------------------------------------------------------------------------------------
        ( (0x1         << 20)   // (3'h1) XO_SEL_DLY            #Adjusts pulse delay
        | (0x1         << 18)   // (2'h1) XO_SEL_CLK_SCN        #Selects division ratio for SCN CLK
        | (XO_FREQ_SEL << 15)   // (3'h1) XO_SEL_CLK_OUT_DIV    #Selects division ratio for the XO CLK output. XO Freq = 2^XO_SEL_CLK_OUT_DIV (kHz)
        | (0x1         << 14)   // (1'h1) XO_SEL_LDO            #Selects LDO output as an input to SCN
        | (0x0         << 13)   // (1'h0) XO_SEL_0P6            #Selects V0P6 as an input to SCN
        | (0x0         << 10)   // (3'h0) XO_I_AMP              #Adjusts VREF body bias buffer current
        | (0x0         <<  3)   // (7'h0) XO_VREF_TUNEB         #Adjust VREF level and TC
        | (0x0         <<  0)   // (3'h0) XO_SEL_VREF           #Selects VREF output from its diode stack
        );

    xo_stop();  // Default value of XO_START_UP is wrong in RegFile, so need to override it.

    #ifdef XO_ALWAYS_RUNNING
        //--- Start XO clock
        xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

        //--- Configure and Start the XO Counter
        set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0, /*auto_reset*/0);
        start_xo_cnt();

        #ifdef ENABLE_XO_PAD
            start_xo_cout();
        #endif

    #endif

    //-------------------------------------------------
    // End of Initialization
    //-------------------------------------------------
    // Disable the PRC wakeup timer
    *REG_WUPT_CONFIG = *REG_WUPT_CONFIG & 0xFF3FFFFF; // WUP_ENABLE=0, WUP_WREQ_EN=0

    // Update the flag
    set_flag(FLAG_INITIALIZED, 1);

    //-------------------------------------------------
    // Go to Sleep
    //-------------------------------------------------
    operation_sleep();
}


//-------------------------------------------------------------------
// Function: operation_sleep
// Args    : None
// Description:
//           Go to sleep indefinitely.
// Return  : None
//-------------------------------------------------------------------
static void operation_sleep(void){
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0;
    
    // Go to Sleep
    mbus_sleep_all();
    while(1);
}


//*******************************************************************
// INTERRUPT HANDLERS 
//*******************************************************************

void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_xot      (void) __attribute__ ((interrupt ("IRQ")));

// GOC/EP IRQ
void handler_ext_int_gocep (void) {
    goc_head   = *(GOC_DATA_IRQ+0);
    goc_data_0 = *(GOC_DATA_IRQ+1);
    goc_data_1 = *(GOC_DATA_IRQ+2);
    *GOC_DATA_IRQ = 0xFFFFFFFF;

    #ifdef DEVEL
        mbus_write_message32(0xE0, goc_head);
        mbus_write_message32(0xE1, goc_data_0);
        mbus_write_message32(0xE2, goc_data_1);
    #endif    
}

// REG0 IRQ (PMU)
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (1 << IRQ_REG0);
}

// REG1 IRQ (MRM)
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (1 << IRQ_REG1);
}

// XOT IRQ
void handler_ext_int_xot(void) { // XOT
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    
    // Disable PRE Watchdog & MBus Watchdog
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;

    // Get the info on who woke up the system
    wakeup_source = *SREG_WAKEUP_SOURCE;
    #ifdef DEVEL
        mbus_write_message32(0xEF, wakeup_source);
    #endif    
    
    // Enable required IRQs
    *NVIC_ISER = (1 << IRQ_GOCEP)
               | (1 << IRQ_REG0)
               | (1 << IRQ_REG1);
    
    // If this is the very first wakeup, initialize the system
    if (!get_flag(FLAG_INITIALIZED)) operation_init();

    else if (WAKEUP_BY_GOCEP || get_flag(FLAG_PEND_INIT_DELAY)) {

        if (get_flag(FLAG_PEND_INIT_DELAY)) {
            goc_head   = prev_goc_head;
            goc_data_0 = prev_goc_data_0;
            goc_data_1 = prev_goc_data_1;
        } 

        uint32_t goc_head_3  = (goc_head>>24) & 0xFF;
        uint32_t goc_head_2  = (goc_head>>16) & 0xFF;
      //uint32_t goc_head_1  = (goc_head>> 8) & 0xFF;
        uint32_t goc_head_0  = (goc_head>> 0) & 0xFF;
      //uint32_t goc_head_hi = (goc_head>>16) & 0xFFFF;
        uint32_t goc_head_lo = (goc_head>> 0) & 0xFFFF;

        uint32_t init_delay   = 0;
        uint32_t offset_sec   = 0;
        uint32_t duration_sec = 0;

        switch (goc_head_3) {

            //------------------------------------------------------------------------
            // goc_head = 0x00xxxxxx: Main Tasks
            //------------------------------------------------------------------------
            case 0x00:

                switch (goc_head_2) {
                    //------------------------------------------------------------------------
                    // Start Recording
                    //------------------------------------------------------------------------
                    // goc_head = 0x0001nnnn: Immediately start recording
                    //                          Record Duration: 'nnnn' seconds (max. 1536 seconds)
                    //                          Initial Delay: 0
                    //                          MRAM Offset: 0
                    // goc_head = 0x0002nnnn: Start Recording after 'goc_data_0' delay
                    //                          Record Duration: 'nnnn' seconds (max. 1536 seconds)
                    //                          Initial Delay: goc_data_0 (in seconds; max 4194303 seconds)
                    //                          MRAM Offset: 0
                    // goc_head = 0x0003nnnn: Start Recording after 'goc_data_0' delay, with 'goc_data_1' offset in MRAM.
                    //                          Record Duration: 'nnnn' seconds (max. 1536 seconds)
                    //                          Initial Delay: goc_data_0 (in seconds; max 4194303 seconds)
                    //                          MRAM Offset: goc_data_1 (in seconds; max 1536 seconds)
                    //------------------------------------------------------------------------
                    case 0x01:
                    case 0x02:
                    case 0x03:

                        // Init Delay Implementation (only for goc_head_2 = 0x02 or 0x03)
                        if ((goc_head_2 != 0x01) && !get_flag(FLAG_PEND_INIT_DELAY)) {
                            prev_goc_head   = goc_head;
                            prev_goc_data_0 = goc_data_0;
                            prev_goc_data_1 = goc_data_1;

                            set_flag(FLAG_PEND_INIT_DELAY, 1);

                            init_delay = get_min(goc_data_0, 0xFFFFFFFF>>(XO_FREQ_SEL+10)) << (XO_FREQ_SEL+10);

                            #ifdef XO_ALWAYS_RUNNING
                                //--- Update the XO threshold
                                set_xo_timer (/*mode*/0, /*timestamp*/init_delay, /*wreq_en*/1, /*irq_en*/0, /*auto_reset*/1);
                            #else
                                //--- Start XO clock
                                xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

                                //--- Configure and Start the XO Counter
                                set_xo_timer(/*mode*/0, /*threshold*/init_delay, /*wreq_en*/1, /*irq_en*/0, /*auto_reset*/1);
                                start_xo_cnt();
                            #endif

                            operation_sleep();

                        }

                        // After the Init Delay 
                        else {

                            // Reset the flag if coming back from the Init Delay
                            if (get_flag(FLAG_PEND_INIT_DELAY)) {

                                #ifdef XO_ALWAYS_RUNNING
                                #else
                                    // Turn off the XO
                                    stop_xo_cnt();
                                    disable_xo_timer();
                                    xo_stop();
                                #endif

                                set_flag(FLAG_PEND_INIT_DELAY, 0);
                            }

                            // Turn on ADO, except ADC
                            ado_ldo_set_mode (/*mode*/1);
                            ado_safr_set_mode(/*mode*/1);
                            ado_amp_set_mode (/*mode*/1);

                            // Start Ping-Pong
                            duration_sec = goc_head_lo;
                            if (goc_head_2==0x03) offset_sec = get_min(goc_data_1, MRM_RECORD_DURATION);
                            mrm_start_rec (/*offset_sec*/offset_sec, /*duration_sec*/duration_sec, /*wait_for_irq*/0);

                            // Enable ADC 
                            ado_adc_set_mode(/*mode*/1);

                            // Wait for IRQ from MRAM
                            WFI();

                            // Turn off ADO
                            ado_adc_set_mode (/*mode*/0);
                            ado_amp_set_mode (/*mode*/0);
                            ado_safr_set_mode(/*mode*/0);
                            ado_ldo_set_mode (/*mode*/0);
                        }

                        break;

                    //------------------------------------------------------------------------
                    // Read Out the Recording
                    //------------------------------------------------------------------------
                    // goc_head = 0x0008nnnn: Read out the Recording.
                    //                          Record Duration: 'nnnn' seconds (max. 1536 seconds)
                    //                          MBus Destination Prefix: goc_data_0
                    //                          MRAM Offset: 0
                    // goc_head = 0x0009nnnn: Read out the Recording, with 'goc_data_1' offset in MRAM.
                    //                          Record Duration: 'nnnn' seconds (max. 1536 seconds)
                    //                          MBus Destination Prefix: goc_data_0
                    //                          MRAM Offset: goc_data_1 (in seconds; max 1536 seconds)
                    //------------------------------------------------------------------------
                    case 0x08:
                    case 0x09:

                        duration_sec = goc_head_lo;
                        if (goc_head_2==0x09) offset_sec = get_min(goc_data_1, MRM_RECORD_DURATION);
                        mrm_read (/*offset_sec*/offset_sec, /*duration_sec*/duration_sec, /*dst_prefix*/goc_data_0&0xFF);

                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x00xxxxxx: Default
                    //------------------------------------------------------------------------
                    default:
                        break;
                }

                break;
            
            //------------------------------------------------------------------------
            // goc_head = 0x01xxxxxx: PMU Control
            //------------------------------------------------------------------------
            case 0x01:

                switch (goc_head_2) {

                    //------------------------------------------------------------------------
                    // goc_head = 0x0100xxnn: PMU Register Write
                    //------------------------------------------------------------------------
                    case 0x00:
                        pmu_reg_write(/*reg_addr*/goc_head_0, /*reg_data*/goc_data_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0101xxnn: PMU Register Read
                    //------------------------------------------------------------------------
                    case 0x01:
                        pmu_reg_read(/*reg_addr*/goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0102xxnn: Set SAR Ratio
                    //------------------------------------------------------------------------
                    case 0x02:
                        pmu_set_sar_ratio (/*ratio*/goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0103xxnn: Set Active Floor setting
                    //------------------------------------------------------------------------
                    case 0x03:
                        pmu_active_floor(/*r*/(goc_data_0>>24)&0xFF, /*ul*/(goc_data_0>>16)&0xFF, /*sl*/(goc_data_0>>8)&0xFF, /*dl*/(goc_data_0>>0)&0xFF, /*b*/goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0104xxnn: Set Sleep Floor setting
                    //------------------------------------------------------------------------
                    case 0x04:
                        pmu_sleep_floor(/*r*/(goc_data_0>>24)&0xFF, /*ul*/(goc_data_0>>16)&0xFF, /*sl*/(goc_data_0>>8)&0xFF, /*dl*/(goc_data_0>>0)&0xFF, /*b*/goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x01xxxxxx: Default
                    //------------------------------------------------------------------------
                    default:
                        break;
                }

                break;

            //------------------------------------------------------------------------
            // goc_head = 0x02xxxxxx: ADO Control
            //------------------------------------------------------------------------
            case 0x02:

                switch (goc_head_2) {
                    //------------------------------------------------------------------------
                    // goc_head = 0x0201xxnn: ADO LDO Control
                    //      - Executes ado_ldo_set_mode(nn)
                    //------------------------------------------------------------------------
                    case 0x01:
                        ado_ldo_set_mode(goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0202xxnn: ADO SAFR Control
                    //      - Executes ado_safr_set_mode(nn)
                    //------------------------------------------------------------------------
                    case 0x02:
                        ado_safr_set_mode(goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0203xxnn: ADO AMP Control
                    //      - Executes ado_amp_set_mode(nn)
                    //------------------------------------------------------------------------
                    case 0x03:
                        ado_amp_set_mode(goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0204xxnn: ADO ADC Control
                    //      - Executes ado_adc_set_mode(nn)
                    //------------------------------------------------------------------------
                    case 0x04:
                        ado_adc_set_mode(goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0205xxnn: ADO CP Control
                    //      - Executes ado_cp_set_mode(nn)
                    //------------------------------------------------------------------------
                    case 0x05:
                        ado_cp_set_mode(goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0206xxnn: ADO Change REC_PGA_GCON
                    //      - Executes ado_set_rec_pga_gcon(nn)
                    //------------------------------------------------------------------------
                    case 0x06:
                        ado_set_rec_pga_gcon(goc_head_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0207xnnn: ADO Change DIV1
                    //      - Executes ado_set_div1(nnn)
                    //------------------------------------------------------------------------
                    case 0x07:
                        ado_set_div1(goc_head&0x1FF);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0208nnnn: Run ADO ADC for 'nnnn' seconds
                    //------------------------------------------------------------------------
                    case 0x08:
                        *NVIC_ISER = (1 << IRQ_XOT);
                        ado_adc_set_mode(/*mode*/1);

                        #ifdef XO_ALWAYS_RUNNING
                            reset_xo_cnt();
                        #else
                            xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);
                        #endif

                        set_xo_timer(/*mode*/0, /*threshold*/goc_head_lo<<(XO_FREQ_SEL+10), /*wreq_en*/0, /*irq_en*/1, /*auto_reset*/0);
                        start_xo_cnt();

                        WFI();

                        ado_adc_set_mode(/*mode*/0);

                        #ifdef XO_ALWAYS_RUNNING
                        #else
                            // Turn off the XO
                            stop_xo_cnt();
                            disable_xo_timer();
                            xo_stop();
                        #endif

                        *NVIC_ICER = (1 << IRQ_XOT);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x02xxxxxx: Default
                    //------------------------------------------------------------------------
                    default:
                        break;
                }

                break;

            //------------------------------------------------------------------------
            // goc_head = 0x03xxxxxx: MRM Control
            //------------------------------------------------------------------------
            case 0x03:

                switch (goc_head_2) {
                    
                    //------------------------------------------------------------------------
                    // goc_head = 0x0300xxnn: MRM Register Write
                    //------------------------------------------------------------------------
                    case 0x00:
                        mbus_remote_register_write(MRM_ADDR, goc_head_0, goc_data_0);
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0301xxnn: MRM Register Read
                    //------------------------------------------------------------------------
                    case 0x01:
                        set_halt_until_mbus_trx();
                        mbus_remote_register_read(MRM_ADDR, goc_head_0, 0x07);
                        set_halt_until_mbus_tx();
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0302xxnn: MRM Register Write then wait for the reply.
                    //------------------------------------------------------------------------
                    case 0x02:
                        set_halt_until_mbus_trx();
                        mbus_remote_register_write(MRM_ADDR, goc_head_0, goc_data_0);
                        set_halt_until_mbus_tx();
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x03xxxxxx: Default
                    //------------------------------------------------------------------------
                    default:
                        break;
                }

                break;

            //------------------------------------------------------------------------
            // goc_head = 0x04xxxxxx: PRE Control
            //------------------------------------------------------------------------
            case 0x04:

                switch (goc_head_2) {
                    //------------------------------------------------------------------------
                    // goc_head = 0x0400xxxx: PRE Register Write
                    //------------------------------------------------------------------------
                    case 0x00:
                        *((volatile uint32_t *) (0xA0000000 | (goc_head_0<<2))) = goc_data_0;
                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0401xxxx: XO Control
                    //------------------------------------------------------------------------
                    case 0x01:
                        
                        switch (goc_head_lo) {

                            //------------------------------------------------------------------------
                            // goc_head = 0x04010000: Stop/Disable XO
                            //------------------------------------------------------------------------
                            case 0x0000:
                                stop_xo_cnt();
                                disable_xo_timer();
                                xo_stop();
                                break;

                            //------------------------------------------------------------------------
                            // goc_head = 0x04010001: Start XO
                            //------------------------------------------------------------------------
                            case 0x0001:
                                //--- Start XO clock
                                xo_start(/*delay_a*/XO_WAIT_A, /*delay_b*/XO_WAIT_B);

                                //--- Configure and Start the XO Counter
                                set_xo_timer(/*mode*/0, /*threshold*/0, /*wreq_en*/0, /*irq_en*/0, /*auto_reset*/0);
                                start_xo_cnt();

                                break;

                            //------------------------------------------------------------------------
                            // goc_head = 0x04010002: Set XO Counter Threshold and go to sleep
                            //------------------------------------------------------------------------
                            case 0x0002:
                                set_xo_timer(/*mode*/0, /*threshold*/goc_data_0, /*wreq_en*/1, /*irq_en*/0, /*auto_reset*/1);
                                break;

                            //------------------------------------------------------------------------
                            // goc_head = 0x04010003: Enable XO clock pad output
                            //------------------------------------------------------------------------
                            case 0x0003:
                                start_xo_cout();
                                break;

                            //------------------------------------------------------------------------
                            // goc_head = 0x04010004: Disable XO clock pad output
                            //------------------------------------------------------------------------
                            case 0x0004:
                                stop_xo_cout();
                                break;

                            //------------------------------------------------------------------------
                            // goc_head = 0x0401xxxx: Default
                            //------------------------------------------------------------------------
                            default:
                                break;
                        }

                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x0402xxxx: XO Control
                    //------------------------------------------------------------------------
                    case 0x02:
                        
                        switch (goc_head_lo) {

                            //------------------------------------------------------------------------
                            // goc_head = 0x04020000: Stay in Active
                            //------------------------------------------------------------------------
                            case 0x0000:
                                delay(goc_data_0);
                                break;

                            //------------------------------------------------------------------------
                            // goc_head = 0x0402xxxx: Default
                            //------------------------------------------------------------------------
                            default:
                                break;
                        }

                        break;

                    //------------------------------------------------------------------------
                    // goc_head = 0x04xxxxxx: Default
                    //------------------------------------------------------------------------
                    default:
                        break;
                }

                break;

            //------------------------------------------------------------------------
            // goc_head = 0xFFxxxxxx: NOP
            //------------------------------------------------------------------------
            case 0xFF:
                break;
            
            //------------------------------------------------------------------------
            // goc_head = 0xxxxxxxxx: Default
            //------------------------------------------------------------------------
            default:
                break;

        }
        
    }
    
    operation_sleep();
    return 0;
}
