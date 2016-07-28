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

#include "pm.h"

#include "cpu/core.h"
#include "cpu/periph.h"


static void sam4l_pm_reset(void) {
	// TODO Check actual chip behavoir
}

static void print_pm(void) {
	// TODO Print state of pm peripheral
}

static bool pm_read(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
	TODO("Power Manager (PM) read is NOP: Returning 0 for addr 0x%08x\n", addr);
	*val = 0;
	return true;
}

static void pm_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	TODO("Power Manager (PM) write is NOP: 0x%08x = 0x%08x\n", addr, val);
}

__attribute__ ((constructor))
void register_sam4l_pm_periph(void) {
	register_reset(sam4l_pm_reset);

	union memmap_fn mem_fn;

	mem_fn.R_fn32 = pm_read;
	register_memmap("SAM4L PM", false, 4, mem_fn, PM_BASE, PM_BASE+PM_SIZE);
	mem_fn.W_fn32 = pm_write;
	register_memmap("SAM4L PM", true, 4, mem_fn, PM_BASE, PM_BASE+PM_SIZE);

	register_periph_printer(print_pm);
}
