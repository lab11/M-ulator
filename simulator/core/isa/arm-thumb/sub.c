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

#include "core/operations/sub.h"

// arm-thumb
static void rsb_imm_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = 0;

	OP_DECOMPILE("RSB<IT> <Rd>,<Rn>,#0", rd, rn);
	return rsb_imm(rd, rn, imm32, setflags);
}

// arm-thumb
static void sbc_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	bool setflags = !in_ITblock();

	OP_DECOMPILE("SBC<IT> <Rdn>,<Rm>", rdn, rm);
	return sbc_reg(rdn, rdn, rm, setflags, SRType_LSL, 0);
}

// arm-thumb
static void sub_imm_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm3 = (inst >> 6) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm3;

	OP_DECOMPILE("SUB<IT> <Rd>,<Rn>,#<imm3>", rd, rn, imm32);
	return sub_imm(rd, rn, imm32, setflags);
}

// arm-thumb
static void sub_imm_t2(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rdn = (inst >> 8) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm8;

	OP_DECOMPILE("SUB<IT> <Rdn>,#<imm8>", rdn, imm32);
	return sub_imm(rdn, rdn, imm32, setflags);
}

// arm-thumb
static void sub_reg_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool setflags = !in_ITblock();

	OP_DECOMPILE("SUB<IT> <Rd>,<Rn><Rm>", rd, rn, rm);
	return sub_reg(rd, rn, rm, SRType_LSL, 0, setflags);
}

// arm-thumb
static void sub_sp_imm_t1(uint16_t inst) {
	uint8_t imm7 = inst & 0x7f;

	uint8_t rd = 13;
	bool setflags = false;
	uint32_t imm32 = imm7 << 2;

	OP_DECOMPILE("SUB<c> SP,SP,#<imm7>", imm32);
	return sub_sp_imm(rd, imm32, setflags);
}

// arm-thumb
static void sub_sp_reg_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool    S    = (inst >> 20) & 0x1;

	bool setflags = S==1;
	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if ((rd == 13) && ((shift_t != SRType_LSL) || (shift_n > 3)))
		CORE_ERR_unpredictable("sub_sp_reg_t1 case 1\n");
	if ((rd == 15) || BadReg(rm))
		CORE_ERR_unpredictable("sub_sp_reg_t1 case 2\n");

	OP_DECOMPILE("SUB{S}<c> <Rd>,SP,<Rm>{,<shift>}",
			setflags, rd, rm, shift_t, shift_n);
	return sub_sp_reg(rd, rm, setflags, shift_t, shift_n);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_sub(void) {
	// (pre-UAL name: neg)
	// rsb_imm_t1: 0100 0010 01<x's>
	register_opcode_mask_16(0x4240, 0xbd80, rsb_imm_t1);

	// sbc_reg_t1: 0100 0001 10<x's>
	register_opcode_mask_16(0x4180, 0xbe40, sbc_reg_t1);

	// sub_imm_t1: 0001 111<x's>
	register_opcode_mask_16(0x1e00, 0xe000, sub_imm_t1);

	// sub_imm_t2: 0011 1<x's>
	register_opcode_mask_16(0x3800, 0xc000, sub_imm_t2);

	// sub_reg_t1: 0001 101<x's>
	register_opcode_mask_16(0x1a00, 0xe400, sub_reg_t1);

	// sub_sp_imm_t1: 1011 0000 1<x's>
	register_opcode_mask_16(0xb080, 0x4f00, sub_sp_imm_t1);

	// sub_sp_reg_t1: 1110 1011 101x 1101 0<x's>
	register_opcode_mask_32(0xebad0000, 0x14428000, sub_sp_reg_t1);
}
