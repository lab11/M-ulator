#include "../cortex_m3.h"
#include "cpsr.h"

int lsl1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t rm_val = CORE_reg_read(rm);
	uint8_t c_flag;
	uint32_t result;

	uint32_t cpsr = CORE_cpsr_read();

	if (immed5 == 0) {
		result = rm_val;
		c_flag = !!(cpsr & xPSR_C);
	} else {
		c_flag = !!( rm_val & (1 << (32 - immed5)) );
		result = rm_val << immed5;
	}

	CORE_reg_write(rd, result);

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				c_flag, !!(cpsr & xPSR_V));
		CORE_cpsr_write(cpsr);
	}

	DBG2("lsls r%02d, r%02d, #%d\n", rd, rm, immed5);

	return SUCCESS;
}

int lsl2(uint32_t inst) {
	uint8_t rs = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t rs_val = CORE_reg_read(rs);
	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t result;

	uint32_t cpsr = CORE_cpsr_read();
	uint8_t cflag = !!(cpsr & xPSR_C);

	if ((rs_val & 0xff) == 0) {
		// nop
		return SUCCESS;
	} else if ((rs_val & 0xff) < 32) {
		cflag = !!(rd & (1 << (32 - (rs_val & 0xff))));
		result = rd_val << (rs_val & 0xff);
	} else if ((rs_val & 0xff) == 32) {
		cflag = rd_val & 0x1;
		result = 0;
	} else {
		cflag = 0;
		result = 0;
	}

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				cflag, !!(cpsr & xPSR_V));
		CORE_cpsr_write(cpsr);
	}

	DBG2("lsl2 r%02d, r%02d\n", rd, rs);

	return SUCCESS;
}

int lsr1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t rm_val = CORE_reg_read(rm);
	uint8_t c_flag;
	uint32_t result;

	uint32_t cpsr = CORE_cpsr_read();

	if (immed5 == 0) {
		result = rm_val;
		c_flag = !!(cpsr & xPSR_C);
	} else {
		c_flag = !!( rm_val & (1 << (immed5 - 1)) );
		result = rm_val >> immed5;
	}

	CORE_reg_write(rd, result);

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				c_flag, !!(cpsr & xPSR_V));
		CORE_cpsr_write(cpsr);
	}

	DBG2("lsrs r%02d, r%02d, #%d\n", rd, rm, immed5);

	return SUCCESS;
}

int asr_imm(uint32_t cpsr, uint8_t setflags, uint8_t rd, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t result;
	uint8_t carry;
	Shift_C(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C), &result, &carry);

	if (rd == 15) {
		// ALUWritePC(result);
		CORE_ERR_not_implemented("ALUWritePC asr_imm\n");
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			cpsr = GEN_NZCV(
					!!(result & xPSR_N),
					result == 0,
					carry,
					!!(cpsr & xPSR_V)
				       );
		}
	}

	DBG2("asr_imm complete\n");

	return SUCCESS;
}


int mov_shifted_reg(uint32_t inst, enum SRType shift_t) {
	uint32_t cpsr = CORE_cpsr_read();

	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst & 0xc) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t S = !!(inst & 0x100000);

	// d = UInt(rd); m = Uint(rm);

	uint8_t setflags = (S == 1);

	uint8_t imm5 = (imm3 << 2) | imm2;
	uint8_t shift_n;
	DecodeImmShift(SRType_ENUM_TO_MASK(shift_t), imm5, &shift_t, &shift_n);

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("BadReg in (fake)mov_shifted_reg\n");

	return asr_imm(cpsr, setflags, rd, rm, shift_t, shift_n);
}

int asr_imm_t2(uint32_t inst) {
	return mov_shifted_reg(inst, ASR);
}

int lsl_imm_t2(uint32_t inst) {
	return mov_shifted_reg(inst, LSL);
}

void register_opcodes_shift(void) {
	// lsl1: 0000 0<x's>
	register_opcode_mask(0x0000, 0xfffff800, lsl1);

	// lsl2: 0100 0000 10 <x's>
	register_opcode_mask(0x4080, 0xffffbf40, lsl2);

	// lsr1: 0000 1<x's>
	register_opcode_mask(0x0800, 0xfffff000, lsr1);

	// asr_imm_t2: 1110 1010 010x 1111 (0)xxx xxxx xx10 xxxx
	register_opcode_mask(0xea4f0020, 0x15a08010, asr_imm_t2);

	// lsr_imm_t2: 1110 1010 010x 1111 (0)xxx xxxx xx01 xxxx
	register_opcode_mask(0xea4f0010, 0x15a08020, lsl_imm_t2);
}
