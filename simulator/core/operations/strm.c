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

void stmdb(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t address = rn_val - 4 * hamming(registers);

	int i;
	for (i=0; i <= 14; i++) {
		if (registers & (1 << i)) {
			write_word(address, CORE_reg_read(i));
			address += 4;
		}
	}

	if (wback) {
		CORE_reg_write(rn, rn_val - 4 * hamming(registers));
	}
}

void stm(uint8_t rn, uint16_t registers, bool wback) {
	uint32_t rn_val = CORE_reg_read(rn);

	uint32_t address = rn_val;

	int i;
	for (i = 0; i <= 14; i++) {
		if (registers & (1 << i)) {
			if ((i == rn) && wback) {
				CORE_ERR_not_implemented("complicated case\n");
			} else {
				write_word(address, CORE_reg_read(i));
			}
			address += 4;
		}
	}

	if (wback)
		CORE_reg_write(rn, rn_val + 4*hamming(registers));
}
