//*******************************************************************
//Author: Gyouho Kim, ZhiYoong Foo
//Description: RADv7 Header File
//*******************************************************************

//Regsiter 0
typedef union radv7_r0{
  struct{ // LSB First
    unsigned RADIO_TUNE_POWER	: 5;
    unsigned RADIO_TUNE_FREQ1	: 4;
    unsigned RADIO_TUNE_FREQ2	: 4;
    unsigned RADIO_EXT_CTRL_EN	: 1;
    unsigned RADIO_EXT_OSC_ENB	: 1;
    unsigned RADIO_TUNE_TX_TIME : 3;
    unsigned RADIO_TUNE_CURRENT_LIMITER : 6;
  };
  uint32_t as_int;
} radv7_r0_t;
#define RADv7_R0_DEFAULT {{0x1F, 0, 0, 0, 1, 5, 0x1F}} // LSB is left
_Static_assert(sizeof(radv7_r0_t) == 4, "Punned Structure Size");

//Regsiter 1
typedef union radv7_r1{
  struct{
    unsigned SCRO_I_LEVEL_SELB		: 7;
    unsigned SCRO_I_TC_SEL			: 3;
    unsigned SCRO_AMP_I_LEVEL_SEL	: 6;
    unsigned SCRO_AMP_I_TC_SEL		: 3;
    unsigned SCRO_FREQ_DIV			: 2;
  };
  uint32_t as_int;
} radv7_r1_t;
#define RADv7_R1_DEFAULT {{0x6F, 1, 2, 2, 3}}
_Static_assert(sizeof(radv7_r1_t) == 4, "Punned Structure Size");

//Regsiter 2
typedef union radv7_r2{
  struct{
    unsigned SCRO_R_SEL			: 11;
    unsigned SCRO_VINHEE_SEL	: 3;
    unsigned SCRO_DIV_SEL		: 4;
    unsigned SCRO_RESET			: 1;
    unsigned SCRO_ENABLE		: 1;
  };
  uint32_t as_int;
} radv7_r2_t;
#define RADv7_R2_DEFAULT {{0x34, 2, 4, 1, 0}}
_Static_assert(sizeof(radv7_r2_t) == 4, "Punned Structure Size");

//Regsiter 3
typedef union radv7_r3{
  struct{
    unsigned RAD_FSM_DATA: 24;
  };
  uint32_t as_int;
} radv7_r3_t;
#define RADv7_R3_DEFAULT {{0x0}}
_Static_assert(sizeof(radv7_r3_t) == 4, "Punned Structure Size");

//Regsiter 4
typedef union radv7_r4{
  struct{
    unsigned RAD_FSM_DATA: 24;
  };
  uint32_t as_int;
} radv7_r4_t;
#define RADv7_R4_DEFAULT {{0x0}}
_Static_assert(sizeof(radv7_r4_t) == 4, "Punned Structure Size");

//Regsiter 5
typedef union radv7_r5{
  struct{
    unsigned RAD_FSM_DATA: 24;
  };
  uint32_t as_int;
} radv7_r5_t;
#define RADv7_R5_DEFAULT {{0x0}}
_Static_assert(sizeof(radv7_r5_t) == 4, "Punned Structure Size");

//Regsiter 6
typedef union radv7_r6{
  struct{
    unsigned RAD_FSM_H_LEN: 5;
    unsigned RAD_FSM_D_LEN: 7;
    unsigned RAD_FSM_C_LEN: 7;
  };
  uint32_t as_int;
} radv7_r6_t;
#define RADv7_R6_DEFAULT {{0x1F, 0x7F, 0x7F}}
_Static_assert(sizeof(radv7_r6_t) == 4, "Punned Structure Size");

//Regsiter 7
typedef union radv7_r7{
  struct{
    unsigned RAD_FSM_SEED: 6;
  };
  uint32_t as_int;
} radv7_r7_t;
#define RADv7_R7_DEFAULT {{1}}
_Static_assert(sizeof(radv7_r7_t) == 4, "Punned Structure Size");

//Regsiter 8
typedef union radv7_r8{
  struct{
    unsigned RAD_FSM_SLEEP:			1;
    unsigned RAD_FSM_ISOLATE:		1;
    unsigned RAD_FSM_RESETn:		1;
    unsigned RAD_FSM_ENABLE:		1;
    unsigned RAD_FSM_IRQ_EN:		1;
    unsigned RAD_FSM_IRQ_REPLY_ADDRESS:	8;
    unsigned RAD_FSM_FSM_STATE:		4;
    unsigned RAD_FSM_LFSR_STATE:	6;
  };
  uint32_t as_int;
} radv7_r8_t;
#define RADv7_R8_DEFAULT {{1,1,0,0,1,0x14,0,0}}
_Static_assert(sizeof(radv7_r8_t) == 4, "Punned Structure Size");


//Regsiter 9
typedef union radv7_r9{
  struct{
    unsigned LC_CLK_RING : 2;
    unsigned LC_CLK_DIV  : 2;
  };
  uint32_t as_int;
} radv7_r9_t;
#define RADv7_R9_DEFAULT {{1,2}}
_Static_assert(sizeof(radv7_r9_t) == 4, "Punned Structure Size");

// Declare the structs
extern volatile radv7_r0_t radv7_r0;
extern volatile radv7_r1_t radv7_r1;
extern volatile radv7_r2_t radv7_r2;
extern volatile radv7_r3_t radv7_r3;
extern volatile radv7_r4_t radv7_r4;
extern volatile radv7_r5_t radv7_r5;
extern volatile radv7_r6_t radv7_r6;
extern volatile radv7_r7_t radv7_r7;
extern volatile radv7_r8_t radv7_r8;
extern volatile radv7_r9_t radv7_r9;

