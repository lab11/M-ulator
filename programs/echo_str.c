#include "lib/uart.h"
#include "lib/led.h"
#include "lib/printf.h"

int main() {
	char str[100];
	char *c = str;

	c--;
	do {
		c++;
		*c = UART_read();
	} while ((*c != '\n') && (c < (str+99)));
	*c = '\0';

	printf("%s\n", str);
	return 0;
}
