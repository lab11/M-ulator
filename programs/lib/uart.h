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
