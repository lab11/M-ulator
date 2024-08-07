//************************************************************
// Desciption: SRRv5 Register File Header File
//      Generated by genRF (Version 1.2) 06/10/2018 12:01:29
//************************************************************

#ifndef SRRV5_RF_H
#define SRRV5_RF_H

// Register 0x00
typedef union srrv5_r00{
  struct{
    unsigned SRR_CL_EN		: 1;
    unsigned SRR_CL_CTRL		: 6;
    unsigned SRR_TRX_CAP_ANTP_TUNE		: 14;
  };
  uint32_t as_int;
} srrv5_r00_t;
#define SRRv5_R00_DEFAULT {{0x0, 0x08, 0x0000}}
_Static_assert(sizeof(srrv5_r00_t) == 4, "Punned Structure Size");

// Register 0x01
typedef union srrv5_r01{
  struct{
    unsigned SRR_TRX_CAP_ANTN_TUNE		: 14;
  };
  uint32_t as_int;
} srrv5_r01_t;
#define SRRv5_R01_DEFAULT {{0x0000}}
_Static_assert(sizeof(srrv5_r01_t) == 4, "Punned Structure Size");

// Register 0x02
typedef union srrv5_r02{
  struct{
    unsigned SRR_TX_BIAS_TUNE		: 13;
    unsigned SRR_TX_EN_OW		: 1;
    unsigned SRR_TX_PULSE_FINE		: 1;
    unsigned SRR_TX_PULSE_FINE_TUNE		: 4;
  };
  uint32_t as_int;
} srrv5_r02_t;
#define SRRv5_R02_DEFAULT {{0x0030, 0x0, 0x0, 0x7}}
_Static_assert(sizeof(srrv5_r02_t) == 4, "Punned Structure Size");

// Register 0x03
typedef union srrv5_r03{
  struct{
    unsigned SRR_RX_BIAS_TUNE		: 13;
    unsigned SRR_RX_EN_OW		: 1;
    unsigned SRR_REC_RST_OW		: 1;
    unsigned SRR_RX_SAMPLE_CAP		: 3;
    unsigned NOT_DEFINED_3_18		: 1;
    unsigned NOT_DEFINED_3_19		: 1;
    unsigned SRR_TRX_ISOLATEN		: 1;
  };
  uint32_t as_int;
} srrv5_r03_t;
#define SRRv5_R03_DEFAULT {{0x0801, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}}
_Static_assert(sizeof(srrv5_r03_t) == 4, "Punned Structure Size");

// Register 0x04
typedef union srrv5_r04{
  struct{
    unsigned RO_EN_RO_V1P2		: 1;
    unsigned RO_EN_RO_V1P2_PRE		: 1;
    unsigned RO_EN_RO_LDO		: 1;
    unsigned RO_RESET		: 1;
    unsigned RO_ISOLATE_CLK		: 1;
    unsigned RO_EN_CLK		: 1;
    unsigned RO_SEL_EXT_CLK		: 1;
    unsigned RO_SEL_DIV		: 2;
    unsigned RO_EN_MONITOR		: 1;
    unsigned LDO_EN_LDO		: 1;
    unsigned LDO_EN_IREF		: 1;
    unsigned LDO_EN_VREF		: 1;
    unsigned LDO_SEL_VOUT		: 3;
    unsigned LDO_VREF_I_AMP		: 4;
  };
  uint32_t as_int;
} srrv5_r04_t;
#define SRRv5_R04_DEFAULT {{0x0, 0x0, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4, 0x0}}
_Static_assert(sizeof(srrv5_r04_t) == 4, "Punned Structure Size");

// Register 0x05
typedef union srrv5_r05{
  struct{
    unsigned RO_V_MIRRb		: 4;
    unsigned RO_I_MIRROR2		: 2;
    unsigned RO_I_MIRROR		: 2;
    unsigned RO_I_CMP		: 4;
    unsigned RO_I_BUF2		: 4;
    unsigned RO_I_BUF		: 4;
    unsigned RO_R_REF		: 4;
  };
  uint32_t as_int;
} srrv5_r05_t;
#define SRRv5_R05_DEFAULT {{0xD, 0x1, 0x1, 0x1, 0x0, 0x0, 0x8}}
_Static_assert(sizeof(srrv5_r05_t) == 4, "Punned Structure Size");

// Register 0x06
typedef union srrv5_r06{
  struct{
    unsigned RO_PDIFF		: 15;
  };
  uint32_t as_int;
} srrv5_r06_t;
#define SRRv5_R06_DEFAULT {{0x0080}}
_Static_assert(sizeof(srrv5_r06_t) == 4, "Punned Structure Size");

// Register 0x07
typedef union srrv5_r07{
  struct{
    unsigned RO_MOM		: 7;
    unsigned RO_MIM		: 7;
  };
  uint32_t as_int;
} srrv5_r07_t;
#define SRRv5_R07_DEFAULT {{0x1C, 0x1C}}
_Static_assert(sizeof(srrv5_r07_t) == 4, "Punned Structure Size");

// Register 0x08
typedef union srrv5_r08{
  struct{
    unsigned RO_POLY		: 24;
  };
  uint32_t as_int;
} srrv5_r08_t;
#define SRRv5_R08_DEFAULT {{0x000040}}
_Static_assert(sizeof(srrv5_r08_t) == 4, "Punned Structure Size");

// Register 0x09
typedef union srrv5_r09{
  struct{
    unsigned SRR_RAD_FSM_TX_DATA_0		: 24;
  };
  uint32_t as_int;
} srrv5_r09_t;
#define SRRv5_R09_DEFAULT {{0x000000}}
_Static_assert(sizeof(srrv5_r09_t) == 4, "Punned Structure Size");

// Register 0x0A
typedef union srrv5_r0A{
  struct{
    unsigned SRR_RAD_FSM_TX_DATA_1		: 24;
  };
  uint32_t as_int;
} srrv5_r0A_t;
#define SRRv5_R0A_DEFAULT {{0x000000}}
_Static_assert(sizeof(srrv5_r0A_t) == 4, "Punned Structure Size");

// Register 0x0B
typedef union srrv5_r0B{
  struct{
    unsigned SRR_RAD_FSM_TX_DATA_2		: 24;
  };
  uint32_t as_int;
} srrv5_r0B_t;
#define SRRv5_R0B_DEFAULT {{0x000000}}
_Static_assert(sizeof(srrv5_r0B_t) == 4, "Punned Structure Size");

// Register 0x0C
typedef union srrv5_r0C{
  struct{
    unsigned SRR_RAD_FSM_TX_DATA_3		: 24;
  };
  uint32_t as_int;
} srrv5_r0C_t;
#define SRRv5_R0C_DEFAULT {{0x000000}}
_Static_assert(sizeof(srrv5_r0C_t) == 4, "Punned Structure Size");

// Register 0x0D
typedef union srrv5_r0D{
  struct{
    unsigned SRR_RAD_FSM_TX_DATA_4		: 24;
  };
  uint32_t as_int;
} srrv5_r0D_t;
#define SRRv5_R0D_DEFAULT {{0x000000}}
_Static_assert(sizeof(srrv5_r0D_t) == 4, "Punned Structure Size");

// Register 0x0E
typedef union srrv5_r0E{
  struct{
    unsigned SRR_RAD_FSM_TX_DATA_5		: 24;
  };
  uint32_t as_int;
} srrv5_r0E_t;
#define SRRv5_R0E_DEFAULT {{0x000000}}
_Static_assert(sizeof(srrv5_r0E_t) == 4, "Punned Structure Size");

// Register 0x0F
typedef union srrv5_r0F{
  struct{
    unsigned SRR_RAD_FSM_TX_DATA_6		: 24;
  };
  uint32_t as_int;
} srrv5_r0F_t;
#define SRRv5_R0F_DEFAULT {{0x000000}}
_Static_assert(sizeof(srrv5_r0F_t) == 4, "Punned Structure Size");

// Register 0x10
typedef union srrv5_r10{
  struct{
    unsigned SRR_RAD_FSM_TX_DATA_7		: 24;
  };
  uint32_t as_int;
} srrv5_r10_t;
#define SRRv5_R10_DEFAULT {{0x000000}}
_Static_assert(sizeof(srrv5_r10_t) == 4, "Punned Structure Size");

// Register 0x11
typedef union srrv5_r11{
  struct{
    unsigned SRR_RAD_FSM_SLEEP		: 1;
    unsigned SRR_RAD_FSM_RSTN		: 1;
    unsigned SRR_RAD_FSM_EN		: 1;
    unsigned SRR_RAD_FSM_TX_H_LEN		: 5;
    unsigned SRR_RAD_FSM_TX_D_LEN		: 8;
  };
  uint32_t as_int;
} srrv5_r11_t;
#define SRRv5_R11_DEFAULT {{0x1, 0x0, 0x0, 0x1F, 0xC0}}
_Static_assert(sizeof(srrv5_r11_t) == 4, "Punned Structure Size");

// Register 0x12
typedef union srrv5_r12{
  struct{
    unsigned SRR_RAD_FSM_TX_PW_LEN		: 10;
    unsigned SRR_RAD_FSM_TX_PS_LEN		: 10;
    unsigned SRR_RAD_FSM_TX_PR_LEN		: 3;
  };
  uint32_t as_int;
} srrv5_r12_t;
#define SRRv5_R12_DEFAULT {{0x0F9, 0x1F3, 0x0}}
_Static_assert(sizeof(srrv5_r12_t) == 4, "Punned Structure Size");

// Register 0x13
typedef union srrv5_r13{
  struct{
    unsigned SRR_RAD_FSM_TX_C_LEN		: 15;
    unsigned SRR_RAD_FSM_SEED		: 6;
    unsigned SRR_RAD_FSM_TX_MODE		: 3;
  };
  uint32_t as_int;
} srrv5_r13_t;
#define SRRv5_R13_DEFAULT {{0x07CF, 0x01, 0x0}}
_Static_assert(sizeof(srrv5_r13_t) == 4, "Punned Structure Size");

// Register 0x14
typedef union srrv5_r14{
  struct{
    unsigned SRR_RAD_FSM_TX_CNST_LEN		: 2;
    unsigned SRR_RAD_FSM_GUARD_LEN		: 14;
    unsigned SRR_RAD_FSM_TX_POWERON_LEN		: 3;
    unsigned SRR_RAD_FSM_RX_POWERON_LEN		: 2;
    unsigned SRR_RAD_FSM_RX_SAMPLE_LEN		: 3;
  };
  uint32_t as_int;
} srrv5_r14_t;
#define SRRv5_R14_DEFAULT {{0x0, 0x0000, 0x4, 0x0, 0x0}}
_Static_assert(sizeof(srrv5_r14_t) == 4, "Punned Structure Size");

// Register 0x15
typedef union srrv5_r15{
  struct{
    unsigned SRR_RAD_FSM_RX_HDR_BITS		: 6;
    unsigned SRR_RAD_FSM_RX_HDR_TH		: 6;
    unsigned SRR_RAD_FSM_RX_DATA_BITS		: 7;
    unsigned SRR_EN_DIG_MONITOR		: 1;
    unsigned SRR_RAD_FSM_TX_HDR_CNST		: 4;
  };
  uint32_t as_int;
} srrv5_r15_t;
#define SRRv5_R15_DEFAULT {{0x08, 0x08, 0x20, 0x0, 0x8}}
_Static_assert(sizeof(srrv5_r15_t) == 4, "Punned Structure Size");

// Register 0x16
typedef union srrv5_r16{
  struct{
    unsigned SRR_DIG_MONITOR_SEL1		: 4;
    unsigned SRR_DIG_MONITOR_SEL2		: 4;
    unsigned SRR_DIG_MONITOR_SEL3		: 4;
  };
  uint32_t as_int;
} srrv5_r16_t;
#define SRRv5_R16_DEFAULT {{0x0, 0x0, 0x0}}
_Static_assert(sizeof(srrv5_r16_t) == 4, "Punned Structure Size");

// Register 0x17
// -- READ-ONLY --

// Register 0x18
// -- READ-ONLY --

// Register 0x19
// -- READ-ONLY --

// Register 0x1A
// -- READ-ONLY --

// Register 0x1B
// -- READ-ONLY --

// Register 0x1C
// -- READ-ONLY --

// Register 0x1D
// -- READ-ONLY --

// Register 0x1E
typedef union srrv5_r1E{
  struct{
    unsigned SRR_IRQ_REPLY_PACKET		: 24;
  };
  uint32_t as_int;
} srrv5_r1E_t;
#define SRRv5_R1E_DEFAULT {{0x001400}}
_Static_assert(sizeof(srrv5_r1E_t) == 4, "Punned Structure Size");

// Register 0x1F
typedef union srrv5_r1F{
  struct{
    unsigned LC_CLK_RING		: 2;
    unsigned LC_CLK_DIV		: 2;
    unsigned LC_CLK_LOAD		: 1;
    unsigned WAKEUP_ON_PEND_REQ		: 1;
  };
  uint32_t as_int;
} srrv5_r1F_t;
#define SRRv5_R1F_DEFAULT {{0x1, 0x2, 0x0, 0x0}}
_Static_assert(sizeof(srrv5_r1F_t) == 4, "Punned Structure Size");

#endif // SRRV5_RF_H