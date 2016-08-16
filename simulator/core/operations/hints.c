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

#include "core/state_sync.h"

void dbg(uint8_t option __attribute__ ((unused))) {
	// NOP-compatible hint
}

void nop(void) {
	;
}

void sev(void) {
	CORE_ERR_not_implemented("Send Event");
}

void wfe(void) {
	CORE_ERR_not_implemented("Wait for Event");
}

void wfi(void) {
	state_wait_for_interrupt();
}

void yield(void) {
	CORE_ERR_not_implemented("Yield");
}

