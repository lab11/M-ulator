#ifndef UTIL_H 
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>

/*
#define WINDOW_SIZE 400
#define FFT_SIZE 512
#define MFCC_SIZE 257   // FFT_SIZE / 2 + 1
#define NFILTERS 26
#define NCOEFF 13
#define MINHZ 128
#define MAXHZ 20000
#define HZ2MEL(X) (1127.0*log(1+(x/700.0)))
*/
#define PI 3.1415926

uint32_t cut_24word(uint32_t original, bool cut_24);

/*
float do_preemphasis(int windowsize, float* inputs, float preemph_prior);

void fft_apply_window(uint16_t window_size, int16_t* frame, int16_t* win);
void prepare_dft_for_fft(uint16_t window_size, uint16_t fft_size, int16_t* raw_input, int16_t* input, uint16_t fft_num, uint16_t fft_num_bits, uint16_t dft_size);
void dft(uint16_t dft_num, uint16_t fft_size, int16_t* input_r, int16_t* input_i, int16_t* dft_r, int16_t* dft_i);
void prepare_PE_fft_input(int16_t* fft_input, int16_t* input_r_dft, int16_t* input_i_dft, uint16_t dft_size, uint16_t fft_size, uint8_t fft_prec);
void extract_fft_output(int16_t* fft_output, int16_t* mfcc_input, uint16_t mfcc_size, uint8_t fft_prec);

void dct_to_dnn(float* inputs, int length);
int find_max(float* inputs, int length);

void window_init(int16_t* win, uint16_t window_size); 
void dft_init(int16_t* dft_r, int16_t* dft_i, uint16_t dft_num);
*/
#endif
