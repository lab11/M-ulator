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

/***
 *
 * To use the generic gpio interface with your platform, you must:
 *
 * Define GENERIC_GPIO_BASE to the desired address of GPIO 0
 * Define GENERIC_GPIO_TOP  to the would-be address of GPIO N+1 (for N GPIOs)
 * Define GENERIC_GPIO_ALIGNMENT to 1 (byte-access), 2 (halfword-access), or
 *                               3 (word-access) -- This WILL impact GPIO_TOP!
 *
 * The generic gpio interface can either coalesce all gpio lines to a single
 * interrupt or have an individual interrrupt for each gpio. This is controlled
 * by whether or not GENERIC_GPIO_COALESCE_INTERRUPTS is defined. The macro
 * GENERIC_GPIO_INTERRUPT_BASE is the interrupt number that is assigned to gpio
 * number 0 (or the unified interrupt). Subsequent interrupts are assigned
 * sequentially.
 *
 * To override the directory the gpios are placed in, set GENERIC_GPIO_DIRNAME
 *
 ***/

#ifndef GPIO_H
#define GPIO_H

#include "core/common.h"

////////////////////////////////////////////////////////////////////////
// Only include this peripheral if requested in the platform memmap.h //
#ifdef GENERIC_GPIO_BASE

#ifndef GENERIC_GPIO_ALIGNMENT
#error GENERIC_GPIO_ALIGNMENT must be set to 1, 2, or 4
#else
static_assert(
		(GENERIC_GPIO_ALIGNMENT == 1)
		|| (GENERIC_GPIO_ALIGNMENT == 2)
		|| (GENERIC_GPIO_ALIGNMENT == 4)
	     , "GENERIC_GPIO_ALIGNMENT must be set to 1, 2, or 4");
#endif

#define GENERIC_GPIO_COUNT ((GENERIC_GPIO_TOP - GENERIC_GPIO_BASE) / GENERIC_GPIO_ALIGNMENT)

#ifndef GENERIC_GPIO_INTERRUPT_BASE
#error GENERIC_GPIO_INTERRUPT_BASE must be defined
#endif

#ifndef GENERIC_GPIO_DIRNAME
#define GENERIC_GPIO_DIRNAME "generic_gpio"
#endif

#ifndef PP_STRING
#define PP_STRING "GIO"
#include "core/pretty_print.h"
#endif

#endif
// Only include this peripheral if requested in the platform memmap.h //
////////////////////////////////////////////////////////////////////////

#endif // GPIO_H

