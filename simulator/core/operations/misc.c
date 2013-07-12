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

	return clz(rd, rm);
}

static void movt(uint8_t rd, uint16_t imm16) {
	uint32_t rd_val = CORE_reg_read(rd);
	rd_val &= 0x0000ffff;	// clear top bits
	uint32_t wide_imm = imm16;
	rd_val |= (wide_imm << 16);
	CORE_reg_write(rd, rd_val);

	DBG2("movt r%02d = 0x%08x\n", rd, rd_val);
}

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

	return movt(rd, imm16);
}

static void ubfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint8_t msbit = lsbit + widthminus1;
	if (msbit <= 31)
		CORE_reg_write(rd, (rn_val >> lsbit) & ((1 << (msbit - lsbit + 1)) - 1));
	else
		CORE_ERR_unpredictable("msb > 31?\n");
}

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

	return ubfx(rd, rn, lsbit, widthminus1);
}

__attribute__ ((constructor))
void register_opcodes_misc(void) {
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

	// ubfx_t1: 1111 0011 1100 xxxx 0xxx xxxx xx0x xxxx
	register_opcode_mask_32(0xf3c00000, 0x0c308020, ubfx_t1);
}
