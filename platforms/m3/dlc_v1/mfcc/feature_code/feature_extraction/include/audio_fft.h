#ifndef AUDIO_FFT_H
#define AUDIO_FFT_H

#include "common.h"

#define MAX_FFTSIZE 13
int ZEROPAD_FLAG;

int fft_init(global_params_t* gbparams, fft_params_t* fftparams, frame_t* frame);
void fft_calculate(fft_params_t* fftparams, frame_t* frame);

void window_init(fft_params_t* fftparams);

void apply_window(fft_params_t* fftparams, float* frame);

#endif /* AUDIO_FFT_H */ 
