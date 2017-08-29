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

// arm-thumb
static void mov_imm_t1(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm8;

	union apsr_t apsr = CORE_apsr_read();
	bool carry = apsr.bits.C;

	OP_DECOMPILE("MOV<IT> <Rd>,#<imm8>", rd, imm8);
	return mov_imm(apsr, setflags, imm32, rd, carry);
}

/* If <rd> and <rm> both in R0-R7 then all thumb */
// arm-v6-m, arm-v7-m, arm-thumb*
static void mov_reg_t1(uint16_t inst) {
	uint8_t rd =  (inst & 0x7);
	uint8_t rm =  (inst & 0x78) >> 3;
	uint8_t D = !!(inst & 0x80);

	// d = UInt(D:rd)
	rd |= (D << 3);

	// m = UInt(rm); unecessary

	uint8_t setflags = false;

	if ((rd == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("mov_reg_t1 unpredictable\n");

	OP_DECOMPILE("MOV<c> <Rd>,<Rm>", rd, rm);
	return mov_reg(rd, rm, setflags);
}

// arm-thumb
static void mov_reg_t2(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	bool setflags = true;

	if (in_ITblock())
		CORE_ERR_unpredictable("illegal in it block\n");

	OP_DECOMPILE("MOVS <Rd>,<Rm>", rd, rm);
	return mov_reg(rd, rm, setflags);
}

__attribute__ ((constructor))
static void register_opcodes_arm_thumb_mov(void) {
	// mov1: 0010 0xxx <x's>
	register_opcode_mask_16(0x2000, 0xd800, mov_imm_t1);

	// This is a weird corner case
	// mov_reg_t1: 0100 0110 xx<x's> <-- arm-v6-m, arm-v7-m
	// mov_reg_t1: 0100 0110 00<x's> <-- arm-thumb
	register_opcode_mask_16(0x4600, 0xb9c0, mov_reg_t1);
    
    // This is a wierd-er corner case
    // GCC uses 'mov r8,r8' (0x46c0) as 'nop'
    register_opcode_mask_16(0x46c0, ~0x46c0, mov_reg_t1);

	// mov_reg_t2: 0000 0000 00xx xxxx
	register_opcode_mask_16(0x0, 0xffc0, mov_reg_t2);
}
