#ifndef PIPELINE_H
#define PIPELINE_H

#include "common.h"

#ifdef M_PROFILE

#define INST_NOP 0x46c0
#define STALL_PC -1U

// Find something unused as a sentinel, we choose
// 1101 1110 xxxx xxxx, which is Permanently UNDEFINED
#define INST_HAZARD 0xde00
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

void pipeline_flush(uint32_t new_pc);

void* ticker(void *stage_fn) __attribute__ ((noreturn));

void pipeline_exception(uint32_t inst);

#endif // PIPELINE_H
