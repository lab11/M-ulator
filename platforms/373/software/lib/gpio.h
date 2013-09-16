/* Copyright (c) 2011-2013  Pat Pannuto <pat.pannuto@gmail.com>
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

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

#include "../../simulator/memmap.h"
#define UGLY_EXTERN_INCLUDE_HACK
#include "../../../../simulator/cpu/common/gpio.h" // XXX: This is ugly
////////////////////////////////////////////////////////////////////////
// Only include this peripheral if requested in the platform memmap.h //
#ifdef GENERIC_GPIO_BASE

#define gpio_base_addr ((volatile gpio_align_t*) GENERIC_GPIO_BASE)
#define gpio_conf_addr ((volatile gpio_align_t*) GENERIC_GPIO_CONF_BASE)

inline __attribute__ ((always_inline))
bool gpio_read(unsigned gpio) {
	return *(gpio_base_addr + gpio);
}

inline __attribute__ ((always_inline))
void gpio_write(unsigned gpio, bool val) {
	*(gpio_base_addr + gpio) = val;
}

inline __attribute__ ((always_inline))
void gpio_conf_enable(unsigned gpio, gpio_align_t flags) {
	gpio_align_t temp = *(gpio_conf_addr + gpio);
	*(gpio_conf_addr + gpio) = temp | flags;
}

inline __attribute__ ((always_inline))
void gpio_conf_disable(unsigned gpio, gpio_align_t flags) {
	gpio_align_t temp = *(gpio_conf_addr + gpio);
	*(gpio_conf_addr + gpio) = temp & ~flags;
}

inline __attribute__ ((always_inline))
void gpio_conf_setto(unsigned gpio, gpio_align_t flags) {
	*(gpio_conf_addr + gpio) = flags;
}

#endif
// Only include this peripheral if requested in the platform memmap.h //
////////////////////////////////////////////////////////////////////////

#endif //GPIO_H
