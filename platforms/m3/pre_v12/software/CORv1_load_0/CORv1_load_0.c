//*******************************************************************
//Author: Yejoong Kim
//Description: Stream into CORv1
//*******************************************************************
#include "PREv12.h"
#include "mbus.h"
#include "../../prog/prog_0.inc"

#define PRC_ADDR    0x1
#define COR_ADDR    0x4

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void init_interrupt (void) {
	*NVIC_ICPR = 0x1FFFF; //Clear All Pending Interrupts
	*NVIC_ISER = 0x0;     //Disable Interrupts
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
void handler_ext_int_15(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_16(void) __attribute__ ((interrupt ("IRQ")));

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
void handler_ext_int_15(void) {/*SPI*/      *NVIC_ICPR = (0x1 << 15); }
void handler_ext_int_16(void) {/*GPIO*/     *NVIC_ICPR = (0x1 << 16); }

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
    mbus_enumerate(COR_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();
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

    //Set Halt Mode
    set_halt_until_mbus_tx();

    //-- Stream the code into CORv1 --//
    
    // COR_RESETn
	mbus_remote_register_write(COR_ADDR,0x0,0x00003D);
	mbus_remote_register_write(COR_ADDR,0x0,0x00001D);
    set_halt_until_mbus_rx();
	mbus_remote_register_read(COR_ADDR,0x0,0x0);
    set_halt_until_mbus_tx();

    // SET CLOCKS
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_00_00, COR_MSG_00_00);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_00_01, COR_MSG_00_01);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_00_02, COR_MSG_00_02);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_00_03, COR_MSG_00_03);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_00_04, COR_MSG_00_04);

    // MBUS_INTERRUPT
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_00, COR_MSG_01_00);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_01, COR_MSG_01_01);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_02, COR_MSG_01_02);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_03, COR_MSG_01_03);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_04, COR_MSG_01_04);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_05, COR_MSG_01_05);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_06, COR_MSG_01_06);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_07, COR_MSG_01_07);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_08, COR_MSG_01_08);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_09, COR_MSG_01_09);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_10, COR_MSG_01_10);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_11, COR_MSG_01_11);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_12, COR_MSG_01_12);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_01_13, COR_MSG_01_13);

    // PROGRAM_CODE
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_02_00, COR_MSG_02_00);

    // SET MEMMAP REG
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_03_00, COR_MSG_03_00);

    // RESET
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_04_00, COR_MSG_04_00);
    delay(1000);
    mbus_write_message ( ((COR_ADDR << 4) | MPQ_MEM_BULK_WRITE), cor_msg_04_01, COR_MSG_04_01);

    //-- End of the streaming --//
    
    // Notify the end of the program
    delay(10000);
    mbus_write_message32(0xDD, 0x0EA7F00D);
    set_halt_until_mbus_rx();
    mbus_write_message32(((COR_ADDR << 4) | MPQ_REG_READ), 0x00000700);
    delay(1000);
    mbus_copy_mem_from_remote_to_any_bulk(COR_ADDR, 0xA2000124,0x7,0x00000000,0x00000000);
    //mbus_sleep_all(); // Go to sleep indefinitely
    while(1);

    return 1;
}
