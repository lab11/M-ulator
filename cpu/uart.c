#include "uart.h"

#include "memmap.h"
#include "bus_interface.h"

/* Placeholder Code */

static bool uart_read(uint32_t addr, uint32_t *val) {
	*val = CORE_poll_uart_status_read();
	return true;
}

static void uart_write(uint32_t addr, uint32_t val) {
	CORE_poll_uart_status_write(val);
}

__attribute__ ((constructor))
void register_memmap_uart(void) {
	register_memmap_read_word(uart_read, POLL_UART_STATUS, POLL_UART_TXDATA+4);
	register_memmap_write_word(uart_write, POLL_UART_STATUS, POLL_UART_TXDATA+4);
}
