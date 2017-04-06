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

#include "core/operations/branch.h"

// arm-thumb
static void b_t1(uint16_t inst) {
	uint8_t imm8 = (inst & 0xff);
	uint8_t cond = (inst & 0xf00) >> 8;

	if (cond == 0xe)
		CORE_ERR_unpredictable("b_t1 UNDEFINED (cond 0xe)\n");

	uint32_t imm32 = SignExtend(imm8 << 1, 9);

	DBG2("PC: %08x, imm32: %08x\n",
			CORE_reg_read(PC_REG), imm32);

	if (in_ITblock())
		CORE_ERR_unpredictable("b_t1 UNPREDICTABLE (b in IT block)\n");

	OP_DECOMPILE("B<c> <label>", cond, imm32);
	return b(cond, imm32);
}

// arm-thumb
static void b_t2(uint16_t inst) {
	uint16_t imm11 = (inst & 0x7ff);

	uint32_t imm32 = SignExtend(imm11 << 1, 12);

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("b_t2 in it block\n");

	OP_DECOMPILE("B<c> <label>", 0xf, imm32);
	return b(0xf, imm32);
}

// arm-thumb
static void bl_t1(uint32_t inst) {
	// top 5 bits fixed
	uint8_t  S = !!(inst & 0x04000000);
	int imm10 =    (inst & 0x03ff0000) >> 16;
	assert(1 == (!!(inst & 0x00008000)));
	assert(1 == (!!(inst & 0x00004000)));
	uint8_t J1 = !!(inst & 0x00002000);
	assert(1 == (!!(inst & 0x00001000)));
	uint8_t J2 = !!(inst & 0x00000800);
	int imm11 =    (inst & 0x000007ff);

	uint8_t I1 = !(J1 ^ S);
	uint8_t I2 = !(J2 ^ S);

	struct {signed int x:25;} s;
	s.x = 0;		// 1
	s.x |= (imm11 << 1);	// 12
	s.x |= (imm10 << 12);	// 22
	s.x |= (I2 << 22);	// 23
	s.x |= (I1 << 23);	// 24
	s.x |= (S << 24);	// 25
	int32_t imm32 = s.x;

	DBG2("Sign bit: %d, imm32: %d 0x%08x\n", S, imm32, imm32);

	// targetInstrSet = CurrentInstrSet

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("bl_t1 in itstate, not ending\n");

	OP_DECOMPILE("BL<c> <label>", imm32);
	return bl_blx(CORE_reg_read(PC_REG), get_isetstate(), imm32);
}

// XXX ISA?
// arm-v5-t*, arm-v6-m, arm-v7-m
static void blx_reg_t1(uint16_t inst) {
	uint8_t rm = (inst >> 3) & 0xf;

	if ((rm == 15) || (in_ITblock() && !last_in_ITblock()))
		CORE_ERR_unpredictable("blx_reg_t1 case\n");

	OP_DECOMPILE("BLX<c> <Rm>", rm);
	return blx_reg(rm);
}

// arm-thumb
static void bx_t1(uint16_t inst) {
	uint8_t rm = (inst >> 3) & 0xf;

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("bx_t1 in it block\n");

	OP_DECOMPILE("BX<c> <Rm>", rm);
	return bx(rm);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_branch(void) {
	// b_t1: 1101 <x's>
	//   ex: xxxx 1111 <x's>
	register_opcode_mask_16_ex(0xd000, 0x2000, b_t1,
			0x0e00, 0x100,
			0x0f00, 0x0,
			0, 0);

	// b_t2: 1110 0<x's>
	register_opcode_mask_16(0xe000, 0x1800, b_t2);

	// bl_t1: 1111 0xxx xxxx xxxx 11x1 <x's>
	register_opcode_mask_32(0xf000d000, 0x08000000, bl_t1);

	// blx_reg_t1: 0100 0111 1xxx x000
	register_opcode_mask_16(0x4780, 0xb807, blx_reg_t1);

	// bx_t1: 0100 0111 0xxx x000
	register_opcode_mask_16(0x4700, 0xb887, bx_t1);
}
