#include "../cortex_m3.h"
#include "cpsr.h"

int adc(uint32_t inst) {
	uint8_t rm = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t cpsr = CORE_cpsr_read();
	uint32_t result = rd_val + rm_val + !(cpsr & xPSR_C);
	CORE_reg_write(rd, result);

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				result < rd_val,
				OVER_ADD(result, rd_val, rm_val + !!(cpsr & xPSR_C)));
		CORE_cpsr_write(cpsr);
	}

	DBG2("adc r%02d, r%02d\n", rd, rm);

	return SUCCESS;
}

int add1(uint32_t inst) {
	int32_t immed3 = (inst & 0x1c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	int32_t rn_val = CORE_reg_read(rn);
	int32_t result = rn_val + immed3;

	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				result < rn_val, OVER_ADD(result, rn_val, immed3));
		CORE_cpsr_write(cpsr);
	}

	DBG2("add1 r%02d = r%02d + %d\n", rd, rn, immed3);

	return SUCCESS;
}

int add2(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	int32_t immed8 = (inst & 0xff);

	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t result = rd_val + immed8;
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				result < rd_val, OVER_ADD(result, rd_val, immed8));
		CORE_cpsr_write(cpsr);
	}

	DBG2("add2 r%02d, #%d\t; 0x%x\n", rd, immed8, immed8);

	return SUCCESS;
}

int add3(uint32_t inst) {
	uint8_t rm = (inst & 0x1c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t result = rn_val + rm_val;
	CORE_reg_write(rd, result);

	uint32_t cpsr = CORE_cpsr_read();

	if (!in_ITblock(ITSTATE)) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0,
				result < rn_val, OVER_ADD(result, rn_val, rm_val));
		CORE_cpsr_write(cpsr);
	}

	DBG2("add3 r%02d, r%02d, r%02d\n", rd, rn, rm);

	return SUCCESS;
}

int add6(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	int32_t immed8 = (inst & 0xff);

	uint32_t sp = CORE_reg_read(SP_REG);
	uint32_t result = sp + (immed8 << 2);
	CORE_reg_write(rd, result);

	DBG2("add6 r%02d, SP, #%d*4\n", rd, immed8);

	return SUCCESS;
}

int add7(uint32_t inst) {
	uint32_t immed7 = inst & 0x7f;

	uint32_t sp = CORE_reg_read(SP_REG);
	sp = sp + (immed7 << 2);
	CORE_reg_write(SP_REG, sp);

	DBG2("add7 sp, #%d*4\t; 0x%x\n", immed7, immed7);

	return SUCCESS;
}

int add_imm(uint8_t rn, uint8_t rd, uint32_t imm32, uint8_t setflags) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result = rn_val + imm32;
	CORE_reg_write(rd, result);

	if (setflags) {
		uint32_t cpsr = CORE_cpsr_read();
		cpsr = GEN_NZCV(
				!!(result & xPSR_N),
				result == 0,
				result < rn_val,
				OVER_ADD(result, rn_val, imm32)
			       );
		CORE_cpsr_write(cpsr);
	}

	DBG2("add r%02d = r%02d + 0x%08x\t%08x = %08x + %08x\n",
			rd, rn, imm32, result, rn_val, imm32);

	return SUCCESS;
}

int add_imm_t3(uint32_t inst) {
	int imm8 =    (inst & 0x000000ff);
	uint8_t rd =  (inst & 0x00000f00) >> 8;
	int imm3 =    (inst & 0x00007000) >> 12;
	assert(0 ==   (inst & 0x00008000));
	uint8_t rn =  (inst & 0x000f0000) >> 16;
	uint8_t S = !!(inst & 0x00100000);
	// skip 5
	uint8_t i = !!(inst & 0x04000000);

	if ((rd == 0xf) && (S ==1)) {
		CORE_ERR_not_implemented("CMN case\n");
	}

	if (rn == 0xd) {
		CORE_ERR_not_implemented("ADD (SP+imm) case\n");
	}

	// d = UInt(rd); unnecessary
	// n = UInt(rn); unnecessary

	uint8_t setflags = (S == 1);

	uint16_t imm12 = (i << 11) | (imm3 << 8) | imm8;
	DBG2("i %d imm3 %01x imm8 %02x\t(imm12 %0x)\n", i, imm3, imm8, imm12);
	//int imm32 = ThumbExpandImm(i:imm3:imm8);
	int imm32 = ThumbExpandImm(imm12);

	if ((rd == 13 || ((rd == 15) && (S == 0))) || (rn == 15))
		CORE_ERR_unpredictable("add_imm_t3 case\n");

	return add_imm(rn, rd, imm32, setflags);
}

int add_reg(uint32_t cpsr, uint8_t setflags, uint8_t rn, uint8_t rm, uint8_t rd, enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t result;

	uint32_t shifted;
	shifted = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));

	result = rn_val + shifted;
	if (rd == 15) {
		// ALUWritePC
		CORE_ERR_not_implemented("ALUWritePC case add_reg\n");
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			cpsr = GEN_NZCV(
					!!(result & xPSR_N),
					result == 0,
					result < rn_val,
					OVER_ADD(result, rn_val, shifted)
				       );
			CORE_cpsr_write(cpsr);
		}
	}

	DBG2("add_reg r%02d = 0x%08x\n", rd, result);

	return SUCCESS;
}

int add_reg_t2(uint32_t inst) {
	uint32_t cpsr = CORE_cpsr_read();

	uint8_t rm = (inst & 0x78) >> 3;
	uint8_t rd = (((inst & 0x80) >> 7) | ((inst & 0x7) >> 0));

	if ((rd == 13) || (rm == 13))
		CORE_ERR_not_implemented("add_reg_t2 -> SP+reg\n");

	// d = UInt(DN:Rdn); n = d; m = UInt(Rm);
	uint8_t rn = rd;
	uint8_t setflags = false;

	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(SRType_LSL, 0, &shift_t, &shift_n);

	if ((rd == 15) && (rm == 15))
		CORE_ERR_unpredictable("add_reg_t2\n");

	if ((rd == 15) && in_ITblock(ITSTATE) && !last_in_ITblock(ITSTATE))
		CORE_ERR_unpredictable("add_reg_t2 it\n");

	return add_reg(cpsr, setflags, rn, rm, rd, shift_t, shift_n);
}

void register_opcodes_add(void) {
	// adc: 0100 0001 01<x's>
	register_opcode_mask(0x4140, 0xffffbe80, adc);

	// add1: 0001 110x xxxx xxxx
	register_opcode_mask(0x1c00, 0xffffe200, add1);

	// add2: 0011 0<x's>
	register_opcode_mask(0x3000, 0xffffc800, add2);

	// add3: 0001 100<x's>
	register_opcode_mask(0x1800, 0xffffe600, add3);

	// add6: 1010 1<s's>
	register_opcode_mask(0xa800, 0xffff5000, add6);

	// add7: 1011 0000 0<x's>
	register_opcode_mask(0xb000, 0xffff4f80, add7);

	// add_imm_t3: 1111 0x01 000x xxxx 0<x's>
	register_opcode_mask(0xf1000000, 0x0ae08000, add_imm_t3);

	// add_reg_t2: 0100 0100 <x's>
	register_opcode_mask(0x4400, 0xffffbb00, add_reg_t2);
}
