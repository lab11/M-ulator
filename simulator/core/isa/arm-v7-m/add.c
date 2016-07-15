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

//XXX: here or ops?
//#include "core/operations/helpers.h"

#include "core/operations/add.h"

// arm-v7-m
static void adc_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool S = (inst >> 20) & 0x1;
	bool i = !!(inst & 0x04000000);

	bool setflags = S;
	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | imm8);

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("Bad reg\n");

	OP_DECOMPILE("ADC{S}<c> <Rd>,<Rn>,#<const>", setflags, rd, rn, imm32);
	return adc_imm(rd, rn, imm32, setflags);
}

// arm-v7-m
static void adc_reg_t2(uint32_t inst) {
	uint8_t rm   = inst & 0xf;
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

	if (BadReg(rd) || BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("adc_reg_t2 case\n");

	OP_DECOMPILE("ADC{S}<c>.W <Rd>,<Rn>,<Rm>{,<shift>}",
			setflags, rd, rn, rm, shift_t, shift_n);
	return adc_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

// arm-v7-m
static void add_imm_t3(uint32_t inst) {
	uint32_t imm8 =    (inst & 0x000000ff);
	uint8_t rd =  (inst & 0x00000f00) >> 8;
	uint32_t imm3 =    (inst & 0x00007000) >> 12;
	assert(0 ==   (inst & 0x00008000));
	uint8_t rn =  (inst & 0x000f0000) >> 16;
	uint32_t S = !!(inst & 0x00100000);
	// skip 5
	uint32_t i = !!(inst & 0x04000000);

	uint8_t setflags = (S == 1);

	uint32_t imm12 = (i << 11) | (imm3 << 8) | imm8;
	DBG2("i %d imm3 %01x imm8 %02x\t(imm12 %0x)\n", i, imm3, imm8, imm12);
	//int imm32 = ThumbExpandImm(i:imm3:imm8);
	uint32_t imm32 = ThumbExpandImm(imm12);

	if ((rd == 13 || ((rd == 15) && (S == 0))) || (rn == 15))
		CORE_ERR_unpredictable("add_imm_t3 case\n");

	OP_DECOMPILE("ADD{S}<c>.W <Rd>,<Rn>,#<const>", setflags, rd, rn, imm32);
	return add_imm(rn, rd, imm32, setflags);
}

// arm-v7-m
static void add_imm_t4(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool i       = (inst >> 26) & 0x1;

	bool setflags = false;

	uint32_t imm32 = imm8 | (imm3 << 8) | (i << 11);

	if (rd >= 13)
		CORE_ERR_unpredictable("add_imm_t4 case\n");

	OP_DECOMPILE("ADDW<c> <Rd>,<Rn>,#<imm12>", rd, rn, imm32);
	return add_imm(rn, rd, imm32, setflags);
}

// arm-v7-m
static void add_reg_t3(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool S = !!(inst & 0x100000);

	bool setflags = S;

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd == 13) || ((rd == 15) && (S == 0)) || (rn == 15) || BadReg(rm))
		CORE_ERR_unpredictable("bad regs\n");

	OP_DECOMPILE("ADD{S}.W <Rd>,<Rn>,<Rm>{,<shift>}",
			setflags, rd, rn, rm, shift_t, shift_n);
	return add_reg(rd, rn, rm, setflags, shift_t, shift_n);
}


// arm-v7-m
static void add_sp_plus_imm_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool S = !!(inst & 0x100000);
	bool i = !!(inst & 0x04000000);

	bool setflags = S;
	uint32_t imm12 = (i << 11) | (imm3 << 8) | (imm8);
	uint32_t imm32 = ThumbExpandImm(imm12);

	if ((rd == 15) && (S == 0))
		CORE_ERR_unpredictable("r15 w/out S\n");

	OP_DECOMPILE("ADD{S}<c>.W <Rd>,SP,#<const>",
			setflags, rd, imm32);
	return add_sp_plus_imm(rd, imm32, setflags);
}

// arm-v7-m
static void add_sp_plus_imm_t4(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool i = !!(inst & 0x04000000);

	bool setflags = false;
	uint32_t imm32 = (i << 11) | (imm3 << 8) | (imm8);

	if (rd == 15)
		CORE_ERR_unpredictable("bad reg 15\n");

	OP_DECOMPILE("ADDW<c> <Rd>,SP,#<imm12>", rd, imm32);
	return add_sp_plus_imm(rd, imm32, setflags);
}

// arm-v7-m
static void add_sp_plus_reg_t3(uint32_t inst) {
	uint8_t rm   = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool    S    = (inst >> 16) & 0x1;

	bool setflags = (S == 1);
	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if (
			((rd == 13) && ((shift_t != SRType_LSL) || shift_n > 3)) ||
			((rd == 15) || (rm > 13))
	   )
		CORE_ERR_unpredictable("add_sp_plug_reg_t3 case\n");

	OP_DECOMPILE("ADD{S}<c>.W <Rd>,SP,<Rm>{,<shift>}",
			setflags, rd, rm, shift_t, shift_n);
	return add_sp_plus_reg(rd, rm, setflags, shift_t, shift_n);
}

// arm-v7-m
static void adr_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool    i    = (inst >> 26) & 0x1;

	uint32_t imm32 = imm8 | (imm3 << 8) | (i << 11);
	bool add = false;

	if (rd > 13)
		CORE_ERR_unpredictable("adr_t2 case\n");

	if (imm32 == 0)
		OP_DECOMPILE("SUB <Rd>,PC,#0", rd);
	else
		OP_DECOMPILE("ADR<c>.W <Rd>,<label>", rd, imm32);
	return adr(rd, add, imm32);
}

// arm-v7-m
static void adr_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool    i    = (inst >> 26) & 0x1;

	uint32_t imm32 = imm8 | (imm3 << 8) | (i << 11);
	bool add = true;

	if (rd > 13)
		CORE_ERR_unpredictable("adr_t2 case\n");

	OP_DECOMPILE("ADR<c>.W <Rd>,<label>", rd, imm32);
	return adr(rd, add, imm32);
}


__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_add(void) {
	// adc_imm_t1: 1111 0x01 010x xxxx 0xxx xxxx xxxx xxxx
	register_opcode_mask_32(0xf1400000, 0x0aa08000, adc_imm_t1);

	// adc_reg_t2: 1110 1011 010x xxxx 0<x's>
	register_opcode_mask_32(0xeb400000, 0x14a08000, adc_reg_t2);

	// add_imm_t3: 1111 0x01 000x xxxx 0<x's>
	register_opcode_mask_32_ex(0xf1000000, 0x0ae08000, add_imm_t3,
			0x100f00, 0x0,
			0xd0000, 0x20000,
			0, 0);

	// add_imm_t4: 1111 0x10 0000 xxxx 0<x's>
	register_opcode_mask_32_ex(0xf2000000, 0x09f08000, add_imm_t4,
			0xf0000, 0x0,
			0xd0000, 0x20000,
			0, 0);

	// add_reg_t3: 1110 1011 000x xxxx 0<x's>
	register_opcode_mask_32_ex(0xeb000000, 0x14e08000, add_reg_t3,
			0x100f00, 0x0,
			0xd0000, 0x20000,
			0, 0);

	// add_sp_plus_imm_t3: 1111 0x01 000x 1101 0<x's>
	register_opcode_mask_32_ex(0xf10d0000, 0x0ae28000, add_sp_plus_imm_t3,
			0x100f00, 0x0,
			0, 0);

	// add_sp_plus_imm_t4: 1111 0x10 0000 1101 0<x's>
	register_opcode_mask_32(0xf20d0000, 0x09f28000, add_sp_plus_imm_t4);

	// add_sp_plus_reg_t3: 1110 1011 000x 1101 0<x's>
	register_opcode_mask_32(0xeb0d0000, 0x14e28000, add_sp_plus_reg_t3);

	// adr_t2: 1111 0x10 1010 1111 0<x's>
	register_opcode_mask_32(0xf2af0000, 0x09508000, adr_t2);

	// adr_t3: 1111 0x10 0000 1111 0<x's>
	register_opcode_mask_32(0xf20f0000, 0x09f08000, adr_t3);
}
