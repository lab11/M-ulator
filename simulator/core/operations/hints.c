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
	OP_DECOMPILE("DBG<c> #<option>", option);
	return dbg(option);
}

static void nop(void) {
	;
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

static void sev(void) {
	CORE_ERR_not_implemented("Send Event");
}

// arm-v6-m, arm-v7-m
static void sev_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("SEV<c>");
	return sev();
}

// arm-v7-m
static void sev_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("SEV<c>.W");
	return sev();
}

static void wfe(void) {
	CORE_ERR_not_implemented("Wait for Event");
}

// arm-v6-m, arm-v7-m
static void wfe_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("WFE<c>");
	return wfe();
}

// arm-v7-m
static void wfe_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("WFE<c>.W");
	return wfe();
}

static void wfi(void) {
	CORE_ERR_not_implemented("Wait For Interrupt");
}

// arm-v6-m, arm-v7-m
static void wfi_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("WFI<c>");
	return wfi();
}

// arm-v7-m
static void wfi_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("WFI<c>.W");
	return wfi();
}

static void yield(void) {
	CORE_ERR_not_implemented("Yield");
}

// arm-v6-m, arm-v7-m
static void yield_t1(uint16_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("YIELD<c>");
	return yield();
}

// arm-v7-m
static void yield_t2(uint32_t inst __attribute__ ((unused))) {
	OP_DECOMPILE("YIELD<c>.W");
	return yield();
}

__attribute__ ((constructor))
void register_opcodes_hints(void) {
	// dbg_t1: 1111 0011 1010 1111 1000 0000 1111 xxxx
	register_opcode_mask_32(0xf3af80f0, 0x0c507f00, dbg_t1);

	// nop_t1: 1011 1111 0000 0000
	register_opcode_mask_16(0xbf00, 0x40ff, nop_t1);

	// nop_t2: 1111 0011 1010 1111 1000 0000 0000 0000
	register_opcode_mask_32(0xf3af8000, 0x0c507fff, nop_t2);

	// sev_t1: 1011 1111 0100 0000
	register_opcode_mask_16(0xbf40, 0x40bf, sev_t1);

	// sev_t2: 1111 0011 1010 1111 1000 0000 0000 0100
	register_opcode_mask_32(0xf3af8004, 0x0c507ffb, sev_t2);

	// wfe_t1: 1011 1111 0010 0000
	register_opcode_mask_16(0xbf20, 0x40df, wfe_t1);

	// wfe_t2: 1111 0011 1010 1111 1000 0000 0000 0010
	register_opcode_mask_32(0xf3af8002, 0x0c507ffd, wfe_t2);

	// wfi_t1: 1011 1111 0011 0000
	register_opcode_mask_16(0xbf30, 0x40cf, wfi_t1);

	// wfi_t2: 1111 0011 1010 1111 1000 0000 0000 0011
	register_opcode_mask_32(0xf3af8003, 0x0c507ffc, wfi_t2);

	// yield_t1: 1011 1111 0001 0000
	register_opcode_mask_16(0xbf10, 0x40ef, yield_t1);

	// yield_t2: 1111 0011 1010 1111 1000 0000 0000 0001
	register_opcode_mask_32(0xf3af8001, 0x0c507ffe, yield_t2);
}
