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

#ifndef PP_STRING
#define PP_STRING "COR"
#include "core/pretty_print.h"
#endif

void register_reset(void(*fn)(void));

union memmap_fn {
	bool (*R_fn32)(uint32_t, uint32_t *);
	void (*W_fn32)(uint32_t, uint32_t);
	bool (*R_fn8)(uint32_t, uint8_t *);
	void (*W_fn8)(uint32_t, uint8_t);
};

void register_memmap(
		const char *name,
		bool write,
		short alignment,
		union memmap_fn mem_fn,
		uint32_t bot,
		uint32_t top
	);

// These functions you must implement in core.c 
void		reset(void);

#ifdef DEBUG2
uint32_t	read_word_quiet(uint32_t addr);
#endif
uint32_t	read_word(uint32_t addr);
void		write_word(uint32_t addr, uint32_t val);
uint16_t	read_halfword(uint32_t addr);
void		write_halfword(uint32_t addr, uint16_t val);
uint8_t		read_byte(uint32_t addr);
void		write_byte(uint32_t addr, uint8_t val);

// Exported for gdb
bool		try_read_byte(uint32_t addr, uint8_t *val)
			__attribute__ ((nonnull));

#endif // CORE_H
