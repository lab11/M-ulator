//************************************************************
// Desciption: HRVv9 Register File Header File
//      Generated by genRF (Version 1.30) 04/29/2019 12:56:03
//************************************************************

#ifndef HRVV9_RF_H
#define HRVV9_RF_H

// Register 0x00
typedef union hrvv9_r00{
  struct{
    unsigned HRV_TOP_CONV_RATIO		: 4;
  };
  uint32_t as_int;
} hrvv9_r00_t;
#define HRVv9_R00_DEFAULT {{0x9}}
_Static_assert(sizeof(hrvv9_r00_t) == 4, "Punned Structure Size");

// Register 0x01
typedef union hrvv9_r01{
  struct{
    unsigned LC_CLK_RING		: 2;
    unsigned LC_CLK_DIV		: 2;
    unsigned HRV_CNT_CNT_ENABLE		: 1;
    // unsigned HRV_CNT_CNT_IRQ		: 1; // READ-ONLY
    unsigned LC_CLK_LOAD		: 1;
    unsigned MBC_WAKEUP_ON_PEND_REQ		: 1;
  };
  uint32_t as_int;
} hrvv9_r01_t;
#define HRVv9_R01_DEFAULT {{0x1, 0x2, 0x0, 0x0, 0x0, 0x0}}
_Static_assert(sizeof(hrvv9_r01_t) == 4, "Punned Structure Size");

// Register 0x02
// -- READ-ONLY --

// Register 0x03
typedef union hrvv9_r03{
  struct{
    unsigned HRV_CNT_CNT_THRESHOLD		: 24;
  };
  uint32_t as_int;
} hrvv9_r03_t;
#define HRVv9_R03_DEFAULT {{0xFFFFFF}}
_Static_assert(sizeof(hrvv9_r03_t) == 4, "Punned Structure Size");

// Register 0x04
typedef union hrvv9_r04{
  struct{
    unsigned HRV_CNT_IRQ_PACKET		: 24;
  };
  uint32_t as_int;
} hrvv9_r04_t;
#define HRVv9_R04_DEFAULT {{0x001400}}
_Static_assert(sizeof(hrvv9_r04_t) == 4, "Punned Structure Size");

#endif // HRVV9_RF_H