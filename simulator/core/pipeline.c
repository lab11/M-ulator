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

#define STAGE PIPE

#include "pipeline.h"
#include "state_sync.h"
#include "opcodes.h"

/* This file defines all of the gloabl state shared across the various
 * stages in the ARM pipeline; this simulator assumes the standard ARMv7
 * three stage pipeline (IF|ID|EX)
 */

// PREFETCH <--> IF
uint32_t pre_if_PC;

// IF <--> ID
uint32_t if_id_PC;
uint32_t if_id_inst;

// ID <--> EX
uint32_t id_ex_PC;
struct op* id_ex_o;
uint32_t id_ex_inst;

#define MAX_PIPELINE_STAGES 8

static struct pipeline_stage {
	struct pipeline_stage *next;

	char *name;
	void (*tick_fn) (void);
	int (*pipeline_flush_fn) (void*);

	void (*run_fn_void) (void);
	int  (*run_fn_args) (void *);
	void* args;
	int ret;

	pthread_t pthread;
	sem_t *start;
	sem_t *done;
} stages[MAX_PIPELINE_STAGES];
static int num_stages;

EXPORT void register_pipeline_stage(int idx, const char* name, void (*fn) (void),
		int (*pipeline_flush_fn) (void*)) {
	if (idx > MAX_PIPELINE_STAGES) {
		WARN("Max pipeline stage count exceeded (req idx: %d)\n", idx);
		ERR(E_UNKNOWN, "Need to increase pipeline.c::MAX_PIPELINE_STAGES\n");
	}

	if (stages[idx].name != NULL) {
		WARN("Duplicate registrations of pipeline index: %d\n", idx);
		WARN("  Previous was: %s\n", stages[idx].name);
		WARN("    Attempting: %s\n", name);
		ERR(E_UNKNOWN, "Dynamic pipeline configuration error\n");
	}

	stages[idx].name = strdup(name);
	assert(stages[idx].name && "malloc strdup");
	stages[idx].tick_fn = fn;
	stages[idx].pipeline_flush_fn = pipeline_flush_fn;

	num_stages = MAX(num_stages, idx+1);
}

#ifndef NO_PIPELINE
static void* ticker(void *idx_void) {
	int idx = *((int *) idx_void);

#ifdef __APPLE__
	if (0 != pthread_setname_np(stages[idx].name))
#else
	if (0 != prctl(PR_SET_NAME, stages[idx].name, 0, 0, 0))
#endif // __APPLE__
		ERR(E_UNKNOWN, "Unexpected error setting thread name: %s", strerror(errno));

	sem_post(stages[idx].done);

	while (1) {
		sem_wait(stages[idx].start);
		if (stages[idx].run_fn_void)
			stages[idx].run_fn_void();
		else if (stages[idx].run_fn_args)
			stages[idx].ret = stages[idx].run_fn_args(stages[idx].args);
		sem_post(stages[idx].done);
	}
}
#endif

EXPORT void pipeline_init(void) {
#ifdef NO_PIPELINE
#else
	for (int idx = 0; idx < num_stages ; idx++) {
		// OS X requires named semaphores
		char name_buf[32];

		snprintf(name_buf, 32, "/%d-%s.start", getpid(), stages[idx].name);
		stages[idx].start = sem_open(name_buf, O_CREAT|O_EXCL, 0600, 0);
		if (stages[idx].start == SEM_FAILED)
			ERR(E_UNKNOWN, "%s\n", strerror(errno));

		snprintf(name_buf, 32, "/%d-%s.done", getpid(), stages[idx].name);
		stages[idx].done = sem_open(name_buf, O_CREAT|O_EXCL, 0600, 0);
		if (stages[idx].done == SEM_FAILED)
			ERR(E_UNKNOWN, "%s\n", strerror(errno));

		pthread_create(&stages[idx].pthread, NULL, ticker, &idx);
		sem_wait(stages[idx].done);
	}
#endif
}

EXPORT void pipeline_flush_exception_handler(uint32_t new_pc) {
	DBG2("Pipeline Flush. new_PC: %08x\n", new_pc);
#ifdef NO_PIPELINE
	for (int i=0; i < num_stages; i++) {
		stages[i].pipeline_flush_fn(&new_pc);
	}
#else
	for (int i=0; i < num_stages; i++) {
		stages[i].args = &new_pc;
		stages[i].run_fn_args = stages[i].pipeline_flush_fn;
		sem_post(stages[i].start);
	}
	for (int i=0; i < num_stages; i++) {
		sem_wait(stages[i].done);
		stages[i].run_fn_args = NULL;
	}
#endif
	DBG2("flush done\n");
}

EXPORT void pipeline_stages_tick(void) {
#ifdef NO_PIPELINE
	for (int i=0; i < num_stages; i++) {
		state_start_tick();
		stages[i].tick_fn();
		state_tock();
	}
#else
	pipeline_thread_run_fn_void(state_start_tick);
	for (int i=0; i < num_stages; i++) {
		stages[i].run_fn_void = stages[i].tick_fn;
		sem_post(stages[i].start);
	}
	for (int i=0; i < num_stages; i++) {
		sem_wait(stages[i].done);
		stages[i].run_fn_void = NULL;
	}
#endif
}

EXPORT void pipeline_stages_tock() {
#ifdef NO_PIPELINE
	return;
#else
	pipeline_thread_run_fn_void(state_tock);
#endif
}

#ifdef HAVE_REPLAY
#ifndef NO_PIPELINE
static int state_seek_for_calling_thread_wrapper(void *target_void) {
	return state_seek_for_calling_thread(*((int *) target_void));
}
#endif

EXPORT int pipeline_state_seek(int target) {
#ifdef NO_PIPELINE
	return state_seek_for_calling_thread(target);
#else
	for (int i=0; i < num_stages; i++) {
		stages[i].args = &target;
		stages[i].run_fn_args = state_seek_for_calling_thread_wrapper;
		sem_post(stages[i].start);
	}

	int ret = INT_MIN;
	for (int i=0; i < num_stages; i++) {
		sem_wait(stages[i].done);
		stages[i].run_fn_args = NULL;
		if (ret == INT_MIN)
			ret = stages[i].ret;
		else if (ret != stages[i].ret) {
			WARN("Pipeline stages in inconsistent state after seeking\n");
			ERR(E_UNKNOWN, "Stage %d cycle %d. Stage %d cycle %d\n",
					i - 1, ret, i, stages[i].ret);
		}
	}

	return ret;
#endif
}
#endif // HAVE_REPLAY

#ifndef NO_PIPELINE
EXPORT void pipeline_thread_run_fn_void(void (*fn) (void)) {
	for (int i=0; i < num_stages; i++) {
		stages[i].run_fn_void = fn;
		sem_post(stages[i].start);
	}
	for (int i=0; i < num_stages; i++) {
		sem_wait(stages[i].done);
		stages[i].run_fn_void = NULL;
	}
}

EXPORT int pipeline_thread_run_fn_args(int (*fn) (void *), void* args) {
	int ret = 0;

	for (int i=0; i < num_stages; i++) {
		if (stages[i].name == NULL)
			break;

		stages[i].args = args;
		stages[i].run_fn_args = fn;
		sem_post(stages[i].start);
	}
	for (int i=0; i < num_stages; i++) {
		sem_wait(stages[i].done);
		stages[i].run_fn_args = NULL;
		ret |= stages[i].ret;
	}

	return ret;
}

EXPORT void pipeline_exception(uint16_t inst __attribute__ ((unused))) {
	CORE_ERR_not_implemented("Pipeline excpetion!\n");
}
#endif
