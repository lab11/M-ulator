//************************************************************
// Desciption: PMUv9 Register File Header File
//      Generated by genRF (Version 1.03) 10/09/2017 00:49:21
//************************************************************

#ifndef PMUV9_RF_H
#define PMUV9_RF_H

// Register 0x00
// -- MEMORY-MAPPED --

// Register 0x01
// -- MEMORY-MAPPED --

// Register 0x02
// -- MEMORY-MAPPED --

// Register 0x03
// -- MEMORY-MAPPED --

// Register 0x04
// -- MEMORY-MAPPED --

// Register 0x05
// -- MEMORY-MAPPED --

// Register 0x06
// -- MEMORY-MAPPED --

// Register 0x07
// -- MEMORY-MAPPED --

// Register 0x08
// -- MEMORY-MAPPED --

// Register 0x09
// -- MEMORY-MAPPED --

// Register 0x0A
// -- MEMORY-MAPPED --

// Register 0x0B
// -- MEMORY-MAPPED --

// Register 0x0C
// -- MEMORY-MAPPED --

// Register 0x0D
// -- MEMORY-MAPPED --

// Register 0x0E
// -- MEMORY-MAPPED --

// Register 0x0F
// -- MEMORY-MAPPED --

// Register 0x10
// -- MEMORY-MAPPED --

// Register 0x11
// -- MEMORY-MAPPED --

// Register 0x12
// -- MEMORY-MAPPED --

// Register 0x13
// -- MEMORY-MAPPED --

// Register 0x14
// -- MEMORY-MAPPED --

// Register 0x15
// -- MEMORY-MAPPED --

// Register 0x16
// -- MEMORY-MAPPED --

// Register 0x17
// -- MEMORY-MAPPED --

// Register 0x18
// -- MEMORY-MAPPED --

// Register 0x19
// -- MEMORY-MAPPED --

// Register 0x1A
// -- MEMORY-MAPPED --

// Register 0x1B
// -- MEMORY-MAPPED --

// Register 0x1C
// -- MEMORY-MAPPED --

// Register 0x1D
// -- MEMORY-MAPPED --

// Register 0x1E
// -- MEMORY-MAPPED --

// Register 0x1F
// -- MEMORY-MAPPED --

// Register 0x20
// -- MEMORY-MAPPED --

// Register 0x21
// -- MEMORY-MAPPED --

// Register 0x22
// -- MEMORY-MAPPED --

// Register 0x23
// -- MEMORY-MAPPED --

// Register 0x24
// -- MEMORY-MAPPED --

// Register 0x25
// -- MEMORY-MAPPED --

// Register 0x26
// -- MEMORY-MAPPED --

// Register 0x27
// -- MEMORY-MAPPED --

// Register 0x28
// -- MEMORY-MAPPED --

// Register 0x29
// -- MEMORY-MAPPED --

// Register 0x2A
// -- MEMORY-MAPPED --

// Register 0x2B
// -- MEMORY-MAPPED --

// Register 0x2C
// -- MEMORY-MAPPED --

// Register 0x2D
// -- MEMORY-MAPPED --

// Register 0x2E
// -- MEMORY-MAPPED --

// Register 0x2F
// -- MEMORY-MAPPED --

// Register 0x30
// -- MEMORY-MAPPED --

// Register 0x31
// -- MEMORY-MAPPED --

// Register 0x32
// -- MEMORY-MAPPED --

// Register 0x33
// -- MEMORY-MAPPED --

// Register 0x34
// -- MEMORY-MAPPED --

// Register 0x35
// -- MEMORY-MAPPED --

// Register 0x36
// -- MEMORY-MAPPED --

// Register 0x37
// -- MEMORY-MAPPED --

// Register 0x38
// -- MEMORY-MAPPED --

// Register 0x39
// -- MEMORY-MAPPED --

// Register 0x3A
// -- MEMORY-MAPPED --

// Register 0x3B
// -- MEMORY-MAPPED --

// Register 0x3C
// -- MEMORY-MAPPED --

// Register 0x3D
// -- MEMORY-MAPPED --

// Register 0x3E
// -- MEMORY-MAPPED --

// Register 0x3F
// -- MEMORY-MAPPED --

// Register 0x40
// -- MEMORY-MAPPED --

// Register 0x41
// -- MEMORY-MAPPED --

// Register 0x42
// -- MEMORY-MAPPED --

// Register 0x43
// -- MEMORY-MAPPED --

// Register 0x44
// -- MEMORY-MAPPED --

// Register 0x45
// -- MEMORY-MAPPED --

// Register 0x46
// -- MEMORY-MAPPED --

// Register 0x47
// -- MEMORY-MAPPED --

// Register 0x48
// -- MEMORY-MAPPED --

// Register 0x49
// -- EMPTY --

// Register 0x4A
// -- EMPTY --

// Register 0x4B
// -- EMPTY --

// Register 0x4C
// -- EMPTY --

// Register 0x4D
// -- EMPTY --

// Register 0x4E
// -- EMPTY --

// Register 0x4F
// -- EMPTY --

// Register 0x50
// -- READ-ONLY --

// Register 0x51
typedef union pmuv9_r51{
  struct{
    unsigned LC_CLK_RING		: 2;
    unsigned LC_CLK_DIV		: 2;
    unsigned PMU_IRQ_EN		: 1;
    unsigned PMU_CHECK_WRITE		: 1;
    unsigned EN_GLITCH_FILT		: 1;
    unsigned MBUS_IGNORE_RX_FAIL		: 1;
  };
  uint32_t as_int;
} pmuv9_r51_t;
#define PMUv9_R51_DEFAULT {{0x1, 0x3, 0x1, 0x0, 0x0, 0x1}}
_Static_assert(sizeof(pmuv9_r51_t) == 4, "Punned Structure Size");

// Register 0x52
typedef union pmuv9_r52{
  struct{
    unsigned INT_RPLY_REG_ADDR		: 8;
    unsigned INT_RPLY_SHORT_ADDR		: 8;
  };
  uint32_t as_int;
} pmuv9_r52_t;
#define PMUv9_R52_DEFAULT {{0x00, 0x10}}
_Static_assert(sizeof(pmuv9_r52_t) == 4, "Punned Structure Size");

// Register 0x53
typedef union pmuv9_r53{
  struct{
    unsigned VCO_CNT_THRESHOLD		: 22;
    unsigned VCO_CNT_MODE		: 1;
    unsigned VCO_CNT_ENABLE		: 1;
  };
  uint32_t as_int;
} pmuv9_r53_t;
#define PMUv9_R53_DEFAULT {{0x000000, 0x0, 0x0}}
_Static_assert(sizeof(pmuv9_r53_t) == 4, "Punned Structure Size");

#endif // PMUV9_RF_H