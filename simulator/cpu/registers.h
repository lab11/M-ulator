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

#ifndef REGISTERS_H
#define REGISTERS_H

#include "core/common.h"

#ifndef PP_STRING
#define PP_STRING "REG"
#include "core/pretty_print.h"
#endif

uint32_t	CORE_reg_read(int r);
void		CORE_reg_write(int r, uint32_t val);
uint32_t	CORE_cpsr_read(void);
void		CORE_cpsr_write(uint32_t val);
uint32_t	CORE_ipsr_read(void);		// Don't need this yet
void		CORE_ipsr_write(uint32_t val);	// Don't need this yet
uint32_t	CORE_epsr_read(void);
void		CORE_epsr_write(uint32_t val);

#ifdef A_PROFILE
#define	T_BIT		(CPSR & 0x0020)
#endif // A_PROFILE

#ifdef M_PROFILE
#define T_BIT		(CORE_epsr_read() & 0x01000000)
#endif // M_PROFILE

#endif // REGISTERS_H

