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

#include "cpu/misc.h"

static void it(uint16_t inst) {
	uint8_t itstate = inst & 0xff;
	write_itstate(itstate);

	DBG2("it{xxx} wrote itstate: %02x\n", itstate);
}

__attribute__ ((constructor))
void register_opcodes_it(void) {
	// Complicated encoding...
	// 1011 1111 firstcond mask
	// mask cannot be 0000
	// firstcond cannot be 1111
	// if firstcont is 1110, then hamming(mask) must be 1

	// The only instr we need to match now is bf03
	//register_opcode_mask(0xbf03, 0xffff40fc, it);
	// 1011 1111 <x's>
	register_opcode_mask_16(0xbf00, 0x4000, it);
}
