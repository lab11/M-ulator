/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef DIRECT_STATE_H_CHECK
#error "Do not #include state.h directly, rather use state_sync or state_async"
#endif

#ifndef STATE_H
#define STATE_H

#include "common.h"

#ifndef PP_STRING
#define PP_STRING "ST-"
#include "pretty_print.h"
#endif

bool state_is_debugging(void);
void state_start_tick(void);
void state_tock(void);
#ifndef NO_PIPELINE
void state_pipeline_flush(uint32_t new_pc);
#endif

/** Handle a state exception, if there is any.
 *
 * Handles exceptions like if the pipeline needs to be flushed.
 *
 * @returns True if an exception was handled, false otherwise.
 */
bool state_handle_exceptions(void);

void state_async_block_start(void);
void state_async_block_end(void);

void state_enter_debugging(void);
void state_exit_debugging(void);

int state_seek_for_calling_thread(int);

#endif // STATE_H
