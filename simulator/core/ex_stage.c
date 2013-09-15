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

#define STAGE EX

#include "ex_stage.h"
#include "pipeline.h"
#include "state_sync.h"

#include "simulator.h"
#include "opcodes.h"

#include "cpu/core.h"
#include "cpu/registers.h"
#include "cpu/misc.h"

static void execute(struct op *o, uint32_t inst) {
#ifdef HAVE_DECOMPILE
	extern bool decompile_ran;
	if (decompile_flag)
		decompile_ran = false;
#endif
	if (o->is16)
		o->op16.fn(inst);
	else
		o->op32.fn(inst);
#ifdef HAVE_DECOMPILE
	if (decompile_flag && !decompile_ran)
		WARN("Missing decompile for %s (%08x)\n", o->name, inst);
#endif
}

// "Private" export from state.c (hack)
EXPORT struct op* state_read_op(struct op **loc);
// Private export from state.c
       bool state_ex_stage_take_async_exception(uint32_t next_pc);
static void tick_ex(void) {
	DBG2("start\n");

	if (state_ex_stage_take_async_exception(SR(&id_ex_PC) - 4))
		return;

	struct op* o = state_read_op(&id_ex_o);
	uint32_t inst = SR(&id_ex_inst);

	assert(NULL != o);
	assert(NULL != o->name);

	// Execute
	if (in_ITblock()) {
		if (eval_cond(CORE_apsr_read(), (read_itstate() & 0xf0) >> 4)) {
			if (printcycles) {
				printf("    P: %08d - 0x%08x : %04x (%s)\t%s\n",
						cycle, id_ex_PC - 4, inst, o->name,
						"ITSTATE {executed}");
			}
			execute(o, inst);
		} else {
			if (printcycles) {
				printf("    P: %08d - 0x%08x : %04x (%s)\t%s\n",
						cycle, id_ex_PC - 4, inst, o->name,
						"ITSTATE {skipped}");
			}
#ifdef HAVE_DECOMPILE
			extern int decompile_flag;
			if (decompile_flag) {
				printf("DECOM: (IT Skipped)\n");
			}
#endif
			DBG2("itstate skipped instruction\n");
		}
		IT_advance();
	} else {
		if (printcycles) {
			if ((id_ex_PC == STALL_PC) && (inst == INST_NOP)) {
				printf("    P: %08d - 0x%08x : <stall>\n",
						cycle, id_ex_PC - 4);
			} else {
				printf("    P: %08d - 0x%08x : %04x (%s)\n",
						cycle, id_ex_PC - 4,
						inst, o->name);
			}
		}
		execute(o, inst);
	}

	DBG2("end\n");
}

static int ex_pipeline_flush(void* new_pc_void __attribute__ ((unused))) {
	return 0;
}

__attribute__ ((constructor))
void register_ex_stage(void) {
	register_pipeline_stage(2, "EX Stage", tick_ex, ex_pipeline_flush);
}
