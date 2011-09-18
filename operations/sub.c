#include "../cortex_m3.h"
#include "cpsr.h"

int rsb_reg(uint32_t cpsr, uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t shifted = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));

	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t result = rn_val - shifted;

	if (rd == 15) {
		// ALUWritePC(result);
		CORE_ERR_not_implemented("ALUWritePC rsb_reg\n");
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			cpsr = GEN_NZCV(
					!!(result & xPSR_N),
					result == 0,
					!(result > rn_val),
					OVER_SUB(result, rn_val, shifted)
				       );
			CORE_cpsr_write(cpsr);
		}
	}

	DBG2("rsb_reg done\n");

	return SUCCESS;
}

int rsb_reg_t1(uint32_t inst) {
	uint32_t cpsr = CORE_cpsr_read();

	uint8_t rm = inst & 0xf;
	uint8_t type = (inst & 0x3) >> 4;
	uint8_t imm2 = (inst & 0xc) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000) >> 20;

	// d = Uint(rd); n = uint(rn); m = uint(rm);
	uint8_t setflags = (S == 1);
	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if (BadReg(rd) || BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("BadReg in rsb_reg_t1\n");

	return rsb_reg(cpsr, setflags, rd, rn, rm, shift_t, shift_n);
}

int sbc(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t cpsr = CORE_cpsr_read();
	uint32_t result = rd_val - rm_val - !(cpsr & xPSR_C);

	CORE_reg_write(rd, result);

	if (!IN_IT_BLOCK) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				!(result > rd_val),
				OVER_SUB(result, rd_val, rm_val - !(cpsr & xPSR_C)));
		CORE_cpsr_write(cpsr);
	}

	DBG2("sbc r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}

int sub1(uint32_t inst) {
	int32_t immed3 = (inst & 0x1c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t result = rn_val - immed3;
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!IN_IT_BLOCK) {
		cpsr = GEN_NZCV(!!(result & xPSR_N),
				result == 0,
				!(result > rn_val),
				OVER_SUB(result, rn_val, immed3));
		CORE_cpsr_write(cpsr);
	}

	DBG2("sub1 %02d, %02d, #%d\n", rd, rn, immed3);

	return SUCCESS;
}

int sub2(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	int32_t immed8 = inst & 0xff;

	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t result = rd_val - immed8;
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!IN_IT_BLOCK) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				!(result > rd_val), OVER_SUB(result, rd_val, immed8));
		CORE_cpsr_write(cpsr);
	}

	DBG2("sub2 %02d, #%d\n", rd, immed8);

	return SUCCESS;
}

int sub4(uint32_t inst) {
	uint16_t immed7 = inst & 0x7f;

	uint32_t sp = CORE_reg_read(SP_REG);
	sp = sp - (immed7 << 2);
	CORE_reg_write(SP_REG, sp);

	DBG2("sub4, sp, #%d*4\t; 0x%x*4\n", immed7, immed7);

	return SUCCESS;
}

void register_opcodes_sub(void) {
	// rsb_reg_t1: 1110 1011 110x xxxx (0)<x's>
	register_opcode_mask(0xebc00000, 0x14208000, rsb_reg_t1);

	// sbc: 0100 0001 10<x's>
	register_opcode_mask(0x4180, 0xffffbe40, sbc);

	// sub1: 0001 111<x's>
	register_opcode_mask(0x1e00, 0xffffe100, sub1);

	// sub2: 0011 1<x's>
	register_opcode_mask(0x3800, 0xffffc000, sub2);

	// sub4: 1011 0000 1<x's>
	register_opcode_mask(0xb080, 0xffff4f00, sub4);
}
