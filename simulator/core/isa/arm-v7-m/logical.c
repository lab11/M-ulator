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

#include "core/operations/logical.h"

// arm-v7-m
static void and_imm_t1(uint32_t inst) {
	union apsr_t apsr = CORE_apsr_read();

	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);
	uint8_t i = !!(inst & 0x04000000);

	if ((rd == 0xf) && (S == 1))
		CORE_ERR_not_implemented("add_imm_t1 --> TST (imm)\n");

	//d = uint(rd); n = uint(rd);
	uint8_t setflags = (S == 1);

	uint32_t imm32;
	bool carry;
	uint16_t imm12 = (i << 11) | (imm3 << 8) | imm8;
	ThumbExpandImm_C(imm12, apsr.bits.C, &imm32, &carry);

	DBG2("rd: %d, rn %d, imm12 0x%03x (%d)\n", rd, rn, imm12, imm12);

	if ( ((rd == 13) || ((rd == 15) && (S == 0))) || BadReg(rn) )
		CORE_ERR_unpredictable("Bad reg combo's in add_imm_t1\n");

	OP_DECOMPILE("AND{S}<c> <Rd>,<Rn>,#<const>", setflags, rd, rn, imm32);
	return and_imm(apsr, setflags, rd, rn, imm32, carry);
}

// arm-v7-m
static void and_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool S = (inst >> 20) & 0x1;

	bool setflags = S;

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd == 13) || ((rd == 15) && (S == 0)) || BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("AND{S}<c>.W <Rd>,<Rn>,<Rm>{,<shift>}",
			setflags, rd, rn, rm, shift_t, shift_n);
	return and_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

// arm-v7-m
static void bic_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);
	uint8_t i = !!(inst & 0x04000000);

	union apsr_t apsr = CORE_apsr_read();

	uint16_t imm12 = (i << 11) | (imm3 << 8) | imm8;
	uint32_t imm32;
	bool carry_out;
	ThumbExpandImm_C(imm12, apsr.bits.C, &imm32, &carry_out);

	if ((rd >= 13) || (rn >= 13))
		CORE_ERR_unpredictable("bic_imm_t1 bad reg\n");

	OP_DECOMPILE("BIC{S}<c> <Rd>,<Rn>,#<const>",
			S, rn, rn, imm32);
	return bic_imm(apsr, S, rd, rn, imm32, carry_out);
}

// arm-v7-m
static void bic_reg_t2(uint32_t inst) {
	uint8_t rm   = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    S    = (inst >> 20) & 0x1;

	bool setflags = S == 1;
	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if ((rd > 13) || (rn > 13) || (rm > 13))
		CORE_ERR_unpredictable("bic_reg_t2 case\n");

	OP_DECOMPILE("BIC{S}<c>.W <Rd>,<Rn>,<Rm>{,<shift>}",
			setflags, rd, rn, rm, shift_t, shift_n);
	return bic_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

// arm-v7-m
static void eor_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool S = !!(inst & 0x100000);
	bool i = !!(inst & 0x04000000);

	bool setflags = S;

	union apsr_t apsr = CORE_apsr_read();
	uint32_t imm32;
	bool carry;
	ThumbExpandImm_C((i << 11) | (imm3 << 8) | imm8, apsr.bits.C, &imm32, &carry);

	if ((rd == 13) || ((rd == 15) && (S == 0)) || BadReg(rn))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("EOR{S}<c> <Rd>,<Rn>,#<const>", rd, rn, imm32);
	return eor_imm(rd, rn, imm32, carry, setflags, apsr);
}

// arm-v7-m
static void eor_reg_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t type = (inst & 0x30) >> 4;
	uint8_t imm2 = (inst & 0xc0) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);

	if ((rd == 0xf) && (S))
		CORE_ERR_unpredictable("eor_reg_t2 -> TEQ\n");

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd == 13) || ((rd == 15) && (S == 0)) || (rn >= 13) || (rm >= 13)) {
		CORE_ERR_unpredictable("eor_reg_t2 bad reg\n");
	}

	OP_DECOMPILE("EOR{S}<c>.W <Rd>,<Rn>,<Rm>{,<shift>}",
			S, rd, rn, rm, shift_t, shift_n);
	return eor_reg(S, rd, rn, rm, shift_t, shift_n);
}

// arm-v7-m
static void mvn_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool    S    = (inst >> 20) & 0x1;
	bool    i    = (inst >> 26) & 0x1;

	bool setflags = S==1;

	union apsr_t apsr = CORE_apsr_read();
	uint32_t imm32;
	bool carry;
	ThumbExpandImm_C(imm8 | (imm3 << 8) | (i << 11), apsr.bits.C,
			&imm32, &carry);

	if (rd > 13)
		CORE_ERR_unpredictable("mvn_imm_t1 case\n");

	OP_DECOMPILE("MVN{S}<c> <Rd>,#<const>", setflags, rd, imm32);
	return mvn_imm(rd, setflags, apsr, imm32, carry);
}

// arm-v7-m
static void mvn_reg_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t type = (inst & 0x30) >> 4;
	uint8_t imm2 = (inst & 0xc0) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t S = !!(inst & 0x100000);

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd >= 13) || (rm >= 13)) {
		CORE_ERR_unpredictable("mvn_reg_t2 bad reg\n");
	}

	OP_DECOMPILE("MVN{S}<c>.W <Rd>,<Rm>{,<shift>}",
			S, rd, rm, shift_t, shift_n);
	return mvn_reg(S, rd, rm, shift_t, shift_n);
}

// arm-v7-m
static void orn_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    S    = (inst >> 20) & 0x1;
	bool    i    = (inst >> 26) & 0x1;

	bool setflags = S==1;

	union apsr_t apsr = CORE_apsr_read();
	uint32_t imm32;
	bool carry;
	ThumbExpandImm_C(imm8 | (imm3 << 8) | (i << 11), apsr.bits.C,
			&imm32, &carry);

	if ((rd > 13) || (rn == 1))
		CORE_ERR_unpredictable("orn_imm_t1 case\n");

	OP_DECOMPILE("ORN{S}<c> <Rd>,<Rn>,#<const>", rd, rn, imm32);
	return orn_imm(rd, rn, setflags, apsr, imm32, carry);
}

// arm-v7-m
static void orn_reg_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    S    = (inst >> 20) & 0x1;

	bool setflags = S==1;
	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if ((rd > 13) || (rn == 13) || (rm > 13))
		CORE_ERR_unpredictable("orn_reg_t1 case\n");

	OP_DECOMPILE("ORN{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}",
			setflags, rd, rn, rm, shift_t, shift_n);
	return orn_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

// arm-v7-m
static void orr_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    S    = (inst >> 20) & 0x1;
	bool    i    = (inst >> 26) & 0x1;

	bool setflags = S==1;

	union apsr_t apsr = CORE_apsr_read();
	uint32_t imm32;
	bool carry;
	ThumbExpandImm_C(imm8 | (imm3 << 8) | (i << 11), apsr.bits.C,
			&imm32, &carry);

	if ((rd > 13) || (rn == 13))
		CORE_ERR_unpredictable("orr_imm_t1 case\n");

	OP_DECOMPILE("ORR{S}<c> <Rd>,<Rn>,#<const>",
			setflags, rd, rn, imm32);
	return orr_imm(rd, rn, setflags, apsr, imm32, carry);
}

// arm-v7-m
static void orr_reg_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t type = (inst & 0x30) >> 4;
	uint8_t imm2 = (inst & 0xc0) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);

	if (rn == 0xf)
		CORE_ERR_unpredictable("orr_reg_t2 -> MOV\n");

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd >= 13) || (rn == 13) || (rm >= 13))
		CORE_ERR_unpredictable("orr_reg_t2 bad reg\n");

	OP_DECOMPILE("ORR{S}<c>.W <Rd>,<Rn>,<Rm>{,<shift>}",
			S, rd, rn, rm, shift_t, shift_n);
	return orr_reg(S, rd, rn, rm, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_logical(void) {
	// and_imm_t1: 1111 0x00 000x xxxx 0<x's>
	register_opcode_mask_32_ex(0xf0000000, 0x0be08000, and_imm_t1,
			0x00100f00, 0x0,
			0, 0);

	// and_reg_t2: 1110 1010 000x xxxx 0xxx xxxx xxxx xxxx
	register_opcode_mask_32_ex(0xea000000, 0x15e08000, and_reg_t2,
			0x00100f00, 0x0,
			0, 0);

	// bic_imm_t1: 1111 0x00 001x xxxx 0<x's>
	register_opcode_mask_32(0xf0200000, 0x0bc08000, bic_imm_t1);

	// bic_reg_t2: 1110 1010 001x xxxx 0<x's>
	register_opcode_mask_32(0xea200000, 0x15c08000, bic_reg_t2);

	// eor_imm_t1: 1111 0x00 100x xxxx 0<x's>
	register_opcode_mask_32_ex(0xf0800000, 0x0b608000, eor_imm_t1,
			0x100f00, 0x0,
			0, 0);

	// eor_reg_t2: 1110 1010 100x xxxx 0<x's>
	register_opcode_mask_32(0xea800000, 0x15608000, eor_reg_t2);

	// mvn_imm_t1: 1111 0x00 011x 1111 0<x's>
	register_opcode_mask_32(0xf06f0000, 0x0b808000, mvn_imm_t1);

	// mvn_reg_t2: 1110 1010 011x 1111 0<x's>
	register_opcode_mask_32(0xea6f0000, 0x15808000, mvn_reg_t2);

	// orn_imm_t1: 1111 0x00 011x xxxx 0<x's>
	register_opcode_mask_32_ex(0xf0600000, 0x0b808000, orn_imm_t1,
			0xf0000, 0x0,
			0, 0);

	// orn_reg_t1: 1110 1010 011x xxxx 0<x's>
	register_opcode_mask_32_ex(0xea600000, 0x15808000, orn_reg_t1,
			0xf0000, 0x0,
			0, 0);

	// orr_imm_t1: 1111 0x00 010x xxxx 0<x's>
	register_opcode_mask_32_ex(0xf0400000, 0x0ba08000, orr_imm_t1,
			0xf0000, 0x0,
			0, 0);

	// Cannot allow               1111 case
	// orr_reg_t2: 1110 1010 010x xxxx 0<x's>
	register_opcode_mask_32_ex(0xea400000, 0x15a08000, orr_reg_t2,
			0x000f0000, 0x00000000,
			0, 0);
}
