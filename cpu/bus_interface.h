#ifndef __BUS_INTERFACE_H
#define __BUS_INTERFACE_H

#include "../common.h"

uint32_t	CORE_rom_read(uint32_t addr);
#ifdef WRITEABLE_ROM
void		CORE_rom_write(uint32_t addr, uint32_t val);
#endif
uint32_t	CORE_ram_read(uint32_t addr);
void		CORE_ram_write(uint32_t addr, uint32_t val);

uint8_t		CORE_poll_uart_status_read();
void		CORE_poll_uart_status_write(uint8_t val);
uint8_t		CORE_poll_uart_rxdata_read();
void		CORE_poll_uart_txdata_write(uint8_t val);

#endif // __BUS_INTERFACE_H
