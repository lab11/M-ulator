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

static inline void dbg(uint8_t option __attribute__ ((unused))) {
	// NOP-compatible hint
}

// arm-v7-m
static void dbg_t1(uint32_t inst) {
	uint8_t option = inst & 0xf;
	return dbg(option);
}

static void nop(void) {
	;
}

// arm-v7-m
static void nop_t1(uint16_t inst __attribute__ ((unused))) {
	return nop();
}

// arm-v7-m
static void nop_t2(uint32_t inst __attribute__ ((unused))) {
	return nop();
}

static void wfi(void) {
	CORE_ERR_not_implemented("Wait For Interrupt");
}

static void wfi_t1(uint16_t inst __attribute__ ((unused))) {
	return wfi();
}

static void wfi_t2(uint32_t inst __attribute__ ((unused))) {
	return wfi();
}

__attribute__ ((constructor))
void register_opcodes_hints(void) {
	// dbg_t1: 1111 0011 1010 1111 1000 0000 1111 xxxx
	register_opcode_mask_32(0xf3af80f0, 0x0c507f00, dbg_t1);

	// nop_t1: 1011 1111 0000 0000
	register_opcode_mask_16(0xbf00, 0x40ff, nop_t1);

	// nop_t2: 1111 0011 1010 1111 1000 0000 0000 0000
	register_opcode_mask_32(0xf3af8000, 0x0c507fff, nop_t2);

	// wfi_t1: 1011 1111 0011 0000
	register_opcode_mask_16(0xbf30, 0x40cf, wfi_t1);

	// wfi_t2: 1111 0011 1010 1111 1000 0000 0000 0011
	register_opcode_mask_32(0xf3af8003, 0x0c507ffc, wfi_t2);
}
