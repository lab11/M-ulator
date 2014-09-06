//*******************************************************************
//Author: Yoonmyung Lee
//Description: SNSv3 Header File
//*******************************************************************

//Register 0x0
typedef union snsv3_r0{
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
} snsv3_r0_t;
#define SNSv3_R0_DEFAULT {{0x0, 0x7, 0x0, 0x1, 0x0, 0x1, 0x1, 0x4, 0x0, 0x0, 0x6, 0x6, 0x1, 0x0}}
_Static_assert(sizeof(snsv3_r0_t) == 4, "Punned Structure Size");

//Register 0x1
typedef union snsv3_r1{
  struct{
    unsigned CDC_S_period	: 9;
    unsigned CDC_R_period	: 9;
    unsigned CDC_CR_ext 	: 3;
  }; 
  uint32_t as_int;
} snsv3_r1_t;
#define SNSv3_R1_DEFAULT {{0x20, 0xF, 0x0}}
_Static_assert(sizeof(snsv3_r1_t) == 4, "Punned Structure Size");

//Register 0x2
//Read Only

//Register 0x3
typedef union snsv3_r3{
  struct{
    unsigned TEMP_SENSOR_R_bmod 	: 4;
    unsigned TEMP_SENSOR_R_tmod		: 4;
    unsigned TEMP_SENSOR_P_bmod		: 4;
    unsigned TEMP_SENSOR_P_tmod		: 4;
    unsigned TEMP_SENSOR_ENABLEb	: 1;
    unsigned TEMP_SENSOR_DELAY_SEL	: 3;
  };
  uint32_t as_int;
} snsv3_r3_t;
#define SNSv3_R3_DEFAULT {0x0, 0xF, 0x0, 0xF, 0x1, 0x2}
_Static_assert(sizeof(snsv3_r3_t) == 4, "Punned Structure Size");

//Register 0x4
typedef union snsv3_r4{
  struct{
    unsigned TEMP_SENSOR_SEL_CT 	: 3;
    unsigned TEMP_SENSOR_AMP_BIAS	: 3;
    unsigned TEMP_SENSOR_R_REF		: 8;
    unsigned TEMP_SENSOR_R_PTAT		: 8;
  };
  uint32_t as_int;
} snsv3_r4_t;
#define SNSv3_R4_DEFAULT {0x3, 0x2, 0x7F, 0x8F}
_Static_assert(sizeof(snsv3_r4_t) == 4, "Punned Structure Size");

//Register 0x5
//Read Only

//Register 0x6
typedef union snsv3_r6{
  struct{
    unsigned MBUS_REPLY_ADDRESS_TEMP_SENSOR	: 8;
    unsigned MBUS_REPLY_ADDRESS_CDC		: 8;
    unsigned MBUS_REPLY_ADDRESS_SAR_ADC		: 8;
  };
  uint32_t as_int;
} snsv3_r6_t;
#define SNSv3_R6_DEFAULT {0x14, 0x15, 0x16}
_Static_assert(sizeof(snsv3_r6_t) == 4, "Punned Structure Size");

//Register 0x7
typedef union snsv3_r7{
  struct{
    unsigned CDC_LDO_CDC_LDO_ENB        : 1;
    unsigned CDC_LDO_CDC_LDO_DLY_ENB    : 1;
    unsigned CDC_LDO_CDC_VREF_MUX_SEL   : 2;
    unsigned CDC_LDO_CDC_VREF_SEL       : 6;
    unsigned ADC_LDO_ADC_LDO_ENB        : 1;
    unsigned ADC_LDO_ADC_LDO_DLY_ENB    : 1;
    unsigned ADC_LDO_ADC_VREF_MUX_SEL   : 2;
    unsigned ADC_LDO_ADC_VREF_SEL       : 6;
    unsigned LC_CLK_CONF                : 4;
  };
  uint32_t as_int;
} snsv3_r7_t;
#define SNSv3_R7_DEFAULT {{0x1, 0x1, 0x2, 0x04, 0x1, 0x1, 0x2, 0x04, 0x9}}
_Static_assert(sizeof(snsv3_r7_t) == 4, "Punned Structure Size");

//Register 0x8
typedef union snsv3_r8{
  struct{
    unsigned SAR_ADC_ADC_DOUT_ISO       : 1;
    unsigned SAR_ADC_OSC_ENB            : 1;
    unsigned SAR_ADC_START_CONV         : 1;
    unsigned SAR_ADC_BYPASS_NUMBER      : 4;
    unsigned SAR_ADC_SINGLE_CONV        : 1;
    unsigned SAR_ADC_SIGNAL_SEL         : 2;
    unsigned SAR_ADC_EXT_SIGNAL_SEL     : 2;
    unsigned SAR_ADC_OSC_SEL            : 3;
    unsigned SAR_ADC_OSC_DIV            : 2;
    unsigned SAR_ADC_DONE               : 1;
  };
  uint32_t as_int;
} snsv3_r8_t;
#define SNSv3_R8_DEFAULT {{0x1, 0x1, 0x0, 0x0, 0x1, 0x0, 0x0, 0x6, 0x1, 0x0}}
_Static_assert(sizeof(snsv3_r8_t) == 4, "Punned Structure Size");
  
//Register 0x9
typedef union snsv3_r9{
  struct{
    unsigned SAR_ADC_COMP_CAP_L         : 5;
    unsigned SAR_ADC_COMP_CAP_R         : 5;
    unsigned SAR_ADC_FAST_DIV_EN        : 1;
  };
  uint32_t as_int;
} snsv3_r9_t;
#define SNSv3_R9_DEFAULT {{0x1F, 0x1F, 0x0}}
_Static_assert(sizeof(snsv3_r9_t) == 4, "Punned Structure Size");

//Register 0xA
//Read Only


