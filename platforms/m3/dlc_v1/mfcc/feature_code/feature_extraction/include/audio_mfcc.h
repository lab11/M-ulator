#ifndef AUDIOMFCC_H
#define AUDIOMFCC_H

#include "common.h"

#define HZ2MEL(x) (1127.0*log(1+(x/700.0)))
#define MEL2HZ(x) (700.0*(exp(x/1127.0)-1))

void mfcc_init(mfcc_params_t* mfccparams, global_params_t* gbparams, int nfilters, int ncoeff, float minhz, float maxhz);

void mfcc_extract(mfcc_params_t* mfccparams, frame_t* frame);

#endif /* AUDIOMFCC_H */
