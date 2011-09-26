#include "../cortex_m3.h"
#include "cpsr.h"

int and(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t result;
	result = CORE_reg_read(rd) & CORE_reg_read(rm);
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!in_ITblock(ITSTATE)) {
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

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				!!(cpsr & xPSR_C), !!(cpsr & xPSR_V));
		CORE_cpsr_write(cpsr);
	}

	DBG2("bics r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}

int bic_imm(uint32_t cpsr, uint8_t setflags, uint8_t rd, uint8_t rn, uint32_t imm32, uint8_t carry) {
	uint32_t result = CORE_reg_read(rn) & (~imm32);
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

	DBG2("bic_imm ran\n");

	return SUCCESS;
}

int bic_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);
	uint8_t i = !!(inst & 0x04000000);

	uint32_t cpsr = CORE_cpsr_read();

	uint16_t imm12 = (i << 11) | (imm3 << 8) | imm8;
	uint32_t imm32;
	uint8_t carry_out;
	ThumbExpandImm_C(imm12, !!(cpsr & xPSR_C), &imm32, &carry_out);

	if ((rd >= 13) || (rn >= 13))
		CORE_ERR_unpredictable("bic_imm_t1 bad reg\n");

	return bic_imm(cpsr, S, rd, rn, imm32, carry_out);
}

int eor_reg(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	uint32_t result;
	uint8_t carry_out;

	uint32_t cpsr = CORE_cpsr_read();

	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, !!(cpsr & xPSR_C), &result, &carry_out);

	result = CORE_reg_read(rn) ^ result;
	CORE_reg_write(rd, result);

	if (setflags) {
		cpsr = GEN_NZCV(
				!!(result & xPSR_N),
				result == 0,
				carry_out,
				!!(cpsr & xPSR_V)
			       );
		CORE_cpsr_write(cpsr);
	}

	return SUCCESS;
}

int eor_reg_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t type = (inst & 0x30) >> 4;
	uint8_t imm2 = (inst & 0xc0) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);

	if ((rd == 0xf) && (S))
		CORE_ERR_unpredictable("eor_reg_t2 -> TEQ\n");

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd == 13) || ((rd == 15) && (S == 0)) || (rn >= 13) || (rm >= 13)) {
		CORE_ERR_unpredictable("eor_reg_t2 bad reg\n");
	}

	return eor_reg(S, rd, rn, rm, shift_t, shift_n);
}

int lsl_reg(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm) {
	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = CORE_reg_read(rm) & 0xff;

	uint32_t result;
	uint8_t carry_out;

	uint32_t cpsr = CORE_cpsr_read();

	Shift_C(CORE_reg_read(rn), 32, shift_t, shift_n, !!(cpsr & xPSR_C), &result, &carry_out);

	CORE_reg_write(rd, result);

	if (setflags) {
		cpsr = GEN_NZCV(
				!!(result & xPSR_N),
				result == 0,
				carry_out,
				!!(cpsr & xPSR_V)
			       );
		CORE_cpsr_write(cpsr);
	}

	return SUCCESS;
}

int lsl_reg_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);

	if ((rd >= 13) || (rn >= 13) || (rm >= 13)) {
		CORE_ERR_unpredictable("lsl_reg_t2 bad reg\n");
	}

	return lsl_reg(S, rd, rn, rm);
}

int mvn_reg(uint8_t setflags, uint8_t rd, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	uint32_t result;
	uint8_t carry_out;

	uint32_t cpsr = CORE_cpsr_read();

	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, !!(cpsr & xPSR_C), &result, &carry_out);

	result = ~result;
	CORE_reg_write(rd, result);

	if (setflags) {
		cpsr = GEN_NZCV(
				!!(result & xPSR_N),
				result == 0,
				carry_out,
				!!(cpsr & xPSR_V)
			       );
		CORE_cpsr_write(cpsr);
	}

	DBG2("mvn_reg did stuff\n");

	return SUCCESS;
}

int mvn_reg_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t type = (inst & 0x30) >> 4;
	uint8_t imm2 = (inst & 0xc0) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t S = !!(inst & 0x100000);

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd >= 13) || (rm >= 13)) {
		CORE_ERR_unpredictable("mvn_reg_t2 bad reg\n");
	}

	return mvn_reg(S, rd, rm, shift_t, shift_n);
}

int orr(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t result;
	result = CORE_reg_read(rd) | CORE_reg_read(rm);
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				!!(cpsr & xPSR_C), !!(cpsr & xPSR_V));
		CORE_cpsr_write(cpsr);
	}

	DBG2("orrs r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}

int orr_reg(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	uint32_t result;
	uint8_t carry_out;

	uint32_t cpsr = CORE_cpsr_read();

	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, !!(cpsr & xPSR_C), &result, &carry_out);

	result = CORE_reg_read(rn) | result;
	CORE_reg_write(rd, result);

	if (setflags) {
		cpsr = GEN_NZCV(
				!!(result & xPSR_N),
				result == 0,
				carry_out,
				!!(cpsr & xPSR_V)
			       );
		CORE_cpsr_write(cpsr);
	}

	DBG2("orr_reg ran\n");

	return SUCCESS;
}

int orr_reg_t2(uint32_t inst) {
	uint8_t rm = (inst & 0xf);
	uint8_t type = (inst & 0x30) >> 4;
	uint8_t imm2 = (inst & 0xc0) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000);

	if (rn == 0xf)
		CORE_ERR_unpredictable("orr_reg_t2 -> MOV\n");

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd >= 13) || (rn == 13) || (rm >= 13))
		CORE_ERR_unpredictable("orr_reg_t2 bad reg\n");

	return orr_reg(S, rd, rn, rm, shift_t, shift_n);
}

int neg(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t result;
	uint32_t rm_val = CORE_reg_read(rm);
	result = 0 - rm_val;
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!in_ITblock(ITSTATE)) {
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

	// bic_imm_t1: 1111 0x00 001x xxxx 0<x's>
	register_opcode_mask(0xf0200000, 0x0bc08000, bic_imm_t1);

	// eor_reg_t2: 1110 1010 100x xxxx 0<x's>
	register_opcode_mask(0xea800000, 0x15608000, eor_reg_t2);

	// lsl_reg_t2: 1111 1010 000x xxxx 1111 xxxx 0000 xxxx
	register_opcode_mask(0xfa00f000, 0x05e000f0, lsl_reg_t2);

	// mvn_reg_t2: 1110 1010 011x 1111 0<x's>
	register_opcode_mask(0xea6f0000, 0x15808000, mvn_reg_t2);

	// orr: 0100 0011 00<x's>
	register_opcode_mask(0x4300, 0xffffbcc0, orr);

	// orr_reg_t2: 1110 1010 010x xxxx 0<x's>
	register_opcode_mask(0xea400000, 0x15a08000, orr_reg_t2);

	// neg: 0100 0010 01<x's>
	register_opcode_mask(0x4240, 0xffffbd80, neg);
}
