/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "helpers.h"

#include "cpu/registers.h"

/* CMP and TST always write apsr bits, regardless of itstate */

void cmn_imm(uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, imm32, 0, &result, &carry, &overflow);

	apsr.bits.N = HIGH_BIT(result);
	apsr.bits.Z = result == 0;
	apsr.bits.C = carry;
	apsr.bits.V = overflow;
	CORE_apsr_write(apsr);
}

void cmn_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t shifted = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry_out, overflow_out;
	AddWithCarry(rn_val, shifted, 0, &result, &carry_out, &overflow_out);

	apsr.bits.N = HIGH_BIT(result);
	apsr.bits.Z = result == 0;
	apsr.bits.C = carry_out;
	apsr.bits.V = overflow_out;
	CORE_apsr_write(apsr);
}

void cmp_imm(uint8_t rn, uint32_t imm32) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t result;
	bool carry_out;
	bool overflow_out;

	DBG2("rn_val: %08x, ~imm32: %08x\n", rn_val, ~imm32);
	AddWithCarry(rn_val, ~imm32, 1, &result, &carry_out, &overflow_out);
	DBG2("result: %08x, carry: %d, overflow: %d\n",
			result, carry_out, overflow_out);

	union apsr_t apsr = CORE_apsr_read();
	apsr.bits.N = HIGH_BIT(result);
	apsr.bits.Z = result == 0;
	apsr.bits.C = carry_out;
	apsr.bits.V = overflow_out;
	CORE_apsr_write(apsr);
}

void cmp_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);

	union apsr_t apsr = CORE_apsr_read();
	uint32_t shifted = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t result;
	bool carry;
	bool overflow;
	AddWithCarry(rn_val, ~shifted, 1, &result, &carry, &overflow);

	apsr.bits.N = HIGH_BIT(result);
	apsr.bits.Z = result == 0;
	apsr.bits.C = carry;
	apsr.bits.V = overflow;
	CORE_apsr_write(apsr);
}

void teq_imm(uint8_t rn, uint32_t imm32, bool carry) {
	uint32_t result = CORE_reg_read(rn) ^ imm32;
	union apsr_t apsr = CORE_apsr_read();
	apsr.bits.N = HIGH_BIT(result);
	apsr.bits.Z = result == 0;
	apsr.bits.C = carry;
	CORE_apsr_write(apsr);
}

void teq_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted;
	bool carry;
	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C,
			&shifted, &carry);
	uint32_t result = CORE_reg_read(rn) ^ shifted;

	apsr.bits.N = HIGH_BIT(result);
	apsr.bits.Z = result == 0;
	apsr.bits.C = carry;
	CORE_apsr_write(apsr);
}

void tst_imm(uint8_t rn, uint32_t imm32, bool carry) {
	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t result = rn_val & imm32;
	apsr.bits.N = HIGH_BIT(result);
	apsr.bits.Z = result == 0;
	apsr.bits.C = carry;
	CORE_apsr_write(apsr);
}

void tst_reg(uint8_t rn, uint8_t rm, enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t shifted;
	bool carry;
	Shift_C(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C,
			&shifted, &carry);

	uint32_t result = CORE_reg_read(rn) & shifted;
	apsr.bits.N = HIGH_BIT(result);
	apsr.bits.Z = result == 0;
	apsr.bits.C = carry;
	CORE_apsr_write(apsr);
}
