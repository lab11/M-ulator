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

// arm-v7-m
static void str_imm_t3(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if (rt == 15)
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("STR<c>.W <Rt>,[<Rn>,#<imm12>]", rt, rn, imm32);
	return str_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void str_imm_t4(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = !!(inst & 0x100);
	bool U = !!(inst & 0x200);
	bool P = !!(inst & 0x400);
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P;
	bool add = U;
	bool wback = W;

	if ((rt == 15) || (wback && (rn == rt)))
		CORE_ERR_unpredictable("Bad regs\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("STR<c> <Rt>,[<Rn>{, #+/-<imm>}]", rt, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("STR<c> <Rt>,[<Rn>, #+/-<imm>]!", rt, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("STR<c> <Rt>,[<Rn>], #+/-<imm>", rt, rn, add, imm32);
	else // !index && !wback -- illegal
		OP_DECOMPILE("!!STR !index && !wback illegal combination?");
	return str_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void str_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	enum SRType shift_t = LSL;
	uint8_t shift_n = imm2;

	if ((rt == 15) || BadReg(rm))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("STR<c>.W <Rt>,[<Rn>,<Rm>{,LSL #<imm2>}]",
			rt, rn, rm, shift_n);
	return str_reg(rt, rn, rm, shift_t, shift_n);
}

// arm-v7-m
static void strb_imm_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	if (rn == 0xf)
		CORE_ERR_unpredictable("strb_imm_t2 rn == 0xf undef\n");

	bool index = true;
	bool add = true;
	bool wback = false;

	uint32_t imm32 = imm12;

	if (rt >= 13)
		CORE_ERR_unpredictable("strb_imm_t2 rt in {13,15}\n");

	OP_DECOMPILE("STRB<c>.W <Rt>,[<Rn>,#<imm12>]", rt, rn, imm32);
	return strb_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void strb_imm_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = !!(inst & 0x100);
	bool U = !!(inst & 0x200);
	bool P = !!(inst & 0x400);
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P;
	bool add = U;
	bool wback = W;

	if (BadReg(rt) || (wback && (rn == rt)))
		CORE_ERR_unpredictable("bad reg\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("STRB<c> <Rt>,[<Rn>{, #+/-<imm>}]", rt, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("STRB<c> <Rt>,[<Rn>, #+/-<imm>]!", rt, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("STRB<c> <Rt>,[<Rn>], #+/-<imm>", rt, rn, add, imm32);
	else // !index && !wback -- illegal
		OP_DECOMPILE("!!STRB !index && !wback illegal combination?");
	return strb_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void strb_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt   = (inst >> 12) & 0xf;
	uint8_t rn   = (inst >> 16) & 0xf;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = imm2;

	if (BadReg(rt) || BadReg(rm))
		CORE_ERR_unpredictable("strb_reg_t2 case\n");

	OP_DECOMPILE("STRB<c>.W <Rt>,[<Rn>,<Rm>{,LSL #<imm2>}]",
			rt, rn, rm, shift_n);
	return strb_reg(rt, rn, rm, shift_t, shift_n);
}

// arm-v7-m
static void strd_imm_t1(uint32_t inst) {
	uint8_t imm8 = (inst & 0xff);
	uint8_t rt2 = (inst & 0xf00) >> 8;
	uint8_t rt = (inst & 0xf000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t W = !!(inst & 0x200000);
	uint8_t U = !!(inst & 0x800000);
	uint8_t P = !!(inst & 0x1000000);

	if ((P == 0) && (W == 0))
		CORE_ERR_unpredictable("strd_imm_t1 -> EX\n");

	uint32_t imm32 = imm8 << 2;
	bool index = P;
	bool add = U;
	bool wback = W;

	if (index && !wback) // Offset
		OP_DECOMPILE("STRD<c> <Rt>,[<Rn>{, #+/-<imm>}]", rt, rn, add, imm8);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("STRD<c> <Rt>,[<Rn>, #+/-<imm>]!", rt, rn, add, imm8);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("STRD<c> <Rt>,[<Rn>], #+/-<imm>", rt, rn, add, imm8);
	else // !index && !wback -- illegal
		OP_DECOMPILE("!!STRD !index && !wback illegal combination?");

	if (wback && ((rn == rt) || (rn == rt2)))
		CORE_ERR_unpredictable("strd_imm_t1 wback + regs\n");

	if ((rn == 15) || (rt == 13) || (rt == 15) || (rt2 == 13) || (rt2 == 15))
		CORE_ERR_unpredictable("strd_imm_t1 bad regs\n");

	return strd_imm(rt, rt2, rn, imm32, index, add, wback);
}

// arm-v7-m
static void strh_imm_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if ((rt == 13) || (rt == 15))
		CORE_ERR_unpredictable("Bad dest reg\n");

	OP_DECOMPILE("STRH<c>.W <Rt>,[<Rn>{,#<imm12>}]", rt, rn, imm32);
	return strh_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void strh_imm_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool    W    = (inst >> 8) & 0x1;
	bool    U    = (inst >> 9) & 0x1;
	bool    P    = (inst >> 10) & 0x1;
	uint8_t rt   = (inst >> 12) & 0xf;
	uint8_t rn   = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P==1;
	bool add = U==1;
	bool wback = W==1;

	if (BadReg(rt) || (wback && (rn == rt)))
		CORE_ERR_unpredictable("strh_imm_t3 case\n");

	if (index && !wback) // Offset
		OP_DECOMPILE("STRH<c> <Rt>,[<Rn>{, #+/-<imm>}]", rt, rn, add, imm32);
	else if (index && wback) // Pre-indexed
		OP_DECOMPILE("STRH<c> <Rt>,[<Rn>, #+/-<imm>]!", rt, rn, add, imm32);
	else if (!index && wback) // Post-indexed
		OP_DECOMPILE("STRH<c> <Rt>,[<Rn>], #+/-<imm>", rt, rn, add, imm32);
	else // !index && !wback -- illegal
		OP_DECOMPILE("!!STRH !index && !wback illegal combination?");
	return strh_imm(rt, rn, imm32, index, add, wback);
}

// arm-v7-m
static void strh_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt   = (inst >> 12) & 0xf;
	uint8_t rn   = (inst >> 16) & 0xf;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = imm2;

	if (BadReg(rt) || BadReg(rm))
		CORE_ERR_unpredictable("strh_reg_t2 case\n");

	OP_DECOMPILE("STRH<c>.W <Rt>,[<Rn>,<Rm>{,LSL #<imm2>}]",
			rt, rn, rm, shift_n);
	return strh_reg(rt, rn, rm, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_str(void) {
	// str_imm_t3: 1111 1000 1100 xxxx xxxx xxxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8c00000, 0x07300000, str_imm_t3,
			0xf0000, 0x0,
			0, 0);

	// str_imm_t4: 1111 1000 0100 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8400800, 0x07b00000, str_imm_t4,
			0x600, 0x100,
			0xd0500, 0x20200,
			0xf0000, 0x0,
			0x0, 0x500,
			0, 0);

	// str_reg_t2: 1111 1000 0100 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8400000, 0x07b00fc0, str_reg_t2,
			0xf0000, 0x0,
			0, 0);

	// strb_imm_t2: 1111 1000 1000 <x's>
	register_opcode_mask_32(0xf8800000, 0x07700000, strb_imm_t2);

	// strb_imm_t3: 1111 1000 0000 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8000800, 0x07f00000, strb_imm_t3,
			0x600, 0x100,
			0xf0000, 0x0,
			0x0, 0x500,
			0, 0);

	// strb_reg_t2: 1111 1000 0000 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8000000, 0x07f00fc0, strb_reg_t2,
			0xf0000, 0x0,
			0, 0);

	// strd_imm_t1: 1110 100x x1x0 <x's>
	register_opcode_mask_32(0xe8400000, 0x16100000, strd_imm_t1);

	// strh_imm_t2: 1111 1000 1010 <x's>
	register_opcode_mask_32_ex(0xf8a00000, 0x07500000, strh_imm_t2,
			0x000f0000, 0x0,
			0, 0);

	// strh_imm_t3: 1111 1000 0010 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8200800, 0x07d00000, strh_imm_t3,
			0x600, 0x100,
			0xf0000, 0x0,
			0x0, 0x500,
			0, 0);

	// strh_reg_t2: 1111 1000 0010 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8200000, 0x07d00fc0, strh_reg_t2,
			0xf0000, 0x0,
			0, 0);
}
