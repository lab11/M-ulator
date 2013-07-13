/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2013  Pat Pannuto <pat.pannuto@gmail.com>
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

#include "opcodes.h"
#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/misc.h"

static void shift_imm(union apsr_t apsr, uint8_t setflags, uint8_t rd, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t result;
	bool carry;
	Shift_C(rm_val, 32, shift_t, shift_n, apsr.bits.C, &result, &carry);

	if (rd == 15) {
		// ALUWritePC(result);
		CORE_ERR_not_implemented("ALUWritePC shift_imm\n");
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			apsr.bits.N = HIGH_BIT(result);
			apsr.bits.Z = result == 0;
			apsr.bits.C = carry;
			CORE_apsr_write(apsr);
		}
	}

	DBG2("shift_imm complete\n");
}

static void shift_imm_t1(uint16_t inst, enum SRType shift_t) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;
	bool setflags = !in_ITblock();

	uint8_t shift_n;
	DecodeImmShift(SRType_ENUM_TO_MASK(shift_t), imm5, &shift_t, &shift_n);

	return shift_imm(CORE_apsr_read(), setflags, rd, rm,
			shift_t, shift_n);
}

// arm-thumb
static void asr_imm_t1(uint16_t inst) {
	return shift_imm_t1(inst, ASR);
}

// arm-thumb
static void lsl_imm_t1(uint16_t inst) {
	return shift_imm_t1(inst, LSL);
}

// arm-thumb
static void lsr_imm_t1(uint16_t inst) {
	return shift_imm_t1(inst, LSR);
}

// arm-v7-m
static void ror_imm_t1(uint32_t inst) {
	return shift_imm_t1(inst, ROR);
}

static void shift_imm_t2(uint32_t inst, enum SRType shift_t) {
	union apsr_t apsr = CORE_apsr_read();

	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool S = !!(inst & 0x100000);

	uint8_t setflags = (S == 1);

	uint8_t imm5 = (imm3 << 2) | imm2;
	uint8_t shift_n;
	DecodeImmShift(SRType_ENUM_TO_MASK(shift_t), imm5, &shift_t, &shift_n);

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("BadReg in shift_imm_t2\n");

	return shift_imm(apsr, setflags, rd, rm, shift_t, shift_n);
}

// arm-v7-m
static void asr_imm_t2(uint32_t inst) {
	return shift_imm_t2(inst, ASR);
}

// arm-v7-m
static void lsl_imm_t2(uint32_t inst) {
	return shift_imm_t2(inst, LSL);
}

// arm-v7-m
static void lsr_imm_t2(uint32_t inst) {
	return shift_imm_t2(inst, LSR);
}

static void shift_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		enum SRType shift_t, bool setflags) {
	uint8_t shift_n = CORE_reg_read(rm);

	uint32_t result;
	bool carry;

	union apsr_t apsr = CORE_apsr_read();

	Shift_C(CORE_reg_read(rn), 32, shift_t, shift_n, apsr.bits.C,
			&result, &carry);

	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		CORE_apsr_write(apsr);
	}
}

static void shift_reg_t1(uint16_t inst, enum SRType shift_t) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rd = rdn;
	uint8_t rn = rdn;
	bool setflags = !in_ITblock();

	return shift_reg(rd, rn, rm, shift_t, setflags);
}

static void shift_reg_t2(uint32_t inst, enum SRType shift_t) {
	uint8_t rm = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;
	bool    S  = (inst >> 20) & 0x1;

	bool setflags = S == 1;

	if ((rd > 13) || (rn > 13) || (rm > 13))
		CORE_ERR_unpredictable("shift_reg_t2 case\n");

	return shift_reg(rd, rn, rm, shift_t, setflags);
}

// arm-thumb
static void asr_reg_t1(uint16_t inst) {
	return shift_reg_t1(inst, SRType_ASR);
}

// arm-v7-m
static void asr_reg_t2(uint32_t inst) {
	return shift_reg_t2(inst, SRType_ASR);
}

// arm-thumb
static void lsl_reg_t1(uint16_t inst) {
	return shift_reg_t1(inst, SRType_LSL);
}

// arm-v7-m
static void lsl_reg_t2(uint32_t inst) {
	return shift_reg_t2(inst, SRType_LSL);
}

// arm-thumb
static void lsr_reg_t1(uint16_t inst) {
	return shift_reg_t1(inst, SRType_LSR);
}

// arm-v7-m
static void lsr_reg_t2(uint32_t inst) {
	return shift_reg_t2(inst, SRType_LSR);
}

// arm-thumb
static void ror_reg_t1(uint16_t inst) {
	return shift_reg_t1(inst, SRType_ROR);
}

// arm-v7-m
static void ror_reg_t2(uint32_t inst) {
	return shift_reg_t2(inst, SRType_ROR);
}

static inline void rrx(uint8_t rm, uint8_t rd, bool setflags) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t result;
	bool carry_out;
	Shift_C(CORE_reg_read(rm), 32, SRType_RRX, 1, apsr.bits.C,
			&result, &carry_out);

	CORE_reg_write(rd, result);
	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry_out;
		CORE_apsr_write(apsr);
	}
}

// arm-v7-m
static void rrx_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	bool    S  = (inst >> 20) & 0x1;

	bool setflags = S==1;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("rrx_t1 case\n");

	return rrx(rm, rd, setflags);
}

__attribute__ ((constructor))
void register_opcodes_shift(void) {
	// asr_imm_t1: 0001 0xxx xxxx xxxx
	register_opcode_mask_16(0x1000, 0xe800, asr_imm_t1);

	// asr_imm_t2: 1110 1010 010x 1111 0xxx xxxx xx10 xxxx
	register_opcode_mask_32(0xea4f0020, 0x15a08010, asr_imm_t2);

	// lsl_imm_t1: 0000 0<x's>
	register_opcode_mask_16_ex(0x0000, 0xf800, lsl_imm_t1,
			0x0, 0x7c0,
			0, 0);

	// lsl_imm_t2: 1110 1010 010x 1111 0xxx xxxx xx00 xxxx
	register_opcode_mask_32(0xea4f0000, 0x15a08030, lsl_imm_t2);

	// lsr_imm_t1: 0000 1<x's>
	register_opcode_mask_16(0x0800, 0xf000, lsr_imm_t1);

	// lsr_imm_t2: 1110 1010 010x 1111 0xxx xxxx xx01 xxxx
	register_opcode_mask_32(0xea4f0010, 0x15a08020, lsr_imm_t2);

	// ror_imm_t1: 1110 1010 010x 1111 0xxx xxxx xx11 xxxx
	register_opcode_mask_32_ex(0xea4f0030, 0x15a08000, ror_imm_t1,
			0x0, 0x70c0,
			0, 0);

	// asr_reg_t1: 0100 0001 00xx xxxx
	register_opcode_mask_16(0x4100, 0xbec0, asr_reg_t1);

	// asr_reg_t2: 1111 1010 010x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask_32(0xfa40f000, 0x05a000f0, asr_reg_t2);

	// lsl_reg_t1: 0100 0000 10 <x's>
	register_opcode_mask_16(0x4080, 0xbf40, lsl_reg_t1);

	// lsl_reg_t2: 1111 1010 000x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask_32(0xfa00f000, 0x05e000f0, lsl_reg_t2);

	// lsr_reg_t1: 0100 0000 11 <x's>
	register_opcode_mask_16(0x40c0, 0xbf00, lsr_reg_t1);

	// lsr_reg_t2: 1111 1010 001x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask_32(0xfa20f000, 0x05c000f0, lsr_reg_t2);

	// ror_reg_t1: 0100 0001 11<x's>
	register_opcode_mask_16(0x41c0, 0xbe00, ror_reg_t1);

	// ror_reg_t2: 1111 1010 011x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask_32(0xfa60f000, 0x058000f0, ror_reg_t2);

	// rrx_t1: 1110 1010 010x 1111 0000 xxxx 0011 xxxx
	register_opcode_mask_32(0xea4f0030, 0x15a0f0c0, rrx_t1);
}
