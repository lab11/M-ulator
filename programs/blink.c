#include "lib/led.h"

int main() {
	int status0 = 1;
	int status1 = 1;

	/* TTTA: What should the LED blinking pattern of this program be? */
	/* 01 01 01 01 01 ... */
	/* 00 11 01 10 00 ... */

	int cnt = 10;

	while (--cnt > 0) {
		red_LED0_set(status0);
		blu_LED1_set(status1);

		status0 = !status0;
		status1 = (status0 + status1) % 2;
	}

	return 0;
}
