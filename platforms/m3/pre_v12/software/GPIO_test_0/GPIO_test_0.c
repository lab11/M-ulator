//*******************************************************************
//Author: Yejoong Kim
//Description: GPIO_test_0
//             test the basic functionality of GPIO
//NOTE: GPIO0 and GPIO3 should be connected off-chip
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
  *NVIC_ISER = 0x10000; //0x18000: Enable only GPIO
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
void handler_ext_int_16(void) {/*GPIO*/     
    *NVIC_ICPR = (0x1 << 16); 
    gpio_rx_num ++;
    if (gpio_rx_num < 8) *GPIO_DATA = gpio_rx_num;
}

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

    uint32_t do_gpio = 0;
    uint32_t do_spi = 1;

    if (do_gpio) {
	    // GPIO initialization
	    // GPIO 0-2: Output
	    // GPIO 3: Input
	    // All others are Output
	    gpio_init (0x000000F7);
	
	    // Enable IRQ from GPIO3
	    gpio_set_irq_mask (0x00000008);
	
	    gpio_rx_num = 1;
	    *GPIO_DATA = gpio_rx_num;
	
	    while (gpio_rx_num < 8);
	
	    // Close GPIO
	    gpio_close();
    }

    if (do_spi) {
        delay(100);
	    enable_io_pad();
	    *SPI_SSPCR0 = 0x0287; // Motorola Mode
	    *SPI_SSPCR1 = 0x2;
	    *SPI_SSPCPSR = 0x02; // Clock Prescale Register
	    //*SPI_SSPDMACR = 0x3;
	    *SPI_SSPDR = 0x1234;

        delay(1000);
	
	    disable_io_pad();
    }

    // Notify the end of the program
    mbus_write_message32(0xDD, 0x0EA7F00D);
    mbus_sleep_all(); // Go to sleep indefinitely
    while(1);

    return 1;
}
