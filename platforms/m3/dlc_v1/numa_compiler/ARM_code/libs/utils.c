#include "utils.h"
#include "stdint.h"
#include "stdbool.h"

////////////////////
uint32_t cut_24word(uint32_t original, bool cut_24) {
	if (!cut_24) return original & 0xffffff;			// [0:23]
	else return (original >> 24);									// [24:end]
}

/*
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

uint16_t log_2(uint32_t x) {
  uint16_t i = 0;
  while ((x / 2) >= 1){ 
    i++;
    x /= 2;
  }
  return i;
}; 

float int2float(int x, uint8_t base) {
  return (float)(x / pow_2(base));
}

int32_t ln(uint32_t x, uint8_t base) {
  return 1;
  // TODO
  // taylor series
// //  if (in < 2) {
//    t = in - 1;
//    out = t - t * t / 2 + t * t * t / 3 - t * t * t * t / 4;
////  } else {
//    t = (in - 1) / (in + 1); 
//    out = 2 * (t - t * t * t / 3 + t * t * t * t * t / 5 - t * t * t * t * t * t * t / 7); 
////  }
}

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

// copied from stackoverflow
float sqrt(float number) {
   long i;
   float x2, y;
   const float threehalfs = 1.5;

   x2 = number * 0.5;
   y  = number;
   i  = * ( long * ) &y;                     // floating point bit level hacking [sic]
   i  = 0x5f3759df - ( i >> 1 );             // Newton's approximation
   y  = * ( float * ) &i;
   y  = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration
   y  = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration
   y  = y * ( threehalfs - ( x2 * y * y ) ); // 3rd iteration

   return 1/y;
}

int16_t complex_to_real(int16_t real, int16_t imag, uint16_t base) {
  float temp = (float)(real * real / base / base) + (float)(imag * imag / base / base);
  return (int16_t)(sqrt(temp) * base);
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
////////////////////
#define preemph_rate 0.97
float do_preemphasis(int window_size, float* inputs, float preemph_prior) {
  int i;
  inputs[0] -= 0.97 * preemph_prior;
  for (i = 1; i < window_size; i++) 
    inputs[i] -= 0.97 * inputs[i - 1];
  return inputs[window_size-1];
}

void fft_apply_window(uint16_t window_size, int16_t* frame, int16_t* win) {
  int i;
  for (i = 0; i < window_size; i++)
    frame[i] = frame[i] * win[i];
}

void prepare_dft_for_fft(uint16_t window_size, uint16_t fft_size, int16_t* raw_input, int16_t* input, uint16_t fft_num, uint16_t fft_num_bits, uint16_t dft_size) {
  uint16_t loc, j;
  // zero-padding
  for (loc = window_size; loc < fft_size; loc++) {
    raw_input[loc] = 0;
  }

  // bit reversal
  uint16_t base;
  for (loc = 0; loc < fft_num * 2; loc++) {
    base = int_bit_reverse(loc, fft_num_bits); // TODO: use a lookup table
    for (j = 0; j < fft_size / fft_num / 2; j++) { 
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
      }
    }
    for (j = 0; j < dft_num; j++) {
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
    if ((fft_prec == 16) || (fft_prec == 8)) {
      fft_input[loc++] = 0;
      fft_input[loc++] = 0;
    }
  }
}

void extract_fft_output(int16_t* fft_output, int16_t* mfcc_input, uint16_t mfcc_size, uint8_t fft_prec) {
  int16_t out0;
  int16_t out1;
  uint16_t i = 0, loc = 0;
  uint16_t base = pow_2(4);  // TODO: int bits 
  while (i < mfcc_size) {
    out0 = fft_output[loc++];
    out1 = fft_output[loc++];
    mfcc_input[i++] = complex_to_real(out0, out1, base);
    if ((fft_prec == 16) || (fft_prec == 8)) { 
      if ((loc % 6) == 4) loc += 2;
    }
  }
}

void mfcc_to_dnn(int32_t* inputs, int length) {
  uint16_t i, j;
  //log
  for (i = 0; i < length; i++) {
    if (inputs[i] > 0) inputs[i] = ln(inputs[i], 10); // todo
    else inputs[i] = 0;
  }
  //dct
  for (i=0; i<26; i++) {
    for (j=0; j<13; j++) {
//      temp[i]+= frame->spec[j]*mfccparams[(i*specsize)+j];
    }
  }
  //dct2dnn: TODO
}

int find_max(float* inputs, int length) { 
  int i, max_value = 0, max_pos = 0;
  for (i = 0; i < length; i++)
    if (*(inputs + i) > max_value) {
			max_value = *(inputs + i);
      max_pos = i;
		}
  return max_pos;
}

////////////////////
void window_init(int16_t* win, uint16_t window_size) {
  uint16_t i;
  for (i = 0; i < window_size; i++)
      win[i] = 1;     // debug: todo for real windowing
}

void dft_init(int16_t* dft_r, int16_t* dft_i, uint16_t dft_num) {
  uint16_t j, k;
  for (j = 0; j < dft_num; j++) {
    for (k = 0; k < dft_num; k++) {
      dft_r[j * dft_num + k] = cos_16(-2*j*k*PI/dft_num); 
      dft_i[j * dft_num + k] = sin_16(-2*j*k*PI/dft_num); 
    }
  }
}

void dct_init() { 
    // TODO
}
*/
