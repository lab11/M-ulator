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
#include "cpu/core.h"

static void SelectInstrSet(uint8_t iset) {
	switch (iset) {
		case INST_SET_ARM:
			DBG2("Set ISETSTATE to Arm\n");
			if (GET_ISETSTATE == INST_SET_THUMBEE) {
				CORE_ERR_unpredictable("ThumbEE -> Arm trans\n");
			} else {
				SET_ISETSTATE(INST_SET_ARM);
			}
			break;
		case INST_SET_THUMB:
			DBG2("Set ISETSTATE to Thumb\n");
			SET_ISETSTATE(INST_SET_THUMB);
			break;
		case INST_SET_JAZELLE:
			DBG2("Set ISETSTATE to Jazelle\n");
			SET_ISETSTATE(INST_SET_JAZELLE);
			break;
		case INST_SET_THUMBEE:
			DBG2("Set ISETSTATE to ThumbEE\n");
			SET_ISETSTATE(INST_SET_THUMBEE);
			break;
		default:
			CORE_ERR_unpredictable("Unknown iset\n");
			break;
	}
}

static void b(uint8_t cond, uint32_t imm32) {
	if (eval_cond(CORE_apsr_read(), cond)) {
		uint32_t pc = CORE_reg_read(PC_REG);
		BranchWritePC(pc + imm32);
		DBG2("b taken old pc %08x new pc %08x (imm32: %08x)\n",
				pc, CORE_reg_read(PC_REG), imm32);
	} else {
		DBG2("b <not taken>\n");
	}
}

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

	return b(cond, imm32);
}

// arm-thumb
static void b_t2(uint16_t inst) {
	uint16_t imm11 = (inst & 0x7ff);

	uint32_t imm32 = SignExtend(imm11 << 1, 12);

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("b_t2 in it block\n");

	return b(0xf, imm32);
}

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

	return b(0xf, imm32);
}

static void bl_blx(uint32_t pc, uint8_t targetInstrSet, uint32_t imm32) {
	uint32_t lr;
	DBG2("pc %08x targetInstrSet %x imm32 %d 0x%08x\n",
			pc, targetInstrSet, imm32, imm32);

	if (GET_ISETSTATE == INST_SET_ARM) {
		lr = pc - 4;
	} else {
		lr = ((pc & 0xfffffffe) | 0x1);
	}
	CORE_reg_write(LR_REG, lr);

	uint32_t targetAddress;
	if (targetInstrSet == INST_SET_ARM) {
		targetAddress = (pc & 0xfffffffc) + imm32;
	} else {
		targetAddress = pc + imm32;
	}

	SelectInstrSet(targetInstrSet);
	BranchWritePC(targetAddress);
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

	return bl_blx(CORE_reg_read(PC_REG), GET_ISETSTATE, imm32);
}

static inline void blx_reg(uint8_t rm) {
	uint32_t target = CORE_reg_read(rm);
	uint32_t next_instr_addr = CORE_reg_read(PC_REG) - 2;
	CORE_reg_write(LR_REG, next_instr_addr | 0x1);
	BLXWritePC(target);
}

// arm-v5-t*, arm-v6-m, arm-v7-m
static void blx_reg_t1(uint16_t inst) {
	uint8_t rm = (inst >> 3) & 0xf;

	if ((rm == 15) || (in_ITblock() && !last_in_ITblock()))
		CORE_ERR_unpredictable("blx_reg_t1 case\n");

	return blx_reg(rm);
}

static inline void bx(uint8_t rm) {
	BXWritePC(CORE_reg_read(rm));
}

// arm-thumb
static void bx_t1(uint16_t inst) {
	uint8_t rm = (inst >> 3) & 0xf;

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("bx_t1 in it block\n");

	return bx(rm);
}

static void tbb(uint8_t rn, uint8_t rm, bool is_tbh) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t halfwords;
	if (is_tbh)
		halfwords = read_halfword(rn_val + (rm_val<<1));
	else
		halfwords = read_byte(rn_val + rm_val);

	BranchWritePC(CORE_reg_read(PC_REG) + 2*halfwords);
}

static void tbb_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	bool H = !!(inst & 0x10);
	uint8_t rn = (inst >> 16) & 0xf;

	bool is_tbh = H;

	if ((rn == 13) || (rm == 13) || (rm == 15))
		CORE_ERR_unpredictable("bad regs\n");

	if (in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("branch in IT\n");

	return tbb(rn, rm, is_tbh);
}

__attribute__ ((constructor))
void register_opcodes_branch(void) {
	// b_t1: 1101 <x's>
	//   ex: xxxx 1111 <x's>
	register_opcode_mask_16_ex(0xd000, 0x2000, b_t1,
			0x0e00, 0x100,
			0x0f00, 0x0,
			0, 0);

	// b_t2: 1110 0<x's>
	register_opcode_mask_16(0xe000, 0x1800, b_t2);

	// b_t3: 1111 0xxx xxxx xxxx 10x0 <x's>
	//   ex:        11 1
	register_opcode_mask_32_ex(0xf0008000, 0x08005000, b_t3,
			0x03800000, 0x0,
			0, 0);

	// b_t4: 1111 0xxx xxxx xxxx 10x1 xxxx xxxx xxxx
	register_opcode_mask_32(0xf0009000, 0x08004000, b_t4);

	// bl_t1: 1111 0xxx xxxx xxxx 11x1 <x's>
	register_opcode_mask_32(0xf000d000, 0x08000000, bl_t1);

	// blx_reg_t1: 0100 0111 1xxx x000
	register_opcode_mask_16(0x4780, 0xb807, blx_reg_t1);

	// bx_t1: 0100 0111 0xxx x000
	register_opcode_mask_16(0x4700, 0xb887, bx_t1);

	// tbb_t1: 1110 1000 1101 xxxx 1111 0000 000x xxxx
	register_opcode_mask_32(0xe8d0f000, 0x17200fe0, tbb_t1);
}
