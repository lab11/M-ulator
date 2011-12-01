#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"

/* CMP and TST always write cpsr bits, regardless of itstate */

void cmp1(uint32_t inst) {
	uint8_t rn = (inst & 0x700) >> 8;
	uint32_t immed_8 = inst & 0xff;

	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t alu_out = rn_val - immed_8;

	uint32_t cpsr = CORE_cpsr_read();

	cpsr = GEN_NZCV(!!(alu_out & xPSR_N), alu_out == 0,
			!(alu_out > rn_val), OVER_SUB(alu_out, rn_val, immed_8));
	CORE_cpsr_write(cpsr);

	DBG2("cmp r%02d, #%d\t; 0x%x\n", rn, immed_8, immed_8);
}

void cmp2(uint32_t inst) {
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
}

void cmp(uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result;
	bool carry_out;
	bool overflow_out;
	AddWithCarry(rn_val, ~imm32, 1, &result, &carry_out, &overflow_out);

	uint32_t cpsr = CORE_cpsr_read();

	cpsr = GEN_NZCV(
			!!(result & xPSR_N),
			result == 0,
			carry_out,
			overflow_out
		       );
	CORE_cpsr_write(cpsr);
}

void cmp_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool i = (inst >> 26) & 0x1;

	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | imm8);

	cmp(rn, imm32);
}

void register_opcodes_cmp(void) {
	// cmp1: 0010 1<x's>
	register_opcode_mask(0x2800, 0xffffd000, cmp1);

	// cmp2: 0100 0010 10<x's>
	register_opcode_mask(0x4280, 0xffffbd40, cmp2);

	// 1111 0x01 1011 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask(0xf1b00f00, 0x0a408000, cmp_t2);
}
