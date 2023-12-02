//*******************************************************************
//Author: Yejoong Kim
//Description: Developed during PREv22 tape-out for verification
//*******************************************************************
#include "PREv22.h"
#include "FLPv3S_RF.h"
#include "PMUv9_RF.h"
#include "SNSv11_RF.h"
#include "RDCv2_RF.h"
#include "mbus.h"

#define PRE_ADDR    0x1
#define FLP_ADDR    0x4
#define NODE_A_ADDR 0x8
#define SNS_ADDR    0xC
#define RDC_ADDR    0x5
#define PMU_ADDR    0xE

// Flag Idx
#define FLAG_ENUM       0
#define FLAG_VCO        1

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t cyc_num;
volatile uint32_t irq_history;
volatile uint32_t irq_handled;
volatile uint32_t temp_val;

volatile uint32_t gpio_mask1 = (1 << 7);
volatile uint32_t gpio_mask2 = (1 << 6);

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_wakeup   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_softreset(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gocep    (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer32  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_timer16  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbustx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusrx   (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusfwd  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg0     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg1     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg2     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg3     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg4     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg5     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg6     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_reg7     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_mbusmem  (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_gpio     (void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_spi      (void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_timer32(void) { // TIMER32
    *NVIC_ICPR = (0x1 << IRQ_TIMER32); irq_history |= (0x1 << IRQ_TIMER32);
    arb_debug_reg(IRQ_TIMER32, 0x00000000);
}
void handler_ext_int_timer16(void) { // TIMER16
    *NVIC_ICPR = (0x1 << IRQ_TIMER16); irq_history |= (0x1 << IRQ_TIMER16);
    arb_debug_reg(IRQ_TIMER16, 0x00000000);
}
void handler_ext_int_reg0(void) { // REG0
    *NVIC_ICPR = (0x1 << IRQ_REG0); irq_history |= (0x1 << IRQ_REG0);
    arb_debug_reg(IRQ_REG0, 0x00000000);
}
void handler_ext_int_reg1(void) { // REG1
    *NVIC_ICPR = (0x1 << IRQ_REG1); irq_history |= (0x1 << IRQ_REG1);
    arb_debug_reg(IRQ_REG1, 0x00000000);
}
void handler_ext_int_reg2(void) { // REG2
    *NVIC_ICPR = (0x1 << IRQ_REG2); irq_history |= (0x1 << IRQ_REG2);
    arb_debug_reg(IRQ_REG2, 0x00000000);
}
void handler_ext_int_reg3(void) { // REG3
    *NVIC_ICPR = (0x1 << IRQ_REG3); irq_history |= (0x1 << IRQ_REG3);
    arb_debug_reg(IRQ_REG3, 0x00000000);
}
void handler_ext_int_reg4(void) { // REG4
    *NVIC_ICPR = (0x1 << IRQ_REG4); irq_history |= (0x1 << IRQ_REG4);
    arb_debug_reg(IRQ_REG4, 0x00000000);
}
void handler_ext_int_reg5(void) { // REG5
    *NVIC_ICPR = (0x1 << IRQ_REG5); irq_history |= (0x1 << IRQ_REG5);
    arb_debug_reg(IRQ_REG5, 0x00000000);
}
void handler_ext_int_reg6(void) { // REG6
    *NVIC_ICPR = (0x1 << IRQ_REG6); irq_history |= (0x1 << IRQ_REG6);
    arb_debug_reg(IRQ_REG6, 0x00000000);
}
void handler_ext_int_reg7(void) { // REG7
    *NVIC_ICPR = (0x1 << IRQ_REG7); irq_history |= (0x1 << IRQ_REG7);
    arb_debug_reg(IRQ_REG7, 0x00000000);
}
void handler_ext_int_mbusmem(void) { // MBUS_MEM_WR
    *NVIC_ICPR = (0x1 << IRQ_MBUS_MEM); irq_history |= (0x1 << IRQ_MBUS_MEM);
    arb_debug_reg(IRQ_MBUS_MEM, 0x00000000);
}
void handler_ext_int_mbusrx(void) { // MBUS_RX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_RX); irq_history |= (0x1 << IRQ_MBUS_RX);
    arb_debug_reg(IRQ_MBUS_RX, 0x00000000);
}
void handler_ext_int_mbustx(void) { // MBUS_TX
    *NVIC_ICPR = (0x1 << IRQ_MBUS_TX); irq_history |= (0x1 << IRQ_MBUS_TX);
    arb_debug_reg(IRQ_MBUS_TX, 0x00000000);
}
void handler_ext_int_mbusfwd(void) { // MBUS_FWD
    *NVIC_ICPR = (0x1 << IRQ_MBUS_FWD); irq_history |= (0x1 << IRQ_MBUS_FWD);
    arb_debug_reg(IRQ_MBUS_FWD, 0x00000000);
}
void handler_ext_int_gocep(void) { // GOCEP
    *NVIC_ICPR = (0x1 << IRQ_GOCEP); irq_history |= (0x1 << IRQ_GOCEP);
    arb_debug_reg(IRQ_GOCEP, 0x00000000);
}
void handler_ext_int_softreset(void) { // SOFT_RESET
    *NVIC_ICPR = (0x1 << IRQ_SOFT_RESET); irq_history |= (0x1 << IRQ_SOFT_RESET);
    arb_debug_reg(IRQ_SOFT_RESET, 0x00000000);
}
void handler_ext_int_wakeup(void) { // WAKE-UP
    *NVIC_ICPR = (0x1 << IRQ_WAKEUP); irq_history |= (0x1 << IRQ_WAKEUP);
    uint32_t wakeup_source = *SREG_WAKEUP_SOURCE;
    *SCTR_REG_CLR_WUP_SOURCE = 1;
    arb_debug_reg(IRQ_WAKEUP, (0x10 << 24) | wakeup_source);
}
void handler_ext_int_spi(void) { // SPI
    *NVIC_ICPR = (0x1 << IRQ_SPI); irq_history |= (0x1 << IRQ_SPI);
    arb_debug_reg(IRQ_SPI, 0x00000000);
}
void handler_ext_int_gpio(void) { // GPIO
    *NVIC_ICPR = (0x1 << IRQ_GPIO); irq_history |= (0x1 << IRQ_GPIO);
    arb_debug_reg(IRQ_GPIO, 0x00000000);
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    set_flag(FLAG_ENUM, 1);
    cyc_num = 0;
    irq_history = 0;

    // Set Halt
    set_halt_until_mbus_trx();

    // Enumeration
    mbus_enumerate(FLP_ADDR);
    mbus_enumerate(NODE_A_ADDR);
    mbus_enumerate(SNS_ADDR);
    mbus_enumerate(RDC_ADDR);
    mbus_enumerate(PMU_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();
}

void pass (uint32_t id, uint32_t data) {
    arb_debug_reg(0xE0, 0x0);
    arb_debug_reg(0xE1, id);
    arb_debug_reg(0xE2, data);
}

void fail (uint32_t id, uint32_t data) {
    arb_debug_reg(0xE8, 0x0);
    arb_debug_reg(0xE9, id);
    arb_debug_reg(0xEA, data);
    *REG_CHIP_ID = 0xFFFFFF; // This will stop the verilog sim.
}

//********************************************************************
// MAIN function starts here             
//********************************************************************

int main() {
    // Initialization Sequence
    if (!get_flag(FLAG_ENUM)) { initialization(); }

    set_spi_pad(1);

//    set_gpio_pad_with_mask(gpio_mask2,(1<<6));
//    gpio_set_dir_with_mask(gpio_mask2,(1<<6));
//    gpio_write_data_with_mask(gpio_mask2,(1<<6));
//
//    mbus_write_message32(0xA0, *GPIO_DATA);
//
//    set_gpio_pad_with_mask(gpio_mask1,(1<<7));
//    gpio_set_dir_with_mask(gpio_mask1,(1<<7));
//    gpio_write_data_with_mask(gpio_mask1,(1<<7));
//
//    mbus_write_message32(0xA0, 0x22222222);
//
//    unfreeze_gpio_out();
//    unfreeze_spi_out();

    // GPIO
    gpio_set_dir(0xFF);
    set_gpio_pad(0xFF);
    gpio_write_data(0x00);
    unfreeze_gpio_out();

    mbus_write_message32(0xA0, 0x11111111);

    *GPIO_DATA = 0xFF;
    *GPIO_DATA = 0x00;

    *((volatile uint32_t *) 0xA0005000) = 0xFF;
    *((volatile uint32_t *) 0xA0005000) = 0x00;

    *((volatile uint32_t *) 0xA0005300) = 0xFF;
    *((volatile uint32_t *) 0xA0005300) = 0x00;


//    gpio_write_data(0x00);
//    gpio_write_data(0x10);
//    gpio_write_data(0x20);
//    gpio_write_data(0x30);
//    gpio_write_data(0x40);
//    gpio_write_data(0x50);
//    gpio_write_data(0x60);
//    gpio_write_data(0x70);
//    gpio_write_data(0x80);
//    gpio_write_data(0x90);
//    gpio_write_data(0xA0);
//    gpio_write_data(0xB0);
//    gpio_write_data(0xC0);
//    gpio_write_data(0xD0);
//    gpio_write_data(0xE0);
//    gpio_write_data(0xF0);
//
//    mbus_write_message32(0xE0, 0x22222222);
//
//    *GPIO_DATA = 0x00;
//    *GPIO_DATA = 0x10;
//    *GPIO_DATA = 0x20;
//    *GPIO_DATA = 0x30;
//    *GPIO_DATA = 0x40;
//    *GPIO_DATA = 0x50;
//    *GPIO_DATA = 0x60;
//    *GPIO_DATA = 0x70;
//    *GPIO_DATA = 0x80;
//    *GPIO_DATA = 0x90;
//    *GPIO_DATA = 0xA0;
//    *GPIO_DATA = 0xB0;
//    *GPIO_DATA = 0xC0;
//    *GPIO_DATA = 0xD0;
//    *GPIO_DATA = 0xE0;
//    *GPIO_DATA = 0xF0;
//
//    mbus_write_message32(0xE0, 0x33333333);

    // Go to sleep
    mbus_sleep_all();

    //Never Quit (should not come here.)
    while(1){          
        asm("nop;"); 
    }

    return 1;
}
