#ifndef MISC_H
#define MISC_H

#include "../common.h"

// These functions you must implemented in misc.c
uint8_t		eval_cond(uint32_t cpsr, uint8_t cond);
uint8_t		read_itstate();
void		write_itstate(uint8_t new_state);
uint8_t		in_ITblock(uint8_t itstate);
void		IT_advance(uint8_t itstate);
uint8_t		last_in_ITblock(uint8_t itstate);

#endif // MISC_H
