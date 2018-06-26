#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
////////////
// caogao
extern int mfcc_prameter_precision;
//float float2fixed(float a, int int_bits_a, int float_bits_a);
float fixed_point_mul(float a, float b, int int_bits_a, int float_bits_a);
float fixed_point_mul_ab(float a, float b, int int_bits_a, int int_bits_b, int float_bits_a, int float_bits_b);
///////////

#define PI 3.141592653589793

typedef struct {
  int numsamples;
  int numframes;
  int samprate;
  int numchannels;
  unsigned int windowsize; //(in samples)  
  int stepsize; //(in samples)
  int fftsize;
  float preemph;
  float preemph_prior;
} global_params_t;

typedef struct {
  unsigned int fftsize;
  unsigned int winsize;
  unsigned int nstages;
  float* twidr;
  float* twidi;
  float* win;
} fft_params_t;

typedef struct {
  float* raw;
  float* spec;
  float* specr;
  float* speci;
} frame_t;

typedef struct {
  float samprate;
  int specsize;
  int nfilters;
  int ncoeff;
  float minhz;
  float maxhz;
  float *coeffs;
  float* fbankmatrix;
  float* dctmatrix;
} mfcc_params_t;


#endif /* COMMON_H */
