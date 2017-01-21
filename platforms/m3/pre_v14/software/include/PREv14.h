//*******************************************************************
//Author: Yejoong Kim
//Description: PREv14 header file
//*******************************************************************

#ifndef PREV14_H
#define PREV14_H

//*********************************************************
// Boolean Constants
//*********************************************************
#define TRUE  1
#define FALSE 0

//*********************************************************
// Reserved IRQ Address
//*********************************************************
#define IRQ16VEC        ((volatile uint32_t *) 0x00000080)
#define IRQ17VEC        ((volatile uint32_t *) 0x00000084)

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
#define REG_MBUS_BITS   ((volatile uint32_t *) 0xA0000024)
#define REG_IRQ_CTRL    ((volatile uint32_t *) 0xA0000028)
#define REG_CLKGEN_TUNE ((volatile uint32_t *) 0xA000002C)
#define REG_XO_CONFIG   ((volatile uint32_t *) 0xA0000030)
#define REG_SRAM_TUNE   ((volatile uint32_t *) 0xA0000034)
#define REG_GOC_TIMEOUT ((volatile uint32_t *) 0xA0000038)
#define REG_GOCEP_FLAG  ((volatile uint32_t *) 0xA000003C)
#define REG_RUN_CPU     ((volatile uint32_t *) 0xA0000040)
#define REG_WUPT_CONFIG ((volatile uint32_t *) 0xA0000044)
#define REG_WUPT_VAL    ((volatile uint32_t *) 0xA0000048)
#define REG_XOT_CONFIG  ((volatile uint32_t *) 0xA000004C)
#define REG_XOT_CONFIGU ((volatile uint32_t *) 0xA0000050)
#define REG_XOT_VAL_L   ((volatile uint32_t *) 0xA0000054)
#define REG_XOT_VAL_U   ((volatile uint32_t *) 0xA0000058)
#define REG_CPS         ((volatile uint32_t *) 0xA000005C)
#define REG_SPI_GPIO    ((volatile uint32_t *) 0xA0000060)
#define REG_CLK_OUT_SEL ((volatile uint32_t *) 0xA0000064)
#define REG_XO_CAP_TUNE ((volatile uint32_t *) 0xA0000068)
#define REG_MBUS_WD     ((volatile uint32_t *) 0xA000007C)
#define REG_STR3_BUFL   ((volatile uint32_t *) 0xA0000084)
#define REG_STR3_BUFU   ((volatile uint32_t *) 0xA0000088)
#define REG_STR3_EN     ((volatile uint32_t *) 0xA000008C)
#define REG_STR3_OFF    ((volatile uint32_t *) 0xA0000090)
#define REG_STR2_BUFL   ((volatile uint32_t *) 0xA0000094)
#define REG_STR2_BUFU   ((volatile uint32_t *) 0xA0000098)
#define REG_STR2_EN     ((volatile uint32_t *) 0xA000009C)
#define REG_STR2_OFF    ((volatile uint32_t *) 0xA00000A0)
#define REG_STR1_BUFL   ((volatile uint32_t *) 0xA00000A4)
#define REG_STR1_BUFU   ((volatile uint32_t *) 0xA00000A8)
#define REG_STR1_EN     ((volatile uint32_t *) 0xA00000AC)
#define REG_STR1_OFF    ((volatile uint32_t *) 0xA00000B0)
#define REG_STR0_BUFL   ((volatile uint32_t *) 0xA00000B4)
#define REG_STR0_BUFU   ((volatile uint32_t *) 0xA00000B8)
#define REG_STR0_EN     ((volatile uint32_t *) 0xA00000BC)
#define REG_STR0_OFF    ((volatile uint32_t *) 0xA00000C0)
#define REG_BLK_WR      ((volatile uint32_t *) 0xA00000CC)

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
// XO Timer
//*********************************************************
#define XOT_RESET       ((volatile uint32_t *) 0xA0001400)
#define XOT_VALUE       ((volatile uint32_t *) 0xA0001404)

//*********************************************************
// MBus Watchdog Timer
//*********************************************************
#define MBCWD_RESET_REQ ((volatile uint32_t *) 0xA0001500)
#define MBCWD_RESET     ((volatile uint32_t *) 0xA0001504)

//*********************************************************
// SPI (0xA000_4000 - 0xA000_4FFF)
//*********************************************************
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

//*********************************************************
// GPIO (0xA000_5000 - 0xA000_5FFF)
//*********************************************************
#define GPIO_DATA       ((volatile uint32_t *) 0xA0005000) // Data to be set or read depending on DIR register
#define GPIO_DIR        ((volatile uint32_t *) 0xA0005400) // Controls direction of PAD enable (0=Output) (1=Input)
#define GPIO_IRQ_MASK   ((volatile uint32_t *) 0xA0005410) // (Active High) Controls whitch GPIO lines to monitor for interrupt

//*********************************************************
// PRC/PREv14 System Tasks
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

//*********************************************************
// INCLUDES...
//*********************************************************
#include <stdint.h>
#include <stdbool.h>
#include "PREv14_RF.h"


//*********************************************************
// FUNCTIONS
//*********************************************************


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
 * @brief   Enable all interrupts
 *
 * @param   N/A
 *
 * @usage   enable_all_irq();
 */
void enable_all_irq();


/**
 * @brief   Disable all interrupts
 *
 * @param   N/A
 *
 * @usage   disable_all_irq();
 */
void disable_all_irq();


/**
 * @brief   Clear all pending interrupts
 *
 * @param   N/A
 *
 * @usage   clear_all_pend_irq();
 */
void clear_all_pend_irq();


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
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = reg_id;
 *
 * @param   reg_id  8-bit Register interrupt masking pattern
 *
 * @usage   set_halt_until_reg(0xF0);
 */
void set_halt_until_reg(uint8_t reg_id);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MEM_WR;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mem_wr();
 */
void set_halt_until_mem_wr(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_RX;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_rx();
 */
void set_halt_until_mbus_rx(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_TX;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_tx();
 */
void set_halt_until_mbus_tx(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_UNTIL_MBUS_FWD;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_fwd();
 */
void set_halt_until_mbus_fwd(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              CONFIG_HALT_CPU     = HALT_DISABLE;
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
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              OLD_MSG_REG_MASK     = 1;
 *
 * @param   N/A
 *
 * @usage   enable_old_msg_irq();
 */
void enable_old_msg_irq(void);


/**
 * @brief   This configures Register0x0A (MMIO 0xA0000028) like below:
 *              OLD_MSG_REG_MASK     = 0;
 *
 * @param   N/A
 *
 * @usage   disable_old_msg_irq();
 */
void disable_old_msg_irq(void);


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
 * @brief   Enable/Disable SPI Input/Output
 *
 * @param   config  1-bit pattern. 
 *                  '1' enables SPI Input/Output pads
 *                  '0' disables SPI Input/Output pads
 */
void set_spi_pad (uint8_t config);


/**
 * @brief   Enable/Disable GPIO pads
 *
 * @param   config  8-bit pattern. 
 *                  '1' enables the corresponding GPIO pad
 *                  '0' disables the corresponding GPIO pad
 */
void set_gpio_pad (uint8_t config);


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

#endif // PREV14_H

