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

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/misc.h"
#include "cpu/core.h"

static void SelectInstrSet(uint8_t iset) {
	switch (iset) {
		case INST_SET_ARM:
			DBG2("Set ISETSTATE to Arm\n");
			if (get_isetstate() == INST_SET_THUMBEE) {
				CORE_ERR_unpredictable("ThumbEE -> Arm trans\n");
			} else {
				set_isetstate(INST_SET_ARM);
			}
			break;
		case INST_SET_THUMB:
			DBG2("Set ISETSTATE to Thumb\n");
			set_isetstate(INST_SET_THUMB);
			break;
		case INST_SET_JAZELLE:
			DBG2("Set ISETSTATE to Jazelle\n");
			set_isetstate(INST_SET_JAZELLE);
			break;
		case INST_SET_THUMBEE:
			DBG2("Set ISETSTATE to ThumbEE\n");
			set_isetstate(INST_SET_THUMBEE);
			break;
		default:
			CORE_ERR_unpredictable("Unknown iset\n");
	}
}

void b(uint8_t cond, uint32_t imm32) {
	if (eval_cond(CORE_apsr_read(), cond)) {
		uint32_t pc = CORE_reg_read(PC_REG);
		BranchWritePC(pc + imm32);
		DBG2("b taken old pc %08x new pc %08x (imm32: %08x)\n",
				pc, CORE_reg_read(PC_REG), imm32);
	} else {
		DBG2("b <not taken>\n");
	}
}


void bl_blx(uint32_t pc, uint8_t targetInstrSet, uint32_t imm32) {
	uint32_t lr;
	DBG2("pc %08x targetInstrSet %x imm32 %d 0x%08x\n",
			pc, targetInstrSet, imm32, imm32);

	if (get_isetstate() == INST_SET_ARM) {
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

void blx_reg(uint8_t rm) {
	uint32_t target = CORE_reg_read(rm);
	uint32_t next_instr_addr = CORE_reg_read(PC_REG) - 2;
	CORE_reg_write(LR_REG, next_instr_addr | 0x1);
	BLXWritePC(target);
}

void bx(uint8_t rm) {
	BXWritePC(CORE_reg_read(rm));
}

void tbb(uint8_t rn, uint8_t rm, bool is_tbh) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t halfwords;
	if (is_tbh)
		halfwords = read_halfword(rn_val + (rm_val<<1));
	else
		halfwords = read_byte(rn_val + rm_val);

	BranchWritePC(CORE_reg_read(PC_REG) + 2*halfwords);
}

