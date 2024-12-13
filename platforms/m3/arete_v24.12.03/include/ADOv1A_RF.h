//************************************************************
// Desciption: ADOv1A Register File Header File
//      Generated by genRF (Version 1.51) 10/13/2023 13:35:54
//************************************************************

//*******************************************************************************************
// This file has been modified as below
//  - Prefix adov1a is replaced by ado
//  - Prefix ADOv1A is replaced by ADO
//-------------------------------------------------------------------------------------------
//  Jan 05 2023 -   First commit 
//  Yejoong Kim (yejoong@cubeworks.io)
//*******************************************************************************************

#ifndef ADOV1A_RF_H
#define ADOV1A_RF_H

// Register 0x00
typedef union ado_r00{
  struct{
    unsigned LDO_PG_IREF		: 1;
    unsigned LDO_CTRLB_IREF_IBIAS_1P4HP		: 4;
    unsigned LDO_CTRLB_IREF_IBIAS_0P9HP		: 4;
    unsigned LDO_PG_LDOCORE_1P4HP		: 1;
    unsigned LDO_PG_VREF_1P4HP		: 1;
    unsigned LDO_PG_LDOCORE_0P9HP		: 1;
    unsigned LDO_PG_VREF_0P9HP		: 1;
    unsigned LDO_CTRL_STBCAP_LDOCORE_1P4HP		: 1;
    unsigned LDO_CTRL_STBCAP_LDOCORE_0P9HP		: 1;
  };
  uint32_t as_int;
} ado_r00_t;
#define ADO_R00_DEFAULT {{0x1, 0x9, 0xA, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0}}
#define ADO_R00_DEFAULT_AS_INT 0x001F53
_Static_assert(sizeof(ado_r00_t) == 4, "Punned Structure Size");

// Register 0x01
typedef union ado_r01{
  struct{
    unsigned LDO_CTRL_VREFLDO_VOUT_1P4HP		: 4;
    unsigned LDO_CTRLB_VREFLDO_I_1P4HP		: 4;
    unsigned LDO_CTRL_VREFLDO_VOUT_0P9HP		: 4;
    unsigned LDO_CTRLB_VREFLDO_I_0P9HP		: 4;
  };
  uint32_t as_int;
} ado_r01_t;
#define ADO_R01_DEFAULT {{0x6, 0x8, 0x4, 0x8}}
#define ADO_R01_DEFAULT_AS_INT 0x008486
_Static_assert(sizeof(ado_r01_t) == 4, "Punned Structure Size");

// Register 0x02
typedef union ado_r02{
  struct{
    unsigned AFE_16K_EN		: 1;
    unsigned IB_GEN_CLK_EN		: 1;
    unsigned IB_GEN_FSETTLE		: 1;
    unsigned BUF_OUT_EN		: 1;
    unsigned IB_CON2		: 4;
    unsigned IB_CON1		: 4;
    unsigned IBC_REC_PGA		: 4;
    unsigned IBC_REC_LNA		: 5;
    unsigned REC_ADCDRI_EN		: 1;
  };
  uint32_t as_int;
} ado_r02_t;
#define ADO_R02_DEFAULT {{0x0, 0x0, 0x1, 0x1, 0x5, 0x4, 0x5, 0x03, 0x0}}
#define ADO_R02_DEFAULT_AS_INT 0x03545C
_Static_assert(sizeof(ado_r02_t) == 4, "Punned Structure Size");

// Register 0x03
typedef union ado_r03{
  struct{
    unsigned MIC_VIR_CBASE		: 2;
    unsigned VCM_PCON		: 3;
    unsigned VCM_NCON		: 3;
    unsigned CP_VDOWN_1P2		: 1;
    unsigned CP_CLK_EN_1P2		: 1;
    unsigned CP_CLK_DIV_1P2		: 2;
  };
  uint32_t as_int;
} ado_r03_t;
#define ADO_R03_DEFAULT {{0x0, 0x4, 0x4, 0x1, 0x0, 0x3}}
#define ADO_R03_DEFAULT_AS_INT 0x000D90
_Static_assert(sizeof(ado_r03_t) == 4, "Punned Structure Size");

// Register 0x04
typedef union ado_r04{
  struct{
    unsigned REC_LNA_AMPEN		: 1;
    unsigned REC_LNA_OUTSHORT_EN		: 1;
    unsigned REC_LNA_CMFB_FSETTLE		: 1;
    unsigned OFFSET_CTRL_LNA		: 3;
  };
  uint32_t as_int;
} ado_r04_t;
#define ADO_R04_DEFAULT {{0x0, 0x1, 0x1, 0x6}}
#define ADO_R04_DEFAULT_AS_INT 0x000036
_Static_assert(sizeof(ado_r04_t) == 4, "Punned Structure Size");

// Register 0x05
typedef union ado_r05{
  struct{
    unsigned REC_LNA_P2_CON		: 6;
    unsigned REC_LNA_P1_CON		: 6;
    unsigned REC_LNA_N2_CON		: 6;
    unsigned REC_LNA_N1_CON		: 6;
  };
  uint32_t as_int;
} ado_r05_t;
#define ADO_R05_DEFAULT {{0x08, 0x08, 0x07, 0x07}}
#define ADO_R05_DEFAULT_AS_INT 0x1C7208
_Static_assert(sizeof(ado_r05_t) == 4, "Punned Structure Size");

// Register 0x06
typedef union ado_r06{
  struct{
    unsigned REC_PGA_AMPEN		: 1;
    unsigned REC_PGA_CMFB_FSETTLE		: 1;
    unsigned REC_PGA_OUTSHORT_EN		: 1;
    unsigned OFFSET_CTRL_PGA		: 3;
    unsigned REC_PGA_CFBADD		: 1;
  };
  uint32_t as_int;
} ado_r06_t;
#define ADO_R06_DEFAULT {{0x0, 0x1, 0x1, 0x2, 0x0}}
#define ADO_R06_DEFAULT_AS_INT 0x000016
_Static_assert(sizeof(ado_r06_t) == 4, "Punned Structure Size");

// Register 0x07
typedef union ado_r07{
  struct{
    unsigned REC_PGA_P2_CON		: 5;
    unsigned REC_PGA_P1_CON		: 5;
    unsigned REC_PGA_N1_MCON		: 2;
    unsigned REC_PGA_N1_LCON		: 3;
  };
  uint32_t as_int;
} ado_r07_t;
#define ADO_R07_DEFAULT {{0x01, 0x01, 0x0, 0x7}}
#define ADO_R07_DEFAULT_AS_INT 0x007021
_Static_assert(sizeof(ado_r07_t) == 4, "Punned Structure Size");

// Register 0x08
typedef union ado_r08{
  struct{
    unsigned REC_PGA_GCON		: 5;
    unsigned REC_PGA_BWCON		: 9;
  };
  uint32_t as_int;
} ado_r08_t;
#define ADO_R08_DEFAULT {{0x01, 0x000}}
#define ADO_R08_DEFAULT_AS_INT 0x000001
_Static_assert(sizeof(ado_r08_t) == 4, "Punned Structure Size");

// Register 0x09
typedef union ado_r09{
  struct{
    unsigned REC_ADCDRI_I_N_CON		: 1;
    unsigned REC_ADCDRI_I_P_CON		: 5;
    unsigned PG_ADC_CLKGEN		: 1;
    unsigned REC_ADC_RESETN		: 1;
    unsigned REC_ADC_ISOL_LC		: 1;
    unsigned REC_ADC_CLK_TUNE		: 3;
    unsigned REC_ADC_EN_PURGE		: 1;
    unsigned REC_ADC_OFFSET_SELP		: 3;
    unsigned REC_ADC_OFFSET_SELN		: 3;
  };
  uint32_t as_int;
} ado_r09_t;
#define ADO_R09_DEFAULT {{0x0, 0x10, 0x1, 0x0, 0x1, 0x0, 0x1, 0x0, 0x0}}
#define ADO_R09_DEFAULT_AS_INT 0x001160
_Static_assert(sizeof(ado_r09_t) == 4, "Punned Structure Size");

// Register 0x0A
typedef union ado_r0A{
  struct{
    unsigned PG_SIG_V1P2_HP		: 1;
    unsigned HP_RST		: 1;
    unsigned DSM_IN		: 6;
    unsigned HP_DSM_EN_B		: 1;
    unsigned FAST_DIV_HP		: 2;
    unsigned IRQ_EN_HP		: 1;
    unsigned CD_HP_LNA_EN		: 1;
    unsigned CD_HP_PGA_EN		: 1;
    unsigned HP_CD_AUTO_EN		: 1;
    unsigned ISO_1P4_HP		: 1;
    unsigned PG_SIG_V1P2_FS		: 1;
    unsigned CLK_GATE		: 1;
    unsigned HP_F_SEL		: 1;
    unsigned ISO_1P4_FS		: 1;
  };
  uint32_t as_int;
} ado_r0A_t;
#define ADO_R0A_DEFAULT {{0x1, 0x1, 0x1F, 0x1, 0x3, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x0, 0x1, 0x1}}
#define ADO_R0A_DEFAULT_AS_INT 0x0D877F
_Static_assert(sizeof(ado_r0A_t) == 4, "Punned Structure Size");

// Register 0x0B
typedef union ado_r0B{
  struct{
    unsigned DIV1		: 14;
  };
  uint32_t as_int;
} ado_r0B_t;
#define ADO_R0B_DEFAULT {{0x0640}}
#define ADO_R0B_DEFAULT_AS_INT 0x000640
_Static_assert(sizeof(ado_r0B_t) == 4, "Punned Structure Size");

// Register 0x0C
typedef union ado_r0C{
  struct{
    unsigned DIV2		: 14;
  };
  uint32_t as_int;
} ado_r0C_t;
#define ADO_R0C_DEFAULT {{0x064A}}
#define ADO_R0C_DEFAULT_AS_INT 0x00064A
_Static_assert(sizeof(ado_r0C_t) == 4, "Punned Structure Size");

// Register 0x0D
// -- EMPTY --

// Register 0x0E
typedef union ado_r0E{
  struct{
    unsigned LC_CLK_RING		: 2;
    unsigned LC_CLK_DIV		: 2;
    unsigned WAKEUP_ON_PEND_REQ		: 1;
  };
  uint32_t as_int;
} ado_r0E_t;
#define ADO_R0E_DEFAULT {{0x2, 0x2, 0x0}}
#define ADO_R0E_DEFAULT_AS_INT 0x00000A
_Static_assert(sizeof(ado_r0E_t) == 4, "Punned Structure Size");

#endif // ADOV1A_RF_H