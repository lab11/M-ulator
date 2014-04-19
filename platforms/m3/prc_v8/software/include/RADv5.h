//*******************************************************************
//Author: ZhiYoong Foo
//Description: RADv5 Header File
//*******************************************************************

//Register 0x0 - 0x1F are used for CDC.

//Regsiter 20
typedef struct radv5_r20{
  unsigned RADIO_TUNE_POWER_1P2 : 5;
} radv5_r20_t;
#define RADv5_R20_DEFAULT {0x0}

//Regsiter 21
typedef struct radv5_r21{
  unsigned RADIO_TUNE_FREQ1_1P2 : 4;
} radv5_r21_t;
#define RADv5_R21_DEFAULT {0x0}

//Regsiter 22
typedef struct radv5_r22{
  unsigned RADIO_TUNE_FREQ2_1P2 : 4;
} radv5_r22_t;
#define RADv5_R22_DEFAULT {0x0}

//Regsiter 23
typedef struct radv5_r23{
  unsigned RADIO_EXT_CTRL_EN_1P2 : 1;
} radv5_r23_t;
#define RADv5_R23_DEFAULT {0x1}

//Regsiter 24
typedef struct radv5_r24{
  unsigned RADIO_EXT_OSC_ENB_1P2 : 1;
} radv5_r24_t;
#define RADv5_R24_DEFAULT {0x1}

//Regsiter 25
typedef struct radv5_r25{
  unsigned RADIO_TUNE_TX_TIME_1P2 : 3;
} radv5_r25_t;
#define RADv5_R25_DEFAULT {0x0}

//Regsiter 26
typedef struct radv5_r26{
  unsigned RADIO_TUNE_CURRENT_LIMITER_1P2 : 6;
} radv5_r26_t;
#define RADv5_R26_DEFAULT {0x3F}

//Regsiter 27
typedef struct radv5_r27{
  unsigned RADIO_DATA_1P2 : 1;
} radv5_r27_t;
#define RADv5_R27_DEFAULT {0x0}

//Regsiter 28
typedef struct radv5_r28{
  unsigned RADIO_EXTRA_0 : 4;
} radv5_r28_t;
#define RADv5_R28_DEFAULT {0x0}

//Regsiter 29
typedef struct radv5_r29{
  unsigned RADIO_EXTRA_1 : 4;
} radv5_r29_t;
#define RADv5_R29_DEFAULT {0xF}

//Regsiter 2A
typedef struct radv5_r2A{
  unsigned LC_CLK_CONF : 4;
} radv5_r2a_t;
#define RADv5_R2A_DEFAULT {0x9}
