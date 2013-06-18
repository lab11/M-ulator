/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
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
		fprintf(stderr, PP_STRING" E: *** Error: ");\
		if (_e == E_NOT_IMPLEMENTED) {\
			fprintf(stderr, "Not implemented error.\n");\
		} else {\
			fprintf(stderr, "Unknown error.\n");\
		}\
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
