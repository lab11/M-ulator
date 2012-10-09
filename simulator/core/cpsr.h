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

#ifndef __CPSR_H
#define __CPSR_H

#define MODE_MASK	0x0000001f
#define T_MASK		0x00000020
#define F_MASK		0x00000040
#define I_MASK		0x00000080
#define A_MASK		0x00000100
#define E_MASK		0x00000200
#define IT_HIGH_MASK	0x0000fc00
#define GE_MASK		0x000f0000
// RESERVED		0x00f00000
#define J_MASK		0x01000000
#define IT_LOW_MASK	0x06000000
#define Q_MASK		0x08000000
#define V_MASK		0x10000000
#define C_MASK		0x20000000
#define Z_MASK		0x40000000
#define N_MASK		0x80000000


#endif //__CPSR_H
