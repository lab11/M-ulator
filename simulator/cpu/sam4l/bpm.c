/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "bpm.h"

#include "cpu/core.h"
#include "cpu/periph.h"


static void sam4l_bpm_reset(void) {
	// TODO Check actual chip behavoir
}

static void print_bpm(void) {
	// TODO Print state of bpm peripheral
}

static bool bpm_read(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
	TODO("Backup Power Manager (BPM) read is NOP: Returning 0 for addr 0x%08x\n", addr);
	*val = 0;
	return true;
}

static void bpm_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	TODO("Backup Power Manager (BPM) write is NOP: 0x%08x = 0x%08x\n", addr, val);
}

__attribute__ ((constructor))
void register_sam4l_bpm_periph(void) {
	register_reset(sam4l_bpm_reset);

	union memmap_fn mem_fn;

	mem_fn.R_fn32 = bpm_read;
	register_memmap("SAM4L BPM", false, 4, mem_fn, BPM_BASE, BPM_BASE+BPM_SIZE);
	mem_fn.W_fn32 = bpm_write;
	register_memmap("SAM4L BPM", true, 4, mem_fn, BPM_BASE, BPM_BASE+BPM_SIZE);

	register_periph_printer(print_bpm);
}
