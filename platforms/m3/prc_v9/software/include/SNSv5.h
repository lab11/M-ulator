//*******************************************************************
//Author: Gyouho Kim
//Description: SNSv5 Header File
//*******************************************************************

//Register 0x0
typedef union snsv5_r0{
  struct{ // LSB First
    unsigned CDCW_IRQ_EN	: 1;
    unsigned CDCW_MODE_PAR	: 1;
    unsigned CDCW_MODE_REF	: 1;
    unsigned CDCW_CTRL_DIV	: 2;
    unsigned CDCW_CTRL_RING	: 2;
    unsigned CDCW_ENABLE 	: 1;
    unsigned CDCW_RESETn 	: 1;
    unsigned CDCW_ISO		: 1;
    unsigned CDCW_PG_VBAT 	: 1;
    unsigned CDCW_PG_V1P2 	: 1;
    unsigned CDCW_PG_VLDO 	: 1;
  };
  uint32_t as_int;
} snsv5_r0_t;
#define SNSv5_R0_DEFAULT {{0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1}} 
_Static_assert(sizeof(snsv5_r0_t) == 4, "Punned Structure Size");

//Register 1
typedef union snsv5_r1{
  struct{
    unsigned CDCW_N_CYCLE_SINGLE	: 10;
    unsigned CDCW_N_INIT_CLK		: 10;
  }; 
  uint32_t as_int;
} snsv5_r1_t;
#define SNSv5_R1_DEFAULT {0x80, 0x8}
_Static_assert(sizeof(snsv5_r1_t) == 4, "Punned Structure Size");

//Register 2
typedef union snsv5_r2{
  struct{
    unsigned CDCW_T_CHARGE		: 10;
    unsigned CDCW_N_CYCLE_SET	: 10;
  }; 
  uint32_t as_int;
} snsv5_r2_t;
#define SNSv5_R2_DEFAULT {0x10, 0x80}
_Static_assert(sizeof(snsv5_r2_t) == 4, "Punned Structure Size");

//Register 3-13
//Read Only

//Register 14
typedef union snsv5_r14{
  struct{
    unsigned TEMP_SENSOR_R_bmod 	: 4;
    unsigned TEMP_SENSOR_R_tmod		: 4;
    unsigned TEMP_SENSOR_P_bmod		: 4;
    unsigned TEMP_SENSOR_P_tmod		: 4;
    unsigned TEMP_SENSOR_ENABLEb	: 1;
    unsigned TEMP_SENSOR_DELAY_SEL	: 3;
  };
  uint32_t as_int;
} snsv5_r14_t;
#define SNSv5_R14_DEFAULT {0x0, 0xF, 0x0, 0xF, 0x1, 0x2}
_Static_assert(sizeof(snsv5_r14_t) == 4, "Punned Structure Size");

//Register 15
typedef union snsv5_r15{
  struct{
    unsigned TEMP_SENSOR_SEL_CT 	: 3;
    unsigned TEMP_SENSOR_AMP_BIAS	: 3;
    unsigned TEMP_SENSOR_R_REF		: 8;
    unsigned TEMP_SENSOR_R_PTAT		: 8;
  };
  uint32_t as_int;
} snsv5_r15_t;
#define SNSv5_R15_DEFAULT {0x3, 0x2, 0x7F, 0x8F}
_Static_assert(sizeof(snsv5_r15_t) == 4, "Punned Structure Size");

//Register 16
//Read Only


//Register 17
typedef union snsv5_r17{
  struct{
    unsigned MBUS_REPLY_ADDRESS_TEMP_SENSOR	: 8;
    unsigned MBUS_REPLY_ADDRESS_CDC		: 8;
    unsigned MBUS_REPLY_ADDRESS_SAR_ADC		: 8;
  };
  uint32_t as_int;
} snsv5_r17_t;
#define SNSv5_R17_DEFAULT {0x14, 0x15, 0x16}
_Static_assert(sizeof(snsv5_r17_t) == 4, "Punned Structure Size");

//Register 18
typedef union snsv5_r18{
  struct{ // LSB first
    unsigned CDC_LDO_CDC_LDO_ENB        : 1;
    unsigned CDC_LDO_CDC_LDO_DLY_ENB    : 1;
    unsigned CDC_LDO_CDC_VREF_MUX_SEL   : 2;
    unsigned CDC_LDO_CDC_VREF_SEL       : 6;
    unsigned CDC_LDO_CDC_CURRENT_2X     : 1;
    unsigned ADC_LDO_ADC_LDO_ENB        : 1;
    unsigned ADC_LDO_ADC_LDO_DLY_ENB    : 1;
    unsigned ADC_LDO_ADC_VREF_MUX_SEL   : 2;
    unsigned ADC_LDO_ADC_VREF_SEL       : 6;
    unsigned ADC_LDO_ADC_CURRENT_2X     : 1;
  };
  uint32_t as_int;
} snsv5_r18_t;
//#define SNSv5_R18_DEFAULT {{0x1, 0x1, 0x2, 0x04, 0x1, 0x1, 0x2, 0x04, 0x9}}
_Static_assert(sizeof(snsv5_r18_t) == 4, "Punned Structure Size");

//Register 19
typedef union snsv5_r19{
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
} snsv5_r19_t;
//#define SNSv5_R19_DEFAULT {{0x1, 0x1, 0x0, 0x0, 0x1, 0x0, 0x0, 0x6, 0x1, 0x0}}
_Static_assert(sizeof(snsv5_r19_t) == 4, "Punned Structure Size");
  
//Register 20
typedef union snsv5_r20{
  struct{
    unsigned SAR_ADC_COMP_CAP_L         : 5;
    unsigned SAR_ADC_COMP_CAP_R         : 5;
    unsigned SAR_ADC_FAST_DIV_EN        : 1;
  };
  uint32_t as_int;
} snsv5_r20_t;
//#define SNSv5_R20_DEFAULT {{0x1F, 0x1F, 0x0}}
_Static_assert(sizeof(snsv5_r20_t) == 4, "Punned Structure Size");

//Register 21
//Read Only


//Register 22
typedef union snsv5_r22{
  struct{
    unsigned LC_CLK_DIV		: 2;
    unsigned LC_CLK_RING    : 2;
  };
  uint32_t as_int;
} snsv5_r22_t;
//#define SNSv5_R22_DEFAULT {{0x2, 0x1}}
_Static_assert(sizeof(snsv5_r22_t) == 4, "Punned Structure Size");


// Declare the structs
extern volatile snsv5_r0_t snsv5_r0;
extern volatile snsv5_r1_t snsv5_r1;
extern volatile snsv5_r2_t snsv5_r2;
extern volatile snsv5_r14_t snsv5_r14;
extern volatile snsv5_r15_t snsv5_r15;
extern volatile snsv5_r17_t snsv5_r17;
extern volatile snsv5_r18_t snsv5_r18;
extern volatile snsv5_r19_t snsv5_r19;
extern volatile snsv5_r20_t snsv5_r20;
extern volatile snsv5_r22_t snsv5_r22;
