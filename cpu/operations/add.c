#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"
#include "../misc.h"

static void adc_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t cpsr = CORE_cpsr_read();
	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, imm32, !!(cpsr & xPSR_C), &result, &carry, &overflow);
	CORE_reg_write(rd, result);

	if (setflags) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0, carry, overflow);
		CORE_cpsr_write(cpsr);
	}
}

static void adc_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool S = (inst >> 20) & 0x1;
	bool i = !!(inst & 0x04000000);

	bool setflags = S;
	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | imm8);

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("Bad reg\n");

	return adc_imm(rd, rn, imm32, setflags);
}

static void adc_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n) {
	uint32_t cpsr = CORE_cpsr_read();

	uint32_t shifted = Shift(CORE_reg_read(rm), 32,
			shift_t, shift_n, !!(cpsr & xPSR_C));

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(CORE_reg_read(rn), shifted, !!(cpsr & xPSR_C),
			&result, &carry, & overflow);

	CORE_reg_write(rd, result);

	if (setflags) {
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0, carry, overflow);
		CORE_cpsr_write(cpsr);
	}
}

static void adc_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;
	bool setflags = !in_ITblock();

	return adc_reg(rdn, rdn, rm, setflags, SRType_LSL, 0);
}

static void add6(uint16_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	uint8_t immed8 = (inst & 0xff);

	uint32_t sp = CORE_reg_read(SP_REG);
	uint32_t result = sp + ((uint32_t)immed8 << 2U);
	CORE_reg_write(rd, result);

	DBG2("add6 r%02d, SP, #%d*4\n", rd, immed8);
}

static void add7(uint16_t inst) {
	uint32_t immed7 = inst & 0x7f;

	uint32_t sp = CORE_reg_read(SP_REG);
	sp = sp + (immed7 << 2);
	CORE_reg_write(SP_REG, sp);

	DBG2("add7 sp, #%d*4\t; 0x%x\n", immed7, immed7);
}

static void add_imm(uint8_t rn, uint8_t rd, uint32_t imm32, uint8_t setflags) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, imm32, 0, &result, &carry, &overflow);

	CORE_reg_write(rd, result);

	if (setflags) {
		uint32_t cpsr = CORE_cpsr_read();
		cpsr = GEN_NZCV(
				!!(result & xPSR_N),
				result == 0,
				carry,
				overflow
			       );
		CORE_cpsr_write(cpsr);
	}

	DBG2("add r%02d = r%02d + 0x%08x\t%08x = %08x + %08x\n",
			rd, rn, imm32, result, rn_val, imm32);
}

static void add_imm_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm3 = (inst >> 6) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm3;

	return add_imm(rn, rd, imm32, setflags);
}

static void add_imm_t2(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rdn = (inst >> 8) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm8;

	return add_imm(rdn, rdn, imm32, setflags);
}

static void add_imm_t3(uint32_t inst) {
	uint32_t imm8 =    (inst & 0x000000ff);
	uint8_t rd =  (inst & 0x00000f00) >> 8;
	uint32_t imm3 =    (inst & 0x00007000) >> 12;
	assert(0 ==   (inst & 0x00008000));
	uint8_t rn =  (inst & 0x000f0000) >> 16;
	uint32_t S = !!(inst & 0x00100000);
	// skip 5
	uint32_t i = !!(inst & 0x04000000);

	uint8_t setflags = (S == 1);

	uint32_t imm12 = (i << 11) | (imm3 << 8) | imm8;
	DBG2("i %d imm3 %01x imm8 %02x\t(imm12 %0x)\n", i, imm3, imm8, imm12);
	//int imm32 = ThumbExpandImm(i:imm3:imm8);
	uint32_t imm32 = ThumbExpandImm(imm12);

	if ((rd == 13 || ((rd == 15) && (S == 0))) || (rn == 15))
		CORE_ERR_unpredictable("add_imm_t3 case\n");

	return add_imm(rn, rd, imm32, setflags);
}

static void add_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t cpsr = CORE_cpsr_read();

	uint32_t shifted;
	shifted = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, shifted, 0, &result, &carry, &overflow);

	if (rd == 15) {
		// ALUWritePC
		CORE_ERR_not_implemented("ALUWritePC case add_reg\n");
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			cpsr = GEN_NZCV(
					!!(result & xPSR_N),
					result == 0,
					carry,
					overflow
				       );
			CORE_cpsr_write(cpsr);
		}
	}

	DBG2("add_reg r%02d = 0x%08x\n", rd, result);
}

static void add_reg_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool setflags = !in_ITblock();

	return add_reg(rd, rn, rm, setflags, SRType_LSL, 0);
}

static void add_reg_t2(uint16_t inst) {
	uint8_t rm = (inst & 0x78) >> 3;
	uint8_t rd = (((inst & 0x80) >> 7) | ((inst & 0x7) >> 0));

	if ((rd == 13) || (rm == 13))
		CORE_ERR_not_implemented("add_reg_t2 -> SP+reg\n");

	// d = UInt(DN:Rdn); n = d; m = UInt(Rm);
	uint8_t rn = rd;
	bool setflags = false;

	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(SRType_LSL, 0, &shift_t, &shift_n);

	if ((rd == 15) && (rm == 15))
		CORE_ERR_unpredictable("add_reg_t2\n");

	if ((rd == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("add_reg_t2 it\n");

	return add_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

static void add_reg_t3(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool S = !!(inst & 0x100000);

	bool setflags = S;

	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if ((rd == 13) || ((rd == 15) && (S == 0)) || (rn == 15) || BadReg(rm))
		CORE_ERR_unpredictable("bad regs\n");

	return add_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

static void add_sp_plus_imm(uint8_t rd, uint32_t imm32, bool setflags) {
	uint32_t sp_val = CORE_reg_read(SP_REG);

	uint32_t result;
	bool carry;
	bool overflow;

	AddWithCarry(sp_val, imm32, 0, &result, &carry, &overflow);

	CORE_reg_write(rd, result);

	if (setflags) {
		uint32_t cpsr = CORE_cpsr_read();
		cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0, carry, overflow);
		CORE_cpsr_write(cpsr);
	}
}

static void add_sp_plus_imm_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool S = !!(inst & 0x100000);
	bool i = !!(inst & 0x04000000);

	bool setflags = S;
	uint32_t imm12 = (i << 11) | (imm3 << 8) | (imm8);
	uint32_t imm32 = ThumbExpandImm(imm12);

	if ((rd == 15) && (S == 0))
		CORE_ERR_unpredictable("r15 w/out S\n");

	return add_sp_plus_imm(rd, imm32, setflags);
}

static void add_sp_plus_imm_t4(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool i = !!(inst & 0x04000000);

	bool setflags = false;
	uint32_t imm32 = (i << 11) | (imm3 << 8) | (imm8);

	if (rd == 15)
		CORE_ERR_unpredictable("bad reg 15\n");

	return add_sp_plus_imm(rd, imm32, setflags);
}

void register_opcodes_add(void) {
	// adc_imm_t1: 1111 0x01 010x xxxx 0xxx xxxx xxxx xxxx
	register_opcode_mask_32(0xf1400000, 0x0aa08000, adc_imm_t1);

	// adc_reg_t1: 0100 0001 01<x's>
	register_opcode_mask_16(0x4140, 0xbe80, adc_reg_t1);

	// add6: 1010 1<s's>
	register_opcode_mask_16(0xa800, 0x5000, add6);

	// add7: 1011 0000 0<x's>
	register_opcode_mask_16(0xb000, 0x4f80, add7);

	// add_imm_t1: 0001 110x xxxx xxxx
	register_opcode_mask_16(0x1c00, 0xe200, add_imm_t1);

	// add_imm_t2: 0011 0<x's>
	register_opcode_mask_16(0x3000, 0xc800, add_imm_t2);

	// add_imm_t3: 1111 0x01 000x xxxx 0<x's>
	register_opcode_mask_32_ex(0xf1000000, 0x0ae08000, add_imm_t3,
			0x100f00, 0x0,
			0xd0000, 0x20000,
			0, 0);

	// add_reg_t1: 0001 100<x's>
	register_opcode_mask_16(0x1800, 0xe600, add_reg_t1);

	// add_reg_t2: 0100 0100 <x's>
	register_opcode_mask_16(0x4400, 0xbb00, add_reg_t2);

	// add_reg_t3: 1110 1011 000x xxxx 0<x's>
	register_opcode_mask_32_ex(0xeb000000, 0x14e08000, add_reg_t3, 0x100f00, 0x0, 0xd0000, 0x20000, 0, 0);

	// add_sp_plus_imm_t3: 1111 0x01 000x 1101 0<x's>
	register_opcode_mask_32_ex(0xf10d0000, 0x0ae28000, add_sp_plus_imm_t3, 0x100f00, 0x0, 0, 0);

	// add_sp_plus_imm_t4: 1111 0x10 0000 1101 0<x's>
	register_opcode_mask_32(0xf20d0000, 0x09f28000, add_sp_plus_imm_t4);
}
