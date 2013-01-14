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

#ifndef MISC_H
#define MISC_H

#include "core/common.h"

#include "cpu/registers.h"

// These functions you must implement in misc.c
uint8_t		eval_cond(union apsr_t apsr, uint8_t cond);
uint8_t		read_itstate(void);
void		write_itstate(uint8_t new_state);
uint8_t		in_ITblock(void);
void		IT_advance(void);
uint8_t		last_in_ITblock(void);

#endif // MISC_H
