/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "helpers.h"

#include "cpu/registers.h"

void sxtb(uint8_t rd, uint8_t rm, uint8_t rotation) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t rotated = Shift(rm_val, 32, ROR, rotation, 0);
	int8_t trunc = (int8_t) rotated;
	int32_t signd = (int32_t) trunc;
	CORE_reg_write(rd, signd);
}

void sxth(uint8_t rd, uint8_t rm, uint8_t rotation) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t rotated = Shift(rm_val, 32, ROR, rotation, 0);
	int16_t trunc = (int16_t) rotated;
	int32_t signd = trunc;
	CORE_reg_write(rd, signd);
}

void uxtb(uint8_t rd, uint8_t rm, uint8_t rotation) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t rotated = Shift(rm_val, 32, ROR, rotation, 0);
	CORE_reg_write(rd, rotated & 0xff);
}

void uxth(uint8_t rd, uint8_t rm, uint8_t rotation) {
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t rotated = Shift(rm_val, 32, ROR, rotation, 0);
	CORE_reg_write(rd, rotated & 0xffff);
}

