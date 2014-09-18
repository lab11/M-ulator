#include "m3_ctrl.h"

void delay(unsigned ticks) {
	unsigned i;

	for (i=0; i < ticks; i++)
		asm("nop;");
}

int main() {
	GPIO_direction_set(0x3);

	while (1) {
		GPIO_set(0);
		delay(1000);
		GPIO_set(3);
		delay(1000);
	}
}
