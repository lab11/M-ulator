#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

//#define fftsize 512
#define fftsize 16
#define MAX_FFTSIZE 13
#define PI 3.1415926

int init(float* specr, float* speci) {
  for (int i=0; i<fftsize; i++) {
//    specr[i] = (i % 7) * 12; 
    specr[i] = i; 
    speci[i] = 0;
  }
}

int fft_init(float* twidr, float* twidi) {
  int nstages;
  for (int i=0; i<fftsize/2; i++) {
    twidr[i] = cos(-2*PI*i/fftsize);
    twidi[i] = sin(-2*PI*i/fftsize);
  }
  for (int i=0; i<=MAX_FFTSIZE; i++) {
    if ((fftsize >> i) == 1) {
      nstages = i;
      break;
    }
  }
//  for (int i=0; i<fftsize; i++) {
//    printf("%f\t",cos(-2*PI*i/fftsize));
//    printf("%f\t",sin(-2*PI*i/fftsize));
//  }
  return nstages;
}

void bit_reverse(float* specr, float* speci) {
  int j = 0;
  int m;
  float tempr, tempi;
  for (int i=0; i<fftsize-1; i++) {
//    printf("i:%d\tj:%d\tm:%d\n", i, j, m); 
    if(i<j) {
      tempr = specr[j];
      tempi = speci[j];
      specr[j] = specr[i];
      speci[j] = speci[i];
      specr[i] = tempr;
      speci[i] = tempi;
    }
    m = fftsize/2;
    while(m<=j) {
      j -= m;
      m /= 2;
    }
    j += m;
  }
}

unsigned int int_bit_reverse(unsigned int input, int num_bits) {
  unsigned int result = 0;
//  printf("test:%x\t%d\n", input, num_bits);
  for (int i = 0; i < num_bits; i++) {
    bool temp = (input >> (num_bits - i - 1) & 1);
//    printf("%d\t%x\n", temp, i);
    result |= temp << i;
  }
  return result;
}

// found online; not tested
unsigned int bitrev(unsigned int n, unsigned int bits)
{
    unsigned int nrev, N;
    unsigned int count;   
    N = 1<<bits;
    count = bits-1;   // initialize the count variable
    nrev = n;
    for(n>>=1; n; n>>=1)
    {
        nrev <<= 1;
        nrev |= n & 1;
        count--;
    }

    nrev <<= count;
    nrev &= N - 1;

    return nrev;
}

int new_bit_reverse(float* specr, float* speci, int fft_num) {
  int dft_size = fftsize / fft_num / 2;
  int fft_num_bits = (int)(log(fft_num) / log(2)) + 1;
  printf("bit_reverse: dft_size:%d\tfft_num_bits:%d\n\n", dft_size, fft_num_bits);
  float old_specr[fftsize];
  float old_speci[fftsize];
  for (int i=0; i<fftsize; i++) {
    old_specr[i] = specr[i];
    old_speci[i] = speci[i];
  }
  unsigned int base;
  for (unsigned int i = 0; i < fft_num * 2; i++) {
    base = int_bit_reverse(i, fft_num_bits); // TODO: use a lookup table
//    printf("result:%d\t%d\n", i, base);
    for (int j = 0; j < dft_size; j++) { 
      specr[i * dft_size + j] = old_specr[base + j * fft_num * 2];
//      printf("assign:%d\t%d\n", i + j, base + j * fft_num * 2);
      speci[i * dft_size + j] = old_speci[base + j * fft_num * 2];
    }
  }
}

void dft(float* specr, float* speci, int dft_num) {
  for (int i = 0; i < fftsize; i += dft_num) {
    float tempr[dft_num];
    float tempi[dft_num];
//    for (int j = 0; j < dft_num; j++)
//      printf("specr:%.2f\t%.2f\n", specr[i + j], speci[i + j]);
    for (int j = 0; j < dft_num; j++) {
      tempr[j] = 0;
      tempi[j] = 0;
      for (int k = 0; k < dft_num; k++) {
        tempr[j] += specr[k+i] * cos(-2*j*k*PI/dft_num) - speci[k+i] * sin(-2*j*k*PI/dft_num); 
        tempi[j] += speci[k+i] * cos(-2*j*k*PI/dft_num) + specr[k+i] * sin(-2*j*k*PI/dft_num); 
//        tempr[j] += specr[k+i] * cos(-2*j*k*PI/dft_num); 
//        tempi[j] += specr[k+i] * sin(-2*j*k*PI/dft_num); 
//        printf("cos:%.2f, sin:%.2f, ", cos(-2*j*k*PI/dft_num), sin(-2*j*k*PI/dft_num));
//        printf("spec_r:%.2f, spec_i:%.2f\n", 
//         specr[k+i] * cos(-2*j*k*PI/dft_num) - speci[k+i] * sin(-2*j*k*PI/dft_num), 
//          speci[k+i] * cos(-2*j*k*PI/dft_num) + specr[k+i] * sin(-2*j*k*PI/dft_num)); 
      }
//      printf("%.2f\t%.2f\n", tempr[j], tempi[j]);
    }
//    printf("%d\n", i);
    for (int j = 0; j < dft_num; j++) {
      specr[j+i] = tempr[j];
      speci[j+i] = tempi[j];
//      printf("%f\t%f\n\n", specr[j], speci[j]);
    }
  }
}

void main() {
//  int fft_num = fftsize / 4;
  int fft_num = fftsize / 16;

  float twidr_all[fftsize/2];
  float twidi_all[fftsize/2];
  float specr[fftsize];
  float speci[fftsize];
  float spec[fftsize];
  
  fft_init(twidr_all, twidi_all);
  init(specr, speci);
  /////////////////////////
  /// caogao
  printf("## fft inputs ##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%d\t", (int)specr[i]);
  }
  printf("\n"); 
  /////////////////////////
  new_bit_reverse(specr, speci, fft_num);
  int dft_size = fftsize / fft_num / 2;
  /////////////////////////
  printf("## bit reversed inputs for dft ##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%d\t", (int)specr[i]);
    printf("%d\t", (int)speci[i]);
  }
  printf("\n"); 
  /////////////////////////
  dft(specr, speci, dft_size);
  /////////////////////////
  printf("## dft outputs ##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%.2f\t", specr[i]);
    printf("%.2f\t", speci[i]);
  }
  printf("\n"); 
  /////////////////////////
  int fft_no = 0;
  while (dft_size < fftsize) {
    printf("dft_size:%d\n", dft_size);
    for (int i=0; i<fftsize; i+=dft_size*2) {   // one butterfly operation
      int twi_interval = fftsize / dft_size / 2;  // fft_num
      int twi_idx = 0;
//      printf("twi_interval:%d\n", twi_interval);
      for (int idx=0; idx<dft_size; idx++) {    // dft_size: half of butterfly length 
        float twidr = cos(-2*twi_idx*PI/fftsize);
        float twidi = sin(-2*twi_idx*PI/fftsize);
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
    printf("## fft %d: ##\n", dft_size); 
    for (int i=0; i<fftsize; i++) {
      printf("%d\t%.2f\t%.2f\n", i, specr[i], speci[i]); 
    }
    printf("\n");
    /// debug
    dft_size *= 2;
  }
  for (int i=0; i<fftsize; i++) {
    spec[i] = sqrt((specr[i]*specr[i])+(speci[i]*speci[i]));
  }
  /////////////////////////
    for (int i=0; i<fftsize; i++) {
      printf("%d %d ", (int)specr[i], (int)speci[i]);
    }
//  printf("## fft result: real##\n"); 
//  for (int i=0; i<fftsize; i++) {
//    if (abs(specr[i]) >= 0.01)
//    printf("%d\t%.2f\n", i, specr[i]); 
//  }
//  printf("\n");
//  printf("## fft result: img ##\n"); 
//  for (int i=0; i<fftsize; i++) {
//    if (abs(speci[i]) >= 0.01)
////    printf("%.2f\t", speci[i]); 
//    printf("%d\t%.2f\n", i, speci[i]); 
//  }
//  printf("\n");
//  printf("## fft result ##\n"); 
////  for (int i=0; i<(fftsize/2)+1; i++) {
//  for (int i=0; i<fftsize; i++) {
//    if (abs(spec[i]) >= 0.01)
////    printf("%.2f\t", spec[i]); 
//    printf("%d\t%.2f\n", i, spec[i]); 
//  }
  printf("\n");
  /////////////////////////
}
