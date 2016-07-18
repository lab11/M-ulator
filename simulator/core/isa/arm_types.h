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

#ifndef __ARM_TYPES_H
#define __ARM_TYPES_H

#define INST_SET_ARM		0x0
#define INST_SET_THUMB		0x1
#define INST_SET_JAZELLE	0x2
#define INST_SET_THUMBEE	0x3

#define SRType_LSL	0x0
#define SRType_LSR	0x1
#define SRType_ASR	0x2
#define SRType_RRX	0x3
#define SRType_ROR	0x3

enum SRType {
	LSL,	// match
	LSR,	// match
	ASR,	// match
	RRX,	// match
	ROR,	// +1
};

#endif // __ARM_TYPES_H

