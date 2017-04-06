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

#include "core/operations/strm.h"

// arm-thumb
static void stm_t1(uint16_t inst) {
	uint8_t register_list = inst & 0xff;
	uint8_t rn = (inst >> 8) & 0x7;

	uint16_t registers = register_list;
	bool wback = true;

	if (hamming(registers) < 1)
		CORE_ERR_unpredictable("stm_t1 case\n");

	OP_DECOMPILE("STM<c> <Rn>!,<registers>", rn, registers);
	return stm(rn, registers, wback);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_strm(void) {
	// stm_t1: 1100 0<x's>
	register_opcode_mask_16(0xc000, 0x3800, stm_t1);
}
