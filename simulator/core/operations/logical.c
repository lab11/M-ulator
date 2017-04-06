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

void and_imm(union apsr_t apsr, uint8_t setflags, uint8_t rd, uint8_t rn,
		uint32_t imm32, uint8_t carry) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result = rn_val & imm32;
	if (rd == 15) {
		// ALUWritePC(result);
		CORE_ERR_not_implemented("ALUWritePC and_imm\n");
	} else {
		CORE_reg_write(rd, result);
		if (setflags) {
			apsr.bits.N = HIGH_BIT(result);
			apsr.bits.Z = result == 0;
			apsr.bits.C = carry;
			CORE_apsr_write(apsr);
		}
	}

	DBG2("and_imm done\n");
}

void and_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted;
	bool carry_out;
	Shift_C(rm_val, 32, shift_t, shift_n, apsr.bits.C, &shifted, &carry_out);

	uint32_t result = rn_val & shifted;
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry_out;
		CORE_apsr_write(apsr);
	}
}

void bic_imm(union apsr_t apsr, uint8_t setflags,
		uint8_t rd, uint8_t rn, uint32_t imm32, uint8_t carry) {
	uint32_t result = CORE_reg_read(rn) & (~imm32);
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		CORE_apsr_write(apsr);
	}
}

void bic_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted;
	bool carry_out;
	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C,
			&shifted, &carry_out);

	uint32_t result = CORE_reg_read(rn) & ~shifted;
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry_out;
		CORE_apsr_write(apsr);
	}
}

void eor_imm(uint8_t rd, uint8_t rn, uint32_t imm32,
		bool carry, bool setflags, union apsr_t apsr) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result = rn_val ^ imm32;
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		CORE_apsr_write(apsr);
	}
}

void eor_reg(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t result;
	bool carry_out;

	union apsr_t apsr = CORE_apsr_read();

	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C, &result, &carry_out);

	result = CORE_reg_read(rn) ^ result;
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry_out;
		CORE_apsr_write(apsr);
	}
}

void mvn_imm(uint8_t rd, bool setflags, union apsr_t apsr,
		uint32_t imm32, bool carry) {
	uint32_t result = ~imm32;
	CORE_reg_write(rd, result);
	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		CORE_apsr_write(apsr);
	}
}

void mvn_reg(uint8_t setflags, uint8_t rd, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t result;
	bool carry_out;

	union apsr_t apsr = CORE_apsr_read();

	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C, &result, &carry_out);

	result = ~result;
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry_out;
		CORE_apsr_write(apsr);
	}
}

void orn_imm(uint8_t rd, uint8_t rn, bool setflags,
		union apsr_t apsr, uint32_t imm32, bool carry) {
	uint32_t result = CORE_reg_read(rn) | ~imm32;
	CORE_reg_write(rd, result);
	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		CORE_apsr_write(apsr);
	}
}

void orn_reg(uint8_t rd, uint8_t rn, uint8_t rm,
		bool setflags, enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted;
	bool carry_out;
	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C,
			&shifted, &carry_out);

	uint32_t result = CORE_reg_read(rn) | ~shifted;
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry_out;
		CORE_apsr_write(apsr);
	}
}

void orr_imm(uint8_t rd, uint8_t rn, bool setflags,
		union apsr_t apsr, uint32_t imm32, bool carry) {
	uint32_t result = CORE_reg_read(rn) | imm32;
	CORE_reg_write(rd, result);
	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry;
		CORE_apsr_write(apsr);
	}
}

void orr_reg(uint8_t setflags, uint8_t rd, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t result;
	bool carry_out;

	union apsr_t apsr = CORE_apsr_read();

	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C, &result, &carry_out);

	result = CORE_reg_read(rn) | result;
	CORE_reg_write(rd, result);

	if (setflags) {
		apsr.bits.N = HIGH_BIT(result);
		apsr.bits.Z = result == 0;
		apsr.bits.C = carry_out;
		CORE_apsr_write(apsr);
	}
}
