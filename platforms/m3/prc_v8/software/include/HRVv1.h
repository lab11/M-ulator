//*******************************************************************
//Author: ZhiYoong Foo
//Description: HRVv1 Header File
//*******************************************************************

//Register 0
typedef struct hrvv1_r0{
  unsigned HRV_TOP_CONV_RATIO 	: 4;
} hrvv1_r0_t;
#define HRVv1_R0_DEFAULT {0xE}

//Register 1
typedef struct hrvv1_r1{
  unsigned HRV_CNT_CNT_ENABLE 	: 1;
  unsigned LRC_CLK_CONF		: 4;
} hrvv1_r1_t;
#define HRVv1_R1_DEFAULT {0x0, 0x9}

//Register 2
//Read Only
