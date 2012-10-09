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

#include "ram.h"
#include "memmap.h"

#include "cpu/core.h"

#include "core/state_sync.h"

#define ADDR_TO_IDX(_addr, _bot) ((_addr - _bot) >> 2)
static uint32_t ram[RAMSIZE >> 2] = {0};

static bool ram_read(uint32_t addr, uint32_t *val) {
#ifdef DEBUG1
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_read");
#endif
	if ((addr >= RAMBOT) && (addr < RAMTOP) && (0 == (addr & 0x3))) {
		*val = SR(&ram[ADDR_TO_IDX(addr, RAMBOT)]);
	} else {
		CORE_ERR_invalid_addr(false, addr);
	}

	return true;
}

static void ram_write(uint32_t addr, uint32_t val) {
#ifdef DEBUG1
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_write");
#endif
	if ((addr >= RAMBOT) && (addr < RAMTOP) && (0 == (addr & 0x3))) {
		SW(&ram[ADDR_TO_IDX(addr, RAMBOT)],val);
	} else {
		CORE_ERR_invalid_addr(true, addr);
	}
}

__attribute__ ((constructor))
void register_memmap_ram(void) {
#if (RAMBOT > RAMTOP)
#error "RAMBOT cannot be larger than RAMTOP"
#elif (RAMBOT == RAMTOP)
#pragma message ( "RAM of size 0. RAM has been disabled" )
#else
	union memmap_fn mem_fn;

	mem_fn.R_fn32 = ram_read;
	register_memmap("RAM", false, 4, mem_fn, RAMBOT, RAMTOP);
	mem_fn.W_fn32 = ram_write;
	register_memmap("RAM", true, 4, mem_fn, RAMBOT, RAMTOP);
#endif
}
