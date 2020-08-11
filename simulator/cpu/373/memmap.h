/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef MEMMAP_H
#define MEMMAP_H

#define ROMBOT 0x00000000
#define ROMTOP 0x00008000

#define RAMBOT 0x20000000
#define RAMTOP 0x20008000

#define REDLED 0x40001000
#define GRNLED 0x40001004
#define BLULED 0x40001008

#define POLL_UART_STATUS 0x40004100
#define POLL_UART_RSTBIT 0
#define POLL_UART_RXBIT  1
#define POLL_UART_TXBIT  2
#define POLL_UART_RXDATA 0x40004104
#define POLL_UART_TXDATA 0x40004108

#define GENERIC_GPIO_BASE 0x4004200
#define GENERIC_GPIO_TOP  0x4004220
#define GENERIC_GPIO_ALIGNMENT 4
#define GENERIC_GPIO_CONF_BASE 0x40043000
#define GENERIC_GPIO_INTERRUPT_BASE 16

//#define PRINT_ROM_ENABLE

#endif // MEMMAP_H
