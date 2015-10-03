//*******************************************************************
//Author: Yejoong Kim
//Description: PREv12 header file
//*******************************************************************

#ifndef PREV12_H
#define PREV12_H

#define TRUE  1
#define FALSE 0

//*******************************************************************
// MMIO Addresses (Based on ARMv6-M)
//*******************************************************************
//Code/Data (0x0000_0000 - 0x0000_1FFC)
//Register File (0xA000_0000 - 0xA000_0FFF)
#define REG0            ((volatile uint32_t *) 0xA0000000)
#define REG1            ((volatile uint32_t *) 0xA0000004)
#define REG2            ((volatile uint32_t *) 0xA0000008)
#define REG3            ((volatile uint32_t *) 0xA000000C)
#define REG4            ((volatile uint32_t *) 0xA0000010)
#define REG5            ((volatile uint32_t *) 0xA0000014)
#define REG6            ((volatile uint32_t *) 0xA0000018)
#define REG7            ((volatile uint32_t *) 0xA000001C)
#define REG_IRQ_CTRL    ((volatile uint32_t *) 0xA0000028)
#define REG_CLKGEN_TUNE ((volatile uint32_t *) 0xA000002C)
#define REG_WUPT_CONFIG ((volatile uint32_t *) 0xA0000034)
#define REG_WUPT_VAL    ((volatile uint32_t *) 0xA0000038)
#define REG_GOCEP_FLAG  ((volatile uint32_t *) 0xA000003C)
#define REG_MBUS_FLAG   ((volatile uint32_t *) 0xA0000044)
#define REG_XO_CONFIG   ((volatile uint32_t *) 0xA0000094)
#define REG_XOT_CONFIG  ((volatile uint32_t *) 0xA0000098)
#define REG_XOT_VAL     ((volatile uint32_t *) 0xA000009C)
#define REG_CPS         ((volatile uint32_t *) 0xA00000A0)
#define REG_PAD_EN      ((volatile uint32_t *) 0xA00000A4)
//TIMER16 (0xA000_1000 - 0xA000_10FF)
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
//TIMER32 (0xA000_1100 - 0xA000_11FF)
#define TIMER32_GO      ((volatile uint32_t *) 0xA0001100)
#define TIMER32_CMP     ((volatile uint32_t *) 0xA0001104)
#define TIMER32_ROI     ((volatile uint32_t *) 0xA0001108)
#define TIMER32_CNT     ((volatile uint32_t *) 0xA000110C)
#define TIMER32_STAT    ((volatile uint32_t *) 0xA0001110)
//TIMERWD (0xA000_1200 - 0xA000_12FF)
#define TIMERWD_GO      ((volatile uint32_t *) 0xA0001200)
#define TIMERWD_CNT     ((volatile uint32_t *) 0xA0001204)
//WUP_TIMER (0xA000_1300 - 0xA000_13FF)
#define WUP_RESET       ((volatile uint32_t *) 0xA0001300)
//XOT_TIMER (0xA000_1400 - 0xA000_14FF)
#define XOT_RESET       ((volatile uint32_t *) 0xA0001400)
//MBUS (0xA000_2000 - 0xA000_2FFF) (See mbus.h)
//#define MBUS_CMD0       ((volatile uint32_t *) 0xA0002000)
//#define MBUS_CMD1       ((volatile uint32_t *) 0xA0002004)
//#define MBUS_CMD2       ((volatile uint32_t *) 0xA0002008)
//#define MBUS_FUID_LEN   ((volatile uint32_t *) 0xA000200C)
//QUICK MBUS (0xA000_3000 - 0xA000_3FFF)
#define MBUS32_ADDR     ((volatile uint32_t *) 0xA0003000) // [Usage] *(MBUS32_ADDR + (mbus_target_addr << 2)) = mbus_data
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
//M3 System Control (0xAFFF_F000 - 0xAFFF_FFFF)
#define SCREG_ADDR      ((volatile uint32_t *) 0xAFFFF000)
//NVIC Registers (0xE000_E100 - 0xE000_EFFF)
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

//*******************************************************************
// M3 System Control Commands
//*******************************************************************
#define SCCMD_HALT_CPU  0xCAFEF00D

//*******************************************************************
// CONFIG_HALT_CPU
//*******************************************************************
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

//*******************************************************************
// Other Dependencies
//*******************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "PREv12_RF.h"

//*******************************************************************
// Register File Declarations
//*******************************************************************
extern volatile prev12_r08_t prev12_r08; // CHIP ID
extern volatile prev12_r09_t prev12_r09; // MBUS THRESHOLD
extern volatile prev12_r0A_t prev12_r0A; // MBUS IRQ REGISTER
extern volatile prev12_r0B_t prev12_r0B; // CLK GEN TUNING
extern volatile prev12_r0C_t prev12_r0C; // SRAM TUNING
extern volatile prev12_r0D_t prev12_r0D; // WUP TIMER SETTING
extern volatile prev12_r10_t prev12_r10; // RUN CPU
extern volatile prev12_r25_t prev15_r26; // XO CLOCK SETTING
extern volatile prev12_r26_t prev12_r26; // XO TIMER SETTING
extern volatile prev12_r28_t prev12_r28; // COTS POWER SWITCH
extern volatile prev12_r29_t prev12_r29; // PAD ENABLE

//*******************************************************************
// IRQ and HALT Settings
//*******************************************************************
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

//*******************************************************************
// System Functions
//*******************************************************************
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

//**************************************************
// IO Pad and COTS Power Switch
//**************************************************
void enable_io_pad (void);
void disable_io_pad (void);
void set_cps (uint32_t cps_config);

//*******************************************************************
// GPIO
//*******************************************************************
void gpio_init (uint32_t dir);
void gpio_set_dir (uint32_t dir);
uint32_t gpio_get_dir (void);
uint32_t gpio_get_data (void);
void gpio_set_data (uint32_t data);
void gpio_write_data (uint32_t data);
void gpio_write_current_data (void);
void gpio_write_raw (uint32_t data);
void gpio_set_bit (uint32_t loc);
void gpio_kill_bit (uint32_t loc);
void gpio_set_2bits (uint32_t loc0, uint32_t loc1);
void gpio_set_irq_mask (uint32_t mask);
void gpio_close (void);

#endif // PREV12_H

