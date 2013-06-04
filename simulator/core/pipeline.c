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
#include "id_stage.h"

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
	state_write_op(STAGE, &id_ex_o, find_op(INST_NOP, false));
	SW(&id_ex_inst, INST_NOP);
}

void* ticker(void *stage_fn) {
	void (*fn) (void) = (void(*)(void)) stage_fn;

#ifndef __APPLE__
	assert(0 == prctl(PR_SET_NAME, "ticker", 0, 0, 0));
#endif

	DBG2("spawned ticker thread executing: %p\n", fn);

	while (1) {
		sem_post(ticker_ready_sem);
		sem_wait(start_tick_sem);
		fn();
		sem_post(end_tick_sem);
		sem_wait(end_tock_sem);
	}
}

void pipeline_exception(uint32_t inst __attribute__ ((unused))) {
	CORE_ERR_not_implemented("Pipeline excpetion!\n");
}
#endif
