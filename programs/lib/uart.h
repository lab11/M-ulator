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

#ifndef __UART_H
#define __UART_H

#include <stdint.h>

#define POLL_UART_STATUS	0x40004100
#define POLL_UART_RXDATA	0x40004104
#define POLL_UART_TXDATA	0x40004108

#define POLL_UART_RESET		(1 << 0)
#define POLL_UART_RX_PENDING	(1 << 1)
#define POLL_UART_TX_BUSY	(1 << 2)

// Reads one character from the UART
uint8_t	UART_read();

// Writes one character to the UART (cannot fail)
void	UART_write(uint8_t);

// Wrapper to UART_write
void	UART_write_str(const char *);

#endif //__UART_H
