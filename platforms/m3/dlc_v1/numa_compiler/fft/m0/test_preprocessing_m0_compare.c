//********************************************************************
// MAIN function starts here             
//********************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#define PI 3.1415926

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

///////////
/*
void fft_apply_window(uint16_t window_size, int16_t* frame, int16_t* win) {
  uint16_t i;
  for (i = 0; i < window_size; i++)
    frame[i] *= win[i];
}

uint16_t int_bit_reverse(uint16_t input, uint16_t num_bits) {
  uint16_t result = 0;
  uint16_t i;
  for (i = 0; i < num_bits; i++) {
    bool temp = (input >> (num_bits - i - 1) & 1);
    result |= temp << i;
  }
  return result;
}

void prepare_fft(uint16_t window_size, uint16_t fft_size, int16_t* raw_input, int16_t* input, uint16_t fft_num, uint16_t fft_num_bits) {
  uint16_t loc, j;
  // zero padding
  for (loc = window_size; loc < fft_size; loc++) {
    raw_input[loc] = 0;
  }

  // bit reversal
  uint16_t base;
  for (loc = 0; loc < fft_num * 2; loc++) {
    base = int_bit_reverse(loc, fft_num_bits);
    for (j = 0; j < fft_size / fft_num / 2; j++) {
      input[loc + j] = raw_input[base + j * fft_num * 2];
    }
  }
}

void extract_fft_output(int16_t* fft_output_r, int16_t* fft_output_i, int16_t* mfcc_input, uint16_t mfcc_size) {
  uint16_t iter;
  uint16_t base = pow_2(1);  // TODO: int bits 
  for (iter = 0; iter < mfcc_size; iter++) 
    mfcc_input[iter] = complex_to_real(fft_output_r[iter], fft_output_i[iter], base);
}
*/

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
/*
    volatile int16_t* raw_fft_input = (volatile int16_t *) RAW_INPUT_ADDR;        // size: 512 * 16 / 8 = 1024 (0x400)
    int16_t* window_parameter = (int16_t*) WINDOW_PARA_ADDR;   // size: 400 * 16 / 8 = 800 (0x320)
//    volatile int16_t* fft_input_r = (int16_t*) FFT_INPUT_ADDR_R;            // size: 512 * 16 / 8 = 1024 (0x400)
    volatile int16_t* fft_input_r = (int16_t*) FFT_INPUT_ADDR_R;            // size: 512 * 16 / 8 = 1024 (0x400)
    volatile int16_t* fft_input_i = (int16_t*) FFT_INPUT_ADDR_I;            // size: 512 * 16 / 8 = 1024 (0x400)
    volatile int16_t* mfcc_input = (int16_t*) MFCC_INPUT_ADDR;          // size: 257 * 16 / 8 = 514 (0x202)
    volatile int16_t* mfcc_output = (int16_t*) MFCC_OUTPUT_ADDR;          // size: 257 * 16 / 8 = 514 (0x202)
    int16_t* dct_parameter = (int16_t *) DCT_PARA_ADDR;  // size: 26 * 13 * 16 / 8 = 672 (0x2A0)
    volatile int16_t* dct_output = (int16_t*) DCT_OUTPUT_ADDR;            // size: 13 * 16 / 8 = 26 
*/

    int16_t raw_fft_input[512];
    int16_t fft_input_r[512];
    int16_t fft_input_i[512];
//    int16_t window_parameter[400];
    int16_t mfcc_input[257];
    int16_t mfcc_output[26];
//    int16_t dct_parameter[338];
    int16_t dct_output[13];
    uint16_t iter;
    uint16_t ii;
    uint16_t idx;

//    init_input;
    for (iter = 0; iter < 400; iter++) {
      raw_fft_input[iter] = iter;
    }

//  fft_apply_window
    for (iter = 0; iter < 22; iter++) {
      fft_input_r[iter] = 0;
    }
    for (; iter < 39; iter++) {
      fft_input_r[iter] = raw_fft_input[iter] >> 4;
    }
    for (; iter < 51; iter++) {
      fft_input_r[iter] = raw_fft_input[iter] >> 3;
    }
    for (; iter < 61; iter++) {
      fft_input_r[iter] = (raw_fft_input[iter] >> 3) + (raw_fft_input[iter] >> 4);
    }
    for (; iter < 70; iter++) {
      fft_input_r[iter] = raw_fft_input[iter] >> 2;
    }
    for (; iter < 79; iter++) {
      fft_input_r[iter] = (raw_fft_input[iter] >> 2) + (raw_fft_input[iter] >> 4);
    }
    for (; iter < 87; iter++) {
      fft_input_r[iter] = (raw_fft_input[iter] >> 2) + (raw_fft_input[iter] >> 3);
    }
    for (; iter < 95; iter++) {
      fft_input_r[iter] = (raw_fft_input[iter] >> 2) - (raw_fft_input[iter] >> 3);
    }
    for (; iter < 400; iter++)  // TODO
      fft_input_r[iter] = raw_fft_input[iter] >> 1;
     
    printf("windowing result:\n");
    for (iter = 0; iter < 400; iter++) {
      float float_result = raw_fft_input[iter] * 0.5 * (1 - cos(2 * PI * iter / 399));
      printf("%d %.2f\n", fft_input_r[iter], float_result);
    }
    printf("\n");
    ////////
    for (iter = 401; iter < 512; iter++) {
      fft_input_r[iter] = 0;
    }
    for (iter = 0; iter < 512; iter++) {
      fft_input_i[iter] = 0;
    }
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
//    extract_fft_output(fft_input_r, fft_input_i, mfcc_input, MFCC_SIZE); 
    for (iter = 0; iter < 257; iter++) {
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
    printf("sqrt result:\n");
    for (iter = 0; iter < 257; iter++) {
      float float_result = sqrt((float)(pow(fft_input_r[iter], 2) + pow(fft_input_i[iter], 2)));
      printf("%d %.2f\n", mfcc_input[iter], float_result);
    }
    printf("\n");
    for (ii = 0; ii < 26; ii++) 
      mfcc_output[ii] = mfcc_input[ii];
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
    printf("log+dct result:\n");
    for (iter = 0; iter < 13; iter++) {
      float float_result = 0;
      for (ii = 0; ii < 26; ii++) {
        float log_result;
        if (mfcc_output[ii] > 0)
          log_result = log(mfcc_output[ii]);
        else
          log_result = 0;
        float_result += log_result * cos((iter + 1) * (PI * (ii + 0.5) / 26));
      }
      printf("%d %.2f\n", mfcc_output[iter], float_result);
    }
    printf("\n");
    /*
    for (iter = 0; iter < 13; iter++)
      printf("%d\t", dct_output[iter]);
    printf("\n");
    */
    return 1;
 		// done
}
