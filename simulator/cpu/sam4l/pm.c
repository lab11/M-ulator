/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "pm.h"

#include "cpu/core.h"
#include "cpu/periph.h"


static void sam4l_pm_reset(void) {
	// TODO Check actual chip behavoir
}

static void print_pm(void) {
	// TODO Print state of pm peripheral
}

static bool pm_read(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
	TODO("Power Manager (PM) read is NOP: Returning 0 for addr 0x%08x\n", addr);
	*val = 0;
	return true;
}

static void pm_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	TODO("Power Manager (PM) write is NOP: 0x%08x = 0x%08x\n", addr, val);
}

__attribute__ ((constructor))
void register_sam4l_pm_periph(void) {
	register_reset(sam4l_pm_reset);

	union memmap_fn mem_fn;

	mem_fn.R_fn32 = pm_read;
	register_memmap("SAM4L PM", false, 4, mem_fn, PM_BASE, PM_BASE+PM_SIZE);
	mem_fn.W_fn32 = pm_write;
	register_memmap("SAM4L PM", true, 4, mem_fn, PM_BASE, PM_BASE+PM_SIZE);

	register_periph_printer(print_pm);
}
