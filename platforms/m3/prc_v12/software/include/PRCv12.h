//*******************************************************************
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

