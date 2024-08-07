//************************************************************
// Desciption: FLPv4L Register File Header File
//      Generated by genRF (Version 1.42) 10/09/2020 00:33:32
//************************************************************

#ifndef FLPV4L_RF_H
#define FLPV4L_RF_H

// Register 0x00
typedef union flpv4l_r00{
  struct{
    unsigned T10us		: 7;
    unsigned T5us		: 6;
    unsigned T3us		: 6;
    unsigned Tcyc_read		: 5;
  };
  uint32_t as_int;
} flpv4l_r00_t;
#define FLPv4L_R00_DEFAULT {{0x09, 0x04, 0x02, 0x1F}}
#define FLPv4L_R00_DEFAULT_AS_INT 0xF84209
_Static_assert(sizeof(flpv4l_r00_t) == 4, "Punned Structure Size");

// Register 0x01
typedef union flpv4l_r01{
  struct{
    unsigned Tprog		: 8;
    unsigned Tcyc_prog		: 16;
  };
  uint32_t as_int;
} flpv4l_r01_t;
#define FLPv4L_R01_DEFAULT {{0x09, 0x007F}}
#define FLPv4L_R01_DEFAULT_AS_INT 0x007F09
_Static_assert(sizeof(flpv4l_r01_t) == 4, "Punned Structure Size");

// Register 0x02
typedef union flpv4l_r02{
  struct{
    unsigned Terase		: 16;
  };
  uint32_t as_int;
} flpv4l_r02_t;
#define FLPv4L_R02_DEFAULT {{0x0100}}
#define FLPv4L_R02_DEFAULT_AS_INT 0x000100
_Static_assert(sizeof(flpv4l_r02_t) == 4, "Punned Structure Size");

// Register 0x03
typedef union flpv4l_r03{
  struct{
    unsigned Tben		: 10;
    unsigned Thvcp_en		: 14;
  };
  uint32_t as_int;
} flpv4l_r03_t;
#define FLPv4L_R03_DEFAULT {{0x031, 0x03E8}}
#define FLPv4L_R03_DEFAULT_AS_INT 0x0FA031
_Static_assert(sizeof(flpv4l_r03_t) == 4, "Punned Structure Size");

// Register 0x04
typedef union flpv4l_r04{
  struct{
    unsigned Tsc_en		: 12;
    unsigned Tmvcp_en		: 12;
  };
  uint32_t as_int;
} flpv4l_r04_t;
#define FLPv4L_R04_DEFAULT {{0x3E8, 0x3E8}}
#define FLPv4L_R04_DEFAULT_AS_INT 0x3E83E8
_Static_assert(sizeof(flpv4l_r04_t) == 4, "Punned Structure Size");

// Register 0x05
typedef union flpv4l_r05{
  struct{
    unsigned Tcap		: 20;
  };
  uint32_t as_int;
} flpv4l_r05_t;
#define FLPv4L_R05_DEFAULT {{0x007CF}}
#define FLPv4L_R05_DEFAULT_AS_INT 0x0007CF
_Static_assert(sizeof(flpv4l_r05_t) == 4, "Punned Structure Size");

// Register 0x06
typedef union flpv4l_r06{
  struct{
    unsigned Tvref		: 17;
  };
  uint32_t as_int;
} flpv4l_r06_t;
#define FLPv4L_R06_DEFAULT {{0x01F3F}}
#define FLPv4L_R06_DEFAULT_AS_INT 0x001F3F
_Static_assert(sizeof(flpv4l_r06_t) == 4, "Punned Structure Size");

// Register 0x07
typedef union flpv4l_r07{
  struct{
    unsigned SRAM_START_ADDR		: 13;
  };
  uint32_t as_int;
} flpv4l_r07_t;
#define FLPv4L_R07_DEFAULT {{0x0000}}
#define FLPv4L_R07_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r07_t) == 4, "Punned Structure Size");

// Register 0x08
typedef union flpv4l_r08{
  struct{
    unsigned FLSH_START_ADDR		: 18;
  };
  uint32_t as_int;
} flpv4l_r08_t;
#define FLPv4L_R08_DEFAULT {{0x00000}}
#define FLPv4L_R08_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r08_t) == 4, "Punned Structure Size");

// Register 0x09
typedef union flpv4l_r09{
  struct{
    unsigned GO		: 1;
    unsigned CMD		: 4;
    unsigned IRQ_EN		: 1;
    unsigned LENGTH		: 13;
  };
  uint32_t as_int;
} flpv4l_r09_t;
#define FLPv4L_R09_DEFAULT {{0x0, 0x0, 0x0, 0x0000}}
#define FLPv4L_R09_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r09_t) == 4, "Punned Structure Size");

// Register 0x0A
typedef union flpv4l_r0A{
  struct{
    unsigned NOT_DEFINED_10_0		: 1;
    unsigned COMP_ISOL		: 1;
    unsigned COMP_CLKENB		: 1;
    unsigned COMP_SLEEP		: 1;
    unsigned VREF_SLEEP		: 1;
  };
  uint32_t as_int;
} flpv4l_r0A_t;
#define FLPv4L_R0A_DEFAULT {{0x0, 0x1, 0x1, 0x1, 0x1}}
#define FLPv4L_R0A_DEFAULT_AS_INT 0x00001E
_Static_assert(sizeof(flpv4l_r0A_t) == 4, "Punned Structure Size");

// Register 0x0B
// -- EMPTY --

// Register 0x0C
typedef union flpv4l_r0C{
  struct{
    unsigned BIT_EN_EXT		: 2;
    unsigned UPDATE_ADDR_EXT		: 1;
    unsigned WRAP_EXT		: 1;
  };
  uint32_t as_int;
} flpv4l_r0C_t;
#define FLPv4L_R0C_DEFAULT {{0x1, 0x0, 0x0}}
#define FLPv4L_R0C_DEFAULT_AS_INT 0x000001
_Static_assert(sizeof(flpv4l_r0C_t) == 4, "Punned Structure Size");

// Register 0x0D
typedef union flpv4l_r0D{
  struct{
    unsigned TIMEOUT_EXT		: 20;
  };
  uint32_t as_int;
} flpv4l_r0D_t;
#define FLPv4L_R0D_DEFAULT {{0xFFFFF}}
#define FLPv4L_R0D_DEFAULT_AS_INT 0x0FFFFF
_Static_assert(sizeof(flpv4l_r0D_t) == 4, "Punned Structure Size");

// Register 0x0E
typedef union flpv4l_r0E{
  struct{
    unsigned SRAM_START_ADDR_EXT		: 13;
  };
  uint32_t as_int;
} flpv4l_r0E_t;
#define FLPv4L_R0E_DEFAULT {{0x0000}}
#define FLPv4L_R0E_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r0E_t) == 4, "Punned Structure Size");

// Register 0x0F
typedef union flpv4l_r0F{
  struct{
    unsigned INT_RPLY_REG_ADDR		: 8;
    unsigned INT_RPLY_SHORT_ADDR		: 8;
  };
  uint32_t as_int;
} flpv4l_r0F_t;
#define FLPv4L_R0F_DEFAULT {{0x00, 0x10}}
#define FLPv4L_R0F_DEFAULT_AS_INT 0x001000
_Static_assert(sizeof(flpv4l_r0F_t) == 4, "Punned Structure Size");

// Register 0x10
typedef union flpv4l_r10{
  struct{
    unsigned BOOT_REG_PATTERN		: 2;
    unsigned NOT_DEFINED_16_2		: 1;
    unsigned NOT_DEFINED_16_3		: 1;
    unsigned NOT_DEFINED_16_4		: 1;
    unsigned NOT_DEFINED_16_5		: 1;
    unsigned NOT_DEFINED_16_6		: 1;
    unsigned NOT_DEFINED_16_7		: 1;
    unsigned NOT_DEFINED_16_8		: 1;
    unsigned NOT_DEFINED_16_9		: 1;
    unsigned NOT_DEFINED_16_10		: 1;
    unsigned NOT_DEFINED_16_11		: 1;
    unsigned NOT_DEFINED_16_12		: 1;
    unsigned NOT_DEFINED_16_13		: 1;
    unsigned NOT_DEFINED_16_14		: 1;
    unsigned NOT_DEFINED_16_15		: 1;
    unsigned BOOT_FLAG_SUCCESS		: 1;
    unsigned BOOT_FLAG_CHKSUM_ERROR		: 1;
    unsigned BOOT_FLAG_INVALID_CMND		: 1;
    unsigned BOOT_FLAG_PWDN		: 1;
    unsigned BOOT_FLAG_WRONG_HEADER		: 1;
    unsigned BOOT_FLAG_ECC_ERROR		: 1;
    unsigned BOOT_FLAG_SLEEP		: 1;
  };
  uint32_t as_int;
} flpv4l_r10_t;
#define FLPv4L_R10_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
#define FLPv4L_R10_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r10_t) == 4, "Punned Structure Size");

// Register 0x11
typedef union flpv4l_r11{
  struct{
    unsigned FLASH_POWER_GO		: 1;
    unsigned FLASH_POWER_SEL_ON		: 1;
    unsigned FLASH_POWER_IRQ_EN		: 1;
    unsigned FLASH_POWER_DO_FLSH		: 1;
    unsigned NOT_DEFINED_17_4		: 1;
    unsigned FLASH_POWER_DO_VREFCOMP		: 1;
  };
  uint32_t as_int;
} flpv4l_r11_t;
#define FLPv4L_R11_DEFAULT {{0x0, 0x1, 0x1, 0x1, 0x0, 0x1}}
#define FLPv4L_R11_DEFAULT_AS_INT 0x00002E
_Static_assert(sizeof(flpv4l_r11_t) == 4, "Punned Structure Size");

// Register 0x12
typedef union flpv4l_r12{
  struct{
    unsigned FLASH_AUTO_ON		: 1;
    unsigned FLASH_AUTO_OFF		: 1;
    unsigned FLASH_AUTO_USE_CUSTOM		: 1;
    unsigned SEL_PWR_ON_WUP		: 3;
    unsigned IRQ_PWR_ON_WUP		: 1;
  };
  uint32_t as_int;
} flpv4l_r12_t;
#define FLPv4L_R12_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0}}
#define FLPv4L_R12_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r12_t) == 4, "Punned Structure Size");

// Register 0x13
typedef union flpv4l_r13{
  struct{
    unsigned PP_STR_EN		: 1;
    unsigned PP_STR_LIMIT		: 19;
  };
  uint32_t as_int;
} flpv4l_r13_t;
#define FLPv4L_R13_DEFAULT {{0x0, 0x00000}}
#define FLPv4L_R13_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r13_t) == 4, "Punned Structure Size");

// Register 0x14
typedef union flpv4l_r14{
  struct{
    unsigned PP_BIT_EN_EXT		: 2;
    unsigned PP_WRAP		: 1;
    unsigned PP_USE_FAST_PROG		: 1;
    unsigned PP_NO_ERR_DETECTION		: 1;
  };
  uint32_t as_int;
} flpv4l_r14_t;
#define FLPv4L_R14_DEFAULT {{0x1, 0x0, 0x0, 0x0}}
#define FLPv4L_R14_DEFAULT_AS_INT 0x000001
_Static_assert(sizeof(flpv4l_r14_t) == 4, "Punned Structure Size");

// Register 0x15
typedef union flpv4l_r15{
  struct{
    unsigned PP_FLSH_ADDR		: 18;
  };
  uint32_t as_int;
} flpv4l_r15_t;
#define FLPv4L_R15_DEFAULT {{0x00000}}
#define FLPv4L_R15_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r15_t) == 4, "Punned Structure Size");

// Register 0x16
typedef union flpv4l_r16{
  struct{
    unsigned PP_LENGTH_STREAMED		: 19;
  };
  uint32_t as_int;
} flpv4l_r16_t;
#define FLPv4L_R16_DEFAULT {{0x00000}}
#define FLPv4L_R16_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r16_t) == 4, "Punned Structure Size");

// Register 0x17
typedef union flpv4l_r17{
  struct{
    unsigned PP_LENGTH_COPIED		: 19;
    unsigned NOT_DEFINED_23_19		: 1;
    unsigned NOT_DEFINED_23_20		: 1;
    unsigned PP_FLAG_COPY_LIMIT		: 1;
    unsigned PP_FLAG_STR_LIMIT		: 1;
    unsigned PP_FLAG_END_OF_FLASH		: 1;
  };
  uint32_t as_int;
} flpv4l_r17_t;
#define FLPv4L_R17_DEFAULT {{0x00000, 0x0, 0x0, 0x0, 0x0, 0x0}}
#define FLPv4L_R17_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r17_t) == 4, "Punned Structure Size");

// Register 0x18
typedef union flpv4l_r18{
  struct{
    unsigned CLK_DIV_SEL		: 2;
    unsigned CLK_RING_SEL		: 4;
  };
  uint32_t as_int;
} flpv4l_r18_t;
#define FLPv4L_R18_DEFAULT {{0x1, 0xC}}
#define FLPv4L_R18_DEFAULT_AS_INT 0x000031
_Static_assert(sizeof(flpv4l_r18_t) == 4, "Punned Structure Size");

// Register 0x19
typedef union flpv4l_r19{
  struct{
    unsigned COMP_CTRL_VOUT		: 3;
    unsigned COMP_CTRL_I_2STG_BAR		: 4;
    unsigned COMP_CTRL_I_1STG		: 4;
    unsigned DISABLE_BYPASS_MIRROR		: 1;
  };
  uint32_t as_int;
} flpv4l_r19_t;
#define FLPv4L_R19_DEFAULT {{0x3, 0x0, 0x8, 0x1}}
#define FLPv4L_R19_DEFAULT_AS_INT 0x000C03
_Static_assert(sizeof(flpv4l_r19_t) == 4, "Punned Structure Size");

// Register 0x1A
// -- EMPTY --

// Register 0x1B
// -- READ-ONLY --

// Register 0x1C
// -- EMPTY --

// Register 0x1D
// -- EMPTY --

// Register 0x1E
// -- READ-ONLY --

// Register 0x1F
typedef union flpv4l_r1F{
  struct{
    unsigned FORCE_RESETN		: 1;
  };
  uint32_t as_int;
} flpv4l_r1F_t;
#define FLPv4L_R1F_DEFAULT {{0x1}}
#define FLPv4L_R1F_DEFAULT_AS_INT 0x000001
_Static_assert(sizeof(flpv4l_r1F_t) == 4, "Punned Structure Size");

// Register 0x20
typedef union flpv4l_r20{
  struct{
    unsigned FLSH_SNT		: 5;
    unsigned FLSH_SET1		: 5;
    unsigned FLSH_SET0		: 5;
  };
  uint32_t as_int;
} flpv4l_r20_t;
#define FLPv4L_R20_DEFAULT {{0x07, 0x04, 0x04}}
#define FLPv4L_R20_DEFAULT_AS_INT 0x001087
_Static_assert(sizeof(flpv4l_r20_t) == 4, "Punned Structure Size");

// Register 0x21
typedef union flpv4l_r21{
  struct{
    unsigned FLSH_SPT2		: 5;
    unsigned FLSH_SPT1		: 5;
    unsigned FLSH_SPT0		: 5;
  };
  uint32_t as_int;
} flpv4l_r21_t;
#define FLPv4L_R21_DEFAULT {{0x04, 0x04, 0x04}}
#define FLPv4L_R21_DEFAULT_AS_INT 0x001084
_Static_assert(sizeof(flpv4l_r21_t) == 4, "Punned Structure Size");

// Register 0x22
typedef union flpv4l_r22{
  struct{
    unsigned FLSH_SYT1		: 5;
    unsigned FLSH_SYT0		: 5;
  };
  uint32_t as_int;
} flpv4l_r22_t;
#define FLPv4L_R22_DEFAULT {{0x07, 0x07}}
#define FLPv4L_R22_DEFAULT_AS_INT 0x0000E7
_Static_assert(sizeof(flpv4l_r22_t) == 4, "Punned Structure Size");

// Register 0x23
typedef union flpv4l_r23{
  struct{
    unsigned FLSH_SRT3		: 5;
    unsigned FLSH_SRT2		: 5;
    unsigned FLSH_SRT1		: 5;
    unsigned FLSH_SRT0		: 5;
  };
  uint32_t as_int;
} flpv4l_r23_t;
#define FLPv4L_R23_DEFAULT {{0x07, 0x03, 0x03, 0x01}}
#define FLPv4L_R23_DEFAULT_AS_INT 0x008C67
_Static_assert(sizeof(flpv4l_r23_t) == 4, "Punned Structure Size");

// Register 0x24
typedef union flpv4l_r24{
  struct{
    unsigned FLSH_SRT6		: 5;
    unsigned FLSH_SRT5		: 5;
    unsigned FLSH_SRT4		: 5;
  };
  uint32_t as_int;
} flpv4l_r24_t;
#define FLPv4L_R24_DEFAULT {{0x01, 0x07, 0x07}}
#define FLPv4L_R24_DEFAULT_AS_INT 0x001CE1
_Static_assert(sizeof(flpv4l_r24_t) == 4, "Punned Structure Size");

// Register 0x25
// -- EMPTY --

// Register 0x26
typedef union flpv4l_r26{
  struct{
    unsigned FLSH_SVR2		: 4;
    unsigned FLSH_SVR1		: 4;
    unsigned FLSH_SVR0		: 4;
    unsigned FLSH_SRIG		: 4;
    unsigned FLSH_SPIG		: 4;
  };
  uint32_t as_int;
} flpv4l_r26_t;
#define FLPv4L_R26_DEFAULT {{0x8, 0x8, 0x7, 0x7, 0xD}}
#define FLPv4L_R26_DEFAULT_AS_INT 0x0D7788
_Static_assert(sizeof(flpv4l_r26_t) == 4, "Punned Structure Size");

// Register 0x27
typedef union flpv4l_r27{
  struct{
    unsigned FLSH_SMV		: 6;
    unsigned FLSH_SHVCT		: 5;
    unsigned FLSH_SHVP		: 5;
    unsigned FLSH_SHVE		: 5;
  };
  uint32_t as_int;
} flpv4l_r27_t;
#define FLPv4L_R27_DEFAULT {{0x08, 0x0F, 0x03, 0x01}}
#define FLPv4L_R27_DEFAULT_AS_INT 0x011BC8
_Static_assert(sizeof(flpv4l_r27_t) == 4, "Punned Structure Size");

// Register 0x28
typedef union flpv4l_r28{
  struct{
    unsigned FLSH_SMVCT1		: 5;
    unsigned FLSH_SMVCT0		: 5;
  };
  uint32_t as_int;
} flpv4l_r28_t;
#define FLPv4L_R28_DEFAULT {{0x07, 0x07}}
#define FLPv4L_R28_DEFAULT_AS_INT 0x0000E7
_Static_assert(sizeof(flpv4l_r28_t) == 4, "Punned Structure Size");

// Register 0x29
// -- EMPTY --

// Register 0x2A
typedef union flpv4l_r2A{
  struct{
    unsigned FLSH_SAB		: 6;
  };
  uint32_t as_int;
} flpv4l_r2A_t;
#define FLPv4L_R2A_DEFAULT {{0x02}}
#define FLPv4L_R2A_DEFAULT_AS_INT 0x000002
_Static_assert(sizeof(flpv4l_r2A_t) == 4, "Punned Structure Size");

// Register 0x2B
// -- EMPTY --

// Register 0x2C
// -- EMPTY --

// Register 0x2D
// -- EMPTY --

// Register 0x2E
// -- EMPTY --

// Register 0x2F
// -- EMPTY --

// Register 0x30
typedef union flpv4l_r30{
  struct{
    unsigned NOT_DEFINED_48_0		: 1;
    unsigned NOT_DEFINED_48_1		: 1;
    unsigned NOT_DEFINED_48_2		: 1;
    unsigned NOT_DEFINED_48_3		: 1;
    unsigned NOT_DEFINED_48_4		: 1;
    unsigned NOT_DEFINED_48_5		: 1;
    unsigned NOT_DEFINED_48_6		: 1;
    unsigned NOT_DEFINED_48_7		: 1;
    unsigned NOT_DEFINED_48_8		: 1;
    unsigned NOT_DEFINED_48_9		: 1;
    unsigned NOT_DEFINED_48_10		: 1;
    unsigned NOT_DEFINED_48_11		: 1;
    unsigned NOT_DEFINED_48_12		: 1;
    unsigned NOT_DEFINED_48_13		: 1;
    unsigned NOT_DEFINED_48_14		: 1;
    unsigned NOT_DEFINED_48_15		: 1;
    unsigned STR_WR_CH1_ALT_ADDR		: 8;
  };
  uint32_t as_int;
} flpv4l_r30_t;
#define FLPv4L_R30_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF0}}
#define FLPv4L_R30_DEFAULT_AS_INT 0xF00000
_Static_assert(sizeof(flpv4l_r30_t) == 4, "Punned Structure Size");

// Register 0x31
typedef union flpv4l_r31{
  struct{
    unsigned NOT_DEFINED_49_0		: 1;
    unsigned NOT_DEFINED_49_1		: 1;
    unsigned NOT_DEFINED_49_2		: 1;
    unsigned NOT_DEFINED_49_3		: 1;
    unsigned NOT_DEFINED_49_4		: 1;
    unsigned NOT_DEFINED_49_5		: 1;
    unsigned NOT_DEFINED_49_6		: 1;
    unsigned NOT_DEFINED_49_7		: 1;
    unsigned NOT_DEFINED_49_8		: 1;
    unsigned NOT_DEFINED_49_9		: 1;
    unsigned NOT_DEFINED_49_10		: 1;
    unsigned NOT_DEFINED_49_11		: 1;
    unsigned NOT_DEFINED_49_12		: 1;
    unsigned NOT_DEFINED_49_13		: 1;
    unsigned NOT_DEFINED_49_14		: 1;
    unsigned NOT_DEFINED_49_15		: 1;
    unsigned STR_WR_CH1_ALT_REG_WR		: 8;
  };
  uint32_t as_int;
} flpv4l_r31_t;
#define FLPv4L_R31_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00}}
#define FLPv4L_R31_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r31_t) == 4, "Punned Structure Size");

// Register 0x32
typedef union flpv4l_r32{
  struct{
    unsigned STR_WR_CH1_BUF_LEN		: 13;
    unsigned NOT_DEFINED_50_13		: 1;
    unsigned NOT_DEFINED_50_14		: 1;
    unsigned NOT_DEFINED_50_15		: 1;
    unsigned NOT_DEFINED_50_16		: 1;
    unsigned NOT_DEFINED_50_17		: 1;
    unsigned NOT_DEFINED_50_18		: 1;
    unsigned NOT_DEFINED_50_19		: 1;
    unsigned NOT_DEFINED_50_20		: 1;
    unsigned STR_WR_CH1_DBLB		: 1;
    unsigned STR_WR_CH1_WRP		: 1;
    unsigned STR_WR_CH1_EN		: 1;
  };
  uint32_t as_int;
} flpv4l_r32_t;
#define FLPv4L_R32_DEFAULT {{0x1FFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1}}
#define FLPv4L_R32_DEFAULT_AS_INT 0xC01FFF
_Static_assert(sizeof(flpv4l_r32_t) == 4, "Punned Structure Size");

// Register 0x33
typedef union flpv4l_r33{
  struct{
    unsigned STR_WR_CH1_BUF_OFF		: 13;
  };
  uint32_t as_int;
} flpv4l_r33_t;
#define FLPv4L_R33_DEFAULT {{0x0000}}
#define FLPv4L_R33_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r33_t) == 4, "Punned Structure Size");

// Register 0x34
typedef union flpv4l_r34{
  struct{
    unsigned NOT_DEFINED_52_0		: 1;
    unsigned NOT_DEFINED_52_1		: 1;
    unsigned NOT_DEFINED_52_2		: 1;
    unsigned NOT_DEFINED_52_3		: 1;
    unsigned NOT_DEFINED_52_4		: 1;
    unsigned NOT_DEFINED_52_5		: 1;
    unsigned NOT_DEFINED_52_6		: 1;
    unsigned NOT_DEFINED_52_7		: 1;
    unsigned NOT_DEFINED_52_8		: 1;
    unsigned NOT_DEFINED_52_9		: 1;
    unsigned NOT_DEFINED_52_10		: 1;
    unsigned NOT_DEFINED_52_11		: 1;
    unsigned NOT_DEFINED_52_12		: 1;
    unsigned NOT_DEFINED_52_13		: 1;
    unsigned NOT_DEFINED_52_14		: 1;
    unsigned NOT_DEFINED_52_15		: 1;
    unsigned STR_WR_CH0_ALT_ADDR		: 8;
  };
  uint32_t as_int;
} flpv4l_r34_t;
#define FLPv4L_R34_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF0}}
#define FLPv4L_R34_DEFAULT_AS_INT 0xF00000
_Static_assert(sizeof(flpv4l_r34_t) == 4, "Punned Structure Size");

// Register 0x35
typedef union flpv4l_r35{
  struct{
    unsigned NOT_DEFINED_53_0		: 1;
    unsigned NOT_DEFINED_53_1		: 1;
    unsigned NOT_DEFINED_53_2		: 1;
    unsigned NOT_DEFINED_53_3		: 1;
    unsigned NOT_DEFINED_53_4		: 1;
    unsigned NOT_DEFINED_53_5		: 1;
    unsigned NOT_DEFINED_53_6		: 1;
    unsigned NOT_DEFINED_53_7		: 1;
    unsigned NOT_DEFINED_53_8		: 1;
    unsigned NOT_DEFINED_53_9		: 1;
    unsigned NOT_DEFINED_53_10		: 1;
    unsigned NOT_DEFINED_53_11		: 1;
    unsigned NOT_DEFINED_53_12		: 1;
    unsigned NOT_DEFINED_53_13		: 1;
    unsigned NOT_DEFINED_53_14		: 1;
    unsigned NOT_DEFINED_53_15		: 1;
    unsigned STR_WR_CH0_ALT_REG_WR		: 8;
  };
  uint32_t as_int;
} flpv4l_r35_t;
#define FLPv4L_R35_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00}}
#define FLPv4L_R35_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r35_t) == 4, "Punned Structure Size");

// Register 0x36
typedef union flpv4l_r36{
  struct{
    unsigned STR_WR_CH0_BUF_LEN		: 13;
    unsigned NOT_DEFINED_54_13		: 1;
    unsigned NOT_DEFINED_54_14		: 1;
    unsigned NOT_DEFINED_54_15		: 1;
    unsigned NOT_DEFINED_54_16		: 1;
    unsigned NOT_DEFINED_54_17		: 1;
    unsigned NOT_DEFINED_54_18		: 1;
    unsigned NOT_DEFINED_54_19		: 1;
    unsigned NOT_DEFINED_54_20		: 1;
    unsigned STR_WR_CH0_DBLB		: 1;
    unsigned STR_WR_CH0_WRP		: 1;
    unsigned STR_WR_CH0_EN		: 1;
  };
  uint32_t as_int;
} flpv4l_r36_t;
#define FLPv4L_R36_DEFAULT {{0x1FFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1}}
#define FLPv4L_R36_DEFAULT_AS_INT 0xC01FFF
_Static_assert(sizeof(flpv4l_r36_t) == 4, "Punned Structure Size");

// Register 0x37
typedef union flpv4l_r37{
  struct{
    unsigned STR_WR_CH0_BUF_OFF		: 13;
  };
  uint32_t as_int;
} flpv4l_r37_t;
#define FLPv4L_R37_DEFAULT {{0x0000}}
#define FLPv4L_R37_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r37_t) == 4, "Punned Structure Size");

// Register 0x38
// -- EMPTY --

// Register 0x39
// -- EMPTY --

// Register 0x3A
typedef union flpv4l_r3A{
  struct{
    unsigned NOT_DEFINED_58_0		: 1;
    unsigned NOT_DEFINED_58_1		: 1;
    unsigned NOT_DEFINED_58_2		: 1;
    unsigned NOT_DEFINED_58_3		: 1;
    unsigned NOT_DEFINED_58_4		: 1;
    unsigned NOT_DEFINED_58_5		: 1;
    unsigned NOT_DEFINED_58_6		: 1;
    unsigned NOT_DEFINED_58_7		: 1;
    unsigned NOT_DEFINED_58_8		: 1;
    unsigned NOT_DEFINED_58_9		: 1;
    unsigned NOT_DEFINED_58_10		: 1;
    unsigned NOT_DEFINED_58_11		: 1;
    unsigned NOT_DEFINED_58_12		: 1;
    unsigned NOT_DEFINED_58_13		: 1;
    unsigned NOT_DEFINED_58_14		: 1;
    unsigned NOT_DEFINED_58_15		: 1;
    unsigned NOT_DEFINED_58_16		: 1;
    unsigned NOT_DEFINED_58_17		: 1;
    unsigned NOT_DEFINED_58_18		: 1;
    unsigned NOT_DEFINED_58_19		: 1;
    unsigned NOT_DEFINED_58_20		: 1;
    unsigned NOT_DEFINED_58_21		: 1;
    unsigned NOT_DEFINED_58_22		: 1;
    unsigned BLK_WR_EN		: 1;
  };
  uint32_t as_int;
} flpv4l_r3A_t;
#define FLPv4L_R3A_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1}}
#define FLPv4L_R3A_DEFAULT_AS_INT 0x800000
_Static_assert(sizeof(flpv4l_r3A_t) == 4, "Punned Structure Size");

// Register 0x3B
// -- EMPTY --

// Register 0x3C
// -- EMPTY --

// Register 0x3D
// -- EMPTY --

// Register 0x3E
// -- EMPTY --

// Register 0x3F
// -- EMPTY --

// Register 0x40
// -- EMPTY --

// Register 0x41
// -- EMPTY --

// Register 0x42
// -- EMPTY --

// Register 0x43
// -- EMPTY --

// Register 0x44
// -- EMPTY --

// Register 0x45
// -- EMPTY --

// Register 0x46
// -- EMPTY --

// Register 0x47
typedef union flpv4l_r47{
  struct{
    unsigned NOT_DEFINED_71_0		: 1;
    unsigned NOT_DEFINED_71_1		: 1;
    unsigned NOT_DEFINED_71_2		: 1;
    unsigned NOT_DEFINED_71_3		: 1;
    unsigned NOT_DEFINED_71_4		: 1;
    unsigned NOT_DEFINED_71_5		: 1;
    unsigned NOT_DEFINED_71_6		: 1;
    unsigned NOT_DEFINED_71_7		: 1;
    unsigned NOT_DEFINED_71_8		: 1;
    unsigned NOT_DEFINED_71_9		: 1;
    unsigned NOT_DEFINED_71_10		: 1;
    unsigned NOT_DEFINED_71_11		: 1;
    unsigned NOT_DEFINED_71_12		: 1;
    unsigned NOT_DEFINED_71_13		: 1;
    unsigned NOT_DEFINED_71_14		: 1;
    unsigned NOT_DEFINED_71_15		: 1;
    unsigned NOT_DEFINED_71_16		: 1;
    unsigned NOT_DEFINED_71_17		: 1;
    unsigned NOT_DEFINED_71_18		: 1;
    unsigned NOT_DEFINED_71_19		: 1;
    unsigned NOT_DEFINED_71_20		: 1;
    unsigned NOT_DEFINED_71_21		: 1;
    unsigned NOT_DEFINED_71_22		: 1;
    unsigned ACT_RST		: 1;
  };
  uint32_t as_int;
} flpv4l_r47_t;
#define FLPv4L_R47_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
#define FLPv4L_R47_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(flpv4l_r47_t) == 4, "Punned Structure Size");

#endif // FLPV4L_RF_H