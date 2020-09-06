/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

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

#ifdef DEBUG1
#define _PP_EXTRA(_msg_dest, _msg_type)\
	do {\
		char _t_name[16];\
		if (0 == _DBG_T_NAME_HELPER(_t_name))\
			fprintf(_msg_dest, "%s %.2s %c: ", PP_STRING, _t_name, _msg_type);\
		else\
			fprintf(_msg_dest, "%s -- %c: ", PP_STRING, _msg_type);\
		fprintf(_msg_dest, "%s:%d\t%s:\t", __FILE__, __LINE__, __func__);\
	} while (0)
#else
#define _PP_EXTRA(_msg_dest, _msg_type)\
	fprintf(_msg_dest, "%s %c: ", PP_STRING, _msg_type)
#endif

#define INFO(...)\
	do {\
		flockfile(stdout); flockfile(stderr);\
		_PP_EXTRA(stdout, 'I'); printf(__VA_ARGS__);\
		funlockfile(stderr); funlockfile(stdout);\
	} while (0)
#define WARN(...)\
	do {\
		flockfile(stdout); flockfile(stderr);\
		_PP_EXTRA(stderr, 'W'); fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr); funlockfile(stdout);\
	} while (0)
#define ERR(_e, ...)\
	do {\
		flockfile(stdout); flockfile(stderr);\
		_PP_EXTRA(stderr, 'E');\
		fprintf(stderr, "Simulator shutting down due to error...\n");\
		_PP_EXTRA(stderr, 'E');\
		fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr); funlockfile(stdout);\
		void sim_terminate(bool);\
		sim_terminate(false);\
		_PP_EXTRA(stderr, 'E');\
		fprintf(stderr, __VA_ARGS__);\
		_PP_EXTRA(stderr, 'E');\
		fprintf(stderr, "*** Error: ");\
		if (_e == E_NOT_IMPLEMENTED) {\
			fprintf(stderr, "Not implemented error.\n");\
		} else {\
			fprintf(stderr, "Unknown error.\n");\
		}\
		if (raiseonerror) raise(SIGTRAP);\
		exit(_e);\
	} while (0)
#define TRAP(...)\
	do {\
		flockfile(stdout); flockfile(stderr);\
		_PP_EXTRA(stderr, 'P');\
		fprintf(stderr, __VA_ARGS__);\
		funlockfile(stderr); funlockfile(stdout);\
		raise(SIGTRAP);\
	} while (0)
#define TODO(...)\
	do {\
		flockfile(stdout); flockfile(stderr);\
		_PP_EXTRA(stdout, 'T');\
		fprintf(stdout, "TODO: ");\
		fprintf(stdout, __VA_ARGS__);\
		funlockfile(stderr); funlockfile(stdout);\
	} while (0)

#endif //PRETTY_PRINT_H
