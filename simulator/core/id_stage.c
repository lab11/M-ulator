/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#define STAGE ID

#include "id_stage.h"
#include "pipeline.h"
#include "state_sync.h"

#include "simulator.h"
#include "opcodes.h"

#include "cpu/core.h"

static void tick_id(void) {
	DBG2("start\n");

	// Instruction Decode
	struct op* o;
	uint32_t inst = if_id_inst;

	o = find_op(if_id_inst);
	if (NULL == o) {
		WARN("No handler registered for inst %x\n", inst);
		CORE_ERR_illegal_instr(inst);
	}

	SW(&id_ex_PC, if_id_PC);
	SW(&id_ex_inst, inst);
	state_write_op(&id_ex_o, o);

	DBG2("end\n");
}

#ifdef NO_PIPELINE
static int id_pipeline_flush(void* new_pc_void) {
	uint32_t new_pc = *((uint32_t *) new_pc_void);
	SW(&id_ex_PC, new_pc);
#else
static int id_pipeline_flush(void* new_pc_void __attribute__ ((unused))) {
	SW(&id_ex_PC, STALL_PC);
	SW(&id_ex_inst, INST_NOP);
	state_write_op(&id_ex_o, find_op(INST_NOP));
#endif
	return 0;
}

__attribute__ ((constructor))
void register_id_stage(void) {
	register_pipeline_stage(1, "ID Stage", tick_id, id_pipeline_flush);
}
