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

#include "opcodes.h"
#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/core.h"
#include "cpu/misc.h"

static void pop_simple(uint16_t inst) {
	uint32_t sp = CORE_reg_read(SP_REG);

	int hamming = 0;
	{
		int reg_list = inst & 0xff;
		for (hamming = 0; reg_list; hamming++) {
			reg_list &= (reg_list - 1); // clears LSB
		}
		hamming += !!(inst & 0x100); // PC?

		assert((hamming > 0) && (hamming <= 9));
	}

	uint32_t start_address = sp;
	uint32_t end_address = sp + 4*hamming;
	uint32_t address = start_address;

	int i;
	for (i = 0; i < 8; i++) {
		if (inst & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}
	if (inst & 0x100) {
		uint32_t val = read_word(address);
		// val &= 0xfffffffe	// simulator allows thumb bit
		CORE_reg_write(PC_REG, val);
		address += 4;
	}

	assert ((end_address == address) && "Sanity check");
	CORE_reg_write(SP_REG, end_address);

	DBG2("pop {%sregisters %02x (bitwise)}\n",
			(inst & 0x100)?"PC and ":"", inst & 0xff);
}

static void pop(uint16_t registers) {
	uint32_t address = CORE_reg_read(SP_REG);

	int i;
	for (i = 0; i <= 14; i++) {
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}

	if (registers & (1 << 15)) {
		LoadWritePC(read_word(address));
	}

	CORE_reg_write(SP_REG, CORE_reg_read(SP_REG) + 4 * hamming(registers));

	DBG2("pop did stuff\n");
}

static void pop_t2(uint32_t inst) {
	uint16_t reg_list = inst & 0x1fff;
	uint8_t M = !!(inst & 0x4000);
	uint8_t P = !!(inst & 0x8000);

	uint16_t registers = (P << 15) | (M << 14) | reg_list;

	DBG2("P %d M %d reg_list %04x, registers %04x\n", P, M, reg_list, registers);

	if ((hamming(registers) < 2) || ((P == 1) && (M == 1)))
		CORE_ERR_unpredictable("pop_t2 not enough regs\n");

	if ((registers & 0x8000) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("pop_t2 itstate\n");

	return pop(registers);
}

static void pop_t3(uint32_t inst) {
	uint8_t rt = (inst >> 12) & 0xf;
	uint16_t registers = (1 << rt);

	if ((rt == 13) || ((rt == 15) && in_ITblock() && !last_in_ITblock()))
		CORE_ERR_unpredictable("bad reg\n");

	return pop(registers);
}

__attribute__ ((constructor))
void register_opcodes_pop(void) {
	// pop: 1011 110<x's>
	register_opcode_mask_16(0xbc00, 0x4200, pop_simple);

	// pop_t2: 1110 1000 1011 1101 xx0<x's>
	register_opcode_mask_32(0xe8bd0000, 0x17422000, pop_t2);

	// pop_t3: 1111 1000 0101 1101 xxxx 1011 0000 0100
	register_opcode_mask_32(0xf85d0b04, 0x07a204fb, pop_t3);
}
