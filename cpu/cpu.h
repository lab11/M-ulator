#ifndef __CPU_H
#define __CPU_H

#include "../common.h"

uint32_t	CORE_reg_read(int r);
void		CORE_reg_write(int r, uint32_t val);
uint32_t	CORE_cpsr_read(void);
void		CORE_cpsr_write(uint32_t val);
uint32_t	CORE_ipsr_read(void);		// Don't need this yet
void		CORE_ipsr_write(uint32_t val);	// Don't need this yet
uint32_t	CORE_epsr_read(void);
void		CORE_epsr_write(uint32_t val);

#endif // __CPU_H
