//*******************************************************************
//Author: Hyeongseok Kim, Gyouho Kim, ZhiYoong Foo
//Description: AFEv1 Header File
//*******************************************************************

//Regsiter 0
typedef union afev1_r0{
  struct{ // LSB First
    unsigned LC_CLK_RING : 2;
    unsigned LC_CLK_DIV  : 2;
    unsigned AFE_ISO     : 1;
  };
  uint32_t as_int;
} afev1_r0_t;
#define AFEv1_R0_DEFAULT {{1, 2, 1}} // LSB is left
_Static_assert(sizeof(afev1_r0_t) == 4, "Punned Structure Size");

//Regsiter 1
typedef union afev1_r1{
  struct{ // LSB First
    unsigned RX_LNA_EN		: 1;
    unsigned RX_LNA_b 		: 4;
    unsigned RX_FLTR1_b0 	: 4;
    unsigned RX_FLTR1_b1 	: 4;    
    unsigned RX_FLTR1_BYPASS 	: 1;
    unsigned RX_GA1_EN		: 1;
    unsigned RX_GA1_b 		: 4;
    unsigned RX_FLTR2_EN 	: 1;
    unsigned RX_FLTR2_SEL 	: 2;    
  };
  uint32_t as_int;
} afev1_r1_t;
#define AFEv1_R1_DEFAULT {{0, 8, 8, 8, 0, 0, 8, 0, 1}} // LSB is left
_Static_assert(sizeof(afev1_r1_t) == 4, "Punned Structure Size");

//Regsiter 2
typedef union afev1_r2{
  struct{ // LSB First
    unsigned RX_FLTR2_BQ1_b0	: 4;
    unsigned RX_FLTR2_BQ1_b1	: 4;
    unsigned RX_FLTR2_BQ1_b2 	: 4;
    unsigned RX_FLTR2_BQ1_b3 	: 4;    
  };
  uint32_t as_int;
} afev1_r2_t;
#define AFEv1_R2_DEFAULT {{0xF, 1, 8, 8}} // LSB is left
_Static_assert(sizeof(afev1_r2_t) == 4, "Punned Structure Size");

//Regsiter 3
typedef union afev1_r3{
  struct{ // LSB First
    unsigned RX_FLTR2_BQ2_b0	: 4;
    unsigned RX_FLTR2_BQ2_b1	: 4;
    unsigned RX_FLTR2_BQ2_b2 	: 4;
    unsigned RX_FLTR2_BQ2_b3 	: 4;    
  };
  uint32_t as_int;
} afev1_r3_t;
#define AFEv1_R3_DEFAULT {{0xF, 3, 8, 8}} // LSB is left
_Static_assert(sizeof(afev1_r3_t) == 4, "Punned Structure Size");

//Regsiter 4
typedef union afev1_r4{
  struct{ // LSB First
    unsigned RX_FLTR2_BQ3_b0	: 4;
    unsigned RX_FLTR2_BQ3_b1	: 4;
    unsigned RX_FLTR2_BQ3_b2 	: 4;
    unsigned RX_FLTR2_BQ3_b3 	: 4;
    unsigned RX_FLTR2_BQ_b4 	: 3;    
  };
  uint32_t as_int;
} afev1_r4_t;
#define AFEv1_R4_DEFAULT {{0xF, 7, 8, 8, 4}} // LSB is left
_Static_assert(sizeof(afev1_r4_t) == 4, "Punned Structure Size");

//Regsiter 5
typedef union afev1_r5{
  struct{ // LSB First
    unsigned RX_BETA_EN		: 1;
    unsigned RX_BETA_b		: 4;
    unsigned RX_GA2_EN	 	: 1;
    unsigned RX_GA2_b0	 	: 4;
    unsigned RX_GA2_b1	 	: 4;
    unsigned RX_GA2_b2	 	: 4;
    unsigned RX_IQ_EN	 	: 1;    
  };
  uint32_t as_int;
} afev1_r5_t;
#define AFEv1_R5_DEFAULT {{0, 8, 0, 8, 8, 8, 0}} // LSB is left
_Static_assert(sizeof(afev1_r5_t) == 4, "Punned Structure Size");

//Regsiter 6
typedef union afev1_r6{
  struct{ // LSB First
    unsigned RX_MIX1_b		: 4;
    unsigned RX_MIX2_b		: 4;
    unsigned RX_BGR_b0	 	: 4;
    unsigned RX_BGR_b1	 	: 4;
    unsigned RX_BGR_b2	 	: 4;
    unsigned RX_BGR_b3	 	: 4;
  };
  uint32_t as_int;
} afev1_r6_t;
#define AFEv1_R6_DEFAULT {{8, 8, 8, 8, 8, 8}} // LSB is left
_Static_assert(sizeof(afev1_r6_t) == 4, "Punned Structure Size");

//Regsiter 7
typedef union afev1_r7{
  struct{ // LSB First
    unsigned PLL_TRI_7_8_EN	: 1;
    unsigned PLL_TRI_CK_EN	: 1;
    unsigned PLL_TRI_MODE 	: 1;
    unsigned PLL_L1_CK_EN 	: 1;
    unsigned PLL_CK_DIV	 	: 2;
    unsigned PLL_REFDIV_EN	: 1;
    unsigned PLL_VCO_TRIM 	: 3;
    unsigned PLL_FBDIV_EN 	: 1;
    unsigned PLL_VCO_EN		: 1;
  };
  uint32_t as_int;
} afev1_r7_t;
#define AFEv1_R7_DEFAULT {{0, 0, 0, 0, 0, 0, 3, 0, 0}} // LSB is left
_Static_assert(sizeof(afev1_r7_t) == 4, "Punned Structure Size");

//Regsiter 8
typedef union afev1_r8{
  struct{ // LSB First
    unsigned PLL_MISC			: 5;
    unsigned PLL_PFD_EN			: 1;
    unsigned PLL_CP_EN	 		: 1;
    unsigned PLL_IREF_EN 		: 1;
    unsigned PLL_CP_IMISM_TRIM 		: 4;
    unsigned PLL_VCO_IBIAS_TRIM		: 4;
    unsigned PLL_CP_IBIAS_TRIM	 	: 4;
  };
  uint32_t as_int;
} afev1_r8_t;
#define AFEv1_R8_DEFAULT {{0x07, 0, 0, 0, 7, 7, 7}} // LSB is left
_Static_assert(sizeof(afev1_r8_t) == 4, "Punned Structure Size");

//Regsiter 9
typedef union afev1_r9{
  struct{ // LSB First
    unsigned PLL_PFD_DEL_TRIM	: 16;
  };
  uint32_t as_int;
} afev1_r9_t;
#define AFEv1_R9_DEFAULT {{0x0007}} // LSB is left
_Static_assert(sizeof(afev1_r9_t) == 4, "Punned Structure Size");

//Regsiter A
typedef union afev1_rA{
  struct{ // LSB First
    unsigned DSP_RESET_N	: 1;
    unsigned DSP_CONFIG		: 23;
  };
  uint32_t as_int;
} afev1_rA_t;
#define AFEv1_RA_DEFAULT {{0, 0x06A15A}} // LSB is left
_Static_assert(sizeof(afev1_rA_t) == 4, "Punned Structure Size");

//Regsiter B
typedef union afev1_rB{
  struct{ // LSB First
    unsigned ADC_COMP2_T2_I	: 5;
    unsigned ADC_COMP2_T1_I	: 5;
    unsigned ADC_COMP3_T2_I	: 5;
    unsigned ADC_COMP3_T1_I	: 5;   
    unsigned ADC_EN		: 1;
    unsigned DSP_EN		: 1;
  };
  uint32_t as_int;
} afev1_rB_t;
#define AFEv1_RB_DEFAULT {{ 0x0, 0x0, 0x07, 0x0, 0, 0}} // LSB is left
_Static_assert(sizeof(afev1_rB_t) == 4, "Punned Structure Size");

//Regsiter C
typedef union afev1_rC{
  struct{ // LSB First
    unsigned ADC_COMP3_T2_Q	: 5;
    unsigned ADC_COMP3_T1_Q	: 5;
    unsigned ADC_COMP1_T2_I	: 5;
    unsigned ADC_COMP1_T1_I	: 5;
  };
  uint32_t as_int;
} afev1_rC_t;
#define AFEv1_RC_DEFAULT {{0x07, 0x0, 0x0, 0x07}} // LSB is left
_Static_assert(sizeof(afev1_rC_t) == 4, "Punned Structure Size");

//Regsiter D
typedef union afev1_rD{
  struct{ // LSB First
    unsigned ADC_COMP1_T2_Q	: 5;
    unsigned ADC_COMP1_T1_Q	: 5;
    unsigned ADC_COMP2_T2_Q	: 5;
    unsigned ADC_COMP2_T1_Q	: 5;
  };
  uint32_t as_int;
} afev1_rD_t;
#define AFEv1_RD_DEFAULT {{0x0, 0x07, 0x0, 0x0}} // LSB is left
_Static_assert(sizeof(afev1_rD_t) == 4, "Punned Structure Size");

// Declare the structs
extern volatile afev1_r0_t afev1_r0;
extern volatile afev1_r1_t afev1_r1;
extern volatile afev1_r2_t afev1_r2;
extern volatile afev1_r3_t afev1_r3;
extern volatile afev1_r4_t afev1_r4;
extern volatile afev1_r5_t afev1_r5;
extern volatile afev1_r6_t afev1_r6;
extern volatile afev1_r7_t afev1_r7;
extern volatile afev1_r8_t afev1_r8;
extern volatile afev1_r9_t afev1_r9;
extern volatile afev1_rA_t afev1_rA;
extern volatile afev1_rB_t afev1_rB;
extern volatile afev1_rC_t afev1_rC;
extern volatile afev1_rD_t afev1_rD;
