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
#include "cpu/core.h"

void str_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + imm32;
	else
		offset_addr = rn_val - imm32;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	uint32_t rt_val = CORE_reg_read(rt);
	write_word_unaligned(address, rt_val);

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

void str_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rt_val = CORE_reg_read(rt);

	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);
	uint32_t address = rn_val + offset;
	uint32_t data = rt_val;
	write_word_unaligned(address, data);
}

void strb_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	DBG2("strb r%02d, [r%02d, #%08x]\n", rt, rn, imm32);

	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rt_val = CORE_reg_read(rt);

	uint32_t offset_addr;
	if (add) {
		offset_addr = rn_val + imm32;
	} else {
		offset_addr = rn_val - imm32;
	}

	uint32_t address;
	if (index) {
		address = offset_addr;
	} else {
		address = rn_val;
	}

	DBG2("address: %08x\n", address);

	write_byte(address, rt_val & 0xff);

	if (wback) {
		CORE_reg_write(rn, offset_addr);
	}

	DBG2("strb_imm ran\n");
}

void strb_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset;
	offset = Shift(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C);
	uint32_t address;
	address = CORE_reg_read(rn) + offset;

	write_byte(address, CORE_reg_read(rt) & 0xff);
}

void strd_imm(uint8_t rt, uint8_t rt2, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t offset_addr;
	if (add) {
		offset_addr = CORE_reg_read(rn) + imm32;
	} else {
		offset_addr = CORE_reg_read(rn) - imm32;
	}

	uint32_t address;
	if (index) {
		address = offset_addr;
	} else {
		address = CORE_reg_read(rn);
	}

	write_word_aligned(address, CORE_reg_read(rt));
	write_word_aligned(address + 4, CORE_reg_read(rt2));

	if (wback) {
		CORE_reg_write(rn, offset_addr);
	}
}

void strh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t rt_val = CORE_reg_read(rt);
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + imm32;
	else
		offset_addr = rn_val - imm32;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	write_halfword_unaligned(address, rt_val & 0xffff);

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

void strh_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset;
	offset = Shift(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C);
	uint32_t address;
	address = CORE_reg_read(rn) + offset;

	write_halfword_unaligned(address, CORE_reg_read(rt) & 0xffff);
}
