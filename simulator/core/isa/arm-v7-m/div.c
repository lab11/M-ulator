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

#include "core/operations/div.h"

// arm-v7-m
static void sdiv_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	if (BadReg(rd) || BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("sdiv_t1 case\n");

	OP_DECOMPILE("SDIV<c> <Rd>,<Rn>,<Rm>", rd, rn, rm);
	return sdiv(rd, rn, rm);
}

// arm-v7-m
static void udiv_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	if (BadReg(rd) || BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("UDIV<c>, <Rd>,<Rn>,<Rm>", rd, rn, rm);
	return udiv(rd, rn, rm);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_div(void) {
	// sdiv_t1: 1111 1011 1001 xxxx 1111 xxxx 1111 xxxx
	register_opcode_mask_32(0xfb90f0f0, 0x04600000, sdiv_t1);

	// udiv_t1: 1111 1011 1011 xxxx 1111 xxxx 1111 xxxx
	register_opcode_mask_32(0xfbb0f0f0, 0x04400000, udiv_t1);
}

