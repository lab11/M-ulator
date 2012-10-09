/* Copyright (c) 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
