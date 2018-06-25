#include "audio_preprocess.h"

void do_preemphasis(global_params_t* gbparams, frame_t* frame) {
  frame->raw[0] = frame->raw[0] - (gbparams->preemph*gbparams->preemph_prior);
  for (int i=1; i<gbparams->windowsize; i++) {
    frame->raw[i] = frame->raw[i] - (gbparams->preemph*frame->raw[i-1]);
  }
  gbparams->preemph_prior = frame->raw[gbparams->windowsize-1];

}
