//*******************************************************************
//Author: ZhiYoong Foo
//Description: HRVv1 Header File
//*******************************************************************

//Register 0
typedef union hrvv1_r0{
  struct{
    unsigned HRV_TOP_CONV_RATIO 	: 4;
  };
  uint32_t as_int;
} hrvv1_r0_t;
#define HRVv1_R0_DEFAULT {0xE}
_Static_assert(sizeof(hrvv1_r0_t) == 4, "Punned Structure Size");

//Register 1
typedef union hrvv1_r1{
  struct{
    unsigned HRV_CNT_CNT_ENABLE 	: 1;
    unsigned LRC_CLK_CONF		: 4;
  };
  uint32_t as_int;
} hrvv1_r1_t;
#define HRVv1_R1_DEFAULT {0x0, 0x9}
_Static_assert(sizeof(hrvv1_r1_t) == 4, "Punned Structure Size");

//Register 2
//Read Only
