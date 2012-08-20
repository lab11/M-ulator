#ifndef MISC_H
#define MISC_H

#include "../common.h"

// These functions you must implement in misc.c
uint8_t		eval_cond(uint32_t cpsr, uint8_t cond);
uint8_t		read_itstate(void);
void		write_itstate(uint8_t new_state);
uint8_t		in_ITblock(void);
void		IT_advance(void);
uint8_t		last_in_ITblock(void);

#endif // MISC_H
