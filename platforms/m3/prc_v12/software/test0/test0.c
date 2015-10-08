//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12_FLSv1L
//*******************************************************************
#include "PRCv12.h"
#include "FLSv1L.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLS_ADDR    0x4

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t num_cycle;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void init_interrupt (void) {
  *NVIC_ICPR = 0x7FFF; //Clear All Pending Interrupts
  *NVIC_ISER = 0x7FFF; //Enable Interrupts
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
    num_cycle = 0;

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

void cycle0 (void) {
    set_halt_until_mbus_rx();
    FLSv1L_turnOnFlash (FLS_ADDR);

    set_halt_until_mbus_tx();
    FLSv1L_enableLargeCap (FLS_ADDR);

    delay(10000); // Wait for a while before turning off flash again

    FLSv1L_disableLargeCap (FLS_ADDR);

    set_halt_until_mbus_rx();
    FLSv1L_turnOffFlash (FLS_ADDR);

    set_halt_until_mbus_tx();
}

void cycle1 (void) {
    // Set SRAM/Flash Start Address
    FLSv1L_setSRAMStartAddr  (FLS_ADDR, 0x00000000);
    FLSv1L_setFlashStartAddr (FLS_ADDR, 0x00000000);

    // Write into FLSv1L SRAM (Stream Channel 0)
    static uint32_t mem_data[16] = {  0x00000000, 0x11111111, 0x22222222, 0x33333333,
                               0x44444444, 0x55555555, 0x66666666, 0x77777777,
                               0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB,
                               0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF,
                               };
    mbus_write_message ( ((FLS_ADDR << 4) | MPQ_MEM_STREAM_WRITE_CH0), mem_data, 16);

    // Read from FLSv1L SRAM and write into PRCv12's SRAM (at the 7kB position)
    set_halt_until_mbus_rx();
    uint32_t mbus_msg[3] = {0x1200000F, 0x00000000, 0x00001C00};
    mbus_write_message ( ((FLS_ADDR << 4) | MPQ_MEM_READ), mbus_msg, 3);
    set_halt_until_mbus_tx();

    mbus_copy_mem_from_local_to_remote_stream (0x0, FLS_ADDR, (uint32_t *) 0x00001C00, 15);
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

    // Display num_cycle;
    mbus_write_message32(0xEF, num_cycle);
    
    // Testing Sequence
    if      (num_cycle == 0) cycle0();   // Test Flash Power On/Off & Interrupts
    else if (num_cycle == 1) cycle1();
    else {
        mbus_write_message32(0xEF, 0x0EA7F00D);
        while(1);
    }
        
    num_cycle++;

    set_wakeup_timer(5, 1, 1);
    mbus_sleep_all();

    while(1);

    return 1;
}

