/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef STATE_ASYNC_H
#define STATE_ASYNC_H

#ifndef PP_STRING
#define PP_STRING "STa"
#include "pretty_print.h"
#endif

#define DIRECT_STATE_H_CHECK
#include "state.h"

// Latchable state
#define SR_A(_l) state_read_async((_l))
export_inline uint32_t state_read_async(uint32_t *loc) __attribute__ ((nonnull));
export_inline uint32_t state_read_async(uint32_t *loc) {
	return *loc;
}
#define SRP_A(_l) state_read_p_async((_l))
export_inline uint32_t* state_read_p_async(uint32_t **loc) __attribute__ ((nonnull));
export_inline uint32_t* state_read_p_async(uint32_t **ploc) {
	return *ploc;
}

#ifdef DEBUG1
#define SW_A(_l, _v) state_write_async_dbg((_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_async_dbg(uint32_t *loc, uint32_t val,
		const char *file, const char *func,
		const int line, const char *target) __attribute__ ((nonnull));
#define SWP_A(_l, _v) state_write_p_async_dbg((_l), (_v),\
		__FILE__, __func__, __LINE__, VAL2STR(_l))
void state_write_p_async_dbg(uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target)
			__attribute__ ((nonnull (1, 3, 4, 6)));
#else
#define SW_A(_l, _v) state_write_async((_l), (_v))
void state_write_async(uint32_t *loc, uint32_t val) __attribute__ ((nonnull));
#define SWP_A(_l, _v) state_write_p_async((_l), (_v))
void state_write_p_async(uint32_t **ploc, uint32_t *pval) __attribute__ ((nonnull (1)));
#endif

void state_assert_interrupt_async(unsigned interrupt);

#endif // STATE_ASYNC_H
