//*******************************************************************
//Author: Yejoong Kim
//Description: Stream into FLSv1L/FLSv1S
//*******************************************************************
#include "PRCv12.h"
#include "FLSv1L.h"
#include "mbus.h"
#include "../../software/PRCv12_FLSv1L_simple_wakeup_sleep/PRCv12_FLSv1L_simple_wakeup_sleep.bootinc"

#define PRC_ADDR    0x1
#define FLS_ADDR    0x4

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void init_interrupt (void) {
  *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
  *NVIC_ISER = 0x0; //Disable Interrupts
}

void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_14(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void)  {/*TIMER32*/  *NVIC_ICPR = (0x1 <<  0); }
void handler_ext_int_1(void)  {/*TIMER16*/  *NVIC_ICPR = (0x1 <<  1); }
void handler_ext_int_2(void)  {/*REG0*/     *NVIC_ICPR = (0x1 <<  2); }
void handler_ext_int_3(void)  {/*REG1*/     *NVIC_ICPR = (0x1 <<  3); }
void handler_ext_int_4(void)  {/*REG2*/     *NVIC_ICPR = (0x1 <<  4); }
void handler_ext_int_5(void)  {/*REG3*/     *NVIC_ICPR = (0x1 <<  5); }
void handler_ext_int_6(void)  {/*REG4*/     *NVIC_ICPR = (0x1 <<  6); }
void handler_ext_int_7(void)  {/*REG5*/     *NVIC_ICPR = (0x1 <<  7); }
void handler_ext_int_8(void)  {/*REG6*/     *NVIC_ICPR = (0x1 <<  8); }
void handler_ext_int_9(void)  {/*REG7*/     *NVIC_ICPR = (0x1 <<  9); }
void handler_ext_int_10(void) {/*MEM_WR*/   *NVIC_ICPR = (0x1 << 10); }
void handler_ext_int_11(void) {/*MBUS_RX*/  *NVIC_ICPR = (0x1 << 11); }
void handler_ext_int_12(void) {/*MBUS_TX*/  *NVIC_ICPR = (0x1 << 12); }
void handler_ext_int_13(void) {/*MBUS_FWD*/ *NVIC_ICPR = (0x1 << 13); }
void handler_ext_int_14(void) {/*GOCEP*/    *NVIC_ICPR = (0x1 << 14); }

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;

    //Chip ID
    write_regfile (REG_CHIP_ID, 0xDEAD);

    // Notify the start of the program
    mbus_write_message32(0xAA, 0xAAAAAAAA);

    //Set Halt
    set_halt_until_mbus_rx();

    //Enumeration
    mbus_enumerate(FLS_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();

    //Optimize FLSv1L Tuning
    FLSv1L_setOptTune(FLS_ADDR);
    
    //Set IRQ Address in FLSv1L (Write irq_payload into REG0)
    FLSv1L_setIRQAddr(FLS_ADDR, 0x10, 0x00);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //Initialize Interrupts
    init_interrupt();
  
    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization(); // Enumeration.
    }

    // Stream the boot code into FLSv1
    mbus_write_message ( ((FLS_ADDR << 4) | MPQ_MEM_STREAM_WRITE_CH0), boot_stream_data, BOOT_STREAM_DATA_LENGTH);

    // Turn on Flash
    set_halt_until_mbus_rx();
    FLSv1L_turnOnFlash(FLS_ADDR);

    if (*REG0 != 0x000003) { 
        set_halt_until_mbus_tx();
        mbus_write_message32 (0xE0, 0xDEADBEEF); 
        mbus_write_message32 (0xE0, *REG0); 
        mbus_sleep_all();
        while(1);
    }
    
    // Connect Large Cap  
    set_halt_until_mbus_tx();
    FLSv1L_enableLargeCap(FLS_ADDR);

    // Erase Flash (8kB = 1 Page)
    set_halt_until_mbus_tx();
    FLSv1L_setFlashStartAddr(FLS_ADDR, 0x00000000);

    set_halt_until_mbus_rx();
    FLSv1L_doEraseFlash(FLS_ADDR);

    if (*REG0 != 0x000074) { 
        set_halt_until_mbus_tx();
        mbus_write_message32 (0xE1, 0xDEADBEEF); 
        mbus_write_message32 (0xE1, *REG0); 
        mbus_sleep_all();
        while(1);
    }

    // Copy SRAM to Flash (8kB = 1 Page)
    set_halt_until_mbus_tx();
    FLSv1L_setFlashStartAddr(FLS_ADDR, 0x00000000);
    FLSv1L_setSRAMStartAddr(FLS_ADDR, 0x00000000);

    set_halt_until_mbus_rx();
    FLSv1L_doCopySRAM2Flash(FLS_ADDR, 0x7FE); // 0x7FE = 2048 words = 8kB

    if (*REG0 != 0x00005C) {
        set_halt_until_mbus_tx();
        mbus_write_message32 (0xE2, 0xDEADBEEF); 
        mbus_write_message32 (0xE2, *REG0); 
        mbus_sleep_all();
        while(1);
    }

    // Turn off Flash
    set_halt_until_mbus_rx();
    FLSv1L_turnOffFlash(FLS_ADDR);
    
    if (*REG0 != 0x000006) {
        set_halt_until_mbus_tx();
        mbus_write_message32 (0xE3, 0xDEADBEEF); 
        mbus_write_message32 (0xE3, *REG0); 
        mbus_sleep_all();
        while(1);
    }

    // Disconnect Large Cap
    set_halt_until_mbus_tx();
    FLSv1L_disableLargeCap(FLS_ADDR);

    // Notify the end of the program
    mbus_write_message32(0xDD, 0x0EA7F00D);
    mbus_sleep_all(); // Go to sleep indefinitely
    while(1);

    return 1;
}

