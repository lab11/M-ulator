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

