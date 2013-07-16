/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
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
void simulator(const char* flash_file);
void sim_terminate(void) __attribute__ ((noreturn));
int state_seek(int);

// Simulator config
extern int gdb_port;
extern struct timespec cycle_time;
extern int printcycles;
extern unsigned dumpatpc;
extern int dumpatcycle;
extern int dumpallcycles;
extern int limitcycles;
extern int returnr0;
extern int usetestflash;

// Simulator state
extern int cycle;

// Simulator synchronization
extern sem_t* ticker_ready_sem;
extern sem_t* start_tick_sem;
extern sem_t* end_tick_sem;
extern sem_t* end_tock_sem;

// XXX: Oh.. so hacky. Thrown in as stopgap while removing unnecessary
// references to simulator.h
#ifdef STAGE
// Head of the registered opcodes list
extern struct op *ops;
void state_write_op(enum stage, struct op **loc, struct op *val)
			__attribute__ ((nonnull (2)));
#endif

#endif // SIMULATOR_H
