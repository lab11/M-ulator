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

// XXX: Workaround for unified shift decoders
#include "core/isa/opcodes.h"

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/misc.h"

void shift_imm(union apsr_t apsr, uint8_t setflags, uint8_t rd, uint8_t rm,
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

void shift_imm_t1(uint16_t inst, enum SRType shift_t) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;
	bool setflags = !in_ITblock();

	uint8_t shift_n;
	DecodeImmShift(SRType_ENUM_TO_MASK(shift_t), imm5, &shift_t, &shift_n);

	if (shift_t == ASR)
		OP_DECOMPILE("ASR<IT> <Rd>,<Rm>,#<imm5>", rd, rm, imm5);
	if (shift_t == LSL)
		OP_DECOMPILE("LSL<IT> <Rd>,<Rm>,#<imm5>", rd, rm, imm5);
	if (shift_t == LSR)
		OP_DECOMPILE("LSR<IT> <Rd>,<Rm>,#<imm5>", rd, rm, imm5);
	if (shift_t == ROR)
		OP_DECOMPILE("ROR<IT> <Rd>,<Rm>,#<imm5>", rd, rm, imm5);
	return shift_imm(CORE_apsr_read(), setflags, rd, rm,
			shift_t, shift_n);
}

void shift_imm_t2(uint32_t inst, enum SRType shift_t) {
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

	if (shift_t == ASR)
		OP_DECOMPILE("ASR{S}<c>.W <Rd>,<Rm>,#<imm5>", setflags, rd, rm, imm5);
	if (shift_t == LSL)
		OP_DECOMPILE("LSL{S}<c>.W <Rd>,<Rm>,#<imm5>", setflags, rd, rm, imm5);
	if (shift_t == LSR)
		OP_DECOMPILE("LSR{S}<c>.W <Rd>,<Rm>,#<imm5>", setflags, rd, rm, imm5);
	if (shift_t == ROR)
		OP_DECOMPILE("ROR{S}<c>.W <Rd>,<Rm>,#<imm5>", setflags, rd, rm, imm5);
	return shift_imm(apsr, setflags, rd, rm, shift_t, shift_n);
}

void shift_reg(uint8_t rd, uint8_t rn, uint8_t rm,
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

void shift_reg_t1(uint16_t inst, enum SRType shift_t) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rd = rdn;
	uint8_t rn = rdn;
	bool setflags = !in_ITblock();

	if (shift_t == ASR)
		OP_DECOMPILE("ASR<IT> <Rdn>,<Rm>", rd, rm);
	if (shift_t == LSL)
		OP_DECOMPILE("LSL<IT> <Rdn>,<Rm>", rd, rm);
	if (shift_t == LSR)
		OP_DECOMPILE("LSR<IT> <Rdn>,<Rm>", rd, rm);
	if (shift_t == ROR)
		OP_DECOMPILE("ROR<IT> <Rdn>,<Rm>", rd, rm);
	return shift_reg(rd, rn, rm, shift_t, setflags);
}

void shift_reg_t2(uint32_t inst, enum SRType shift_t) {
	uint8_t rm = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;
	bool    S  = (inst >> 20) & 0x1;

	bool setflags = S == 1;

	if ((rd > 13) || (rn > 13) || (rm > 13))
		CORE_ERR_unpredictable("shift_reg_t2 case\n");

	if (shift_t == ASR)
		OP_DECOMPILE("ASR{S}<c>.W <Rd>,<Rn>,<Rm>", rd, rn, rm);
	if (shift_t == LSL)
		OP_DECOMPILE("LSL{S}<c>.W <Rd>,<Rn>,<Rm>", rd, rn, rm);
	if (shift_t == LSR)
		OP_DECOMPILE("LSR{S}<c>.W <Rd>,<Rn>,<Rm>", rd, rn, rm);
	if (shift_t == ROR)
		OP_DECOMPILE("ROR{S}<c>.W <Rd>,<Rn>,<Rm>", rd, rn, rm);
	return shift_reg(rd, rn, rm, shift_t, setflags);
}

void rrx(uint8_t rm, uint8_t rd, bool setflags) {
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
