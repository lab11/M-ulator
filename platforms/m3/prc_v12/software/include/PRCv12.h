//*******************************************************************
//Author: Yejoong Kim
//Description: PRCv12 header file
//*******************************************************************

#ifndef PRCV12_H
#define PRCV12_H

#define TRUE  1
#define FALSE 0

//ARMv6 Architecture System Address
//NVIC Registers
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

//MMIO Addresses
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

#define TIMER32_GO      ((volatile uint32_t *) 0xA0001100)
#define TIMER32_CMP     ((volatile uint32_t *) 0xA0001104)
#define TIMER32_ROI     ((volatile uint32_t *) 0xA0001108)
#define TIMER32_CNT     ((volatile uint32_t *) 0xA000110C)
#define TIMER32_STAT    ((volatile uint32_t *) 0xA0001110)

#define TIMERWD_GO      ((volatile uint32_t *) 0xA0001200)
#define TIMERWD_CNT     ((volatile uint32_t *) 0xA0001204)

#define WUPT_RESET      ((volatile uint32_t *) 0xA0001300)

#define SYS_CTRL_REG_ADDR     ((volatile uint32_t *) 0xAFFFF000)
#define SYS_CTRL_CMD_HALT_CPU 0xCAFEF00D

//CONFIG_HALT_CPU
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

#include <stdint.h>
#include <stdbool.h>
#include "PRCv12_RF.h"

// MBUS IRQ Setting
void disable_all_mbus_irq(void);
uint8_t get_current_halt_config(void);
void set_halt_config(uint8_t new_config);
void set_mbus_irq_reg(uint8_t RF_WR, uint8_t MEM_WR, uint8_t MBUS_RX, uint8_t MBUS_TX, uint8_t MBUS_FWD, uint8_t OLD_MSG, uint8_t HALT_CONFIG);
void set_halt_until_reg(uint8_t reg_id);
void set_halt_until_mem_wr(void);
void set_halt_until_mbus_rx(void);
void set_halt_until_mbus_tx(void);
void set_halt_until_mbus_fwd(void);
void set_halt_disable(void);
void halt_cpu(void);

// Possible reg values:
//   4'b0000~4'b0111: MSG REG 0~8
void write_config_reg(uint8_t reg, uint32_t data);
int read_config_reg(uint8_t reg);
void delay(unsigned ticks);
void WFI();
void config_timer16(uint32_t cmp0, uint32_t cmp1, uint8_t irq_en, uint32_t cnt, uint32_t status);
void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt, uint32_t status);
void config_timerwd(uint32_t cnt);
void set_wakeup_timer( uint16_t timestamp, uint8_t irq_en, uint8_t reset );
void set_clkfreq( uint8_t fastmode, uint8_t div_core, uint8_t div_mbus, uint8_t ring );
void pon_reset( void ); // FIXME: Not verified yet.
void sleep_req_by_sw( void ); // FIXME: Not verified yet.
void arb_debug_reg (uint32_t code);

#endif // PRCV12_H

