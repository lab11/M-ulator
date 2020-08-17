/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef STATE_SYNC_H
#define STATE_SYNC_H

#ifndef PP_STRING
#define PP_STRING "STs"
#include "pretty_print.h"
#endif

#define DIRECT_STATE_H_CHECK
#include "state.h"

// Latchable state
#define SR(_l) state_read((_l))
export_inline uint32_t state_read(uint32_t *loc) __attribute__ ((nonnull));
export_inline uint32_t state_read(uint32_t *loc) {
	return *loc;
}
#define SRP(_l) state_read_p((_l))
export_inline uint32_t* state_read_p(uint32_t **loc) __attribute__ ((nonnull));
export_inline uint32_t* state_read_p(uint32_t **ploc) {
	return *ploc;
}

#ifdef DEBUG1
#define SW(_l, _v) state_write_dbg((_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_dbg(uint32_t *loc, uint32_t val,
		const char *file, const char *func,
		const int line, const char *target) __attribute__ ((nonnull));
#define SWP(_l, _v) state_write_p_dbg((_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_p_dbg(uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target)
			__attribute__ ((nonnull (1, 3, 4, 6)));
#else
#define SW(_l, _v) state_write((_l), (_v))
void state_write(uint32_t *loc, uint32_t val)
	__attribute__ ((nonnull));
#define SWP(_l, _v) state_write_p((_l), (_v))
void state_write_p(uint32_t **ploc, uint32_t *pval) __attribute__ ((nonnull (1)));
#endif

void state_wait_for_interrupt(void);

#endif // STATE_SYNC_H
