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

#include "helpers.h"

#include "cpu/registers.h"
#include "cpu/core.h"
#include "cpu/misc.h"

void ldm(uint8_t rn, uint16_t registers, bool wback) {
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

void ldmdb(uint8_t rn, uint16_t registers, bool wback) {
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
