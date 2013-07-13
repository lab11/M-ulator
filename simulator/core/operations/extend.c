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

static void sxtb(uint8_t rd, uint8_t rm, uint8_t rotation) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t rotated = Shift(rm_val, 32, ROR, rotation, 0);
	int8_t trunc = (int8_t) rotated;
	int32_t signd = (int32_t) trunc;
	CORE_reg_write(rd, signd);
}

// arm-v6-m, arm-v7-m
static void sxtb_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rotation = 0;

	return sxtb(rd, rm, rotation);
}

// arm-v7-m
static void sxtb_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rotate = (inst >> 4) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;

	uint8_t rotation = rotate << 3;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("sxtb_t2 case\n");

	return sxtb(rd, rm, rotation);
}

static void sxth(uint8_t rd, uint8_t rm, uint8_t rotation) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t rotated = Shift(rm_val, 32, ROR, rotation, 0);
	int16_t trunc = (int16_t) rotated;
	int32_t signd = trunc;
	CORE_reg_write(rd, signd);
}

// arm-v6-m, arm-v7-m
static void sxth_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst >> 3) & 0x7;

	uint8_t rotation = 0;
	return sxth(rd, rm, rotation);
}

// arm-v7-m
static void sxth_t2(uint32_t inst) {
	uint8_t rm = inst & 0xf;
	uint8_t rotate = (inst >> 4) & 0x3;
	uint8_t rd = (inst >> 8) & 0xf;

	uint8_t rotation = rotate << 3;

	if (BadReg(rd) || BadReg(rm))
		CORE_ERR_unpredictable("sxth_t2 case\n");

	return sxth(rd, rm, rotation);
}

// XXX: remove attribute after completing implementation
static void uxtb(uint8_t rd, uint8_t rm __attribute__ ((unused)), uint8_t rotation) {
	uint32_t rd_val = CORE_reg_read(rd);

	uint32_t rotated;
	if (rotation != 0) {
		CORE_ERR_not_implemented("uxtb rotation\n");
	} else {
		rotated = rd_val;
	}

	rd_val = rotated & 0xff;
	CORE_reg_write(rd, rd_val);

	DBG2("uxtb wrote r%02d = %08x\n", rd, rd_val);
}

static void uxtb_t1(uint16_t inst) {
	uint8_t rd = inst & 0x7;
	uint8_t rm = (inst & 0x38) >> 3;

	uint8_t rotation = 0;

	return uxtb(rd, rm, rotation);
}

__attribute__ ((constructor))
void register_opcodes_extend(void) {
	// sxtb_t1: 1011 0010 01xx xxxx
	register_opcode_mask_16(0xb240, 0x4d80, sxtb_t1);

	// sxtb_t2: 1111 1010 0100 1111 1111 xxxx 10xx xxxx
	register_opcode_mask_32(0xfa4ff080, 0x05b00040, sxtb_t2);

	// sxth_t1: 1011 0010 00xx xxxx
	register_opcode_mask_16(0xb200, 0x4dc0, sxth_t1);

	// sxth_t2: 1111 1010 0000 1111 1111 xxxx 10xx xxxx
	register_opcode_mask_32(0xfa0ff080, 0x05f00040, sxth_t2);

	// uxtb_t1: 1011 0010 11<x's>
	register_opcode_mask_16(0xb2c0, 0x4d00, uxtb_t1);
}
