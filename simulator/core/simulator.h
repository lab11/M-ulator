/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"

#ifndef PP_STRING
#define PP_STRING "---"
#include "pretty_print.h"
#endif

/////////////////////////////
// SIMULATOR CONFIGURATION //
/////////////////////////////

#include MEMMAP_HEADER

/////////////////
// ERROR CODES //
/////////////////

#define E_BAD_FLASH	10
#define E_READONLY	11
#define E_WRITEONLY	12

//////////////////////
// EXPORTED SYMBOLS //
//////////////////////

// The simulator core

/** Run simulator.
 *
 * Only one instance of the emulator is supported.
 *
 * This function sets up the simulator Sets up thread name, tries reading in
 * the flash ROM for the simulation (fails if this is not possible or can't be
 * loaded through GDB), sets up signal handling (spins off a thread to deal
 * with SIGINT, and ignores SIGPIPE), then launches a thread per peripheral
 * (which are configured prior to calling this function). If pipelining is
 * enabled, the pipeline is also initialized, and then the simulation is
 * finally started.
 *
 * @param[in] flash_file Path to the file to use as flash memory. If NULL, if
 *  GDB is attached the program will warn that it is still possible to run the
 *  simulator if one is loaded through GDB, else it will exit the program with
 *  an error.
 *
 * @post The simulator is running. This function will not return unless the
 *  simulator crashes or errors out.
 */
void simulator(const char* flash_file);

/** Terminates the simulator.
 *
 * This can only be called by one thread. If any other thread calls it at the
 * same time, the function will trigger the program to exit with a failure
 * condition.
 *
 * Under normal operation, this function logs the number of cycles the
 * simulator executed, the number of cycles spent accessing unaligned memory,
 * and if the emulator had been started previously, a rough guess of the
 * average frequency of the simulator (number of cycles / time elapsed).
 *
 * If the emulator is being asked to exit, this function exits the program
 * either with SUCESS or with the value in r0 (if it's configured to do so).
 *
 * @param[in] should_exit Whether the simulator should cause the program to
 *  exit or not.
 *
 * @post If should_exit is true, the program will end. Else, the simulator is
 *  stopped/finished and the function returns.
 */
void sim_terminate(bool should_exit);

// FIXME This one is in state.c. Does this matter?
/** Handle a state exception, if there is any.
 *
 * Handles exceptions like if the pipeline needs to be flushed.
 *
 * @returns True if an exception was handled, false otherwise.
 */
bool state_handle_exceptions(void);

// Simulator config
extern int gdb_port;
extern struct timespec cycle_time;
extern int printcycles;
#ifdef HAVE_DECOMPILE
extern int decompile_flag;
#endif
#ifdef HAVE_MEMTRACE
extern int memtrace_flag;
#endif
extern unsigned dumpatpc;
extern int dumpatcycle;
extern int dumpallcycles;
extern int limitcycles;
extern int returnr0;
extern int usetestflash;

// Simulator state
extern int cycle;
#ifdef HAVE_REPLAY
/** Seek to the specified simulated cycle, if possible.
 *
 * This function allows to set the simulator state to a cycle in the past or,
 * if the cycle state has already been rewound, in the future. It updates the
 * simulator (and the pipeline if enabled) cycle to the value specified. This
 * operation can fail if the simulator and pipeline states fall out of sync. If
 * the target cycle is too far foward, it sets the cycle to the latest possible
 * cycle the simulator knows.
 *
 * @param[in] target Target cycle to seek to.
 *
 * @returns True on success, false if it was not able to.
 * @post On success, the simulator was set to the specified target cycle. On
 * failure, most likely the pipeline and simulator are no longer in sync.
 */
bool simulator_state_seek(int target);
#endif

// FIXME misnomer
/** Record the time when the simulator is no longer running (this doesn't
 * actually check that it's not running, nor does it force the simulator to
 * stop!).
 *
 * The value recorded is used to calculate the effective frequency of the
 * simulator. This function is not meant to be called when the simulator is
 * already "sleeping".
 */
void sim_sleep(void);

//FIXME misnomer
/** Reset the recorded start time of the simulator.
 *
 * The value recorded is used to calculate the effective frequency of the
 * simulator.
 */
void sim_wakeup(void);

// XXX: Oh.. so hacky. Thrown in as stopgap while removing unnecessary
// references to simulator.h
#ifdef STAGE
// Head of the registered opcodes list
extern struct op *ops;
void state_write_op(struct op **loc, struct op *val) __attribute__ ((nonnull (1)));
#endif

#endif // SIMULATOR_H
