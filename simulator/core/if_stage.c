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

#define STAGE IF

#include "if_stage.h"
#include "pipeline.h"
#include "opcodes.h"
#include "state_sync.h"

#include "cpu/core.h"
#include "cpu/registers.h"
#include "cpu/core.h"

// Branch Target Forwarding
// Currently, no speculative branching,
// but we can make unconditional branches quick
static void branch_target_forward32(uint32_t target, uint32_t inst, uint32_t *pc) {
#ifdef NO_PIPELINE
	(void) target;
	(void) inst;
	(void) pc;
	return;
#else
	if (match_mask32(inst, 0xf0009000, 0x08004000)) {
		DBG2("btf: b_t4\n");
		// This is b_t4
		uint16_t imm11 = inst & 0x7ff;
		bool J2 = !!(inst & 0x800);
		bool J1 = !!(inst & 0x2000);
		uint16_t imm10 = (inst >> 16) & 0x3ff;
		bool S = !!(inst & 0x04000000);

		bool I1 = !(J1 ^ S);
		bool I2 = !(J2 ^ S);
		uint32_t imm32 = (I1 << 23) | (I2 << 22) | (imm10 << 12) | (imm11 << 1);
		if (S)
			imm32 |= 0xff000000;

		*pc = target + imm32;
		return;
	}

	if (match_mask32(inst, 0xf000d000, 0x08000000)) {
		DBG2("btf: bl_t1\n");
		// This is a bl_t1
		uint16_t imm11 = inst & 0x7ff;
		bool J2 = !!(inst & 0x800);
		bool J1 = !!(inst & 0x2000);
		uint16_t imm10 = (inst >> 16) & 0x3ff;
		bool S = !!(inst & 0x0400000);

		bool I1 = !(J1 ^ S);
		bool I2 = !(J2 ^ S);
		uint32_t imm32 = (I1 << 23) | (I2 << 22) | (imm10 << 12) | (imm11 << 1);

		if (S)
			imm32 |= 0xff000000;

		*pc = target + imm32;
		return;
	}
#endif
}

static void branch_target_forward16(uint32_t target, uint32_t inst, uint32_t *pc) {
#ifdef NOPIPELINE
	WARN("FIXME: This shouldn't be here in NOPIPELINE\n");
#endif
	if (match_mask32(inst, 0xe000, 0x1000)) {
		DBG2("btf: b_t2\n");
		// This is b_t2
		uint16_t imm11 = inst & 0x7ff;
		uint32_t imm32 = imm11 << 1;
		if (imm32 & 0x800)
			imm32 |= 0xfffff000;

		*pc = target + imm32;
		return;
	}

	if (match_mask32(inst, 0x4700, 0xb880)) {
		DBG2("btf: bx_t1\n");
		uint8_t rm = (inst >> 3) & 0xf;

		*pc = CORE_reg_read(rm) & 0xfffffffe;
		return;
	}
}

static void tick_if(void) {
	static uint32_t last_pc;
	uint32_t inst = 0;

	uint32_t pc = SR(&pre_if_PC);

	DBG2("start\n");

#ifndef NO_PIPELINE
	// Poor man's pipeline hazard
	if (pc > 0xf0000000) {
		CORE_WARN("Build a proper pipeline exception mechanism\n");
		SW(&if_id_PC, HAZARD_PC);
		SW(&if_id_inst, INST_HAZARD);
		SW(&pre_if_PC, HAZARD_PC);

		DBG2("end from pipeline hazard\n");
		return;
	}
#endif

	// Instruction Fetch
	union epsr_t epsr = CORE_epsr_read();
	if (epsr.bits.T) {
		DBG2("Reading thumb mode instruction\n");
		inst = read_halfword(pc);

		// If inst[15:11] are any of
		// 11101, 11110, or 11111 then this is
		// a 32-bit thumb inst
		switch (inst & 0xf800) {
			case 0xe800:
			case 0xf000:
			case 0xf800:
			{
				SW(&last_pc, pc);
				pc = pc + 2;

				inst <<= 16;
				inst |= read_halfword(pc);
				pc = pc + 2;

				branch_target_forward32(SR(&pre_if_PC) + 4, inst, &pc);

				break;
			}
			default:
				// 16-bit thumb inst
				SW(&last_pc, pc);
				pc = pc + 2;

				branch_target_forward16(SR(&pre_if_PC) + 4, inst, &pc);
		}

		// A5.1.2 p153
		// use of 0b1111 as a register specifier
		// reading PC must *always* return inst addr + 4
		SW(&if_id_PC, SR(&pre_if_PC) + 4);
		SW(&if_id_inst, inst);
		SW(&pre_if_PC, pc);
	} else {
#ifdef M_PROFILE
		WARN("if_id_PC: %08x\n", if_id_PC);
		CORE_ERR_not_implemented("This CPU conforms to the ARM-7M profile, which requires the T bit to always be set\n");
#endif
		CORE_ERR_not_implemented("IF stage for non M-Profile\n");
		DBG2("Reading arm mode instruction\n");
	}

	DBG2("end\n");
}

static int if_pipeline_flush(void* new_pc_void) {
	uint32_t new_pc = *((uint32_t *) new_pc_void);
#ifdef NO_PIPELINE
	SW(&pre_if_PC, new_pc);
	SW(&if_id_PC, new_pc);
#else
	SW(&pre_if_PC, new_pc);
	SW(&if_id_PC, STALL_PC);
	SW(&if_id_inst, INST_NOP);
#endif
	return 0;
}

__attribute__ ((constructor))
void register_if_stage(void) {
	register_pipeline_stage(0, "IF Stage", tick_if, if_pipeline_flush);
}
