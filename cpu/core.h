#ifndef CORE_H
#define CORE_H

#include "../common.h"

// These functions you must implement in core.c 
void		reset(void);

bool		try_read_word(uint32_t addr, uint32_t *val)
			__attribute__ ((nonnull));
uint32_t	read_word(uint32_t addr);
void		write_word(uint32_t addr, uint32_t val);
uint16_t	read_halfword(uint32_t addr);
void		write_halfword(uint32_t addr, uint16_t val);
bool		try_read_byte(uint32_t addr, uint8_t *val)
			__attribute__ ((nonnull));
uint8_t		read_byte(uint32_t addr);
void		write_byte(uint32_t addr, uint8_t val);

#endif // CORE_H
