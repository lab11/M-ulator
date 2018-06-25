#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define windowsize 400
#define fftsize 512
#define MAX_FFTSIZE 13
#define PI 3.1415926

int init(float* specr, float* speci) {
  for (int i=0; i<fftsize; i++) {
    specr[i] = i % 8;
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
  return nstages;
}

void bit_reverse(float* specr, float* speci) {
  int j = 0;
  int m;
  float tempr, tempi;
  for (int i=0; i<fftsize-1; i++) {
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

void main() {
  float* twidr = (float*) malloc((fftsize/2)*sizeof(float));
  float* twidi = (float*) malloc((fftsize/2)*sizeof(float));
  float* specr = (float*) malloc(fftsize*sizeof(float));
  float* speci = (float*) malloc(fftsize*sizeof(float));
  float* spec = (float*) malloc(((fftsize/2)+1)*sizeof(float));
  
  int nstages = fft_init(twidr, twidi);
  init(specr, speci);

  unsigned int stage, twidincr, incr, butt, nbutts;
  float temp1r, temp1i, temp2r, temp2i;
  /////////////////////////
  /// caogao
  printf("## fft inputs ##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%.2f\t", specr[i]); 
  }
  printf("\n"); 
  printf("## fft parameter ##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%.2f\t", twidr[i]); 
  }
  printf("\n");
  /////////////////////////
  twidincr = 1;
  nbutts = fftsize/2;
  incr = fftsize/2;
  for (stage=1; stage<=nstages; stage++) {
    int idx = 0;
    for (int i=0; i<(1<<(stage-1)); i++) {
      int tw = 0;
      for (butt=0; butt<incr; butt++) {
	temp1r = specr[idx] + specr[idx+incr];
	temp1i = speci[idx] + speci[idx+incr];
	temp2r = specr[idx] - specr[idx+incr];
	temp2i = speci[idx] - speci[idx+incr];
	specr[idx] = temp1r;
	speci[idx] = temp1i;
	specr[idx+incr] = (temp2r*twidr[tw]) - (temp2i*twidi[tw]);
	speci[idx+incr] = (temp2r*twidi[tw]) + (temp2i*twidr[tw]);
	idx++;
	tw += twidincr;
//  printf("idx%d,tw%d:\n", idx, tw);
//  printf("idx%f,tw%f:\n", temp1r, fftparams->twidr[tw]);
      }
      idx += incr;
    }
    incr = incr >> 1;
    twidincr = twidincr << 1;
  }
  bit_reverse(specr, speci);
//  for (int i=0; i<(fftsize/2)+1; i++) {
  for (int i=0; i<fftsize; i++) {
    spec[i] = sqrt((specr[i]*specr[i])+(speci[i]*speci[i]));
  }
  /////////////////////////
  /// caogao
  printf("## fft result: real##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%.2f\t", specr[i]); 
  }
  printf("\n");
  printf("## fft result: img ##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%.2f\t", speci[i]); 
  }
  printf("\n");
  printf("## fft result ##\n"); 
//  for (int i=0; i<(fftsize/2)+1; i++) {
  for (int i=0; i<fftsize; i++) {
    printf("%.2f\t", spec[i]); 
  }
  printf("\n");
  /////////////////////////
}
