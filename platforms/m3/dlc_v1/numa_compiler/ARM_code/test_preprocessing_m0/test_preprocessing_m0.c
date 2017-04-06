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

#define STACK_TOP             0x6000
#define RAW_INPUT_ADDR        0x6000 
#define FFT_INPUT_ADDR_R      0x6800 
#define FFT_INPUT_ADDR_I      0x6c00 
//#define WINDOW_PARA_ADDR      0x7000
#define MFCC_INPUT_ADDR       0x7000 
#define MFCC_OUTPUT_ADDR       0x7300 
//#define DCT_PARA_ADDR         0x8000
#define DCT_OUTPUT_ADDR         0x7600

#define FFT_SIZE 512
#define FFT_PREC 6 
#define DFT_SIZE 8
#define FFT_NUM 32
#define FFT_NUM_BITS 6  // log2(FFT_NUM) + 1
#define FFT_INST 6      // total number of FFT instructions
#define WINDOW_SIZE 400
#define MFCC_SIZE 257
#define FILTER_SIZE 26
#define DCT_SIZE 13

int main() {
	//////////////////////////////////////////////////////
  // init
  // NLI_X, NLI_A are for debuggin and timing
  *DNN_NLI_A_0 = 1;
  *DNN_NLI_X_0 = 1;
//  volatile int16_t* raw_fft_input = (volatile int16_t *) RAW_INPUT_ADDR;        // size: 512 * 16 / 8 = 1024 (0x400)
  volatile int32_t* raw_fft_input = (volatile int32_t *) RAW_INPUT_ADDR;        // size: 512 * 16 / 8 = 1024 (0x400)
//  volatile int16_t* fft_input_r = (int16_t*) FFT_INPUT_ADDR_R;            // size: 512 * 16 / 8 = 1024 (0x400)
//  volatile int16_t* fft_input_i = (int16_t*) FFT_INPUT_ADDR_I;            // size: 512 * 16 / 8 = 1024 (0x400)
  volatile int32_t* fft_input_r = (int32_t*) FFT_INPUT_ADDR_R;            // size: 512 * 32 / 8 = 1024 (0x400)
  volatile int32_t* fft_input_i = (int32_t*) FFT_INPUT_ADDR_I;            // size: 512 * 32 / 8 = 1024 (0x400)
  volatile int16_t* mfcc_input = (int16_t*) MFCC_INPUT_ADDR;          // size: 257 * 16 / 8 = 514 (0x202)
  volatile int16_t* mfcc_output = (int16_t*) MFCC_OUTPUT_ADDR;          // size: 26 * 16 / 8 
  volatile int16_t* dct_output = (int16_t*) DCT_OUTPUT_ADDR;            // size: 13 * 16 / 8 = 26 
//    int16_t raw_fft_input[FFT_SIZE];
//    int16_t fft_input_r[FFT_SIZE];
//    int16_t fft_input_i[FFT_SIZE];
//    int16_t mfcc_input[MFCC_SIZE];
//    int16_t dct_output[DCT_SIZE];
  int iter;
  int ii;
  int idx;

///////////////////////
//  init_input;
  for (iter = 0; iter < WINDOW_SIZE; iter++) {
    raw_fft_input[iter] = iter;
  }
  *DNN_NLI_A_0 = 2;

///////////////////////
//  fft_apply_window
  int iter_2;
  for (iter = 0, iter_2 = WINDOW_SIZE; iter < 23; iter++, iter_2--) {
    raw_fft_input[iter] = 0;                // 0 / 16
    raw_fft_input[iter_2] = 0;                // 0 / 16
  }
  for (; iter < 40; iter++, iter_2--) {
    raw_fft_input[iter] = raw_fft_input[iter] >> 4;          // 1 / 16
    raw_fft_input[iter_2] = raw_fft_input[iter_2] >> 4;          // 1 / 16
  }
  for (; iter < 52; iter++, iter_2--) {
    raw_fft_input[iter] = raw_fft_input[iter] >> 3;          // 2 / 16
    raw_fft_input[iter_2] = raw_fft_input[iter_2] >> 3;          // 2 / 16
  }
  for (; iter < 62; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 3) + (raw_fft_input[iter] >> 4);          // 3 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 3) + (raw_fft_input[iter_2] >> 4);          // 3 / 16
  }
  for (; iter < 71; iter++, iter_2--) {
    raw_fft_input[iter] = raw_fft_input[iter] >> 2;          // 4 / 16
    raw_fft_input[iter_2] = raw_fft_input[iter_2] >> 2;          // 4 / 16
  }
  for (; iter < 80; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 2) + (raw_fft_input[iter] >> 4);          // 5 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 2) + (raw_fft_input[iter_2] >> 4);          // 5 / 16
  }
  for (; iter < 88; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 2) + (raw_fft_input[iter] >> 3);          // 6 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 2) + (raw_fft_input[iter_2] >> 3);          // 6 / 16
  }
  for (; iter < 96; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 2) - (raw_fft_input[iter] >> 3);          // 7 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 2) - (raw_fft_input[iter_2] >> 3);          // 7 / 16
  }
  for (; iter < 104; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 1);          // 8 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 1);          // 8 / 16
  }
  for (; iter < 112; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 1) + (raw_fft_input[iter] >> 4);          // 9 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 1) + (raw_fft_input[iter_2] >> 4);          // 9 / 16
  }
  for (; iter < 120; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 1) + (raw_fft_input[iter] >> 3);          // 10 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 1) + (raw_fft_input[iter_2] >> 3);          // 10 / 16
  }
  for (; iter < 129; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 1) + (raw_fft_input[iter] >> 3) + (raw_fft_input[iter] >> 4);          // 11 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 1) + (raw_fft_input[iter_2] >> 3) + (raw_fft_input[iter_2] >> 4);          // 11 / 16
  }
  for (; iter < 137; iter++, iter_2--) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 1) + (raw_fft_input[iter] >> 2);          // 12 / 16
    raw_fft_input[iter_2] = (raw_fft_input[iter_2] >> 1) + (raw_fft_input[iter_2] >> 2);          // 12 / 16
  }
  for (; iter < 148; iter++, iter_2--) {
    raw_fft_input[iter] = raw_fft_input[iter] - (raw_fft_input[iter] >> 3) - (raw_fft_input[iter] >> 4);          // 13 / 16
    raw_fft_input[iter_2] = raw_fft_input[iter_2] - (raw_fft_input[iter_2] >> 3) - (raw_fft_input[iter_2] >> 4);          // 13 / 16
  }
  for (; iter < 160; iter++, iter_2--) {
    raw_fft_input[iter] = raw_fft_input[iter] - (raw_fft_input[iter] >> 3);          // 14 / 16
    raw_fft_input[iter_2] = raw_fft_input[iter_2] - (raw_fft_input[iter_2] >> 3);          // 14 / 16
  }
  for (; iter < 177; iter++, iter_2--) {
    raw_fft_input[iter] = raw_fft_input[iter] - (raw_fft_input[iter] >> 4);          // 15 / 16
    raw_fft_input[iter_2] = raw_fft_input[iter_2] - (raw_fft_input[iter_2] >> 4);          // 15 / 16
  }
//  for (; iter < 137; iter++) {
//    raw_fft_input[iter] = (raw_fft_input[iter] >> 1) + (raw_fft_input[iter] >> 2);          // 16 / 16
//  }
  for (iter = WINDOW_SIZE; iter < FFT_SIZE; iter++)  // TODO: overlap w/ other operations 
    raw_fft_input[iter] = 0;

  *DNN_NLI_A_0 = 3;

///////////////////////
// prepare dft for fft
  // bit reversal
  int loc, j;
  int base;

  for (loc = 0; loc < FFT_NUM * 2; loc++) {
    // DFT_SIZE = 8 only 
    base = bit_reverse_6_table[loc]; 
    for (j = 0; j < DFT_SIZE; j++) { 
//      fft_input_r[loc * DFT_SIZE + j] = raw_fft_input[base + j * FFT_NUM * 2];
      // DFT_SIZE = 8 only 
      fft_input_r[(loc << 3) + j] = raw_fft_input[base + (j << 6)];
    }
  }
  // DFT_SIZE = 8:
  // 0 64 128 192 256 320 384 0 
  // 32 96 160 224 228 253 0 0
  // ...
  *DNN_NLI_A_0 = 4;
  for (iter = 0; iter < FFT_SIZE; iter++) {
    *DNN_NLI_X_0 = fft_input_r[iter];
  }
  *DNN_NLI_A_0 = 3;
///////////////////////
// dft
  int k;
  int32_t temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7;
  for (base = 0; base < FFT_SIZE; base += DFT_SIZE) {
    temp5 = -fft_input_r[base+1] + fft_input_r[base+3] - fft_input_r[base+5] + fft_input_r[base+7];
    temp6 = (fft_input_r[base+1] + fft_input_r[base+3]); 
    temp6 = temp6 - (temp6 >> 2) - (temp6 >> 4); 
    temp7 = (fft_input_r[base+5] + fft_input_r[base+7]); 
    temp7 = temp7 - (temp7 >> 2) - (temp7 >> 4); 
    temp6 = -temp6 + temp7;      // -0.7[1] - 0.7[3] + 0.7[5] + 0.7[7]
    temp7 = -fft_input_r[base+2] + fft_input_r[base+6]; 
    fft_input_i[base+0] = 0; 
    fft_input_i[base+4] = 0; 
    fft_input_i[base+2] = temp5;                   // -1 + 3 - 5 + 7
    fft_input_i[base+6] = -temp5;                  // 1 - 3 + 5 - 7 
    fft_input_i[base+1] = temp7 + temp6;      // -2 + 6 - 0.7[1] - 0.7[3] + 0.7[5] + 0.7[7]
    fft_input_i[base+3] = -temp7 + temp6;     // 2 - 6 - 0.7[1] - 0.7[3] + 0.7[5] + 0.7[7]
    fft_input_i[base+5] = temp7 - temp6;      // -2 + 6 + 0.7[1] + 0.7[3] - 0.7[5] - 0.7[7]
    fft_input_i[base+7] = -temp7 - temp6;     // 2 - 6 + 0.7[1] + 0.7[3] - 0.7[5] - 0.7[7] 
    temp0 = fft_input_r[base] + fft_input_r[base+4]; 
    temp1 = fft_input_r[base+2] + fft_input_r[base+6];
    temp2 = fft_input_r[base+1] + fft_input_r[base+3] + fft_input_r[base+5] + fft_input_r[base+7];
    temp3 = (fft_input_r[base+1] + fft_input_r[base+7]); 
    temp3 = temp3 - (temp3 >> 2) - (temp3 >> 4); 
    temp4 = (fft_input_r[base+3] + fft_input_r[base+5]); 
    temp4 = temp4 - (temp4 >> 2) - (temp4 >> 4); 
    temp3 = temp3 - temp4;      // 0.7[1] - 0.7[3] - 0.7[5] + 0.7[7]
    temp4 = fft_input_r[base] - fft_input_r[base+4]; 
    fft_input_r[base+0] = temp0 + temp1 + temp2;  // sum[0:7]
    fft_input_r[base+4] = temp0 + temp1 - temp2;  // 0 + 2 + 4 + 6 - 1 - 3 - 5 - 7
    fft_input_r[base+2] = temp0 - temp1;          // 0 - 2 + 4 - 6
    fft_input_r[base+6] = temp0 - temp1;          // 0 - 2 + 4 - 6
    fft_input_r[base+1] = temp4 + temp3;     // 0 - 4 + 0.7[1] - 0.7[3] - 0.7[5] + 0.7[7]
    fft_input_r[base+3] = temp4 - temp3;     // 0 - 4 - 0.7[1] + 0.7[3] + 0.7[5] - 0.7[7]
    fft_input_r[base+5] = temp4 - temp3;     // 0 - 4 - 0.7[1] + 0.7[3] + 0.7[5] - 0.7[7]
    fft_input_r[base+7] = temp4 + temp3;     // 0 - 4 + 0.7[1] - 0.7[3] - 0.7[5] + 0.7[7]
  }
  // use fft
  /*
  dft_size = 1;
  while (dft_size < 8) {
    for (j = 0; j < FFT_SIZE; j += (dft_size << 1)) {
      int twi_interval = FFT_SIZE / dft_size / 2;
      int twi_idx = 0;
      for (loc = 0; loc < dft_size; loc++) {
        float twidr = cos(-2*twi_idx*PI/FFT_SIZE);
        float twidi = sin(-2*twi_idx*PI/FFT_SIZE);
        int tempr = fft_input_r[j + loc + dft_size] * twidr - fft_input_i[j + loc + dft_size] * twidi;
        int tempi = fft_input_r[j + loc + dft_size] * twidi + fft_input_i[j + loc + dft_size] * twidr;
        fft_input_r[j + loc + dft_size] = fft_input_r[j + loc] - tempr;
        fft_input_r[j + loc] += tempr;
        fft_input_i[j + loc + dft_size] = fft_input_i[j + loc] - tempi;
        fft_input_i[j + loc] += tempi;
        twi_idx += twi_interval;
      }
    dft_size << 1;
  }*/
  // dft_size = 1;
  // twidr = 1, twidi = 0, fft_input_i = 0
  /*
  int tempr, tempi;
  for (j = 0; j < FFT_SIZE; j += 2) {
    tempr = fft_input_r[j + 1];
    fft_input_r[j + 1] = fft_input_r[j] - tempr;
    fft_input_r[j] += tempr;
  }
  *DNN_NLI_A_0 = 5;
  for (iter = 0; iter < FFT_SIZE; iter++) {
    *DNN_NLI_X_0 = fft_input_r[iter];
//    *DNN_NLI_X_1 = fft_input_i[iter];
  }
  *DNN_NLI_A_0 = 2;
  // dft_size = 2;
  // twidr = 1,0, twidi = 0,-1, fft_input_i = 0
  for (j = 0; j < FFT_SIZE; j += 4) {
    // loc = 0
    tempr = fft_input_r[j + 2];
    fft_input_r[j + 2] = fft_input_r[j] - tempr;
    fft_input_r[j] += tempr;
    fft_input_i[j + 2] = 0;   // need initialization
    fft_input_i[j] = 0;       // need initialization
    // loc = 1
    tempi = fft_input_r[j + 2 + 1] * -1;
    fft_input_r[j + 2 + 1] = fft_input_r[j + 1];
//    fft_input_r[j + 1] += 0;
    fft_input_i[j + 2 + 1] = -tempi;
    fft_input_i[j + 1] = tempi;
  }
  *DNN_NLI_A_0 = 5;
  for (iter = 0; iter < FFT_SIZE; iter++) {
    *DNN_NLI_X_0 = fft_input_r[iter];
    *DNN_NLI_X_1 = fft_input_i[iter];
  }
  *DNN_NLI_A_0 = 1;
  // dft_size = 4;
  // twidr = 1,0.71,0,-0.71, twidi = 0,-0.71,-1,-0.71, fft_input_i != 0
  int temp_1, temp_2, temp_3, temp_4;
  for (j = 0; j < FFT_SIZE; j += 8) {
    // loc = 0
    tempr = fft_input_r[j + 4];
    tempi = -fft_input_i[j + 4];
    fft_input_r[j + 4] = fft_input_r[j] - tempr;
    fft_input_r[j] += tempr;
    fft_input_i[j + 4] = fft_input_i[j] - tempi;
    fft_input_i[j] += tempi;
    // loc = 1
    temp_1 = fft_input_r[j + 4 + 1];
    temp_2 = fft_input_i[j + 4 + 1];
    temp_3 = (temp_1 - (temp_1 >> 2) - (temp_1 >> 4));   // 0.71
    temp_4 = (temp_2 - (temp_2 >> 2) - (temp_2 >> 4));   // 0.71
    tempr = temp_3 + temp_4;
    tempi = -temp_3 + temp_4;
    fft_input_r[j + 4 + 1] = fft_input_r[j + 1] - tempr;
    fft_input_r[j + 1] += tempr;
    fft_input_i[j + 4 + 1] = fft_input_i[j + 1] - tempi;
    fft_input_i[j + 1] += tempi;
    // loc = 2
    tempr = fft_input_i[j + 4 + 2];
    tempi = -fft_input_r[j + 4 + 2];
    fft_input_r[j + 4 + 2] = fft_input_r[j + 2] - tempr;
    fft_input_r[j + 2] += tempr;
    fft_input_i[j + 4 + 2] = fft_input_i[j + 2] - tempi;
    fft_input_i[j + 2] += tempi;
    // loc = 3
    temp_1 = fft_input_r[j + 4 + 3];
    temp_2 = fft_input_i[j + 4 + 3];
    temp_3 = (temp_1 - (temp_1 >> 2) - (temp_1 >> 4));  // 0.71
    temp_4 = (temp_2 - (temp_2 >> 2) - (temp_2 >> 4));  // 0.71
    tempr = -temp_3 + temp_4;
    tempi = -temp_3 - temp_4;
    fft_input_r[j + 4 + 3] = fft_input_r[j + 3] - tempr;
    fft_input_r[j + 3] += tempr;
    fft_input_i[j + 4 + 3] = fft_input_i[j + 3] - tempi;
    fft_input_i[j + 3] += tempi;
  }
  *DNN_NLI_A_0 = 5;
  for (iter = 0; iter < FFT_SIZE; iter++) {
    *DNN_NLI_X_0 = fft_input_r[iter];
    *DNN_NLI_X_1 = fft_input_i[iter];
  }
  *DNN_NLI_A_0 = 0;
  */
  *DNN_NLI_A_0 = 5;
///////////////////////
// write PE fft input

  write_dnn_sram_6_fft_input(0x2000, fft_input_r, fft_input_i);
  *DNN_NLI_A_0 = 6;
  read_dnn_sram_6_fft_output(0x2000, fft_input_r, fft_input_i);
  *DNN_NLI_A_0 = 7;
	/////////////////////////////////////////
  // fft 
  /*
  uint16_t dft_size = DFT_SIZE;
  uint16_t base;
  while (dft_size < FFT_SIZE) {
    for (base = 0; base < FFT_SIZE; base += dft_size*2) {   // one butterfly operation
      uint16_t twi_interval = (FFT_SIZE / dft_size / 2);  // fft_num
      uint16_t twi_idx = 0;
      uint16_t idx = 0;
//      *DNN_NLI_X_0 += 1;
      for (idx=0; idx < dft_size; idx++) {    // dft_size: half of butterfly length 
//        int16_t twidr = cos_16(-2*twi_idx*PI/FFT_SIZE);
//        int16_t twidi = sin_16(-2*twi_idx*PI/FFT_SIZE);
        int16_t twidr_t = twidr[idx];
        int16_t twidi_t = twidi[idx];
        int16_t tempr = fft_input_r[base+idx+dft_size] * twidr_t - fft_input_i[base+idx+dft_size] * twidi_t;
        int16_t tempi = fft_input_r[base+idx+dft_size] * twidi_t + fft_input_i[base+idx+dft_size] * twidr_t;
        tempr << 14;  // shift after multiply with dft parameter
        tempi << 14;
        fft_input_r[base+idx+dft_size] = fft_input_r[base+idx] - tempr; // has to compute this first
        fft_input_r[base+idx] += tempr;
        fft_input_i[base+idx+dft_size] = fft_input_i[base+idx] - tempi;
        fft_input_i[base+idx] += tempi;
        twi_idx += twi_interval;
      }
    }
    dft_size *= 2;
    *DNN_NLI_X_0 += 1;
  }
  */
	//////////////////////////////////////
///////////////////////
//  extract_fft_output(fft_input_r, fft_input_i, mfcc_input, MFCC_SIZE); 
  int32_t real, imag;
  for (iter = 0; iter < MFCC_SIZE; iter++) {
    real = fft_input_r[iter];
    imag = fft_input_i[iter];
    // approximation
    if (real < 0) real = -real;
    if (imag < 0) imag = -imag;
    if (real > imag)
      mfcc_input[iter] = real - (real >> 4) + (imag >> 1) - (imag >> 2);
//      mfcc_input[iter] = real + (imag >> 1) - (imag >> 2);
    else
      mfcc_input[iter] = imag - (imag >> 4) + (real >> 1) - (real >> 2);
//      mfcc_input[iter] = imag + (real >> 1) - (real >> 2);
  }
  *DNN_NLI_A_0 = 8;
  ///////////////////////
  // TODO
//  write_dnn_sram_6_mfcc_input(0x2000, mfcc_input);
//  *DNN_NLI_A_0 = 9;
//  read_dnn_sram_6_mfcc_output(0x2000, mfcc_output);
//  *DNN_NLI_A_0 = 10;
  // write MFCC on DNN
  for (ii = 0; ii < 26; ii++) {
    mfcc_output[ii] = mfcc_input[ii];
  }
  ///////////////////////
  // dct
  *DNN_NLI_A_0 = 11;
  for (iter = 0; iter < DCT_SIZE; iter++) {
    dct_output[iter] = 0;
    for (ii = 0; ii < 26; ii++) {
//      dct_output[iter] += ln16(mfcc_input[(ii + iter * 26) % MFCC_SIZE]) * dct_parameter[ii + iter * 26];
        int16_t temp_1 = mfcc_output[ii];
        temp_1 = (temp_1 >> 4) + (temp_1 >> 8) + (temp_1 >> 6);   // TODO
        dct_output[iter] += temp_1;
    }
  }
  *DNN_NLI_A_0 = 12;
  for (iter = 0; iter < DCT_SIZE; iter++) {
    *DNN_NLI_X_0 = dct_output[iter];
  }
  *DNN_NLI_A_0 = 13;
  signal_done();
	//////////////////////////////////////////////////////
  // PE init
	set_dnn_insts();
//	set_nli_parameters();

  uint16_t temp = 0;
  for (iter = 0; iter < FFT_SIZE; iter++) {
    temp += fft_input_r[iter];
    temp -= fft_input_i[iter];
  }
  *DNN_NLI_X_0 = temp;
  return 1;
 	// done
}
