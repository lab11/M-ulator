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

#include "cpu/registers.h"
#include "cpu/misc.h"

static void execute(struct op *o, uint32_t inst) {
	if (o->is16)
		o->op16.fn(inst);
	else
		o->op32.fn(inst);
}

void tick_ex(void) {
	DBG2("start\n");

	assert(NULL != id_ex_o);
	assert(NULL != id_ex_o->name);

	// Execute
	if (in_ITblock()) {
		if (eval_cond(CORE_apsr_read(), (read_itstate() & 0xf0) >> 4)) {
			if (printcycles) {
				printf("    P: %08d - 0x%08x : %04x (%s)\t%s\n",
						cycle, id_ex_PC - 4, id_ex_inst, id_ex_o->name,
						"ITSTATE {executed}");
			}
			execute(id_ex_o, id_ex_inst);
		} else {
			if (printcycles) {
				printf("    P: %08d - 0x%08x : %04x (%s)\t%s\n",
						cycle, id_ex_PC - 4, id_ex_inst, id_ex_o->name,
						"ITSTATE {skipped}");
			}
			DBG2("itstate skipped instruction\n");
		}
		IT_advance();
	} else {
		if (printcycles) {
			if ((id_ex_PC == STALL_PC) && (id_ex_inst == INST_NOP)) {
				printf("    P: %08d - 0x%08x : <stall>\n",
						cycle, id_ex_PC - 4);
			} else {
				printf("    P: %08d - 0x%08x : %04x (%s)\n",
						cycle, id_ex_PC - 4,
						id_ex_inst, id_ex_o->name);
			}
		}
		execute(id_ex_o, id_ex_inst);
	}

	DBG2("end\n");
}
