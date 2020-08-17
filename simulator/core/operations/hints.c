/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "core/state_sync.h"

void dbg(uint8_t option __attribute__ ((unused))) {
	// NOP-compatible hint
}

void nop(void) {
	;
}

void sev(void) {
	CORE_ERR_not_implemented("Send Event");
}

void wfe(void) {
	CORE_ERR_not_implemented("Wait for Event");
}

void wfi(void) {
	state_wait_for_interrupt();
}

void yield(void) {
	CORE_ERR_not_implemented("Yield");
}

