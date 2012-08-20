#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

#ifndef PP_STRING
#error The macro PP_STRING must be defined with a 3 character prefix
#endif

#include <signal.h>
extern int raiseonerror; // storage in simulator.c

/////////////////
// ERROR CODES //
/////////////////

// Generic errors
#define E_NOT_IMPLEMENTED	1
#define E_UNKNOWN		2

// 10-19 are simulator.h

/////////////////////////
// PRETTY PRINT MACROS //
/////////////////////////

#define INFO(...)\
	do {\
		flockfile(stdout);\
		printf("\r"PP_STRING" I: "); printf(__VA_ARGS__);\
		funlockfile(stdout);\
	} while (0)
#define WARN(...)\
	do {\
		flockfile(stderr);\
		fprintf(stderr, PP_STRING" W: "); fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr);\
	} while (0)
#define ERR(_e, ...)\
	do {\
		flockfile(stderr);\
		fprintf(stderr, PP_STRING" E: ");\
		fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr);\
		if (raiseonerror) raise(SIGTRAP);\
		exit(_e);\
	} while (0)
#define TRAP(...)\
	do {\
		flockfile(stderr);\
		fprintf(stderr, PP_STRING" T: ");\
		fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr);\
		raise(SIGTRAP);\
	} while (0)

#endif //PRETTY_PRINT_H
