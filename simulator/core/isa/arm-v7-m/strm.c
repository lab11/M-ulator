/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2016  Pat Pannuto <pat.pannuto@gmail.com>
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

#include "core/isa/opcodes.h"
#include "core/isa/decode_helpers.h"

#include "core/operations/strm.h"

// arm-v7-m
static void stmdb_t1(uint32_t inst) {
	uint16_t register_list = inst & 0xfff;
	bool M = (inst >> 14) & 0x1;
	uint8_t rn = (inst >> 16) & 0xf;
	bool W = (inst >> 21) & 0x1;

	uint16_t registers = (M << 13) | register_list;

	if ((rn == 1) || (hamming(registers) < 2))
		CORE_ERR_unpredictable("stmdb_t1 bad register set\n");

	if (W && (registers & (1 << rn)))
		CORE_ERR_unpredictable("stmdb_t1 wback overlaps regs\n");

	OP_DECOMPILE("STMDB<c> <Rn>{!},<registers>", rn, W, registers);
	stmdb(rn, registers, W);
}

// arm-v7-m
static void stm_t2(uint32_t inst) {
	uint16_t register_list = inst & 0xfff;
	bool M = !!(inst & 0x4000);
	uint8_t rn = (inst >> 16) & 0xf;
	bool W = !!(inst & 0x200000);

	uint16_t registers = (M << 13) | register_list;
	bool wback = W;

	if ((rn == 15) || (hamming(registers) < 2))
		CORE_ERR_unpredictable("bad regs\n");

	if (wback && (registers & (1 << rn)))
		CORE_ERR_unpredictable("wback conflict\n");

	OP_DECOMPILE("STM<c>.W <Rn>{!},<registers>",
			rn, wback, registers);
	return stm(rn, registers, wback);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_strm(void) {
	// 1110 1001 00x0 xxxx 0x0x xxxx xxxx xxxx
	//             1  1101
	register_opcode_mask_32_ex(0xe9000000, 0x16d0a000, stmdb_t1,
			0x002d0000, 0x00020000,
			0, 0);

	// stm{ia,ea}_t2: 1110 1000 10x0 xxxx 0x0x xxxx xxxx xxxx
	register_opcode_mask_32(0xe8800000, 0x1750a000, stm_t2);
}
