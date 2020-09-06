/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef MISC_H
#define MISC_H

#include "core/common.h"

#include "cpu/registers.h"

// These functions you must implement in misc.c
uint8_t		eval_cond(union apsr_t apsr, uint8_t cond);
uint8_t		read_itstate(void);
void		write_itstate(uint8_t new_state);
uint8_t		in_ITblock(void);
void		IT_advance(void);
uint8_t		last_in_ITblock(void);

#endif // MISC_H
