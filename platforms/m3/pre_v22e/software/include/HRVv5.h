//*******************************************************************
//Author: Gyouho Kim
//Description: HRVv5 Header File
//*******************************************************************

//Register 0x0
typedef union hrvv5_r0{
  struct{ // LSB First
    unsigned HRV_TOP_CONV_RATIO	: 4;
  };
  uint32_t as_int;
} hrvv5_r0_t;
#define HRVv5_R0_DEFAULT {{0xE}} // LSB is left
_Static_assert(sizeof(hrvv5_r0_t) == 4, "Punned Structure Size");

//Register 1
typedef union hrvv5_r1{
  struct{
    unsigned LRC_CLK_CONF: 4;
    unsigned HRV_CNT_CNT_ENABLE: 1;
    unsigned MBUS_REPLY_ADDRESS_HRV_CNT: 8;
    unsigned HRV_CNT_CNT_IRQ: 1;
  }; 
  uint32_t as_int;
} hrvv5_r1_t;
#define HRVv5_R1_DEFAULT {0x9, 0, 0x14, 0}
_Static_assert(sizeof(hrvv5_r1_t) == 4, "Punned Structure Size");

typedef union hrvv5_r3{
  struct{ // LSB First
    unsigned HRV_CNT_CNT_THRESHOLD	: 24;
  };
  uint32_t as_int;
} hrvv5_r3_t;
#define HRVv5_R3_DEFAULT {{0xFFFFFF}} // LSB is left
_Static_assert(sizeof(hrvv5_r3_t) == 4, "Punned Structure Size");

typedef union hrvv5_r4{
  struct{ // LSB First
    unsigned VOLTAGE_CLAMP_V_TUNE	: 6;
    unsigned VOLTAGE_CLAMP_I_TUNE	: 5;
  };
  uint32_t as_int;
} hrvv5_r4_t;
#define HRVv5_R4_DEFAULT {{0x13, 0x7}} // LSB is left
_Static_assert(sizeof(hrvv5_r4_t) == 4, "Punned Structure Size");

// Declare the structs
extern volatile hrvv5_r0_t hrvv5_r0;
extern volatile hrvv5_r1_t hrvv5_r1;
extern volatile hrvv5_r3_t hrvv5_r3;
extern volatile hrvv5_r4_t hrvv5_r4;
