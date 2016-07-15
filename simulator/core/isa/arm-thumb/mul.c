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

#include "core/operations/mul.h"

// arm-thumb
static void mul_t1(uint16_t inst) {
	uint8_t rdm = inst & 0x7;
	uint8_t rn = inst & 0x7;

	bool setflags = !in_ITblock();

	OP_DECOMPILE("MUL<IT> <Rdm>,<Rn>,<Rdm>", rdm, rn, rdm);
	return mul(setflags, rdm, rn, rdm);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_mul(void) {
	// mul_t1: 0100 0011 01xx xxxx
	register_opcode_mask_16(0x4340, 0xbc80, mul_t1);
}
