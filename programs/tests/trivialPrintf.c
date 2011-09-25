#include "lib/uart.h"
#include "lib/printf.h"

int main() {
	char resp;

	UART_write_str("\"1 arg string test >>%%s<<\\n\", \"FooBar\"\n");
	printf("1 arg string test >>%s<<\n", "FooBar");
	UART_write_str("Was this format correct (y/n): ");
	resp = UART_read();

	if (resp == 'n')
		return 1;

	// Run more tests

	UART_write_str("All printf tests succeeded\n");
	return 0;
}
