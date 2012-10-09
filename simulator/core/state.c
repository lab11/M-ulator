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

#define DIRECT_STATE_H_CHECK

#include "state.h"
#include "state_sync.h"
#include "state_async.h"

#include "simulator.h"
#include "id_stage.h"
#include "pipeline.h"

// state flags #defines:
// Bottom byte aligns with enum stage
#define STATE_STALL_PRE		PRE
#define STATE_STALL_IF		IF
#define STATE_STALL_ID		ID
#define STATE_STALL_EX		EX
#define STATE_STALL_PIPE	PIPE
#define STATE_STALL_SIM		SIM
#define STATE_STALL_UNK		UNK
#define STATE_STALL_MASK	0xff

#define STATE_IO_BARRIER	    0x100
#ifndef NO_PIPELINE
#define STATE_PIPELINE_FLUSH	   0x1000
#define STATE_PIPELINE_RUNNING	   0x8000
#endif
#define STATE_PERIPH_WRITTEN	  0x10000
#define STATE_BLOCKING_ASYNC	 0x800000

#define STATE_DEBUGGING		0x1000000

struct state_change {
	struct state_change *prev;
	struct state_change *next;

	/* Holds changes made __during__ this cycle; there may be multiple
	   list entries per cycle. Thus, to return to the beginning of a
	   cycle n (e.g. before it had executed), the list must have traversed
	   such that the list head reaches an entry of cycle less than n */
	int cycle;
	enum stage g;
	uint32_t *flags;
	uint32_t *loc;
	uint32_t val;
	uint32_t prev_val;
	uint32_t **ploc;
	uint32_t *pval;
	uint32_t *prev_pval;
#ifdef DEBUG1
	const char* file;
	const char* func;
	int line;
	const char* target;
#endif
};

static uint32_t state_start_flags = 0;
static struct state_change state_start = {
	.prev = NULL,
	.next = NULL,
	.cycle = -1,
	.g = UNK,
	.flags = &state_start_flags,
	.loc = NULL,
	.val = 0,
	.prev_val = 0,
	.ploc = NULL,
	.pval = NULL,
	.prev_pval = NULL,
#ifdef DEBUG1
	.file = NULL,
	.func = NULL,
	.line = -1,
	.target = NULL,
#endif
};

static struct state_change* state_head = &state_start;
static struct state_change* cycle_head = NULL;

static uint32_t* state_flags_cur = &state_start_flags;

static struct op* o_hack = NULL;

#ifndef NO_PIPELINE
static uint32_t state_pipeline_new_pc = -1;
#endif

#ifdef DEBUG1
static pthread_mutex_t state_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
static pthread_mutex_t async_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
#else
static pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t async_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

EXPORT bool state_is_debugging(void) {
	return *state_flags_cur & STATE_DEBUGGING;
}

EXPORT void state_async_block_start(void) {
	pthread_mutex_lock(&async_mutex);
	pthread_mutex_lock(&state_mutex);
	DBG2("async_block started\n");
	*state_flags_cur |= STATE_BLOCKING_ASYNC;
	pthread_mutex_unlock(&state_mutex);
}

EXPORT void state_async_block_end(void) {
	pthread_mutex_lock(&state_mutex);
	*state_flags_cur &= ~STATE_BLOCKING_ASYNC;
	pthread_mutex_unlock(&state_mutex);
	pthread_mutex_unlock(&async_mutex);
	DBG2("async_block ended\n");
}

static void state_block_async_io(void) {
	pthread_mutex_lock(&async_mutex);
#ifdef DEBUG1
	assert(0 == pthread_mutex_trylock(&state_mutex));
	pthread_mutex_unlock(&state_mutex);
#endif
	*state_flags_cur |= STATE_BLOCKING_ASYNC;
}

static void state_unblock_async_io(void) {
	*state_flags_cur &= ~STATE_BLOCKING_ASYNC;
	pthread_mutex_unlock(&async_mutex);
}

EXPORT void state_start_tick(void) {
	state_block_async_io();
#ifdef DEBUG2
if (debug_ >= 2) {
	flockfile(stdout); flockfile(stderr);
	printf("\n%08d TICK TICK TICK TICK TICK TICK TICK TICK TICK\n", cycle);
	funlockfile(stderr); funlockfile(stdout);
}
#endif
	DBG2("CLOCK --> high (cycle %08d)\n", cycle);
	//assert((state_head->cycle+1) == cycle);
	cycle_head = state_head;

	if (NULL != state_head->next) {
		WARN("Simulator re-excuting at cycle %d\n", cycle);
		WARN("Discarding all future state\n");

		struct state_change* s = state_head->next;
		while (s->next != NULL) {
			s = s->next;
			if (s->cycle != s->prev->cycle)
				free(s->prev->flags);
			free(s->prev);
		}
		free(s->flags);
		free(s);

		state_head->next = NULL;
	}

	state_flags_cur = calloc(1, sizeof(uint32_t));
	assert((NULL != state_flags_cur) && "OOM");

	o_hack = NULL;

	state_unblock_async_io();
}

static void _state_tock(void) {
	if (!(*state_flags_cur & STATE_STALL_ID)) {
		if (cycle > 0) {
			assert((NULL != o_hack) &&
					"nothing set instruction for EX this cycle");
		}
		id_ex_o = o_hack;
	}

	struct state_change* orig_cycle_head = cycle_head;

	while (NULL != cycle_head) {
		assert(cycle_head->cycle == cycle);
		if (cycle_head->g & *cycle_head->flags) {
#ifdef DEBUG1
			// This write has been stalled, skip it
			;
#else
			// This write has been stalled, remove it
			struct state_change* delete_me = cycle_head;

			// shouldn't be able to stall most initial state
			// XXX: what about after a replay? (no... this is ok)
			assert(NULL != cycle_head->prev);
			cycle_head->prev->next = cycle_head->next;
			// but we could be the last state...
			if (NULL != cycle_head->next) {
				cycle_head->next->prev = cycle_head->prev;
			}
			// update checking bookkeeping
			if (cycle_head == orig_cycle_head) {
				orig_cycle_head = cycle_head->next;
				if (NULL == orig_cycle_head) {
					WARN("stall generated empty cycle\n");
				}
			}
			// back the head up so the loop can advance it
			cycle_head = cycle_head->prev;
			free(delete_me);
#endif
		}
		else if (cycle_head->loc) {
			DBG2("(%s::%s:%d: %s: %p = %08x (was %08x)\n",
					cycle_head->file, cycle_head->func,
					cycle_head->line, cycle_head->target,
					cycle_head->loc, cycle_head->val,
					cycle_head->prev_val);
			*cycle_head->loc = cycle_head->val;
		} else if (cycle_head->ploc) {
			DBG2("(%s::%s:%d: %s: %p = %p (was %p)\n",
					cycle_head->file, cycle_head->func,
					cycle_head->line, cycle_head->target,
					cycle_head->ploc, cycle_head->pval,
					cycle_head->prev_pval);
			*cycle_head->ploc = cycle_head->pval;
		} else {
			ERR(E_UNKNOWN, "loc and ploc NULL?\n");
		}
		cycle_head = cycle_head->next;
	}

	// Not going to do better than O(2n) for detecting duplicates in an
	// unsorted list
	while (NULL != orig_cycle_head) {
		if (orig_cycle_head->loc) {
#ifdef NO_PIPELINE
			// Allow the PC to alias for branches
			if (orig_cycle_head->loc == &pre_if_PC) {
				orig_cycle_head = orig_cycle_head->next;
				continue;
			}
#endif
			if (*orig_cycle_head->loc != orig_cycle_head->val) {
#ifdef DEBUG1
				ERR(E_UNPREDICTABLE, "(%s): %p was aliased, expected %08x, got %08x\n",
						orig_cycle_head->target,
#else
				ERR(E_UNPREDICTABLE, "loc %p was aliased, expected %08x, got %08x\n",
#endif
						orig_cycle_head->loc,
						orig_cycle_head->val,
						*orig_cycle_head->loc
#ifdef DEBUG1 // Appease ()'s balance
				   );
#else
				   );
#endif
			}
		} else {
			// But async stuff is allowed to alias
			;
		}
		orig_cycle_head = orig_cycle_head->next;
	}
}

EXPORT void state_tock(void) {
	state_block_async_io();

#ifdef DEBUG2
if (debug_ >= 2) {
	flockfile(stdout); flockfile(stderr);
	printf("\n%08d TOCK TOCK TOCK TOCK TOCK TOCK TOCK TOCK TOCK\n", cycle);
	funlockfile(stderr); funlockfile(stdout);
}
#endif
	DBG2("CLOCK --> low (cycle %08d)\n", cycle);
	// cycle_head is state_head from end of previous cycle here
	cycle_head = cycle_head->next;
	assert((NULL != cycle_head) && "nothing written this cycle?");

	_state_tock();

#ifndef NO_PIPELINE
	if (*state_flags_cur & STATE_PIPELINE_FLUSH) {
		// Leverage the existing state mechanism to simplify replay,
		// conceptually adding extra writes to the end of this cycle
		// that alias the previous writes to actually flush the pipeline
		cycle_head = state_head;
		*cycle_head->flags |= STATE_PIPELINE_RUNNING;
		o_hack = NULL;

		pipeline_flush(state_pipeline_new_pc);

		cycle_head = cycle_head->next;

		_state_tock();

		*state_flags_cur &= ~STATE_PIPELINE_RUNNING;
	}
#endif

	if (*state_flags_cur & STATE_PERIPH_WRITTEN) {
		// XXX: Replace when periphs are split from sim core
		//print_periphs();
	}

	state_unblock_async_io();
}

EXPORT uint32_t state_read(enum stage g __attribute__ ((unused)),
		uint32_t *loc) {
	return *loc;
}

EXPORT uint32_t state_read_async(enum stage g __attribute__ ((unused)),
		bool in_block, uint32_t *loc) {
	uint32_t ret;
	if (!in_block)
		pthread_mutex_lock(&async_mutex);
	pthread_mutex_lock(&state_mutex);
	ret = state_read(g, loc);
	if (!in_block)
		pthread_mutex_unlock(&async_mutex);
	pthread_mutex_unlock(&state_mutex);
	return ret;
}

EXPORT uint32_t* state_read_p(enum stage g __attribute__ ((unused)),
		uint32_t **loc) {
	return *loc;
}

EXPORT uint32_t* state_read_p_async(enum stage g __attribute__ ((unused)),
		bool in_block, uint32_t **loc) {
	uint32_t *ret;
	if (!in_block)
		pthread_mutex_lock(&async_mutex);
	pthread_mutex_lock(&state_mutex);
	ret = *loc;
	if (!in_block)
		pthread_mutex_unlock(&async_mutex);
	pthread_mutex_unlock(&state_mutex);
	return ret;
}

#ifdef DEBUG1
static void _state_write_dbg(enum stage g, bool async,
		uint32_t *loc, uint32_t val,
		uint32_t** ploc, uint32_t* pval,
		const char *file, const char* func,
		const int line, const char *target) {
#else
static void _state_write(enum stage g, bool async,
		uint32_t *loc, uint32_t val,
		uint32_t** ploc, uint32_t* pval) {
#endif
	// If debugger is changing values, write them directly. We do not track
	// debugger writes (as I'm not sure the best architecture / usage model
	// around that), so for now it's quite probable / possible that writing
	// values with the debugger could do some strange things for replay.
	if (*state_flags_cur & STATE_DEBUGGING) {
		if (loc)
			*loc = val;
		else
			*ploc = pval;
		return;
	}

#ifndef DEBUG1
	// don't track state we won't write anyways, except in debug mode
	// race for flags is OK here, since this check is always racing anyway
	if (*state_flags_cur & g & STATE_STALL_MASK) {
		return;
	}
#endif
	pthread_mutex_lock(&state_mutex);

	assert((NULL != loc) || (NULL != ploc));

	// Record:
	struct state_change* s = malloc(sizeof(struct state_change));
	assert((NULL != s) && "OOM");

	DBG2("cycle: %08d\t(%s): loc %p val %08x\n",
			cycle, target, loc, val);

	s->prev = state_head;
	s->next = NULL;
	s->cycle = cycle;
	s->g = g;
	s->flags = state_flags_cur;
	s->loc = loc;
	s->val = val;
	s->prev_val = (loc) ? *loc : 0;
	s->ploc = ploc;
	s->pval = pval;
	s->prev_pval = (ploc) ? *ploc : NULL;
#ifdef DEBUG1
	s->file = file;
	s->func = func;
	s->line = line;
	s->target = target;
#endif
	state_head->next = s;
	state_head = s;

#ifdef NO_PIPELINE
	if (true) {
		DBG1("  SW: %s:%d\t%s = %08x\n", file, line, target, val);
#else
	if (async) {
#endif
		if (loc) {
			*loc = val;
		} else {
			assert(NULL != ploc);
			*ploc = pval;
		}
	}

	if (g & PERIPH)
		*state_flags_cur |= STATE_PERIPH_WRITTEN;

	pthread_mutex_unlock(&state_mutex);
}

#ifdef DEBUG1
EXPORT void state_write_dbg(enum stage g, uint32_t *loc, uint32_t val,
		const char *file, const char* func,
		const int line, const char *target) {
	_state_write_dbg(g, false, loc, val, NULL, NULL, file, func, line, target);
}

EXPORT void state_write_p_dbg(enum stage g, uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target) {
	_state_write_dbg(g, false, NULL, 0, ploc, pval, file, func, line, target);
}

EXPORT void state_write_async_dbg(enum stage g, bool in_block,
		uint32_t *loc, uint32_t val,
		const char *file, const char* func,
		const int line, const char *target) {
	if (!in_block)
		pthread_mutex_lock(&async_mutex);
	_state_write_dbg(g, true, loc, val, NULL, NULL, file, func, line, target);
	if (!in_block)
		pthread_mutex_unlock(&async_mutex);
}

EXPORT void state_write_p_async_dbg(enum stage g, bool in_block,
		uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target) {
	if (!in_block)
		pthread_mutex_lock(&async_mutex);
	_state_write_dbg(g, true, NULL, 0, ploc, pval, file, func, line, target);
	if (!in_block)
		pthread_mutex_unlock(&async_mutex);
}
#else
EXPORT void state_write(enum stage g, uint32_t *loc, uint32_t val) {
	_state_write(g, false, loc, val, NULL, NULL);
}

EXPORT void state_write_p(enum stage g, uint32_t **ploc, uint32_t *pval) {
	_state_write(g, false, NULL, 0, ploc, pval);
}

EXPORT void state_write_async(enum stage g, bool in_block,
		uint32_t *loc, uint32_t val) {
	if (!in_block)
		pthread_mutex_lock(&async_mutex);
	_state_write(g, true, loc, val, NULL, NULL);
	if (!in_block)
		pthread_mutex_unlock(&async_mutex);
}

EXPORT void state_write_p_async(enum stage g, bool in_block,
		uint32_t **ploc, uint32_t *pval) {
	if (!in_block)
		pthread_mutex_lock(&async_mutex);
	_state_write(g, true, NULL, 0, ploc, pval);
	if (!in_block)
		pthread_mutex_unlock(&async_mutex);
}
#endif

EXPORT void state_write_op(enum stage g __attribute__ ((unused)), struct op **loc, struct op *val) {
	pthread_mutex_lock(&state_mutex);
	// Lazy hack since every other bit of preserved state is a uint32_t[*]
	// At some point in time state saving will likely have to be generalized,
	// until then, however, this will suffice
	assert(loc == &id_ex_o);
	o_hack = val;
#ifdef NO_PIPELINE
	id_ex_o = val;
#endif
	pthread_mutex_unlock(&state_mutex);
}

EXPORT void stall(enum stage g) {
	pthread_mutex_lock(&state_mutex);
#ifdef DEBUG1
	assert((g <= STATE_STALL_MASK) && "stalling illegal stage");
#endif
	if (g & ~(PRE|IF|ID)) {
		ERR(E_UNPREDICTABLE, "Stalling for non PRE|IF|ID needs namespace fix\n");
	}
	*state_flags_cur |= g;
	pthread_mutex_unlock(&state_mutex);
}

#ifndef NO_PIPELINE
EXPORT void state_pipeline_flush(uint32_t new_pc) {
	pthread_mutex_lock(&state_mutex);
	*state_flags_cur |= STATE_PIPELINE_FLUSH;
	state_pipeline_new_pc = new_pc;
	pthread_mutex_unlock(&state_mutex);
}
#endif

EXPORT void state_enter_debugging(void) {
	pthread_mutex_lock(&state_mutex);
	*state_flags_cur |= STATE_DEBUGGING;
	pthread_mutex_unlock(&state_mutex);
}

EXPORT void state_exit_debugging(void) {
	pthread_mutex_lock(&state_mutex);
	*state_flags_cur &= ~STATE_DEBUGGING;
	pthread_mutex_unlock(&state_mutex);
}

// Returns 0 on success
// Returns >0 on tolerable error (e.g. seek past end)
// Returns <0 on catastrophic error
EXPORT int state_seek(int target_cycle) {
	// This assertion relies on *something* being written every cycle,
	// which should hold since the PC is written every cycle at least.
	// However, if we ever go cycle-accurate, much of the state_seek
	// logic will require a revisit
	assert(state_head->cycle == cycle);

	if (target_cycle > cycle) {
		DBG2("seeking forward from %d to %d\n", cycle, target_cycle);
		while (target_cycle > cycle) {
			if (state_head->next == NULL) {
				WARN("Request to seek to cycle %d failed\n",
						target_cycle);
				WARN("State is only known up to cycle %d\n",
						state_head->cycle);
				WARN("Simulator left at cycle %d\n", cycle);
				return 1;
			}

			state_head = state_head->next;
			if (state_head->loc) {
				*(state_head->loc) = state_head->val;
			} else {
				*(state_head->ploc) = state_head->pval;
			}
			if (
					(NULL == state_head->next) ||
					(state_head->next->cycle > state_head->cycle)
			   ) {
				cycle++;
			}
		}
		return 0;
	} else if (target_cycle == cycle) {
		WARN("Request to seek to current cycle ignored\n");
		return 1;
	} else {
		DBG2("seeking backward from %d to %d\n", cycle, target_cycle);
		while (state_head->cycle > target_cycle) {
			if (*state_head->flags & STATE_IO_BARRIER) {
				WARN("Cannot rewind past I/O access\n");
				WARN("Simulator left at cycle %08d\n", cycle);
				return 1;
			}

			assert(NULL != state_head->prev);

			DBG2("cycle: %d target: %d head: %d\n",
					cycle, target_cycle, state_head->cycle);

			if (state_head->loc) {
				DBG2("Resetting %p to %08x\n",
						state_head->loc, state_head->prev_val);
				*(state_head->loc) = state_head->prev_val;
			} else {
				DBG2("Resetting %p to %p\n",
						state_head->ploc, state_head->prev_pval);
				*(state_head->ploc) = state_head->prev_pval;
			}

			state_head = state_head->prev;

			if (cycle != state_head->cycle) {
				cycle = state_head->cycle;
			}
		}

		// One last bit of fixup since we don't actually track
		// the op pointer
		id_ex_o = find_op(id_ex_inst, false);

		return 0;
	}
}
