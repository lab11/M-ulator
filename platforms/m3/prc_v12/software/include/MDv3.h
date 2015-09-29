//*******************************************************************
//Author: Gyouho Kim
//Description: MDv3 Header File
//*******************************************************************

//Regsiter 0
typedef union mdv3_r0{
  struct{ // LSB First
    unsigned TAKE_IMAGE 	: 1;
    unsigned START_MD 		: 1;
    unsigned STOP_MD 		: 1;
    unsigned INT_TIME 		: 10;
    unsigned MD_INT_TIME 	: 11;
  };
  uint32_t as_int;
} mdv3_r0_t;
#define MDv3_R0_DEFAULT {{0, 0, 0, 5, 15}} // LSB is left
_Static_assert(sizeof(mdv3_r0_t) == 4, "Punned Structure Size");

//Regsiter 1
typedef union mdv3_r1{
  struct{
    unsigned MD_TEST_MODE	: 1;
    unsigned MD_LOWRES		: 1;
    unsigned MD_LOWRES_B	: 1;
    unsigned MD_TH_EN		: 1;
    unsigned MD_TH_CLEAR	: 1;
    unsigned MD_TH			: 7;
    unsigned MD_FLAG_TOPAD_SEL : 1;
    unsigned TEST_RESET_ADC	: 1;
    unsigned TEST_COL_EN	: 1;
    unsigned TEST_MODE_ADC	: 1;
    unsigned EXTRA_BIT		: 1;
  };
  uint32_t as_int;
} mdv3_r1_t;
#define MDv3_R1_DEFAULT {{0, 1, 0, 0, 0, 5, 1, 0, 0, 0, 0}}
_Static_assert(sizeof(mdv3_r1_t) == 4, "Punned Structure Size");

//Regsiter 2
typedef union mdv3_r2{
  struct{
    unsigned MD_RESULTS_MASK	: 10;
    unsigned CDS_OFF			: 1;
    unsigned EN_ALL_EDGE		: 1;
    unsigned START_DELAY		: 7;
    unsigned SLEEP_ADC			: 1;
    unsigned PRESLEEP_ADC		: 1;
    unsigned SLEEP_MD			: 1;
    unsigned PRESLEEP_MD		: 1;
    unsigned RESET_MD			: 1;
  };
  uint32_t as_int;
} mdv3_r2_t;
#define MDv3_R2_DEFAULT {{0x3FF, 0, 0, 3, 1, 1, 1, 1, 1}}
_Static_assert(sizeof(mdv3_r2_t) == 4, "Punned Structure Size");

//Regsiter 3
typedef union mdv3_r3{
  struct{
    unsigned VDD_CC_ENB_0P6		: 1;
    unsigned VDD_CC_ENB_1P2		: 1;
    unsigned VNW_ENB_0P6		: 1; // Unused
    unsigned VNW_ENB_1P2		: 1; // Unused
    unsigned SEL_VREF			: 3;
    unsigned SEL_VREFP			: 3;
    unsigned SEL_VBN			: 2;
    unsigned SEL_VBP			: 2;
    unsigned SEL_VB_RAMP		: 4;
    unsigned SEL_RAMP			: 6;
  };
  uint32_t as_int;
} mdv3_r3_t;
#define MDv3_R3_DEFAULT {{0, 1, 1, 0, 0, 7, 3, 3, 0xF, 2}}
_Static_assert(sizeof(mdv3_r3_t) == 4, "Punned Structure Size");

//Regsiter 4
typedef union mdv3_r4{
  struct{
    unsigned SEL_ADC		: 3;
    unsigned SEL_ADC_B		: 3;
    unsigned SEL_PULSE		: 2;
    unsigned SEL_PULSE_COL	: 2;
    unsigned SEL_CC			: 3;
    unsigned SEL_CC_B		: 3;
    unsigned PULSE_SKIP		: 1;
    unsigned PULSE_SKIP_COL	: 1;
	unsigned TAVG			: 1;
	unsigned TAVG_B			: 1;
  };
  uint32_t as_int;
} mdv3_r4_t;
#define MDv3_R4_DEFAULT {{1, 6, 1, 0, 2, 5, 0, 0, 0, 1}}
_Static_assert(sizeof(mdv3_r4_t) == 4, "Punned Structure Size");

//Regsiter 5
typedef union mdv3_r5{
  struct{
    unsigned SEL_CLK_RING		: 2;
    unsigned SEL_CLK_RING_4US	: 2;
    unsigned SEL_CLK_RING_ADC	: 2;
    unsigned SEL_CLK_DIV		: 3;
    unsigned SEL_CLK_DIV_4US	: 2;
    unsigned SEL_CLK_DIV_ADC	: 1;
	unsigned CLK_EN_MD			: 1;
	unsigned CLK_EN_ADC			: 1;
	unsigned SEL_CLK_RING_LC	: 2;
	unsigned SEL_CLK_DIV_LC		: 2;
  };
  uint32_t as_int;
} mdv3_r5_t;
#define MDv3_R5_DEFAULT {{2, 0, 0, 3, 1, 1, 0, 0, 0, 0}}
_Static_assert(sizeof(mdv3_r5_t) == 4, "Punned Structure Size");

//Regsiter 6
typedef union mdv3_r6{
  struct{
    unsigned RESET_ADC_WRAPPER	: 1;
    unsigned START_ROW_IDX		: 8;
    unsigned END_ROW_IDX		: 8;
	unsigned ROW_SKIP			: 1;
	unsigned COL_SKIP			: 1;
	unsigned IMG_8BIT			: 1;
	unsigned MD_READ_MODE		: 1;
	unsigned ROW_IDX_EN			: 1;
  };
  uint32_t as_int;
} mdv3_r6_t;
#define MDv3_R6_DEFAULT {{1, 0, 160, 0, 0, 1, 0, 0}}
_Static_assert(sizeof(mdv3_r6_t) == 4, "Punned Structure Size");

//Regsiter 7
typedef union mdv3_r7{
  struct{
    unsigned MD_FLAG_ROW_IDX	: 4;
    unsigned MD_FLAG_COL_VAL	: 10;
    unsigned ADC_ERROR			: 1;
    unsigned ISOLATE_MD			: 1;
    unsigned ISOLATE_GPIO		: 1;
    unsigned ISOLATE_ADC_WRAPPER: 1;
  };
  uint32_t as_int;
} mdv3_r7_t;
#define MDv3_R7_DEFAULT {{0, 0, 0, 1, 1, 1}}
_Static_assert(sizeof(mdv3_r7_t) == 4, "Punned Structure Size");

//Regsiter 8
typedef union mdv3_r8{
  struct{
    unsigned MBUS_REPLY_ADDR_FLAG		: 8;
    unsigned MBUS_START_ADDR			: 7;
    unsigned MBUS_LENGTH_M1				: 7;
  };
  uint32_t as_int;
} mdv3_r8_t;
#define MDv3_R8_DEFAULT {{0x17, 0, 80}}
_Static_assert(sizeof(mdv3_r8_t) == 4, "Punned Structure Size");


//Regsiter 9
typedef union mdv3_r9{
  struct{
    unsigned MBUS_REPLY_ADDR_DATA : 8;
  };
  uint32_t as_int;
} mdv3_r9_t;
#define MDv3_R9_DEFAULT {{0x24}}
_Static_assert(sizeof(mdv3_r9_t) == 4, "Punned Structure Size");

// Declare the structs
extern volatile mdv3_r0_t mdv3_r0;
extern volatile mdv3_r1_t mdv3_r1;
extern volatile mdv3_r2_t mdv3_r2;
extern volatile mdv3_r3_t mdv3_r3;
extern volatile mdv3_r4_t mdv3_r4;
extern volatile mdv3_r5_t mdv3_r5;
extern volatile mdv3_r6_t mdv3_r6;
extern volatile mdv3_r7_t mdv3_r7;
extern volatile mdv3_r8_t mdv3_r8;
extern volatile mdv3_r9_t mdv3_r9;

