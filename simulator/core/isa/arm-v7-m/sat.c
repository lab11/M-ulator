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

#include "core/operations/sat.h"

// arm-v7-m
static void ssat_t1(uint32_t inst) {
	uint8_t sat_imm = inst & 0x1f;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	uint8_t sh = (inst >> 21) & 0x1;

	uint8_t saturate_to = sat_imm+1;
	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(sh<<1, imm2|(imm3<<2), &shift_t, &shift_n);

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("ssat_t1 case\n");

	OP_DECOMPILE("SSAT<c> <Rd>,#<imm5>,<Rn>{,<shift>}",
			rd, saturate_to, rn, shift_t, shift_n);
	return ssat(rd, rn, saturate_to, shift_t, shift_n);
}

// arm-v7-m
static void usat_t1(uint32_t inst) {
	uint8_t sat_imm = inst & 0x1f;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	uint8_t sh = (inst >> 21) & 0x1;

	uint8_t saturate_to = sat_imm;
	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(sh<<1, imm2|(imm3<<2), &shift_t, &shift_n);

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("ssat_t1 case\n");

	OP_DECOMPILE("USAT<c> <Rd>,#<imm5>,<Rn>{,<shift>}",
			rd, saturate_to, rn, shift_t, shift_n);
	return usat(rd, rn, saturate_to, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_sat(void) {
	// ssat_t1: 1111 0011 00x0 xxxx 0xxx xxxx xx0x xxxx
	register_opcode_mask_32(0xf3000000, 0x0cd08020, ssat_t1);

	// usat_t1: 1111 0011 10x0 xxxx 0xxx xxxx xx0x xxxx
	register_opcode_mask_32_ex(0xf3800000, 0x0c508020, usat_t1,
			0x200000, 0x70c0,
			0, 0);
}
