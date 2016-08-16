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

#include "leds.h"

#include "memmap.h"

#include "cpu/periph.h"
#include "cpu/core.h"

#include "core/state_sync.h"

uint32_t red;
uint32_t grn;
uint32_t blu;

static void print_leds_line(void) {
	int i;

	printf("Simple LED Dev  ");

	printf("| R: ");
	for (i = 0; i < 8; i++)
		printf("%d ", !!(red & (1 << i)));

	printf("| G: ");
	for (i = 0; i < 8; i++)
		printf("%d ", !!(grn & (1 << i)));

	printf("| B: ");
	for (i = 0; i < 8; i++)
		printf("%d ", !!(blu & (1 << i)));

	printf("|\n");
}

static bool led_read(uint32_t addr, uint32_t *val,
		bool debugger __attribute__ ((unused)) ) {
	switch (addr) {
		case REDLED:
			*val = SR(&red);
			break;
		case GRNLED:
			*val = SR(&grn);
			break;
		case BLULED:
			*val = SR(&blu);
			break;
		default:
			return false;
	}

	return true;
}

static void led_write(uint32_t addr, uint32_t val,
		bool debugger __attribute__ ((unused)) ) {
	switch (addr) {
		case REDLED:
			SW(&red, val & 0xff); // device emulates 8 leds
			break;
		case GRNLED:
			SW(&grn, val & 0xff);
			break;
		case BLULED:
			SW(&blu, val & 0xff);
			break;
		default:
			assert(false && "Bad LED memmap");
	}
}

__attribute__ ((constructor))
void register_led_periph(void) {
	union memmap_fn mem_fn;

	mem_fn.R_fn32 = led_read;
	register_memmap("Simple LED", false, 4, mem_fn, REDLED, BLULED+4);
	mem_fn.W_fn32 = led_write;
	register_memmap("Simple LED", true, 4, mem_fn, REDLED, BLULED+4);

	register_periph_printer(print_leds_line);
}
