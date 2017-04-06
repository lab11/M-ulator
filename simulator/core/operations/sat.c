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

static void SignedSatQ(int32_t i, uint8_t N, int32_t *result, bool *sat) {
	int64_t extreme = ((1ULL << (N-1)) - 1);

	if (i > extreme) {
		*result = (int32_t) extreme;
		*sat = true;
	} else if (i < -extreme) {
		*result = (int32_t) -extreme;
		*sat = true;
	} else {
		*result = i;
		*sat = false;
	}
}

static void UnsignedSatQ(int32_t i, uint8_t N, uint32_t *result, bool *sat) {
	int64_t extreme = ((1ULL << N) - 1);
	if (i > extreme) {
		*result = (int32_t) extreme;
		*sat = true;
	} else if (i < 0) {
		*result = 0;
		*sat = true;
	} else {
		*result = i;
		*sat = false;
	}
}

void ssat(uint8_t rd, uint8_t rn, uint8_t saturate_to,
		enum SRType shift_t, uint8_t shift_n) {
	int32_t operand;

	// Carry field can be set to 0 instead of APSR.C is the value guaranteed
	// to be ignored as shift_n cannot be 0 in this instruction encoding.
	operand = Shift(CORE_reg_read(rn), 32, shift_t, shift_n, 0 /* see comment */);

	int32_t result;
	bool sat;
	SignedSatQ(operand, saturate_to, &result, &sat);

	CORE_reg_write(rd, result);
	if (sat) {
		union apsr_t apsr = CORE_apsr_read();
		apsr.bits.Q = 1;
		CORE_apsr_write(apsr);
	}
}

void usat(uint8_t rd, uint8_t rn, uint8_t saturate_to,
		enum SRType shift_t, uint8_t shift_n) {
	int32_t operand;

	// Carry field can be set to 0 instead of APSR.C is the value guaranteed
	// to be ignored as shift_n cannot be 0 in this instruction encoding.
	operand = Shift(CORE_reg_read(rn), 32, shift_t, shift_n, 0 /* see comment */);

	uint32_t result;
	bool sat;
	UnsignedSatQ(operand, saturate_to, &result, &sat);

	CORE_reg_write(rd, result);
	if (sat) {
		union apsr_t apsr = CORE_apsr_read();
		apsr.bits.Q = 1;
		CORE_apsr_write(apsr);
	}
}
