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

#include "core/operations/hints.h"

// arm-v7-m
static void dbg_t1(uint32_t inst) {
	uint8_t option = inst & 0xf;
	OP_DECOMPILE("DBG<c> #<option>", option);
	return dbg(option);
}

// arm-v7-m
static void nop_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("NOP<c>");
	return nop();
}

// arm-v7-m
static void nop_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("NOP<c>.W");
	return nop();
}

// arm-v7-m
static void sev_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("SEV<c>.W");
	return sev();
}

// arm-v7-m
static void wfe_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("WFE<c>.W");
	return wfe();
}

// arm-v7-m
static void wfi_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("WFI<c>.W");
	return wfi();
}

// arm-v7-m
static void yield_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("YIELD<c>.W");
	return yield();
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_hints(void) {
	// dbg_t1: 1111 0011 1010 1111 1000 0000 1111 xxxx
	register_opcode_mask_32(0xf3af80f0, 0x0c507f00, dbg_t1);

	// nop_t1: 1011 1111 0000 0000
	register_opcode_mask_16(0xbf00, 0x40ff, nop_t1);

	// nop_t2: 1111 0011 1010 1111 1000 0000 0000 0000
	register_opcode_mask_32(0xf3af8000, 0x0c507fff, nop_t2);

	// sev_t2: 1111 0011 1010 1111 1000 0000 0000 0100
	register_opcode_mask_32(0xf3af8004, 0x0c507ffb, sev_t2);

	// wfe_t2: 1111 0011 1010 1111 1000 0000 0000 0010
	register_opcode_mask_32(0xf3af8002, 0x0c507ffd, wfe_t2);

	// wfi_t2: 1111 0011 1010 1111 1000 0000 0000 0011
	register_opcode_mask_32(0xf3af8003, 0x0c507ffc, wfi_t2);

	// yield_t2: 1111 0011 1010 1111 1000 0000 0000 0001
	register_opcode_mask_32(0xf3af8001, 0x0c507ffe, yield_t2);
}

