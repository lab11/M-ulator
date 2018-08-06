//*******************************************************************************************
// PREv17 PERIPHERAL HEADER
//-------------------------------------------------------------------------------------------
// < DESCRIPTION >
//  - PREv17 Peripheral header file
//  - Includes definitions and functions for
//      GPIO, SPI, COTS Power Switches
//-------------------------------------------------------------------------------------------
// < LIST OF FUNCTIONS >
// void spi_enable_pad (void);
// void spi_disable_pad (void);
// void spi_freeze (void);
// void spi_unfreeze (void);
// void gpio_initialize (void)
// void gpio_enable_pad (uint8_t pattern);
// void gpio_disable_pad (uint8_t pattern);
// void gpio_freeze (void);
// void gpio_unfreeze (void);
// void gpio_enable_pos_wreq (uint8_t pattern);
// void gpio_disable_pos_wreq (uint8_t pattern);
// void gpio_enable_neg_wreq (uint8_t pattern);
// void gpio_disable_neg_wreq (uint8_t pattern);
// void gpio_set_dir_in (uint8_t pattern);
// void gpio_set_dir_out (uint8_t pattern);
// void gpio_set_dir_inout (uint8_t in_pattern, uint8_t out_pattern);
// void gpio_write_one (uint8_t pattern);
// void gpio_write_zero (uint8_t pattern);
// void gpio_write_onezero (uint8_t one_pattern, uint8_t zero_pattern);
// void gpio_write_direct (uint8_t data);
// void gpio_write_mask (uint8_t mask, uint8_t data);
// uint32_t gpio_read (void);
// void gpio_enable_irq (uint8_t pattern);
// void gpio_disable_irq (uint8_t pattern);
// void gpio_mbus_sleep_all (void);
// void cps_on (uint8_t pattern);
// void cps_off (uint8_t pattern);
//-------------------------------------------------------------------------------------------
// < UPDATE HISTORY >
//  Aug 02 2018 -   First Commit
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@umich.edu)
//******************************************************************************************* 

#ifndef PREV17_PERIPHERAL_H
#define PREV17_PERIPHERAL_H

//*********************************************************
// INCLUDES...
//*********************************************************
#include <stdint.h>
#include <stdbool.h>
#include "PREv17.h"

//*********************************************************
// MMIO Defines
//*********************************************************
#define PRE17P_REG_CPS          ((volatile uint32_t *) 0xA000005C)
#define PRE17P_REG_PERI         ((volatile uint32_t *) 0xA0000060)
#define PRE17P_GPIO_DATA        ((volatile uint32_t *) 0xA0005000) // Data to be set or read depending on DIR register
#define PRE17P_GPIO_DIR         ((volatile uint32_t *) 0xA0005400) // Controls direction of PAD enable (0=Output) (1=Input)
#define PRE17P_GPIO_IRQ_MASK    ((volatile uint32_t *) 0xA0005410) // (Active High) Controls whitch GPIO lines to monitor for interrupt

//*********************************************************
// FUNCTIONS
//*********************************************************

/**
 * @brief   Enable SPI Input/Output Pads
 * @param   none
 */
void spi_enable_pad (void);

/**
 * @brief   Disable SPI Input/Output Pads
 * @param   none
 */
void spi_disable_pad (void);

/**
 * @brief   Freeze SPI output
 * @param   none
 */
void spi_freeze (void);

/**
 * @brief   Un-Freeze SPI output
 * @param   none
 */
void spi_unfreeze (void);

/**
 * @brief   Initialize GPIO global variables
 * @param   none
 */
void gpio_initialize (void);

/**
 * @brief   Enable GPIO Pads
 * @param   pattern     If 1, the corresponding GPIO pad becomes enable.
 *                      If 0, it is ignored.
 * @example
 *          gpio_enable_pad (0x01);
 *              GPIO[0] pad becomes enable.
 *              GPIO[7:1] pads keep their previous state.
 */
void gpio_enable_pad (uint8_t pattern);

/**
 * @brief   Disable GPIO Pads
 * @param   pattern     If 1, the corresponding GPIO pad becomes disable.
 *                      If 0, it is ignored.
 * @example
 *          gpio_disable_pad (0x03);
 *              GPIO[1:0] pads become disable.
 *              GPIO[7:2] pads keep their previous state.
 */
void gpio_disable_pad (uint8_t pattern);

/**
 * @brief   Freeze GPIO output
 * @param   none
 */
void gpio_freeze (void);

/**
 * @brief   Un-Freeze GPIO output after synchronizing *GPIO_DATA and __gpio_data__
 * @param   none
 */
void gpio_unfreeze (void);

/**
 * @brief   Enable GPIO Posedge Wakeup Request
 * @param   pattern     If 1, the corresponding GPIO pad posedge wakeup request is enabled.
 *                      If 0, it is ignored.
 * @example
 *          gpio_enable_pos_wreq (0x01);
 *              GPIO[0] pad has a posedge wakeup request enabled.
 *              GPIO[7:1] pads keep their previous configuration.
 */
void gpio_enable_pos_wreq (uint8_t pattern);

/**
 * @brief   Disable GPIO Posedge Wakeup Request
 * @param   pattern     If 1, the corresponding GPIO pad posedge wakeup request is disabled.
 *                      If 0, it is ignored.
 * @example
 *          gpio_disable_pos_wreq (0x03);
 *              GPIO[1:0] pads have a posedge wakeup request disabled.
 *              GPIO[7:2] pads keep their previous configuration.
 */
void gpio_disable_pos_wreq (uint8_t pattern);

/**
 * @brief   Enable GPIO Negedge Wakeup Request
 * @param   pattern     If 1, the corresponding GPIO pad negedge wakeup request is enabled.
 *                      If 0, it is ignored.
 * @example
 *          gpio_enable_neg_wreq (0x01);
 *              GPIO[0] pad has a negedge wakeup request enabled.
 *              GPIO[7:1] pads keep their previous configuration.
 */
void gpio_enable_neg_wreq (uint8_t pattern);

/**
 * @brief   Disable GPIO Negedge Wakeup Request
 * @param   pattern     If 1, the corresponding GPIO pad negedge wakeup request is disabled.
 *                      If 0, it is ignored.
 * @example
 *          gpio_disable_neg_wreq (0x03);
 *              GPIO[1:0] pads have a negedge wakeup request disabled.
 *              GPIO[7:2] pads keep their previous configuration.
 */
void gpio_disable_neg_wreq (uint8_t pattern);

/**
 * @brief   Set direction of GPIO pads to "input"
 * @param   pattern     If 1, the corresponding GPIO pad direction is set to "input".
 *                      If 0, it is ignored.
 * @example
 *          gpio_set_dir_in (0x01);
 *              GPIO[0] pad direction is set to "input".
 *              GPIO[7:1] pads keep their previous direction setting.
 */
void gpio_set_dir_in (uint8_t pattern);

/**
 * @brief   Set direction of GPIO pads to "output"
 * @param   pattern     If 1, the corresponding GPIO pad direction is set to "output".
 *                      If 0, it is ignored.
 * @example
 *          gpio_set_dir_out (0x03);
 *              GPIO[1:0] pads direction is set to "output".
 *              GPIO[7:2] pads keep their previous direction setting.
 */
void gpio_set_dir_out (uint8_t pattern);

/**
 * @brief   Set direction of GPIO pads
 * @param   in_pattern  If 1, the corresponding GPIO pad direction is set to "input".
 *                      If 0, it is ignored.
 *          out_pattern If 1, the corresponding GPIO pad direction is set to "output".
 *                      If 0, it is ignored.
 * @note    If both bits in in_pattern and out_pattern are 1, out_pattern will be used.
 * @example
 *          gpio_set_dir_inout (0x03, 0x10);
 *              GPIO[1:0] pads direction is set to "input".
 *              GPIO[4] pads direction is set to "output".
 *              Other pads keep their previous direction setting.
 */
void gpio_set_dir_inout (uint8_t in_pattern, uint8_t out_pattern);

/**
 * @brief   Write "1" to selected GPIO pads
 * @param   pattern     If 1, write "1" to the corresponding GPIO pad.
 *                      If 0, it is ignored.
 * @note    If the corresponding GPIO pad direction is set to "input",
 *          the corresponding bit in "pattern" is ignored.
 * @example
 *          gpio_write_one (0x01);
 *              GPIO[0] will be written "1".
 *              GPIO[7:1] pads keep their previous values.
 */
void gpio_write_one (uint8_t pattern);

/**
 * @brief   Write "0" to selected GPIO pads
 * @param   pattern     If 1, write "0" to the corresponding GPIO pad.
 *                      If 0, it is ignored.
 * @note    If the corresponding GPIO pad direction is set to "input",
 *          the corresponding bit in "pattern" is ignored.
 * @example
 *          gpio_write_one (0x05);
 *              GPIO[2] and GPIO[0] will be written "0".
 *              Other GPIO pads keep their previous values.
 */
void gpio_write_zero (uint8_t pattern);

/**
 * @brief   Write to GPIO
 * @param   one_pattern     If 1, the corresponding GPIO pad is written "1"
 *                          If 0, it is ignored.
 *          zero_pattern    If 1, the corresponding GPIO pad is written "0"
 *                          If 0, it is ignored.
 * @note    If both bits in one_pattern and zero_pattern are 1, one_pattern will be used.
 * @example
 *          gpio_write_onezero (0x03, 0x10);
 *              GPIO[1:0] pads are written "1".
 *              GPIO[4] pad is written "0".
 *              Other pads keep their previous value.
 */
void gpio_write_onezero (uint8_t one_pattern, uint8_t zero_pattern);

/**
 * @brief   Directly write the given data in GPIO
 * @param   data        Data used to overwrite GPIO
 * @note    If the corresponding GPIO pad direction is set to "input",
 *          the corresponding bit in "data" is ignored.
 * @example
 *          gpio_write_direct (0xF0);
 *              GPIO[7:0] will be written 0xF0.
 */
void gpio_write_direct (uint8_t data);

/**
 * @brief   Directly write the given data in GPIO with mask
 * @param   mask        Masking Pattern
 *          data        Data used to overwrite GPIO
 * @note    If the corresponding GPIO pad direction is set to "input",
 *          the corresponding bit in "data" is ignored.
 * @example
 *          gpio_write_mask (0x01, 0xFF);
 *              GPIO[0] will be written to 1.
 *              GPIO[7:1] keep their previous value.
 */
void gpio_write_mask (uint8_t mask, uint8_t data);

/**
 * @brief   Return the current GPIO data
 * @param   none
 */
uint32_t gpio_read (void);

/**
 * @brief   Enable GPIO IRQ
 * @param   pattern     If 1, the corresponding GPIO IRQ becomes enable.
 *                      If 0, it is ignored.
 * @example
 *          gpio_enable_irq (0x01);
 *              GPIO[0] IRQ becomes enable.
 *              GPIO[7:1] IRQ keep their previous state.
 */
void gpio_enable_irq (uint8_t pattern);

/**
 * @brief   Disable GPIO IRQ
 * @param   pattern     If 1, the corresponding GPIO IRQ becomes disable.
 *                      If 0, it is ignored.
 * @example
 *          gpio_disable_irq (0x03);
 *              GPIO[1:0] IRQ become disable.
 *              GPIO[7:2] IRQ keep their previous state.
 */
void gpio_disable_irq (uint8_t pattern);

/**
 * @brief   Freeze GPIO then go to sleep (and stay there)
 * @param   none
 */
void gpio_mbus_sleep_all (void);

/**
 * @brief   Turn on CPS
 * @param   pattern     If 1, the corresponding CPS becomes on.
 *                      If 0, it is ignored.
 * @example
 *          cps_on (0x01);
 *              CPS[0] becomes on.
 *              CPS[2:1] keep their previous state.
 */
void cps_on (uint8_t pattern);

/**
 * @brief   Turn off CPS
 * @param   pattern     If 1, the corresponding CPS becomes off.
 *                      If 0, it is ignored.
 * @example
 *          cps_off (0x03);
 *              CPS[1:0] become off.
 *              CPS[2] keeps its previous state.
 */
void cps_off (uint8_t pattern);

#endif // PREV17_PERIPHERAL_H
