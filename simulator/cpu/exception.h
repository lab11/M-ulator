/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
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
