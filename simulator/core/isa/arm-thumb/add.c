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
#include "core/isa/arm_types.h"

#include "core/operations/add.h"

// arm-thumb
static void adc_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;
	bool setflags = !in_ITblock();

	OP_DECOMPILE("ADC<IT> <Rdn>,<Rm>", rdn, rm);

	return adc_reg(rdn, rdn, rm, setflags, SRType_LSL, 0);
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

// arm-thumb
static void adr_t1(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0x7;

	uint32_t imm32 = imm8 << 2;
	bool add = true;

	OP_DECOMPILE("ADR<c> <Rd>,<label>", rd, imm32);
	return adr(rd, add, imm32);
}


__attribute__ ((constructor))
static void register_opcodes_arm_thumb_add(void) {
	// adc_reg_t1: 0100 0001 01<x's>
	register_opcode_mask_16(0x4140, 0xbe80, adc_reg_t1);

	// add_imm_t1: 0001 110x xxxx xxxx
	register_opcode_mask_16(0x1c00, 0xe200, add_imm_t1);

	// add_imm_t2: 0011 0<x's>
	register_opcode_mask_16(0x3000, 0xc800, add_imm_t2);

	// add_reg_t1: 0001 100<x's>
	register_opcode_mask_16(0x1800, 0xe600, add_reg_t1);

	// add_reg_t2: 0100 0100 <x's>
	register_opcode_mask_16(0x4400, 0xbb00, add_reg_t2);

	// add_sp_plus_imm_t1: 1010 1<x's>
	register_opcode_mask_16(0xa800, 0x5000, add_sp_plus_imm_t1);

	// add_sp_plus_imm_t2: 1011 0000 0<x's>
	register_opcode_mask_16(0xb000, 0x4f80, add_sp_plus_imm_t2);

	// add_sp_plus_reg_t1: 0100 0100 x110 1xxx
	register_opcode_mask_16(0x4468, 0xbb10, add_sp_plus_reg_t1);

	// add_sp_plus_reg_t2: 0100 0100 1xxx x101
	register_opcode_mask_16_ex(0x4485, 0xbb02, add_sp_plus_reg_t2,
			0x68, 0x10,
			0, 0);

	// adr_t1: 1010 0<x's>
	register_opcode_mask_16(0xa000, 0x5800, adr_t1);
}
