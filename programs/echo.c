#include "lib/uart.h"
#include "lib/led.h"

int main() {
	char c;
	c = UART_read();
	UART_write(c);
	return 0;
}
