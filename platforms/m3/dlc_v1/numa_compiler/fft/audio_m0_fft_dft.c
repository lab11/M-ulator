#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define WINDOW_SIZE 162
#define FFT_SIZE 16
#define MAX_FFTSIZE 13
#define PI 3.1415926

void init_input(int16_t *fft_input, uint16_t windowsize) {
  uint16_t iter;
  for (iter = 0; iter < windowsize; iter++) {
//    fft_input[iter] = (iter % 7) * 12;
    fft_input[iter] = iter;
  }
}

uint32_t pow_2(uint32_t x) {
  uint32_t temp, result;
  switch (x) {
    case 0:
      return 1; break;
    case 1:
      return 2; break;
    case 2:
      return 4; break;
    case 4:
      return 16; break;
    case 6:
      return 64; break;
    case 8:
      return 256; break;
    case 12:
      return 4096; break;
    default:
      temp = x - 2;
      result = 4;
      while (temp-- > 0)
        result *= 2; 
      return result;
      break;
  }
}

unsigned int log_2(unsigned int x) {
  unsigned int i = 0;
  while ((x / 2) >= 1){ 
    i++;
    x /= 2;
  }
  return i;
}; 

//#include <math.h>
int16_t cos_16(float in) {
  uint16_t shift = pow_2(14);   // shift after multiply with dft parameter
  float in2 = in * in;
  float in4 = in2 * in2;
  float in6 = in4 * in2;
  float result = 1 - in2 / 2 + in4 / 24 - in6 / 720; // taylor series
  return (int)result * shift;
}

int16_t sin_16(float in) {
  uint16_t shift = pow_2(14);   // shift after multiply with dft parameter
  float in2 = in * in;
  float in3 = in2 * in;
  float in5 = in3 * in2;
  float in7 = in5 * in2;
  float result = in - in3 / 6 + in5 / 120 - in7 / 5040; // taylor series
  return (int)result * shift;
}

void dft_init(int16_t* dft_r, int16_t* dft_i, uint16_t dft_num) {
  uint16_t j, k;
  for (j = 0; j < dft_num; j++) {
    for (k = 0; k < dft_num; k++) {
      dft_r[j * dft_num + k] = cos_16(-2*j*k*PI/dft_num); 
      dft_i[j * dft_num + k] = sin_16(-2*j*k*PI/dft_num); 
      printf("idx:%d dft_r:%d dft_i:%d\n", j*k, dft_r[j * dft_num + k], dft_i[j * dft_num + k]);
    }
  }
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

void prepare_dft_for_fft(uint16_t window_size, uint16_t fft_size, int16_t* raw_input, int16_t* input, uint16_t fft_num, uint16_t fft_num_bits, uint16_t dft_size) {
  uint16_t loc;
  // zero-padding
  for (loc = window_size; loc < fft_size; loc++) {
    raw_input[loc] = 0;
  }

  // bit reversal
  uint16_t base;
  for (loc = 0; loc < fft_num * 2; loc++) {
    base = int_bit_reverse(loc, fft_num_bits); // TODO: use a lookup table
//    printf("base:%d loc:%d\n", base, loc);
    for (int j = 0; j < fft_size / fft_num / 2; j++) { 
      input[loc * dft_size + j] = raw_input[base + j * fft_num * 2];
    }
  }
}

void dft(uint16_t dft_num, uint16_t fft_size, int16_t* input_r, int16_t* input_i, int16_t* dft_r, int16_t* dft_i) {
  uint16_t base, j, k;
  uint16_t shift = pow_2(14);   // shift after multiply with dft parameter
  for (base = 0; base < fft_size; base += dft_num) {
    int16_t tempr[dft_num];
    int16_t tempi[dft_num];
    for (j = 0; j < dft_num; j++) {
      tempr[j] = 0;
      tempi[j] = 0;
      for (k = 0; k < dft_num; k++) {
        tempr[j] += input_r[k+base] * dft_r[j * dft_num + k] / shift;    // input_i is 0, no need to consider 
        tempi[j] += input_r[k+base] * dft_i[j * dft_num + k] / shift; 
        printf("dft_r:%d, dft_i:%d, ", dft_r[j * dft_num + k] / shift, dft_i[j * dft_num + k] / shift); 
        printf("input_r:%d, input_i:%d\n", input_r[k+base] * dft_r[j * dft_num + k] / shift, input_r[k+base] * dft_i[j * dft_num + k] / shift); 
      }
    }
    for (int j = 0; j < dft_num; j++) {
      input_r[j+base] = tempr[j];
      input_i[j+base] = tempi[j];
    }
  }
}

void separate_groups(int16_t* inputs, int16_t* inputs_0, int16_t* inputs_1, uint16_t dft_size, uint16_t fft_size) {
  uint16_t i, in0, in1;
  bool group;
  for (i = 0, in0 = 0, in1 = 0; i < fft_size; i++) {
    group = (i / dft_size) % 2;
    if (group == 0) inputs_0[in0++] = inputs[i]; 
    else inputs_1[in1++] = inputs[i]; 
  }
}

void prepare_PE_fft_input(int16_t* fft_input, int16_t* input_r_dft, int16_t* input_i_dft, uint16_t dft_size, uint16_t fft_size, uint8_t fft_prec) {
  int16_t input_r0[fft_size / 2];
  int16_t input_r1[fft_size / 2];
  int16_t input_i0[fft_size / 2];
  int16_t input_i1[fft_size / 2];
  
  separate_groups(input_r_dft, input_r0, input_r1, dft_size, fft_size);
  separate_groups(input_i_dft, input_i0, input_i1, dft_size, fft_size);
  uint16_t i, loc;
  for (i = 0, loc = 0; i < fft_size / 2; i++) {
    fft_input[loc++] = input_r0[i];
    fft_input[loc++] = input_i0[i];
    fft_input[loc++] = input_r1[i];
    fft_input[loc++] = input_i1[i];
//    if ((fft_prec == 16) || (fft_prec == 8)) {
//      fft_input[loc++] = 0;
//      fft_input[loc++] = 0;
//    }
  }
}
///////////////////////////////////////////
void main() {
  unsigned int fft_num = FFT_SIZE / 4;    // ** depends on precision **
//  fft_num = 2;
  unsigned int fft_num_bits = log_2(fft_num) + 1; 
  unsigned int dft_size = FFT_SIZE / fft_num / 2;
  unsigned int fft_zero_pad_size = FFT_SIZE * 2 * 3 / 2;
  printf("dft_size:%d\tfft_num_bits:%d\n\n", dft_size, fft_num_bits);

  int16_t raw_fft_input[FFT_SIZE];    // size: 512 * 16 / 32 = 200 (0x100)
  int16_t fft_input[fft_zero_pad_size * 2];            // size: 512 * 16 * 2 / 32 * 1.5 = 768 (0x300)
  int16_t fft_output[fft_zero_pad_size * 2];          // size: 512 * 16 * 2 / 32 * 1.5 = 768 (0x300)
  int16_t window_parameter[WINDOW_SIZE];   // size: 400 * 16 / 32 = 200 (0xC8).
//  fft_iit(twidr_all, twidi_all);
  int16_t dft_parameter_r[dft_size * dft_size];       // size: 2 * 2 * 16 / 32 = 2 (0x2). 14 fractional bits 
  int16_t dft_parameter_i[dft_size * dft_size];

  init_input(raw_fft_input, WINDOW_SIZE);
  dft_init(dft_parameter_r, dft_parameter_i, dft_size);
  /////////////////////////
  printf("## fft inputs ##\n"); 
  for (int i=0; i<FFT_SIZE; i++) {
    printf("%d\t", (int)raw_fft_input[i]);
  }
  printf("\n"); 
  /////////////////////////
  int16_t input_r_dft[FFT_SIZE];
  int16_t input_i_dft[FFT_SIZE];
  prepare_dft_for_fft(WINDOW_SIZE, FFT_SIZE, raw_fft_input, input_r_dft, fft_num, fft_num_bits, dft_size);
  /////////////////////////
  printf("## new fft inputs ##\n"); 
  for (int i=0; i<FFT_SIZE; i++) {
    printf("%d\t", (int)input_r_dft[i]);
  }
  printf("\n"); 
  /////////////////////////
  dft(dft_size, FFT_SIZE, input_r_dft, input_i_dft, dft_parameter_r, dft_parameter_i);
  /////////////////////////
  printf("## dft outputs ##\n"); 
  for (int i=0; i<FFT_SIZE; i++) {
    printf("%d\t", input_r_dft[i]);
    printf("%d\t", input_i_dft[i]);
  }
  printf("\n"); 
  /////////////////////////
  prepare_PE_fft_input(fft_input, input_r_dft, input_i_dft, dft_size, FFT_SIZE, 16);
  /////////////////////////
  printf("## final outputs ##\n"); 
//  for (int i=0; i<fft_zero_pad_size; i++) {
  for (int i=0; i<FFT_SIZE * 2; i++) {
    printf("%d\t", fft_input[i]);
  }
  printf("\n"); 
  /////////////////////////
  int fft_no = 0;
  while (dft_size < FFT_SIZE) {
//    printf("dft_size:%d\n", dft_size);
    for (int i=0; i<FFT_SIZE; i+=dft_size*2) {   // one butterfly operation
      int twi_interval = FFT_SIZE / dft_size / 2;  // fft_num
      int twi_idx = 0;
//      printf("twi_interval:%d\n", twi_interval);
      for (int idx=0; idx<dft_size; idx++) {    // dft_size: half of butterfly length 
        float twidr = cos(-2*twi_idx*PI/FFT_SIZE);
        float twidi = sin(-2*twi_idx*PI/FFT_SIZE);
        /// debug
//        printf("## fft %d: weight##\n", dft_size); 
//        printf("%.2f\t%.2f\n", twidr, twidi); 
        /// debug
        float tempr = specr[i+idx+dft_size] * twidr - speci[i+idx+dft_size] * twidi;
        float tempi = specr[i+idx+dft_size] * twidi + speci[i+idx+dft_size] * twidr;
//        printf("%d\t%d\n", i+idx, dft_size);
//        printf("idx:%d\t%d\t%.2f\t%.2f\n", idx, twi_idx, twidr, twidi);
        specr[i+idx+dft_size] = specr[i+idx] - tempr; // has to compute this first
        specr[i+idx] += tempr;
        speci[i+idx+dft_size] = speci[i+idx] - tempi;
        speci[i+idx] += tempi;
//        printf("%.2f\t%.2f\t%.2f\t%.2f\n", specr[idx], specr[idx+dft_size], speci[idx], speci[idx+dft_size]);
 //       printf("\n");
        twi_idx += twi_interval;
      }
    }
    /// debug
//    printf("## fft %d: ##\n", dft_size); 
//    for (int i=0; i<FFT_SIZE; i++) {
//      printf("%d\t%.2f\t%.2f\n", i, specr[i], speci[i]); 
//    }
//    printf("\n");
    /// debug
    dft_size *= 2;
  }
  for (int i=0; i<FFT_SIZE; i++) {
    spec[i] = sqrt((specr[i]*specr[i])+(speci[i]*speci[i]));
  }
  /////////////////////////
  for (int i=0; i<FFT_SIZE; i++) {
    printf("%d %d ", (int)specr[i], (int)speci[i]);
  }
//  printf("## fft result: real##\n"); 
//  for (int i=0; i<FFT_SIZE; i++) {
//    if (abs(specr[i]) >= 0.01)
//    printf("%d\t%.2f\n", i, specr[i]); 
//  }
//  printf("\n");
//  printf("## fft result: img ##\n"); 
//  for (int i=0; i<FFT_SIZE; i++) {
//    if (abs(speci[i]) >= 0.01)
////    printf("%.2f\t", speci[i]); 
//    printf("%d\t%.2f\n", i, speci[i]); 
//  }
//  printf("\n");
//  printf("## fft result ##\n"); 
////  for (int i=0; i<(FFT_SIZE/2)+1; i++) {
//  for (int i=0; i<FFT_SIZE; i++) {
//    if (abs(spec[i]) >= 0.01)
////    printf("%.2f\t", spec[i]); 
//    printf("%d\t%.2f\n", i, spec[i]); 
//  }
  printf("\n");
  /////////////////////////
}
