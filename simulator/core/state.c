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

#ifndef NO_PIPELINE
static uint32_t state_pipeline_new_pc = -1;
static thread_local struct op* state_next_id_ex_o = NULL;
#endif

struct state_change {
	uint32_t *loc;
	uint32_t val;
	uint32_t **ploc;
	uint32_t *pval;
#ifdef HAVE_REPLAY
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


////
#define STATE_MAX_WRITES 32
static thread_local int state_tls_count = 0;

#ifdef HAVE_REPLAY
struct state_change_list {
	int cycle;
	struct state_change_list *next;
	struct state_change_list *prev;

	int write_count;
	struct state_change writes[STATE_MAX_WRITES];
};

static thread_local struct state_change_list write_root = {
	.cycle = 0,
	.next = NULL,
	.prev = NULL,
	.write_count = 0,
};
static thread_local struct state_change_list* write_cur;
#else
static thread_local struct state_change writes[STATE_MAX_WRITES];
#endif // HAVE_REPLAY
////

#ifdef HAVE_STDATOMIC
#ifndef NO_PIPELINE
static atomic_flag pipeline_flush_flag = ATOMIC_FLAG_INIT(false);
#endif
static atomic_bool debugging_bool = ATOMIC_BOOL_INIT(false);
#elif defined(CLANG_ATOMIC)
#ifndef NO_PIPELINE
static int pipeline_flush_bool = false;
#endif

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

EXPORT void state_start_tick(void) {
	state_tls_count = 0;

#ifdef HAVE_REPLAY
	if (unlikely(NULL == write_cur))
		write_cur = &write_root;

	if (write_cur->cycle != (cycle - 1)) {
		WARN("Local cycle does not match global cycle?\n");
		ERR(E_UNKNOWN, "write_cur->cycle: %d, cycle - 1: %d\n",
				write_cur->cycle, cycle - 1);
	}

	if (NULL != write_cur->next) {
		DBG1("Simulator re-excuting at cycle %d\n", cycle);
		DBG1("Discarding all future state\n");

		struct state_change_list* l = write_cur->next;
		while (l->next != NULL) {
			l = l->next;
			free(l->prev);
		}
		free(l);
		write_cur->next = NULL;
	}

	write_cur->next = malloc(sizeof(struct state_change_list));
	assert(write_cur->next && "Allocating state history memory");
	write_cur->next->prev = write_cur;
	write_cur = write_cur->next;
	write_cur->cycle = cycle;
	write_cur->next = NULL;
	write_cur->write_count = 0;
#endif
}

#ifdef CYCLE_ACCURATE
/* "private" export to cpu/exception.c */
void state_take_exception(void (*fn) (void)) {
}
#else
void state_take_exception(void) {
}
#endif

EXPORT void state_write_op(struct op **loc, struct op *val);
EXPORT bool state_handle_exceptions(void) {
#ifndef NO_PIPELINE
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
		pipeline_flush_exception_handler(state_pipeline_new_pc);
		state_write_op(&id_ex_o, find_op(INST_NOP));
		return true;
	}
#endif

	return false;
}

EXPORT void state_tock(void) {
#ifdef HAVE_REPLAY
#define W write_cur->writes
	write_cur->write_count = state_tls_count;
#else
#define W writes
#endif
	for (int i = 0; i < state_tls_count; i++) {
		if (W[i].loc != NULL)
			*(W[i].loc) = W[i].val;
		else
			*(W[i].ploc) = W[i].pval;
	}
#ifndef NO_PIPELINE
	if (state_next_id_ex_o != NULL) {
		id_ex_o = state_next_id_ex_o;
		state_next_id_ex_o = NULL;
	}
#endif // NO_PIPELINE
#undef W
}

EXPORT uint32_t state_read(uint32_t *loc) {
	return *loc;
}

EXPORT uint32_t state_read_async(uint32_t *loc) {
	uint32_t ret;
	ret = state_read(loc);
	return ret;
}

EXPORT uint32_t* state_read_p(uint32_t **ploc) {
	return *ploc;
}

EXPORT uint32_t* state_read_p_async(uint32_t **ploc) {
	uint32_t *ret;
	ret = *ploc;
	return ret;
}

#ifdef DEBUG1
static void _state_write_dbg(uint32_t *loc, uint32_t val,
		uint32_t** ploc, uint32_t* pval,
		const char *file, const char* func,
		const int line, const char *target) {
#else
static void _state_write(uint32_t *loc, uint32_t val,
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
	int s_c = state_tls_count++;
	if (unlikely(s_c == STATE_MAX_WRITES)) {
		WARN("Maximum write location count exceeded\n");
		ERR(E_UNKNOWN, "Need to increment state.c::STATE_MAX_WRITES\n");
	}
#ifdef HAVE_REPLAY
#define S write_cur->writes[s_c].
#else
#define S writes[s_c].
#endif

	DBG2("cycle: %08d\t(%s): loc %p val %08x\n",
			cycle, target, loc, val);

	S loc = loc;
	S val = val;
	S ploc = ploc;
	S pval = pval;
#ifdef HAVE_REPLAY
	S prev_val = (loc) ? *loc : 0;
	S prev_pval = (ploc) ? *ploc : NULL;
#endif
#ifdef DEBUG1
	S file = file;
	S func = func;
	S line = line;
	S target = target;
#endif
#undef S
}

#ifdef DEBUG1
static void _state_write_async_dbg(uint32_t *loc, uint32_t val,
		uint32_t** ploc, uint32_t* pval,
		const char *file, const char* func,
		const int line, const char *target) {
#else
static void _state_write_async(uint32_t *loc, uint32_t val,
		uint32_t** ploc, uint32_t* pval) {
#endif

#ifdef HAVE_REPLAY
	if (unlikely(NULL == write_cur))
		write_cur = &write_root;

	// XXX: There are races / issues here if anything async happens
	//      while seeking through state

	if (write_cur->cycle < cycle)
		state_start_tick();

#ifdef DEBUG1
	_state_write_dbg(loc, val, ploc, pval, file, func, line, target);
#else
	_state_write(loc, val, ploc, pval);
#endif

#else // !HAVE_REPLAY
#ifdef DEBUG1
	// Suppress unused warnings in this compile option path
	(void) file;
	(void) func;
	(void) line;
	(void) target;
#endif
#endif // HAVE_REPLAY

	// "state tock"
	if (loc) {
		*loc = val;
	} else {
		assert(NULL != ploc);
		*ploc = pval;
	}
}

#ifdef DEBUG1
EXPORT void state_write_dbg(uint32_t *loc, uint32_t val,
		const char *file, const char* func,
		const int line, const char *target) {
	_state_write_dbg(loc, val, NULL, NULL, file, func, line, target);
}

EXPORT void state_write_p_dbg(uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target) {
	_state_write_dbg(NULL, 0, ploc, pval, file, func, line, target);
}

EXPORT void state_write_async_dbg(uint32_t *loc, uint32_t val,
		const char *file, const char* func,
		const int line, const char *target) {
	_state_write_async_dbg(loc, val, NULL, NULL, file, func, line, target);
}

EXPORT void state_write_p_async_dbg(uint32_t **ploc, uint32_t *pval,
		const char *file, const char* func,
		const int line, const char *target) {
	_state_write_async_dbg(NULL, 0, ploc, pval, file, func, line, target);
}
#else
EXPORT void state_write(uint32_t *loc, uint32_t val) {
	_state_write(loc, val, NULL, NULL);
}

EXPORT void state_write_p(uint32_t **ploc, uint32_t *pval) {
	_state_write(NULL, 0, ploc, pval);
}

EXPORT void state_write_async(uint32_t *loc, uint32_t val) {
	_state_write_async(loc, val, NULL, NULL);
}

EXPORT void state_write_p_async(uint32_t **ploc, uint32_t *pval) {
	_state_write_async(NULL, 0, ploc, pval);
}
#endif

// Lazy hack since every other bit of preserved state is a uint32_t[*]
// At some point in time state saving will likely have to be generalized,
// until then, however, this will suffice
EXPORT struct op* state_read_op(struct op **loc) {
	assert(loc == &id_ex_o);
	return id_ex_o;
}

EXPORT void state_write_op(struct op **loc, struct op *val) {
	assert(loc == &id_ex_o);
#ifdef NO_PIPELINE
	id_ex_o = val;
#else
	if (state_is_debugging())
		id_ex_o = val;
	else
		state_next_id_ex_o = val;
#endif // NO_PIPELINE
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
#endif // NO_PIPELINE

#ifdef HAVE_REPLAY
// Returns 0 on success
// Returns >0 on tolerable error (e.g. seek past end)
// Returns <0 on catastrophic error
EXPORT int state_seek_for_calling_thread(int target_cycle) {
	int current_cycle = cycle;

	if (write_cur == NULL) {
		DBG1("Seek request before any cycle executed. Ignored\n");
		return current_cycle;
	}

	if (target_cycle > current_cycle) {
		DBG1("seeking forward from %d to %d\n", current_cycle, target_cycle);
		while (target_cycle > current_cycle) {
			if (write_cur->next == NULL) {
				break;
			}

			for (int i=0; i < write_cur->write_count; i++) {
				if (write_cur->writes[i].loc) {
					*(write_cur->writes[i].loc) = write_cur->writes[i].val;
				} else {
					*(write_cur->writes[i].ploc) = write_cur->writes[i].pval;
				}
			}

			write_cur = write_cur->next;
			current_cycle = write_cur->cycle;
		}
	} else if (target_cycle == current_cycle) {
		DBG1("Request to seek to current cycle ignored\n");
	} else {
		DBG1("seeking backward from %d to %d\n", current_cycle, target_cycle);
		while (write_cur->cycle > target_cycle) {
			if (write_cur->prev == NULL) {
				assert(current_cycle == 0);
				break;
			}

			for (int i=0; i < write_cur->write_count; i++) {
				if (write_cur->writes[i].loc) {
					*(write_cur->writes[i].loc) = write_cur->writes[i].prev_val;
				} else {
					*(write_cur->writes[i].ploc) = write_cur->writes[i].prev_pval;
				}
			}
			write_cur = write_cur->prev;
			current_cycle = write_cur->cycle;
		}
	}

	// One last bit of fixup since we don't actually track
	// the op pointer
	// XXX: threads
	id_ex_o = find_op(id_ex_inst);

	return current_cycle;
}
#else
EXPORT int state_seek(int target_cycle __attribute__ ((unused))) {
	return -1;
}
#endif //HAVE_REPLAY
