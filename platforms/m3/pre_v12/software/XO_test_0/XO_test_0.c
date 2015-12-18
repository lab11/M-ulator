//*******************************************************************
//Author: Yu Zeng
//Description: XO_test_0
//*******************************************************************
#include "PREv12.h"
#include "mbus.h"

#define PRC_ADDR    0x1
#define FLS_ADDR    0x4

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t gpio_rx_num;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void init_interrupt (void) {
  *NVIC_ICPR = 0x1FFFF; //Clear All Pending Interrupts
  *NVIC_ISER = 0x1FFFF; //Enable all interrupts
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
    mbus_enumerate(FLS_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();

	delay(1000);
	mbus_write_message32(0xDD, 0x0);
	delay(1000);
	write_regfile (REG_XO_CONFIG, 0x0011E5);
	delay(1021);
	mbus_write_message32(0xDD, 0x1);
	delay(60000); // 1s
	write_regfile (REG_XO_CONFIG, 0x0011E7);
	delay(1021);
	mbus_write_message32(0xDD, 0x2);
	delay(40000); // 300us
	write_regfile (REG_XO_CONFIG, 0x0011E4);
	delay(1021);
	mbus_write_message32(0xDD, 0x3);
	delay(600000); // 1s 
	
	write_regfile (REG_XO_CONFIG, 0x0011D4);	
	delay(60000);
	mbus_write_message32(0xDD, 0x4);
	
	//write_regfile (REG_XO_CONFIG, 0x0011C5);
	//mbus_write_message32(0xDD, 0x5);
	//delay(6000);
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {

    //Initialize Interrupts
    init_interrupt();//cygwin.com/cygw
  
    // Initialization Sequence
    if (enumerated != 0xDEADBEEF) { 
        initialization(); // Enumeration.
    }
//cygwin.com/cygw



// 0000_0000_000 1_000 0_11 00_0101
// 0010C5 : Default

// 0000_0000_000 1_000 0_11 10_0101
// 0010E5/D/9/1
// 1s

// 0000_0000_000 1_000 0_11 10_0111
// 0010E7/F/B/3
// 300us

// 0000_0000_000 1_000 0_11 10_0100
// 0010E4/C/8/0
// 1s

// 0000_0000_000 1_000 0_11 01_0100
// 0010D4/C/8/0
//  clock 

// to turn off
// 0000_0000_000 1_000 0_11 00_0101
// 0010C5

	//delay(1000);
	

	/*
	delay(1000);
	mbus_write_message32(0xDD, 0x0);
	delay(1000);
	write_regfile (REG_XO_CONFIG, 0x0011E5);
	delay(1021);
	mbus_write_message32(0xDD, 0x1);
	delay(60000); // 1s

	write_regfile (REG_XO_CONFIG, 0x0011E7);
	delay(1021);
	mbus_write_message32(0xDD, 0x2);
	delay(40000); // 300us

	write_regfile (REG_XO_CONFIG, 0x0011E4);
	delay(1021);
	mbus_write_message32(0xDD, 0x3);
	delay(600000); // 1s

	
	write_regfile (REG_XO_CONFIG, 0x0011D4);	
	delay(1000);
	mbus_write_message32(0xDD, 0x4);
	*/
	delay(1021);

	
	
    //mbus_write_message32(0xDD, 0xBBBBBBBB);
//	while(1);
	// clock

    //Set Halt Mode
	//delay(600000);
    set_halt_until_mbus_tx();

	while(1)
	{
	*XOT_RESET = 1;
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	}
	//

    // Notify the end of the program
    mbus_write_message32(0xDD, 0x0EA7F00D);

	//set_wakeup_timer ( 3, 1, 1);

	// XOT Counter
	write_regfile (REG_XOT_CONFIG,     (1 /*IRQ_IN_SLEEP_ONLY */ << 16)
						| (0 /* IRQ_EN */ << 15 )
						| (32767 /* TSTAMP_SAT */ << 0)); // 15-bit counter (~1sec)
	mbus_write_message32(0xDD, *REG_XOT_CONFIG);
	//*XOT_RESET = 0;
//	while(1);
 
     
	*XOT_RESET = 0;
	mbus_write_message32(0xDD, *REG_XOT_VAL);
	delay(1021);
	mbus_write_message32(0xDD, *REG_XOT_VAL);
//	delay(1021);
    mbus_sleep_all(); // Go to sleep
    //while(1);
	while(*REG_XOT_CONFIG!=0);
	
    return 1;

}
