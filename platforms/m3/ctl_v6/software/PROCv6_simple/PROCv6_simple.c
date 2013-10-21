#include "m3_proc.h"

void delay(unsigned ticks) {
	unsigned i;

	for (i=0; i < ticks; i++)
		asm("nop;");
}

int main() {
  
  enumerate(3);
  delay (10000);
  enumerate(4);
  delay (10000);
  write_mbus_register(4,3,0xBF0F0);

  //  sleep();

  while (1){}

}
