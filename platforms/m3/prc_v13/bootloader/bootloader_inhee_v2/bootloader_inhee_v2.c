//*******************************************************************
//Author: Yejoong Kim
//Description: Stream into FLPv1S
//*******************************************************************
#include "PRCv13.h"
#include "mbus.h"
#include "../../software/PMUv3H_temp_deepsleep_v2/PMUv3H_temp_deepsleep_v2.bootinc"

//#define FLPv2      
#define PRC_ADDR 0x1
#define RAD_ADDR 0x4
#define SNS_ADDR 0x5
#define HRV_ADDR 0x6
#define FLS_ADDR 0x7
#define PMU_ADDR 0x8

// DELAYS (For MBus Snooping)
#define MBUS_DELAY  1000

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************

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

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************

void bootloader_mbus_msg32_dly (uint32_t addr, uint32_t data) {
    mbus_write_message32 (addr, data);
    delay(MBUS_DELAY);
}

void fail (uint32_t id) {
    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly (0xE0, 0xDEADBEEF); 
    bootloader_mbus_msg32_dly (0xE1, id); 
    bootloader_mbus_msg32_dly (0xE2, *REG0); 
    mbus_sleep_all();
    while(1);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //Initialize Interrupts
    disable_all_irq();
  
    // Notify the start of the program
    bootloader_mbus_msg32_dly(0xEE, 0xAAAAAAAA);

    //Enumeration
    set_halt_until_mbus_rx();
 	mbus_enumerate(RAD_ADDR);
 	mbus_enumerate(SNS_ADDR);
 	mbus_enumerate(HRV_ADDR);
 	mbus_enumerate(FLS_ADDR);
 	mbus_enumerate(PMU_ADDR);
    delay(MBUS_DELAY);

    // Stream the boot code into Flash Layer
    set_halt_until_mbus_tx();
    mbus_write_message ( ((FLS_ADDR << 4) | MPQ_MEM_STREAM_WRITE_CH0), boot_stream_data, BOOT_STREAM_DATA_LENGTH);
    delay(10000);

//-----------------------------------------------------------------------------------------
// FLPv2S(L) / FLPv2L(L)
//-----------------------------------------------------------------------------------------
#ifdef FLPv2
    // Tune Flash
    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x26 << 24) | 0x0D7788); // Program Current
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x27 << 24) | 0x011BC8); // Erase Pump Diode Chain
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x01 << 24) | 0x000109); // Tprog idle time
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x19 << 24) | 0x000F03); // Voltage Clamper Tuning
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x12 << 24) | 0x000003); // Auto Power On/Off

    // Erase Flash
    uint32_t page_start_addr = 0;
    uint32_t idx;

    for (idx=0; idx<8; idx++){ // Only 8 Pages (8kB) are used.
        page_start_addr = (idx << 8);

        set_halt_until_mbus_tx();
        bootloader_mbus_msg32_dly ((FLS_ADDR << 4), ((0x09 << 24) | page_start_addr)); // Set FLSH_START_ADDR

        set_halt_until_mbus_rx();
        bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x07 << 24) | 0x000029); // Page Erase
        if (*REG0 != 0x00004F) fail((0xFFFF << 16) | idx);
    }

    // Copy SRAM to Flash (Fast Programming)
    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x09 << 24) | 0x0); // Set FLSH_START_ADDR
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x08 << 24) | 0x0); // Set SRAM_START_ADDR

    set_halt_until_mbus_rx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x07 << 24) | 0x01FFE7); // Fast Programming
    if (*REG0 != 0x00005D) fail(1);


    // Erase Flash for Data (inhee)

    page_start_addr = (0x10 << 8);

    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), ((0x09 << 24) | page_start_addr)); // Set FLSH_START_ADDR

    set_halt_until_mbus_rx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x07 << 24) | 0x000029); // Page Erase



//-----------------------------------------------------------------------------------------
// FLPv1S / FLPv1L
//-----------------------------------------------------------------------------------------
#elif FLPv1
    // Tune Flash
    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x19 << 24) | 0x000F03); // Voltage Clamper Tuning
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x02 << 24) | 0x000100); // Terase
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x04 << 24) | 0x000020); // Tcyc_prog

    // Turn on Flash
    set_halt_until_mbus_rx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x11 << 24) | 0x00002F);
    if (*REG0 != 0x0000B5) fail(0);

    // Erase Flash
    uint32_t page_start_addr = 0;
    uint32_t idx;

    for (idx=0; idx<8; idx++){ // Only 8 Pages (8kB) are used.
        page_start_addr = (idx << 8);

        set_halt_until_mbus_tx();
        bootloader_mbus_msg32_dly ((FLS_ADDR << 4), ((0x09 << 24) | page_start_addr)); // Set FLSH_START_ADDR

        set_halt_until_mbus_rx();
        bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x07 << 24) | 0x000029); // Page Erase
        if (*REG0 != 0x00004F) fail((0xFFFF << 16) | idx);
    }

    // Copy SRAM to Flash (Fast Programming)
    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x09 << 24) | 0x0); // Set FLSH_START_ADDR
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x08 << 24) | 0x0); // Set SRAM_START_ADDR

    set_halt_until_mbus_rx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x07 << 24) | 0x01FFE7); // Fast Programming
    if (*REG0 != 0x00005D) fail(1);

    // Turn off Flash
    set_halt_until_mbus_rx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x11 << 24) | 0x00002D);
    if (*REG0 != 0x0000BB) fail(2);

//-----------------------------------------------------------------------------------------
// FLSv2S / FLSv2L
//-----------------------------------------------------------------------------------------
#elif FLSv2
    // Tune Flash
    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x19 << 24) | 0x3C4703); // Voltage Clamper Tuning

    // Turn on Flash (including large cap)
    set_halt_until_mbus_rx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x11 << 24) | 0x00003F);
    if (*REG0 != 0x0000B5) fail(0);

    // Erase Flash
    uint32_t page_start_addr = 0;
    uint32_t idx;

    for (idx=0; idx<4; idx++){ // Only 4 Pages (8kB) are used.
        page_start_addr = (idx << 9);

        set_halt_until_mbus_tx();
        bootloader_mbus_msg32_dly ((FLS_ADDR << 4), ((0x09 << 24) | page_start_addr)); // Set FLSH_START_ADDR

        set_halt_until_mbus_rx();
        bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x07 << 24) | 0x000029); // Page Erase
        if (*REG0 != 0x000055) fail((0xFFFF << 16) | idx);
    }

    // Copy SRAM to Flash (Fast Programming)
    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x09 << 24) | 0x0); // Set FLSH_START_ADDR
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x08 << 24) | 0x0); // Set SRAM_START_ADDR

    set_halt_until_mbus_rx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x07 << 24) | 0x01FFE5); // Programming
    if (*REG0 != 0x00003D) fail(1);

    // Turn off Flash
    set_halt_until_mbus_rx();
    bootloader_mbus_msg32_dly ((FLS_ADDR << 4), (0x11 << 24) | 0x00003D);
    if (*REG0 != 0x0000BB) fail(2);
#endif

    // Notify the end of the program
    set_halt_until_mbus_tx();
    bootloader_mbus_msg32_dly(0xEE, 0x0EA7F00D);


/////////////////  Deep-Sleep Operation ///////////////:w


	mbus_remote_register_write(PMU_ADDR,0x4E,0xF);  // F for 25C, 300 for 125C
	delay(MBUS_DELAY*10);
	mbus_remote_register_write(PMU_ADDR,0x4F,0xDEAD);
	delay(MBUS_DELAY*10);


//    mbus_sleep_all(); // Go to sleep indefinitely
    while(1);

    return 1;
}

