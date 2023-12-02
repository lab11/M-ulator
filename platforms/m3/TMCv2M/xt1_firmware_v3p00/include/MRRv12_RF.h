//************************************************************
// Desciption: MRRv12 Register File Header File
//      Generated by genRF (Version 1.51) 11/04/2022 16:25:01
//************************************************************

#ifndef MRRV12_RF_H
#define MRRV12_RF_H

// Register 0x00
typedef union mrrv12_r00{
  struct{
    unsigned TRX_SEL_RING		: 2;
    unsigned TRX_RESETB		: 1;
    unsigned TRX_SEL_VLDO		: 4;
    unsigned TRX_SEL_IBIAS		: 4;
    unsigned TRX_EN_LDO		: 1;
    unsigned TRX_VREF_SMPb		: 1;
    unsigned TRX_CL_CTRL		: 7;
    unsigned TRX_ENb_CONT_RC		: 1;
    unsigned TRX_EN_CL		: 1;
    unsigned NOT_DEFINED_0_22		: 1;
    unsigned TRX_EN_CLK_MON		: 1;
  };
  uint32_t as_int;
} mrrv12_r00_t;
#define MRRv12_R00_DEFAULT {{0x2, 0x0, 0x9, 0xB, 0x0, 0x1, 0x08, 0x1, 0x0, 0x0, 0x0}}
#define MRRv12_R00_DEFAULT_AS_INT 0x1115CA
_Static_assert(sizeof(mrrv12_r00_t) == 4, "Punned Structure Size");

// Register 0x01
typedef union mrrv12_r01{
  struct{
    unsigned TRX_RF_GOC_OUT_ENABLE		: 1;
    unsigned TRX_RF_GOC_SEL_OSC_STAGE		: 1;
    unsigned TRX_RF_GOC_SEL_DIV		: 2;
  };
  uint32_t as_int;
} mrrv12_r01_t;
#define MRRv12_R01_DEFAULT {{0x1, 0x0, 0x0}}
#define MRRv12_R01_DEFAULT_AS_INT 0x000001
_Static_assert(sizeof(mrrv12_r01_t) == 4, "Punned Structure Size");

// Register 0x02
typedef union mrrv12_r02{
  struct{
    unsigned TRX_MIXER_SEL_IBIAS		: 4;
    unsigned TRX_LO_BUF_SEL_IBIAS		: 4;
    unsigned TRX_LO_SEL_RLOAD		: 3;
    unsigned TRX_LNA_SEL_IBIAS		: 4;
    unsigned TRX_CMP_SEL_IBIAS		: 4;
    unsigned TRX_TX_BIAS_TUNE		: 4;
    unsigned TRX_PGb_RO		: 1;
  };
  uint32_t as_int;
} mrrv12_r02_t;
#define MRRv12_R02_DEFAULT {{0x2, 0x0, 0x1, 0x2, 0x2, 0x5, 0x0}}
#define MRRv12_R02_DEFAULT_AS_INT 0x291102
_Static_assert(sizeof(mrrv12_r02_t) == 4, "Punned Structure Size");

// Register 0x03
typedef union mrrv12_r03{
  struct{
    unsigned TRX_MIXER_EN_MON		: 1;
    unsigned TRX_IF_EN_MON		: 1;
    unsigned TRX_IF_SEL_MON		: 1;
    unsigned TRX_IF2_SEL_GAIN		: 5;
    unsigned TRX_IF2_SEL_IBIAS		: 3;
    unsigned TRX_IF1_SEL_GAIN		: 5;
    unsigned TRX_IF1_SEL_IBIAS		: 3;
    unsigned TRX_IF_SEL_VIN		: 1;
  };
  uint32_t as_int;
} mrrv12_r03_t;
#define MRRv12_R03_DEFAULT {{0x0, 0x0, 0x0, 0x00, 0x5, 0x00, 0x5, 0x0}}
#define MRRv12_R03_DEFAULT_AS_INT 0x050500
_Static_assert(sizeof(mrrv12_r03_t) == 4, "Punned Structure Size");

// Register 0x04
typedef union mrrv12_r04{
  struct{
    unsigned TRX_CTRL_OVERRIDE		: 3;
  };
  uint32_t as_int;
} mrrv12_r04_t;
#define MRRv12_R04_DEFAULT {{0x0}}
#define MRRv12_R04_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r04_t) == 4, "Punned Structure Size");

// Register 0x05
typedef union mrrv12_r05{
  struct{
    unsigned TRX_CAP_TUNE_FINE_OVR		: 7;
    unsigned TRX_CAP_TUNE_MID_OVR		: 4;
    unsigned TRX_CAP_TUNE_COARSE_OVR		: 6;
  };
  uint32_t as_int;
} mrrv12_r05_t;
#define MRRv12_R05_DEFAULT {{0x00, 0x0, 0x00}}
#define MRRv12_R05_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r05_t) == 4, "Punned Structure Size");

// Register 0x06
typedef union mrrv12_r06{
  struct{
    unsigned TRX_RX_SEL_IBIAS_OVR		: 4;
    unsigned TRX_LO_SEL_IBIAS_OVR		: 7;
    unsigned NOT_DEFINED_6_11		: 1;
    unsigned TRX_LO_SEL_CAP_OVR		: 8;
  };
  uint32_t as_int;
} mrrv12_r06_t;
#define MRRv12_R06_DEFAULT {{0x0, 0x00, 0x0, 0x00}}
#define MRRv12_R06_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r06_t) == 4, "Punned Structure Size");

// Register 0x07
typedef union mrrv12_r07{
  struct{
    unsigned TRX_IF2_EN_VBIAS_OVR		: 1;
    unsigned TRX_IF2_EN_OVR		: 1;
    unsigned TRX_IF1_EN_OVR		: 1;
    unsigned TRX_MIXER_EN_OVR		: 1;
    unsigned TRX_LO_EN_OVR		: 1;
    unsigned TRX_LNA_EN_OVR		: 1;
    unsigned TRX_CMP_EN_OVR		: 1;
    unsigned TRX_EN_RX_IREF_OVR		: 1;
    unsigned TRX_EN_RX_OVR		: 1;
    unsigned TRX_EN_PO_OVR		: 1;
  };
  uint32_t as_int;
} mrrv12_r07_t;
#define MRRv12_R07_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
#define MRRv12_R07_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r07_t) == 4, "Punned Structure Size");

// Register 0x08
typedef union mrrv12_r08{
  struct{
    unsigned DO_RX_DATA		: 1;
    unsigned DO_RX_PO_CAP_TUNE		: 1;
    unsigned DO_RX_PO_CURR_TUNE		: 1;
    unsigned DO_RX_LO_CURR_TUNE		: 1;
    unsigned DO_RX_LO_CAP_TUNE		: 1;
    unsigned DO_TX_DATA		: 1;
    unsigned DO_TX_PO_CAP_TUNE		: 1;
    unsigned DO_RX_PO_CAP_TUNE_ONCE		: 1;
    unsigned DO_RX_PO_CURR_TUNE_ONCE		: 1;
    unsigned DO_RX_LO_CURR_TUNE_ONCE		: 1;
    unsigned DO_RX_LO_CAP_TUNE_ONCE		: 1;
    unsigned DO_TX_PO_CAP_TUNE_ONCE		: 1;
  };
  uint32_t as_int;
} mrrv12_r08_t;
#define MRRv12_R08_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0}}
#define MRRv12_R08_DEFAULT_AS_INT 0x000060
_Static_assert(sizeof(mrrv12_r08_t) == 4, "Punned Structure Size");

// Register 0x09
typedef union mrrv12_r09{
  struct{
    unsigned TX_LEN		: 8;
  };
  uint32_t as_int;
} mrrv12_r09_t;
#define MRRv12_R09_DEFAULT {{0x00}}
#define MRRv12_R09_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r09_t) == 4, "Punned Structure Size");

// Register 0x0A
typedef union mrrv12_r0A{
  struct{
    unsigned N_TX_TUNE_IDL		: 4;
    unsigned N_TX_TUNE_ACT		: 4;
    unsigned TX_TUNE_CNT_DLY		: 1;
  };
  uint32_t as_int;
} mrrv12_r0A_t;
#define MRRv12_R0A_DEFAULT {{0x0, 0x2, 0x0}}
#define MRRv12_R0A_DEFAULT_AS_INT 0x000020
_Static_assert(sizeof(mrrv12_r0A_t) == 4, "Punned Structure Size");

// Register 0x0B
typedef union mrrv12_r0B{
  struct{
    unsigned N_TX_CHARGING		: 10;
  };
  uint32_t as_int;
} mrrv12_r0B_t;
#define MRRv12_R0B_DEFAULT {{0x3FF}}
#define MRRv12_R0B_DEFAULT_AS_INT 0x0003FF
_Static_assert(sizeof(mrrv12_r0B_t) == 4, "Punned Structure Size");

// Register 0x0C
typedef union mrrv12_r0C{
  struct{
    unsigned N_TX_DATA_IDL		: 4;
    unsigned N_TX_DATA_ACT		: 4;
  };
  uint32_t as_int;
} mrrv12_r0C_t;
#define MRRv12_R0C_DEFAULT {{0x1, 0x2}}
#define MRRv12_R0C_DEFAULT_AS_INT 0x000021
_Static_assert(sizeof(mrrv12_r0C_t) == 4, "Punned Structure Size");

// Register 0x0D
// -- EMPTY --

// Register 0x0E
typedef union mrrv12_r0E{
  struct{
    unsigned N_RX_TUNE_IDL		: 4;
    unsigned N_RX_TUNE_ACT		: 4;
    unsigned RX_TUNE_CNT_DLY		: 1;
  };
  uint32_t as_int;
} mrrv12_r0E_t;
#define MRRv12_R0E_DEFAULT {{0x0, 0x2, 0x0}}
#define MRRv12_R0E_DEFAULT_AS_INT 0x000020
_Static_assert(sizeof(mrrv12_r0E_t) == 4, "Punned Structure Size");

// Register 0x0F
// -- MEMORY-MAPPED --

// Register 0x10
// -- MEMORY-MAPPED --

// Register 0x11
// -- EMPTY --

// Register 0x12
typedef union mrrv12_r12{
  struct{
    unsigned TRX_CAP_TUNE_FINE_SAVED		: 7;
    unsigned TRX_CAP_TUNE_MID_SAVED		: 4;
    unsigned TRX_CAP_TUNE_COARSE_SAVED		: 6;
  };
  uint32_t as_int;
} mrrv12_r12_t;
#define MRRv12_R12_DEFAULT {{0x3F, 0x7, 0x1F}}
#define MRRv12_R12_DEFAULT_AS_INT 0x00FBBF
_Static_assert(sizeof(mrrv12_r12_t) == 4, "Punned Structure Size");

// Register 0x13
// -- EMPTY --

// Register 0x14
typedef union mrrv12_r14{
  struct{
    unsigned RX_LO_CAP_TUNE_SAVED		: 8;
  };
  uint32_t as_int;
} mrrv12_r14_t;
#define MRRv12_R14_DEFAULT {{0x00}}
#define MRRv12_R14_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r14_t) == 4, "Punned Structure Size");

// Register 0x15
// -- EMPTY --

// Register 0x16
typedef union mrrv12_r16{
  struct{
    unsigned RX_LO_CURR_TUNE_SAVED		: 7;
  };
  uint32_t as_int;
} mrrv12_r16_t;
#define MRRv12_R16_DEFAULT {{0x3F}}
#define MRRv12_R16_DEFAULT_AS_INT 0x00003F
_Static_assert(sizeof(mrrv12_r16_t) == 4, "Punned Structure Size");

// Register 0x17
// -- EMPTY --

// Register 0x18
typedef union mrrv12_r18{
  struct{
    unsigned RX_PO_CURR_TUNE_SAVED		: 4;
  };
  uint32_t as_int;
} mrrv12_r18_t;
#define MRRv12_R18_DEFAULT {{0x0}}
#define MRRv12_R18_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r18_t) == 4, "Punned Structure Size");

// Register 0x19
typedef union mrrv12_r19{
  struct{
    unsigned TX_PO_CAP_CNT_TARG_OVR		: 18;
    unsigned TX_PO_CAP_CNT_TARG_ERR		: 6;
  };
  uint32_t as_int;
} mrrv12_r19_t;
#define MRRv12_R19_DEFAULT {{0x00000, 0x01}}
#define MRRv12_R19_DEFAULT_AS_INT 0x040000
_Static_assert(sizeof(mrrv12_r19_t) == 4, "Punned Structure Size");

// Register 0x1A
typedef union mrrv12_r1A{
  struct{
    unsigned TX_PO_CAP_CNT_VAL		: 18;
  };
  uint32_t as_int;
} mrrv12_r1A_t;
#define MRRv12_R1A_DEFAULT {{0x00000}}
#define MRRv12_R1A_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r1A_t) == 4, "Punned Structure Size");

// Register 0x1B
typedef union mrrv12_r1B{
  struct{
    unsigned RX_LO_CAP_CNT_TARG_OVR		: 18;
  };
  uint32_t as_int;
} mrrv12_r1B_t;
#define MRRv12_R1B_DEFAULT {{0x00000}}
#define MRRv12_R1B_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r1B_t) == 4, "Punned Structure Size");

// Register 0x1C
typedef union mrrv12_r1C{
  struct{
    unsigned RX_LO_CAP_CNT_VAL		: 18;
  };
  uint32_t as_int;
} mrrv12_r1C_t;
#define MRRv12_R1C_DEFAULT {{0x00000}}
#define MRRv12_R1C_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r1C_t) == 4, "Punned Structure Size");

// Register 0x1D
typedef union mrrv12_r1D{
  struct{
    unsigned RX_LO_CURR_CNT_TARG_OVR		: 18;
    unsigned RX_LO_CURR_CNT_TARG_ERR		: 6;
  };
  uint32_t as_int;
} mrrv12_r1D_t;
#define MRRv12_R1D_DEFAULT {{0x00000, 0x03}}
#define MRRv12_R1D_DEFAULT_AS_INT 0x0C0000
_Static_assert(sizeof(mrrv12_r1D_t) == 4, "Punned Structure Size");

// Register 0x1E
typedef union mrrv12_r1E{
  struct{
    unsigned RX_LO_CURR_CNT_VAL		: 18;
  };
  uint32_t as_int;
} mrrv12_r1E_t;
#define MRRv12_R1E_DEFAULT {{0x00000}}
#define MRRv12_R1E_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r1E_t) == 4, "Punned Structure Size");

// Register 0x1F
typedef union mrrv12_r1F{
  struct{
    unsigned RX_PO_CURR_CNT_TARG_OVR		: 18;
    unsigned RX_PO_CURR_CNT_TARG_ERR		: 6;
  };
  uint32_t as_int;
} mrrv12_r1F_t;
#define MRRv12_R1F_DEFAULT {{0x00000, 0x03}}
#define MRRv12_R1F_DEFAULT_AS_INT 0x0C0000
_Static_assert(sizeof(mrrv12_r1F_t) == 4, "Punned Structure Size");

// Register 0x20
typedef union mrrv12_r20{
  struct{
    unsigned RX_PO_CURR_CNT_VAL		: 18;
  };
  uint32_t as_int;
} mrrv12_r20_t;
#define MRRv12_R20_DEFAULT {{0x00000}}
#define MRRv12_R20_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r20_t) == 4, "Punned Structure Size");

// Register 0x21
typedef union mrrv12_r21{
  struct{
    unsigned RX_PO_CAP_CNT_TARG_OVR		: 18;
    unsigned RX_PO_CAP_CNT_TARG_ERR		: 6;
  };
  uint32_t as_int;
} mrrv12_r21_t;
#define MRRv12_R21_DEFAULT {{0x00000, 0x01}}
#define MRRv12_R21_DEFAULT_AS_INT 0x040000
_Static_assert(sizeof(mrrv12_r21_t) == 4, "Punned Structure Size");

// Register 0x22
typedef union mrrv12_r22{
  struct{
    unsigned RX_PO_CAP_CNT_VAL		: 18;
  };
  uint32_t as_int;
} mrrv12_r22_t;
#define MRRv12_R22_DEFAULT {{0x00000}}
#define MRRv12_R22_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r22_t) == 4, "Punned Structure Size");

// Register 0x23
// -- EMPTY --

// Register 0x24
// -- EMPTY --

// Register 0x25
// -- EMPTY --

// Register 0x26
// -- EMPTY --

// Register 0x27
// -- EMPTY --

// Register 0x28
// -- EMPTY --

// Register 0x29
// -- EMPTY --

// Register 0x2A
// -- EMPTY --

// Register 0x2B
// -- EMPTY --

// Register 0x2C
// -- EMPTY --

// Register 0x2D
typedef union mrrv12_r2D{
  struct{
    unsigned IRQ_REG_ADDR		: 8;
    unsigned IRQ_SHORT_ADDR		: 8;
    unsigned IRQ_ENABLE		: 1;
  };
  uint32_t as_int;
} mrrv12_r2D_t;
#define MRRv12_R2D_DEFAULT {{0x04, 0x10, 0x1}}
#define MRRv12_R2D_DEFAULT_AS_INT 0x011004
_Static_assert(sizeof(mrrv12_r2D_t) == 4, "Punned Structure Size");

// Register 0x2E
// -- READ-ONLY --

// Register 0x2F
typedef union mrrv12_r2F{
  struct{
    unsigned LC_CLK_RING		: 2;
    unsigned LC_CLK_DIV		: 2;
    unsigned LC_CLK_OUT_ENABLE		: 1;
  };
  uint32_t as_int;
} mrrv12_r2F_t;
#define MRRv12_R2F_DEFAULT {{0x1, 0x3, 0x0}}
#define MRRv12_R2F_DEFAULT_AS_INT 0x00000D
_Static_assert(sizeof(mrrv12_r2F_t) == 4, "Punned Structure Size");

// Register 0x30
// -- EMPTY --

// Register 0x31
// -- EMPTY --

// Register 0x32
// -- EMPTY --

// Register 0x33
// -- EMPTY --

// Register 0x34
// -- EMPTY --

// Register 0x35
// -- EMPTY --

// Register 0x36
// -- EMPTY --

// Register 0x37
// -- EMPTY --

// Register 0x38
// -- EMPTY --

// Register 0x39
// -- EMPTY --

// Register 0x3A
// -- EMPTY --

// Register 0x3B
// -- EMPTY --

// Register 0x3C
// -- EMPTY --

// Register 0x3D
typedef union mrrv12_r3D{
  struct{
    unsigned NOT_DEFINED_61_0		: 1;
    unsigned NOT_DEFINED_61_1		: 1;
    unsigned NOT_DEFINED_61_2		: 1;
    unsigned NOT_DEFINED_61_3		: 1;
    unsigned NOT_DEFINED_61_4		: 1;
    unsigned NOT_DEFINED_61_5		: 1;
    unsigned NOT_DEFINED_61_6		: 1;
    unsigned NOT_DEFINED_61_7		: 1;
    unsigned NOT_DEFINED_61_8		: 1;
    unsigned NOT_DEFINED_61_9		: 1;
    unsigned NOT_DEFINED_61_10		: 1;
    unsigned NOT_DEFINED_61_11		: 1;
    unsigned NOT_DEFINED_61_12		: 1;
    unsigned NOT_DEFINED_61_13		: 1;
    unsigned NOT_DEFINED_61_14		: 1;
    unsigned NOT_DEFINED_61_15		: 1;
    unsigned STR_WR_CH0_ALT_ADDR		: 8;
  };
  uint32_t as_int;
} mrrv12_r3D_t;
#define MRRv12_R3D_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF0}}
#define MRRv12_R3D_DEFAULT_AS_INT 0xF00000
_Static_assert(sizeof(mrrv12_r3D_t) == 4, "Punned Structure Size");

// Register 0x3E
typedef union mrrv12_r3E{
  struct{
    unsigned NOT_DEFINED_62_0		: 1;
    unsigned NOT_DEFINED_62_1		: 1;
    unsigned NOT_DEFINED_62_2		: 1;
    unsigned NOT_DEFINED_62_3		: 1;
    unsigned NOT_DEFINED_62_4		: 1;
    unsigned NOT_DEFINED_62_5		: 1;
    unsigned NOT_DEFINED_62_6		: 1;
    unsigned NOT_DEFINED_62_7		: 1;
    unsigned NOT_DEFINED_62_8		: 1;
    unsigned NOT_DEFINED_62_9		: 1;
    unsigned NOT_DEFINED_62_10		: 1;
    unsigned NOT_DEFINED_62_11		: 1;
    unsigned NOT_DEFINED_62_12		: 1;
    unsigned NOT_DEFINED_62_13		: 1;
    unsigned NOT_DEFINED_62_14		: 1;
    unsigned NOT_DEFINED_62_15		: 1;
    unsigned STR_WR_CH0_ALT_REG_WR		: 8;
  };
  uint32_t as_int;
} mrrv12_r3E_t;
#define MRRv12_R3E_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00}}
#define MRRv12_R3E_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r3E_t) == 4, "Punned Structure Size");

// Register 0x3F
typedef union mrrv12_r3F{
  struct{
    unsigned STR_WR_CH0_BUF_LEN		: 3;
    unsigned NOT_DEFINED_63_3		: 1;
    unsigned NOT_DEFINED_63_4		: 1;
    unsigned NOT_DEFINED_63_5		: 1;
    unsigned NOT_DEFINED_63_6		: 1;
    unsigned NOT_DEFINED_63_7		: 1;
    unsigned NOT_DEFINED_63_8		: 1;
    unsigned NOT_DEFINED_63_9		: 1;
    unsigned NOT_DEFINED_63_10		: 1;
    unsigned NOT_DEFINED_63_11		: 1;
    unsigned NOT_DEFINED_63_12		: 1;
    unsigned NOT_DEFINED_63_13		: 1;
    unsigned NOT_DEFINED_63_14		: 1;
    unsigned NOT_DEFINED_63_15		: 1;
    unsigned NOT_DEFINED_63_16		: 1;
    unsigned NOT_DEFINED_63_17		: 1;
    unsigned NOT_DEFINED_63_18		: 1;
    unsigned NOT_DEFINED_63_19		: 1;
    unsigned NOT_DEFINED_63_20		: 1;
    unsigned STR_WR_CH0_DBLB		: 1;
    unsigned STR_WR_CH0_WRP		: 1;
    unsigned STR_WR_CH0_EN		: 1;
  };
  uint32_t as_int;
} mrrv12_r3F_t;
#define MRRv12_R3F_DEFAULT {{0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1}}
#define MRRv12_R3F_DEFAULT_AS_INT 0x800007
_Static_assert(sizeof(mrrv12_r3F_t) == 4, "Punned Structure Size");

// Register 0x40
typedef union mrrv12_r40{
  struct{
    unsigned STR_WR_CH0_BUF_OFF		: 3;
  };
  uint32_t as_int;
} mrrv12_r40_t;
#define MRRv12_R40_DEFAULT {{0x0}}
#define MRRv12_R40_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r40_t) == 4, "Punned Structure Size");

// Register 0x41
// -- EMPTY --

// Register 0x42
// -- EMPTY --

// Register 0x43
typedef union mrrv12_r43{
  struct{
    unsigned NOT_DEFINED_67_0		: 1;
    unsigned NOT_DEFINED_67_1		: 1;
    unsigned NOT_DEFINED_67_2		: 1;
    unsigned NOT_DEFINED_67_3		: 1;
    unsigned NOT_DEFINED_67_4		: 1;
    unsigned NOT_DEFINED_67_5		: 1;
    unsigned NOT_DEFINED_67_6		: 1;
    unsigned NOT_DEFINED_67_7		: 1;
    unsigned NOT_DEFINED_67_8		: 1;
    unsigned NOT_DEFINED_67_9		: 1;
    unsigned NOT_DEFINED_67_10		: 1;
    unsigned NOT_DEFINED_67_11		: 1;
    unsigned NOT_DEFINED_67_12		: 1;
    unsigned NOT_DEFINED_67_13		: 1;
    unsigned NOT_DEFINED_67_14		: 1;
    unsigned NOT_DEFINED_67_15		: 1;
    unsigned NOT_DEFINED_67_16		: 1;
    unsigned NOT_DEFINED_67_17		: 1;
    unsigned NOT_DEFINED_67_18		: 1;
    unsigned NOT_DEFINED_67_19		: 1;
    unsigned NOT_DEFINED_67_20		: 1;
    unsigned NOT_DEFINED_67_21		: 1;
    unsigned NOT_DEFINED_67_22		: 1;
    unsigned BLK_WR_EN		: 1;
  };
  uint32_t as_int;
} mrrv12_r43_t;
#define MRRv12_R43_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1}}
#define MRRv12_R43_DEFAULT_AS_INT 0x800000
_Static_assert(sizeof(mrrv12_r43_t) == 4, "Punned Structure Size");

// Register 0x44
// -- EMPTY --

// Register 0x45
// -- EMPTY --

// Register 0x46
// -- EMPTY --

// Register 0x47
// -- EMPTY --

// Register 0x48
// -- EMPTY --

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
typedef union mrrv12_r50{
  struct{
    unsigned NOT_DEFINED_80_0		: 1;
    unsigned NOT_DEFINED_80_1		: 1;
    unsigned NOT_DEFINED_80_2		: 1;
    unsigned NOT_DEFINED_80_3		: 1;
    unsigned NOT_DEFINED_80_4		: 1;
    unsigned NOT_DEFINED_80_5		: 1;
    unsigned NOT_DEFINED_80_6		: 1;
    unsigned NOT_DEFINED_80_7		: 1;
    unsigned NOT_DEFINED_80_8		: 1;
    unsigned NOT_DEFINED_80_9		: 1;
    unsigned NOT_DEFINED_80_10		: 1;
    unsigned NOT_DEFINED_80_11		: 1;
    unsigned NOT_DEFINED_80_12		: 1;
    unsigned NOT_DEFINED_80_13		: 1;
    unsigned NOT_DEFINED_80_14		: 1;
    unsigned NOT_DEFINED_80_15		: 1;
    unsigned NOT_DEFINED_80_16		: 1;
    unsigned NOT_DEFINED_80_17		: 1;
    unsigned NOT_DEFINED_80_18		: 1;
    unsigned NOT_DEFINED_80_19		: 1;
    unsigned NOT_DEFINED_80_20		: 1;
    unsigned NOT_DEFINED_80_21		: 1;
    unsigned NOT_DEFINED_80_22		: 1;
    unsigned ACT_RST		: 1;
  };
  uint32_t as_int;
} mrrv12_r50_t;
#define MRRv12_R50_DEFAULT {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
#define MRRv12_R50_DEFAULT_AS_INT 0x000000
_Static_assert(sizeof(mrrv12_r50_t) == 4, "Punned Structure Size");

#endif // MRRV12_RF_H