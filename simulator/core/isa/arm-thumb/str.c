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

#include "core/operations/str.h"

// arm-thumb
static void str_imm_t1(uint16_t inst) {
	uint8_t imm5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rt = (inst & 0x7) >> 0;

	uint32_t imm32 = imm5 << 2;

	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("STR<c> <Rt>, [<Rn>{,#<imm5>}]", rt, rn, imm5 * 4U);
	return str_imm(rt, rn, imm32, index, add, wback);
}

// arm-thumb
static void str_imm_t2(uint16_t inst) {
	uint8_t rt = (inst & 0x700) >> 8;
	uint8_t imm8 = inst & 0xff;

	uint8_t rn = 13;
	uint32_t imm32 = imm8 << 2;

	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("STR<c> <Rt>,[SP,#<imm8>]", rt, imm8 << 2);
	return str_imm(rt, rn, imm32, index, add, wback);
}

// arm-thumb
static void str_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("STR<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return str_reg(rt, rn, rm, shift_t, shift_n);
}

// arm-thumb
static void strb_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("STRB<c> <Rt>,[<Rn>,#<imm5>]", rt, rn, imm32);
	return strb_imm(rt, rn, imm32, index, add, wback);
}

// arm-thumb
static void strb_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("STRB<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return strb_reg(rt, rn, rm, shift_t, shift_n);
}

// arm-thumb
static void strh_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5 << 1;

	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("STRH<c> <Rt>,[<Rn>{,#<imm5>}]", rt, rn, imm32);
	return strh_imm(rt, rn, imm32, index, add, wback);
}

// arm-thumb
static void strh_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("STRH<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return strh_reg(rt, rn, rm, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_str(void) {
	// str_imm_t1: 0110 0<x's>
	register_opcode_mask_16(0x6000, 0x9800, str_imm_t1);

	// str_imm_t2: 1001 0<x's>
	register_opcode_mask_16(0x9000, 0x6800, str_imm_t2);

	// str_reg_t1: 0101 000x xxxx xxxx
	register_opcode_mask_16(0x5000, 0xae00, str_reg_t1);

	// strb_imm_t1: 0111 0<x's>
	register_opcode_mask_16(0x7000, 0x8800, strb_imm_t1);

	// strb_reg_t1: 0101 010x <x's>
	register_opcode_mask_16(0x5400, 0xaa00, strb_reg_t1);

	// strh_imm_t1: 1000 0<x's>
	register_opcode_mask_16(0x8000, 0x7800, strh_imm_t1);

	// strh_reg_t1: 0101 001x <x's>
	register_opcode_mask_16(0x5200, 0xac00, strh_reg_t1);
}
