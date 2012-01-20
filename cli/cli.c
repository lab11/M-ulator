#include "../common.h"
#include "../simulator.h"

#include <getopt.h>

static void usage_fail(int retcode) {
	printf("\nUSAGE: ./simulator [OPTS]\n\n");
	printf("\
-- DEBUGGING -------------------------------------------------------------------\
\t-g, --gdb [port]\n\
\t\tAct as a remote target for gdb to debug. The optional port\n\
\t\targument specifies the port to listen on, otherwise a random\n\
\t\tport will be selected.\
\t-c, --dumpatpc PC_IN_HEX\n\
\t\tExecute until PC reaches the given value. The simulator will\n\
\t\tpause at the given PC and prompt for a new pause PC\n\
\t-y, --dumpatcycle N\n\
\t\tExecute until cycle N and print the current machine state.\n\
\t\tYou will be prompted for a new N at the pause\n\
\t-d, --dumpallcycles\n\
\t\tPrints the machine state every cycle -- This is a lot of text\n\
\t-p, --printcycles\n\
\t\tPrints cycle count and inst to execute every cycle (before exec)\n\
\t\tConceptually, this executes between the intstruction fetch and\n\
\t\tdecode stage, so the PC is already advanced\n\
\t-s, --slowsim\n\
\t\tSlows simulation down, running an instruction every .1s\n\
\t-e, --raiseonerror\n\
\t\tRaises a SIGTRAP for gdb on errors before dying\n\
\t\t(Useful for debugging with gdb\n\
-- RUN TIME --------------------------------------------------------------------\
\t-r, --returnr0\n\
\t\tSets simulator binary return code to the return\n\
\t\tcode of the executed program on simulator exit\n\
\t-m, --limit\n\
\t\tLimit CPU execution to N cycles, returns failure if hit\n\
\t\t(useful for catching runaway test cases)\n\
\t-f, --flash FILE\n\
\t\tFlash FILE into ROM before executing\n\
\t\t(this file is likely somthing.bin)\n\
\t-l, --showledwrites\n\
\t\tPrints LED state every time the LEDs are written to\n\
\t-u, --polluartport "VAL2STR(POLL_UART_PORT)"\n\
\t\tThe port number to communicate with the polled UART device\n\
\t--usetestflash\n\
\t\tInstead of reading a flash.mem, use a prebuilt internal\n\
\t\tflash.mem file. The internal flash.mem will run a valid\n\
\t\tinstance of the supplied echo program\n\
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
	uint16_t polluartport = POLL_UART_PORT;

	// Command line parsing
	while (1) {
		static struct option long_options[] = {
			{"gdb",           optional_argument, 0,              'g'},
			{"dumpatpc",      required_argument, 0,              'c'},
			{"dumpatcycle",   required_argument, 0,              'y'},
			{"dumpallcycles", no_argument,       &dumpallcycles, 'd'},
			{"printcycles",   no_argument,       &printcycles,   'p'},
			{"slowsim",       no_argument,       &slowsim,       's'},
			{"raiseonerror",  no_argument,       &raiseonerror,  'e'},
			{"returnr0",      no_argument,       &returnr0,      'r'},
			{"limit",         required_argument, 0,              'm'},
			{"flash",         required_argument, 0,              'f'},
			{"showledwrites", no_argument,       &showledwrites, 'l'},
			{"polluartport",  required_argument, 0,              'u'},
			{"usetestflash",  no_argument,       &usetestflash,  1},
			{"help",          no_argument,       0,              '?'},
			{0,0,0,0}
		};
		int option_index = 0;
		int c;

		c = getopt_long(argc, argv, "g::c:y:dpserm:f:lu:?", long_options,
				&option_index);

		if (c == -1) break;

		switch (c) {
			case 0:
				// option set a flag
				break;

			case 'g':
				gdb_port = optarg ? atoi(optarg) : 0;
				break;

			case 'c':
				dumpatpc = strtol(optarg, NULL, 16);
				INFO("Simulator will pause at execute of PC %x\n",
						dumpatpc);
				break;

			case 'y':
				dumpatcycle = atoi(optarg);
				INFO("Simulator will pause at cycle %d\n",
						dumpatcycle);
				break;

			case 'd':
				dumpallcycles = true;
				break;

			case 'p':
				printcycles = true;
				break;

			case 's':
				slowsim = true;
				break;

			case 'e':
				raiseonerror = true;
				break;

			case 'r':
				returnr0 = true;
				break;

			case 'm':
				limitcycles = atoi(optarg);
				INFO("Simulator will terminate at cycle %d\n",
						limitcycles);
				break;

			case'f':
				flash_file = optarg;
				INFO("Simulator will use %s as flash\n",
						flash_file);
				break;

			case 'l':
				showledwrites = true;
				break;

			case 'u':
			{
				int temp = atoi(optarg);
				assert(temp == ((uint16_t) temp));
				polluartport = (uint16_t) temp;
				break;
			}

			case '?':
			default:
				usage();
				break;
		}
	}

	simulator(flash_file, polluartport);

	ERR(E_UNKNOWN, "Simluator returned to cli main thread?\n");
}
