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

#include "cpu/memmap.h"

/////////////////
// ERROR CODES //
/////////////////

#define E_BAD_FLASH	10
#define E_READONLY	11
#define E_WRITEONLY	12

////////////////////
// INTERNAL TYPES //
////////////////////

struct op {
	struct op *prev;
	struct op *next;
	uint32_t ones_mask;
	uint32_t zeros_mask;
	int ex_cnt;
	uint32_t *ex_ones;
	uint32_t *ex_zeros;
	bool is16;
	union {
		void (*fn32) (uint32_t);
		void (*fn16) (uint16_t);
	} fn;
	const char *name;
};

//////////////////////
// EXPORTED SYMBOLS //
//////////////////////

// The simulator core
void simulator(const char* flash_file);
void sim_terminate(void) __attribute__ ((noreturn));
int state_seek(int);

// Simulator config
extern int gdb_port;
extern int slowsim;
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
extern sem_t ticker_ready_sem;
extern sem_t start_tick_sem;
extern sem_t end_tick_sem;
extern sem_t end_tock_sem;

// XXX: Oh.. so hacky. Thrown in as stopgap while removing unnecessary
// references to simulator.h
#ifdef STAGE
// Head of the registered opcodes list
extern struct op *ops;
void state_write_op(enum stage, struct op **loc, struct op *val)
			__attribute__ ((nonnull (2)));
#endif

#endif // SIMULATOR_H
