/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "opcodes.h"
#include "helpers.h"

#include "cpu/cpu.h"
#include "cpu/core.h"

static void str1(uint16_t inst) {
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

static void str3(uint16_t inst) {
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

static void str_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
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

static void str_imm_t3(uint32_t inst) {
	uint16_t imm12 = inst & 0xfff;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	uint32_t imm32 = imm12;
	bool index = true;
	bool add = true;
	bool wback = false;

	if (rt == 15)
		CORE_ERR_unpredictable("bad reg\n");

	return str_imm(rt, rn, imm32, index, add, wback);
}

static void str_imm_t4(uint32_t inst) {
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

static void str_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rt_val = CORE_reg_read(rt);

	uint32_t cpsr = CORE_cpsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, !!(cpsr & xPSR_C));
	uint32_t address = rn_val + offset;
	uint32_t data = rt_val;
	write_word(address, data);
}

static void str_reg_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	enum SRType shift_t = LSL;
	uint8_t shift_n = 0;

	return str_reg(rt, rn, rm, shift_t, shift_n);
}

static void str_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t imm2 = (inst >> 4) & 0x3;
	uint8_t rt = (inst >> 12) & 0xf;
	uint8_t rn = (inst >> 16) & 0xf;

	enum SRType shift_t = LSL;
	uint8_t shift_n = imm2;

	if ((rt == 15) || BadReg(rm))
		CORE_ERR_unpredictable("bad reg\n");

	return str_reg(rt, rn, rm, shift_t, shift_n);
}

static void strb_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	DBG2("strb r%02d, [r%02d, #%08x]\n", rt, rn, imm32);

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

	DBG2("address: %08x\n", address);

	write_byte(address, rt_val & 0xff);

	if (wback) {
		CORE_reg_write(rn, offset_addr);
	}

	DBG2("strb_imm ran\n");
}

static void strb_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5;
	bool index = true;
	bool add = true;
	bool wback = false;

	return strb_imm(rt, rn, imm32, index, add, wback);
}

static void strb_imm_t2(uint32_t inst) {
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

static void strb_imm_t3(uint32_t inst) {
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

static void strd_imm(uint32_t inst) {
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

static void strh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
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

static void strh_imm_t1(uint16_t inst) {
	uint8_t rt = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm5 = (inst >> 6) & 0x1f;

	uint32_t imm32 = imm5 << 1;

	bool index = true;
	bool add = true;
	bool wback = false;

	return strh_imm(rt, rn, imm32, index, add, wback);
}

static void strh_imm_t2(uint32_t inst) {
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

__attribute__ ((constructor))
void register_opcodes_str(void) {
	// str1: 0110 0<x's>
	register_opcode_mask_16(0x6000, 0x9800, str1);

	// str3: 1001 0<x's>
	register_opcode_mask_16(0x9000, 0x6800, str3);

	// str_imm_t3: 1111 1000 1100 xxxx xxxx xxxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8c00000, 0x07300000, str_imm_t3,
			0xf0000, 0x0,
			0, 0);

	// str_imm_t4: 1111 1000 0100 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8400800, 0x07b00000, str_imm_t4,
			0x600, 0x100,
			0xd0500, 0x20200,
			0xf0000, 0x0,
			0x0, 0x500,
			0, 0);

	// str_reg_t1: 0101 000x xxxx xxxx
	register_opcode_mask_16(0x5000, 0xae00, str_reg_t1);

	// str_reg_t2: 1111 1000 0100 xxxx xxxx 0000 00xx xxxx
	register_opcode_mask_32_ex(0xf8400000, 0x07b00fc0, str_reg_t2,
			0xf0000, 0x0,
			0, 0);

	// strb_imm_t1: 0111 0<x's>
	register_opcode_mask_16(0x7000, 0x8800, strb_imm_t1);

	// strb_imm_t2: 1111 1000 1000 <x's>
	register_opcode_mask_32(0xf8800000, 0x07700000, strb_imm_t2);

	// strb_imm_t3: 1111 1000 0000 xxxx xxxx 1xxx xxxx xxxx
	register_opcode_mask_32_ex(0xf8000800, 0x07f00000, strb_imm_t3,
			0x600, 0x100,
			0xf0000, 0x0,
			0x0, 0x500,
			0, 0);

	// strd_imm: 1110 100x x1x0 <x's>
	register_opcode_mask_32(0xe8400000, 0x16100000, strd_imm);

	// strh_imm_t1: 1000 0<x's>
	register_opcode_mask_16(0x8000, 0x7800, strh_imm_t1);

	// strh_imm_t2: 1111 1000 1010 <x's>
	register_opcode_mask_32_ex(0xf8a00000, 0x07500000, strh_imm_t2,
			0x000f0000, 0x0,
			0, 0);
}
