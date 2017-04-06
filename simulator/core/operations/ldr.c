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
#include "cpu/misc.h"

void ldr_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool index, bool add, bool wback) {
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

	uint32_t data = read_word(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	if (rt == 15) {
		if (address & 0x3)
			CORE_ERR_unpredictable("Illegal load of PC in ldr imm\n");
		else
			LoadWritePC(data);
	} else {
		CORE_reg_write(rt, data);
	}
}

void ldr_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	uint32_t data = read_word(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	if (rt == 15) {
		if (address & 0x3)
			CORE_ERR_unpredictable("bad dest pc\n");
		else
			LoadWritePC(data);
	} else {
		CORE_reg_write(rt, data);
	}
}

void ldr_lit(bool add, uint8_t rt, uint32_t imm32) {
	uint32_t base = 0xfffffffc & CORE_reg_read(PC_REG);

	uint32_t addr;
	if (add) {
		addr = base + imm32;
	} else {
		addr = base - imm32;
	}

	uint32_t data = read_word(addr);

	if (rt == 15) {
		if (addr & 0x3) {
			CORE_ERR_unpredictable("ldr_lit misaligned\n");
		} else {
			LoadWritePC(data);
		}
	} else {
		CORE_reg_write(rt, data);
	}
}

void ldrb_imm(uint8_t rt, uint8_t rn, uint32_t imm32, bool add, bool index, bool wback) {
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

	CORE_reg_write(rt, read_byte(address));

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

void ldrb_lit(uint8_t rt, uint32_t imm32, bool add) {
	uint32_t base = CORE_reg_read(PC_REG) & 0xfffffffc;

	uint32_t address;
	if (add)
		address = base + imm32;
	else
		address = base - imm32;

	CORE_reg_write(rt, read_byte(address));
}

void ldrb_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		enum SRType shift_t, uint8_t shift_n,
		bool index, bool add, bool wback __attribute__ ((unused))) {
	assert(wback == false);

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	CORE_reg_write(rt, read_byte(address));
}

void ldrd_imm(uint8_t rt, uint8_t rt2, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t offset_addr;
	if (add) {
		offset_addr = CORE_reg_read(rn) + imm32;
	} else {
		offset_addr = CORE_reg_read(rn) - imm32;
	}

	uint32_t addr;
	if (index) {
		addr = offset_addr;
	} else {
		addr = CORE_reg_read(rn);
	}

	CORE_reg_write(rt, read_word(addr));
	CORE_reg_write(rt2, read_word(addr + 4));

	if (wback) {
		CORE_reg_write(rn, offset_addr);
	}
}

void ldrd_lit(uint8_t rt, uint8_t rt2, uint32_t imm32, bool add) {
	uint32_t pc_val = CORE_reg_read(PC_REG);
	if (pc_val & 0x3)
		CORE_ERR_unpredictable("Unaligned PC in ldrd_lit\n");

	uint32_t address;
	address = (add) ? pc_val + imm32 : pc_val - imm32;

	CORE_reg_write(rt, read_word(address));
	CORE_reg_write(rt2, read_word(address+4));
}

void ldrh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + imm32 : rn_val - imm32;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	uint32_t data;
	data = read_halfword(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	CORE_reg_write(rt, data);
}

void ldrh_lit(uint8_t rt, uint32_t imm32, bool add) {
	uint32_t pc_val = CORE_reg_read(PC_REG);
	uint32_t base = pc_val & ~0x3;
	uint32_t address;
	address = (add) ? base + imm32 : base - imm32;
	uint32_t data = read_halfword(address);
	CORE_reg_write(rt, data);
}

void ldrh_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset;
	offset = Shift(CORE_reg_read(rm), 32, shift_t, shift_n, apsr.bits.C);

	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + offset : rn_val - offset;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	uint32_t data = read_halfword(address);

	if (wback)
		CORE_reg_write(rn, offset_addr);

	CORE_reg_write(rt, data);
}

void ldrsb_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + imm32 : rn_val - imm32;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	int8_t sbyte = (int8_t) read_byte(address);
	int32_t sword = sbyte;
	CORE_reg_write(rt, (uint32_t) sword);

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

void ldrsb_lit(uint8_t rt, uint32_t imm32, bool add) {
	uint32_t pc_val = CORE_reg_read(PC_REG);
	uint32_t base = pc_val & ~0x3;
	uint32_t address;
	address = (add) ? base + imm32 : base - imm32;

	int8_t sbyte = (int8_t) read_byte(address);
	int32_t sword = sbyte;
	CORE_reg_write(rt, (uint32_t) sword);
}

void ldrsb_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback __attribute__ ((unused)),
		enum SRType shift_t, uint8_t shift_n) {
	assert(wback == false);

	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);

	uint32_t offset_addr;
	if (add)
		offset_addr = rn_val + offset;
	else
		offset_addr = rn_val - offset;

	uint32_t address;
	if (index)
		address = offset_addr;
	else
		address = rn_val;

	int32_t signd = (int32_t) read_byte(address);
	CORE_reg_write(rt, signd);
}

void ldrsh_imm(uint8_t rt, uint8_t rn, uint32_t imm32,
		bool index, bool add, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + imm32 : rn_val - imm32;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	int16_t shword = (int16_t) read_halfword(address);
	int32_t sword = shword;
	CORE_reg_write(rt, (uint32_t) sword);

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

void ldrsh_lit(uint8_t rt, uint32_t imm32, bool add) {
	uint32_t pc_val = CORE_reg_read(PC_REG);
	uint32_t base = pc_val & ~0x3;
	uint32_t address;
	address = (add) ? base + imm32 : base - imm32;

	int16_t shword = (int16_t) read_halfword(address);
	int32_t sword = shword;
	CORE_reg_write(rt, (uint32_t) sword);
}

void ldrsh_reg(uint8_t rt, uint8_t rn, uint8_t rm,
		bool index, bool add, bool wback,
		enum SRType shift_t, uint8_t shift_n) {
	uint32_t rm_val = CORE_reg_read(rm);
	uint32_t rn_val = CORE_reg_read(rn);
	union apsr_t apsr = CORE_apsr_read();

	uint32_t offset = Shift(rm_val, 32, shift_t, shift_n, apsr.bits.C);
	uint32_t offset_addr;
	offset_addr = (add) ? rn_val + offset : rn_val - offset;

	uint32_t address;
	address = (index) ? offset_addr : rn_val;

	int16_t shword = (int16_t) read_halfword(address);
	int32_t sword = shword;
	CORE_reg_write(rt, (uint32_t) sword);

	if (wback)
		CORE_reg_write(rn, offset_addr);
}

