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

#ifndef __APPLE__
#include <sys/prctl.h>
#endif

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

#ifndef NO_PIPELINE
#include "simulator.h"

void pipeline_flush(uint32_t new_pc) {
	DBG2("new_PC: %08x\n", new_pc);
	SW(&pre_if_PC, new_pc);
	SW(&if_id_PC, STALL_PC);
	SW(&if_id_inst, INST_NOP);
	SW(&id_ex_PC, STALL_PC);
	SW(&id_ex_inst, INST_NOP);
}

void* ticker(void *param) {
	// Make a local copy so compiler can prove values never change
	struct ticker_params tp;
	memcpy(&tp, param, sizeof(struct ticker_params));

#ifdef __APPLE__
	assert(0 == pthread_setname_np(tp.name));
#ifdef DEBUG1
	{
	char buf[16];
	assert(0 == pthread_getname_np(pthread_self(), buf, 16));
	assert(0 == strcmp(buf, tp.name));
	}
#endif // DEBUG1
#else
	assert(0 == prctl(PR_SET_NAME, tp.name, 0, 0, 0));
#endif // __APPLE__

	DBG2("spawned %s thread executing: %p\n", tp.name, tp.fn);

	while (1) {
		sem_post(ticker_ready_sem);
		sem_wait(start_tick_sem);
		state_start_tick();
		tp.fn();
		sem_post(end_tick_sem);
		sem_wait(end_tock_sem);
		if (tp.always_tick || stages_should_tock)
			state_tock();
	}
}

void pipeline_exception(uint16_t inst __attribute__ ((unused))) {
	CORE_ERR_not_implemented("Pipeline excpetion!\n");
}
#endif
