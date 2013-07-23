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
#include "opcodes.h"
#include "pipeline.h"

#if defined(__has_include)
# if __has_include(<stdatomic.h>)
#  include <stdatomic.h>
#  define HAVE_STDATOMIC
# else
#  define CLANG_ATOMIC
   typedef enum memory_order {
   	  memory_order_relaxed, memory_order_consume, memory_order_acquire,
   	    memory_order_release, memory_order_acq_rel, memory_order_seq_cst
   } memory_order;
#  define atomic_fetch_add(_p, _i) __c11_atomic_fetch_add(_p, _i, memory_order_seq_cst)
#  define atomic_store(_p, _i)     __c11_atomic_store(_p, _i, memory_order_seq_cst)
#  define atomic_load(_p)          __c11_atomic_load(_p, memory_order_seq_cst)
# endif
# if __has_include(<threads.h>)
#  include <threads.h>
# else
#  define thread_local __thread
# endif
#else
# error No C11 atomic / thread support?
#endif


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

struct state_change {
	struct state_change *prev;
	struct state_change *next;

	/* Holds changes made __during__ this cycle; there may be multiple
	   list entries per cycle. Thus, to return to the beginning of a
	   cycle n (e.g. before it had executed), the list must have traversed
	   such that the list head reaches an entry of cycle less than n */
	uint32_t *loc;
	uint32_t val;
	uint32_t **ploc;
	uint32_t *pval;
#ifdef HAVE_REPLAY
	int cycle;
	uint32_t prev_val;
	uint32_t *prev_pval;
#endif
#ifdef DEBUG1
	const char* file;
	const char* func;
	int line;
	const char* target;
#endif
};

#ifdef HAVE_REPLAY
static struct state_change state_start = {
	.prev = NULL,
	.next = NULL,
	.loc = NULL,
	.val = 0,
	.ploc = NULL,
	.pval = NULL,
#ifdef HAVE_REPLAY
	.cycle = -1,
	.prev_val = 0,
	.prev_pval = NULL,
#endif
#ifdef DEBUG1
	.file = NULL,
	.func = NULL,
	.line = -1,
	.target = NULL,
#endif // DEBUG1
};
#endif // HAVE_REPLAY

#ifndef NO_PIPELINE
static uint32_t state_pipeline_new_pc = -1;
#endif

////
#define STATE_MAX_THREADS 16
#define STATE_MAX_WRITES 32

static thread_local int state_tls_id = -1;
#ifdef HAVE_REPLAY
#else
static struct state_change writes[STATE_MAX_THREADS][STATE_MAX_WRITES];
static thread_local int state_tls_count = 0;
#endif // HAVE_REPLAY
#ifndef NO_PIPELINE
static thread_local struct op* state_next_id_ex_o = NULL;
#endif

#ifdef HAVE_STDATOMIC
static atomic_int state_thread_count = ATOMIC_VAR_INIT(0);
#elif defined(CLANG_ATOMIC)
static _Atomic(int) state_thread_count;

__attribute__ ((constructor))
void clang_state_thread_count_atomic_init(void) {
	__c11_atomic_init(&state_thread_count, 0);
}
#endif

static inline unsigned state_id(void) {
	if (unlikely(state_tls_id == -1)) {
		state_tls_id = atomic_fetch_add(&state_thread_count, 1);
#ifdef HAVE_REPLAY
#else
		if (state_tls_id == STATE_MAX_THREADS) {
			WARN("Maximum static thread count exceeded\n");
			ERR(E_UNKNOWN, "Need to increment state.c::STATE_MAX_THREADS\n");
		}
#endif
	}
	return state_tls_id;
}
////

#ifdef HAVE_STDATOMIC
static atomic_flag pipeline_flush_flag = ATOMIC_FLAG_INIT(false);
static atomic_bool debugging_bool = ATOMIC_BOOL_INIT(false);
#elif defined(CLANG_ATOMIC)
static int pipeline_flush_bool = false;
static _Atomic(_Bool) debugging_bool;

__attribute__ ((constructor))
void clang_pipeline_debugging_atomic_bools_init(void) {
	__c11_atomic_init(&debugging_bool, false);
}
#endif

EXPORT bool state_is_debugging(void) {
	return atomic_load(&debugging_bool);
}

EXPORT void state_enter_debugging(void) {
	DBG2("Entering debugging\n");
	atomic_store(&debugging_bool, true);
}

EXPORT void state_exit_debugging(void) {
	atomic_store(&debugging_bool, false);
	DBG2("Exited debugging\n");
}

EXPORT void state_async_block_start(void) {
	//WARN("nop\n");
	DBG2("async_block started\n");
}

EXPORT void state_async_block_end(void) {
	//WARN("nop\n");
	DBG2("async_block ended\n");
}

static void state_block_async_io(void) {
	//WARN("nop\n");
}

static void state_unblock_async_io(void) {
	//WARN("nop\n");
}

static void _state_start_tick(void) {
#ifdef HAVE_REPLAY
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
#else
	state_tls_count = 0;
#endif
}

EXPORT void state_start_tick(void) {
	state_block_async_io();
	_state_start_tick();
	state_unblock_async_io();
}

EXPORT bool state_handle_exceptions(void) {
	bool pipeline;
#ifdef HAVE_STDATOMIC
	pipeline = atomic_flag_test_and_set(&pipeline_flush_flag);
	atomic_flag_clear(&pipeline_flush_flag);
#elif defined(CLANG_ATOMIC)
	pipeline = __sync_lock_test_and_set(&pipeline_flush_bool, 1);
	__sync_lock_release(&pipeline_flush_bool);
#endif
	if (pipeline) {
		DBG2("Exception: Pipeline Flush\n");
		pipeline_flush(state_pipeline_new_pc);
		id_ex_o = find_op(INST_NOP);
		return true;
	}

	return false;
}

static int _state_tock(void) {
#ifdef HAVE_REPLAY
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
#else // -> !HAVE_REPLAY
	int i;
	int id = state_id();
	for (i = 0; i < state_tls_count; i++) {
		if (writes[id][i].loc != NULL)
			*(writes[id][i].loc) = writes[id][i].val;
		else
			*(writes[id][i].ploc) = writes[id][i].pval;
	}
#ifndef NO_PIPELINE
	if (state_next_id_ex_o != NULL) {
		id_ex_o = state_next_id_ex_o;
		state_next_id_ex_o = NULL;
	}
#endif // NO_PIPELINE
	return state_tls_count;
#endif // !HAVE_REPLAY
}

EXPORT int state_tock(void) {
	int ret;
	state_block_async_io();
	ret = _state_tock();
	state_unblock_async_io();
	return ret;
}

EXPORT uint32_t state_read(enum stage g __attribute__ ((unused)),
		uint32_t *loc) {
	return *loc;
}

EXPORT uint32_t state_read_async(enum stage g __attribute__ ((unused)),
		bool in_block, uint32_t *loc) {
	uint32_t ret;
//	if (!in_block)
//		pthread_mutex_lock(&async_mutex);
	ret = state_read(g, loc);
//	if (!in_block)
//		pthread_mutex_unlock(&async_mutex);
	return ret;
}

EXPORT uint32_t* state_read_p(enum stage g __attribute__ ((unused)),
		uint32_t **ploc) {
	return *ploc;
}

EXPORT uint32_t* state_read_p_async(enum stage g __attribute__ ((unused)),
		bool in_block, uint32_t **ploc) {
	uint32_t *ret;
//	if (!in_block)
//		pthread_mutex_lock(&async_mutex);
	ret = *ploc;
//	if (!in_block)
//		pthread_mutex_unlock(&async_mutex);
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
	//
	// This mechanism is also leveraged to write directly to memory when the
	// simulator is first starting up (flashing the program image).
	if (state_is_debugging()) {
		if (loc)
			*loc = val;
		else
			*ploc = pval;
		return;
	}

	assert((NULL != loc) || (NULL != ploc));

	// Record:
#ifdef HAVE_REPLAY
	struct state_change* s = malloc(sizeof(struct state_change));
	assert((NULL != s) && "OOM");
#define S s->
#else
	int s_i = state_id();
	int s_c = state_tls_count++;
	if (unlikely(s_c == STATE_MAX_WRITES)) {
		WARN("Maximum write location count exceeded\n");
		ERR(E_UNKNOWN, "Need to increment state.c::STATE_MAX_WRITES\n");
	}
#define S writes[s_i][s_c].
#endif

	DBG2("cycle: %08d\t(%s): loc %p val %08x\n",
			cycle, target, loc, val);

	//s->prev = state_head;
	//s->next = NULL;
	S loc = loc;
	S val = val;
	S ploc = ploc;
	S pval = pval;
#ifdef HAVE_REPLAY
	S cycle = cycle;
	S prev_val = (loc) ? *loc : 0;
	S prev_pval = (ploc) ? *ploc : NULL;
#endif
#ifdef DEBUG1
	S file = file;
	S func = func;
	S line = line;
	S target = target;
#endif
	//state_head->next = s;
	//state_head = s;

	if (async) {
		if (loc) {
			*loc = val;
		} else {
			assert(NULL != ploc);
			*ploc = pval;
		}
	}
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
//	if (!in_block)
//		pthread_mutex_lock(&async_mutex);
	_state_write_dbg(g, true, loc, val, NULL, NULL, file, func, line, target);
//	if (!in_block)
//		pthread_mutex_unlock(&async_mutex);
}

EXPORT void state_write_p_async_dbg(enum stage g, bool in_block,
		uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target) {
//	if (!in_block)
//		pthread_mutex_lock(&async_mutex);
	_state_write_dbg(g, true, NULL, 0, ploc, pval, file, func, line, target);
//	if (!in_block)
//		pthread_mutex_unlock(&async_mutex);
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
//	if (!in_block)
//		pthread_mutex_lock(&async_mutex);
	_state_write(g, true, loc, val, NULL, NULL);
//	if (!in_block)
//		pthread_mutex_unlock(&async_mutex);
}

EXPORT void state_write_p_async(enum stage g, bool in_block,
		uint32_t **ploc, uint32_t *pval) {
//	if (!in_block)
//		pthread_mutex_lock(&async_mutex);
	_state_write(g, true, NULL, 0, ploc, pval);
//	if (!in_block)
//		pthread_mutex_unlock(&async_mutex);
}
#endif

EXPORT void state_write_op(enum stage g __attribute__ ((unused)), struct op **loc, struct op *val) {
	// Lazy hack since every other bit of preserved state is a uint32_t[*]
	// At some point in time state saving will likely have to be generalized,
	// until then, however, this will suffice
	assert(loc == &id_ex_o);
#ifdef NO_PIPELINE
	id_ex_o = val;
#else
#ifdef DEBUG1
	static bool why_is_there_no_pthread_null = false;
	static pthread_t sanity;
	if (why_is_there_no_pthread_null == false) {
		why_is_there_no_pthread_null = true;
		sanity = pthread_self();
	} else {
		assert(sanity == pthread_self());
	}
#endif // DEBUG1
	state_next_id_ex_o = val;
#endif // NO_PIPELINE
}

EXPORT void stall(enum stage g) {
	CORE_ERR_not_implemented("stall\n");
}

#ifndef NO_PIPELINE
EXPORT void state_pipeline_flush(uint32_t new_pc) {
	DBG2("pipeline flush. new_pc: %08x\n", new_pc);
	bool flag;
#ifdef HAVE_STDATOMIC
	flag = atomic_flag_test_and_set(&pipeline_flush_flag);
#elif defined(CLANG_ATOMIC)
	flag = __sync_lock_test_and_set(&pipeline_flush_bool, 1);
#endif
	assert((flag == false) && "duplicate pipeline flushes?");
	state_pipeline_new_pc = new_pc;
}
#endif

#ifdef HAVE_REPLAY
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
		id_ex_o = find_op(id_ex_inst);

		return 0;
	}
}
#endif //HAVE_REPLAY
