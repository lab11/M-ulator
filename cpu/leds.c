#include "leds.h"

#include "memmap.h"
#include "led_interface.h"

/* Placeholder Code */

static bool led_read(uint32_t addr, uint32_t *val) {
	*val = CORE_red_led_read();
	return true;
}

static void led_write(uint32_t addr, uint32_t val) {
	CORE_red_led_write(val);
}

__attribute__ ((constructor))
void register_memmap_led(void) {
	register_memmap_read_word(led_read, REDLED, BLULED+4);
	register_memmap_write_word(led_write, REDLED, BLULED+4);
}
