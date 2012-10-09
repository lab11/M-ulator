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

#define STAGE ID

#include "id_stage.h"
#include "pipeline.h"
#include "state_sync.h"

#include "simulator.h"

EXPORT bool match_mask(uint32_t inst, uint32_t ones_mask, uint32_t zeros_mask) {
	return
		(ones_mask  == (ones_mask  & inst)) &&
		(zeros_mask == (zeros_mask & ~inst));
}

static struct op* _find_op(struct op* o, uint32_t inst) {
	while (NULL != o) {
		if (match_mask(inst, o->ones_mask, o->zeros_mask)) {
			// The mask matched, now verify there isn't an exception
			int i;
			bool exception = false;
			for (i = 0; i < o->ex_cnt; i++) {
				uint32_t ones_mask  = o->ex_ones[i];
				uint32_t zeros_mask = o->ex_zeros[i];
				if (match_mask(inst, ones_mask, zeros_mask)) {
					DBG2("Collision averted via exception\n");
					exception = true;
				}
			}

			if (!exception)
				break;
		}
		o = o->next;
	}

	return o;
}

EXPORT struct op* find_op(uint32_t inst, bool reorder) {
	struct op *o = ops;
	o = _find_op(o, inst);

	if (NULL != o) {
		// Check for ambiguity, and let's report all of them now
		struct op* t = _find_op(o->next, inst);
		bool ambiguous = false;

		while (NULL != t) {
			WARN("AMB\t%s:\tones %08x zeros %08x\n", t->name,
					t->ones_mask, t->zeros_mask);
			ambiguous = true;
			t = _find_op(t->next, inst);
		}

		if (ambiguous) {
			WARN("AMB\t%s:\tones %08x zeros %08x\n", o->name,
					o->ones_mask, o->zeros_mask);
			WARN("AMB\tAmbiguous instruction detected!\n");
			WARN("Instruction %08x at PC %08x matched multiple masks\n",
					inst,
					(inst & 0xffff0000) ? if_id_PC-4 : if_id_PC-2);
			CORE_ERR_illegal_instr(inst);
		}
	}

	if (o && reorder) {
		// ...
	}

	return o;
}

EXPORT void tick_id(void) {
	DBG2("start\n");

	// Instruction Decode
	struct op* o;
	uint32_t inst = if_id_inst;

	o = find_op(if_id_inst, true);
	if (NULL == o) {
		WARN("No handler registered for inst %x\n", inst);
		CORE_ERR_illegal_instr(inst);
	}

	SW(&id_ex_PC, if_id_PC);
	state_write_op(STAGE, &id_ex_o, o);
	SW(&id_ex_inst, inst);

	DBG2("end\n");
}
