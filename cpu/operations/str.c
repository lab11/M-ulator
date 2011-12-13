#include "opcodes.h"
#include "helpers.h"

#include "../cpu.h"
#include "../core.h"

void str1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t address = CORE_reg_read(rn) + (immed5 * 4U);
	uint32_t rd_val = CORE_reg_read(rd);
	if ((address & 0x3) == 0) {
		write_word(address, rd_val);
	} else {
		// misaligned
		CORE_ERR_unpredictable("str1, misaligned\n");
	}

	DBG2("str1 r%02d, [r%02d, #%d*4]\t0x%08x = 0x%08x\n",
			rd, rn, immed5, address, rd_val);
}

void str2(uint32_t inst) {
	uint8_t rm = (inst & 0x1c) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7);

	uint32_t address = CORE_reg_read(rn) + CORE_reg_read(rm);

	if (address & 0x3) {
		CORE_ERR_unpredictable("str2 bad addr\n");
	} else {
		write_word(address, CORE_reg_read(rd));
	}

	DBG2("str r%02d, [r%02d, r%02d]\n", rd, rn, rm);
}

void str3(uint32_t inst) {
	uint8_t rd = (inst & 0x700) >> 8;
	uint16_t immed8 = inst & 0xff;

	uint32_t sp = CORE_reg_read(SP_REG);
	uint32_t rd_val = CORE_reg_read(rd);

	uint32_t address = sp + (immed8 << 2);
	if ((address & 0x3) == 0) {
		write_word(address, rd_val);
	} else {
		CORE_ERR_invalid_addr(true, address);
	}

	DBG2("str r%02d, [sp, #%d * 4]\n", rd, immed8);
}

void strb1(uint32_t inst) {
	uint8_t immed5 = (inst & 0x7c0) >> 6;
	uint8_t rn = (inst & 0x38) >> 3;
	uint8_t rd = (inst & 0x7) >> 0;

	uint32_t address = CORE_reg_read(rn) + immed5;
	write_byte(address, CORE_reg_read(rd) & 0xff);

	DBG2("strb r%02d, [r%02d, #%d]\n", rd, rn, immed5);
}

void str_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool index, bool add, bool wback) {
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

	uint32_t rt_val = CORE_reg_read(rt);
	write_word(address, rt_val);

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

void str_imm_t4(uint32_t inst) {
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

	if ((rt == 15) || (wback && (rn == rt)))
		CORE_ERR_unpredictable("Bad regs\n");

	return str_imm(rt, rn, imm32, index, add, wback);
}

void strb_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rt_val = CORE_reg_read(rt);

	uint32_t offset_addr;
	if (add) {
		offset_addr = rn_val + imm32;
	} else {
		offset_addr = rn_val - imm32;
	}

	uint32_t address;
	if (index) {
		address = offset_addr;
	} else {
		address = rn_val;
	}

	write_byte(address, rt_val & 0xff);

	if (wback) {
		CORE_reg_write(rn, offset_addr);
	}

	DBG2("strb_imm ran\n");
}

void strb_imm_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	if (rn == 0xf)
		CORE_ERR_unpredictable("strb_imm_t2 rn == 0xf undef\n");

	bool index = true;
	bool add = true;
	bool wback = false;

	uint32_t imm32 = imm12;

	if (rt >= 13)
		CORE_ERR_unpredictable("strb_imm_t2 rt in {13,15}\n");

	return strb_imm(rt, rn, imm32, index, add, wback);
}

void strb_imm_t3(uint32_t inst) {
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

	if (BadReg(rt) || (wback && (rn == rt)))
		CORE_ERR_unpredictable("bad reg\n");

	return strb_imm(rt, rn, imm32, index, add, wback);
}

void strd_imm(uint32_t inst) {
	uint8_t imm8 = (inst & 0xff);
	uint8_t rt2 = (inst & 0xf00) >> 8;
	uint8_t rt = (inst & 0xf000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t W = !!(inst & 0x200000);
	uint8_t U = !!(inst & 0x800000);
	uint8_t P = !!(inst & 0x1000000);

	if ((P == 0) && (W == 0))
		CORE_ERR_unpredictable("strd_imm -> EX\n");

	uint32_t imm32 = imm8 << 2;
	// index = P
	// add = U
	// wback = W

	if (W && ((rn == rt) || (rn == rt2)))
		CORE_ERR_unpredictable("strd_imm wback + regs\n");

	if ((rn == 15) || (rt >= 13) || (rt2 >= 13))
		CORE_ERR_unpredictable("strd_imm bad regs\n");

	uint32_t offset_addr;
	if (U) {
		offset_addr = CORE_reg_read(rn) + imm32;
	} else {
		offset_addr = CORE_reg_read(rn) - imm32;
	}

	uint32_t address;
	if (P) {
		address = offset_addr;
	} else {
		address = CORE_reg_read(rn);
	}

	write_word(address, CORE_reg_read(rt));
	write_word(address + 4, CORE_reg_read(rt2));

	if (W) {
		CORE_reg_write(rn, offset_addr);
	}

	DBG2("strd_imm did lots of stuff\n");
}

void strh_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool index, bool add, bool wback) {
	uint32_t rt_val = CORE_reg_read(rt);
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

	write_halfword(address, rt_val & 0xffff);

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

void strh_imm_t2(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if ((rt == 13) || (rt == 15))
		CORE_ERR_unpredictable("Bad dest reg\n");

	return strh_imm(rt, rn, imm32, index, add, wback);
}

void register_opcodes_str(void) {
	// str1: 0110 0<x's>
	register_opcode_mask(0x6000, 0xffff9800, str1);

	// str2: 0101 000<x's>
	register_opcode_mask(0x5000, 0xffffae00, str2);

	// str3: 1001 0<x's>
	register_opcode_mask(0x9000, 0xffff6800, str3);

	// strb1: 0111 0<x's>
	register_opcode_mask(0x7000, 0xffff8800, strb1);

	// str_imm_t4: 1111 1000 0100 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_ex(0xf8400800, 0x07b00000, str_imm_t4,
			0x600, 0x100,
			0xd0500, 0x20200,
			0xf0000, 0x0,
			0x0, 0x500,
			0, 0);

	// strb_imm_t2: 1111 1000 1000 <x's>
	register_opcode_mask(0xf8800000, 0x07700000, strb_imm_t2);

	// strb_imm_t3: 1111 1000 0000 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_ex(0xf8000800, 0x07f00000, strb_imm_t3, 0x600, 0x100, 0xf0000, 0x0, 0x0, 0x500, 0, 0);

	// strd_imm: 1110 100x x1x0 <x's>
	register_opcode_mask(0xe8400000, 0x16100000, strd_imm);

	// strh_imm_t2: 1111 1000 1010 <x's>
	register_opcode_mask_ex(0xf8a00000, 0x07500000, strh_imm_t2, 0x000f0000, 0x0, 0, 0);
}
