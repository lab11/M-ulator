#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"
#include "../core.h"
#include "../misc.h"

static void ldr1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t addr = CORE_reg_read(rn) + (immed5 * 4);
	if ((addr & 0x3) == 0) {
		CORE_reg_write(rd, read_word(addr));
	} else {
		// misaligned
		DBG2("addr: %08x\n", addr);
		DBG2("immed5: %x, immed5*4: %x\n", immed5, immed5*4);
		CORE_ERR_unpredictable("misaligned access, ldr1\n");
	}

	DBG2("ldr1 r%02d, [r%02d, #%d*4]\n", rd, rn, immed5);
}

static void ldr2(uint32_t inst) {
	uint8_t rm = (inst & 0x1c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7);

	uint32_t addr = CORE_reg_read(rn) + CORE_reg_read(rm);

	if (addr & 0x3) {
		CORE_ERR_unpredictable("ldr2 bad addr\n");
	} else {
		CORE_reg_write(rd, read_word(addr));
	}

	DBG2("ldr2 r%02d, [r%02d, r%02d]\n", rd, rn, rm);
}

static void ldr4(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	uint32_t immed8 = inst & 0xff;

	uint32_t sp = CORE_reg_read(SP_REG);

	uint32_t addr = sp + (immed8 << 2);

	if ((addr & 0x3) == 0) {
		uint32_t rd_val = read_word(addr);
		CORE_reg_write(rd, rd_val);
	} else {
		CORE_ERR_invalid_addr(true, addr);
	}

	DBG2("ldr r%02d, [sp, #%d * 4]\n", rd, immed8);
}

static void ldr_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + imm32;
	else
		offset_addr = rn_val - imm32;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	uint32_t data = read_word(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	if (rt == 15) {
		if (address & 0x3)
			CORE_ERR_unpredictable("Illegal load of PC in ldr imm\n");
		else
			LoadWritePC(data);
	} else {
		CORE_reg_write(rt, data);
	}
}

static void ldr_imm_t3(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if ((rt == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("invalid ldr imm to pc\n");

	ldr_imm(rt, rn, imm32, index, add, wback);
}

static void ldr_imm_t4(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = !!(inst & 0x100);
	bool U = !!(inst & 0x200);
	bool P = !!(inst & 0x400);
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;
	bool index = P;
	bool add = U;
	bool wback = W;

	if ((wback && (rn == rt)) || ((rt == 15) && in_ITblock() && !last_in_ITblock()))
		CORE_ERR_unpredictable("bad regs\n");

	return ldr_imm(rt, rn, imm32, index, add, wback);
}

static void ldr_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t cpsr = CORE_cpsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	uint32_t data = read_word(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	if (rt == 15) {
		if (address & 0x3)
			CORE_ERR_unpredictable("bad dest pc\n");
		else
			LoadWritePC(data);
	} else {
		CORE_reg_write(rt, data);
	}
}

static void ldr_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = LSL;
	uint8_t shift_n = imm2;

	if (BadReg(rm))
		CORE_ERR_unpredictable("bad reg\n");

	if ((rt == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("it probs\n");

	return ldr_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

static void ldr_lit(bool add, uint8_t rt, uint32_t imm32) {
	uint32_t base = 0xfffffffc & CORE_reg_read(PC_REG);

	uint32_t addr;
	if (add) {
		addr = base + imm32;
	} else {
		addr = base - imm32;
	}

	uint32_t data = read_word(addr);

	if (rt == 15) {
		if (addr & 0x3) {
			CORE_ERR_unpredictable("ldr_lit misaligned\n");
		} else {
			LoadWritePC(data);
		}
	} else {
		CORE_reg_write(rt, data);
	}

	DBG2("ldr_lit completed\n");
}

static void ldr_lit_t1(uint32_t inst) {
	uint32_t imm8 = inst & 0xff;
	uint8_t rt = (inst & 0x700) >> 8;

	uint32_t imm32 = imm8 << 2;

	return ldr_lit(true, rt, imm32);
}

static void ldr_lit_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst & 0xf000) >> 12;
	bool U = !!(inst & 0x00800000);

	if ((rt == 15) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("ldr_lit_t2 PC in IT\n");

	uint32_t imm32 = imm12;

	return ldr_lit(U, rt, imm32);
}

static void ldrb1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t addr = CORE_reg_read(rn) + immed5;
	CORE_reg_write(rd, read_byte(addr));

	DBG2("ldrb r%02d, [r%02d, #%d]\n", rd, rn, immed5);
}

static void ldrb_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool add, bool index, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + imm32;
	else
		offset_addr = rn_val - imm32;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	CORE_reg_write(rt, read_byte(address));

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

static void ldrb_imm_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if (rt == 13)
		CORE_ERR_unpredictable("reg 13 not allowed\n");

	ldrb_imm(rt, rn, imm32, add, index, wback);
}

static void ldrb_imm_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	bool W = !!(inst & 0x100);		// wback
	bool U = !!(inst & 0x200);		// add
	bool P = !!(inst & 0x400);		// index
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm8;

	if ((rt == 13) && (W && (rn == rt)))
		CORE_ERR_unpredictable("bad regs\n");

	if ((rt == 15) && ((P == 0) || (U == 1) || (W == 1)))
		CORE_ERR_unpredictable("bad regs / flags\n");

	ldrb_imm(rt, rn, imm32, U, P, W);
}

static void ldrb_reg(uint8_t rt, uint8_t rn, uint8_t rm, enum SRType shift_t,
		uint8_t shift_n, bool index, bool add, bool wback) {
	assert(wback == false);

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t cpsr = CORE_cpsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	CORE_reg_write(rt, read_byte(address));
}

static void ldrb_reg_t1(uint32_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	return ldrb_reg(rt, rn, rm, shift_t, shift_n, index, add, wback);
}

static void ldrb_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = SRType_LSL;
	uint8_t shift_n = imm2;

	if ((rt == 13) || BadReg(rm))
		CORE_ERR_unpredictable("bad reg\n");

	return ldrb_reg(rt, rn, rm, shift_t, shift_n, index, add, wback);
}

static void ldrsb_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	assert(wback == false);

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t cpsr = CORE_cpsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	int32_t signd = (int32_t) read_byte(address);
	CORE_reg_write(rt, signd);
}

static void ldrsb_reg_t1(uint32_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool index = true;
	bool add = true;
	bool wback = false;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	return ldrsb_reg(rt, rn, rm, index, add, wback, shift_t, shift_n);
}

static void ldrd_imm(uint32_t inst) {
	uint32_t imm8 = (inst & 0xff);
	uint8_t rt2 = (inst & 0xf00) >> 8;
	uint8_t rt = (inst & 0xf000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t W = !!(inst & 0x200000);
	uint8_t U = !!(inst & 0x800000);
	uint8_t P = !!(inst & 0x1000000);

	uint32_t imm32 = imm8 << 2;
	// index = P
	// add = U
	// wback = W

	if (W && ((rn == rt) || (rn == rt2)))
		CORE_ERR_unpredictable("ldrd_imm wbck + regs\n");

	if ((rt >= 13) || (rt2 >= 13) || (rt == rt2))
		CORE_ERR_unpredictable("ldrd_imm bad regs\n");

	uint32_t offset_addr;
	if (U) {
		offset_addr = CORE_reg_read(rn) + imm32;
	} else {
		offset_addr = CORE_reg_read(rn) - imm32;
	}

	uint32_t addr;
	if (P) {
		addr = offset_addr;
	} else {
		addr = CORE_reg_read(rn);
	}

	CORE_reg_write(rt, read_word(addr));
	CORE_reg_write(rt2, read_word(addr + 4));

	if (W) {
		CORE_reg_write(rn, offset_addr);
	}

	DBG2("ldrd_imm did lots of stuff\n");
}

void register_opcodes_ld(void) {
	// ldr1: 0110 1<x's>
	register_opcode_mask(0x6800, 0xffff9000, ldr1);

	// ldr2: 0101 100<x's>
	register_opcode_mask(0x5800, 0xffffa600, ldr2);

	// ldr4: 1001 1<x's>
	register_opcode_mask(0x9800, 0xffff6000, ldr4);

	// ldr_imm_t3: 1111 1000 1101 xxxx <x's>
	//                            1111
	register_opcode_mask_ex(0xf8d00000, 0x07200000, ldr_imm_t3,
			0x000f0000, 0x0, 0, 0);

	// ldr_imm_t4: 1111 1000 0101 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_ex(0xf8500800, 0x07a00000, ldr_imm_t4,
			0xf0000, 0x0,
			0x600, 0x100,
			0xd0304, 0x204fb,
			0x0, 0x500,
			0, 0);

	// ldr_reg_t2: 1111 1000 0101 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_ex(0xf8500000, 0x07a00fc0, ldr_reg_t2,
			0xf0000, 0x0, 0, 0);

	// ldr_lit_t1: 0100 1<x's>
	register_opcode_mask(0x4800, 0xffffb000, ldr_lit_t1);

	// ldr_lit_t2: 1111 1000 x101 1111 <x's>
	register_opcode_mask(0xf85f0000, 0x07200000, ldr_lit_t2);

	// ldrb1: 0111 1<x's>
	register_opcode_mask(0x7800, 0xffff8000, ldrb1);

	// ldrb_imm_t2: 1111 1000 1001 <x's>
	register_opcode_mask_ex(0xf8900000, 0x07600000, ldrb_imm_t2, 0xf000, 0x0, 0xf0000, 0x0, 0, 0);

	// ldrb_imm_t3: 1111 1000 0001 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_ex(0xf8100800, 0x07e00000, ldrb_imm_t3, 0xf400, 0x300, 0xf0000, 0x0, 0x600, 0x100, 0x0, 0x500, 0, 0);

	// ldrb_reg_t1: 0101 110x xxxx xxxx
	register_opcode_mask(0x5c00, 0xffffa200, ldrb_reg_t1);

	// ldrb_reg_t2: 1111 1000 0001 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_ex(0xf8100000, 0x07e00fc0, ldrb_reg_t2, 0xf000, 0x0, 0xf0000, 0x0, 0, 0);

	// ldrsb_reg_t1: 0101 011x xxxx xxxx
	register_opcode_mask(0x5600, 0xffffa800, ldrsb_reg_t1);

	// ldrd_imm: 1110 100x x1x1 <x's>
	register_opcode_mask_ex(0xe8500000, 0x16000000, ldrd_imm, 0x0, 0x01200000, 0xf0000, 0x0, 0, 0);
}
