//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12 header file
//*******************************************************************

#ifndef PRCV12_H
#define PRCV12_H

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

//*********************************************************
// INCLUDES...
//*********************************************************
#include <stdint.h>
#include <stdbool.h>
#include "PRCv12_RF.h"


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

#endif // PRCV12_H

