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

#include <sys/prctl.h>
#include <sys/stat.h>

// XXX: Temporary fix, see note at end of simulator.h
#define PP_STRING "---"
#include "pretty_print.h"
// ^^ need to resolve ordering
#include "state_sync.h"
#include "simulator.h"

#include "pipeline.h"
#include "if_stage.h"
#include "id_stage.h"
#include "ex_stage.h"
#include "cpu/core.h"
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
EXPORT sem_t ticker_ready_sem;
EXPORT sem_t start_tick_sem;
EXPORT sem_t end_tick_sem;
EXPORT sem_t end_tock_sem;
#define NUM_TICKERS	3	// IF, ID, EX

/* Peripherals */
static void join_periph_threads (void);

/* Config */
EXPORT int gdb_port = -1;
#define GDB_ATTACHED (gdb_port != -1)
EXPORT int slowsim = 0;
EXPORT int printcycles = 0;
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

	printf("[Cycle %d]\t\t\t", cycle);
	printf("\t  N: %d  Z: %d  C: %d  V: %d  ",
			!!(CORE_cpsr_read() & xPSR_N),
			!!(CORE_cpsr_read() & xPSR_Z),
			!!(CORE_cpsr_read() & xPSR_C),
			!!(CORE_cpsr_read() & xPSR_V)
	      );
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
				rom[(i-ROMBOT)/4] = read_word(i);

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
				ram[(i-RAMBOT)/4] = read_word(i);

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

		case 'q':
		case 't':
			exit(EXIT_SUCCESS);

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
			printf("   seek [INTEGER]	Seek to cycle\n");
			printf("                        (forward 1 cycle default)\n");
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
	WARN("CORE_ERR_illegal_instr, inst: %04x\n", inst);
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

static int opcode_masks;
EXPORT struct op *ops = NULL;

static int _register_opcode_mask(uint32_t ones_mask, uint32_t zeros_mask,
		bool is16, void *fn, const char* fn_name, va_list va_args) {
	struct op *o = malloc(sizeof(struct op));

	o->prev = NULL;
	o->next = ops;		// ops is NULL on first pass, this is fine
	o->ones_mask = ones_mask;
	o->zeros_mask = zeros_mask;
	o->is16 = is16;
	if (is16)
		o->fn.fn16 = fn;
	else
		o->fn.fn32 = fn;
	o->name = fn_name;

	o->ex_cnt = 0;
	o->ex_ones = NULL;
	o->ex_zeros = NULL;

	ones_mask  = va_arg(va_args, uint32_t);
	zeros_mask = va_arg(va_args, uint32_t);
	while (ones_mask || zeros_mask) {
		// Make the assumption that callers will have one, at most
		// two exceptions; go with the simple realloc scheme
		unsigned idx = o->ex_cnt;

		o->ex_cnt++;
		o->ex_ones  = realloc(o->ex_ones,  o->ex_cnt * sizeof(uint32_t));
		assert(NULL != o->ex_ones && "realloc");
		o->ex_zeros = realloc(o->ex_zeros, o->ex_cnt * sizeof(uint32_t));
		assert(NULL != o->ex_zeros && "realloc");

		o->ex_ones[idx]  = ones_mask;
		o->ex_zeros[idx] = zeros_mask;

		ones_mask  = va_arg(va_args, uint32_t);
		zeros_mask = va_arg(va_args, uint32_t);
	}

	// Add new element to list head b/c that's easy and it doesn't matter
	if (NULL == ops) {
		ops = o;
	} else {
		ops->prev = o;
		ops = o;
	}

	return ++opcode_masks;
}

static int register_opcode_mask_ex(uint32_t ones_mask, uint32_t zeros_mask,
		bool is16, void *fn, const char* fn_name, va_list va_args) {

	if (is16)
		assert(0xffff0000 == (0xffff0000 & zeros_mask));

	if (NULL == ops) {
		// first registration
		return _register_opcode_mask(ones_mask, zeros_mask,
				is16, fn, fn_name, va_args);
	}

	struct op* o = ops;

	// XXX: Make better
	while (NULL != o) {
		if (
				(o->ones_mask  == ones_mask) &&
				(o->zeros_mask == zeros_mask)
		   ) {
			break;
		}
		o = o->next;
	}
	if (o) {
		ERR(E_BAD_OPCODE, "Duplicate opcode mask.\n"\
				"\tExisting  registration: 1's %x, 0's %x (%s)\n"\
				"\tAttempted registration: 1's %x, 0's %x (%s)\n"\
				, o->ones_mask, ones_mask, o->name\
				, o->zeros_mask, zeros_mask, o->name);
	}

	return _register_opcode_mask(ones_mask, zeros_mask,
			is16, fn, fn_name, va_args);
}

EXPORT int register_opcode_mask_16_ex_real(uint16_t ones_mask, uint16_t zeros_mask,
		void (*fn) (uint16_t), const char *fn_name, ...) {
	va_list va_args;
	va_start(va_args, fn_name);

	int ret;

	ret = register_opcode_mask_ex(ones_mask, 0xffff0000 | zeros_mask,
			true, fn, fn_name, va_args);

	va_end(va_args);

	return ret;
}

EXPORT int register_opcode_mask_16_real(uint16_t ones_mask, uint16_t zeros_mask,
		void (*fn) (uint16_t), const char* fn_name) {
	return register_opcode_mask_16_ex_real(ones_mask, zeros_mask,
			fn, fn_name, 0, 0);
}

EXPORT int register_opcode_mask_32_ex_real(uint32_t ones_mask, uint32_t zeros_mask,
		void (*fn) (uint32_t), const char* fn_name, ...) {

	if ((zeros_mask & 0xffff0000) == 0) {
		WARN("%s registered zeros_mask requiring none of the top 4 bytes\n",
				fn_name);
		ERR(E_BAD_OPCODE, "Use register_opcode_mask_16 instead");
	}

	va_list va_args;
	va_start(va_args, fn_name);

	int ret;
	ret = register_opcode_mask_ex(ones_mask, zeros_mask,
			false, fn, fn_name, va_args);

	va_end(va_args);

	return ret;
}

EXPORT int register_opcode_mask_32_real(uint32_t ones_mask, uint32_t zeros_mask,
		void (*fn) (uint32_t), const char *fn_name) {
	return register_opcode_mask_32_ex_real(ones_mask, zeros_mask, fn, fn_name, 0, 0);
}

static int sim_execute(void) {
	// XXX: What if the debugger wants to execute the same instruction two
	// cycles in a row? How do we allow this?
	static uint32_t prev_pc = STALL_PC;

	uint32_t cur_pc = CORE_reg_read(PC_REG);
	if ((prev_pc == cur_pc) && (prev_pc != STALL_PC)) {
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
#ifdef NO_PIPELINE
	// Not the common code path, try to catch if things have changed
	assert(NUM_TICKERS == 3);

	cycle++;

	state_start_tick();
	DBG1("tick_if\n");
	tick_if();
	DBG1("tick_id\n");
	tick_id();
	DBG1("tick_ex\n");
	tick_ex();
	state_tock();

#else
	// Now we're committed to starting the next cycle
	cycle++;

	int i;

	// Start a clock tick
	state_start_tick();

	// Don't let stages steal wakeups
	for (i = 0; i < NUM_TICKERS; i++) {
		sem_wait(&ticker_ready_sem);
	}

	// Start off each stage
	for (i = 0; i < NUM_TICKERS; i++) {
		sem_post(&start_tick_sem);
	}

	// Wait for all stages to complete
	for (i = 0; i < NUM_TICKERS; i++) {
		sem_wait(&end_tick_sem);
	}

	// Latch hardware
	state_tock();

	// Notify stages this cycle is complete
	for (i = 0; i < NUM_TICKERS; i++) {
		sem_post(&end_tock_sem);
	}
#endif

	return SUCCESS;
}

static void sim_reset(void) __attribute__ ((noreturn));
static void sim_reset(void) {
	int ret;

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
	cycle = 0;
	state_start_tick();
	reset();
	state_tock();
	INFO("De-asserting reset pin\n");

	INFO("Entering main loop...\n");
	do {
		if (slowsim) {
			static struct timespec s = {0, NSECS_PER_SEC/10};
			nanosleep(&s, NULL);
		}

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
	print_full_state();
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

static void load_opcodes(void) {
	// N.B. These are registered before main via constructor attributes
	INFO("Registered %d opcode mask%s\n", opcode_masks,
			(opcode_masks == 1) ? "":"s");

#ifndef NO_PIPELINE
	// Fake instructions used to propogate pipeline exceptions
	register_opcode_mask_32_real(INST_HAZARD, ~INST_HAZARD, pipeline_exception, "Pipeline Excpetion");
#endif
}

static void* sig_thread(void *arg) {
	sigset_t *set = (sigset_t *) arg;
	int s, sig;

	prctl(PR_SET_NAME, "signal hander", 0, 0, 0);

	for (;;) {
		s = sigwait(set, &sig);
		if (s != 0) {
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
	volatile bool *en;
	void *arg;
	pthread_t pthread;
};

static struct periph_thread periph_threads;

EXPORT void register_periph_thread(
		pthread_t (*fn)(void *), volatile bool *en, void *arg) {
	if (periph_threads.fn == NULL) {
		periph_threads.fn = fn;
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
	// Init uninit'd globals
#ifndef NO_PIPELINE
	assert(0 == sem_init(&ticker_ready_sem, 0, 0));
	assert(0 == sem_init(&start_tick_sem, 0, 0));
	assert(0 == sem_init(&end_tick_sem, 0, 0));
	assert(0 == sem_init(&end_tock_sem, 0, 0));
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
	pthread_create(&ifstage_pthread, NULL, ticker, tick_if);
	pthread_create(&idstage_pthread, NULL, ticker, tick_id);
	pthread_create(&exstage_pthread, NULL, ticker, tick_ex);
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

