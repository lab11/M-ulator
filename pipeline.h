#ifndef PIPELINE_H
#define PIPELINE_H

#include "common.h"

#ifdef M_PROFILE

#define INST_NOP 0x46c0

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

#endif // PIPELINE_H
