//************************************************************
// Desciption: PRCv20 Register File Header File
//      Generated by genRF (Version 1.30) 04/29/2019 12:55:52
//************************************************************

#ifndef PRCV20_RF_H
#define PRCV20_RF_H

// Register 0x00 [AMBA:0xA0000000]
typedef union prcv20_r00{
  struct{
    unsigned MBUS_R0		: 24;
  };
  uint32_t as_int;
} prcv20_r00_t;
#define PRCv20_R00_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r00_t) == 4, "Punned Structure Size");

// Register 0x01 [AMBA:0xA0000004]
typedef union prcv20_r01{
  struct{
    unsigned MBUS_R1		: 24;
  };
  uint32_t as_int;
} prcv20_r01_t;
#define PRCv20_R01_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r01_t) == 4, "Punned Structure Size");

// Register 0x02 [AMBA:0xA0000008]
typedef union prcv20_r02{
  struct{
    unsigned MBUS_R2		: 24;
  };
  uint32_t as_int;
} prcv20_r02_t;
#define PRCv20_R02_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r02_t) == 4, "Punned Structure Size");

// Register 0x03 [AMBA:0xA000000C]
typedef union prcv20_r03{
  struct{
    unsigned MBUS_R3		: 24;
  };
  uint32_t as_int;
} prcv20_r03_t;
#define PRCv20_R03_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r03_t) == 4, "Punned Structure Size");

// Register 0x04 [AMBA:0xA0000010]
typedef union prcv20_r04{
  struct{
    unsigned MBUS_R4		: 24;
  };
  uint32_t as_int;
} prcv20_r04_t;
#define PRCv20_R04_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r04_t) == 4, "Punned Structure Size");

// Register 0x05 [AMBA:0xA0000014]
typedef union prcv20_r05{
  struct{
    unsigned MBUS_R5		: 24;
  };
  uint32_t as_int;
} prcv20_r05_t;
#define PRCv20_R05_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r05_t) == 4, "Punned Structure Size");

// Register 0x06 [AMBA:0xA0000018]
typedef union prcv20_r06{
  struct{
    unsigned MBUS_R6		: 24;
  };
  uint32_t as_int;
} prcv20_r06_t;
#define PRCv20_R06_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r06_t) == 4, "Punned Structure Size");

// Register 0x07 [AMBA:0xA000001C]
typedef union prcv20_r07{
  struct{
    unsigned MBUS_R7		: 24;
  };
  uint32_t as_int;
} prcv20_r07_t;
#define PRCv20_R07_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r07_t) == 4, "Punned Structure Size");

// Register 0x08 [AMBA:0xA0000020]
typedef union prcv20_r08{
  struct{
    unsigned GOCEP_CHIP_ID		: 16;
  };
  uint32_t as_int;
} prcv20_r08_t;
#define PRCv20_R08_DEFAULT {{0x0000}}
_Static_assert(sizeof(prcv20_r08_t) == 4, "Punned Structure Size");

// Register 0x09 [AMBA:0xA0000024]
typedef union prcv20_r09{
  struct{
    unsigned MBUS_NUM_BITS_THRESHOLD		: 20;
  };
  uint32_t as_int;
} prcv20_r09_t;
#define PRCv20_R09_DEFAULT {{0x80008}}
_Static_assert(sizeof(prcv20_r09_t) == 4, "Punned Structure Size");

// Register 0x0A [AMBA:0xA0000028]
typedef union prcv20_r0A{
  struct{
    unsigned FLAGS		: 24;
  };
  uint32_t as_int;
} prcv20_r0A_t;
#define PRCv20_R0A_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r0A_t) == 4, "Punned Structure Size");

// Register 0x0B [AMBA:0xA000002C]
typedef union prcv20_r0B{
  struct{
    unsigned GOC_SEL		: 4;
    unsigned GOC_SEL_DLY		: 2;
    unsigned GOC_ONECLK_MODE		: 1;
    unsigned GOC_CLK_GEN_SEL_FREQ		: 3;
    unsigned GOC_CLK_GEN_SEL_DIV		: 2;
    unsigned NOT_DEFINED_11_12		: 1;
    unsigned CLK_GEN_RING		: 2;
    unsigned CLK_GEN_DIV_MBC		: 3;
    unsigned CLK_GEN_DIV_CORE		: 3;
    unsigned CLK_GEN_HIGH_FREQ		: 1;
  };
  uint32_t as_int;
} prcv20_r0B_t;
#define PRCv20_R0B_DEFAULT {{0x8, 0x0, 0x0, 0x7, 0x1, 0x0, 0x1, 0x2, 0x3, 0x0}}
_Static_assert(sizeof(prcv20_r0B_t) == 4, "Punned Structure Size");

// Register 0x0C [AMBA:0xA0000030]
// -- EMPTY --

// Register 0x0D [AMBA:0xA0000034]
// -- READ-ONLY --

// Register 0x0E [AMBA:0xA0000038]
typedef union prcv20_r0E{
  struct{
    unsigned GOCEP_TIMEOUT		: 24;
  };
  uint32_t as_int;
} prcv20_r0E_t;
#define PRCv20_R0E_DEFAULT {{0x1E8480}}
_Static_assert(sizeof(prcv20_r0E_t) == 4, "Punned Structure Size");

// Register 0x0F [AMBA:0xA000003C]
typedef union prcv20_r0F{
  struct{
    unsigned GOCEP_PASS		: 2;
    unsigned GOCEP_FAIL		: 1;
    unsigned GOCEP_FAIL_CHIP_ID		: 1;
    unsigned GOCEP_FAIL_HEADER_CHECKSUM		: 1;
    unsigned GOCEP_FAIL_MEM_CHECKSUM		: 1;
    unsigned GOCEP_FAIL_TIMEOUT		: 1;
    unsigned GOCEP_FAIL_PREMATURE		: 1;
    unsigned GOCEP_CPU_WAS_RUNNING		: 1;
    unsigned NOT_DEFINED_15_9		: 1;
    unsigned NOT_DEFINED_15_10		: 1;
    unsigned NOT_DEFINED_15_11		: 1;
    unsigned NOT_DEFINED_15_12		: 1;
    unsigned NOT_DEFINED_15_13		: 1;
    unsigned NOT_DEFINED_15_14		: 1;
    unsigned NOT_DEFINED_15_15		: 1;
    unsigned NOT_DEFINED_15_16		: 1;
    unsigned NOT_DEFINED_15_17		: 1;
    unsigned NOT_DEFINED_15_18		: 1;
    unsigned NOT_DEFINED_15_19		: 1;
    unsigned NOT_DEFINED_15_20		: 1;
    unsigned NOT_DEFINED_15_21		: 1;
    unsigned NOT_DEFINED_15_22		: 1;
    unsigned GOCEP_FREEZE_RUN_CPU		: 1;
  };
  uint32_t as_int;
} prcv20_r0F_t;
#define PRCv20_R0F_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
_Static_assert(sizeof(prcv20_r0F_t) == 4, "Punned Structure Size");

// Register 0x10 [AMBA:0xA0000040]
typedef union prcv20_r10{
  struct{
    unsigned RUN_CPU		: 1;
  };
  uint32_t as_int;
} prcv20_r10_t;
#define PRCv20_R10_DEFAULT {{0x0}}
_Static_assert(sizeof(prcv20_r10_t) == 4, "Punned Structure Size");

// Register 0x11 [AMBA:0xA0000044]
typedef union prcv20_r11{
  struct{
    unsigned WUP_CNT_SAT		: 22;
    unsigned NOT_DEFINED_17_22		: 1;
    unsigned WUP_WREQ_EN		: 1;
  };
  uint32_t as_int;
} prcv20_r11_t;
#define PRCv20_R11_DEFAULT {{0x000000, 0x0, 0x0}}
_Static_assert(sizeof(prcv20_r11_t) == 4, "Punned Structure Size");

// Register 0x12 [AMBA:0xA0000048]
// -- READ-ONLY --

// Register 0x13 [AMBA:0xA000004C]
// -- EMPTY --

// Register 0x14 [AMBA:0xA0000050]
// -- EMPTY --

// Register 0x15 [AMBA:0xA0000054]
// -- EMPTY --

// Register 0x16 [AMBA:0xA0000058]
// -- EMPTY --

// Register 0x17 [AMBA:0xA000005C]
// -- EMPTY --

// Register 0x18 [AMBA:0xA0000060]
// -- EMPTY --

// Register 0x19 [AMBA:0xA0000064]
// -- EMPTY --

// Register 0x1A [AMBA:0xA0000068]
// -- EMPTY --

// Register 0x1B [AMBA:0xA000006C]
typedef union prcv20_r1B{
  struct{
    unsigned WAKEUP_ON_PEND_REQ		: 4;
    unsigned ENABLE_SOFT_RESET		: 1;
    unsigned PUF_CHIP_ID_ISOLATE		: 1;
    unsigned PUF_CHIP_ID_SLEEP		: 1;
  };
  uint32_t as_int;
} prcv20_r1B_t;
#define PRCv20_R1B_DEFAULT {{0x0, 0x0, 0x1, 0x1}}
_Static_assert(sizeof(prcv20_r1B_t) == 4, "Punned Structure Size");

// Register 0x1C [AMBA:0xA0000070]
typedef union prcv20_r1C{
  struct{
    unsigned SRAM0_USE_INVERTER_SA		: 1;
    unsigned SRAM0_TUNE_DECODER_DLY		: 5;
    unsigned SRAM0_TUNE_WL_WIDTH		: 3;
    unsigned SRAM0_TUNE_ASO_DLY		: 5;
  };
  uint32_t as_int;
} prcv20_r1C_t;
#define PRCv20_R1C_DEFAULT {{0x0, 0x10, 0x2, 0x00}}
_Static_assert(sizeof(prcv20_r1C_t) == 4, "Punned Structure Size");

// Register 0x1D [AMBA:0xA0000074]
// -- EMPTY --

// Register 0x1E [AMBA:0xA0000078]
// -- READ-ONLY --

// Register 0x1F [AMBA:0xA000007C]
typedef union prcv20_r1F{
  struct{
    unsigned MBUS_WATCHDOG_THRESHOLD		: 24;
  };
  uint32_t as_int;
} prcv20_r1F_t;
#define PRCv20_R1F_DEFAULT {{0x16E360}}
_Static_assert(sizeof(prcv20_r1F_t) == 4, "Punned Structure Size");

// Register 0x20 [AMBA:0xA0000080]
// -- EMPTY --

// Register 0x21 [AMBA:0xA0000084]
// -- EMPTY --

// Register 0x22 [AMBA:0xA0000088]
// -- EMPTY --

// Register 0x23 [AMBA:0xA000008C]
// -- EMPTY --

// Register 0x24 [AMBA:0xA0000090]
// -- EMPTY --

// Register 0x25 [AMBA:0xA0000094]
// -- EMPTY --

// Register 0x26 [AMBA:0xA0000098]
// -- EMPTY --

// Register 0x27 [AMBA:0xA000009C]
// -- EMPTY --

// Register 0x28 [AMBA:0xA00000A0]
// -- EMPTY --

// Register 0x29 [AMBA:0xA00000A4]
typedef union prcv20_r29{
  struct{
    unsigned NOT_DEFINED_41_0		: 1;
    unsigned NOT_DEFINED_41_1		: 1;
    unsigned NOT_DEFINED_41_2		: 1;
    unsigned NOT_DEFINED_41_3		: 1;
    unsigned NOT_DEFINED_41_4		: 1;
    unsigned NOT_DEFINED_41_5		: 1;
    unsigned NOT_DEFINED_41_6		: 1;
    unsigned NOT_DEFINED_41_7		: 1;
    unsigned NOT_DEFINED_41_8		: 1;
    unsigned NOT_DEFINED_41_9		: 1;
    unsigned NOT_DEFINED_41_10		: 1;
    unsigned NOT_DEFINED_41_11		: 1;
    unsigned NOT_DEFINED_41_12		: 1;
    unsigned NOT_DEFINED_41_13		: 1;
    unsigned NOT_DEFINED_41_14		: 1;
    unsigned NOT_DEFINED_41_15		: 1;
    unsigned STR_WR_CH1_ALT_ADDR		: 8;
  };
  uint32_t as_int;
} prcv20_r29_t;
#define PRCv20_R29_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF0}}
_Static_assert(sizeof(prcv20_r29_t) == 4, "Punned Structure Size");

// Register 0x2A [AMBA:0xA00000A8]
typedef union prcv20_r2A{
  struct{
    unsigned NOT_DEFINED_42_0		: 1;
    unsigned NOT_DEFINED_42_1		: 1;
    unsigned NOT_DEFINED_42_2		: 1;
    unsigned NOT_DEFINED_42_3		: 1;
    unsigned NOT_DEFINED_42_4		: 1;
    unsigned NOT_DEFINED_42_5		: 1;
    unsigned NOT_DEFINED_42_6		: 1;
    unsigned NOT_DEFINED_42_7		: 1;
    unsigned NOT_DEFINED_42_8		: 1;
    unsigned NOT_DEFINED_42_9		: 1;
    unsigned NOT_DEFINED_42_10		: 1;
    unsigned NOT_DEFINED_42_11		: 1;
    unsigned NOT_DEFINED_42_12		: 1;
    unsigned NOT_DEFINED_42_13		: 1;
    unsigned NOT_DEFINED_42_14		: 1;
    unsigned NOT_DEFINED_42_15		: 1;
    unsigned STR_WR_CH1_ALT_REG_WR		: 8;
  };
  uint32_t as_int;
} prcv20_r2A_t;
#define PRCv20_R2A_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00}}
_Static_assert(sizeof(prcv20_r2A_t) == 4, "Punned Structure Size");

// Register 0x2B [AMBA:0xA00000AC]
typedef union prcv20_r2B{
  struct{
    unsigned STR_WR_CH1_BUF_LEN		: 11;
    unsigned NOT_DEFINED_43_11		: 1;
    unsigned NOT_DEFINED_43_12		: 1;
    unsigned NOT_DEFINED_43_13		: 1;
    unsigned NOT_DEFINED_43_14		: 1;
    unsigned NOT_DEFINED_43_15		: 1;
    unsigned NOT_DEFINED_43_16		: 1;
    unsigned NOT_DEFINED_43_17		: 1;
    unsigned NOT_DEFINED_43_18		: 1;
    unsigned NOT_DEFINED_43_19		: 1;
    unsigned NOT_DEFINED_43_20		: 1;
    unsigned STR_WR_CH1_DBLB		: 1;
    unsigned STR_WR_CH1_WRP		: 1;
    unsigned STR_WR_CH1_EN		: 1;
  };
  uint32_t as_int;
} prcv20_r2B_t;
#define PRCv20_R2B_DEFAULT {{0x7FF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1}}
_Static_assert(sizeof(prcv20_r2B_t) == 4, "Punned Structure Size");

// Register 0x2C [AMBA:0xA00000B0]
typedef union prcv20_r2C{
  struct{
    unsigned STR_WR_CH1_BUF_OFF		: 11;
  };
  uint32_t as_int;
} prcv20_r2C_t;
#define PRCv20_R2C_DEFAULT {{0x000}}
_Static_assert(sizeof(prcv20_r2C_t) == 4, "Punned Structure Size");

// Register 0x2D [AMBA:0xA00000B4]
typedef union prcv20_r2D{
  struct{
    unsigned NOT_DEFINED_45_0		: 1;
    unsigned NOT_DEFINED_45_1		: 1;
    unsigned NOT_DEFINED_45_2		: 1;
    unsigned NOT_DEFINED_45_3		: 1;
    unsigned NOT_DEFINED_45_4		: 1;
    unsigned NOT_DEFINED_45_5		: 1;
    unsigned NOT_DEFINED_45_6		: 1;
    unsigned NOT_DEFINED_45_7		: 1;
    unsigned NOT_DEFINED_45_8		: 1;
    unsigned NOT_DEFINED_45_9		: 1;
    unsigned NOT_DEFINED_45_10		: 1;
    unsigned NOT_DEFINED_45_11		: 1;
    unsigned NOT_DEFINED_45_12		: 1;
    unsigned NOT_DEFINED_45_13		: 1;
    unsigned NOT_DEFINED_45_14		: 1;
    unsigned NOT_DEFINED_45_15		: 1;
    unsigned STR_WR_CH0_ALT_ADDR		: 8;
  };
  uint32_t as_int;
} prcv20_r2D_t;
#define PRCv20_R2D_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF0}}
_Static_assert(sizeof(prcv20_r2D_t) == 4, "Punned Structure Size");

// Register 0x2E [AMBA:0xA00000B8]
typedef union prcv20_r2E{
  struct{
    unsigned NOT_DEFINED_46_0		: 1;
    unsigned NOT_DEFINED_46_1		: 1;
    unsigned NOT_DEFINED_46_2		: 1;
    unsigned NOT_DEFINED_46_3		: 1;
    unsigned NOT_DEFINED_46_4		: 1;
    unsigned NOT_DEFINED_46_5		: 1;
    unsigned NOT_DEFINED_46_6		: 1;
    unsigned NOT_DEFINED_46_7		: 1;
    unsigned NOT_DEFINED_46_8		: 1;
    unsigned NOT_DEFINED_46_9		: 1;
    unsigned NOT_DEFINED_46_10		: 1;
    unsigned NOT_DEFINED_46_11		: 1;
    unsigned NOT_DEFINED_46_12		: 1;
    unsigned NOT_DEFINED_46_13		: 1;
    unsigned NOT_DEFINED_46_14		: 1;
    unsigned NOT_DEFINED_46_15		: 1;
    unsigned STR_WR_CH0_ALT_REG_WR		: 8;
  };
  uint32_t as_int;
} prcv20_r2E_t;
#define PRCv20_R2E_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00}}
_Static_assert(sizeof(prcv20_r2E_t) == 4, "Punned Structure Size");

// Register 0x2F [AMBA:0xA00000BC]
typedef union prcv20_r2F{
  struct{
    unsigned STR_WR_CH0_BUF_LEN		: 11;
    unsigned NOT_DEFINED_47_11		: 1;
    unsigned NOT_DEFINED_47_12		: 1;
    unsigned NOT_DEFINED_47_13		: 1;
    unsigned NOT_DEFINED_47_14		: 1;
    unsigned NOT_DEFINED_47_15		: 1;
    unsigned NOT_DEFINED_47_16		: 1;
    unsigned NOT_DEFINED_47_17		: 1;
    unsigned NOT_DEFINED_47_18		: 1;
    unsigned NOT_DEFINED_47_19		: 1;
    unsigned NOT_DEFINED_47_20		: 1;
    unsigned STR_WR_CH0_DBLB		: 1;
    unsigned STR_WR_CH0_WRP		: 1;
    unsigned STR_WR_CH0_EN		: 1;
  };
  uint32_t as_int;
} prcv20_r2F_t;
#define PRCv20_R2F_DEFAULT {{0x7FF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1}}
_Static_assert(sizeof(prcv20_r2F_t) == 4, "Punned Structure Size");

// Register 0x30 [AMBA:0xA00000C0]
typedef union prcv20_r30{
  struct{
    unsigned STR_WR_CH0_BUF_OFF		: 11;
  };
  uint32_t as_int;
} prcv20_r30_t;
#define PRCv20_R30_DEFAULT {{0x000}}
_Static_assert(sizeof(prcv20_r30_t) == 4, "Punned Structure Size");

// Register 0x31 [AMBA:0xA00000C4]
// -- EMPTY --

// Register 0x32 [AMBA:0xA00000C8]
// -- EMPTY --

// Register 0x33 [AMBA:0xA00000CC]
typedef union prcv20_r33{
  struct{
    unsigned NOT_DEFINED_51_0		: 1;
    unsigned NOT_DEFINED_51_1		: 1;
    unsigned NOT_DEFINED_51_2		: 1;
    unsigned NOT_DEFINED_51_3		: 1;
    unsigned NOT_DEFINED_51_4		: 1;
    unsigned NOT_DEFINED_51_5		: 1;
    unsigned NOT_DEFINED_51_6		: 1;
    unsigned NOT_DEFINED_51_7		: 1;
    unsigned NOT_DEFINED_51_8		: 1;
    unsigned NOT_DEFINED_51_9		: 1;
    unsigned NOT_DEFINED_51_10		: 1;
    unsigned NOT_DEFINED_51_11		: 1;
    unsigned NOT_DEFINED_51_12		: 1;
    unsigned NOT_DEFINED_51_13		: 1;
    unsigned NOT_DEFINED_51_14		: 1;
    unsigned NOT_DEFINED_51_15		: 1;
    unsigned NOT_DEFINED_51_16		: 1;
    unsigned NOT_DEFINED_51_17		: 1;
    unsigned NOT_DEFINED_51_18		: 1;
    unsigned NOT_DEFINED_51_19		: 1;
    unsigned NOT_DEFINED_51_20		: 1;
    unsigned NOT_DEFINED_51_21		: 1;
    unsigned NOT_DEFINED_51_22		: 1;
    unsigned BLK_WR_EN		: 1;
  };
  uint32_t as_int;
} prcv20_r33_t;
#define PRCv20_R33_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1}}
_Static_assert(sizeof(prcv20_r33_t) == 4, "Punned Structure Size");

// Register 0x34 [AMBA:0xA00000D0]
// -- EMPTY --

// Register 0x35 [AMBA:0xA00000D4]
// -- EMPTY --

// Register 0x36 [AMBA:0xA00000D8]
// -- EMPTY --

// Register 0x37 [AMBA:0xA00000DC]
// -- EMPTY --

// Register 0x38 [AMBA:0xA00000E0]
// -- EMPTY --

// Register 0x39 [AMBA:0xA00000E4]
// -- EMPTY --

// Register 0x3A [AMBA:0xA00000E8]
// -- EMPTY --

// Register 0x3B [AMBA:0xA00000EC]
// -- EMPTY --

// Register 0x3C [AMBA:0xA00000F0]
// -- EMPTY --

// Register 0x3D [AMBA:0xA00000F4]
// -- EMPTY --

// Register 0x3E [AMBA:0xA00000F8]
// -- EMPTY --

// Register 0x3F [AMBA:0xA00000FC]
typedef union prcv20_r3F{
  struct{
    unsigned CHIP_ID		: 24;
  };
  uint32_t as_int;
} prcv20_r3F_t;
#define PRCv20_R3F_DEFAULT {{0x000000}}
_Static_assert(sizeof(prcv20_r3F_t) == 4, "Punned Structure Size");

// Register 0x40 [AMBA:0xA0000100]
typedef union prcv20_r40{
  struct{
    unsigned NOT_DEFINED_64_0		: 1;
    unsigned NOT_DEFINED_64_1		: 1;
    unsigned NOT_DEFINED_64_2		: 1;
    unsigned NOT_DEFINED_64_3		: 1;
    unsigned NOT_DEFINED_64_4		: 1;
    unsigned NOT_DEFINED_64_5		: 1;
    unsigned NOT_DEFINED_64_6		: 1;
    unsigned NOT_DEFINED_64_7		: 1;
    unsigned NOT_DEFINED_64_8		: 1;
    unsigned NOT_DEFINED_64_9		: 1;
    unsigned NOT_DEFINED_64_10		: 1;
    unsigned NOT_DEFINED_64_11		: 1;
    unsigned NOT_DEFINED_64_12		: 1;
    unsigned NOT_DEFINED_64_13		: 1;
    unsigned NOT_DEFINED_64_14		: 1;
    unsigned NOT_DEFINED_64_15		: 1;
    unsigned NOT_DEFINED_64_16		: 1;
    unsigned NOT_DEFINED_64_17		: 1;
    unsigned NOT_DEFINED_64_18		: 1;
    unsigned NOT_DEFINED_64_19		: 1;
    unsigned NOT_DEFINED_64_20		: 1;
    unsigned NOT_DEFINED_64_21		: 1;
    unsigned NOT_DEFINED_64_22		: 1;
    unsigned ACT_RST		: 1;
  };
  uint32_t as_int;
} prcv20_r40_t;
#define PRCv20_R40_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
_Static_assert(sizeof(prcv20_r40_t) == 4, "Punned Structure Size");

#endif // PRCV20_RF_H