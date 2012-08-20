#ifndef STATE_H
#define STATE_H

#include "common.h"

enum stage {
	PRE  = 0x1,
	IF   = 0x2,
	ID   = 0x4,
	EX   = 0x8,
	PIPE = 0x10,
	SIM  = 0x20,
	PERIPH = 0x40,
	UNK  = 0x80,
	/* MAX: 0xff */
};

#ifndef STAGE
#pragma message "Assigning UNK stage"
#define STAGE UNK
#endif

// Not 100% sure this is appropriate here, but no callers of this yet
// XXX: Moved in core refactor - consider final placement
void stall(enum stage);

void state_enter_debugging(void);
void state_exit_debugging(void);

// Latchable state
#define SR(_l) state_read(STAGE, (_l))
uint32_t state_read(enum stage, uint32_t *loc) __attribute__ ((nonnull));
#define SR_A(_l) state_read_async(STAGE, (_l))
uint32_t state_read_async(enum stage, uint32_t *loc) __attribute__ ((nonnull));
#define SRP(_l) state_read_p(STAGE, (_l))
uint32_t* state_read_p(enum stage, uint32_t **loc) __attribute__ ((nonnull));
#ifdef DEBUG1
#define SW(_l, _v) state_write_dbg(STAGE, (_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_dbg(enum stage, uint32_t *loc, uint32_t val,
		const char *file, const char *func,
		const int line, const char *target) __attribute__ ((nonnull));
#define SW_A(_l, _v) state_write_async_dbg(STAGE, (_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_async_dbg(enum stage, uint32_t *loc, uint32_t val,
		const char *file, const char *func,
		const int line, const char *target) __attribute__ ((nonnull));
#define SWP(_l, _v) state_write_p_dbg(STAGE, (_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_p_dbg(enum stage, uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target)
			__attribute__ ((nonnull (2, 4, 5, 7)));
#else
#define SW(_l, _v) state_write(STAGE, (_l), (_v))
void state_write(enum stage, uint32_t *loc, uint32_t val)
	__attribute__ ((nonnull));
#define SW_A(_l, _v) state_write_async(STAGE, (_l), (_v))
void state_write_async(enum stage, uint32_t *loc, uint32_t val)
	__attribute__ ((nonnull));
#define SWP(_l, _v) state_write_p(STAGE, (_l), (_v))
void state_write_p(enum stage, uint32_t **ploc, uint32_t *pval)
	__attribute__ ((nonnull (2)));
#endif

#endif // STATE_H
