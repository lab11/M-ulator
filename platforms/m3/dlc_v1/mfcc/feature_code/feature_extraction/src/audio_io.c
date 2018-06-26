/*!
 * @file                    audio_io.c
 *
 * @brief                   Implementation of the audio input/output interface
 *                          
 *
 * @author                  xyzteam
 *
 * Operating Environment
 *           Processor         - 
 * Compiler with Version Number
 *           IDE               - 
 *           BIOS              - 
 *           Compiler          - 
 *
 * Copyright Copyright (c) 2011, xyzteam.
 * All rights reserved. This document/code contains information that is
 * proprietary to xyzteam. No part of this document/code may be
 * reproduced or used in whole or part in any form or by any means-
 * graphic, electronic or mechanical without the written permission of
 * xyzteam.
 *
 *------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------*/
/*                                 HEADER INCLUDES                           */
/*---------------------------------------------------------------------------*/

/* System Includes */

/* Library Includes */

/* Local Includes */
#include "audio_io.h"

/*---------------------------------------------------------------------------*/
/*                              LOCAL MACROS                                 */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                              EXTERN VARIABLES                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                              GLOBAL VARIABLES                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
SNDFILE *userfile;
SF_INFO sndfileinfo;

/*                              STATIC VARIABLES                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         STATIC FUNCTION PROTOTYPES                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                       INTERFACE FUNCTION DEFINITIONS                      */
/*---------------------------------------------------------------------------*/


void init_io(char *file, global_params_t* gbparams) {
  userfile = sf_open(file, SFM_READ, &sndfileinfo);
  gbparams->numsamples = sndfileinfo.frames;
  gbparams->samprate = sndfileinfo.samplerate;
  gbparams->numchannels = sndfileinfo.channels;
///////////////////
// caogao
//  printf("numsamples: %d\n", sndfileinfo.frames);     // 47309
//  printf("samplerate: %d\n", sndfileinfo.samplerate); // 16000
//  printf("numchannels: %d\n", sndfileinfo.channels);  // 1
///////////////////
  gbparams->windowsize = 400;
  gbparams->stepsize = 160;
  gbparams->fftsize = 512;
  gbparams->preemph = 0.97;
  gbparams->preemph_prior = 0.0;
}

int read_frame(global_params_t* gbparams, frame_t* frameptr) {
  int winsize = gbparams->windowsize;
  int stepsize = gbparams->stepsize;
  int chann = gbparams->numchannels;
  frameptr->raw = (float*) malloc(winsize*sizeof(float));
  if (chann > 1) {
    float* temp = (float*) malloc (winsize*chann*sizeof(float));
    if (sf_readf_float(userfile,temp,winsize)==winsize) {
      for (int i=0; i<winsize; i++) {
	frameptr->raw[i]=0;
	for (int k=0; k<chann; k++) {
	    frameptr->raw[i] += temp[i*chann + k];
	  }
	frameptr->raw[i] /= chann;
      }
    return 1;
    sf_seek(userfile,stepsize-winsize,SEEK_CUR);
    }
    else { 
      return 0;
    }
  }
  else {
    if (sf_readf_float(userfile,frameptr->raw,winsize) == winsize) {
      return 1;
    sf_seek(userfile,stepsize-winsize,SEEK_CUR);
    }
    else { 
      return 0;     
    }
  }
}
/* END OF FILE */
