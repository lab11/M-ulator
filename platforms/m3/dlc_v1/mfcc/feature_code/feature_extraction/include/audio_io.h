/*!
 * @file                    audio_io.h
 *
 * @brief                   Header file containing interface functions related
 *                          to audio file input/output
 *
 * @author                  xyzteam
 *
 * Operating Environment
 *           Processor         - 
 * Compiler with Version Number
 *           IDE               - 
 *           BIOS              - 
 *           Compiler          - GCC 
 *
 * Copyright Copyright (c) 2012, xyzteam.
 * All rights reserved. This document/code contains information that is
 * proprietary to xyzteam. No part of this document/code may be
 * reproduced or used in whole or part in any form or by any means-
 * graphic, electronic or mechanical without the written permission of
 * xyzteam.
 *
 *------------------------------------------------------------------------------
 */

#ifndef  AUDIO_IO_H
#define  AUDIO_IO_H



/*----------------------------------------------------------------------------*/
/*                               HEADER INCLUDES                              */
/*----------------------------------------------------------------------------*/
/* System Includes */


/* Library Includes */



/*----------------------------------------------------------------------------*/
/*                                TYPE DEFINITIONS                            */
/*----------------------------------------------------------------------------*/

/* Variables */

//Derived from libsndfile
#include <sndfile.h> 
#include "common.h"
/*----------------------------------------------------------------------------*/
/*                               CONTANTS & ENUMERATIONS                      */
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*                               MACROS                                       */
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*                               FUNCTION PROTOTYPE                           */
/*----------------------------------------------------------------------------*/

void init_io(char *file, global_params_t* gbparams);
int read_frame(global_params_t* gbparams, frame_t* frameptr);
#endif /*AUDIO_IO_H*/



