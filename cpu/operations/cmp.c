#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"

/* CMP and TST always write cpsr bits, regardless of itstate */

static void cmn_imm(uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t cpsr = CORE_cpsr_read();

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, imm32, 0, &result, &carry, &overflow);

	cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0, carry, overflow);
	CORE_cpsr_write(cpsr);
}

static void cmn_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool i = !!(inst & 0x04000000);

	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | (imm8));

	if (rn == 15)
		CORE_ERR_unpredictable("bad reg\n");

	return cmn_imm(rn, imm32);
}

static void cmp1(uint16_t inst) {
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

static void cmp2(uint16_t inst) {
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

static void cmp_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t cpsr = CORE_cpsr_read();
	uint32_t shifted = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, ~shifted, 1, &result, &carry, &overflow);

	cpsr = GEN_NZCV(!!(result & xPSR_N), result == 0, carry, overflow);
	CORE_cpsr_write(cpsr);
}

static void cmp_reg_t2(uint16_t inst) {
	uint8_t rn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0xf;
	bool N = (inst >> 7) & 0x1;

	rn = (N << 3) | rn;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	if ((rn < 8) && (rm < 8))
		CORE_ERR_unpredictable("cmp bad reg\n");

	if ((rn == 15) && (rm == 15))
		CORE_ERR_unpredictable("cmp bad regs\n");

	return cmp_reg(rn, rm, shift_t, shift_n);
}

static void cmp_imm(uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result;
	bool carry_out;
	bool overflow_out;

	DBG2("rn_val: %08x, ~imm32: %08x\n", rn_val, ~imm32);
	AddWithCarry(rn_val, ~imm32, 1, &result, &carry_out, &overflow_out);
	DBG2("result: %08x, carry: %d, overflow: %d\n",
			result, carry_out, overflow_out);

	uint32_t cpsr = CORE_cpsr_read();

	cpsr = GEN_NZCV(
			!!(result & xPSR_N),
			result == 0,
			carry_out,
			overflow_out
		       );
	CORE_cpsr_write(cpsr);
}

static void cmp_imm_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;
	bool i = (inst >> 26) & 0x1;

	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | imm8);

	DBG2("imm32: %08x\n", imm32);

	cmp_imm(rn, imm32);
}

void register_opcodes_cmp(void) {
	// cmn_imm_t1: 1111 0x01 0001 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xf1100f00, 0x0ae08000, cmn_imm_t1);

	// cmp1: 0010 1<x's>
	register_opcode_mask_16(0x2800, 0xd000, cmp1);

	// cmp2: 0100 0010 10<x's>
	register_opcode_mask_16(0x4280, 0xbd40, cmp2);

	// cmp_reg_t2: 0100 0101 xxxx xxxx
	register_opcode_mask_16(0x4500, 0xba00, cmp_reg_t2);

	// 1111 0x01 1011 xxxx 0xxx 1111 xxxx xxxx
	register_opcode_mask_32(0xf1b00f00, 0x0a408000, cmp_imm_t2);
}
