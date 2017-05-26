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

#include "core/common.h"

// For access to 'raiseonerror' flag
#define PP_STRING "CLI"
#include "core/pretty_print.h"

#include "core/simulator.h"

#include <getopt.h>
#include <ctype.h>
#include <math.h>

#include STATIC_ROM_HEADER

static void usage_fail(int retcode) {
	printf("\nUSAGE: ./simulator [OPTS]\n\n");
	printf("\
-- DEBUGGING -------------------------------------------------------------------\n\
\t-g, --gdb [port]\n\
\t\tAct as a remote target for gdb to debug. The optional port\n\
\t\targument specifies the port to listen on, otherwise a random\n\
\t\tport will be selected.\n\
\t-c, --dumpatpc PC_IN_HEX\n\
\t\tExecute until PC reaches the given value. The simulator will\n\
\t\tpause at the given PC and prompt for a new pause PC\n\
\t-y, --dumpatcycle N\n\
\t\tExecute until cycle N and print the current machine state.\n\
\t\tYou will be prompted for a new N at the pause\n\
\t-a, --dumpallcycles\n\
\t\tPrints the machine state every cycle -- This is a lot of text\n\
\t-p, --printcycles\n\
\t\tPrints cycle count and inst to execute every cycle (before exec)\n\
\t\tConceptually, this executes between the intstruction fetch and\n\
\t\tdecode stage, so the PC is already advanced\n"
	      );
#ifdef HAVE_DECOMPILE
	printf("\
\t-d, --decompile\n\
\t\tPrint real-time \"decompilation\" of instructions as they are\n\
\t\texecuted. Values of all instruction arguments before execution\n\
\t\tare printed in ()'s. Removing these (e.g. sed 's/([^)]*)//g')\n\
\t\tshould generate a legal, exectuable stream of assembly\n\
\t\t(with obvious caveats of branches, pc-relative ldr/str, etc)\n"
	      );
#endif
#ifdef HAVE_MEMTRACE
	printf("\
\t-m, --memory-trace\n\
\t\tPrint all memory accesses as they are executed.\n"
	      );
#endif
	printf("\
\t-s, --speed SPEED\n\
\t\tSpecify a clock speed to run at. The units are assumed to be kHz\n\
\t\tunless otherwise specified. A warning will be printed if the \n\
\t\tsimulator cannot keep up the requested speed. Setting speed\n\
\t\tto 0 (default) will run unthrottled.\n\
\t-e, --raiseonerror\n\
\t\tRaises a SIGTRAP for gdb on errors before dying\n\
\t\t(Useful for debugging with gdb)\n\
-- RUN TIME --------------------------------------------------------------------\n\
\t-r, --returnr0\n\
\t\tSets simulator binary return code to the return\n\
\t\tcode of the executed program on simulator exit\n\
\t-l, --limit\n\
\t\tLimit CPU execution to N cycles, returns failure if hit\n\
\t\t(useful for catching runaway test cases)\n\
\t-T, --no-terminate\n\
\t\tDo not terminate when code branches to self (run forever)\n\
\t--rzwi-memory\n\
\t\tTreat accesses to unknown memory addresses as 'read zero,\n\
\t\twrite ignore'. Can be useful for partially implemented cores\n\
\t-f, --flash FILE\n\
\t\tFlash FILE into ROM before executing\n\
\t\t(this file is likely somthing.bin)\n\
\t--usetestflash\n\
\t\tFlash the simulator with a built-in test program before running\n\
\t\tConflicts with -f. The test flash program is:\n"
	      );
#ifdef STATIC_ROM_BLURB
	printf("%s", STATIC_ROM_BLURB);
#else
	printf("\
\t\t\tNo information available.\n\
\t\t\tPlease #define STATIC_ROM_BLURB in static_rom.h\n"
	      );
#endif
	printf("\
\n\
"\
	       );
	exit(retcode);
}

static void usage(void) {
	usage_fail(0);
}

int main(int argc, char **argv) {
	const char *flash_file = NULL;

	// Command line parsing
	while (1) {
		static struct option long_options[] = {
			{"gdb",           optional_argument, 0,              'g'},
			{"dumpatpc",      required_argument, 0,              'c'},
			{"dumpatcycle",   required_argument, 0,              'y'},
			{"dumpallcycles", no_argument,       &dumpallcycles, 'a'},
			{"printcycles",   no_argument,       &printcycles,   'p'},
#ifdef HAVE_DECOMPILE
			{"decompile",     no_argument,       &decompile_flag,'d'},
#endif
#ifdef HAVE_MEMTRACE
			{"memtrace",      no_argument,       &memtrace_flag, 'm'},
#endif
			{"speed",         required_argument, 0,              's'},
			{"raiseonerror",  no_argument,       &raiseonerror,  'e'},
			{"returnr0",      no_argument,       &returnr0,      'r'},
			{"limit",         required_argument, 0,              'l'},
			{"no-terminate",  no_argument,       &CONF_no_terminate, 'T'},
			{"rzwi-memory",   no_argument,       &CONF_rzwi_memory, 2},
			{"flash",         required_argument, 0,              'f'},
			{"usetestflash",  no_argument,       &usetestflash,  1},
			{"help",          no_argument,       0,              '?'},
			{0,0,0,0}
		};
		int option_index = 0;
		int c;

		char optstring[64] = "g::c:y:aps:erl:Tf:?";
#ifdef HAVE_DECOMPILE
		strncat(optstring, "d", 64);
#endif
#ifdef HAVE_MEMTRACE
		strncat(optstring, "m", 64);
#endif

		c = getopt_long(argc, argv, optstring, long_options, &option_index);

		if (c == -1) break;

		switch (c) {
			case 0:
				// option set a flag
				break;

			case 'g':
				gdb_port = optarg ? atoi(optarg) : 0;
				break;

			case 'c':
			{
				long ret = strtol(optarg, NULL, 16);
				dumpatpc = (unsigned) ret;
				if (dumpatpc != ret)
					ERR(E_UNKNOWN, "Requested cycle (%ld) too large\n", ret);

				INFO("Simulator will pause at execute of PC %x\n",
						dumpatpc);
				break;
			}

			case 'y':
				dumpatcycle = atoi(optarg);
				INFO("Simulator will pause at cycle %d\n",
						dumpatcycle);
				break;

			case 'a':
				dumpallcycles = true;
				break;

			case 'p':
				printcycles = true;
				break;

#ifdef HAVE_DECOMPILE
			case 'd':
				decompile_flag = true;
				break;
#endif
#ifdef HAVE_MEMTRACE
			case 'm':
				memtrace_flag = true;
				break;
#endif

			case 's':
			{
				long double simspeed;
				char *endptr;
				simspeed = strtold(optarg, &endptr);
				while (isspace(*endptr)) endptr++;
				if ((*endptr == 'h') || (*endptr == 'H')) {
					if (simspeed <= 1) {
						WARN("Speed must be > 1 Hz. Set to 1.1 Hz\n");
						simspeed = 1.1;
					}
				} else if ((*endptr == 'k') || (*endptr == 'K'))
					simspeed *= 1000;
				else if ((*endptr == 'm') || (*endptr == 'M'))
					simspeed *= 1000000;
				else if ((*endptr == 'g') || (*endptr == 'G')) {
					WARN("You're kidding right? The simulator is not\n");
					WARN("capable of real-time simulation of a GHz core\n");
					ERR(E_UNKNOWN, "Requested speed too fast\n");
				} else if (*endptr == '\0') {
					// Default is kHz
					simspeed *= 1000;
				} else {
					ERR(E_UNKNOWN, "Unrecognized speed format character: %c\n",
							*endptr);
				}

				simspeed = 1 / simspeed;
				cycle_time.tv_sec = floorl(simspeed);
				assert(cycle_time.tv_sec == 0);
				cycle_time.tv_nsec = roundl(simspeed * 1000000000);
				DBG1("Cycle time set to %ld nsec\n", cycle_time.tv_nsec);

				// Allow us to only check nsec to see if timing is requested
				if (cycle_time.tv_sec && (!cycle_time.tv_nsec))
					cycle_time.tv_nsec++;
				break;
			}

			case 'e':
				raiseonerror = true;
				break;

			case 'r':
				returnr0 = true;
				break;

			case 'l':
				limitcycles = atoi(optarg);
				INFO("Simulator will terminate at cycle %d\n",
						limitcycles);
				break;

			case'f':
				flash_file = optarg;
				INFO("Simulator will use %s as flash\n",
						flash_file);
				break;

			case '?':
			default:
				usage();
				break;
		}
	}

	if (flash_file && usetestflash) {
		ERR(E_BAD_FLASH, "Only one of -f or --usetestflash may be used\n");
	} else if (usetestflash) {
		INFO("Simulator will use internal test flash\n");
	}

	simulator(flash_file);

	ERR(E_UNKNOWN, "Simluator returned to cli main thread?\n");
}
