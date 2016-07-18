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

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/misc.h"

void bfc(uint8_t rd, uint8_t msbit, uint8_t lsbit) {
	if (lsbit < msbit)
		CORE_ERR_unpredictable("bfc msbit < lsbit?\n");

	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t mask = ((1 << (msbit+1)) - 1) - ((1 << lsbit) - 1);
	rd_val &= ~mask;
	CORE_reg_write(rd, rd_val);
}

void bfi(uint8_t rd, uint8_t rn, uint8_t msbit, uint8_t lsbit) {
	if (lsbit < msbit)
		CORE_ERR_unpredictable("bfi msbit < lsbit?\n");

	uint32_t rd_val = CORE_reg_read(rd);
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t mask = ((1 << (msbit+1)) - 1) - ((1 << lsbit) - 1);
	rd_val &= ~mask;
	rd_val |= (rn_val & (mask >> lsbit)) << lsbit;
	CORE_reg_write(rd, rd_val);
}

void clz(uint8_t rd, uint8_t rm) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t result = __builtin_clz(rm_val);
	CORE_reg_write(rd, result);
}

void movt(uint8_t rd, uint16_t imm16) {
	uint32_t rd_val = CORE_reg_read(rd);
	rd_val &= 0x0000ffff;	// clear top bits
	uint32_t wide_imm = imm16;
	rd_val |= (wide_imm << 16);
	CORE_reg_write(rd, rd_val);
}

void rbit(uint8_t rm, uint8_t rd) {
	CORE_reg_write(rd, reverse_bits(CORE_reg_read(rm)));
}

void rev(uint8_t rd, uint8_t rm) {
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

void rev16(uint8_t rd, uint8_t rm) {
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

void revsh(uint8_t rd, uint8_t rm) {
	uint32_t rm_val = CORE_reg_read(rm);

	int8_t sbyte = (int8_t) (rm_val & 0xff);
	int32_t sword = (sbyte << 8);
	uint32_t result = (uint32_t) sword;
	result &= ~0xff;
	result |= ((rm_val >> 8) & 0xff);

	CORE_reg_write(rd, result);
}

void sbfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1) {
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

void ubfx(uint8_t rd, uint8_t rn, uint8_t lsbit, uint8_t widthminus1) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint8_t msbit = lsbit + widthminus1;
	if (msbit <= 31)
		CORE_reg_write(rd, (rn_val >> lsbit) & ((1 << (msbit - lsbit + 1)) - 1));
	else
		CORE_ERR_unpredictable("msb > 31?\n");
}
