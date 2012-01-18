#define STAGE IF

#include "if_stage.h"
#include "id_stage.h"
#include "pipeline.h"
#include "simulator.h"

#include "cpu/cpu.h"
#include "cpu/core.h"

// Branch Target Forwarding
// Currently, no speculative branching,
// but we can make unconditional branches quick
static void branch_target_forward32(uint32_t target, uint32_t inst, uint32_t *pc) {
	if (match_mask(inst, 0xf0009000, 0x08004000)) {
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

	if (match_mask(inst, 0xf000d000, 0x08000000)) {
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
}

static void branch_target_forward16(uint32_t target, uint32_t inst, uint32_t *pc) {
	if (match_mask(inst, 0xe000, 0x1000)) {
		DBG2("btf: b_t2\n");
		// This is b_t2
		uint16_t imm11 = inst & 0x7ff;
		uint32_t imm32 = imm11 << 1;
		if (imm32 & 0x800)
			imm32 |= 0xfffff000;

		*pc = target + imm32;
		return;
	}

	if (match_mask(inst, 0x4700, 0xb880)) {
		DBG2("btf: bx_t1\n");
		uint8_t rm = (inst >> 3) & 0xf;

		*pc = CORE_reg_read(rm) & 0xfffffffe;
		return;
	}
}

void tick_if(void) {
	static uint32_t last_pc;
	uint32_t inst = 0;

	uint32_t pc = SR(&pre_if_PC);

	DBG2("start\n");

	// Poor man's pipeline hazard
	if (pc > 0xf0000000) {
		CORE_WARN("Build a proper pipeline exception mechanism\n");
		SW(&if_id_PC, HAZARD_PC);
		SW(&if_id_inst, INST_HAZARD);
		SW(&pre_if_PC, HAZARD_PC);

		DBG2("end from pipeline hazard\n");
		return;
	}

	// Instruction Fetch
	if (T_BIT) {

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
				// 32-bit thumb inst
				DBG2("inst %04x is 32-bit\n", inst);

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
		CORE_ERR_not_implemented("This CPU conforms to the ARM-7M profile, which requires the T bit to always be set\n");
#endif
		CORE_ERR_not_implemented("IF stage for non M-Profile\n");
		DBG2("Reading arm mode instruction\n");
	}

	DBG2("end\n");
}
