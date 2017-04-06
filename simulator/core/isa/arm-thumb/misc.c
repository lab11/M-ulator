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

#include "core/operations/misc.h"

// arm-v6-m, arm-v7-m
static void rev_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REV<c> <Rd>,<Rm>", rd, rm);
	return rev(rd, rm);
}

// arm-v6-m, arm-v7-m
static void rev16_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REV16<c> <Rd>,<Rm>", rd, rm);
	return rev16(rd, rm);
}

// arm-v6-m, arm-v7-m
static void revsh_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REVSH<c> <Rd>,<Rm>", rd, rm);
	return revsh(rd, rm);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_misc(void) {
	// rev_t1: 1011 1010 00xx xxxx
	register_opcode_mask_16(0xba00, 0x45c0, rev_t1);

	// rev16_t1: 1011 1010 01xx xxxx
	register_opcode_mask_16(0xba40, 0x4580, rev16_t1);

	// revsh_t1: 1011 1010 11xx xxxx
	register_opcode_mask_16(0xbac0, 0x4500, revsh_t1);
}
