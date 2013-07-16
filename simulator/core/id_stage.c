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
#include "opcodes.h"

EXPORT void tick_id(void) {
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
	state_write_op(STAGE, &id_ex_o, o);
	SW(&id_ex_inst, inst);

	DBG2("end\n");
}
