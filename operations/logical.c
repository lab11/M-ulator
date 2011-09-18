#include "../cortex_m3.h"
#include "cpsr.h"

int and(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t result;
	result = CORE_reg_read(rd) & CORE_reg_read(rm);
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!IN_IT_BLOCK) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				!!(cpsr & xPSR_C), !!(cpsr & xPSR_V));
		CORE_cpsr_write(cpsr);
	}

	DBG2("ands r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}

int and_imm(uint32_t cpsr, uint8_t setflags, uint8_t rd, uint8_t rn, uint32_t imm32, uint8_t carry) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result = rn_val & imm32;
	if (rd == 15) {
		// ALUWritePC(result);
		CORE_ERR_not_implemented("ALUWritePC and_imm\n");
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			cpsr = GEN_NZCV(
					!!(result & xPSR_N),
					result == 0,
					carry,
					!!(cpsr & xPSR_V)
				       );
			CORE_cpsr_write(cpsr);
		}
	}

	DBG2("and_imm done\n");

	return SUCCESS;
}

int and_imm_t1(uint32_t inst) {
	uint32_t cpsr = CORE_cpsr_read();

	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);
	uint8_t i = !!(inst & 0x04000000);

	if ((rd == 0xf) && (S == 1))
		CORE_ERR_not_implemented("add_imm_t1 --> TST (imm)\n");

	//d = uint(rd); n = uint(rd);
	uint8_t setflags = (S == 1);

	uint32_t imm32;
	uint8_t carry;
	uint16_t imm12 = (i << 11) | (imm3 << 8) | imm8;
	ThumbExpandImm_C(imm12, !!(cpsr & xPSR_C), &imm32, &carry);

	DBG2("rd: %d, rn %d, imm12 0x%03x (%d)\n", rd, rn, imm12, imm12);

	if ( ((rd == 13) || ((rd == 15) && (S == 0))) || BadReg(rn) )
		CORE_ERR_unpredictable("Bad reg combo's in add_imm_t1\n");

	return and_imm(cpsr, setflags, rd, rn, imm32, carry);
}

int bic(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t result;
	result = CORE_reg_read(rd) & ~CORE_reg_read(rm);
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!IN_IT_BLOCK) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				!!(cpsr & xPSR_C), !!(cpsr & xPSR_V));
		CORE_cpsr_write(cpsr);
	}

	DBG2("bics r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}

int orr(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t result;
	result = CORE_reg_read(rd) | CORE_reg_read(rm);
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!IN_IT_BLOCK) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				!!(cpsr & xPSR_C), !!(cpsr & xPSR_V));
		CORE_cpsr_write(cpsr);
	}

	DBG2("orrs r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}

int neg(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t result;
	uint32_t rm_val = CORE_reg_read(rm);
	result = 0 - rm_val;
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!IN_IT_BLOCK) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				!(result > 0), OVER_SUB(result, 0, rm_val));
		CORE_cpsr_write(cpsr);
	}

	DBG2("negs r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}

void register_opcodes_logical(void) {
	// and: 0100 0000 00<x's>
	register_opcode_mask(0x4000, 0xffffbfc0, and);

	// and_imm_t1: 1111 0x00 000x xxxx 0<x's>
	register_opcode_mask(0xf0000000, 0x0be08000, and_imm_t1);

	// bic: 0100 0011 10<x's>
	register_opcode_mask(0x4380, 0xffffbc40, bic);

	// orr: 0100 0011 00<x's>
	register_opcode_mask(0x4300, 0xffffbcc0, orr);

	// neg: 0100 0010 01<x's>
	register_opcode_mask(0x4240, 0xffffbd80, neg);
}
