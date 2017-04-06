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

void rsb_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags) {
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

void rsb_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
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

void sbc_imm(uint8_t rd, uint8_t rn, bool setflags, uint32_t imm32) {
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

void sbc_reg(uint8_t rd, uint8_t rn, uint8_t rm, bool setflags,
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

void sub_imm(uint8_t rd, uint8_t rn, uint32_t imm32, bool setflags) {
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

void sub_reg(uint8_t rd, uint8_t rn, uint8_t rm,
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

void sub_sp_imm(uint8_t rd, uint32_t imm32, bool setflags) {
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

void sub_sp_reg(uint8_t rd, uint8_t rm, bool setflags,
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
