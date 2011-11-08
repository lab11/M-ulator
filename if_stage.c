#define STAGE IF

#include "if_stage.h"
#include "pipeline.h"
#include "simulator.h"

#include "cpu/cpu.h"
#include "cpu/core.h"

void tick_if(void) {
	static uint32_t last_pc;
	uint32_t inst = 0;

	uint32_t pc = pre_if_PC;

	DBG2("start\n");

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
				break;
			}
			default:
				// 16-bit thumb inst
				SW(&last_pc, pc);
				pc = pc + 2;
		}

		// A5.1.2 p153
		// use of 0b1111 as a register specifier
		// reading PC must *always* return inst addr + 4
		SW(&if_id_PC, pre_if_PC + 4);
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
