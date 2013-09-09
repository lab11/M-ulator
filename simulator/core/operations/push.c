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

static void push(const uint16_t registers) {
	uint32_t sp = CORE_reg_read(SP_REG);

	uint32_t address = sp - 4 * hamming(registers);

	int i;
	for (i=0; i <= 14; i++) {
		if (registers & (1 << i)) {
			write_word(address, CORE_reg_read(i));
			address += 4;
		}
	}

	CORE_reg_write(SP_REG, sp - 4 * hamming(registers));
}

// arm-thumb
static void push_t1(uint16_t inst) {
	uint8_t register_list = inst & 0xff;
	bool M = (inst >> 8) & 0x1;

	uint16_t registers = register_list | (M << 14);

	if (hamming(registers) < 1)
		CORE_ERR_unpredictable("push_t1 case\n");

	OP_DECOMPILE("PUSH<c> <registers>", registers);
	return push(registers);
}

// arm-v7-m
static void push_t2(uint32_t inst) {
	uint16_t register_list = inst & 0x1fff;
	bool M = (inst >> 14) & 0x1;

	uint16_t registers = (M << 14) | register_list;

	if (hamming(registers) < 2)
		CORE_ERR_unpredictable("Too few regs to push\n");

	OP_DECOMPILE("PUSH<c> <registers>", registers);
	push(registers);
}

// arm-v7-m
static void push_t3(uint32_t inst) {
	uint8_t rt = (inst >> 12) & 0xf;
	uint16_t registers = (1 << rt);

	if (BadReg(rt))
		CORE_ERR_unpredictable("bad reg\n");

	OP_DECOMPILE("PUSH<c> <registers>", registers);
	push(registers);
}

__attribute__ ((constructor))
static void register_opcodes_push(void) {
	// 1011 010x <x's>
	register_opcode_mask_16(0xb400, 0x4a00, push_t1);

	// 1110 1001 0010 1101 0x0x xxxx xxxx xxxx
	register_opcode_mask_32(0xe92d0000, 0x16d2a000, push_t2);

	// 1111 1000 0100 1101 xxxx 1101 0000 0100 (t3)
	register_opcode_mask_32(0xf84d0d04, 0x07b202fb, push_t3);
}
