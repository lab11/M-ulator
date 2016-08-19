//*******************************************************************
//Author: Yejoong Kim Ziyun Li Cao Gao
//Description: M0 code for Deep Learning Processor project
//*******************************************************************
#include "DLCv1.h"
//#include "FLSv2S_RF.h"
//#include "PMUv2_RF.h"
#include "DLCv1_RF.h"
#include "mbus.h"
#include "dnn_parameters.h"
#include "utils.h"

#define DLC_ADDR    0x4

//********************************************************************
// Global Variables
//********************************************************************
volatile uint32_t enumerated;
volatile uint32_t cyc_num;
volatile uint32_t irq_history;

//*******************************************************************
// INTERRUPT HANDLERS
//*******************************************************************
void handler_ext_int_0(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_1(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_2(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_3(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_4(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_5(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_6(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_7(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_8(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_9(void)  __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_10(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_11(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_12(void) __attribute__ ((interrupt ("IRQ")));
void handler_ext_int_13(void) __attribute__ ((interrupt ("IRQ")));

void handler_ext_int_0(void) { *NVIC_ICPR = (0x1 << 0); // TIMER32
    irq_history |= (0x1 << 0);
    *REG0 = 0x1000;
    *REG1 = 0x1;//*TIMER32_CNT;
    *REG2 = 0x1;///*TIMER32_STAT;
    //*TIMER32_STAT = 0x0;
    arb_debug_reg (0xA0000000);
    arb_debug_reg ((0xA1 << 24) | *REG0); // 0x1000
    arb_debug_reg ((0xA1 << 24) | *REG1); // TIMER32_CNT
    arb_debug_reg ((0xA1 << 24) | *REG2); // TIMER32_STAT
    }
void handler_ext_int_1(void) { *NVIC_ICPR = (0x1 << 1); // TIMER16
    irq_history |= (0x1 << 1);
    *REG0 = 0x1001;
    *REG1 = 0x1;//*TIMER16_CNT;
    *REG2 = 0x1;//*TIMER16_STAT;
    //*TIMER16_STAT = 0x0;
    arb_debug_reg (0xA0000001);
    arb_debug_reg ((0xA1 << 24) | *REG0); // 0x1001
    arb_debug_reg ((0xA1 << 24) | *REG1); // TIMER16_CNT
    arb_debug_reg ((0xA1 << 24) | *REG2); // TIMER16_STAT
    }
void handler_ext_int_2(void) { *NVIC_ICPR = (0x1 << 2); // REG0
    irq_history |= (0x1 << 2);
    arb_debug_reg (0xA0000002);
}
void handler_ext_int_3(void) { *NVIC_ICPR = (0x1 << 3); // REG1
    irq_history |= (0x1 << 3);
    arb_debug_reg (0xA0000003);
}
void handler_ext_int_4(void) { *NVIC_ICPR = (0x1 << 4); // REG2
    irq_history |= (0x1 << 4);
    arb_debug_reg (0xA0000004);
}
void handler_ext_int_5(void) { *NVIC_ICPR = (0x1 << 5); // REG3
    irq_history |= (0x1 << 5);
    arb_debug_reg (0xA0000005);
}
void handler_ext_int_6(void) { *NVIC_ICPR = (0x1 << 6); // REG4
    irq_history |= (0x1 << 6);
    arb_debug_reg (0xA0000006);
}
void handler_ext_int_7(void) { *NVIC_ICPR = (0x1 << 7); // REG5
    irq_history |= (0x1 << 7);
    arb_debug_reg (0xA0000007);
}
void handler_ext_int_8(void) { *NVIC_ICPR = (0x1 << 8); // REG6
    irq_history |= (0x1 << 8);
    arb_debug_reg (0xA0000008);
}
void handler_ext_int_9(void) { *NVIC_ICPR = (0x1 << 9); // REG7
    irq_history |= (0x1 << 9);
    arb_debug_reg (0xA0000009);
}
void handler_ext_int_10(void) { *NVIC_ICPR = (0x1 << 10); // MEM WR
    irq_history |= (0x1 << 10);
    arb_debug_reg (0xA000000A);
}
void handler_ext_int_11(void) { *NVIC_ICPR = (0x1 << 11); // MBUS_RX
    irq_history |= (0x1 << 11);
    arb_debug_reg (0xA000000B);
}
void handler_ext_int_12(void) { *NVIC_ICPR = (0x1 << 12); // MBUS_TX
    irq_history |= (0x1 << 12);
    arb_debug_reg (0xA000000C);
}
void handler_ext_int_13(void) { *NVIC_ICPR = (0x1 << 13); // MBUS_FWD
    irq_history |= (0x1 << 13);
    arb_debug_reg (0xA000000D);
}

//*******************************************************************
// USER FUNCTIONS
//*******************************************************************
void initialization (void) {

    enumerated = 0xDEADBEEF;
    cyc_num = 0;
    irq_history = 0;

    // Set Halt
    set_halt_until_mbus_rx();

    // Enumeration
    //mbus_enumerate(DLC_ADDR);

    //Set Halt
    set_halt_until_mbus_tx();
}

void pass (uint32_t id, uint32_t data) {
    arb_debug_reg (0x0EA7F00D);
    arb_debug_reg (id);
    arb_debug_reg (data);
}

void fail (uint32_t id, uint32_t data) {
    arb_debug_reg (0xDEADBEEF);
    arb_debug_reg (id);
    arb_debug_reg (data);
    //*REG_CHIP_ID = 0xFFFF; // This will stop the verilog sim.
}

//********************************************************************
// MAIN function starts here             
//********************************************************************
#define PI 3.1415926

///////////
/*
void init_input(int16_t* fft_input, uint16_t windowsize) {
  uint16_t iter;
  for (iter = 0; iter < windowsize; iter++)
    fft_input[iter] = (iter % 10) * 50;
}
void window_init(int16_t* window_parameter, uint16_t windowsize) {
  uint16_t iter;
  for (iter = 0; iter < windowsize; iter++)
    window_parameter[iter] = 1; 
}
*/
///////////
uint32_t pow_2(uint32_t x) {
  uint32_t temp, result;
  switch (x) {
    case 0:
      return 1; 
    case 1:
      return 2; 
    case 2:
      return 4; 
    case 4:
      return 16;
    case 6:
      return 64;
    case 8:
      return 256; 
    case 12:
      return 4096;
    default:
      temp = x - 2;
      result = 4;
      while (temp-- > 0)
        result *= 2; 
      return result;
  }
}

int16_t cos_16(float in) {
//  uint16_t shift = pow_2(14);   // shift after multiply with dft parameter
  float in2 = in * in;
  float in4 = in2 * in2;
  float in6 = in4 * in2;
  float result = 1 - in2 / 2 + in4 / 24 - in6 / 720; // taylor series
  return (int)result << 14;
}

int16_t sin_16(float in) {
//  uint16_t shift = pow_2(14);   // shift after multiply with dft parameter
  float in2 = in * in;
  float in3 = in2 * in;
  float in5 = in3 * in2;
  float in7 = in5 * in2;
  float result = in - in3 / 6 + in5 / 120 - in7 / 5040; // taylor series
  return (int)result << 14;
}

uint32_t sqrt32(uint32_t n) {
  register uint32_t root, remainder, place;

  root = 0;
  remainder = n;
  place = 0x40000000;
  while (place > remainder) {
    place = place >> 2;
  }

  while (place) {
    if (remainder >= root + place) {
      remainder = remainder - root - place;
      root = root + (place << 1);
    }
    root = root >> 1;
    place = place >> 2;
  }

  return root;
}

inline int16_t ln16(int16_t value) {
  if (value < 0) return 0;
  else return value;
}

static const uint8_t bit_reverse_6_table[] = {
0x0 , 0x20 , 0x10 , 0x30 , 0x8 , 0x28 , 0x18 , 0x38 , 0x4 , 0x24 , 0x14 , 0x34 , 0xc , 0x2c , 0x1c , 0x3c , 0x2 , 0x22 , 0x12 , 0x32 , 0xa , 0x2a , 0x1a , 0x3a , 0x6 , 0x26 , 0x16 , 0x36 , 0xe , 0x2e , 0x1e , 0x3e , 0x1 , 0x21 , 0x11 , 0x31 , 0x9 , 0x29 , 0x19 , 0x39 , 0x5 , 0x25 , 0x15 , 0x35 , 0xd , 0x2d , 0x1d , 0x3d , 0x3 , 0x23 , 0x13 , 0x33 , 0xb , 0x2b , 0x1b , 0x3b , 0x7 , 0x27 , 0x17 , 0x37 , 0xf , 0x2f , 0x1f , 0x3f
};

static const uint8_t bit_reverse_7_table[] = {
  0x0 , 0x40 , 0x20 , 0x60 , 0x10 , 0x50 , 0x30 , 0x70 , 0x8 , 0x48 , 0x28 , 0x68 , 0x18 , 0x58 , 0x38 , 0x78 , 0x4 , 0x44 , 0x24 , 0x64 , 0x14 , 0x54 , 0x34 , 0x74 , 0xc , 0x4c , 0x2c , 0x6c , 0x1c , 0x5c , 0x3c , 0x7c , 0x2 , 0x42 , 0x22 , 0x62 , 0x12 , 0x52 , 0x32 , 0x72 , 0xa , 0x4a , 0x2a , 0x6a , 0x1a , 0x5a , 0x3a , 0x7a , 0x6 , 0x46 , 0x26 , 0x66 , 0x16 , 0x56 , 0x36 , 0x76 , 0xe , 0x4e , 0x2e , 0x6e , 0x1e , 0x5e , 0x3e , 0x7e , 0x1 , 0x41 , 0x21 , 0x61 , 0x11 , 0x51 , 0x31 , 0x71 , 0x9 , 0x49 , 0x29 , 0x69 , 0x19 , 0x59 , 0x39 , 0x79 , 0x5 , 0x45 , 0x25 , 0x65 , 0x15 , 0x55 , 0x35 , 0x75 , 0xd , 0x4d , 0x2d , 0x6d , 0x1d , 0x5d , 0x3d , 0x7d , 0x3 , 0x43 , 0x23 , 0x63 , 0x13 , 0x53 , 0x33 , 0x73 , 0xb , 0x4b , 0x2b , 0x6b , 0x1b , 0x5b , 0x3b , 0x7b , 0x7 , 0x47 , 0x27 , 0x67 , 0x17 , 0x57 , 0x37 , 0x77 , 0xf , 0x4f , 0x2f , 0x6f , 0x1f , 0x5f , 0x3f , 0x7f
};

// TODO: optimize w/ lookup table
uint16_t int_bit_reverse(uint16_t input, uint16_t num_bits) {
  uint16_t result = 0;
  uint16_t i;
  for (i = 0; i < num_bits; i++) {
    bool temp = (input >> (num_bits - i - 1) & 1);
    result |= temp << i;
  }
  return result;
}
///////////

#define COL_SIZE 32
#define ROW_SIZE 32

int main() {
	//////////////////////////////////////////////////////
  // init
  // NLI_X, NLI_A are for debuggin and timing
  *DNN_NLI_A_0 = 1;
  *DNN_NLI_X_0 = 1;
  int32_t input[COL_SIZE];
  int32_t parameter[COL_SIZE * ROW_SIZE];
  int32_t output[ROW_SIZE];
  int iter;
  int ii;

///////////////////////
//  init;
  for (iter = 0; iter < COL_SIZE; iter++)
    input[iter] = iter;
  for (ii = 0; ii < ROW_SIZE; ii++)
    for (iter = 0; iter < COL_SIZE; iter++)
      parameter[ii * COL_SIZE + iter] = (ii * COL_SIZE + iter) >> 2;
  *DNN_NLI_A_0 = 2;

///////////////////////
// shift
//  for (iter = 0; iter < COL_SIZE; iter++) {
//    input[iter] = iter;
//  }
//  *DNN_NLI_A_0 = 3;

///////////////////////
// multiplication
  int jj = 0;
  int test;
  for (ii = 0; ii < ROW_SIZE; ii++) {
    output[ii] = 0;
    for (iter = 0; iter < COL_SIZE; iter++) {
      output[ii] += parameter[jj++] * input[iter];
    }
  }
  *DNN_NLI_A_0 = 3;
///////////////////////
// output
  for (ii = 0; ii < ROW_SIZE; ii++)
    *DNN_NLI_X_0 = output[ii];
  *DNN_NLI_A_0 = 4;
  signal_done();
  set_dnn_insts();
  return 1;
 	// done
}
