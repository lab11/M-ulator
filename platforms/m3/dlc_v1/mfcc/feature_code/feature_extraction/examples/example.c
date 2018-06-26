#include "audio_io.h"
#include "audio_fft.h"
#include "audio_preprocess.h"
#include "audio_mfcc.h"

void main(int argc, char* argv[]) {
  global_params_t* gbparams;
  fft_params_t* fftparams;
  frame_t* frame;
  mfcc_params_t* mfccparams;
  frame = (frame_t*) malloc (sizeof(frame_t));
  gbparams = (global_params_t*) malloc(sizeof(global_params_t));
  fftparams = (fft_params_t*) malloc(sizeof(fft_params_t));
  mfccparams = (mfcc_params_t*) malloc(sizeof(mfcc_params_t));

  int mfcc_parameter_precision;
  char* wav_file;
  if (argc < 2) {
    wav_file = "../../sr584.wav";
    mfcc_parameter_precision = 4;
  } else {
    wav_file = argv[1];
  }

  init_io(wav_file,gbparams);
  fft_init(gbparams,fftparams,frame);  
  mfcc_init(mfccparams,gbparams,24,13,128.0,20000.0);
  while(read_frame(gbparams,frame) == 1) {    // 118 frames
//    do_preemphasis(gbparams,frame);
//    fft_calculate(fftparams,frame);
//    mfcc_extract(mfccparams,frame);
//    for (int i=0; i<mfccparams->ncoeff; i++) {
//      printf("%f ",mfccparams->coeffs[i]);
//    }
//    printf("\n");
  }
}
