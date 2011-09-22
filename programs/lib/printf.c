#include <stdarg.h>

#include "printf.h"
#include "uart.h"

/* Suggest getting it working for a fixed number
   of arguments first. Also, you do not have to
   support every format specifier that printf does,
   the "major" ones are sufficient: d, u, x, c, s */

int printf(const char *format, ...) {
	va_list v;
	va_start(v, format);

	char *s;

	s = va_arg(v, char *);

	UART_write_str(s);
	UART_write('\n');

	va_end(v);

	return 0;
}
