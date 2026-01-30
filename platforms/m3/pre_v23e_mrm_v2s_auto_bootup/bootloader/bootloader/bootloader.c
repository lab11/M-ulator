//*******************************************************************************************
// Bootloader for a Multi-MRMv2S System
//-------------------------------------------------------------------------------------------
// < PREPARATION >
//
//  - Prepare *.bootinc file of the main program. Prepare a dummy .bootinc file as well.
//      Update the #include paths as needed.
//
//  - Update the short prefixes and enumerations as needed.
//
//  - Update 'MRM Layer Handling' section. Add/remove MRM layers as needed.
//
//  - [IMPORTANT] Choose a proper TMC configuration in mrm_init() in libs/MRMv2S.c
//      For verilog simulation    : Use mrm_enable_tmc_rst_auto_wk_for_sim()
//      For real silicon (post-CP): Use mrm_enable_tmc_rst_auto_wk()
//
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Nov 12 2025 -   First Commit
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io, yejoong@umich.edu)
//******************************************************************************************* 
#include "mbus.h"
#include "PREv23E.h"
#include "MRMv2S.h"
#include "MRMv2S_RF.h"
#include "PMUv13_RF.h"
#include "../../software/dummy/dummy.bootinc"
#include "../../software/main/main.bootinc"

#define PRE_ADDR    0x1
#define MRM0_ADDR   0x2
#define MRM1_ADDR   0x3
#define MRM2_ADDR   0x4
#define PMU_ADDR    0x9

//*******************************************************************************************
// FLAG BIT INDEXES
//*******************************************************************************************
#define FLAG_ENUMERATED         0
#define FLAG_INITIALIZED        1

//********************************************************************
// MRM Functions
//********************************************************************

uint32_t flash_mrm_with_dummy_program(uint32_t mrm_prefix) {

    // Write to the SRAM
    mbus_write_message ( ((mrm_prefix << 4) | MPQ_MEM_STREAM_WRITE_CH0), dummy_boot_stream_data, DUMMY_BOOT_STREAM_DATA_LENGTH);

    // Copy into MRAM
    return mrm_sram2mram(   /*mrm_prefix*/mrm_prefix, 
                            /*sram_pid*/0, 
                            /*num_pages*/((DUMMY_BOOT_STREAM_DATA_LENGTH-1)>>MRM_LOG2_NUM_WORDS_PER_PAGE)+1, 
                            /*mram_pid*/0
                        );
};

uint32_t flash_mrm_with_program(uint32_t mrm_prefix) {

    // Write to the SRAM
    mbus_write_message ( ((mrm_prefix << 4) | MPQ_MEM_STREAM_WRITE_CH0), boot_stream_data, BOOT_STREAM_DATA_LENGTH);

    // Copy into MRAM
    return mrm_sram2mram(   /*mrm_prefix*/mrm_prefix, 
                            /*sram_pid*/0, 
                            /*num_pages*/((BOOT_STREAM_DATA_LENGTH-1)>>MRM_LOG2_NUM_WORDS_PER_PAGE)+1, 
                            /*mram_pid*/0
                        );
};

//*******************************************************************
// INTERRUPT HANDLERS 
//*******************************************************************

void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));

// REG1 IRQ (MRM)
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (1 << IRQ_REG1);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //-------------------------------------------------
    // PRE Tuning
    //-------------------------------------------------
    
    // Disable PRE Watchdog & MBus Watchdog
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;

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
    mbus_enumerate(MRM0_ADDR);
    mbus_enumerate(MRM1_ADDR);
    mbus_enumerate(MRM2_ADDR);
    mbus_enumerate(PMU_ADDR);
    set_halt_until_mbus_tx();
    set_flag(FLAG_ENUMERATED, 1);

    //-------------------------------------------------
    // MRM Layer Handling
    //-------------------------------------------------
    
    //--- Force Reset MRMv2S_CTRL
    mbus_write_message32(MRM0_ADDR<<4, 0x1F000000);
    mbus_write_message32(MRM1_ADDR<<4, 0x1F000000);
    mbus_write_message32(MRM2_ADDR<<4, 0x1F000000);

    //--- Change Bootup Flag
    mbus_write_message32(MRM0_ADDR<<4, 0x10000003);
    mbus_write_message32(MRM1_ADDR<<4, 0x10000003);
    mbus_write_message32(MRM2_ADDR<<4, 0x10000003);

    //--- Force Release Reset MRMv2S_CTRL
    mbus_write_message32(MRM0_ADDR<<4, 0x1F000001);
    mbus_write_message32(MRM1_ADDR<<4, 0x1F000001);
    mbus_write_message32(MRM2_ADDR<<4, 0x1F000001);

    //--- MRAM Initialization
    mrm_init(/*mrm_prefix*/MRM0_ADDR, /*clk_gen_s*/64, /*ldo_v0p8*/0, /*ldo_v1p8*/4);
    mrm_init(/*mrm_prefix*/MRM1_ADDR, /*clk_gen_s*/64, /*ldo_v0p8*/0, /*ldo_v1p8*/4);
    mrm_init(/*mrm_prefix*/MRM2_ADDR, /*clk_gen_s*/64, /*ldo_v0p8*/0, /*ldo_v1p8*/4);

    //--- Programming
    flash_mrm_with_dummy_program(/*mrm_prefix*/MRM0_ADDR);
    flash_mrm_with_dummy_program(/*mrm_prefix*/MRM1_ADDR);
    flash_mrm_with_program(/*mrm_prefix*/MRM2_ADDR);

    //-------------------------------------------------
    // End of Bootloader
    //-------------------------------------------------
    // Notify the end of the program
    set_halt_until_mbus_tx();
    mbus_write_message32(0xEE, 0x0EA7F00D);
    mbus_sleep_all(); // Go to sleep indefinitely
    while(1);

    return 1;
}

