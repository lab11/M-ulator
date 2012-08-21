#include "periph.h"
#include "leds.h"

#include "memmap.h"
#include "core.h"

#include "../state.h"

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

static bool led_read(uint32_t addr, uint32_t *val) {
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

static void led_write(uint32_t addr, uint32_t val) {
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
	register_memmap(false, 4, mem_fn, REDLED, BLULED+4);
	mem_fn.W_fn32 = led_write;
	register_memmap(true, 4, mem_fn, REDLED, BLULED+4);

	register_periph_printer(print_leds_line);
}
