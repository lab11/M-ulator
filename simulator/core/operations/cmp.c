/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
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

#include "cpu/cpu.h"

/* CMP and TST always write cpsr bits, regardless of itstate */

static void cmn_imm(uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t cpsr = CORE_cpsr_read();

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, imm32, 0, &result, &carry, &overflow);

	cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0, carry, overflow);
	CORE_cpsr_write(cpsr);
}

static void cmn_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool i = !!(inst & 0x04000000);

	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | (imm8));

	if (rn == 15)
		CORE_ERR_unpredictable("bad reg\n");

	return cmn_imm(rn, imm32);
}

static void cmp_imm(uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result;
	bool carry_out;
	bool overflow_out;

	DBG2("rn_val: %08x, ~imm32: %08x\n", rn_val, ~imm32);
	AddWithCarry(rn_val, ~imm32, 1, &result, &carry_out, &overflow_out);
	DBG2("result: %08x, carry: %d, overflow: %d\n",
			result, carry_out, overflow_out);

	uint32_t cpsr = CORE_cpsr_read();

	cpsr = GEN_NZCV(
			!!(result & xPSR_N),
			result == 0,
			carry_out,
			overflow_out
		       );
	CORE_cpsr_write(cpsr);
}

static void cmp_imm_t1(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rn = (inst >> 8) & 0x7;

	uint32_t imm32 = imm8;

	return cmp_imm(rn, imm32);
}

static void cmp_imm_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool i = (inst >> 26) & 0x1;

	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | imm8);

	DBG2("imm32: %08x\n", imm32);

	return cmp_imm(rn, imm32);
}

static void cmp_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t cpsr = CORE_cpsr_read();
	uint32_t shifted = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, ~shifted, 1, &result, &carry, &overflow);

	cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0, carry, overflow);
	CORE_cpsr_write(cpsr);
}

static void cmp_reg_t1(uint16_t inst) {
	uint8_t rn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	return cmp_reg(rn, rm, SRType_LSL, 0);
}

static void cmp_reg_t2(uint16_t inst) {
	uint8_t rn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0xf;
	bool N = (inst >> 7) & 0x1;

	rn = (N << 3) | rn;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	if ((rn < 8) && (rm < 8))
		CORE_ERR_unpredictable("cmp bad reg\n");

	if ((rn == 15) && (rm == 15))
		CORE_ERR_unpredictable("cmp bad regs\n");

	return cmp_reg(rn, rm, shift_t, shift_n);
}

static void cmp_reg_t3(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;

	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, (imm3 << 2) | imm2, &shift_t, &shift_n);

	if ((rn == 15) || BadReg(rm))
		CORE_ERR_unpredictable("bad regs\n");

	return cmp_reg(rn, rm, shift_t, shift_n);
}

__attribute__ ((constructor))
void register_opcodes_cmp(void) {
	// cmn_imm_t1: 1111 0x01 0001 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xf1100f00, 0x0ae08000, cmn_imm_t1);

	// cmp_imm_t1: 0010 1<x's>
	register_opcode_mask_16(0x2800, 0xd000, cmp_imm_t1);

	// cmp_imm_t2: 1111 0x01 1011 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xf1b00f00, 0x0a408000, cmp_imm_t2);

	// cmp_reg_t1: 0100 0010 10<x's>
	register_opcode_mask_16(0x4280, 0xbd40, cmp_reg_t1);

	// cmp_reg_t2: 0100 0101 xxxx xxxx
	register_opcode_mask_16(0x4500, 0xba00, cmp_reg_t2);

	// cmp_reg_t3: 1110 1011 1011 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xebb00f00, 0x14408000, cmp_reg_t3);
}
