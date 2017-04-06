/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2016  Pat Pannuto <pat.pannuto@gmail.com>
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

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/pop.h"

// arm-thumb
static void pop_t1(uint16_t inst) {
	bool P = inst & 0x100;
	uint16_t registers = (inst & 0xff) | (P << 15);

	if (hamming(registers) == 0) {
		CORE_ERR_unpredictable("pop_t1 with no registers");
	}

	OP_DECOMPILE("POP<c> <registers>", registers);
	return pop(registers);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_pop(void) {
	// pop_t1: 1011 110<x's>
	register_opcode_mask_16(0xbc00, 0x4200, pop_t1);
}
