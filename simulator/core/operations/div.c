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
#include "cpu/exception.h"

//#include "cpu/common/private_peripheral_bus/ppb.h"
//#define DIV_0_TRP (read_word(CONFIGURATION_CONTROL) & CONFIGURATION_CONTROL_DIV_0_TRAP_MASK)
#define DIV_0_TRP 1

void sdiv(uint8_t rd, uint8_t rn, uint8_t rm) {
	int32_t rn_val = CORE_reg_read(rn);
	int32_t rm_val = CORE_reg_read(rm);

	int32_t result;

	if (rm_val == 0) {
		if (DIV_0_TRP)
			return UsageFault_divide_by_0();
		else
			result = 0;
	} else {
		result = rn_val / rm_val;
	}

	CORE_reg_write(rd, result);
}

void udiv(uint8_t rd, uint8_t rn, uint8_t rm) {
	uint32_t rn_val = CORE_reg_read(rn);
	uint32_t rm_val = CORE_reg_read(rm);

	uint32_t result;

	if (rm_val == 0) {
		if (DIV_0_TRP)
			return UsageFault_divide_by_0();
		else
			result = 0;
	} else {
		result = rn_val / rm_val;
	}

	CORE_reg_write(rd, result);
}

