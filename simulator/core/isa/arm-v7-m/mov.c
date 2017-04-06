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

#include "core/operations/mov.h"

// arm-v7-m
static void mov_imm_t2(uint32_t inst) {
	union apsr_t apsr = CORE_apsr_read();

	int   imm8 =  (inst & 0x000000ff);
	uint8_t Rd =  (inst & 0x00000f00) >> 8;
	int   imm3 =  (inst & 0x00007000) >> 12;
	uint8_t S = !!(inst & 0x00100000);
	uint8_t i = !!(inst & 0x04000000);

	uint8_t setflags = (S == 0x1);

	// (imm32, carry) = ThumbExpandImm_C(i:imm3:imm8, APSR.C);
	uint32_t arg;
	arg = imm8;
	arg |= (imm3 << 8);
	arg |= (i << 11);
	uint32_t imm32;
	bool carry;
	ThumbExpandImm_C(arg, apsr.bits.C, &imm32, &carry);

	if ((Rd == 13) || (Rd == 15)) {
		CORE_ERR_unpredictable("mov to SP or PC\n");
	}

	OP_DECOMPILE("MOV{S}<c>.W <Rd>,#<const>", setflags, Rd, imm32);
	return mov_imm(apsr, setflags, imm32, Rd, carry);
}

// arm-v7-m
static void mov_imm_t3(uint32_t inst) {
	union apsr_t apsr = CORE_apsr_read();

	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t imm4 = (inst & 0xf0000) >> 16;
	uint8_t i = !!(inst & 0x04000000);

	// d = uint(rd);
	uint8_t setflags = false;

	uint32_t imm32 = (imm4 << 12) | (i << 11) | (imm3 << 8) | imm8;

	if (BadReg(rd))
		CORE_ERR_unpredictable("BadReg in mov_imm_t3\n");

	OP_DECOMPILE("MOVW<c> <Rd>,#<imm16>", rd, imm32);
	// carry set to 0 irrelevant since setflags is false
	return mov_imm(apsr, setflags, imm32, rd, 0);
}

// arm-v7-m
static void mov_reg_t3(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	bool    S  = (inst >> 16) & 0x1;

	bool setflags = S==1;

	if (setflags && ((rd > 13) || (rm > 13)))
		CORE_ERR_unpredictable("mov_reg_t3 case 1\n");
	if (!setflags && ((rd == 15) || (rm == 15) || ((rd == 13) && (rm == 13))))
		CORE_ERR_unpredictable("mov_reg_t3 case 2\n");

	OP_DECOMPILE("MOV{S}<c>.W <Rd>,<Rm>", setflags, rd, rm);
	return mov_reg(rd, rm, setflags);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_mov(void) {
	// mov_imm_t2: 1111 0x00 010x 1111 0<x's>
	register_opcode_mask_32(0xf04f0000, 0x0ba08000, mov_imm_t2);

	// mov_imm_t3: 1111 0x10 0100 xxxx 0<x's>
	register_opcode_mask_32(0xf2400000, 0x09b08000, mov_imm_t3);

	// mov_reg_t3: 1110 1010 010x 1111 0000 xxxx 0000 xxxx
	register_opcode_mask_32(0xea4f0000, 0x15a0f0f0, mov_reg_t3);
}
