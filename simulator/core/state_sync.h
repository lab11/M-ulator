#ifndef STATE_SYNC_H
#define STATE_SYNC_H

#ifndef PP_STRING
#define PP_STRING "STs"
#include "pretty_print.h"
#endif

#define DIRECT_STATE_H_CHECK
#include "state.h"

// Latchable state
#define SR(_l) state_read(STAGE, (_l))
uint32_t state_read(enum stage, uint32_t *loc) __attribute__ ((nonnull));
#define SRP(_l) state_read_p(STAGE, (_l))
uint32_t* state_read_p(enum stage, uint32_t **loc) __attribute__ ((nonnull));
#ifdef DEBUG1
#define SW(_l, _v) state_write_dbg(STAGE, (_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_dbg(enum stage, uint32_t *loc, uint32_t val,
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
#define SWP(_l, _v) state_write_p(STAGE, (_l), (_v))
void state_write_p(enum stage, uint32_t **ploc, uint32_t *pval)
	__attribute__ ((nonnull (2)));
#endif

#endif // STATE_SYNC_H
