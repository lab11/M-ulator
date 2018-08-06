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

#include "PREv17_PERIPHERAL.h"
#include "mbus.h"

//*********************************************************
// GPIO Retentive Variables
//*********************************************************
volatile uint32_t __gpio_data__;
volatile uint32_t __gpio_dir__;
volatile uint32_t __gpio_irq_mask__;

//**************************************************
// SPI
//**************************************************

void spi_enable_pad (void) {
    *PRE17P_REG_PERI = *PRE17P_REG_PERI | 0x00300000; 
}

void spi_disable_pad (void) {
    *PRE17P_REG_PERI = *PRE17P_REG_PERI & 0xFFCFFFFF; 
}

void spi_freeze (void) {
    *PRE17P_REG_PERI = *PRE17P_REG_PERI | 0x00800000;
}

void spi_unfreeze (void) {
    *PRE17P_REG_PERI = *PRE17P_REG_PERI & 0xFF7FFFFF;
}

//**************************************************
// GPIO
//**************************************************

void gpio_initialize (void) {
    __gpio_data__ = 0x00; // All Zero
    __gpio_dir__ = 0x00;  // All Input
    __gpio_irq_mask__ = 0x00;  // All Disabled
    *PRE17P_GPIO_DATA = __gpio_data__;
    *PRE17P_GPIO_DIR  = __gpio_dir__;
    *PRE17P_GPIO_IRQ_MASK  = __gpio_irq_mask__;
}

void gpio_enable_pad (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xFF;
    *PRE17P_REG_PERI = *PRE17P_REG_PERI | pattern_;
}

void gpio_disable_pad (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    *PRE17P_REG_PERI = *PRE17P_REG_PERI & pattern_;
}

void gpio_freeze (void) {
    *PRE17P_REG_PERI = *PRE17P_REG_PERI | 0x00010000;
}

void gpio_unfreeze (void) {
    *PRE17P_GPIO_DATA = __gpio_data__;
    *PRE17P_GPIO_DIR  = __gpio_dir__;
    *PRE17P_GPIO_IRQ_MASK  = __gpio_irq_mask__;
    *PRE17P_REG_PERI = *PRE17P_REG_PERI & 0xFFFEFFFF;
}

void gpio_enable_pos_wreq (uint8_t pattern) {
    uint32_t pattern_ = (pattern & 0xF) << 12;
    *PRE17P_REG_PERI = *PRE17P_REG_PERI | pattern_;
}

void gpio_disable_pos_wreq (uint8_t pattern) {
    uint32_t pattern_ = ~((pattern | 0x00000000) << 12);
    *PRE17P_REG_PERI = *PRE17P_REG_PERI & pattern_;
}

void gpio_enable_neg_wreq (uint8_t pattern) {
    uint32_t pattern_ = (pattern & 0xF) << 8;
    *PRE17P_REG_PERI = *PRE17P_REG_PERI | pattern_;
}

void gpio_disable_neg_wreq (uint8_t pattern) {
    uint32_t pattern_ = ~((pattern | 0x00000000) << 8);
    *PRE17P_REG_PERI = *PRE17P_REG_PERI & pattern_;
}

void gpio_set_dir_in (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    __gpio_dir__ = __gpio_dir__ & pattern_;
    *PRE17P_GPIO_DIR = __gpio_dir__;
}

void gpio_set_dir_out (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xFF;
    __gpio_dir__ = __gpio_dir__ | pattern_;
    *PRE17P_GPIO_DIR = __gpio_dir__;
}

void gpio_set_dir_inout (uint8_t in_pattern, uint8_t out_pattern) {
    uint32_t in_pattern_ = ~(in_pattern | 0x00000000);
    uint32_t out_pattern_ = out_pattern & 0xFF;
    __gpio_dir__ = __gpio_dir__ & in_pattern_;
    __gpio_dir__ = __gpio_dir__ | out_pattern_;
    *PRE17P_GPIO_DIR = __gpio_dir__;
}

void gpio_write_one (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xFF;
    __gpio_data__ = __gpio_data__ | pattern_;
    *PRE17P_GPIO_DATA = __gpio_data__;
}

void gpio_write_zero (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    __gpio_data__ = __gpio_data__ & pattern_;
    *PRE17P_GPIO_DATA = __gpio_data__;
}

void gpio_write_onezero (uint8_t one_pattern, uint8_t zero_pattern) {
    uint32_t one_pattern_  = one_pattern & 0xFF;
    uint32_t zero_pattern_ = ~(zero_pattern | 0x00000000);
    __gpio_data__ = __gpio_data__ & zero_pattern_;
    __gpio_data__ = __gpio_data__ | one_pattern_;
    *PRE17P_GPIO_DATA = __gpio_data__;
}

void gpio_write_direct (uint8_t data) {
    __gpio_data__ = data;
    *PRE17P_GPIO_DATA = __gpio_data__;
}

void gpio_write_mask (uint8_t mask, uint8_t data) {
    uint32_t mask_ = mask & 0xFF;
    __gpio_data__ = (__gpio_data__ & ~mask_) | (mask_ & data);
    *PRE17P_GPIO_DATA = __gpio_data__;
}

uint32_t gpio_read (void) {
// If DIR = OUT, use __gpio_data__
// If DIR = IN,  use *PRE17P_GPIO_DATA
    uint32_t dir_out = __gpio_dir__;
    return ((*PRE17P_GPIO_DATA & ~dir_out) | (__gpio_data__ & dir_out));
}

void gpio_enable_irq (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xFF;
    __gpio_irq_mask__ = __gpio_irq_mask__ | pattern_;
    *PRE17P_GPIO_IRQ_MASK = __gpio_irq_mask__;
}

void gpio_disable_irq (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    __gpio_irq_mask__ = __gpio_irq_mask__ & pattern_;
    *PRE17P_GPIO_IRQ_MASK = __gpio_irq_mask__;
}

void gpio_mbus_sleep_all (void) {
    gpio_freeze();
    *((volatile uint32_t *) 0xA0003010) = 0; // Sleep Message
    while(1);
}

//**************************************************
// COTS Power Switches
//**************************************************

void cps_on (uint8_t pattern) {
    uint32_t pattern_ = pattern & 0xF;
    *PRE17P_REG_CPS = *PRE17P_REG_CPS | pattern_;
}

void cps_off (uint8_t pattern) {
    uint32_t pattern_ = ~(pattern | 0x00000000);
    *PRE17P_REG_CPS = *PRE17P_REG_CPS & pattern_;
}

