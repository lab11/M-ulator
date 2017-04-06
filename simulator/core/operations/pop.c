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

void pop(uint16_t registers) {
	uint32_t address = CORE_reg_read(SP_REG);

	int i;
	for (i = 0; i <= 14; i++) {
		if (registers & (1 << i)) {
			CORE_reg_write(i, read_word(address));
			address += 4;
		}
	}

	if (registers & (1 << 15)) {
		LoadWritePC(read_word(address));
	}

	CORE_reg_write(SP_REG, CORE_reg_read(SP_REG) + 4 * hamming(registers));
}
