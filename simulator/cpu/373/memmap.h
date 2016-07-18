/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
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
