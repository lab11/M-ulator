#include "uart.h"

uint8_t UART_read() {
	volatile uint8_t *uart_status = (uint8_t *) POLL_UART_STATUS;
	volatile uint8_t *uart_rx = (uint8_t *) POLL_UART_RXDATA;

	while (!((*uart_status) & POLL_UART_RX_PENDING))
		;

	return *uart_rx;
}

void UART_write(uint8_t c) {
	volatile uint8_t *uart_status = (uint8_t *) POLL_UART_STATUS;
	volatile uint8_t *uart_tx = (uint8_t *) POLL_UART_TXDATA;

	while ((*uart_status) & POLL_UART_TX_BUSY)
		;

	*uart_tx = c;
}

void UART_write_str(const char *s) {
	while (*s) {
		UART_write(*s);
		s++;
	}
}
