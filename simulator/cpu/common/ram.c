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

#include MEMMAP_HEADER
#include "ram.h"

////////////////////////////////////////////////////////////////////////
// Only include this peripheral if requested in the platform memmap.h //
#ifdef HAVE_RAM

#include "cpu/core.h"

#include "core/state_sync.h"

#define ADDR_TO_IDX(_addr, _bot) ((_addr - _bot) >> 2)
static uint32_t ram[RAMSIZE >> 2] = {0};

#ifndef FAVOR_SPEED
// Detect attempts to overwrite flashed code (most likely an error)
static uint32_t code_bot = 0;
static uint32_t code_top = 0;
#endif

EXPORT void flash_RAM(const uint8_t *image, int offset, uint32_t nbytes) {
	memcpy(ram+offset, image, nbytes);
#ifndef FAVOR_SPEED
	code_bot = offset;
	code_top = offset+nbytes;
#endif
	INFO("Flashed %d bytes to RAM\n", nbytes);
}

EXPORT size_t dump_RAM(FILE *fp) {
	return fwrite(ram, RAMSIZE, 1, fp);
}

static bool ram_read(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
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

static void ram_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
#ifdef DEBUG1
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_write");
#endif
#ifndef FAVOR_SPEED
	if (code_bot != code_top) {
		if ((addr >= code_bot) && (addr < code_top)) {
			WARN("Attempt to write to address %08x\n", addr);
			WARN("Which is inside flashed code image (%08x-%08x)\n",
					code_bot, code_top);
			WARN("This is almost certainly an error (stack overflow)\n");
		}
	}
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

#endif // HAVE_RAM
// Only include this peripheral if requested in the platform memmap.h //
////////////////////////////////////////////////////////////////////////
