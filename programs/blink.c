#include "lib/led.h"

int main() {
	int status0 = 1;
	int status1 = 1;

	/* TTTA: What should the LED blinking pattern of this program be? */
	/* 10 10 10 10 10 ... */
	/* 11 00 11 00 11 ... */

	int cnt = 10;

	while (--cnt > 0) {
		red_LED0_set(status0);
		blu_LED1_set(status1);

		status0 = !status0;
		status1 = (status0 + status1) % 2;
	}

	return 0;
}
