#include "m3_ctrl.h"

void delay(unsigned ticks) {
	unsigned i;

	for (i=0; i < ticks; i++)
		asm("nop;");
}

int main() {
	while (1) {
		// M3 MMIO i2c interface sends from least sig byte
		// to most sig byte, but display tools all print one
		// byte at a time; set this up so deadbeef prints correctly
		write_i2c_message(0x98, 4, 0xefbeadde);
		delay(1000);
	}
}
