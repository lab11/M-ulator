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
#include "rom.h"

////////////////////////////////////////////////////////////////////////
// Only include this peripheral if requested in the platform memmap.h //
#ifdef HAVE_ROM

#include "cpu/core.h"

#include "core/state_sync.h"

#define ADDR_TO_IDX(_addr, _bot) ((_addr - _bot) >> 2)
static uint32_t rom[ROMSIZE >> 2] = {0};

#ifndef FAVOR_SPEED
// Detect attempts to overwrite flashed code (most likely an error)
static uint32_t code_bot = 0;
static uint32_t code_top = 0;
#endif

EXPORT void flash_ROM(const uint8_t *image, int offset, uint32_t nbytes) {
	if ((offset % 4) != 0) {
		CORE_ERR_runtime("ROM flash desination must be word-aligned");
	}
	offset >>= 2;
	memcpy(rom+offset, image, nbytes);
#ifndef FAVOR_SPEED
	code_bot = offset;
	code_top = offset+nbytes;
#endif
	INFO("Flashed %d bytes to ROM\n", nbytes);
}

#ifdef PRINT_ROM_ENABLE
EXPORT size_t dump_ROM(FILE *fp) {
	return fwrite(rom, ROMSIZE, 1, fp);
}
#endif

static bool rom_read(uint32_t addr, uint32_t *val, bool debugger) {
#ifdef DEBUG1
	assert((addr >= ROMBOT) && (addr < ROMTOP) && "CORE_rom_read");
#endif
#ifdef BOOTLOADER_BOT
	if ((addr >= BOOTLOADER_BOT) && (addr < BOOTLOADER_TOP)) {
		if (!debugger) {
			WARN("Attempt to read bootloader region at %08x\n", addr);
			CORE_ERR_invalid_addr(false, addr);
		}
	}
#else
	(void) debugger;
#endif
	if ((addr >= ROMBOT) && (addr < ROMTOP) && (0 == (addr & 0x3))) {
		*val = SR(&rom[ADDR_TO_IDX(addr, ROMBOT)]);
	} else {
		CORE_ERR_invalid_addr(false, addr);
	}

	return true;
}

static void rom_write(uint32_t addr, uint32_t val, bool debugger) {
	if (!debugger) {
		WARN("Attempt to write ROM");
		CORE_ERR_invalid_addr(true, addr);
	}

	DBG2("ROM Write request addr %x (idx: %d)\n", addr, ADDR_TO_IDX(addr, ROMBOT));
#ifdef DEBUG1
	assert((addr >= ROMBOT) && (addr < ROMTOP) && "CORE_rom_write");
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
#ifdef BOOTLOADER_BOT
	if ((addr >= BOOTLOADER_BOT) && (addr < BOOTLOADER_TOP)) {
		WARN("Attempt to write bootloader region at %08x\n", addr);
		CORE_ERR_invalid_addr(true, addr);
	}
#endif
	if ((addr >= ROMBOT) && (addr < ROMTOP) && (0 == (addr & 0x3))) {
		SW(&rom[ADDR_TO_IDX(addr, ROMBOT)],val);
	} else {
		CORE_ERR_invalid_addr(true, addr);
	}
}

__attribute__ ((constructor))
void register_memmap_rom(void) {
#if (ROMBOT > ROMTOP)
#error "ROMBOT cannot be larger than ROMTOP"
#elif (ROMBOT == ROMTOP)
#pragma message ( "ROM of size 0. ROM has been disabled" )
#else
	union memmap_fn mem_fn;

	mem_fn.R_fn32 = rom_read;
	register_memmap("ROM", false, 4, mem_fn, ROMBOT, ROMTOP);
	mem_fn.W_fn32 = rom_write;
	register_memmap("ROM", true, 4, mem_fn, ROMBOT, ROMTOP);
#endif
}

#endif // HAVE_ROM
// Only include this peripheral if requested in the platform memmap.h //
////////////////////////////////////////////////////////////////////////
