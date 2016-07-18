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

#include "core/operations/cb.h"

// arm-v7-m
static void cbz_t1(uint16_t inst) {
	uint8_t rn = (inst & 0x7);
	uint8_t imm5 = (inst >> 3) & 0x1f;
	bool i = (inst >> 9) & 0x1;
	bool op = (inst >> 11) & 0x1;

	uint32_t imm32 = (i << 6) | (imm5 << 1);
	bool nonzero = op == 1;

	if (in_ITblock())
		CORE_ERR_unpredictable("cb{n}z in IT block\n");

	if (nonzero)
		OP_DECOMPILE("CBNZ <Rn>,<label>", rn, imm32);
	else
		OP_DECOMPILE("CBZ <Rn>,<label>", rn, imm32);
	cbz(nonzero, rn, imm32);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_cb(void) {
	// cb{N}Z: 1011 x0x1 <x's>
	register_opcode_mask_16(0xb100, 0x4400, cbz_t1);
}

