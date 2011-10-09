#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"
#include <signal.h>

/////////////////////////////
// SIMULATOR CONFIGURATION //
/////////////////////////////

#include "cpu/memmap.h"

#define ROMSIZE (ROMTOP - ROMBOT) // In bytes
#define RAMSIZE (RAMTOP - RAMBOT) // In bytes

/////////////////
// ERROR CODES //
/////////////////

// Internal Simulator Errors
#define E_NOT_IMPLEMENTED	1
#define E_BAD_FLASH		2
#define E_UNKNOWN		3

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
	void (*fn) (uint32_t);
	const char *name;
};

//////////////////////
// EXPORTED SYMBOLS //
//////////////////////

// Simulator state
extern int cycle;

// Simulator synchronization
extern sem_t ticker_ready_sem;
extern sem_t start_tick_sem;
extern sem_t end_tick_sem;
extern sem_t end_tock_sem;

// Head of the registered opcodes list
extern struct op *ops;

// Some configuration flags
extern int printcycles;
extern int raiseonerror;

// Latchable state
void state_write(uint32_t *loc, uint32_t val);
void state_write_op(struct op **loc, struct op *val);

/////////////////////////
// PRETTY PRINT MACROS //
/////////////////////////

#define INFO(...)\
	do {\
		flockfile(stdout);\
		printf("--- I: "); printf(__VA_ARGS__);\
		funlockfile(stdout);\
	} while (0)
#define WARN(...)\
	do {\
		flockfile(stderr);\
		fprintf(stderr, "--- W: "); fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr);\
	} while (0)
#define ERR(_e, ...)\
	do {\
		flockfile(stderr);\
		fprintf(stderr, "--- E: ");\
		fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr);\
		if (raiseonerror) raise(SIGTRAP);\
		exit(_e);\
	} while (0)

#endif // SIMULATOR_H
