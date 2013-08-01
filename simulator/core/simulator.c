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

#ifndef __APPLE__
#include <sys/prctl.h>
#endif
#include <sys/stat.h>

// XXX: Temporary fix, see note at end of simulator.h
#define PP_STRING "---"
#include "pretty_print.h"
// ^^ need to resolve ordering
#include "state_sync.h"
#include "simulator.h"

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


////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////

static volatile sig_atomic_t sigint = 0;
static volatile bool shell_running = false;

/* Tick Control */
EXPORT sem_t* ticker_ready_sem;
EXPORT sem_t* start_tick_sem;
EXPORT sem_t* end_tick_sem;
EXPORT sem_t* end_tock_sem;
#define NUM_TICKERS	3	// IF, ID, EX

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

/* Test Flash */

// utils/bintoarray.sh
#define STATIC_ROM_NUM_BYTES (80 * 4)
static const uint32_t static_rom[80] = {0x20003FFC,0x55,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0xBF00E7FE,0xF862F000,0xBF00E7FA,0xB083B480,0xF244AF00,0xF2C41300,0x603B0300,0x1304F244,0x300F2C4,0x683B607B,0xB2DB781B,0x302F003,0xD0F82B00,0x781B687B,0x4618B2DB,0x70CF107,0xBC8046BD,0xBF004770,0xB085B480,0x4603AF00,0xF24471FB,0xF2C41300,0x60BB0300,0x1308F244,0x300F2C4,0x68BB60FB,0xB2DB781B,0x304F003,0xD1F82B00,0x79FA68FB,0xF107701A,0x46BD0714,0x4770BC80,0xB082B580,0x6078AF00,0x687BE008,0x4618781B,0xFFD8F7FF,0xF103687B,0x607B0301,0x781B687B,0xD1F22B00,0x708F107,0xBD8046BD,0x181B4B05,0x2A006818,0x2201BF03,0x4390408A,0x60184310,0x30004770,0x40001000,0xB082B580,0xF7FFAF00,0x4603FF9B,0x79FB71FB,0xF7FF4618,0xF04FFFB3,0x46180300,0x708F107,0xBD8046BD};

/* State */

EXPORT int cycle = -1;
#ifndef NO_PIPELINE
EXPORT bool stages_should_tock = false;
#endif
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

	printf("[Cycle %d]\t\t\t", cycle);
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

static void print_reg_state(void) {
	DIVIDERe;
	print_periphs();
	DIVIDERd;
	print_reg_state_internal();
}

#ifndef NO_PIPELINE
static void print_stages(void) {
	printf("Stages:\n");
	printf("\tPC's:\tPRE_IF %08x, IF_ID %08x, ID_EX %08x\n",
			pre_if_PC, if_id_PC, id_ex_PC);
}
#endif

static const char *get_dump_name(char c) {
	static char name[] = "/tmp/rom.\0\0\0\0\0\0\0\0\0";
	if ('\0' == name[strlen("/tmp/rom.")])
		if (0 != getlogin_r(name + strlen("/tmp/rom."), 9))
			perror("getting username for rom/ram dump");
	name[strlen("/tmp/r")] = c;
	return name;
}

static void print_full_state(void) {
	DIVIDERe;
	print_periphs();

	DIVIDERd;
	print_reg_state_internal();

#ifndef NO_PIPELINE
	DIVIDERd;
	print_stages();
#endif

	DIVIDERd;

	{
		const char *file;
		size_t i;

#if defined (HAVE_ROM) && defined (PRINT_ROM_EN)
		file = get_dump_name('o');
		FILE* romfp = fopen(file, "w");
		if (romfp) {
			uint32_t rom[ROMSIZE >> 2] = {0};
			for (i = ROMBOT; i < ROMBOT+ROMSIZE; i += 4)
#ifdef DEBUG2
				rom[(i-ROMBOT)/4] = read_word_quiet(i);
#else
				rom[(i-ROMBOT)/4] = read_word(i);
#endif

			i = fwrite(rom, ROMSIZE, 1, romfp);
			printf("Wrote %8zu bytes to %-29s "\
					"(Use 'hexdump -C' to view)\n",
					i*ROMSIZE, file);
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
			uint32_t ram[RAMSIZE >> 2] = {0};
			for (i = RAMBOT; i < RAMBOT+RAMSIZE; i += 4)
#ifdef DEBUG2
				ram[(i-RAMBOT)/4] = read_word_quiet(i);
#else
				ram[(i-RAMBOT)/4] = read_word(i);
#endif

			i = fwrite(ram, RAMSIZE, 1, ramfp);
			printf("Wrote %8zu bytes to %-29s "\
					"(Use 'hexdump -C' to view)\n",
					i*RAMSIZE, file);
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
				state_seek(target);
				print_full_state();
			}
			return _shell();
		}
#endif

		case 'q':
		case 't':
			sim_terminate();

		case 'r':
		{
			const char *file;

#if defined (HAVE_ROM) && defined (PRINT_ROM_EN)
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
				assert(-1 != asprintf(&cmd, "hexdump -C %s", file));
				FILE *out = popen(cmd, "r");

				char buf[100];
				while ( fgets(buf, 99, out) ) {
					printf("%s", buf);
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
#if defined (HAVE_ROM) && defined (PRINT_ROM_EN)
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

EXPORT void CORE_WARN_real(const char *f, int l, const char *msg) {
	WARN("%s:%d\t%s\n", f, l, msg);
}

EXPORT void CORE_ERR_read_only_real(const char *f, int l, uint32_t addr) {
	print_full_state();
	ERR(E_READONLY, "%s:%d\t%#08x is read-only\n", f, l, addr);
}

EXPORT void CORE_ERR_write_only_real(const char *f, int l, uint32_t addr) {
	print_full_state();
	ERR(E_WRITEONLY, "%s:%d\t%#08x is write-only\n", f, l, addr);
}

EXPORT void CORE_ERR_invalid_addr_real(const char *f, int l, uint8_t is_write, uint32_t addr) {
	static bool dumping = false;
	if (dumping) {
		WARN("Err generating core dump, aborting\n");
	} else {
		dumping = true;
		WARN("CORE_ERR_invalid_addr %s address: 0x%08x\n",
				is_write ? "writing":"reading", addr);
		WARN("Dumping Core...\n");
		print_full_state();
	}
	ERR(E_INVALID_ADDR, "%s:%d\tTerminating due to invalid addr\n", f, l);
}

EXPORT void CORE_ERR_illegal_instr_real(const char *f, int l, uint32_t inst) {
	WARN("CORE_ERR_illegal_instr, inst: %08x\n", inst);
	WARN("Dumping core...\n");
	print_full_state();
	ERR(E_UNKNOWN, "%s:%d\tUnknown inst\n", f, l);
}

EXPORT void CORE_ERR_unpredictable_real(const char *f, int l, const char *opt_msg) {
	ERR(E_UNPREDICTABLE, "%s:%d\tCORE_ERR_unpredictable -- %s\n", f, l, opt_msg);
}

EXPORT void CORE_ERR_not_implemented_real(const char *f, int l, const char *opt_msg) {
	ERR(E_NOT_IMPLEMENTED, "%s:%d\tCORE_ERR_not_implemented -- %s\n", f, l, opt_msg);
}


////////////////////////////////////////////////////////////////////////////////
// SIMULATOR
////////////////////////////////////////////////////////////////////////////////

#if _POSIX_TIMERS > 0
static struct timespec last_cycle_time;
#elif __APPLE__
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
static uint64_t last_cycle_time;
static mach_timebase_info_data_t sTimebaseInfo;
#endif

static void sim_delay_reset() {
#if _POSIX_TIMERS > 0
	// Perform check here to verify system (i) has CLOCK_REALTIME and (ii)
	// supports clock_gettime. This prevents requiring the check in the
	// main loop.
	assert(0 == clock_gettime(CLOCK_REALTIME, &last_cycle_time));
	last_cycle_time.tv_nsec = -1;
#elif __APPLE__
	mach_timebase_info(&sTimebaseInfo);
	last_cycle_time = -1;
#endif
}

#if (_POSIX_TIMERS > 0) || (defined __APPLE__)
static void sim_delay_warn(long ns) {
	static int warn_count = 0;
	if (warn_count < 5) {
		WARN("Timing requirement missed\n");
		WARN("Cycle exceeded requested time by %ld ns\n", ns);
		warn_count++;
	} else if (warn_count++ == 5) {
		WARN("Timing requirement missed\n");
		WARN("Too many timing violations. Warnings suppressed\n");
	}
}
#endif

static void sim_delay(void) {
#if _POSIX_TIMERS > 0
	if (last_cycle_time.tv_nsec != -1) {
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
#elif __APPLE__
	/* https://developer.apple.com/library/mac/#qa/qa1398/_index.html */
	if (last_cycle_time != (uint64_t) -1) {
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

	uint32_t cur_pc = CORE_reg_read(PC_REG);
	if ((prev_pc == cur_pc) && (prev_pc != STALL_PC)) {
		DBG1("cycle: %d prev_pc: %08x cur_pc: %08x\n", cycle, prev_pc, cur_pc);
		if (GDB_ATTACHED) {
			INFO("Simulator determined PC 0x%08x is branch to self, breaking for gdb.\n", cur_pc);
			shell();
		} else {
			INFO("Simulator determined PC 0x%08x is branch to self, terminating.\n", cur_pc);
			sim_terminate();
		}
	} else {
		prev_pc = cur_pc;
	}

	cycle++;
	DBG2("Begin cycle %d.............................cycle %d\n", cycle, cycle);

#ifdef NO_PIPELINE
	// Not the common code path, try to catch if things have changed
	assert(NUM_TICKERS == 3);

	state_start_tick();
	DBG1("tick_if\n");
	tick_if();
	state_tock();

	state_start_tick();
	DBG1("tick_id\n");
	tick_id();
	state_tock();

	state_start_tick();
	DBG1("tick_ex\n");
	tick_ex();
	if (!state_handle_exceptions()) {
		state_tock();
	} else {
		CORE_ERR_not_implemented("excpetions (what to tock/save) in NO_PIPELINE");
		INFO("state_tock skipped due to exceptions\n");
	}
#else
	int i;

	// Start a clock tick
	state_start_tick();

	// Start off each stage
	for (i = 0; i < NUM_TICKERS; i++) {
		sem_post(start_tick_sem);
	}

	// Wait for all stages to complete
	for (i = 0; i < NUM_TICKERS; i++) {
		sem_wait(end_tick_sem);
	}

	stages_should_tock = !state_handle_exceptions();

	if (!stages_should_tock) {
		// Latch hardware
		state_tock();
	} else {
		assert(0 == state_tock());
	}

	// Notify stages this cycle is complete
	for (i = 0; i < NUM_TICKERS; i++) {
		sem_post(end_tock_sem);
	}

	for (i = 0; i < NUM_TICKERS; i++) {
		sem_wait(ticker_ready_sem);
	}
#endif

	__sync_synchronize();

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
	state_start_tick();
	reset();
	state_handle_exceptions();
	state_exit_debugging();
	__sync_synchronize();
	INFO("De-asserting reset pin\n");

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

EXPORT void sim_terminate(void) {
	join_periph_threads();
	INFO("Simulator shutdown successfully.\n");
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
	assert(0 == pthread_setname_np("signal handler"));
#else
	assert(0 == prctl(PR_SET_NAME, "signal handler", 0, 0, 0));
#endif

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
	struct periph_time_travel tt;
	volatile bool *en;
	void *arg;
	pthread_t pthread;
};

static struct periph_thread periph_threads;

EXPORT void register_periph_thread(
		pthread_t (*fn)(void *), struct periph_time_travel tt,
		volatile bool *en, void *arg) {
	if (periph_threads.fn == NULL) {
		periph_threads.fn = fn;
		periph_threads.tt = tt;
		periph_threads.en = en;
		periph_threads.arg = arg;
	} else {
		struct periph_thread *cur = &periph_threads;
		while (cur->next != NULL)
			cur = cur->next;
		cur->next = malloc(sizeof(struct periph_thread));
		cur = cur->next;
		cur->next = NULL;
		cur->fn = fn;
		cur->tt = tt;
		cur->en = en;
		cur->arg = arg;
	}
}

static void flash_image(const uint8_t *image, const uint32_t num_bytes){
#if defined (HAVE_ROM)
	if (ROMBOT == 0x0) {
		flash_ROM(image, num_bytes);
		return;
	}
#endif
	// Enter debugging to circumvent state-tracking code and write directly
	state_enter_debugging();
	unsigned i;
	for (i=0; i<num_bytes; i++) {
		write_byte(i, image[i]);
	}
	state_exit_debugging();
	INFO("Wrote %d bytes to memory\n", num_bytes);
}

EXPORT void simulator(const char *flash_file) {
	const char thread_name[16] = "Simulator main";
#ifdef __APPLE__
	assert(0 == pthread_setname_np(thread_name));
#else
	assert(0 == prctl(PR_SET_NAME, thread_name, 0, 0, 0));
#endif
	// Init uninit'd globals
#ifndef NO_PIPELINE
	{
		// OS X requires named semaphores, but we really don't want
		// them, so use PID to create a unique namespace for this
		// instance of the simulator
		char name_buf[32];

		sprintf(name_buf, "/%d-%s", getpid(), "ticker_ready_sem");
		ticker_ready_sem = sem_open(name_buf, O_CREAT|O_EXCL, 0600, 0);
		if (ticker_ready_sem == SEM_FAILED) {
			WARN("%s\n", strerror(errno));
			ERR(E_UNKNOWN, "Initilizing ticker_ready_sem\n");
		}

		sprintf(name_buf, "/%d-%s", getpid(), "start_tick_sem");
		start_tick_sem = sem_open(name_buf, O_CREAT|O_EXCL, 0600, 0);
		if (start_tick_sem == SEM_FAILED) {
			WARN("%s\n", strerror(errno));
			ERR(E_UNKNOWN, "Initilizing start_tick_sem\n");
		}

		sprintf(name_buf, "/%d-%s", getpid(), "end_tick_sem");
		end_tick_sem = sem_open(name_buf, O_CREAT|O_EXCL, 0600, 0);
		if (end_tick_sem == SEM_FAILED) {
			WARN("%s\n", strerror(errno));
			ERR(E_UNKNOWN, "Initilizing end_tick_sem\n");
		}

		sprintf(name_buf, "/%d-%s", getpid(), "end_tock_sem");
		end_tock_sem = sem_open(name_buf, O_CREAT|O_EXCL, 0600, 0);
		if (end_tock_sem == SEM_FAILED) {
			WARN("%s\n", strerror(errno));
			ERR(E_UNKNOWN, "Initilizing end_tock_sem\n");
		}
	}
#endif

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
			int flashfd = open(flash_file, O_RDONLY);
			ssize_t ret;

			if (-1 == flashfd) {
				ERR(E_BAD_FLASH, "Could not open '%s' for reading\n",
						flash_file);
			}

			struct stat flash_stat;
			if (0 != fstat(flashfd, &flash_stat)) {
				ERR(E_BAD_FLASH, "Could not get flash file size: %s\n",
						strerror(errno));
			}

			{
				uint8_t flash[flash_stat.st_size];

				ret = read(flashfd, flash, flash_stat.st_size);
				if (ret < 0) {
					WARN("%s\n", strerror(errno));
					ERR(E_BAD_FLASH, "Failed to read flash file '%s'\n",
							flash_file);
				}
				flash_image(flash, ret);
			}

			INFO("Succesfully loaded image: %s\n", flash_file);
		}
	}

	load_opcodes();

	// Prep signal-related stuff:
	signal(SIGPIPE, SIG_IGN);

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	assert(0 == pthread_sigmask(SIG_BLOCK, &set, NULL));

	// Spawn signal handling thread
	pthread_t sig_pthread;
	pthread_create(&sig_pthread, NULL, &sig_thread, (void *) &set);

	// Spawn peripheral threads
	if (periph_threads.fn != NULL) {
		struct periph_thread *cur = &periph_threads;
		while (cur != NULL) {
			*cur->en = true;
			cur->pthread = cur->fn(cur->arg);
			cur = cur->next;
		}
	}

	// Spawn pipeline stage threads
#ifndef NO_PIPELINE
	pthread_t ifstage_pthread;
	pthread_t idstage_pthread;
	pthread_t exstage_pthread;
	struct ticker_params ifstage_params = {
		.name = "IF ticker",
		.fn = tick_if,
		.always_tick = false,
	};
	struct ticker_params idstage_params = {
		.name = "ID ticker",
		.fn = tick_id,
		.always_tick = false,
	};
	struct ticker_params exstage_params = {
		.name = "EX ticker",
		.fn = tick_ex,
		.always_tick = true,
	};
	pthread_create(&ifstage_pthread, NULL, ticker, &ifstage_params);
	pthread_create(&idstage_pthread, NULL, ticker, &idstage_params);
	pthread_create(&exstage_pthread, NULL, ticker, &exstage_params);

	// Wait for threads to init; consume initial wakeups
	{
		int i;
		for (i = 0; i < NUM_TICKERS; i++) {
			sem_wait(ticker_ready_sem);
		}
	}
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
			*cur->en = false;
			pthread_join(cur->pthread, NULL);
			cur = cur->next;
		}
	}
}

