//*******************************************************************
//Author: Yejoong Kim
//Description: PREv13 header file
//*******************************************************************

#ifndef PREV13_H
#define PREV13_H

//*********************************************************
// Boolean Constants
//*********************************************************
#define TRUE  1
#define FALSE 0

//*********************************************************
// ARMv6 Architecture NVIC Registers
//*********************************************************
#define NVIC_ISER       ((volatile uint32_t *) 0xE000E100)  // Interrupt Set-Enable Register
#define NVIC_ICER       ((volatile uint32_t *) 0xE000E180)  // Interrupt Clear-Enable Register
#define NVIC_ISPR       ((volatile uint32_t *) 0xE000E200)  // Interrupt Set-Pending Register
#define NVIC_ICPR       ((volatile uint32_t *) 0xE000E280)  // Interrupt Clear-Pending Register
#define NVIC_IPR0       ((volatile uint32_t *) 0xE000E400)  // Interrupt Priority Register
#define NVIC_IPR1       ((volatile uint32_t *) 0xE000E404)  // Interrupt Priority Register
#define NVIC_IPR2       ((volatile uint32_t *) 0xE000E408)  // Interrupt Priority Register
#define NVIC_IPR3       ((volatile uint32_t *) 0xE000E40C)  // Interrupt Priority Register
#define NVIC_IPR4       ((volatile uint32_t *) 0xE000E410)  // Interrupt Priority Register
#define NVIC_IPR5       ((volatile uint32_t *) 0xE000E414)  // Interrupt Priority Register
#define NVIC_IPR6       ((volatile uint32_t *) 0xE000E418)  // Interrupt Priority Register
#define NVIC_IPR7       ((volatile uint32_t *) 0xE000E41C)  // Interrupt Priority Register

//*********************************************************
// Register File MMIO Addresses
//*********************************************************
#define REG0            ((volatile uint32_t *) 0xA0000000)
#define REG1            ((volatile uint32_t *) 0xA0000004)
#define REG2            ((volatile uint32_t *) 0xA0000008)
#define REG3            ((volatile uint32_t *) 0xA000000C)
#define REG4            ((volatile uint32_t *) 0xA0000010)
#define REG5            ((volatile uint32_t *) 0xA0000014)
#define REG6            ((volatile uint32_t *) 0xA0000018)
#define REG7            ((volatile uint32_t *) 0xA000001C)
#define REG_CHIP_ID     ((volatile uint32_t *) 0xA0000020)
#define REG_MBUS_THRES  ((volatile uint32_t *) 0xA0000024)
#define REG_IRQ_CTRL    ((volatile uint32_t *) 0xA0000028)
#define REG_CLKGEN_TUNE ((volatile uint32_t *) 0xA000002C)
#define REG_SRAM_TUNE   ((volatile uint32_t *) 0xA0000030)
#define REG_WUPT_CONFIG ((volatile uint32_t *) 0xA0000034)
#define REG_WUPT_VAL    ((volatile uint32_t *) 0xA0000038)
#define REG_GOCEP_FLAG  ((volatile uint32_t *) 0xA000003C)
#define REG_RUN_CPU     ((volatile uint32_t *) 0xA0000040)
#define REG_MBUS_FLAG   ((volatile uint32_t *) 0xA0000044)
#define REG_XO_CONFIG   ((volatile uint32_t *) 0xA0000094)
#define REG_XOT_CONFIG  ((volatile uint32_t *) 0xA0000098)
#define REG_XOT_VAL     ((volatile uint32_t *) 0xA000009C)
#define REG_CPS         ((volatile uint32_t *) 0xA00000A0)
#define REG_PAD_EN      ((volatile uint32_t *) 0xA00000A4)

//*********************************************************
// TIMER16 MMIO Addresses
//*********************************************************
#define TIMER16_GO      ((volatile uint32_t *) 0xA0001000)
#define TIMER16_CMP0    ((volatile uint32_t *) 0xA0001004)
#define TIMER16_CMP1    ((volatile uint32_t *) 0xA0001008)
#define TIMER16_IRQEN   ((volatile uint32_t *) 0xA000100C)
#define TIMER16_CAP0    ((volatile uint32_t *) 0xA0001010)
#define TIMER16_CAP1    ((volatile uint32_t *) 0xA0001014)
#define TIMER16_CAP2    ((volatile uint32_t *) 0xA0001018)
#define TIMER16_CAP3    ((volatile uint32_t *) 0xA000101C)
#define TIMER16_CNT     ((volatile uint32_t *) 0xA0001020)
#define TIMER16_STAT    ((volatile uint32_t *) 0xA0001024)

//*********************************************************
// TIMER32 MMIO Addresses
//*********************************************************
#define TIMER32_GO      ((volatile uint32_t *) 0xA0001100)
#define TIMER32_CMP     ((volatile uint32_t *) 0xA0001104)
#define TIMER32_ROI     ((volatile uint32_t *) 0xA0001108)
#define TIMER32_CNT     ((volatile uint32_t *) 0xA000110C)
#define TIMER32_STAT    ((volatile uint32_t *) 0xA0001110)

//*********************************************************
// TIMERWD (Watch-Dog Tiimer) MMIO Addresses
//*********************************************************
#define TIMERWD_GO      ((volatile uint32_t *) 0xA0001200)
#define TIMERWD_CNT     ((volatile uint32_t *) 0xA0001204)

//*********************************************************
// Wake-Up Timer Reset MMIO Addresses
//*********************************************************
#define WUPT_RESET      ((volatile uint32_t *) 0xA0001300)

//*********************************************************
// PRC/PREv12 System Tasks
//*********************************************************
#define SYS_CTRL_REG_ADDR     ((volatile uint32_t *) 0xAFFFF000)
#define SYS_CTRL_CMD_HALT_CPU 0xCAFEF00D

//*********************************************************
// CONFIG_HALT_CPU Values
//*********************************************************
#define HALT_UNTIL_REG0     0x0
#define HALT_UNTIL_REG1     0x1
#define HALT_UNTIL_REG2     0x2
#define HALT_UNTIL_REG3     0x3
#define HALT_UNTIL_REG4     0x4
#define HALT_UNTIL_REG5     0x5
#define HALT_UNTIL_REG6     0x6
#define HALT_UNTIL_REG7     0x7
#define HALT_UNTIL_MEM_WR   0x8
#define HALT_UNTIL_MBUS_RX  0x9
#define HALT_UNTIL_MBUS_TX  0xA
#define HALT_UNTIL_MBUS_FWD 0xB
#define HALT_DISABLE        0xF

//XOT_TIMER (0xA000_1400 - 0xA000_14FF)
#define XOT_RESET       ((volatile uint32_t *) 0xA0001400)
//SPI (0xA000_4000 - 0xA000_4FFF)
#define SPI_SSPCR0      ((volatile uint32_t *) 0xA0004000) // Control register 0 (Default: 16'h0000)
#define SPI_SSPCR1      ((volatile uint32_t *) 0xA0004004) // Control register 1 (Default: 4'h0)
#define SPI_SSPDR       ((volatile uint32_t *) 0xA0004008) // Data register (Default: 16'h0000)
#define SPI_SSPSR       ((volatile uint32_t *) 0xA000400C) // Status register (Default: 5'h03)
#define SPI_SSPCPSR     ((volatile uint32_t *) 0xA0004010) // Clock prescale register (Default: 8'h00)
#define SPI_SSPIMSC     ((volatile uint32_t *) 0xA0004014) // Interrupt mask set or clear register (Default: 4'h0)
#define SPI_SSPRIS      ((volatile uint32_t *) 0xA0004018) // Raw interrupt status register (Default: 4'h8)
#define SPI_SSPMIS      ((volatile uint32_t *) 0xA000401C) // Masked interrupt status register (Default: 4'h0)
#define SPI_SSPICR      ((volatile uint32_t *) 0xA0004020) // Interrupt clear register (Default: 4'h0)
#define SPI_SSPDMACR    ((volatile uint32_t *) 0xA0004024) // DMA control register (Default: 2'h0)
//GPIO (0xA000_5000 - 0xA000_5FFF)
#define GPIO_DATA       ((volatile uint32_t *) 0xA0005000) // Data to be set or read depending on DIR register
#define GPIO_DIR        ((volatile uint32_t *) 0xA0005400) // Controls direction of PAD enable (0=Output) (1=Input)
#define GPIO_IRQ_MASK   ((volatile uint32_t *) 0xA0005410) // (Active High) Controls whitch GPIO lines to monitor for interrupt

//*********************************************************
// INCLUDES...
//*********************************************************
#include <stdint.h>
#include <stdbool.h>
#include "PREv13_RF.h"


//*********************************************************
// FUNCTIONS
//*********************************************************

/**
 * @brief   Write into Always-On Register File
 *
 *          It could have been just a simple MMIO write. However, PRC/PREv12 have hold-violations
 *          due to slow level converter when CPU attemps to write into Register File. So you should use
 *          layer controller interrupt to do this.
 *
 * @param   reg_addr    32-bit MMIO Address of the register to be written
 * @param   data        24-bit data to be written. This is a 32-bit variable, so upper 8-bit is ignored.
 *
 * @usage   write_regfile ((volatile uint32_t *) 0xA0000000, 0x123456);
 *          write_regfile (REG0, 0x123456);
 */
void write_regfile (volatile uint32_t* reg_addr, uint32_t data);


/**
 * @brief   Insert 'nop's 
 *
 * @param   ticks       Num of 'nop's to be inserted. The actual idle time would be 2*ticks cycles due to the 'for' loop.
 *
 * @usage   delay(100);
 */
void delay(unsigned ticks);


/**
 * @brief   Wait for interrupt
 *
 * @param   N/A
 *
 * @usage   WFI();
 */
void WFI();


/**
 * @brief   Disable all MBus-related interrupts
 *
 *          This configures Register0x0A (MMIO 0xA0000028) like below:
 *              OLD_MSG_REG_MASK    = 0x1  // Backward compatibility enabled
 *              CONFIG_HALT_CPU     = 0xF  // CPU Halt Disabled
 *              MBUS_FWD_IRQ_MASK   = 0x0  // Disabled
 *              MBUS_TX_IRQ_MASK    = 0x0  // Disabled
 *              MBUS_RX_IRQ_MASK    = 0x0  // Disabled
 *              MEM_WR_IRQ_MASK     = 0x0  // Disabled
 *              RF_WR_IRQ_MASK      = 0x00 // Disabled
 *
 * @param   N/A
 *
 * @usage   disable_all_mbus_irq();
 */
void disable_all_mbus_irq(void);


/**
 * @brief   Set a new CONFIG_HALT_CPU value
 *
 * @param   new_config  4-bit CONFIG_HALT_CPU value
 *
 * @usage   set_halt_config(0x9);
 *          set_halt_config(HALT_UNTIL_MBUS_RX);
 */
void set_halt_config(uint8_t new_config);


/**
 * @brief   Return the current CONFIG_HALT_CPU value
 *
 * @param   N/A
 *
 * @usage   uint8_t current_halt_cpu_setting = get_current_halt_config();
 */
uint8_t get_current_halt_config(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) using the input parameters:
 *              OLD_MSG_REG_MASK    = OLD_MSG;
 *              CONFIG_HALT_CPU     = HALT_CONFIG;
 *              MBUS_FWD_IRQ_MASK   = MBUS_FWD;
 *              MBUS_TX_IRQ_MASK    = MBUS_TX;
 *              MBUS_RX_IRQ_MASK    = MBUS_RX;
 *              MEM_WR_IRQ_MASK     = MEM_WR;
 *              RF_WR_IRQ_MASK      = RF_WR;
 *
 * @param   See Above
 *
 * @usage   set_mbus_irq_reg(0xF0, 0x0, 0x1, 0x0, 0x0, 0x1, 0xF);
 */
void set_mbus_irq_reg(uint8_t RF_WR, uint8_t MEM_WR, uint8_t MBUS_RX, uint8_t MBUS_TX, uint8_t MBUS_FWD, uint8_t OLD_MSG, uint8_t HALT_CONFIG);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = reg_id;
 *          Other signals (OLD_MSG_REG_MASK, MBUS_FWD_IRQ_MASK, MBUS_TX_IRQ_MASK, MBUS_RX_IRQ_MASK, MEM_WR_IRQ_MASK, RF_WR_IRQ_MASK) remain unchanged
 *
 * @param   reg_id  8-bit Register interrupt masking pattern
 *
 * @usage   set_halt_until_reg(0xF0);
 */
void set_halt_until_reg(uint8_t reg_id);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MEM_WR;
 *          Other signals (OLD_MSG_REG_MASK, MBUS_FWD_IRQ_MASK, MBUS_TX_IRQ_MASK, MBUS_RX_IRQ_MASK, MEM_WR_IRQ_MASK, RF_WR_IRQ_MASK) remain unchanged
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mem_wr();
 */
void set_halt_until_mem_wr(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_RX;
 *          Other signals (OLD_MSG_REG_MASK, MBUS_FWD_IRQ_MASK, MBUS_TX_IRQ_MASK, MBUS_RX_IRQ_MASK, MEM_WR_IRQ_MASK, RF_WR_IRQ_MASK) remain unchanged
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_rx();
 */
void set_halt_until_mbus_rx(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_TX;
 *          Other signals (OLD_MSG_REG_MASK, MBUS_FWD_IRQ_MASK, MBUS_TX_IRQ_MASK, MBUS_RX_IRQ_MASK, MEM_WR_IRQ_MASK, RF_WR_IRQ_MASK) remain unchanged
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_tx();
 */
void set_halt_until_mbus_tx(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_FWD;
 *          Other signals (OLD_MSG_REG_MASK, MBUS_FWD_IRQ_MASK, MBUS_TX_IRQ_MASK, MBUS_RX_IRQ_MASK, MEM_WR_IRQ_MASK, RF_WR_IRQ_MASK) remain unchanged
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_fwd();
 */
void set_halt_until_mbus_fwd(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_DISABLE;
 *          Other signals (OLD_MSG_REG_MASK, MBUS_FWD_IRQ_MASK, MBUS_TX_IRQ_MASK, MBUS_RX_IRQ_MASK, MEM_WR_IRQ_MASK, RF_WR_IRQ_MASK) remain unchanged
 *
 * @param   N/A
 *
 * @usage   set_halt_disable();
 */
void set_halt_disable(void);


/**
 * @brief   Immediately put CPU in halt. CPU resumes its operation when the event specifiedin CONFIG_HALT_CPU occurs.
 *
 * @param   N/A
 *
 * @usage   halt_cpu();
 */
void halt_cpu(void);


/**
 * @brief   Configure the 16-bit timer
 *
 * @param   cmp0    Number0 to compare
 * @param   cmp1    Number1 to compare
 * @param   irq_en  Interrupt Enable
 * @param   cnt     Manually set the timer counter
 * @param   status  Manually set the timer status
 */
void config_timer16(uint32_t cmp0, uint32_t cmp1, uint8_t irq_en, uint32_t cnt, uint32_t status);


/**
 * @brief   Configure the 32-bit timer
 *
 * @param   cmp     Number0 to compare
 * @param   roi     Reset on Interrupt
 * @param   cnt     Manually set the timer counter
 * @param   status  Manually set the timer status
 */
void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt, uint32_t status);

/**
 * @brief   Configure the Watch-Dog timer
 *
 * @param   cnt     Manually set the timer counter
 */
void config_timerwd(uint32_t cnt);


/**
 * @brief   Configure the wake-up timer
 *
 * @param   timestamp   Counter value at which the timer generates an interrupt
 * @param   irq_en      Interrupt Enable
 * @param   reset       1 will reset the wake-up timer
 */
void set_wakeup_timer( uint16_t timestamp, uint8_t irq_en, uint8_t reset );


/**
 * @brief   Write into ARB debug register
 *          !!!    THIS IS FOR VERILOG SIM ONLY    !!!
 *          !!!IT WILL NOT WORK ON THE SILICON CHIP!!!
 *
 * @param   code        data to be written to the debug register
 */
void arb_debug_reg (uint32_t code);

//**************************************************
// IO Pad and COTS Power Switch
//**************************************************

/**
 * @brief   Enable I/O pads
 */
void enable_io_pad (void);


/**
 * @brief   Disable I/O pads
 */
void disable_io_pad (void);


/**
 * @brief   Enable/Disable COTS switches
 *
 * @param   cps_config  3-bit pattern. 
 *                      '1' enables the corresponding COTS switch.
 *                      '0' disables the corresponding COTS switch.
 */
void set_cps (uint32_t cps_config);


//*******************************************************************
// GPIO
//*******************************************************************

/**
 * @brief   Initialize GPIO
 *
 * @param   dir     8-bit pattern setting GPIO directions.
 *                  '1' sets the corresponding GPIO bit as output
 *                  '0' sets the corresponding GPIO bit as input
 */
void gpio_init (uint32_t dir);


/**
 * @brief   Set GPIO directions
 *
 * @param   dir     8-bit pattern setting GPIO directions.
 *                  '1' sets the corresponding GPIO bit as output
 *                  '0' sets the corresponding GPIO bit as input
 */
void gpio_set_dir (uint32_t dir);


/**
 * @brief   Return the current GPIO directions
 *          The return value is a 8-bit pattern indicating GPIO directions.
 *              '1' means the corresponding GPIO bit is output.
 *              '0' means the corresponding GPIO bit is input.
 */
uint32_t gpio_get_dir (void);


/**
 * @brief   Return the current GPIO data
 *          The return value is a 8-bit GPIO data
 */
uint32_t gpio_get_data (void);


/**
 * @brief   Set GPIO data
 *          It does NOT change GPIO output. It just sets the internal variable 'gpio_data_'.
 *          In order to change the GPIO output, use this function, and then use gpio_write_current_data()
 *          If you want to change gpio_data_ as well as the actual GPIO output, then use gpio_write_data()
 *
 * @param   data    8-bit data
 */
void gpio_set_data (uint32_t data);


/**
 * @brief   Change GPIO output using gpio_data_
 *          Make sure gpio_data_ has the correct value. You may need to use gpio_set_data().
 */
void gpio_write_current_data (void);



/**
 * @brief   Change GPIO output
 *          This function changes the GPIO output as well as gpio_data_.
 *
 * @param   data    8-bit data
 */
void gpio_write_data (uint32_t data);


/**
 * @brief   Change GPIO output
 *          This function changes the GPIO output, but does NOT update gpio_data_.
 *
 * @param   data    8-bit data
 */
void gpio_write_raw (uint32_t data);


/**
 * @brief   Set the corresponding GPIO bit to 1
 *
 * @param   loc     GPIO pin location.
 */
void gpio_set_bit (uint32_t loc);


/**
 * @brief   Set the corresponding GPIO bit to 0
 *
 * @param   loc     GPIO pin location.
 */
void gpio_kill_bit (uint32_t loc);


/**
 * @brief   Set the corresponding GPIO bits to 1. You are setting the 2 bits at the same time.
 *
 * @param   loc0    GPIO pin location.
 * @param   loc1    GPIO pin location.
 */
void gpio_set_2bits (uint32_t loc0, uint32_t loc1);


/**
 * @brief   Set GPIO Interrupt mask
 *          '1' means the corresponding bit's IRQ is enabled.
 *          '0' means the corresponding bit's IRQ is disabled.
 *
 * @param   mask    8-bit mask pattern
 */
void gpio_set_irq_mask (uint32_t mask);


/**
 * @brief   Terminate GPIO operations.
 *          Internally, it is same as disable_io_pad()
 */
void gpio_close (void);

#endif // PREV13_H

