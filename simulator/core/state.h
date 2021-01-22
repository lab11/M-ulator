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

/** Return whether the debugging state is active.
 *
 * Debugging state is affected by state_enter_debugging() and
 * state_exit_debugging().
 *
 * @return True if debugging is active, false otherwise.
 */
bool state_is_debugging(void);

/** Start/reset the write state tracking.
 *
 * Internally, it merely resets the index to the write state tracker to 0.
 *
 * If replay is enabled, this manages the linked list of states. It adds the
 * current state to the linked list, invalidating any states that are newer
 * than the current one (if there are any, this happens if we're re-executing
 * after rewinding). The current cycle count is recorded in the state.
 */
void state_start_tick(void);

/** Update the simulator state based on the current write state.
 *
 * Specficially, it goes through all write states that have not been committed
 * simce the last start_start_tick call and writes the value specified in the
 * write state to the specified location (there are two variants of this-- if
 * state_change.loc is set, the state_change.val variable is written to
 * *state_change.loc, else the pointer at state_change.pval is written to the
 * location pointed to by state_change.ploc).
 */
void state_tock(void);

#ifndef NO_PIPELINE
/** Signal the emulator to start flushing the pipeline.
 *
 * This sets a flag that the thread managing the pipeline watches for to start
 * flushing the pipeline.
 *
 * @param[in] new_pc The new PC value to set the simulator to once flushing is
 *  completed.
 */
void state_pipeline_flush(uint32_t new_pc);
#endif

/** Handle a state exception, if there is any.
 *
 * Handles exceptions like if the pipeline needs to be flushed.
 *
 * @returns True if an exception was handled, false otherwise.
 */
bool state_handle_exceptions(void);

/** (TODO) Mark the beginning of an async device atomic block.
 *
 * Not implemented yet.
 * In a pathological case, async devices could, in theory, commit what should
 * be an atomic operation for them across two CPU cycles, thus the simulator
 * state tracking will see these as two operations instead of one. By defining
 * async atomic blocks, all operations between the start and stop of these are
 * kept within the same state slice/window, to allow for safe rollbacks and
 * fastforwarding.
 */
void state_async_block_start(void);

/** (TODO) Mark the end of an async device atomic block.
 *
 * See state_async_block_start() for a description.
 */
void state_async_block_end(void);

/** Set the emulator's debug flag to true.
 *
 * When this is set, other components monitoring this flag should enable their
 * state debugging paths, by, for example, flushing state changes immediately
 * instead of caching them.
 */
void state_enter_debugging(void);

/** Set the emulator's debug flag to false.
 *
 * See state_enter_debugging(). This does the opposite, signaling to other
 * components to disable their state debugging paths.
 */
void state_exit_debugging(void);

/** Seek through the simulation state to the specified target cycle, if
 * possible.
 *
 * This replays the history going backwards or forwards, if it exists.
 *
 * @return The cycle seeked to.
 */
int state_seek_for_calling_thread(int target_cycle);

#endif // STATE_H
