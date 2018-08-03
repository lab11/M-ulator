//*******************************************************************************************
// PREv17 PERIPHERAL LIBRARY
//-------------------------------------------------------------------------------------------
// < DESCRIPTION >
//  - PREv17 Peripheral lib file
//  - Includes definitions and functions for
//      GPIO, SPI, COTS Power Switches
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Aug 02 2018 -   First Commit
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@umich.edu)
//******************************************************************************************* 

#include "PREv17.h"
#include "PREv17_PERIPHERAL.h"
#include "mbus.h"

//*********************************************************
// GPIO Retentive Variables
//*********************************************************
volatile uint32_t __gpio_data__ = 0x0;
volatile uint32_t __gpio_dir__  = 0x0;

//**************************************************
// SPI
//**************************************************

void spi_enable_pad (void) {
    *REG_PERIPHERAL = *REG_PERIPHERAL | 0x00300000; 
}

void spi_disable_pad (void) {
    *REG_PERIPHERAL = *REG_PERIPHERAL & 0xFFCFFFFF; 
}

void spi_freeze (void) {
    *REG_PERIPHERAL = *REG_PERIPHERAL | 0x00800000;
}

void spi_unfreeze (void) {
    *REG_PERIPHERAL = *REG_PERIPHERAL & 0xFF7FFFFF;
}

//**************************************************
// GPIO
//**************************************************

void gpio_enable_pad (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xFF;
    *REG_PERIPHERAL = *REG_PERIPHERAL | pattern_;
}

void gpio_disable_pad (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    *REG_PERIPHERAL = *REG_PERIPHERAL & pattern_;
}

void gpio_freeze (void) {
    *REG_PERIPHERAL = *REG_PERIPHERAL | 0x00010000;
}

void gpio_unfreeze (void) {
    *GPIO_DATA = __gpio_data__;
    *REG_PERIPHERAL = *REG_PERIPHERAL & 0xFFFEFFFF;
}

void gpio_enable_pos_wreq (uint8_t pattern) {
    uint32_t pattern_ = (pattern & 0xF) << 12;
    *REG_PERIPHERAL = *REG_PERIPHERAL | pattern_;
}

void gpio_disable_pos_wreq (uint8_t pattern) {
    uint32_t pattern_ = ~((pattern << 12) | 0x00000000);
    *REG_PERIPHERAL = *REG_PERIPHERAL & pattern_;
}

void gpio_enable_neg_wreq (uint8_t pattern) {
    uint32_t pattern_ = (pattern & 0xF) << 8;
    *REG_PERIPHERAL = *REG_PERIPHERAL | pattern_;
}

void gpio_disable_neg_wreq (uint8_t pattern) {
    uint32_t pattern_ = ~((pattern << 8) | 0x00000000);
    *REG_PERIPHERAL = *REG_PERIPHERAL & pattern_;
}

void gpio_set_dir_in (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    __gpio_dir__ = __gpio_dir__ & pattern_;
    *GPIO_DIR = __gpio_dir__;
}

void gpio_set_dir_out (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xFF;
    __gpio_dir__ = __gpio_dir__ | pattern_;
    *GPIO_DIR = __gpio_dir__;
}

void gpio_set_dir_inout (uint8_t in_pattern, uint8_t out_pattern) {
    uint32_t in_pattern_ = ~(in_pattern | 0x00000000);
    uint32_t out_pattern_ = out_pattern & 0xFF;
    __gpio_dir__ = __gpio_dir__ & in_pattern_;
    __gpio_dir__ = __gpio_dir__ | out_pattern_;
    *GPIO_DIR = __gpio_dir__;
}

void gpio_write_one (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xFF;
    __gpio_data__ = __gpio_data__ | pattern_;
    *GPIO_DATA = __gpio_data__;
}

void gpio_write_zero (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    __gpio_data__ = __gpio_data__ & pattern_;
    *GPIO_DATA = __gpio_data__;
}

void gpio_write_onezero (uint8_t one_pattern, uint8_t zero_pattern) {
    uint32_t one_pattern_  = one_pattern & 0xFF;
    uint32_t zero_pattern_ = ~(zero_pattern | 0x00000000);
    __gpio_data__ = __gpio_data__ & zero_pattern_;
    __gpio_data__ = __gpio_data__ | one_pattern_;
    *GPIO_DATA = __gpio_data__;
}

void gpio_write_direct (uint8_t data) {
    __gpio_data__ = data;
    *GPIO_DATA = __gpio_data__;
}

void gpio_write_mask (uint8_t mask, uint8_t data) {
    uint32_t mask_ = mask & 0xFF;
    __gpio_data__ = (__gpio_data__ & ~mask_) | (mask_ & data);
    *GPIO_DATA = __gpio_data__;
}

uint32_t gpio_read (void) {
// If DIR = OUT, use __gpio_data__
// If DIR = IN,  use *GPIO_DATA
    uint32_t dir_out = __gpio_dir__;
    return ((*GPIO_DATA & ~dir_out) | (__gpio_data__ & dir_out));
}

void gpio_enable_irq (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xFF;
    *GPIO_IRQ_MASK = *GPIO_IRQ_MASK | pattern_;
}

void gpio_disable_irq (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    *GPIO_IRQ_MASK = *GPIO_IRQ_MASK & pattern_;
}

void gpio_mbus_sleep_all (void) {
    gpio_freeze();
    mbus_sleep_all();
    while(1);
}

//**************************************************
// COTS Power Switches
//**************************************************

void cps_on (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xF;
    *REG_CPS = *REG_CPS | pattern_;
}

void cps_off (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    *REG_CPS = *REG_CPS & pattern_;
}

