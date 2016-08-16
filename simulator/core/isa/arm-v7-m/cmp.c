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

#include "core/operations/cmp.h"

// arm-v7-m
static void cmn_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool i = !!(inst & 0x04000000);

	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | (imm8));

	if (rn == 15)
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("CMN<c> <Rn>,#<const>", rn, imm32);
	return cmn_imm(rn, imm32);
}

// arm-v7-m
static void cmn_reg_t2(uint32_t inst) {
	uint8_t rm   = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;

	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if ((rn == 15) || (rm > 13))
		CORE_ERR_unpredictable("cmn_reg_t2 case\n");

	OP_DECOMPILE("CMN<c>.W <Rn>,<Rm>{,<shift>}", rn, rm, shift_t, shift_n);
	return cmn_reg(rn, rm, shift_t, shift_n);
}

// arm-v7-m
static void cmp_imm_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool i = (inst >> 26) & 0x1;

	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | imm8);

	DBG2("imm32: %08x\n", imm32);

	OP_DECOMPILE("CMP<c>.W <Rn>,#<const>", rn, imm32);
	return cmp_imm(rn, imm32);
}

// arm-v7-m
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

	OP_DECOMPILE("CMP<c>.W <Rn>,<Rm>{,<shift>}", rn, rm, shift_t, shift_n);
	return cmp_reg(rn, rm, shift_t, shift_n);
}

// arm-v7-m
static void teq_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    i    = (inst >> 26) & 0x1;

	union apsr_t apsr = CORE_apsr_read();
	uint32_t imm32;
	bool carry;
	ThumbExpandImm_C(imm8 | (imm3 << 8) | (i << 11), apsr.bits.C,
			&imm32, &carry);

	if (BadReg(rn))
		CORE_ERR_unpredictable("teq_imm_t1 case\n");

	OP_DECOMPILE("TEQ<c> <Rn>,#<const>", rn, imm32);
	return teq_imm(rn, imm32, carry);
}

// arm-v7-m
static void teq_reg_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;

	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if (BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("teq_reg_t1 case\n");

	OP_DECOMPILE("TEQ<c> <Rn>,<Rm>{,<shift>}", rn, rm, shift_t, shift_n);
	return teq_reg(rn, rm, shift_t, shift_n);
}

// arm-v7-m
static void tst_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool i = !!(inst & 0x04000000);

	uint16_t imm12 = (i << 11) | (imm3 << 8) | (imm8);
	uint32_t imm32;
	bool carry;
	union apsr_t apsr = CORE_apsr_read();

	ThumbExpandImm_C(imm12, apsr.bits.C, &imm32, &carry);

	if ((rn == 13) || (rn == 15))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("TST<c> <Rn>,#<const>", rn, imm32);
	return tst_imm(rn, imm32, carry);
}

// arm-v7-m
static void tst_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;

	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if (BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("tst_reg_t2 case\n");

	OP_DECOMPILE("TST<c>.W <Rn>,<Rm>{,<shift>}",
			rn, rm, shift_t, shift_n);
	return tst_reg(rn, rm, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_cmp(void) {
	// cmn_imm_t1: 1111 0x01 0001 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xf1100f00, 0x0ae08000, cmn_imm_t1);

	// cmn_reg_t2: 1110 1011 0001 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xeb100f00, 0x14e08000, cmn_reg_t2);

	// cmp_imm_t2: 1111 0x01 1011 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xf1b00f00, 0x0a408000, cmp_imm_t2);

	// cmp_reg_t3: 1110 1011 1011 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xebb00f00, 0x14408000, cmp_reg_t3);

	// teq_imm_t1: 1111 0x00 1001 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xf0900f00, 0x0b608000, teq_imm_t1);

	// teq_reg_t1: 1110 1010 1001 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xea900f00, 0x15608000, teq_reg_t1);

	// tst_imm_t1: 1111 0x00 0001 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xf0100f00, 0x0be08000, tst_imm_t1);

	// tst_reg_t2: 1110 1010 0001 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xea100f00, 0x15e08000, tst_reg_t2);
}
