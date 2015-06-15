//*******************************************************************
//Author: Yoonmyung Lee
//        Zhiyoong Foo
//Description: PRCv11 header file
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
void write_config_reg(uint8_t reg, uint32_t data);
int read_config_reg(uint8_t reg);
void delay(unsigned ticks);
void WFI();
void config_timer32(uint32_t cmp, uint8_t roi, uint32_t cnt);
void set_wakeup_timer( uint16_t timestamp, uint8_t on, uint8_t reset );
void set_clkfreq( uint8_t fastmode, uint8_t div_cm, uint8_t div_mbus, uint8_t ring );
void pon_reset( void );
void sleep_req_by_sw( void );


//Register 0x0
typedef union prcv11_r0{
  struct{
    unsigned val 	: 24;
  };
  uint32_t as_int;
} prcv11_r0_t;
#define PRCv11_R0_DEFAULT {0x000000}
//_Static_assert(sizeof(prcv11_r0_t) == 4, "Punned Structure Size");

//Register 0x1
typedef union prcv11_r1{
  struct{
    unsigned val 	: 24;
  };
  uint32_t as_int;
} prcv11_r1_t;
#define PRCv11_R1_DEFAULT {{0x000000}}
//_Static_assert(sizeof(prcv11_r1_t) == 4, "Punned Structure Size");

//Register 0x2
typedef union prcv11_r2{
  struct{
    unsigned val 	: 24;
  };
  uint32_t as_int;
} prcv11_r2_t;
#define PRCv11_R2_DEFAULT {{0x000000}}
//_Static_assert(sizeof(prcv11_r2_t) == 4, "Punned Structure Size");

//Register 0x3
typedef union prcv11_r3{
  struct{
    unsigned val 	: 24;
  };
  uint32_t as_int;
} prcv11_r3_t;
#define PRCv11_R3_DEFAULT {{0x000000}}
//_Static_assert(sizeof(prcv11_r3_t) == 4, "Punned Structure Size");

//Register 0x4
typedef union prcv11_r4{
  struct{
    unsigned val 	: 24;
  };
  uint32_t as_int;
} prcv11_r4_t;
#define PRCv11_R4_DEFAULT {{0x000000}}
//_Static_assert(sizeof(prcv11_r4_t) == 4, "Punned Structure Size");

//Register 0x5
typedef union prcv11_r5{
  struct{
    unsigned val 	: 24;
  };
  uint32_t as_int;
} prcv11_r5_t;
#define PRCv11_R5_DEFAULT {{0x000000}}
//_Static_assert(sizeof(prcv11_r5_t) == 4, "Punned Structure Size");

//Register 0x6
typedef union prcv11_r6{
  struct{
    unsigned val 	: 24;
  };
  uint32_t as_int;
} prcv11_r6_t;
#define PRCv11_R6_DEFAULT {{0x000000}}
//_Static_assert(sizeof(prcv11_r6_t) == 4, "Punned Structure Size");

//Register 0x7
typedef union prcv11_r7{
  struct{
    unsigned val 	: 24;
  };
  uint32_t as_int;
} prcv11_r7_t;
#define PRCv11_R7_DEFAULT {{0x000000}}
//_Static_assert(sizeof(prcv11_r7_t) == 4, "Punned Structure Size");

#endif // M3_H

