#include "m3_ctrl.h"

void delay(unsigned ticks) {
	unsigned i;

	for (i=0; i < ticks; i++)
		asm("nop;");
}

int main() {
	while (1) {
		write_i2c_message(0x98, 4, 0xdeadbeef);
		delay(10);
	}
}
