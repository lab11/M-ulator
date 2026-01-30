//*******************************************************************
// C SOURCE CODE FOR PRC/PRE VERILOG TESTBENCH
// main.c
//-------------------------------------------------------------------
// < UPDATE HISTORY >
//  Sample
//-------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//*******************************************************************
#include "PREv23E.h"
#include "MRMv2S_RF.h"
#include "PMUv13_RF.h"
#include "mbus.h"

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

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
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
    if (!get_flag(FLAG_ENUMERATED)) {
        set_halt_until_mbus_trx();
        mbus_enumerate(MRM0_ADDR);
        mbus_enumerate(MRM1_ADDR);
        mbus_enumerate(MRM2_ADDR);
        mbus_enumerate(PMU_ADDR);
        set_halt_until_mbus_tx();
        set_flag(FLAG_ENUMERATED, 1);
    }

    // Update the flag
    set_flag(FLAG_INITIALIZED, 1);

}

static void operation_sleep(void){
    // Reset GOC_DATA_IRQ
    *GOC_DATA_IRQ = 0xFFFFFFFF;
    
    // Go to Sleep
    mbus_sleep_all();
    while(1);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    
    // Disable PRE Watchdog & MBus Watchdog
    *TIMERWD_GO  = 0;
    *REG_MBUS_WD = 0;

    if (!get_flag(FLAG_INITIALIZED)) 
        operation_init();

    uint32_t i;
    for (i=0; i<10; i++) {
        mbus_write_message32(0xE0, i);
    }

    operation_sleep();
    return 0;
}
