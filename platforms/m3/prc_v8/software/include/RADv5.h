//*******************************************************************
//Author: ZhiYoong Foo
//Description: RADv5 Header File
//*******************************************************************

//Register 0x0 - 0x1F are used for CDC.

//Regsiter 20
typedef union radv5_r20{
  struct{
    unsigned RADIO_TUNE_POWER_1P2 : 5;
  };
  uint32_t as_int;
} radv5_r20_t;
#define RADv5_R20_DEFAULT {{0x0}}
_Static_assert(sizeof(radv5_r20_t) == 4, "Punned Structure Size");

//Regsiter 21
typedef union radv5_r21{
  struct{
    unsigned RADIO_TUNE_FREQ1_1P2 : 4;
  };
  uint32_t as_int;
} radv5_r21_t;
#define RADv5_R21_DEFAULT {{0x0}}
_Static_assert(sizeof(radv5_r21_t) == 4, "Punned Structure Size");

//Regsiter 22
typedef union radv5_r22{
  struct{
    unsigned RADIO_TUNE_FREQ2_1P2 : 4;
  };
  uint32_t as_int;
} radv5_r22_t;
#define RADv5_R22_DEFAULT {{0x0}}
_Static_assert(sizeof(radv5_r22_t) == 4, "Punned Structure Size");

//Regsiter 23
typedef union radv5_r23{
  struct{
    unsigned RADIO_EXT_CTRL_EN_1P2 : 1;
  };
  uint32_t as_int;
} radv5_r23_t;
#define RADv5_R23_DEFAULT {{0x1}}
_Static_assert(sizeof(radv5_r23_t) == 4, "Punned Structure Size");

//Regsiter 24
typedef union radv5_r24{
  struct{
    unsigned RADIO_EXT_OSC_ENB_1P2 : 1;
  };
  uint32_t as_int;
} radv5_r24_t;
#define RADv5_R24_DEFAULT {{0x1}}
_Static_assert(sizeof(radv5_r24_t) == 4, "Punned Structure Size");

//Regsiter 25
typedef union radv5_r25{
  struct{
    unsigned RADIO_TUNE_TX_TIME_1P2 : 3;
  };
  uint32_t as_int;
} radv5_r25_t;
#define RADv5_R25_DEFAULT {{0x0}}
_Static_assert(sizeof(radv5_r25_t) == 4, "Punned Structure Size");

//Regsiter 26
typedef union radv5_r26{
  struct{
    unsigned RADIO_TUNE_CURRENT_LIMITER_1P2 : 6;
  };
  uint32_t as_int;
} radv5_r26_t;
#define RADv5_R26_DEFAULT {{0x3F}}
_Static_assert(sizeof(radv5_r26_t) == 4, "Punned Structure Size");

//Regsiter 27
typedef union radv5_r27{
  struct{
    unsigned RADIO_DATA_1P2 : 1;
  };
  uint32_t as_int;
} radv5_r27_t;
#define RADv5_R27_DEFAULT {{0x0}}
_Static_assert(sizeof(radv5_r27_t) == 4, "Punned Structure Size");

//Regsiter 28
typedef union radv5_r28{
  struct{
    unsigned RADIO_EXTRA_0 : 4;
  };
  uint32_t as_int;
} radv5_r28_t;
#define RADv5_R28_DEFAULT {{0x0}}
_Static_assert(sizeof(radv5_r28_t) == 4, "Punned Structure Size");

//Regsiter 29
typedef union radv5_r29{
  struct{
    unsigned RADIO_EXTRA_1 : 4;
  };
  uint32_t as_int;
} radv5_r29_t;
#define RADv5_R29_DEFAULT {{0xF}}
_Static_assert(sizeof(radv5_r29_t) == 4, "Punned Structure Size");

//Regsiter 2A
typedef union radv5_r2A{
  struct{
    unsigned LC_CLK_CONF : 4;
  };
  uint32_t as_int;
} radv5_r2a_t;
#define RADv5_R2A_DEFAULT {{0x9}}
_Static_assert(sizeof(radv5_r2a_t) == 4, "Punned Structure Size");
