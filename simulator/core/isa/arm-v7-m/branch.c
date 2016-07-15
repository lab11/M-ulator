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

// arm-v7-m
static void b_t3(uint32_t inst) {
	uint16_t imm11 = (inst & 0x7ff);
	bool J2 = !!(inst & 0x800);
	bool J1 = !!(inst & 0x2000);
	uint8_t imm6 = (inst >> 16) & 0x3f;
	uint8_t cond = (inst >> 22) & 0xf;
	bool S = !!(inst & 0x04000000);

	/* THIS FUCKING INSTRUCTION DECODE IS MISSING FROM THE M SERIES ARM ARM,
	   Turns out it is __NOT__ the same as the t4 encoding, as the absence
	   of documentation may lead a naiive reader to believe. From the A
	   seris ARM ARM we learn:

	   if cond<3:1> == ‘111’ then SEE “Related encodings”;
	   imm32 = SignExtend(S:J2:J1:imm6:imm11:’0’, 32);
	   if InITBlock() then UNPREDICTABLE;
	*/

	uint32_t imm32 = (J2 << 19) | (J1 << 18) | (imm6 << 12) | (imm11 << 1);
	if (S) {
		imm32 |= 0xfff00000;
	}

	if (in_ITblock())
		CORE_ERR_unpredictable("b_t3 not allowed in IT block\n");

	OP_DECOMPILE("B<c>.W <label>", cond, imm32);
	return b(cond, imm32);
}

// arm-v7-m
static void b_t4(uint32_t inst) {
	uint16_t imm11 = inst & 0x7ff;
	bool J2 = !!(inst & 0x800);
	bool J1 = !!(inst & 0x2000);
	uint16_t imm10 = (inst >> 16) & 0x3ff;
	bool S = !!(inst & 0x04000000);

	bool I1 = !(J1 ^ S);
	bool I2 = !(J2 ^ S);
	uint32_t imm32 = (I1 << 23) | (I2 << 22) | (imm10 << 12) | (imm11 << 1);
	if (S) {
		imm32 |= 0xff000000;
	}

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("bad it time\n");

	OP_DECOMPILE("B<c>.W <label>", 0xf, imm32);
	return b(0xf, imm32);
}

// arm-v7-m
static void tbb_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	bool H = !!(inst & 0x10);
	uint8_t rn = (inst >> 16) & 0xf;

	bool is_tbh = H;

	if ((rn == 13) || (rm == 13) || (rm == 15))
		CORE_ERR_unpredictable("bad regs\n");

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("branch in IT\n");

	if (is_tbh)
		OP_DECOMPILE("TBH<c> [<Rn>,<Rm>,LSL #1]", rn, rm);
	else
		OP_DECOMPILE("TBB<c> [<Rn>,<Rm>]", rn, rm);
	return tbb(rn, rm, is_tbh);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_branch(void) {
	// b_t3: 1111 0xxx xxxx xxxx 10x0 <x's>
	//   ex:        11 1
	register_opcode_mask_32_ex(0xf0008000, 0x08005000, b_t3,
			0x03800000, 0x0,
			0, 0);

	// b_t4: 1111 0xxx xxxx xxxx 10x1 xxxx xxxx xxxx
	register_opcode_mask_32(0xf0009000, 0x08004000, b_t4);

	// tbb_t1: 1110 1000 1101 xxxx 1111 0000 000x xxxx
	register_opcode_mask_32(0xe8d0f000, 0x17200fe0, tbb_t1);
}
