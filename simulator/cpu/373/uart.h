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

#ifndef UART_H
#define UART_H

#include "core/common.h"

#ifndef PP_STRING
#define PP_STRING "URT"
#include "core/pretty_print.h"
#endif

// CONFIGURATION
#define POLL_UART_PORT 4100
//#define POLL_UART_BUFSIZE 16
//#define POLL_UART_BAUD 1200
#define POLL_UART_BUFSIZE 8192
#define POLL_UART_BAUD 9600

#endif // UART_H
