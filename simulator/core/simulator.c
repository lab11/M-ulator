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

#define STAGE SIM

#include <sys/time.h>

// XXX: Temporary fix, see note at end of simulator.h
#define PP_STRING "---"
#include "pretty_print.h"
// ^^ need to resolve ordering
#include "state_sync.h"
#include "simulator.h"

#include "loader.h"
#include "opcodes.h"
#include "pipeline.h"
#include "if_stage.h"
#include "id_stage.h"
#include "ex_stage.h"
#include "cpu/core.h"
#include "cpu/periph.h"
#include "cpu/registers.h"
#include "cpu/common/rom.h"
#include "cpu/common/ram.h"
#include "cpu/misc.h"
#include "gdb.h"

#include STATIC_ROM_HEADER

////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////

static volatile sig_atomic_t sigint = 0;
static volatile bool shell_running = false;

/* Peripherals */
static void join_periph_threads (void);

/* Config */
EXPORT int gdb_port = -1;
#define GDB_ATTACHED (gdb_port != -1)
EXPORT struct timespec cycle_time;
EXPORT int printcycles = 0;
#ifdef HAVE_DECOMPILE
EXPORT int decompile_flag = 0;
#endif
#ifdef HAVE_MEMTRACE
EXPORT int memtrace_flag = 0;
#endif
#ifdef DEBUG1
EXPORT int raiseonerror = 1;
#else
EXPORT int raiseonerror = 0;
#endif
EXPORT int limitcycles = -1;
EXPORT unsigned dumpatpc = -3;
EXPORT int dumpatcycle = -1;
EXPORT int dumpallcycles = 0;
EXPORT int returnr0 = 0;
EXPORT int usetestflash = 0;


/* State */

EXPORT int cycle = -1;
static void sim_delay_reset(void);

////////////////////////////////////////////////////////////////////////////////
// CORE
////////////////////////////////////////////////////////////////////////////////

// STATUS FUNCTIONS //

#define DIVIDERe printf("\r================================================================================\n")
#define DIVIDERd printf("--------------------------------------------------------------------------------\n")

// XXX: Define a consistent odering mechanism
struct periph_printer {
	struct periph_printer *next;
	void (*fn)(void);
};

static struct periph_printer periph_printers;

EXPORT void register_periph_printer(void (*fn)(void)) {
	if (periph_printers.fn == NULL) {
		periph_printers.fn = fn;
	} else {
		struct periph_printer *cur = &periph_printers;
		while (cur->next != NULL)
			cur = cur->next;
		cur->next = malloc(sizeof(struct periph_printer));
		cur = cur->next;
		cur->next = NULL;
		cur->fn = fn;
	}
}

static void print_periphs(void) {
	if (periph_printers.fn != NULL) {
		struct periph_printer *cur = &periph_printers;
		while (cur != NULL) {
			cur->fn();
			cur = cur->next;
		}
	}
}

static void print_reg_state_internal(void) {
	int i;
	union apsr_t apsr = CORE_apsr_read();
	union epsr_t epsr = CORE_epsr_read();

	printf("[Cycle %d]\t\t", cycle);
	printf("\t  T: %d", epsr.bits.T);
	printf("\t  N: %d  Z: %d  C: %d  V: %d  ",
			apsr.bits.N, apsr.bits.Z, apsr.bits.C, apsr.bits.V);
	printf("| ITSTATE: %02x  ", read_itstate());
	printf("\n");
	for (i=0; i<12; ) {
		printf("\tr%02d: %8x\tr%02d: %8x\tr%02d: %8x\tr%02d: %8x\n",
				i, CORE_reg_read(i),
				i+1, CORE_reg_read(i+1),
				i+2, CORE_reg_read(i+2),
				i+3, CORE_reg_read(i+3)
		      );
		i+=4;
	}
	printf("\tr12: %8x\t SP: %8x\t LR: %8x\t PC: %8x\n",
			CORE_reg_read(12),
			CORE_reg_read(SP_REG),
			CORE_reg_read(LR_REG),
			CORE_reg_read(PC_REG)
	      );
}

#ifndef NO_PIPELINE
static void print_stages(void) {
	printf("Stages:\n");
	printf("\tPC's:\tPRE_IF %08x, IF_ID %08x, ID_EX %08x\n",
			pre_if_PC, if_id_PC, id_ex_PC);
}
#endif

static void print_reg_state(void) {
	DIVIDERe;
	print_periphs();
	DIVIDERd;
	print_reg_state_internal();
#ifndef NO_PIPELINE
	DIVIDERd;
	print_stages();
#endif
}

static const char *get_dump_name(char c) {
	static char name[] = "/tmp/rom.\0\0\0\0\0\0\0\0\0";
	if ('\0' == name[strlen("/tmp/rom.")])
		if (0 != getlogin_r(name + strlen("/tmp/rom."), 9))
			perror("getting username for rom/ram dump");
	name[strlen("/tmp/r")] = c;
	return name;
}

static void print_full_state(void) {
	print_reg_state();

	DIVIDERd;

	{
		const char *file;
		size_t ret;

#if defined (HAVE_ROM) && defined (PRINT_ROM_ENABLE)
		file = get_dump_name('o');
		FILE* romfp = fopen(file, "w");
		if (romfp) {
			ret = dump_ROM(romfp);
			printf("Wrote %8zu bytes to %-29s "\
					"(Use 'hexdump -C' to view)\n",
					ret*ROMSIZE, file);
			fclose(romfp);
		} else {
			perror("No ROM dump");
		}
#endif

		// rom --> ram
		file = get_dump_name('a');

#if defined (HAVE_RAM)
		FILE* ramfp = fopen(file, "w");
		if (ramfp) {
			ret = dump_RAM(ramfp);
			printf("Wrote %8zu bytes to %-29s "\
					"(Use 'hexdump -C' to view)\n",
					ret*RAMSIZE, file);
			fclose(ramfp);
		} else {
			perror("No RAM dump");
		}
#endif
	}

	DIVIDERe;
}

static void _shell(void) {
	static char buf[100];

	// protect <EOF> replay
	buf[0] = '\0';
	printf("> ");
	if (buf != fgets(buf, 100, stdin)) {
		buf[0] = 'h';
		buf[1] = '\0';
	}

	switch (buf[0]) {
		case 'p':
			sscanf(buf, "%*s %x", &dumpatpc);
			return;

#ifdef HAVE_REPLAY
		case 'b':
			sprintf(buf, "s %d", cycle - 1);
			// fall thru

		case 's':
		{
			int ret;
			int target;
			ret = sscanf(buf, "%*s %d", &target);

			if (-1 == ret) {
				target = cycle + 1;
			} else if (1 != ret) {
				WARN("Error parsing input (ret %d?)\n", ret);
				return _shell();
			}

			if (target < 0) {
				WARN("Ignoring seek to negative cycle\n");
			} else if (target == cycle) {
				WARN("Ignoring seek to current cycle\n");
			} else {
				simulator_state_seek(target);
				print_full_state();
			}
			return _shell();
		}
#endif

		case 'q':
		case 't':
			sim_terminate(true);

		case 'r':
		{
			const char *file;

#if defined (HAVE_ROM) && defined (PRINT_ROM_ENABLE)
			if (buf[1] == 'o') {
				file = get_dump_name('o');
			} else
#endif
			if (buf[1] == 'a') {
				file = get_dump_name('a');
			} else {
				file = NULL;
				buf[1] = '\0';
				// now fall through 'c' to help
			}

			if (file) {
				char *cmd;
				if (-1 == asprintf(&cmd, "hexdump -C %s", file))
					ERR(E_UNKNOWN, "Failure allocating cmd string\n");
				FILE *out = popen(cmd, "r");

				char hex_buf[100];
				while ( fgets(hex_buf, 99, out) ) {
					printf("%s", hex_buf);
				}

				pclose(out);

				return _shell();
			}
		}

		case '\n':
			sprintf(buf, "cycle %d\n", cycle+1);
		case 'c':
			if (buf[1] == 'y') {
				int requested_cycle;
				sscanf(buf, "%*s %d", &requested_cycle);
				if (requested_cycle < cycle) {
					WARN("Request to execute into the past ignored\n");
					WARN("Did you mean 'seek %d'?\n", requested_cycle);
					return _shell();
				} else if (requested_cycle == cycle) {
					WARN("Request to execute to current cycle ignored\n");
					return _shell();
				} else {
					dumpatcycle = requested_cycle;
					return;
				}
			} else if (buf[1] == 'o') {
				dumpatcycle = 0;
				dumpatpc = 0;
				return;
			}
			// not 'cy' or 'co', fall thru

		case 'h':
		default:
			printf(">> The following commands are recognized:\n");
			printf("   <none>		Advance 1 cycle\n");
			printf("   pc HEX_ADDR		Stop at pc\n");
			printf("   cycle INTEGER	Stop at cycle\n");
#ifdef HAVE_REPLAY
			printf("   seek [INTEGER]	Seek to cycle\n");
#endif
#if defined (HAVE_ROM) && defined (PRINT_ROM_ENABLE)
			printf("   rom			Print ROM contents\n");
#endif
#if defined (HAVE_RAM)
			printf("   ram			Print RAM contents\n");
#endif
			printf("   continue		Continue\n");
			printf("   terminate		Terminate Simulation\n");
			return _shell();
	}
}

static void shell(void) {
	// Reset the cycle frequency governer to prevent spurious warnings.
	sim_delay_reset();

	if (GDB_ATTACHED) {
#ifdef DEBUG1
		print_full_state();
		stop_and_wait_for_gdb();
		print_full_state();
		return;
#else
		return stop_and_wait_for_gdb();
#endif
	}

	print_full_state();
	shell_running = true;
	_shell();
	shell_running = false;
}

////////////////////////////////////////////////////////////////////////////////

// When things are crashing, we try to print out helpful information, however
// that can cause nested errors. This flag bails directly in that case.
static bool _crashing = false;

EXPORT void CORE_WARN_real(const char *f, int l, const char *msg) {
	WARN("%s:%d\t%s\n", f, l, msg);
}

EXPORT void CORE_ERR_read_only_real(const char *f, int l, uint32_t addr) {
	_crashing = true;
	print_full_state();
	ERR(E_READONLY, "%s:%d\t%#08x is read-only\n", f, l, addr);
}

EXPORT void CORE_ERR_write_only_real(const char *f, int l, uint32_t addr) {
	_crashing = true;
	print_full_state();
	ERR(E_WRITEONLY, "%s:%d\t%#08x is write-only\n", f, l, addr);
}

EXPORT void CORE_ERR_invalid_addr_real(const char *f, int l, uint8_t is_write, uint32_t addr) {
	static bool dumping = false;
	if (dumping) {
		WARN("Err generating core dump, aborting\n");
	} else if (_crashing) {
		WARN("Generating core dump threw an error, aborting\n");
	} else {
		dumping = true;
		WARN("CORE_ERR_invalid_addr %s address: 0x%08x\n",
				is_write ? "writing":"reading", addr);
		WARN("Dumping Core...\n");
		print_full_state();
	}
	ERR(E_INVALID_ADDR, "%s:%d\tTerminating due to invalid addr (%s %08x)\n", f, l,
			is_write ? "WRITE":"READ", addr);
}

EXPORT void CORE_ERR_illegal_instr_real(const char *f, int l, uint32_t inst) {
	_crashing = true;
	WARN("CORE_ERR_illegal_instr, inst: %08x\n", inst);
	WARN("Dumping core...\n");
	print_full_state();
	ERR(E_UNKNOWN, "%s:%d\tUnknown inst\n", f, l);
}

EXPORT void CORE_ERR_unpredictable_real(const char *f, int l, const char *opt_msg) {
	_crashing = true;
	WARN("Dumping core...\n");
	print_full_state();
	ERR(E_UNPREDICTABLE, "%s:%d\tCORE_ERR_unpredictable -- %s\n", f, l, opt_msg);
}

EXPORT void CORE_ERR_runtime_real(const char *f, int l, const char *opt_msg) {
	_crashing = true;
	WARN("Dumping core...\n");
	print_full_state();
	ERR(E_RUNTIME, "%s:%d\tCORE_ERR_runtime -- %s\n", f, l, opt_msg);
}

EXPORT void CORE_ERR_not_implemented_real(const char *f, int l, const char *opt_msg) {
	_crashing = true;
	WARN("Dumping core...\n");
	print_full_state();
	ERR(E_NOT_IMPLEMENTED, "%s:%d\tCORE_ERR_not_implemented -- %s\n", f, l, opt_msg);
}


////////////////////////////////////////////////////////////////////////////////
// SIMULATOR
////////////////////////////////////////////////////////////////////////////////

#if _POSIX_TIMERS > 0
static struct timespec last_cycle_time;
#elif defined __APPLE__
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
static uint64_t last_cycle_time;
static mach_timebase_info_data_t sTimebaseInfo;
#endif

#ifdef HAVE_REPLAY
EXPORT bool simulator_state_seek(int target) {
	WARN("Async peripheral state not changed\n");
	int pipeline_cycle = pipeline_state_seek(target);
	int simulator_cycle = state_seek_for_calling_thread(target);
	if (pipeline_cycle != simulator_cycle) {
		WARN("Pipeline and simulator core out of sync after seek\n");
		ERR(E_UNKNOWN, "Pipeline cycle %d. Simulator cycle %d\n",
				pipeline_cycle, simulator_cycle);
	}
	cycle = simulator_cycle;

	if (cycle != target) {
		WARN("Could not seek to cycle %d. Simulator left at cycle %d\n",
				target, cycle);
	}

	return cycle != target;
}
#endif

static struct timeval sim_execute_time_start = {0, 0};
static bool sim_awake = true;
static double sim_elapsed;
EXPORT void sim_sleep(void) {
	if (!sim_awake) {
		WARN("Multiple calls to sim_sleep; freq likely broken\n");
		return;
	}
	sim_awake = false;

	const double usec_per_sec = 1000000;
	struct timeval end;
	gettimeofday(&end, NULL);
	double elapsed = (double) (end.tv_sec - sim_execute_time_start.tv_sec);
	elapsed += (double) ((end.tv_usec - sim_execute_time_start.tv_usec) / usec_per_sec);
	sim_elapsed += elapsed;
}

EXPORT void sim_wakeup(void) {
	sim_awake = true;
	gettimeofday(&sim_execute_time_start, NULL);
}

static void sim_delay_reset() {
#if _POSIX_TIMERS > 0
	// Perform check here to verify system (i) has CLOCK_REALTIME and (ii)
	// supports clock_gettime. This prevents requiring the check in the
	// main loop.
	if (0 != clock_gettime(CLOCK_REALTIME, &last_cycle_time))
		ERR(E_UNKNOWN, "clock_gettime failed unexpectedly: %s", strerror(errno));
	last_cycle_time.tv_nsec = -1;
#elif defined __APPLE__
	mach_timebase_info(&sTimebaseInfo);
	last_cycle_time = -1;
#endif
}

#if (_POSIX_TIMERS > 0) || (defined __APPLE__)
static void sim_delay_warn(long ns) {
	static int warn_count = 0;
	if (warn_count < 5) {
		WARN("Timing requirement missed\n");
		WARN("Cycle %d exceeded requested time by %ld ns\n", cycle, ns);
		warn_count++;
	} else if (warn_count++ == 5) {
		WARN("Timing requirement missed\n");
		WARN("Too many timing violations. Warnings suppressed\n");
	}
}
#endif

static void sim_delay(void) {
#if _POSIX_TIMERS > 0
	if (likely(last_cycle_time.tv_nsec != -1)) {
		struct timespec this_cycle_time;
		clock_gettime(CLOCK_REALTIME, &this_cycle_time);

		struct timespec cycle_delay_time = {0, 0};
		cycle_delay_time.tv_nsec = cycle_time.tv_nsec -
			(this_cycle_time.tv_nsec - last_cycle_time.tv_nsec);
		if (cycle_delay_time.tv_nsec > 0) {
			nanosleep(&cycle_delay_time, NULL);
		} else {
			sim_delay_warn(-cycle_delay_time.tv_nsec);
		}
	}
	clock_gettime(CLOCK_REALTIME, &last_cycle_time);
#elif defined __APPLE__
	/* https://developer.apple.com/library/mac/#qa/qa1398/_index.html */
	if (likely(last_cycle_time != (uint64_t) -1)) {
		uint64_t this_cycle_time, elapsed;
		uint64_t elapsedNano;
		long delay;
		this_cycle_time = mach_absolute_time();

		elapsed = this_cycle_time - last_cycle_time;

		// (code & comment from above link)
		// Do the maths. We hope that the multiplication doesn't
		// overflow; the price you pay for working in fixed point.
		elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
		delay = cycle_time.tv_nsec - elapsedNano;

		if (delay > 0) {
			struct timespec cycle_delay_time = {
				.tv_sec = 0,
				.tv_nsec = delay
			};
			nanosleep(&cycle_delay_time, NULL);
		} else {
			sim_delay_warn(-delay);
		}
	}
	last_cycle_time = mach_absolute_time();
#else
	WARN("Cannot control frequency. No timer implementation is\n");
	WARN("available for the current platform. Have support for:\n");
	WARN("\tPosix timers (_POSIX_TIMERS defined and > 0)\n");
	WARN("\tMach timers (__APPLE__ defined)\n");
	ERR(E_NOT_IMPLEMENTED, "No timer available for platform\n");
#endif
}

static int sim_execute(void) {
	// XXX: What if the debugger wants to execute the same instruction two
	// cycles in a row? How do we allow this?
	static uint32_t prev_pc = STALL_PC;

	cycle++;
	DBG2("Begin cycle %d.............................cycle %d\n", cycle, cycle);

	// Simulator main thread ticks and tocks so that branch-to-self logic resets
	state_start_tick();

	uint32_t cur_pc = CORE_reg_read(PC_REG);
	if ((SR(&prev_pc) == cur_pc) && (SR(&prev_pc) != STALL_PC)) {
		DBG1("cycle: %d prev_pc: %08x cur_pc: %08x\n", cycle, SR(&prev_pc), cur_pc);
		if (CONF_no_terminate) {
			static bool notice = false;
			if (!notice) {
				INFO("PC 0x%08x is branch to self, simulation will loop forever.\n", cur_pc);
				INFO("(ctrl-c to quit)\n");
				notice = true;
			}
		} else {
			if (GDB_ATTACHED) {
				INFO("Simulator determined PC 0x%08x is branch to self, breaking for gdb.\n", cur_pc);
				shell();
			} else {
				INFO("Simulator determined PC 0x%08x is branch to self, terminating.\n", cur_pc);
				sim_terminate(true);
			}
		}
	} else {
		SW(&prev_pc, cur_pc);
	}

	state_tock();

	// Start a clock tick
	pipeline_stages_tick();
	state_handle_exceptions();
	pipeline_stages_tock();

	if (cycle_time.tv_nsec)
		sim_delay();

	return SUCCESS;
}

static void sim_reset(void) __attribute__ ((noreturn));
static void sim_reset(void) {
	int ret;

	// Initilize the frequency governer
	sim_delay_reset();

	if (GDB_ATTACHED) {
		gdb_init(gdb_port);
#ifdef DEBUG1
		print_full_state();
		wait_for_gdb();
		print_full_state();
#else
		wait_for_gdb();
#endif
	}

	INFO("Asserting reset pin\n");
	state_enter_debugging();
	cycle = 0;
	reset();
	state_handle_exceptions();
	state_exit_debugging();
	INFO("De-asserting reset pin\n");

	gettimeofday(&sim_execute_time_start, NULL);

	INFO("Entering main loop...\n");
	do {
		if (sigint) {
			sigint = 0;
			shell();
		} else
		if ((limitcycles != -1) && limitcycles <= cycle) {
			ERR(E_UNKNOWN, "Cycle limit (%d) reached.\n", limitcycles);
		} else
		if (dumpatcycle == cycle) {
			shell();
		} else
		if ((dumpatpc & 0xfffffffe) == (CORE_reg_read(PC_REG) & 0xfffffffe)) {
			shell();
		} else
		if (dumpallcycles) {
			print_reg_state();
		}
	} while (SUCCESS == (ret = sim_execute()) );

	WARN("Simulation terminated with error code: %u\n", ret);
	WARN("Dumping core...\n");
	print_full_state();
	ERR(ret, "Terminating\n");
}

EXPORT void sim_terminate(bool should_exit) {
	static bool terminating = false;
	if (terminating) {
		WARN("Nested calls to terminate. Dying\n");
		exit(EXIT_FAILURE);
	}
	terminating = true;

	if ((sim_execute_time_start.tv_sec != 0) && (sim_execute_time_start.tv_usec != 0)) {
		sim_sleep();
		double freq = cycle / sim_elapsed;
		INFO("Approximate average frequency: %f hz\n", freq);
	}
	INFO("Simulator executed %d cycle%s\n", cycle, (cycle == 1) ? "":"s");
	if (core_stats_unaligned_cycle_penalty != 0) {
		WARN("Wasted %u cycle(s) to unaligned memory accesses\n",
				core_stats_unaligned_cycle_penalty);
	}
	join_periph_threads();
	INFO("Simulator shutdown successfully.\n");
	if (!should_exit)
		return;
	if (returnr0) {
		uint32_t r0 = CORE_reg_read(0);
		DBG2("Return code is r0: %08x\n", r0);
		exit(r0);
	}
	exit(EXIT_SUCCESS);
}

static void power_on(void) __attribute__ ((noreturn));
static void power_on(void) {
	INFO("Powering on processor...\n");
	sim_reset();
}

int register_opcode_mask_16_real(uint16_t, uint16_t, void (*fn) (uint16_t), const char *);
static void load_opcodes(void) {
	// N.B. These are registered before main via constructor attributes
	INFO("Registered %d opcode mask%s\n", get_opcode_masks(),
			(get_opcode_masks() == 1) ? "":"s");

	opcode_statistics();

#ifndef NO_PIPELINE
	// Fake instructions used to propogate pipeline exceptions
	uint16_t hazard = INST_HAZARD;
	register_opcode_mask_16_real(hazard, ~hazard, pipeline_exception, "Pipeline Excpetion");
#endif
}

#ifdef __APPLE__
#include "external/include/valgrind.h"
#endif

static void* sig_thread(void *arg) {
	sigset_t *set = (sigset_t *) arg;
	int s, sig;

#ifdef __APPLE__
	if (0 != pthread_setname_np("signal handler"))
#else
	if (0 != prctl(PR_SET_NAME, "signal handler", 0, 0, 0))
#endif
		ERR(E_UNKNOWN, "Unexpected error setting thread name: %s\n", strerror(errno));

	for (;;) {
		s = sigwait(set, &sig);
		if (s != 0) {
#ifdef __APPLE__
			// Valgrind on OS X does not support sig wait
			if (RUNNING_ON_VALGRIND) {
				WARN("Running under valgrind, no signal handler available\n");
				return NULL;
			}
#endif
			ERR(E_UNKNOWN, "sigwait failed?\n");
		}

		if (sig == SIGINT) {
			if (shell_running) {
				flockfile(stdout); flockfile(stderr);
				printf("\nQuit\n");
				exit(0);
			} else {
				if (sigint == 1) {
					flockfile(stdout); flockfile(stderr);
					printf("\nQuit\n");
					exit(0);
				}
				INFO("Caught SIGINT, again to quit\n");
				sigint = 1;
			}
		} else {
			ERR(E_UNKNOWN, "caught unknown signal %d\n", sig);
		}
	}
}

struct periph_thread {
	struct periph_thread *next;
	pthread_t (*fn)(void *);
	const char *name;
	struct periph_time_travel tt;
	volatile bool *en;
	int fd;
	void *arg;
	pthread_t pthread;
	bool active;
};

static struct periph_thread periph_threads;

EXPORT void register_periph_thread(
		pthread_t (*fn)(void *), const char *name,
		struct periph_time_travel tt,
		volatile bool *en, int fd,
		void *arg) {
	if (periph_threads.fn == NULL) {
		periph_threads.fn = fn;
		periph_threads.name = name;
		periph_threads.tt = tt;
		periph_threads.en = en;
		periph_threads.arg = arg;
		periph_threads.fd = fd;
		periph_threads.active = false;
	} else {
		struct periph_thread *cur = &periph_threads;
		while (cur->next != NULL)
			cur = cur->next;
		cur->next = malloc(sizeof(struct periph_thread));
		cur = cur->next;
		cur->next = NULL;
		cur->fn = fn;
		cur->name = name;
		cur->tt = tt;
		cur->en = en;
		cur->fd = fd;
		cur->arg = arg;
		cur->active = false;
	}
}

EXPORT void simulator(const char *flash_file) {
	const char thread_name[16] = "Simulator main";
#ifdef __APPLE__
	if (0 != pthread_setname_np(thread_name))
#else
	if (0 != prctl(PR_SET_NAME, thread_name, 0, 0, 0))
#endif
		ERR(E_UNKNOWN, "Unexpected error setting thread name: %s\n", strerror(errno));

	// Read in flash
	if (usetestflash) {
		flash_image((const uint8_t*) static_rom, STATIC_ROM_NUM_BYTES);
		INFO("Loaded internal test flash\n");
	} else {
		if (NULL == flash_file) {
			if (GDB_ATTACHED) {
				WARN("No binary image specified, you will have to 'load' one with gdb\n");
			} else {
				ERR(E_BAD_FLASH, "--flash or --usetestflash required, see --help\n");
			}
		} else {
			load_file(flash_file);
		}
	}

	load_opcodes();

	// Prep signal-related stuff:
	signal(SIGPIPE, SIG_IGN);

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	{
		int ret;
		if (0 != (ret = pthread_sigmask(SIG_BLOCK, &set, NULL)) )
			ERR(E_UNKNOWN, "pthread_sigmask: %s", strerror(ret));
	}

	// Spawn signal handling thread
	pthread_t sig_pthread;
	pthread_create(&sig_pthread, NULL, &sig_thread, (void *) &set);

	// Spawn peripheral threads
	if (periph_threads.fn != NULL) {
		struct periph_thread *cur = &periph_threads;
		while (cur != NULL) {
			if (cur->en)
				*cur->en = true;
			cur->pthread = cur->fn(cur->arg);
			cur->active = true;
			cur = cur->next;
		}
	}

#ifndef NO_PIPELINE
	pipeline_init();
#endif

	power_on();

	// Should not get here, proper exit comes from self-branch detection
	ERR(E_UNKNOWN, "core thread terminated unexpectedly\n");
}

static void join_periph_threads(void) {
	if (periph_threads.fn != NULL) {
		INFO("Shutting down all peripherals\n");

		struct periph_thread *cur = &periph_threads;
		while (cur != NULL) {
			DBG1("Shutting down peripheral: %s\n", cur->name);
			if (cur->en)
				*cur->en = false;
			if (cur->fd) {
				if (write(cur->fd, "S", 1) != 1) {
					ERR(E_UNKNOWN, "thread kill pipe: %s\n",
							strerror(errno));
				}
			}
			if (cur->active) {
				pthread_join(cur->pthread, NULL);
				cur->active = false;
			} else {
				DBG1("(Skipping inactive periph: %s)\n", cur->name);
			}
			cur = cur->next;
		}
	}
}

