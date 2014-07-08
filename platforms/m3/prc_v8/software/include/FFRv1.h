//*******************************************************************
//Author: ZhiYoong Foo
//Description: FFRv1 Header File
//*******************************************************************

//Register 0
typedef union ffrv1_r0{
  struct{
    unsigned TX_pulse_b	: 4;
    unsigned TX_VCO_b 	: 7;
  };
  uint32_t as_int;
} ffrv1_r0_t;
#define FFRv1_R0_DEFAULT {{0x4, 0x18}}
_Static_assert(sizeof(ffrv1_r0_t) == 4, "Punned Structure Size");

//Register 1
typedef union ffrv1_r1{
  struct{
    unsigned TX_FSM_TX_DATA	: 24;
  }; 
  uint32_t as_int;
} ffrv1_r1_t;
#define FFRv1_R1_DEFAULT {{0x0}}
_Static_assert(sizeof(ffrv1_r1_t) == 4, "Punned Structure Size");

//Register 2
//TX_FSM_FSM_STATE is Read Only
typedef union ffrv1_r2{
  struct{
    unsigned TX_FSM_FSM_STATE	: 3;
    unsigned TX_FSM_T_Z_DEL	: 8;
    unsigned TX_FSM_N_HEADER	: 4;
    unsigned TX_FSM_TX2TX	: 8;
    unsigned TX_FSM_START	: 1;
  };
  uint32_t as_int;
} ffrv1_r2_t;
#define FFRv1_R2_DEFAULT {{0x3, 0x10, 0x4, 0x20, 0x1}}
_Static_assert(sizeof(ffrv1_r2_t) == 4, "Punned Structure Size");

//Register 3
typedef union ffrv1_r3{
  struct{
    unsigned I_LIMIT_IL_cal32_0	: 5; 
    unsigned I_LIMIT_IL_bypass	: 4;
    unsigned I_LIMIT_IL_iTune	: 6;
    unsigned I_LIMIT_IL_EN	: 1;
  };
  uint32_t as_int;
} ffrv1_r3_t;
#define FFRv1_R3_DEFAULT {{0x1F, 0xF, 0x0, 0x0}}
_Static_assert(sizeof(ffrv1_r3_t) == 4, "Punned Structure Size");

//Register 4
typedef union ffrv1_r4{
  struct{
    unsigned CLKGEN_RINGSEL	: 4;
    unsigned I_LIMIT_IL_cal16 	: 8;
    unsigned I_LIMIT_IL_cal32_2	: 5;
    unsigned I_LIMIT_IL_cal32_1	: 5;
  };
  uint32_t as_int;
} ffrv1_r4_t;
#define FFRv1_R4_DEFAULT {{0xFF, 0x1F, 0x1F, 0x0}}
_Static_assert(sizeof(ffrv1_r4_t) == 4, "Punned Structure Size");
