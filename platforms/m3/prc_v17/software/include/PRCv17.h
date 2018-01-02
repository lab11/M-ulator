//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv17 header file
//*******************************************************************

#ifndef PRCV17_H
#define PRCV17_H
#define PRCv17

//*********************************************************
// Boolean Constants
//*********************************************************
#define TRUE  1
#define FALSE 0

//*********************************************************
// GOC Data Address
//*********************************************************
#define GOC_DATA_IRQ ((volatile uint32_t *) 0x8C)

//*********************************************************
// M0 Interrupts
//---------------------------------------------------------
// NOTE: These values must be consistent with files below:
//       vectors.s, PRCv17.v, PRCv17_debug.v
//*********************************************************
#define IRQ_WAKEUP     0
#define IRQ_SOFT_RESET 1
#define IRQ_GOCEP      2
#define IRQ_TIMER32    3
#define IRQ_TIMER16    4
#define IRQ_MBUS_TX    5
#define IRQ_MBUS_RX    6
#define IRQ_MBUS_FWD   7
#define IRQ_REG0       8
#define IRQ_REG1       9
#define IRQ_REG2       10
#define IRQ_REG3       11
#define IRQ_REG4       12
#define IRQ_REG5       13
#define IRQ_REG6       14
#define IRQ_REG7       15
#define IRQ_MBUS_MEM   16

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
// ARMv6 Architecture System Control and ID Registers
//*********************************************************
#define SCID_ACTLR      ((volatile uint32_t *) 0xE000E008)  // The Auxiliary Control Register
#define SCID_CPUID      ((volatile uint32_t *) 0xE000ED00)  // CPUID Base Register
#define SCID_ICSR       ((volatile uint32_t *) 0xE000ED04)  // Interrupt Control State Register
#define SCID_VTOR       ((volatile uint32_t *) 0xE000ED08)  // Vector Table Offset Register
#define SCID_AIRCR      ((volatile uint32_t *) 0xE000ED0C)  // Application Interrupt and Reset Control Register
#define SCID_SCR        ((volatile uint32_t *) 0xE000ED10)  // Optional System Control Register
#define SCID_CCR        ((volatile uint32_t *) 0xE000ED14)  // Configuration and Control Register
#define SCID_SHPR2      ((volatile uint32_t *) 0xE000ED1C)  // System Handler Priority Register 2
#define SCID_SHPR3      ((volatile uint32_t *) 0xE000ED20)  // System Handler Priority Register 3
#define SCID_SHCSR      ((volatile uint32_t *) 0xE000ED24)  // System Handler Control and State Regsiter
#define SCID_DFSR       ((volatile uint32_t *) 0xE000ED30)  // Debug Fault Status Register

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
#define REG_FLAGS       ((volatile uint32_t *) 0xA0000028)
#define REG_CLKGEN_TUNE ((volatile uint32_t *) 0xA000002C)
#define REG_SRAM_TUNE   ((volatile uint32_t *) 0xA0000034)
#define REG_GOC_TIMEOUT ((volatile uint32_t *) 0xA0000038)
#define REG_GOCEP_FLAG  ((volatile uint32_t *) 0xA000003C)
#define REG_RUN_CPU     ((volatile uint32_t *) 0xA0000040)
#define REG_WUPT_CONFIG ((volatile uint32_t *) 0xA0000044)
#define REG_WUPT_VAL    ((volatile uint32_t *) 0xA0000048)
#define REG_SOFT_RESET  ((volatile uint32_t *) 0xA0000068)
#define REG_MBUS_FLAG   ((volatile uint32_t *) 0xA0000078)
#define REG_MBUS_WD     ((volatile uint32_t *) 0xA000007C)
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
#define TIMER16_CNT     ((volatile uint32_t *) 0xA0001010)
#define TIMER16_STAT    ((volatile uint32_t *) 0xA0001014)

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
#define WUPT_VAL        ((volatile uint32_t *) 0xA0001300)

//*********************************************************
// MBus Watchdog Timer
//*********************************************************
#define MBCWD_RESET_REQ ((volatile uint32_t *) 0xA0001500)
#define MBCWD_RESET     ((volatile uint32_t *) 0xA0001504)

//*********************************************************
// PRC/PREv17 System Registers
//*********************************************************
#define SREG_CONF_HALT      ((volatile uint32_t *) 0xA000A000)
#define SREG_MBUS_TX_IRQ    ((volatile uint32_t *) 0xA000A004)
#define SREG_WAKEUP_SOURCE  ((volatile uint32_t *) 0xA000A008)  // Read-Only

//*********************************************************
// PRC/PREv17 System Control
//*********************************************************
#define SCTR_REG_HALT_ADDR      ((volatile uint32_t *) 0xAFFFF000)
#define SCTR_REG_RESUME_ADDR    ((volatile uint32_t *) 0xAFFFF004)
#define SCTR_CMD_HALT_CPU       0xBAADF00D
#define SCTR_CMD_RESUME_CPU     0xCAFEF00D

//*********************************************************
// SREG_CONF_HALT Values
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
#define HALT_UNTIL_MBUS_TX  0x9
#define HALT_UNTIL_MBUS_RX  0xA
#define HALT_UNTIL_MBUS_FWD 0xB
#define HALT_UNTIL_MBUS_TRX 0xC
#define HALT_DISABLE        0xF

//*********************************************************
// INCLUDES...
//*********************************************************
#include <stdint.h>
#include <stdbool.h>
#include "PRCv17_RF.h"


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
 * @brief   Set flag
 *
 * @param   bit_idx     bit index into which the value is written
 *          value       flag value
 *
 * @usage   set_flag(0, 1);
 */
uint32_t set_flag(uint32_t bit_idx, uint32_t value);


/**
 * @brief   Get flag
 *
 * @param   bit_idx     bit index in Flag Register to read
 *
 * @usage   get_flag(0);
 */
uint8_t get_flag(uint32_t bit_idx);


/**
 * @brief   Set a new SREG_CONF_HALT value
 *
 * @param   new_config  4-bit SREG_CONF_HALT value
 *
 * @usage   set_halt_config(0x9);
 *          set_halt_config(HALT_UNTIL_MBUS_RX);
 */
void set_halt_config(uint8_t new_config);


/**
 * @brief   Return the current SREG_CONF_HALT value
 *
 * @param   N/A
 *
 * @usage   uint8_t current_halt_cpu_setting = get_current_halt_config();
 */
uint8_t get_current_halt_config(void);


/**
 * @brief   This configures SREG_CONF_HALT like below:
 *              SREG_CONF_HALT     = reg_id;
 *
 * @param   reg_id  8-bit Register interrupt masking pattern
 *
 * @usage   set_halt_until_reg(0xF0);
 */
void set_halt_until_reg(uint32_t reg_id);


/**
 * @brief   This configures SREG_CONF_HALT like below:
 *              SREG_CONF_HALT     = HALT_UNTIL_MEM_WR;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mem_wr();
 */
void set_halt_until_mem_wr(void);


/**
 * @brief   This configures SREG_CONF_HALT like below:
 *              SREG_CONF_HALT     = HALT_UNTIL_MBUS_RX;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_rx();
 */
void set_halt_until_mbus_rx(void);


/**
 * @brief   This configures SREG_CONF_HALT like below:
 *              SREG_CONF_HALT     = HALT_UNTIL_MBUS_TX;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_tx();
 */
void set_halt_until_mbus_tx(void);


/**
 * @brief   This configures SREG_CONF_HALT like below:
 *              SREG_CONF_HALT     = HALT_UNTIL_MBUS_TRX;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_trx();
 */
void set_halt_until_mbus_trx(void);


/**
 * @brief   This configures SREG_CONF_HALT like below:
 *              SREG_CONF_HALT     = HALT_UNTIL_MBUS_FWD;
 *
 * @param   N/A
 *
 * @usage   set_halt_until_mbus_fwd();
 */
void set_halt_until_mbus_fwd(void);


/**
 * @brief   This configures SREG_CONF_HALT like below:
 *              SREG_CONF_HALT     = HALT_DISABLE;
 *
 * @param   N/A
 *
 * @usage   set_halt_disable();
 */
void set_halt_disable(void);


/**
 * @brief   Immediately put CPU in halt. CPU resumes its operation when the event specifiedin SREG_CONF_HALT occurs.
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
 * @brief Disables the Watch-Dog timer
 *
 */
void disable_timerwd();

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
 *
 * @param   id          debug id (informational use only)
 *          code        debug code (informational use only)
 */
void arb_debug_reg (uint8_t id, uint32_t code);


#endif // PRCV17_H
