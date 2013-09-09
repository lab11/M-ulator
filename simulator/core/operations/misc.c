/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2013  Pat Pannuto <pat.pannuto@gmail.com>
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

static inline void bfc(uint8_t rd, uint8_t msbit, uint8_t lsbit) {
	if (lsbit < msbit)
		CORE_ERR_unpredictable("bfc msbit < lsbit?\n");

	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t mask = ((1 << (msbit+1)) - 1) - ((1 << lsbit) - 1);
	rd_val &= ~mask;
	CORE_reg_write(rd, rd_val);
}

// arm-v7-m
static void bfc_t1(uint32_t inst) {
	uint8_t msb  = inst & 0x1f;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;

	uint8_t msbit = msb;
	uint8_t lsbit = imm2 | (imm3 << 2);

	if (rd > 13)
		CORE_ERR_unpredictable("bfc_t1 case\n");

	OP_DECOMPILE("BFC<c> <Rd>,#<lsb>,#<width>", rd, lsbit, msbit-lsbit+1);
	return bfc(rd, msbit, lsbit);
}

static inline void bfi(uint8_t rd, uint8_t rn, uint8_t msbit, uint8_t lsbit) {
	if (lsbit < msbit)
		CORE_ERR_unpredictable("bfi msbit < lsbit?\n");

	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t mask = ((1 << (msbit+1)) - 1) - ((1 << lsbit) - 1);
	rd_val &= ~mask;
	rd_val |= (rn_val & (mask >> lsbit)) << lsbit;
	CORE_reg_write(rd, rd_val);
}

// arm-v7-m
static void bfi_t1(uint32_t inst) {
	uint8_t msb  = inst & 0x1f;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd   = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn   = (inst >> 16) & 0xf;

	uint8_t msbit = msb;
	uint8_t lsbit = imm2 | (imm3 << 2);

	if ((rd > 13) || (rn == 13))
		CORE_ERR_unpredictable("bfi_t1 case\n");

	OP_DECOMPILE("BFI<c> <Rd>,<Rn>,#<lsb>,#<width>",
			rd, rn, lsbit, msbit-lsbit+1);
	return bfi(rd, rn, msbit, lsbit);
}

static inline void clz(uint8_t rd, uint8_t rm) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t result = __builtin_clz(rm_val);
	CORE_reg_write(rd, result);
}

// arm-v7-m
static void clz_t1(uint32_t inst) {
	uint8_t rm_lsb = inst & 0xf;
	uint8_t rd     = (inst >> 8) & 0xf;
	uint8_t rm_msb = (inst >> 16) & 0xf;

	if (rm_lsb != rm_msb)
		CORE_ERR_unpredictable("clz_t1 rm_lsb != rm_msb\n");

	uint8_t rm = rm_lsb;

	if ((rd > 13) || (rm > 13))
		CORE_ERR_unpredictable("clz_t1 case\n");

	OP_DECOMPILE("CLZ<c> <Rd>,<Rm>", rd, rm);
	return clz(rd, rm);
}

static inline void movt(uint8_t rd, uint16_t imm16) {
	uint32_t rd_val = CORE_reg_read(rd);
	rd_val &= 0x0000ffff;	// clear top bits
	uint32_t wide_imm = imm16;
	rd_val |= (wide_imm << 16);
	CORE_reg_write(rd, rd_val);
}

// arm-v7-m
static void movt_t1(uint32_t inst) {
	uint8_t imm8 = inst & 0xff;
	uint8_t rd = (inst & 0xf00) >> 8;
	uint8_t imm3 = (inst & 0x7000) >> 12;
	uint8_t imm4 = (inst & 0xf0000) >> 16;
	uint8_t i = !!(inst & 0x04000000);

	// d = uint(rd)
	uint16_t imm16 = (imm4 << 12) | (i << 11) | (imm3 << 8) | imm8;

	if (BadReg(rd))
		CORE_ERR_unpredictable("BadReg movt_t1\n");

	OP_DECOMPILE("MOVT<c> <Rd>,#<imm16>", rd, imm16);
	return movt(rd, imm16);
}

static inline void rbit(uint8_t rm, uint8_t rd) {
	CORE_reg_write(rd, reverse_bits(CORE_reg_read(rm)));
}

// arm-v7-m
static void rbit_t1(uint32_t inst) {
	uint8_t rm_lsb = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t rm_msb = (inst >> 16) & 0xf;

	if (rm_lsb != rm_msb)
		CORE_ERR_unpredictable("rm_lsb != rm_msb\n");

	uint8_t rm = rm_lsb;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("rbit_t1 case\n");

	OP_DECOMPILE("RBIT<c> <Rd>,<Rm>", rd, rm);
	return rbit(rm, rd);
}

static void rev(uint8_t rd, uint8_t rm) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t result;
	result = (
			(((rm_val >> 24) & 0xff) << 0) |
			(((rm_val >> 16) & 0xff) << 8) |
			(((rm_val >> 8) & 0xff) << 16) |
			(((rm_val >> 0) & 0xff) << 24)
		 );
	CORE_reg_write(rd, result);
}

// arm-v6-m, arm-v7-m
static void rev_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REV<c> <Rd>,<Rm>", rd, rm);
	return rev(rd, rm);
}

// arm-v7-m
static void rev_t2(uint32_t inst) {
	uint8_t rm_lsb = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t rm_msb = (inst >> 16) & 0xf;

	if (rm_lsb != rm_msb)
		CORE_ERR_unpredictable("rm_lsb != rm_msb\n");

	uint8_t rm = rm_lsb;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("rbit_t1 case\n");

	OP_DECOMPILE("REV.W<c> <Rd>,<Rm>", rd, rm);
	return rev(rd, rm);
}

static void rev16(uint8_t rd, uint8_t rm) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t result;
	result = (
			(((rm_val >> 24) & 0xff) << 16) |
			(((rm_val >> 16) & 0xff) << 24) |
			(((rm_val >> 8) & 0xff) << 0) |
			(((rm_val >> 0) & 0xff) << 8)
		 );
	CORE_reg_write(rd, result);
}

// arm-v6-m, arm-v7-m
static void rev16_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REV16<c> <Rd>,<Rm>", rd, rm);
	return rev16(rd, rm);
}

// arm-v7-m
static void rev16_t2(uint32_t inst) {
	uint8_t rm_lsb = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t rm_msb = (inst >> 16) & 0xf;

	if (rm_lsb != rm_msb)
		CORE_ERR_unpredictable("rm_lsb != rm_msb\n");

	uint8_t rm = rm_lsb;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("rbit_t1 case\n");

	OP_DECOMPILE("REV16<c>.W <Rd>,<Rm>", rd, rm);
	return rev16(rd, rm);
}

static void revsh(uint8_t rd, uint8_t rm) {
	uint32_t rm_val = CORE_reg_read(rm);

	int8_t sbyte = (int8_t) (rm_val & 0xff);
	int32_t sword = (sbyte << 8);
	uint32_t result = (uint32_t) sword;
	result &= ~0xff;
	result |= ((rm_val >> 8) & 0xff);

	CORE_reg_write(rd, result);
}

// arm-v6-m, arm-v7-m
static void revsh_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	OP_DECOMPILE("REVSH<c> <Rd>,<Rm>", rd, rm);
	return revsh(rd, rm);
}

// arm-v7-m
static void revsh_t2(uint32_t inst) {
	uint8_t rm_lsb = inst & 0xf;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t rm_msb = (inst >> 16) & 0xf;

	if (rm_lsb != rm_msb)
		CORE_ERR_unpredictable("rm_lsb != rm_msb\n");

	uint8_t rm = rm_lsb;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("rbit_t1 case\n");

	OP_DECOMPILE("REVSH<c>.W <Rd>,<Rm>", rd, rm);
	return revsh(rd, rm);
}

static inline void sbfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint8_t msbit = lsbit + widthminus1;

	uint32_t result = (rn_val >> lsbit) & ((1 << (msbit - lsbit + 1)) - 1);
	if (rn_val & (1 << msbit))
		result |= ~((1 << (msbit - lsbit + 1)) - 1);

	if (msbit <= 31)
		CORE_reg_write(rd, result);
	else
		CORE_ERR_unpredictable("msb > 31?\n");
}

// arm-v7-m
static void sbfx_t1(uint32_t inst) {
	uint8_t widthm1 = inst & 0x1f;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;

	uint8_t lsbit = (imm3 << 2) | imm2;
	uint8_t widthminus1 = widthm1;

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("SBFX<c> <Rd>,<Rn>,#<lsb>,#<width>",
			rd, rn, lsbit, widthminus1+1);
	return sbfx(rd, rn, lsbit, widthminus1);
}

static inline void ubfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint8_t msbit = lsbit + widthminus1;
	if (msbit <= 31)
		CORE_reg_write(rd, (rn_val >> lsbit) & ((1 << (msbit - lsbit + 1)) - 1));
	else
		CORE_ERR_unpredictable("msb > 31?\n");
}

// arm-v7-m
static void ubfx_t1(uint32_t inst) {
	uint8_t widthm1 = inst & 0x1f;
	uint8_t imm2 = (inst >> 6) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;
	uint8_t imm3 = (inst >> 12) & 0x7;
	uint8_t rn = (inst >> 16) & 0xf;

	uint8_t lsbit = (imm3 << 2) | imm2;
	uint8_t widthminus1 = widthm1;

	if (BadReg(rd) || BadReg(rn))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("UBFX<c> <Rd>,<Rn>,#<lsb>,#<width>",
			rd, rn, lsbit, widthminus1+1);
	return ubfx(rd, rn, lsbit, widthminus1);
}

__attribute__ ((constructor))
static void register_opcodes_misc(void) {
	// bfc_t1: 1111 0011 0110 1111 0xxx xxxx xx0x xxxx
	register_opcode_mask_32(0xf36f0000, 0x0c908020, bfc_t1);

	// bfi_t1: 1111 0011 0110 xxxx 0xxx xxxx xx0x xxxx
	register_opcode_mask_32_ex(0xf3600000, 0x0c908020, bfi_t1,
			0xf0000, 0x0,
			0, 0);

	// clz_t1: 1111 1010 1011 xxxx 1111 xxxx 1000 xxxx
	register_opcode_mask_32(0xfab0f080, 0x05400070, clz_t1);

	// movt_t1: 1111 0x10 1100 xxxx 0<x's>
	register_opcode_mask_32(0xf2c00000, 0x09308000, movt_t1);

	// rbit_t1: 1111 1010 1001 xxxx 1111 xxxx 1010 xxxx
	register_opcode_mask_32(0xfa90f0a0, 0x05600050, rbit_t1);

	// rev_t1: 1011 1010 00xx xxxx
	register_opcode_mask_16(0xba00, 0x45c0, rev_t1);

	// rev_t2: 1111 1010 1001 xxxx 1111 xxxx 1000 xxxx
	register_opcode_mask_32(0xfa90f080, 0x05600070, rev_t2);

	// rev16_t1: 1011 1010 01xx xxxx
	register_opcode_mask_16(0xba40, 0x4580, rev16_t1);

	// rev16_t2: 1111 1010 1001 xxxx 1111 xxxx 1001 xxxx
	register_opcode_mask_32(0xfa90f090, 0x05600060, rev16_t2);

	// revsh_t1: 1011 1010 11xx xxxx
	register_opcode_mask_16(0xbac0, 0x4500, revsh_t1);

	// revsh_t2: 1111 1010 1001 xxxx 1111 xxxx 1011 xxxx
	register_opcode_mask_32(0xfa90f0b0, 0x05600040, revsh_t2);

	// sbfx_t1: 1111 0011 0100 xxxx 0xxx xxxx xx0x xxxx
	register_opcode_mask_32(0xf3400000, 0x0cb08020, sbfx_t1);

	// ubfx_t1: 1111 0011 1100 xxxx 0xxx xxxx xx0x xxxx
	register_opcode_mask_32(0xf3c00000, 0x0c308020, ubfx_t1);
}
