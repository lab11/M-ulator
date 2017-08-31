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

#ifndef CORE_H
#define CORE_H

#include "core/common.h"

#include <stdbool.h>

#ifndef PP_STRING
#define PP_STRING "COR"
#include "core/pretty_print.h"
#endif

/* in case some compilers don't recognize this keyword */
#ifndef _Atomic
#warning "Skipping _Atomic"
#define _Atomic 
#endif 

void register_reset(void(*fn)(void));

union memmap_fn {
	bool (*R_fn32)(uint32_t, uint32_t *, bool);
	void (*W_fn32)(uint32_t, uint32_t, bool);
	bool (*R_fn8)(uint32_t, uint8_t *, bool);
	void (*W_fn8)(uint32_t, uint8_t, bool);
};

void register_memmap(
		const char *name,
		bool write,
		short alignment,
		union memmap_fn mem_fn,
		uint32_t bot,
		uint32_t top
	);


extern _Atomic _Bool	_CORE_in_reset;
void		reset(void);

uint32_t	read_word_quiet(uint32_t addr);
uint32_t	read_word(uint32_t addr);
void		write_word(uint32_t addr, uint32_t val);
void		write_word_aligned(uint32_t addr, uint32_t val);
void		write_word_unaligned(uint32_t addr, uint32_t val);
uint16_t	read_halfword(uint32_t addr);
void		write_halfword(uint32_t addr, uint16_t val);
void		write_halfword_unaligned(uint32_t addr, uint16_t val);
uint8_t		read_byte(uint32_t addr);
void		write_byte(uint32_t addr, uint8_t val);

bool		gdb_read_byte(uint32_t addr, uint8_t *val)
			__attribute__ ((nonnull));
void		gdb_write_byte(uint32_t addr, uint8_t val);

#ifdef HAVE_MEMTRACE
extern int memtrace_flag;
#define MEMTRACE_READ(_width, _addr, _val) do {\
	if (memtrace_flag) {\
		printf("MEMTR:  READ 0x%08x -> 0x%08x, %d\n", _addr, _val, _val);\
	}\
} while (0);
#define MEMTRACE_READ_ERR(_width, _addr) do {\
	if (memtrace_flag) {\
		printf("MEMTR:  READ 0x%08x -> !! ERROR !!\n", _addr);\
	}\
} while (0);
#define MEMTRACE_WRITE(_width, _addr, _val) do {\
	if (memtrace_flag) {\
		printf("MEMTR: WRITE 0x%08x <- 0x%08x, %d\n", _addr, _val, _val);\
	}\
} while (0);
#define MEMTRACE_WRITE_ERR(_width, _addr, _val) do {\
	if (memtrace_flag) {\
		printf("MEMTR: WRITE 0x%08x <- !! ERROR !!\n", _addr);\
	}\
} while (0);
#else
#define MEMTRACE_READ(...)
#define MEMTRACE_READ_ERR(...)
#define MEMTRACE_WRITE(...)
#define MEMTRACE_WRITE_ERR(...)
#endif

extern unsigned core_stats_unaligned_cycle_penalty;

#endif // CORE_H
