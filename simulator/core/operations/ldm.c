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

static void ldm(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t address = CORE_reg_read(rn);

	int i;
	for (i = 0; i <= 14; i++) {	// stupid arm inclusive for
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}
	if (registers & 0x8000) {
		//LoadWritePC(MemA[address, 4]);
		CORE_ERR_not_implemented("ldm PC");
	}

	if (wback && ((registers & (1 << rn)) == 0)) {
		CORE_reg_write(rn, CORE_reg_read(rn) + 4U*hamming(registers));
	}
}

// arm-thumb
static void ldm_t1(uint16_t inst) {
	uint8_t registers = inst & 0xff;
	uint8_t rn = (inst >> 8) & 0x7;

	bool wback = registers & (1 << rn);

	if (hamming(registers) < 1)
		CORE_ERR_unpredictable("ldm_t1 load no regs?\n");

	return ldm(rn, registers, wback);
}

// arm-v7-m
static void ldm_t2(uint32_t inst) {
	uint16_t reg_list = inst & 0x1fff;
	bool M = !!(inst & 0x4000);
	bool P = !!(inst & 0x8000);
	uint8_t rn = (inst & 0xf0000) >> 16;
	bool W = !!(inst & 0x200000);

	if ((W == 1) && (rn == 0xd))
		CORE_ERR_unpredictable("ldm_t2 --> pop\n");

	uint16_t registers = (P << 15) | (M << 14) | reg_list;
	bool wback = (W == 1);

	if ((rn == 15) || (hamming(registers) < 2) || ((P == 1) && (M == 1)))
		CORE_ERR_unpredictable("ldm_t2, long ||'s\n");

	if ((registers & 0x8000) && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("ldm_t2, itstate stuff\n");

	if (wback && (registers & (1 << (rn))))
		CORE_ERR_unpredictable("ldm_t2, writeback mismatch?\n");

	return ldm(rn, registers, wback);
}

static inline void ldmdb(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t address = CORE_reg_read(rn) - 4*hamming(registers);

	int i;
	for (i=0;i<14;i++) {
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}
	if (registers & (1 << 15)) {
		//LoadWritePC(MemA[address, 4]);
		CORE_ERR_not_implemented("ldmdb PC");
	}

	if (wback && ((registers & (1 << rn)) == 0)) {
		CORE_reg_write(rn, CORE_reg_read(rn) - 4U*hamming(registers));
	}
}

// arm-v7-m
static void ldmdb_t1(uint32_t inst) {
	/* { literal
		uint16_t register_list = inst & 0x1fff;
		uint16_t registers = register_list | (M << 14) | (P << 15);
	} */
	uint16_t registers = inst & 0xdfff;
	bool M = (inst >> 14) & 0x1;
	bool P = (inst >> 15) & 0x1;
	uint8_t rn = (inst >> 16) & 0xf;
	bool W = (inst >> 21) & 0x1;

	bool wback = W==1;

	if ((rn == 15) || (hamming(registers) < 2) || ((P==1) && (M==1)))
		CORE_ERR_unpredictable("ldmdb_t1 case 1\n");
	if (P && in_ITblock() && !last_in_ITblock())
		CORE_ERR_unpredictable("ldmdb_t1 case 2\n");
	if (wback && (registers & (1 << rn)))
		CORE_ERR_unpredictable("ldmdb_t1 case 3\n");

	return ldmdb(rn, registers, wback);
}


__attribute__ ((constructor))
void register_opcodes_ldm(void) {
	// ldm_t1: 1100 1<x's>
	register_opcode_mask_16(0xc800, 0x3000, ldm_t1);

	// LDM_t2: 1110 1000 10x1 xxxx xx0<x's>
	// Illegal:            1  1101
	//register_opcode_mask(0xe8900000, 0x17402000, ldm_t2);
	// enfore W == 0
	register_opcode_mask_32_ex(0xe8900000, 0x17402000, ldm_t2,
			0x002d0000, 0x00020000,
			0, 0);

	// ldmdb_t1: 1110 1001 00x1 xxxx 110x <x's>
	register_opcode_mask_32(0xe910c000, 0x16c02000, ldmdb_t1);
}
