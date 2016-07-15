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

#include "core/operations/ldr.h"

// arm-thumb
static void ldr_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5 << 2;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("LDR<c> <Rt>, [<Rn>{,#<imm5>}]", rt, rn, imm32);
	return ldr_imm(rt, rn, imm32, index, add, wback);
}

// arm-thumb
static void ldr_imm_t2(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rt   = (inst >> 8) & 0x7;

	uint8_t rn = 13;
	uint32_t imm32 = imm8 << 2;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("LDR<c> <Rt>, [SP{,#<imm8>}]", rt, imm32);
	return ldr_imm(rt, rn, imm32, index, add, wback);
}

// arm-thumb
static void ldr_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDR<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldr_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

// arm-thumb
static void ldr_lit_t1(uint16_t inst) {
	uint32_t imm8 = inst & 0xff;
	uint8_t rt = (inst & 0x700) >> 8;

	uint32_t imm32 = imm8 << 2;

	OP_DECOMPILE("LDR<c> <Rt>,<label>", rt, imm32);
	return ldr_lit(true, rt, imm32);
}

// arm-thumb
static void ldrb_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("LDRB<c> <Rt>,[<Rn>{,#<imm5>}]", rt, rn, imm5);
	return ldrb_imm(rt, rn, imm32, add, index, wback);
}

// arm-thumb
static void ldrb_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDRB<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldrb_reg(rt, rn, rm, shift_t, shift_n, index, add, wback);
}

// arm-thumb
static void ldrh_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5 << 1;
	bool index = true;
	bool add = true;
	bool wback = false;

	OP_DECOMPILE("LDRH <Rt>,[<Rn>{,#<imm5>}]", rt, rn, imm32);
	return ldrh_imm(rt, rn, imm32, index, add, wback);
}

// arm-thumb
static void ldrh_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDRH<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldrh_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

// arm-thumb
static void ldrsb_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDRSB<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldrsb_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

// arm-thumb
static void ldrsh_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = true;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("LDRSH<c> <Rt>,[<Rn>,<Rm>]", rt, rn, rm);
	return ldrsh_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_ldr(void) {
	// ldr_imm_t1: 0110 1<x's>
	register_opcode_mask_16(0x6800, 0x9000, ldr_imm_t1);

	// ldr_imm_t2: 1001 1<x's>
	register_opcode_mask_16(0x9800, 0x6000, ldr_imm_t2);

	// ldr_reg_t1: 0101 100<x's>
	register_opcode_mask_16(0x5800, 0xa600, ldr_reg_t1);

	// ldr_lit_t1: 0100 1<x's>
	register_opcode_mask_16(0x4800, 0xb000, ldr_lit_t1);

	// ldrb_imm_t1: 0111 1<x's>
	register_opcode_mask_16(0x7800, 0x8000, ldrb_imm_t1);

	// ldrb_reg_t1: 0101 110x xxxx xxxx
	register_opcode_mask_16(0x5c00, 0xa200, ldrb_reg_t1);

	// ldrh_imm_t1: 1000 1<x's>
	register_opcode_mask_16(0x8800, 0x7000, ldrh_imm_t1);

	// ldrh_reg_t1: 0101 101<x's>
	register_opcode_mask_16(0x5a00, 0xa400, ldrh_reg_t1);

	// ldrsb_reg_t1: 0101 011x xxxx xxxx
	register_opcode_mask_16(0x5600, 0xa800, ldrsb_reg_t1);

	// ldrsh_reg_t1: 0101 111x <x's>
	register_opcode_mask_16(0xa000, 0x5e00, ldrsh_reg_t1);
}
