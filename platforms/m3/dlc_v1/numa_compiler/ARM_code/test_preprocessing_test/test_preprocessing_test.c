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
#define RAW_INPUT_ADDR        0x7000 
#define FFT_INPUT_ADDR_R      0x6500 
#define FFT_INPUT_ADDR_I      0x6a00 
#define WINDOW_PARA_ADDR      0x7000
#define MFCC_INPUT_ADDR       0x7400 
#define MFCC_OUTPUT_ADDR       0x7700 
#define DCT_PARA_ADDR         0x8000
#define DCT_OUTPUT_ADDR         0x8100
#define FFT_PREC 16     // essentially hard-coded: other precision won't work
#define FFT_NUM 256     
#define FFT_NUM_BITS 9  // log2(FFT_NUM) + 1
#define FFT_INST 8      // total number of FFT instructions
#define DFT_SIZE 1
#define FFT_SIZE 512
#define WINDOW_SIZE 400
#define MFCC_SIZE 257

#define INT_BITS 0
#define INT_BITS_2 0

int main() {
	//////////////////////////////////////////////////////
  // init
  // NLI_X, NLI_A are for debuggin and timing
  *DNN_NLI_A_0 = 1;
  *DNN_NLI_X_0 = 1;
  volatile int16_t* raw_fft_input = (volatile int16_t *) RAW_INPUT_ADDR;        // size: 512 * 16 / 8 = 1024 (0x400)
  volatile int16_t* fft_input_r = (int16_t*) FFT_INPUT_ADDR_R;            // size: 512 * 16 / 8 = 1024 (0x400)
  volatile int16_t* fft_input_i = (int16_t*) FFT_INPUT_ADDR_I;            // size: 512 * 16 / 8 = 1024 (0x400)
  volatile int16_t* mfcc_input = (int16_t*) MFCC_INPUT_ADDR;          // size: 257 * 16 / 8 = 514 (0x202)
  volatile int16_t* mfcc_output = (int16_t*) MFCC_OUTPUT_ADDR;          // size: 257 * 16 / 8 = 514 (0x202)
  volatile int16_t* dct_output = (int16_t*) DCT_OUTPUT_ADDR;            // size: 13 * 16 / 8 = 26 
//    int16_t raw_fft_input[512];
//    int16_t fft_input_r[512];
//    int16_t fft_input_i[512];
//    int16_t window_parameter[400];
//    int16_t mfcc_input[257];
//    int16_t dct_parameter[338];
//    int16_t dct_output[13];
  uint16_t iter;
  uint16_t ii;
  uint16_t idx;

///////////////////////
//  init_input;
  for (iter = 0; iter < 400; iter++) {
    raw_fft_input[iter] = iter;
  }
  *DNN_NLI_A_0 = 2;

///////////////////////
//  fft_apply_window
  for (iter = 0; iter < 22; iter++) {
    raw_fft_input[iter] = 0;
  }
  for (; iter < 39; iter++) {
    raw_fft_input[iter] = raw_fft_input[iter] >> 4;
  }
  for (; iter < 51; iter++) {
    raw_fft_input[iter] = raw_fft_input[iter] >> 3;
  }
  for (; iter < 61; iter++) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 3) + (raw_fft_input[iter] >> 4);
  }
  for (; iter < 70; iter++) {
    raw_fft_input[iter] = raw_fft_input[iter] >> 2;
  }
  for (; iter < 79; iter++) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 2) + (raw_fft_input[iter] >> 4);
  }
  for (; iter < 87; iter++) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 2) + (raw_fft_input[iter] >> 3);
  }
  for (; iter < 95; iter++) {
    raw_fft_input[iter] = (raw_fft_input[iter] >> 2) - (raw_fft_input[iter] >> 3);
  }
  for (; iter < 400; iter++)  // TODO
    raw_fft_input[iter] = raw_fft_input[iter] >> 1;
  for (; iter < 512; iter++)  // TODO: overlap w/ other operations 
    raw_fft_input[iter] = 0;

  *DNN_NLI_A_0 = 3;

///////////////////////
// prepare dft for fft
  // bit reversal
  uint16_t loc, j;
  uint16_t base;
  const uint16_t fft_num = 128, fft_num_bits = 8, fft_size = 512, dft_size = 2;

  for (loc = 0; loc < fft_num * 2; loc++) {
    base = int_bit_reverse(loc, fft_num_bits); // TODO: use a lookup table
    for (j = 0; j < fft_size / fft_num / 2; j++) { 
      fft_input_r[loc * dft_size + j] = raw_fft_input[base + j * fft_num * 2];
    }
  }

  *DNN_NLI_A_0 = 4;
///////////////////////
// dft
  uint16_t k;
  int16_t tempr[dft_size];
  int16_t tempi[dft_size];
  for (base = 0; base < fft_size; base += dft_size) {
    for (j = 0; j < dft_size; j++) {
      tempr[j] = 0;
      tempi[j] = 0;
      for (k = 0; k < dft_size; k++) {
        // TODO
        tempr[j] += fft_input_r[k+base] >> 2; 
        tempi[j] += -fft_input_r[k+base] >> 2;
        // only half of the equation: imag part are all 0
      }
    }
    for (j = 0; j < dft_size; j++) {
      fft_input_r[j+base] = tempr[j];
      fft_input_i[j+base] = tempi[j];
    }
  }
///////////////////////
// write PE fft input
/*
    write_dnn_sram_fft(0x0000, fft_input_r, fft_input_i, 200);
    *DNN_NLI_A_0 = 4;
*/
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
//        *DNN_NLI_X_0 += 1;
        for (idx=0; idx < dft_size; idx++) {    // dft_size: half of butterfly length 
//          int16_t twidr = cos_16(-2*twi_idx*PI/FFT_SIZE);
//          int16_t twidi = sin_16(-2*twi_idx*PI/FFT_SIZE);
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
    *DNN_NLI_A_0 = 5;
    *DNN_NLI_X_0 = 2;
    for (iter = 0; iter < 257; iter++) {
      *DNN_NLI_X_0 = 1;
      int16_t real = fft_input_r[iter];
      int16_t imag = fft_input_i[iter];
      // approximation
      if (real < 0) real = -real;
      if (imag < 0) imag = -imag;
      if (real > imag)
        mfcc_input[iter] = real - (real >> 4) + (imag >> 1) - (imag >> 2);
      else
        mfcc_input[iter] = imag - (imag >> 4) + (real >> 1) - (real >> 2);
    }
    *DNN_NLI_A_0 = 6;
    ///////////////////////
    // TODO
    // write MFCC on DNN
    for (ii = 0; ii < 26; ii++) {
      mfcc_output[ii] = mfcc_input[ii];
    }
    ///////////////////////
    // dct
    *DNN_NLI_A_0 = 7;
    for (iter = 0; iter < 13; iter++) {
      dct_output[iter] = 0;
      for (ii = 0; ii < 26; ii++) {
//        dct_output[iter] += ln16(mfcc_input[(ii + iter * 26) % 257]) * dct_parameter[ii + iter * 26];
          idx = ii + iter * 26;
          int16_t temp_1 = mfcc_output[ii];
          temp_1 = (temp_1 >> 4) + (temp_1 >> 8) + (temp_1 >> 6);   // TODO
          dct_output[iter] += temp_1;
      }
    }
    *DNN_NLI_A_0 = 8;
    for (iter = 0; iter < 13; iter++) {
      *DNN_NLI_X_0 = dct_output[iter];
    }
    *DNN_NLI_A_0 = 9;
    signal_done();
		//////////////////////////////////////////////////////
    // PE init
		set_dnn_insts();
//		set_nli_parameters();

    uint16_t temp = 0;
    for (iter = 0; iter < 512; iter++) {
      temp += fft_input_r[iter];
      temp -= fft_input_i[iter];
    }
    *DNN_NLI_X_0 = temp;
    return 1;
 		// done
}
