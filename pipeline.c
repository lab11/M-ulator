#define STAGE PIPE

#include "pipeline.h"
#include "simulator.h"
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

	DBG2("spawned ticker thread executing: %p\n", fn);

	while (1) {
		sem_post(&ticker_ready_sem);
		sem_wait(&start_tick_sem);
		fn();
		sem_post(&end_tick_sem);
		sem_wait(&end_tock_sem);
	}
}

void pipeline_exception(uint32_t inst __attribute__ ((unused))) {
	CORE_ERR_not_implemented("Pipeline excpetion!\n");
}
#endif
