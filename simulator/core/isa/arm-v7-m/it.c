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

#include "core/operations/it.h"

// arm-v7-m
static void it_t1(uint16_t inst) {
	uint8_t itstate = inst & 0xff;
	OP_DECOMPILE("IT{x{y{z}}}<q> <firstcond>", itstate); // IT -> special
	return it(inst);
}

__attribute__ ((constructor))
static void register_opcodes_arm_v7_m_it(void) {
	// it_t1: 1011 1111 xxxx xxxx
	register_opcode_mask_16_ex(0xbf00, 0x4000, it_t1,
			0x0000, 0x000f,
			0, 0);
}
