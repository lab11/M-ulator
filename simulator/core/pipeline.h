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

#ifndef PIPELINE_H
#define PIPELINE_H

#include "common.h"

#ifdef M_PROFILE

#define INST_NOP 0x4600U
#define STALL_PC (-1U & 0xfffffffe)

// Find something unused as a sentinel, we choose
// 1101 1110 xxxx xxxx, which is Permanently UNDEFINED
#define INST_HAZARD 0xde00U
#define HAZARD_PC -5U

#endif //M_PROFILE

// PREFETCH <--> IF
extern uint32_t pre_if_PC;

// IF <--> ID
extern uint32_t if_id_PC;
extern uint32_t if_id_inst;

// ID <--> EX
extern uint32_t id_ex_PC;
extern struct op* id_ex_o;
extern uint32_t id_ex_inst;

void register_pipeline_stage(int idx, const char* name, void (*tick_fn) (void),
		int (*pipeline_flush_fn) (void* new_pc));

void pipeline_init(void);
void pipeline_flush_exception_handler(uint32_t new_pc);
void pipeline_stages_tick(void);
void pipeline_stages_tock(void);
#ifdef HAVE_REPLAY
int  pipeline_state_seek(int target);
#endif

void pipeline_exception(uint16_t inst);

#ifndef NO_PIPELINE
void pipeline_thread_run_fn_void(void (*fn) (void));
int pipeline_thread_run_fn_args(int (*fn) (void *), void *args);
#endif

#endif // PIPELINE_H
