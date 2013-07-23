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

#ifndef DIRECT_STATE_H_CHECK
#error "Do not #include state.h directly, rather use state_sync or state_async"
#endif

#ifndef STATE_H
#define STATE_H

#include "common.h"

#ifndef PP_STRING
#define PP_STRING "ST-"
#include "pretty_print.h"
#endif

enum stage {
	PRE  = 0x1,
	IF   = 0x2,
	ID   = 0x4,
	EX   = 0x8,
	PIPE = 0x10,
	SIM  = 0x20,
	PERIPH = 0x40,
	UNK  = 0x80,
	/* MAX: 0xff */
};

#ifndef STAGE
#pragma message "Assigning UNK stage"
#define STAGE UNK
#endif

// Not 100% sure this is appropriate here, but no callers of this yet
// XXX: Moved in core refactor - consider final placement
void stall(enum stage);

bool state_is_debugging(void);
void state_start_tick(void);
int state_tock(void);
#ifndef NO_PIPELINE
void state_pipeline_flush(uint32_t new_pc);
#endif

void state_async_block_start(void);
void state_async_block_end(void);

void state_enter_debugging(void);
void state_exit_debugging(void);

#endif // STATE_H
