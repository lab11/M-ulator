//*******************************************************************
//Author: ZhiYoong Foo
//Description: SNSv2 Header File
//*******************************************************************

//Register 0
typedef union snsv2_r0{
  struct{
    unsigned CDC_ext_select_CR 	: 1;
    unsigned CDC_max_select 	: 3;
    unsigned CDC_ext_max 	: 1;
    unsigned CDC_CR_fixB 	: 1;
    unsigned CDC_ext_clk 	: 1;
    unsigned CDC_outck_in 	: 1;
    unsigned CDC_on_adap 	: 1;
    unsigned CDC_s_recycle 	: 3;
    unsigned CDC_Td 		: 2;
    unsigned CDC_OP_on 		: 1;
    unsigned CDC_Bias_2nd 	: 3;
    unsigned CDC_Bias_1st 	: 3;
    unsigned CDC_EXT_RESET 	: 1;
    unsigned CDC_CLK 		: 1;
  };
  uint32_t as_int;
} snsv2_r0_t;
#define SNSv2_R0_DEFAULT {{0x0, 0x7, 0x0, 0x1, 0x0, 0x1, 0x1, 0x4, 0x0, 0x0, 0x6, 0x6, 0x1, 0x0}}
_Static_assert(sizeof(snsv2_r0_t) == 4, "Punned Structure Size");

//Register 1
typedef union snsv2_r1{
  struct{
    unsigned CDC_S_period	: 9;
    unsigned CDC_R_period	: 9;
    unsigned CDC_CR_ext 	: 3;
  }; 
  uint32_t as_int;
} snsv2_r1_t;
#define SNSv2_R1_DEFAULT {{0x20, 0xF, 0x0}}
_Static_assert(sizeof(snsv2_r1_t) == 4, "Punned Structure Size");

//Register 2
//Read Only

//Register 3
typedef union snsv2_r3{
  struct{
    unsigned TEMP_SENSOR_R_bmod 	: 4;
    unsigned TEMP_SENSOR_R_tmod		: 4;
    unsigned TEMP_SENSOR_P_bmod		: 4;
    unsigned TEMP_SENSOR_P_tmod		: 4;
    unsigned TEMP_SENSOR_ENABLEb	: 1;
    unsigned TEMP_SENSOR_DELAY_SEL	: 3;
  };
  uint32_t as_int;
} snsv2_r3_t;
#define SNSv2_R3_DEFAULT {0x0, 0xF, 0x0, 0xF, 0x1, 0x2}
_Static_assert(sizeof(snsv2_r3_t) == 4, "Punned Structure Size");

//Register 4
typedef union snsv2_r4{
  struct{
    unsigned TEMP_SENSOR_SEL_CT 	: 3;
    unsigned TEMP_SENSOR_AMP_BIAS	: 3;
    unsigned TEMP_SENSOR_R_REF		: 8;
    unsigned TEMP_SENSOR_R_PTAT		: 8;
  };
  uint32_t as_int;
} snsv2_r4_t;
#define SNSv2_R4_DEFAULT {0x3, 0x2, 0x7F, 0x8F}
_Static_assert(sizeof(snsv2_r4_t) == 4, "Punned Structure Size");

//Register 5
//Read Only

//Register 6
typedef union snsv2_r6{
  struct{
    unsigned MBUS_REPLY_ADDRESS_TEMP_SENSOR	: 8;
    unsigned MBUS_REPLY_ADDRESS_CDC		: 8;
    unsigned LC_CLK_CONF			: 4;
  };
  uint32_t as_int;
} snsv2_r6_t;
#define SNSv2_R6_DEFAULT {0x14, 0x15, 0x9}
_Static_assert(sizeof(snsv2_r6_t) == 4, "Punned Structure Size");

//Register 7
typedef union snsv2_r7{
  struct{
    unsigned REFGENREV_CDC_OFF	: 1;
    unsigned REFGENREV_SEL	: 3;
  };
  uint32_t as_int;
} snsv2_r7_t;
#define SNSv2_R7_DEFAULT {{0x1, 0x3}}
_Static_assert(sizeof(snsv2_r7_t) == 4, "Punned Structure Size");
