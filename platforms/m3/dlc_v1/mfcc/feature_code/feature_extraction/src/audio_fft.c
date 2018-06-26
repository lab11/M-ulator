#include "audio_fft.h"


int fft_init(global_params_t* gbparams, fft_params_t* fftparams, frame_t* frame) {
  unsigned int nstages;
  if (gbparams->windowsize > gbparams->fftsize) {
    printf("Frame size > FFT size. Please correct.\n");
    return 0;
  }
  else if (gbparams->windowsize < gbparams->fftsize) {
    ZEROPAD_FLAG = 1;
  }
  else {
    ZEROPAD_FLAG = 0;
  }
  fftparams->winsize = gbparams->windowsize;
  fftparams->fftsize = gbparams->fftsize;

  fftparams->twidr = (float*) malloc((gbparams->fftsize/2)*sizeof(float));
  fftparams->twidi = (float*) malloc((gbparams->fftsize/2)*sizeof(float));
  for (int i=0; i<gbparams->fftsize/2; i++) {
    fftparams->twidr[i] = cos(-2*PI*i/gbparams->fftsize);
    fftparams->twidi[i] = sin(-2*PI*i/gbparams->fftsize);
  }
  for (int i=0; i<=MAX_FFTSIZE; i++) {
    if ((gbparams->fftsize >> i) == 1) {
      nstages = i;
      break;
    }
  }
  fftparams->nstages = nstages;
  frame->specr = (float*) malloc(gbparams->fftsize*sizeof(float));
  frame->speci = (float*) malloc(gbparams->fftsize*sizeof(float));
  frame->spec = (float*) malloc(((gbparams->fftsize/2)+1)*sizeof(float));
  window_init(fftparams);
}

void window_init(fft_params_t* fftparams) {
  int winsize = fftparams->fftsize;
  /* Hanning Window */
  fftparams->win = (float*) malloc(winsize*sizeof(float));
  for (int i=0; i<winsize; i++) {
    fftparams->win[i] = 0.5*(1-cos(2*PI*i/(winsize-1)));
  }
}

void apply_window(fft_params_t* fftparams, float* frame) {
  int winsize = fftparams->fftsize;
  for (int i=0; i<winsize; i++) {
    frame[i] = frame[i]*fftparams->win[i];
  }
}

void bit_reverse(fft_params_t* fftparams, frame_t* frame) {
  int fftsize = fftparams->fftsize;
  int j = 0;
  int m;
  float tempr, tempi;
  for (int i=0; i<fftsize-1; i++) {
    if(i<j) {
      tempr = frame->specr[j];
      tempi = frame->speci[j];
      frame->specr[j] = frame->specr[i];
      frame->speci[j] = frame->speci[i];
      frame->specr[i] = tempr;
      frame->speci[i] = tempi;
    }
    m = fftsize/2;
    while(m<=j) {
      j -= m;
      m /= 2;
    }
    j += m;
  }
}

void fft_calculate(fft_params_t* fftparams, frame_t* frame) {
  unsigned int stage, nstages, twidincr, incr, fftsize, butt, nbutts, winsize;
  float temp1r, temp1i, temp2r, temp2i;
  nstages = fftparams->nstages;
  fftsize = fftparams->fftsize;
  winsize = fftparams->winsize;
  if(ZEROPAD_FLAG==0) {
    memcpy(frame->specr,frame->raw,fftsize*sizeof(float));
    memset(frame->speci,0,fftsize*sizeof(float));
    apply_window(fftparams,frame->specr);
  }
  else {
    memcpy(frame->specr,frame->raw,winsize*sizeof(float));
    memset(frame->specr+winsize,0,(fftsize-winsize)*sizeof(float));
    memset(frame->speci,0,fftsize*sizeof(float));
    apply_window(fftparams,frame->specr);
  }
  /////////////////////////
  /// caogao
  printf("## fft inputs ##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%f\t", frame->specr[i]); 
  }
  printf("\n"); 
  printf("## fft parameter ##\n"); 
  for (int i=0; i<fftsize; i++) {
    printf("%f\t", fftparams->twidr[i]); 
  }
  printf("\n");
  /////////////////////////
  twidincr = 1;
  nbutts = fftsize/2;
  incr = fftsize/2;
  for (stage=1; stage<=nstages; stage++) {
//    printf("stage%d:\n", stage);
    int idx = 0;
    for (int i=0; i<(1<<(stage-1)); i++) {
//    printf("i%d:\n", i);
      int tw = 0;
      for (butt=0; butt<incr; butt++) {
	temp1r = frame->specr[idx] + frame->specr[idx+incr];
	temp1i = frame->speci[idx] + frame->speci[idx+incr];
	temp2r = frame->specr[idx] - frame->specr[idx+incr];
	temp2i = frame->speci[idx] - frame->speci[idx+incr];
	frame->specr[idx] = temp1r;
	frame->speci[idx] = temp1i;
	frame->specr[idx+incr] = (temp2r*fftparams->twidr[tw]) - (temp2i*fftparams->twidi[tw]);
	frame->speci[idx+incr] = (temp2r*fftparams->twidi[tw]) + (temp2i*fftparams->twidr[tw]);
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
  bit_reverse(fftparams,frame);
  for (int i=0; i<(fftsize/2)+1; i++) {
    frame->spec[i] = sqrt((frame->specr[i]*frame->specr[i])+(frame->speci[i]*frame->speci[i]));
  }
  /////////////////////////
  /// caogao
  printf("## fft result ##\n"); 
  for (int i=0; i<(fftsize/2)+1; i++) {
    printf("%f\t", frame->spec[i]); 
  }
  printf("\n");
  /////////////////////////
}


