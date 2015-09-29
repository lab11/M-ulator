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
 * Define GENERIC_GPIO_CONF_BASE to desired address of GPIO 0 config register
 * Generic GPIO configuration provides 4 options:
 *   - OUTPUT_EN    - Sets the gpio to output mode, driving the set value
 *   - INT_EDG0_EN  - Enables falling-edge triggered interrupts
 *   - INT_EDG1_EN  - Enables rising-edge triggered interrupts
 *   - INT_EDGX_EN  - Alias for EDG0_EN | EDG1_EN
 *   - INT_LVL0_EN  - Enables low level-triggered interrupts
 *   - INT_LVL1_EN  - Enables high level-triggered interrupts
 *   - PULLUP_EN    - Enables an internal pull-up resistor
 *   - PULLDOWN_EN  - Enables an internal pull-down resistor
 * Illegal combinations (e.g. pull-up and pull-down enabled) or ambiguous
 * combinations (e.g. edg0 and lvl0 enabled, which fires first?) are undefined.
 * All configuration options default to disabled.
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

#ifndef GENERIC_GPIO_H
#define GENERIC_GPIO_H

#ifndef UGLY_EXTERN_INCLUDE_HACK
#include "core/common.h"
#endif

////////////////////////////////////////////////////////////////////////
// Only include this peripheral if requested in the platform memmap.h //
#ifdef GENERIC_GPIO_BASE

#ifndef GENERIC_GPIO_ALIGNMENT
 #error GENERIC_GPIO_ALIGNMENT must be set to 1, 2, or 4
#else
#ifndef UGLY_EXTERN_INCLUDE_HACK
_Static_assert(
		(GENERIC_GPIO_ALIGNMENT == 1)
		|| (GENERIC_GPIO_ALIGNMENT == 2)
		|| (GENERIC_GPIO_ALIGNMENT == 4)
	     , "GENERIC_GPIO_ALIGNMENT must be set to 1, 2, or 4");
#endif
 #if GENERIC_GPIO_ALIGNMENT == 4
  typedef uint32_t gpio_align_t;
 #elif GENERIC_GPIO_ALIGNMENT == 2
  typedef uint16_t gpio_align_t;
 #elif GENERIC_GPIO_ALIGNMENT == 1
  typedef uint8_t gpio_align_t;
 #else
  #error Invalid GENERIC_GPIO_ALIGNMENT
 #endif
#endif

#define GENERIC_GPIO_COUNT ((GENERIC_GPIO_TOP - GENERIC_GPIO_BASE) / GENERIC_GPIO_ALIGNMENT)

#ifndef GENERIC_GPIO_CONF_BASE
#error GENERIC_GPIO_CONF_BASE must be defined
#endif

#define GENERIC_GPIO_CONF_TOP (GENERIC_GPIO_CONF_BASE + (GENERIC_GPIO_TOP - GENERIC_GPIO_BASE))

#define GENERIC_GPIO_CONF_OUTPUT_EN_BIT    0
#define GENERIC_GPIO_CONF_INT_EDG0_EN_BIT  1
#define GENERIC_GPIO_CONF_INT_EDG1_EN_BIT  2
#define GENERIC_GPIO_CONF_INT_LVL0_EN_BIT  3
#define GENERIC_GPIO_CONF_INT_LVL1_EN_BIT  4
#define GENERIC_GPIO_CONF_PULLUP_EN_BIT    5
#define GENERIC_GPIO_CONF_PULLDOWN_EN_BIT  6
	/* n.b.: Confs limited to 8 bits for GENERIC_GPIO_ALIGNMENT == 1 */

#define GENERIC_GPIO_CONF_OUTPUT_EN_MASK    (1 << GENERIC_GPIO_CONF_OUTPUT_EN_BIT)
#define GENERIC_GPIO_CONF_INT_EDG0_EN_MASK  (1 << GENERIC_GPIO_CONF_INT_EDG0_EN_BIT)
#define GENERIC_GPIO_CONF_INT_EDG1_EN_MASK  (1 << GENERIC_GPIO_CONF_INT_EDG1_EN_BIT)
#define GENERIC_GPIO_CONF_INT_EDGX_EN_MASK  (GENERIC_GPIO_CONF_INT_EDG0_EN_MASK | GENERIC_GPIO_CONF_INT_EDG1_EN_MASK)
#define GENERIC_GPIO_CONF_INT_LVL0_EN_MASK  (1 << GENERIC_GPIO_CONF_INT_LVL0_EN_BIT)
#define GENERIC_GPIO_CONF_INT_LVL1_EN_MASK  (1 << GENERIC_GPIO_CONF_INT_LVL1_EN_BIT)
#define GENERIC_GPIO_CONF_PULLUP_EN_MASK    (1 << GENERIC_GPIO_CONF_PULLUP_EN_BIT)
#define GENERIC_GPIO_CONF_PULLDOWN_EN_MASK  (1 << GENERIC_GPIO_CONF_PULLDOWN_EN_BIT)

#ifndef GENERIC_GPIO_INTERRUPT_BASE
#error GENERIC_GPIO_INTERRUPT_BASE must be defined
#endif

#ifndef GENERIC_GPIO_DIRNAME
#define GENERIC_GPIO_DIRNAME "generic_gpio"
#endif

#ifndef UGLY_EXTERN_INCLUDE_HACK
#ifndef PP_STRING
#define PP_STRING "GIO"
#include "core/pretty_print.h"
#endif
#endif

#endif
// Only include this peripheral if requested in the platform memmap.h //
////////////////////////////////////////////////////////////////////////

#endif // GENERIC_GPIO_H

