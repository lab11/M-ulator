#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"

/* CMP and TST always write cpsr bits, regardless of itstate */

int cmp1(uint32_t inst) {
	uint8_t rn = (inst & 0x700) >> 8;
	uint32_t immed_8 = inst & 0xff;

	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t alu_out = rn_val - immed_8;

	uint32_t cpsr = CORE_cpsr_read();

	cpsr = GEN_NZCV(!!(alu_out & xPSR_N), alu_out == 0,
			!(alu_out > rn_val), OVER_SUB(alu_out, rn_val, immed_8));
	CORE_cpsr_write(cpsr);

	DBG2("cmp r%02d, #%d\t; 0x%x\n", rn, immed_8, immed_8);

	return SUCCESS;
}

int cmp2(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rn = (inst & 0x7) >> 0;

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t alu_out = rn_val - rm_val;

	uint32_t cpsr = CORE_cpsr_read();

	cpsr = GEN_NZCV(!!(alu_out & xPSR_N), alu_out == 0,
			!(alu_out > rn_val), OVER_SUB(alu_out, rn_val, rm_val));
	CORE_cpsr_write(cpsr);

	DBG2("cmp r%02d, r%02d\n", rn, rm);

	return SUCCESS;
}

void register_opcodes_cmp(void) {
	// cmp1: 0010 1<x's>
	register_opcode_mask(0x2800, 0xffffd000, cmp1);

	// cmp2: 0100 0010 10<x's>
	register_opcode_mask(0x4280, 0xffffbd40, cmp2);
}
