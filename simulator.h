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

// The simulator core
void simulator(const char* flash_file, int polluartport);

// Simulator config
#define POLL_UART_PORT 4100
#define POLL_UART_BUFSIZE 16
#define POLL_UART_BAUD 1200
extern int slowsim;
extern int printcycles;
extern int raiseonerror;
extern int showledwrites;
extern int dumpatpc;
extern int dumpatcycle;
extern int dumpallcycles;
extern int returnr0;
extern int usetestflash;

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
#define SR(_l) state_read((_l))
uint32_t state_read(uint32_t *loc);
#define SR_A(_l) state_read_async((_l))
uint32_t state_read_async(uint32_t *loc);
#define SRP(_l) state_read_p((_l))
uint32_t* state_read_p(uint32_t **loc);
#ifdef DEBUG1
#define SW(_l, _v) state_write_dbg((_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_dbg(uint32_t *loc, uint32_t val,
		const char *file, const char *func,
		const int line, const char *target);
#define SW_A(_l, _v) state_write_async_dbg((_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_async_dbg(uint32_t *loc, uint32_t val,
		const char *file, const char *func,
		const int line, const char *target);
#define SWP(_l, _v) state_write_p_dbg((_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_p_dbg(uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target);
void state_write_op(struct op **loc, struct op *val);
#else
#define SW(_l, _v) state_write((_l), (_v))
void state_write(uint32_t *loc, uint32_t val);
#define SW_A(_l, _v) state_write_async((_l), (_v))
void state_write_async(uint32_t *loc, uint32_t val);
#define SWP(_l, _v) state_write_p((_l), (_v))
void state_write_p(uint32_t **ploc, uint32_t *pval);
void state_write_op(struct op **loc, struct op *val);
#endif

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
#define TRAP(...)\
	do {\
		flockfile(stderr);\
		fprintf(stderr, "--- T: ");\
		fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr);\
		raise(SIGTRAP);\
	} while (0)

#endif // SIMULATOR_H
