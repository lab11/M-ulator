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

#include "cpu/registers.h"
#include "cpu/misc.h"

static void rsb_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags) {
	uint32_t result;
	bool carry;
	bool overflow;

	AddWithCarry(~CORE_reg_read(rn), imm32, 1, &result, &carry, &overflow);

	CORE_reg_write(rd, result);
	if (setflags) {
		union apsr_t apsr = CORE_apsr_read();
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

// arm-thumb
static void rsb_imm_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = 0;

	return rsb_imm(rd, rn, imm32, setflags);
}

// arm-v7-m
static void rsb_imm_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    S    = (inst >> 20) & 0x1;
	bool    i    = (inst >> 26) & 0x1;

	bool setflags = S==1;
	uint32_t imm32 = ThumbExpandImm(imm8 | (imm3 << 8) | (i << 11));

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("rsb_imm_t2 case\n");

	return rsb_imm(rd, rn, imm32, setflags);
}

static inline void rsb_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t shifted = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(~CORE_reg_read(rn), shifted, 1,
			&result, &carry, &overflow);

	CORE_reg_write(rd, result);
	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

// arm-v7-m
static void rsb_reg_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst & 0x3) >> 4;
	uint8_t imm2 = (inst & 0xc) >> 6;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	uint8_t S = !!(inst & 0x100000) >> 20;

	// d = Uint(rd); n = uint(rn); m = uint(rm);
	bool setflags = (S == 1);
	enum SRType shift_t;
	uint8_t shift_n;
	uint8_t imm5 = (imm3 << 2) | imm2;
	DecodeImmShift(type, imm5, &shift_t, &shift_n);

	if (BadReg(rd) || BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("BadReg in rsb_reg_t1\n");

	return rsb_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

static inline void sbc_imm(uint8_t rd, uint8_t rn, bool setflags, uint32_t imm32) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(CORE_reg_read(rn), ~imm32, apsr.bits.C,
			&result, &carry, &overflow);

	CORE_reg_write(rd, result);
	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

static void sbc_imm_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    S    = (inst >> 20) & 0x1;
	bool    i    = (inst >> 26) & 0x1;

	bool setflags = S==1;
	uint32_t imm32 = ThumbExpandImm(imm8 | (imm3 << 8) | (i << 11));

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("sbc_imm_t1 case\n");

	return sbc_imm(rd, rn, setflags, imm32);
}

static void sbc_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted = Shift(CORE_reg_read(rm), 32,
			shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(CORE_reg_read(rn), ~shifted, apsr.bits.C,
			&result, &carry, &overflow);

	CORE_reg_write(rd, result);
	if (setflags) {
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

// arm-thumb
static void sbc_reg_t1(uint16_t inst) {
	uint8_t rdn = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	bool setflags = !in_ITblock();

	return sbc_reg(rdn, rdn, rm, setflags, SRType_LSL, 0);
}

// arm-v7-m
static void sbc_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    S    = (inst >> 20) & 0x1;

	bool setflags = S==1;
	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if (BadReg(rd) || BadReg(rn) || BadReg(rm))
		CORE_ERR_unpredictable("sbc_reg_t2 case\n");

	return sbc_reg(rd, rn, rm, setflags, shift_t, shift_n);
}

static void sub_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags) {
	bool carry;
	bool overflow;
	uint32_t result;

	AddWithCarry(CORE_reg_read(rn), ~imm32, 1, &result, &carry, &overflow);
	CORE_reg_write(rd, result);

	if (setflags) {
		union apsr_t apsr = CORE_apsr_read();
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}

	DBG2("sub_imm ran\n");
}

// arm-thumb
static void sub_imm_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t imm3 = (inst >> 6) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm3;

	return sub_imm(rd, rn, imm32, setflags);
}

// arm-thumb
static void sub_imm_t2(uint16_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rdn = (inst >> 8) & 0x7;

	bool setflags = !in_ITblock();
	uint32_t imm32 = imm8;

	return sub_imm(rdn, rdn, imm32, setflags);
}

// arm-v7-m
static void sub_imm_t3(uint32_t inst) {
	uint8_t imm8 = (inst & 0xff);
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t rn = (inst & 0xf0000) >> 16;
	bool S = !!(inst & 0x100000);
	bool i = !!(inst & 0x04000000);

	uint16_t arg = (i << 11) | (imm3 << 8) | imm8;
	uint32_t imm32 = ThumbExpandImm(arg);

	if ((rd == 13) || ((rd == 15) && (S == 0)) || (rn == 15))
		CORE_ERR_unpredictable("sub_imm_t3 bad regs\n");

	return sub_imm(rd, rn, imm32, S);
}

// arm-v7-m
static void sub_imm_t4(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;
	bool    i    = (inst >> 26) & 0x1;

	bool setflags = false;
	uint32_t imm32 = imm8 | (imm3 << 8) | (i << 11);

	if (BadReg(rd))
		CORE_ERR_unpredictable("sub_imm_t4 case\n");

	return sub_imm(rd, rn, imm32, setflags);
}

static void sub_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n, bool setflags) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted;
	shifted = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, ~shifted, 1, &result, &carry, &overflow);

	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

// arm-thumb
static void sub_reg_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rn = (inst >> 3) & 0x7;
	uint8_t rm = (inst >> 6) & 0x7;

	bool setflags = !in_ITblock();

	return sub_reg(rd, rn, rm, SRType_LSL, 0, setflags);
}

// arm-v7-m
static void sub_reg_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst > 16) & 0xf;
	bool S = !!(inst & 0x100000);

	bool setflags = S;

	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, (imm3 << 2) | imm2, &shift_t, &shift_n);

	if ((rd == 13) || ((rd == 15) && (S == 0)) || (rn == 15) || BadReg(rm))
		CORE_ERR_unpredictable("Bad reg\n");

	return sub_reg(rd, rn, rm, shift_t, shift_n, setflags);
}

static void sub_sp_imm(uint8_t rd, uint32_t imm32, bool setflags) {
	uint32_t sp_val = CORE_reg_read(SP_REG);

	uint32_t result;
	bool carry;
	bool overflow;

	AddWithCarry(sp_val, ~imm32, 1, &result, &carry, &overflow);
	CORE_reg_write(rd, result);

	if (setflags) {
		union apsr_t apsr = CORE_apsr_read();
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
		CORE_apsr_write(apsr);
	}
}

static void sub_sp_imm_t1(uint16_t inst) {
	uint8_t imm7 = inst & 0x7f;

	uint8_t rd = 13;
	bool setflags = false;
	uint32_t imm32 = imm7 << 2;

	OP_DECOMPILE("SUB<c> SP,SP,#<imm7>", imm7);
	return sub_sp_imm(rd, imm32, setflags);
}

static void sub_sp_imm_t2(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool S = !!(inst & 0x100000);
	bool i = !!(inst & 0x04000000);

	bool setflags = S;
	uint32_t imm32 = ThumbExpandImm((i << 11) | (imm3 << 8) | imm8);

	if ((rd == 15) && (S == 0))
		CORE_ERR_unpredictable("bad reg\n");

	return sub_sp_imm(rd, imm32, setflags);
}

static void sub_sp_imm_t3(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool    i    = (inst >> 26) & 0x1;

	bool setflags = false;
	uint32_t imm32 = imm8 | (imm3 << 8) | (i << 11);

	if (BadReg(rd))
		CORE_ERR_unpredictable("sub_sp_imm_t3 case\n");

	return sub_sp_imm(rd, imm32, setflags);
}

static inline void sub_sp_reg(uint8_t rd, uint8_t rm, bool setflags,
		enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted;
	shifted = Shift(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(CORE_reg_read(SP_REG), ~shifted, 1,
			&result, &carry, &overflow);

	CORE_reg_write(rd, result);
	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		apsr.bits.V = overflow;
	}
}

// arm-thumb
static void sub_sp_reg_t1(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t type = (inst >> 4) & 0x3;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	bool    S    = (inst >> 20) & 0x1;

	bool setflags = S==1;
	enum SRType shift_t;
	uint8_t shift_n;
	DecodeImmShift(type, imm2 | (imm3 << 2), &shift_t, &shift_n);

	if ((rd == 13) && ((shift_t != SRType_LSL) || (shift_n > 3)))
		CORE_ERR_unpredictable("sub_sp_reg_t1 case 1\n");
	if ((rd == 15) || BadReg(rm))
		CORE_ERR_unpredictable("sub_sp_reg_t1 case 2\n");

	return sub_sp_reg(rd, rm, setflags, shift_t, shift_n);
}

__attribute__ ((constructor))
void register_opcodes_sub(void) {
	// (pre-UAL name: neg)
	// rsb_imm_t1: 0100 0010 01<x's>
	register_opcode_mask_16(0x4240, 0xbd80, rsb_imm_t1);

	// rsb_imm_t2: 1111 0x01 110x xxxx 0<x's>
	register_opcode_mask_32(0xf1c00000, 0x0a208000, rsb_imm_t2);

	// rsb_reg_t1: 1110 1011 110x xxxx (0)<x's>
	register_opcode_mask_32(0xebc00000, 0x14208000, rsb_reg_t1);

	// sbc_imm_t1: 1111 0x01 011x xxxx 0<x's>
	register_opcode_mask_32(0xf1600000, 0x0a808000, sbc_imm_t1);

	// sbc_reg_t1: 0100 0001 10<x's>
	register_opcode_mask_16(0x4180, 0xbe40, sbc_reg_t1);

	// sbc_reg_t2: 1110 1011 011x xxxx 0<x's>
	register_opcode_mask_32(0xeb600000, 0x14808000, sbc_reg_t2);

	// sub_imm_t1: 0001 111<x's>
	register_opcode_mask_16(0x1e00, 0xe000, sub_imm_t1);

	// sub_imm_t2: 0011 1<x's>
	register_opcode_mask_16(0x3800, 0xc000, sub_imm_t2);

	// sub_imm_t3: 1111 0x01 101x xxxx 0xxx xxxx xxxx xxxx
	register_opcode_mask_32_ex(0xf1a00000, 0x0a408000, sub_imm_t3,
			0x00100f00, 0x0,
			0x000d0000, 0x00020000,
			0, 0);

	// sub_imm_t4: 1111 0x10 1010 xxxx 0<x's>
	register_opcode_mask_32_ex(0xf2a00000, 0x09508000, sub_imm_t4,
			0xf0000, 0x0,
			0xd0000, 0x20000,
			0, 0);

	// sub_reg_t1: 0001 101<x's>
	register_opcode_mask_16(0x1a00, 0xe400, sub_reg_t1);

	// sub_reg_t2: 1110 1011 101x xxxx 0<x's>
	register_opcode_mask_32_ex(0xeba00000, 0x14408000, sub_reg_t2,
			0x100f00, 0x0,
			0xd0000, 0x20000,
			0, 0);

	// sub_sp_imm_t1: 1011 0000 1<x's>
	register_opcode_mask_16(0xb080, 0x4f00, sub_sp_imm_t1);

	// sub_sp_imm_t2: 1111 0x01 101x 1101 0<x's>
	register_opcode_mask_32_ex(0xf1ad0000, 0x0a428000, sub_sp_imm_t2,
			0x100f00, 0x0,
			0, 0);

	// sub_sp_imm_t3: 1111 0x10 1010 1101 0<x's>
	register_opcode_mask_32(0xf2ad0000, 0x09528000, sub_sp_imm_t3);

	// sub_sp_reg_t1: 1110 1011 101x 1101 0<x's>
	register_opcode_mask_32(0xebad0000, 0x14428000, sub_sp_reg_t1);
}
