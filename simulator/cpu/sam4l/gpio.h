/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2016  Pat Pannuto <pat.pannuto@gmail.com>
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

#ifndef GPIO_H
#define GPIO_H

#include "core/common.h"

#ifndef PP_STRING
#define PP_STRING "GIO"
#include "core/pretty_print.h"
#endif

#define GPIO_BASE      0x400E1000
#define GPIO_PORT_SIZE     0x0200

// Simplified
#define GPIO_PORT_COUNT 4

#define GPIO_END (GPIO_BASE + GPIO_PORT_COUNT * GPIO_PORT_SIZE)

#endif // GPIO_H
