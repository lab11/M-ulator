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
#include "cpu/misc.h"

void mla(uint8_t rd, uint8_t rn, uint8_t rm, uint8_t ra) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t ra_val = CORE_reg_read(ra);

	uint64_t operand1 = rn_val;
	uint64_t operand2 = rm_val;
	uint64_t addend = ra_val;
	uint64_t result = operand1 * operand2 + addend;

	CORE_reg_write(rd, result & 0xffffffff);
}

void mls(uint8_t rd, uint8_t rn, uint8_t rm, uint8_t ra) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t ra_val = CORE_reg_read(ra);

	uint32_t operand1 = rn_val;
	uint32_t operand2 = rm_val;
	uint32_t addend = ra_val;
	uint32_t result = addend - operand1 * operand2;
	CORE_reg_write(rd, result);
}

void mul(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm) {
	uint32_t result;

	result = CORE_reg_read(rn) * CORE_reg_read(rm);
	CORE_reg_write(rd, result);

	if (setflags) {
		union apsr_t apsr = CORE_apsr_read();
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		CORE_apsr_write(apsr);
	}
}

void smull(uint8_t rdlo, uint8_t rdhi, uint8_t rn, uint8_t rm,
		bool setflags __attribute__ ((unused))) {
	assert(setflags == false);

	int64_t rn_val = CORE_reg_read(rn);
	int64_t rm_val = CORE_reg_read(rm);
	int64_t result = rn_val * rm_val;

	CORE_reg_write(rdhi, (result >> 32) & 0xffffffff);
	CORE_reg_write(rdlo, result & 0xffffffff);
}

void umull(uint8_t rdlo, uint8_t rdhi, uint8_t rn, uint8_t rm,
		bool setflags __attribute__ ((unused))) {
	assert(setflags == false);

	uint64_t rn_val = CORE_reg_read(rn);
	uint64_t rm_val = CORE_reg_read(rm);
	uint64_t result = rn_val * rm_val;

	CORE_reg_write(rdhi, (result >> 32) & 0xffffffff);
	CORE_reg_write(rdlo, result & 0xffffffff);
}
