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

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "core/common.h"

#ifndef PP_STRING
#define PP_STRING "EXC"
#include "core/pretty_print.h"
#endif

enum ExceptionType {
	Reset = 1,
	NMI = 2,
	HardFault = 3,
	MemManage = 4,
	BusFault = 5,
	UsageFault = 6,
	/* 7-10 reserved */
	SVCall = 11,
	DebugMonitor = 12,
	/* 13 reserved */
	PendSV = 14,
	SysTick = 15,
	/* 16+ --> ExternalInterrupt0+ */
	ExtInt0 = 16,
	ExtInt1 = 17,
	ExtInt2 = 18,
	ExtInt3 = 19,
	MAX_EXCEPTION_TYPE
};

void generic_exception(enum ExceptionType, bool precise,
		uint32_t fault_inst, uint32_t next_inst);
void exception_return(uint32_t exception_return_pc);

void UsageFault_divide_by_0(void);

#endif // EXCEPTION_H
