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

#include "cpu/registers.h"
#include "cpu/misc.h"

static void adc_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags) {
	uint32_t rn_val = CORE_reg_read(rn);

	union apsr_t apsr = CORE_apsr_read();
	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, imm32, apsr.bits.C, &result, &carry, &overflow);
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

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

static void adc_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted = Shift(CORE_reg_read(rm), 32,
			shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(CORE_reg_read(rn), shifted, apsr.bits.C,
			&result, &carry, & overflow);

	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

// arm-thumb
static void adc_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;
	bool setflags = !in_ITblock();

	OP_DECOMPILE("ADC<IT> <Rdn>,<Rm>", rdn, rm);

	return adc_reg(rdn, rdn, rm, setflags, SRType_LSL, 0);
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

static void add_imm(uint8_t rn, uint8_t rd, uint32_t imm32, uint8_t setflags) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, imm32, 0, &result, &carry, &overflow);

	CORE_reg_write(rd, result);

	if (setflags) {
		union apsr_t apsr = CORE_apsr_read();
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}

	DBG2("add r%02d = r%02d + 0x%08x\t%08x = %08x + %08x\n",
			rd, rn, imm32, result, rn_val, imm32);
}

// arm-thumb
static void add_imm_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm3 = (inst >> 6) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm3;

	OP_DECOMPILE("ADD<IT> <Rd>,<Rn>,#<imm3>", rd, rn, imm3);
	return add_imm(rn, rd, imm32, setflags);
}

// arm-thumb
static void add_imm_t2(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rdn = (inst >> 8) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm8;

	OP_DECOMPILE("ADD<IT> <Rdn>,#<imm8>", rdn, imm8);
	return add_imm(rdn, rdn, imm32, setflags);
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

static void add_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted;
	shifted = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, shifted, 0, &result, &carry, &overflow);

	if (rd == 15) {
		// ALUWritePC
		CORE_ERR_not_implemented("ALUWritePC case add_reg\n");
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			apsr.bits.N = HIGH_BIT(result);
			apsr.bits.Z = result == 0;
			apsr.bits.C = carry;
			apsr.bits.V = overflow;
			CORE_apsr_write(apsr);
		}
	}

	DBG2("add_reg r%02d = 0x%08x\n", rd, result);
}

// arm-thumb
static void add_reg_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool setflags = !in_ITblock();

	OP_DECOMPILE("ADD<IT> <Rd>,<Rn>,<Rm>", rd, rn, rm);
	return add_reg(rd, rn, rm, setflags, SRType_LSL, 0);
}

// arm-thumb
static void add_reg_t2(uint16_t inst) {
	uint8_t rm = (inst & 0x78) >> 3;
	uint8_t rd = (((inst & 0x80) >> 7) | ((inst & 0x7) >> 0));

	if ((rd == 13) || (rm == 13))
		CORE_ERR_not_implemented("add_reg_t2 -> SP+reg\n");

	// d = UInt(DN:Rdn); n = d; m = UInt(Rm);
	uint8_t rn = rd;
	bool setflags = false;

	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(SRType_LSL, 0, &shift_t, &shift_n);

	if ((rd == 15) && (rm == 15))
		CORE_ERR_unpredictable("add_reg_t2\n");

	if ((rd == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("add_reg_t2 it\n");

	OP_DECOMPILE("ADD<c> <Rdn>,<Rm>", rd, rm);
	return add_reg(rd, rn, rm, setflags, shift_t, shift_n);
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

static void add_sp_plus_imm(uint8_t rd, uint32_t imm32, bool setflags) {
	uint32_t sp_val = CORE_reg_read(SP_REG);

	uint32_t result;
	bool carry;
	bool overflow;

	AddWithCarry(sp_val, imm32, 0, &result, &carry, &overflow);

	CORE_reg_write(rd, result);

	if (setflags) {
		union apsr_t apsr = CORE_apsr_read();
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

// arm-thumb
static void add_sp_plus_imm_t1(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0x7;

	bool setflags = false;
	uint32_t imm32 = (imm8 << 2);

	OP_DECOMPILE("ADD<c> <Rd>,SP,#<imm8>", rd, imm8);
	return add_sp_plus_imm(rd, imm32, setflags);
}

// arm-thumb
static void add_sp_plus_imm_t2(uint16_t inst) {
	uint8_t imm7 = inst & 0x7f;

	uint8_t rd = 13;
	bool setflags = false;
	uint32_t imm32 = (imm7 << 2);

	OP_DECOMPILE("ADD<c> SP,SP,#<imm7>", imm32);
	return add_sp_plus_imm(rd, imm32, setflags);
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

static void add_sp_plus_reg(uint8_t rd, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n) {

	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted = Shift(CORE_reg_read(rm), 32,
			shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(CORE_reg_read(SP_REG), shifted, 0,
			&result, &carry, &overflow);

	if (rd == 15) {
		assert(setflags == false);
		//ALUWritePC(result);
		CORE_ERR_not_implemented("ALUWritePC add_sp_plus_reg\n");
	} else {
		CORE_reg_write(rd, result);

		if (setflags) {
			apsr.bits.N = HIGH_BIT(result);
			apsr.bits.Z = result == 0;
			apsr.bits.C = carry;
			apsr.bits.V = overflow;
			CORE_apsr_write(apsr);
		}
	}
}

// arm-thumb
static void add_sp_plus_reg_t1(uint16_t inst) {
	uint8_t rdm = inst & 0x7;
	bool DM     = (inst >> 7) & 0x1;

	uint8_t rd = rdm | (DM << 3);
	uint8_t rm = rdm | (DM << 3);
	bool setflags = false;

	if ((rd == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("add_sp_plus_reg_t1 case\n");

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("ADD<c> <Rdm>,SP,<Rdm>", rd, rd);
	return add_sp_plus_reg(rd, rm, setflags, shift_t, shift_n);
}

// arm-thumb
static void add_sp_plus_reg_t2(uint16_t inst) {
	uint8_t rm = (inst >> 3) & 0xf;

	uint8_t rd = 13;
	bool setflags = false;
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = 0;

	OP_DECOMPILE("ADD<c> SP,<Rm>", rm);
	return add_sp_plus_reg(rd, rm, setflags, shift_t, shift_n);
}

// arm-v7-m
static void add_sp_plug_reg_t3(uint32_t inst) {
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

static void adr(uint8_t rd, bool add, uint32_t imm32) {
	uint32_t result;
	if (add)
		result = (CORE_reg_read(PC_REG) & 0xfffffffc) + imm32;
	else
		result = (CORE_reg_read(PC_REG) & 0xfffffffc) - imm32;
	CORE_reg_write(rd, result);
}

// arm-thumb
static void adr_t1(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0x7;

	uint32_t imm32 = imm8 << 2;
	bool add = true;

	OP_DECOMPILE("ADR<c> <Rd>,<label>", rd, imm32);
	return adr(rd, add, imm32);
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
static void register_opcodes_add(void) {
	// adc_imm_t1: 1111 0x01 010x xxxx 0xxx xxxx xxxx xxxx
	register_opcode_mask_32(0xf1400000, 0x0aa08000, adc_imm_t1);

	// adc_reg_t1: 0100 0001 01<x's>
	register_opcode_mask_16(0x4140, 0xbe80, adc_reg_t1);

	// adc_reg_t2: 1110 1011 010x xxxx 0<x's>
	register_opcode_mask_32(0xeb400000, 0x14a08000, adc_reg_t2);

	// add_imm_t1: 0001 110x xxxx xxxx
	register_opcode_mask_16(0x1c00, 0xe200, add_imm_t1);

	// add_imm_t2: 0011 0<x's>
	register_opcode_mask_16(0x3000, 0xc800, add_imm_t2);

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

	// add_reg_t1: 0001 100<x's>
	register_opcode_mask_16(0x1800, 0xe600, add_reg_t1);

	// add_reg_t2: 0100 0100 <x's>
	register_opcode_mask_16(0x4400, 0xbb00, add_reg_t2);

	// add_reg_t3: 1110 1011 000x xxxx 0<x's>
	register_opcode_mask_32_ex(0xeb000000, 0x14e08000, add_reg_t3,
			0x100f00, 0x0,
			0xd0000, 0x20000,
			0, 0);

	// add_sp_plus_imm_t1: 1010 1<x's>
	register_opcode_mask_16(0xa800, 0x5000, add_sp_plus_imm_t1);

	// add_sp_plus_imm_t2: 1011 0000 0<x's>
	register_opcode_mask_16(0xb000, 0x4f80, add_sp_plus_imm_t2);

	// add_sp_plus_imm_t3: 1111 0x01 000x 1101 0<x's>
	register_opcode_mask_32_ex(0xf10d0000, 0x0ae28000, add_sp_plus_imm_t3,
			0x100f00, 0x0,
			0, 0);

	// add_sp_plus_imm_t4: 1111 0x10 0000 1101 0<x's>
	register_opcode_mask_32(0xf20d0000, 0x09f28000, add_sp_plus_imm_t4);

	// add_sp_plus_reg_t1: 0100 0100 x110 1xxx
	register_opcode_mask_16(0x4468, 0xbb10, add_sp_plus_reg_t1);

	// add_sp_plus_reg_t2: 0100 0100 1xxx x101
	register_opcode_mask_16_ex(0x4485, 0xbb02, add_sp_plus_reg_t2,
			0x68, 0x10,
			0, 0);

	// add_sp_plug_reg_t3: 1110 1011 000x 1101 0<x's>
	register_opcode_mask_32(0xeb0d0000, 0x14e28000, add_sp_plug_reg_t3);

	// adr_t1: 1010 0<x's>
	register_opcode_mask_16(0xa000, 0x5800, adr_t1);

	// adr_t2: 1111 0x10 1010 1111 0<x's>
	register_opcode_mask_32(0xf2af0000, 0x09508000, adr_t2);

	// adr_t3: 1111 0x10 0000 1111 0<x's>
	register_opcode_mask_32(0xf20f0000, 0x09f08000, adr_t3);
}
