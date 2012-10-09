#ifndef __CPU_H
#define __CPU_H

#include "core/common.h"

uint32_t	CORE_reg_read(int r);
void		CORE_reg_write(int r, uint32_t val);
uint32_t	CORE_cpsr_read(void);
void		CORE_cpsr_write(uint32_t val);
uint32_t	CORE_ipsr_read(void);		// Don't need this yet
void		CORE_ipsr_write(uint32_t val);	// Don't need this yet
uint32_t	CORE_epsr_read(void);
void		CORE_epsr_write(uint32_t val);

#ifdef M_PROFILE

#define T_BIT		(CORE_epsr_read() & 0x01000000)

#endif // M_PROFILE

#endif // __CPU_H
