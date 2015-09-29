//*******************************************************************
<<<<<<< HEAD
//Author: Yejoong Kim
//Description: PRCv12 header file
//*******************************************************************

#ifndef PRCV12_H
#define PRCV12_H

#define MSG0  0xA0000000
#define MSG1  0xA0000004
#define MSG2  0xA0000008
#define MSG3  0xA000000C
#define MSG0  0xA0000010
#define MSG1  0xA0000014
#define MSG2  0xA0000018
#define MSG3  0xA000001C
#define NUM_EXT_INT 14

#include <stdint.h>
#include <stdbool.h>
#include "PRCv12_RF.h"

// Interrupt Functions
void handler_ext_int_0(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));

// Clear All pending interrupts and Enable interrupts
// Some interrupts may be enabled/disabled by the interrupt masking signals from RF.
void init_interrupt(void);

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

#endif // PRCV12_H
=======
//Author: Gyouho Kim
//        Zhiyoong Foo
//		  Yejoong Kim
//Description: PRCv12 header file
//*******************************************************************

#ifndef M3_H
#define M3_H

#define MSG0  0xA0001000
#define MSG1  0xA0001004
#define MSG2  0xA0001008
#define MSG3  0xA000100C
#define IMSG0 0xA0001010
#define IMSG1 0xA0001014
#define IMSG2 0xA0001018
#define IMSG3 0xA000101C

#define IRQ10VEC 0x00000068
#define IRQ11VEC 0x0000006C

#include <stdint.h>
#include <stdbool.h>

// Possible reg values:
// Read-Only
//   4'b0000~4'b0011: (Non-INT) MSG REG 0~3
//   4'b0100~4'b0111: INT MSG REG 0~3
// R/W
//           4'b1000: CHIP_ID
//           4'b1001: DMA_INFO
//           4'b1010: GOC_CTRL
//           4'b1011: PMU_CTRL
int write_config_reg(uint8_t reg, uint32_t data);
void delay(unsigned ticks);
void WFI();
void config_timer( uint8_t timer_id, uint8_t go, uint8_t roi, uint32_t init_val, uint32_t sat_val);
void set_wakeup_timer( uint16_t timestamp, uint8_t on, uint8_t reset );
void set_clkfreq( uint8_t fastmode, uint8_t div_cm, uint8_t div_mbus, uint8_t ring );
void pmu_div5_ovrd( uint8_t ovrd );
void pmu_div6_ovrd( uint8_t ovrd );
void pmu_set_force_wake( uint8_t set );
void pon_reset( void );
void sleep_req_by_sw( void );
uint32_t generate_ECC( uint32_t data_in ); 

#endif // M3_H
>>>>>>> d33c1a0a94b8f9f6a48504f759d7a12dd8637ff5

