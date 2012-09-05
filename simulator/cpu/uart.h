#ifndef UART_H
#define UART_H

#include "../common.h"

#ifndef PP_STRING
#define PP_STRING "URT"
#include "../pretty_print.h"
#endif

// CONFIGURATION
#define POLL_UART_PORT 4100
//#define POLL_UART_BUFSIZE 16
//#define POLL_UART_BAUD 1200
#define POLL_UART_BUFSIZE 8192
#define POLL_UART_BAUD 9600

#endif // UART_H
