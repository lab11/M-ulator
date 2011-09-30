#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>

#include "cortex_m3.h"		// defines M_PROFILE
#include "operations/opcodes.h"

/////////////
// PROFILE //
/////////////

// In my dreams, this could ultimately grow to
// support A, R, or M profile processors. That's
// a long way out in reality, but let's make some
// effort to codify some differences as we go

#if defined (M_PROFILE)
#if defined (R_PROFILE)
#error "Must choose one of M or R profiles, not both"
#endif // R
#if defined (A_PROFILE)
#error "Must choose one of M or A profiles, not both"
#endif // A
#endif // M
#if defined (R_PROFILE) && defined (A_PROFILE)
#error "Must choose one of R or A profiles, not both"
#endif // R && A


/////////////////////////
// PRETTY PRINT MACROS //
/////////////////////////

#define INFO(...) printf("--- I: "); printf(__VA_ARGS__)
#define WARN(...) fprintf(stderr, "--- W: "); fprintf(stderr, __VA_ARGS__)
#define ERR(_e, ...)\
	do {\
		fprintf(stderr, "--- E: ");\
		fprintf(stderr, __VA_ARGS__);\
		if (raiseonerror) raise(SIGTRAP);\
		exit(_e);\
	} while (0)

#ifdef DEBUG2
#ifndef GRADE
#define GRADE
#endif
#endif

#ifdef GRADE

#define READ 0
#define WRITE 1

#define LED  "leds"
#define UART "uart"

#define G_REG(_rw, _reg)\
	do {\
		if (_rw) {\
			printf("G~G G: "); \
			printf("%c %02d %x\n", _rw ? 'w':'r', _reg, reg[_reg]);\
		}\
	} while (0)
#define G_CPSR(_rw)\
	do {\
		if (_rw) {\
			printf("G~G C: "); \
			printf("%c %d %d %d %d %08x\n", _rw ? 'w':'r',\
					!!(CPSR & 0x80000000),\
					!!(CPSR & 0x40000000),\
					!!(CPSR & 0x20000000),\
					!!(CPSR & 0x10000000),\
					(CPSR));\
		}\
	} while (0)
#define G_IPSR(_rw)\
	do {\
		if (_rw) {\
			printf("G~G I: "); \
			printf("%c %x\n", _rw ? 'w':'r', IPSR);\
		}\
	} while (0)
#define G_EPSR(_rw)\
	do {\
		if (_rw) {\
			printf("G~G E: "); \
			printf("%c %x\n", _rw ? 'w':'r', EPSR);\
		}\
	} while (0)
#define G_ROM(_rw, _addr) printf("G~G O: "); \
	printf("%c %08x %x\n", _rw ? 'w':'r',\
			_addr, rom[ADDR_TO_IDX(_addr, ROMBOT)]);
#define G_RAM(_rw, _addr) printf("G~G A: "); \
	printf("%c %08x %x\n", _rw ? 'w':'r',\
			_addr, ram[ADDR_TO_IDX(_addr, RAMBOT)]);
#define G_PERIPH(_rw, _type, _idx) printf("G~G P: "); \
	printf("%c %s %d\n", _rw ? 'w':'r', _type, _idx);
#else
#define G_REG(...)
#define G_CPSR(...)
#define G_IPSR(...)
#define G_EPSR(...)
#define G_ROM(...)
#define G_RAM(...)
#define G_PERIPH(...)
#endif

static void usage_fail(int retcode) {
	printf("\nUSAGE: ./simulator [OPTS]\n\n");
	printf("\
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
\t-r, --returnr0\n\
\t\tSets simulator binary return code to the return\n\
\t\tcode of the executed program on simulator exit\n\
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

////////////////////////////////////////////////////////////////////////////////
// GLOBALS
////////////////////////////////////////////////////////////////////////////////

#define NSECS_PER_SEC 1000000000

/* UARTs */
#define POLL_UART_PORT 4100
#define POLL_UART_BUFSIZE 16
#define POLL_UART_BAUD 1200
static void *poll_uart_thread(void *);
static pthread_mutex_t poll_uart_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  poll_uart_cond  = PTHREAD_COND_INITIALIZER;
static int poll_uart_client = -1;

// circular buffer
// head is location to read valid data from
// tail is location to write data to
// characters are lost if not read fast enough
// head == tail --> buffer is full (not that it matters)
// head == NULL --> buffer if empty
static char poll_uart_buffer[POLL_UART_BUFSIZE];
static char *poll_uart_head = NULL;
static char *poll_uart_tail = poll_uart_buffer;
static const struct timespec poll_uart_baud_sleep =\
		{0, (NSECS_PER_SEC/POLL_UART_BAUD)*8};	// *8 bytes vs bits

uint8_t poll_uart_status_read();
void poll_uart_status_write();
uint8_t poll_uart_rxdata_read();
void poll_uart_txdata_write(uint8_t val);

/* Config */

static int slowsim = 0;
#ifdef DEBUG2
static int printcycles = 1;
static int raiseonerror = 1;
#else
static int printcycles = 0;
static int raiseonerror = 0;
#endif
static int showledwrites = 0;
static int dumpatpc = -1;
static int dumpatcycle = -1;
static int dumpallcycles = 0;
#ifdef GRADE
static int returnr0 = 1;
#else
static int returnr0 = 0;
#endif

/* Test Flash */

// programs/bintoarray.sh
#define STATIC_ROM_NUM_BYTES (80 * 4)
static uint32_t static_rom[80] = {0x20003FFC,0x55,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0xBF00E7FE,0xF862F000,0xBF00E7FA,0xB083B480,0xF244AF00,0xF2C41300,0x603B0300,0x1304F244,0x300F2C4,0x683B607B,0xB2DB781B,0x302F003,0xD0F82B00,0x781B687B,0x4618B2DB,0x70CF107,0xBC8046BD,0xBF004770,0xB085B480,0x4603AF00,0xF24471FB,0xF2C41300,0x60BB0300,0x1308F244,0x300F2C4,0x68BB60FB,0xB2DB781B,0x304F003,0xD1F82B00,0x79FA68FB,0xF107701A,0x46BD0714,0x4770BC80,0xB082B580,0x6078AF00,0x687BE008,0x4618781B,0xFFD8F7FF,0xF103687B,0x607B0301,0x781B687B,0xD1F22B00,0x708F107,0xBD8046BD,0x181B4B05,0x2A006818,0x2201BF03,0x4390408A,0x60184310,0x30004770,0x40001000,0xB082B580,0xF7FFAF00,0x4603FF9B,0x79FB71FB,0xF7FF4618,0xF04FFFB3,0x46180300,0x708F107,0xBD8046BD};

/* State */

static int opcode_masks;

static int cycle = -1;

static uint32_t rom[ROMSIZE >> 2] = {0};
static uint32_t ram[RAMSIZE >> 2] = {0};

#define ADDR_TO_IDX(_addr, _bot) ((_addr - _bot) >> 2)

/* Ignore mode transitions for now */
#define ASSERT_VALID_REG(_r) assert(_r >= 0 && _r < 16)
static uint32_t reg[16];

#define SP	(reg[SP_REG])
#define LR	(reg[LR_REG])
#define PC	(reg[PC_REG])

#ifdef A_PROFILE
// (|= 0x0010) Start is user mode
// (|= 0x0020) CPU only supports thumb (for now)
static uint32_t apsr = 0x0030;

// "Current" program status register
//   0-4: M[4:0]	// Mode field: 10000 user  10001 FIQ 10010 IRQ 10011 svc
			//             10111 abort 11011 und 11111 sys
//     5: T		// Thumb bit
//     6: F		// Fast Interrupt disable (FIQ interrupts)
//     7: I		// Interrupt disable (IRQ interrupts)
//     8: A		// Asynchronous abort disable
//     9: E		// Endianness execution state bit (0: little, 1: big)
// 10-15: IT[7:2]	// it-bits
// 16-19: GE[3:0]	// Greater than or equal flags for SIMD instructions
// 20-23: <reserved>
//    24: J		// Jazelle bit
// 25-26: IT[1:0]	// it-bits
//    27: Q		// Cumulative saturation
//    28: V		// Overflow
//    29: C		// Carry
//    30: Z		// Zero
//    31: N		// Negative
static uint32_t *cpsr = &apsr;
#define CPSR		(*cpsr)
#define	T_BIT		(CPSR & 0x0020)

#define ITSTATE			( ((CPSR & 0xfc00) >> 8) | ((CPSR & 0x06000000) >> 25) )

#endif // A

#ifdef M_PROFILE

static uint32_t apsr;
//  0-15: <reserved>
// 16-19: GE[3:0]	// for DSP extension
// 20-26: <reserved>
//    27: Q		// Cumulative saturation
//    28: V		// Overflow
//    29: C		// Carry
//    30: Z		// Zero
//    31: N		// Negative
static uint32_t ipsr = 0x0;
//   0-8: 0 or Exception Number
//  9-31: <reserved>
static uint32_t epsr = 0x01000000;
//   0-9: <reserved>
// 10-15: ICI/IT	//
// 16-23: <reserved>
//    24: T		// Thumb bit
// 25-26: ICI/IT	//
// 27-31: <reserved>
#define T_BIT		(epsr & 0x01000000)

#define CPSR		(apsr)
#define APSR		(apsr)
#define IPSR		(ipsr)
#define EPSR		(epsr)
#define IAPSR		(IPSR | APSR)
#define EAPSR		(EPSR | APSR)
#define XPSR		(IPSR | APSR | EPSR)
#define IEPSR		(IPSR | EPSR)

#define ITSTATE		( ((EPSR & 0xfc00) >> 8) | ((EPSR & 0x06000000) >> 25) )

static uint32_t primask __attribute__ ((unused)) = 0x0;
//     0: priority	The exception mask register, a 1-bit register.
//			Setting PRIMASK to 1 raises the execution priority to 0.
static uint32_t basepri __attribute__ ((unused)) = 0x0;
/* The base priority mask, an 8-bit register. BASEPRI changes the priority
 * level required for exception preemption. It has an effect only when BASEPRI
 * has a lower value than the unmasked priority level of the currently
 * executing software.  The number of implemented bits in BASEPRI is the same
 * as the number of implemented bits in each field of the priority registers,
 * and BASEPRI has the same format as those fields.  For more information see
 * Maximum supported priority value on page B1-636.  A value of zero disables
 * masking by BASEPRI.
 */
static uint32_t faultmask __attribute__ ((unused)) = 0x0;
/* The fault mask, a 1-bit register. Setting FAULTMASK to 1 raises the
 * execution priority to -1, the priority of HardFault. Only privileged
 * software executing at a priority below -1 can set FAULTMASK to 1. This means
 * HardFault and NMI handlers cannot set FAULTMASK to 1. Returning from any
 * exception except NMI clears FAULTMASK to 0.
 */

static uint32_t control __attribute__ ((unused)) = 0x0;
//     0: nPRIV, thread mode only (0 == privileged, 1 == unprivileged)
//     1: SPSEL, thread mode only (0 == use SP_main, 1 == use SP_process)
//     2: FPCA, (1 if FP extension active)

#endif // M


/* Peripherals */
#define RED 0
#define GRN 1
#define BLU 2
#define LED_COLORS 3
static uint32_t leds[LED_COLORS] = {0};

////////////////////////////////////////////////////////////////////////////////
// STATUS FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

#define DIVIDERe printf("================================================================================\n")
#define DIVIDERd printf("--------------------------------------------------------------------------------\n")

static void print_leds_line(void) {
	int i;

	printf("Cycle: %8d ", cycle);

	printf("| R: ");
	for (i = 0; i < 8; i++)
		printf("%d ", !!(leds[RED] & (1 << i)));

	printf("| G: ");
	for (i = 0; i < 8; i++)
		printf("%d ", !!(leds[GRN] & (1 << i)));

	printf("| B: ");
	for (i = 0; i < 8; i++)
		printf("%d ", !!(leds[BLU] & (1 << i)));

	printf("|\n");
}

static void print_reg_state_internal(void) {
	int i;

	printf("Registers:\t\t\t");
	printf("\t  N: %d  Z: %d  C: %d  V: %d  ",
			!!(CPSR & xPSR_N),
			!!(CPSR & xPSR_Z),
			!!(CPSR & xPSR_C),
			!!(CPSR & xPSR_V)
	      );
	printf("| ITSTATE: %02x\n", ITSTATE);
	for (i=0; i<16; ) {
		printf("\tr%02d: %8x\tr%02d: %8x\tr%02d: %8x\tr%02d: %8x\n",
				i, reg[i], i+1, reg[i+1],
				i+2, reg[i+2], i+3, reg[i+3]);
		i+=4;
	}
}

static void print_reg_state(void) {
	DIVIDERe;
	print_leds_line();
	DIVIDERd;
	print_reg_state_internal();
}

static void print_full_state(void) {
	int i;

	DIVIDERe;
	print_leds_line();

	DIVIDERd;
	print_reg_state_internal();
	DIVIDERd;
/*
	FILE* romfp = fopen("/tmp/373rom", "w");
	if (romfp) {
		i = fwrite(rom, ROMSIZE, 1, romfp);
		printf("Wrote %8d bytes to /tmp/373rom "\
				"\t\t(Use 'hexdump -C' to view)\n", i*ROMSIZE);
		fclose(romfp);
	} else {
		perror("Failed to open /tmp/373rom for writing -- No ROM dump");
	}
*/
	FILE* ramfp = fopen("/tmp/373ram", "w");
	if (ramfp) {
		i = fwrite(ram, RAMSIZE, 1, ramfp);
		printf("Wrote %8d bytes to /tmp/373ram "\
				"\t\t(Use 'hexdump -C' to view)\n", i*RAMSIZE);
		fclose(ramfp);
	} else {
		perror("Failed to open /tmp/373ram for writing -- No RAM dump");
	}

	DIVIDERe;
}

// This needs to start splitting into multiple source files
// project ease v. quality; added to fixes
static int state_seek(int);

static void shell(void) {
	static char buf[100];

	// protect <EOF> replay
	buf[0] = '\0';
	printf("> ");
	fgets(buf, 100, stdin);

	switch (buf[0]) {
		case '\n':
			dumpatpc = 0;
			dumpatcycle = cycle + 1;
			return;

		case 'p':
			sscanf(buf, "%*s %x", &dumpatpc);
			return;

		case 'r':
		{
			int ret;
			int target;
			ret = sscanf(buf, "%*s %d", &target);

			if (-1 == ret)
				target = cycle - 1;
			else
				assert(1 == ret);

			if (target < 0) {
				WARN("Ignoring seek to negative cycle\n");
			} else {
				state_seek(target);
				print_full_state();
			}
			return shell();
		}

		case 'q':
		case 't':
			exit(EXIT_SUCCESS);

		case 'c':
			if (buf[1] == 'y') {
				sscanf(buf, "%*s %d", &dumpatcycle);
				return;
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
			printf("   rewind [INTEGER]	Rewind to cycle\n");
			printf("                        (back 1 cycle default\n");
			printf("   continue		Continue\n");
			printf("   terminate		Terminate Simulation\n");
			return shell();
	}
}

////////////////////////////////////////////////////////////////////////////////
// CORE
////////////////////////////////////////////////////////////////////////////////

struct state_change {
	struct state_change *prev;
	struct state_change *next;

	/* Holds changes made __during__ this cycle; there may be multiple
	   list entries per cycle. Thus, to return to the beginning of a
	   cycle n (e.g. before it had executed), the list must have traversed
	   such that the list head reaches an entry of cycle less than n */
	int cycle;
	uint32_t *loc;
	uint32_t prev_val;
};

static struct state_change state_start = {
	.prev = NULL,
	.next = NULL,
	.cycle = -1,
	.loc = NULL,
	.prev_val = 0,
};

static struct state_change state_io_barrier_state;

static struct state_change* state_head = &state_start;

static void state_write(uint32_t *loc, uint32_t val) {
	if (
			(state_head == &state_io_barrier_state) &&
			(state_head->cycle == cycle)
	   ) {
		// This write is in a barrier state, we can't rewind
		// it anyways, so don't bother recording it
		;
	} else {
		// Record:

		struct state_change* s = malloc(sizeof(struct state_change));

		// To write new state, we must be either executing the same cycle, or
		// have advanced to the next cycle; o/w we lost synchronization somehow
		assert((state_head->cycle == cycle) || ((state_head->cycle+1) == cycle));

		DBG2("cycle: %08d\tloc %p val %08x\n", cycle, loc, val);

		s->prev = state_head;
		s->next = NULL;
		s->cycle = cycle;
		s->loc = loc;
		s->prev_val = *loc;
		state_head->next = s;
		state_head = s;
	}

	// Now let's not forget to actually update the real state
	*loc = val;
}

// Returns 0 on success
// Returns >0 on tolerable error (e.g. seek past end)
// Returns <0 on catastrophic error
static int state_seek(int target_cycle) {
	// This assertion relies on *something* being written every cycle,
	// which should hold since the PC is written every cycle at least.
	// However, if we ever go cycle-accurate, much of the state_seek
	// logic will require a revisit
	assert(state_head->cycle == cycle);

	if (target_cycle > cycle) {
		/* Could support replay easily if we hold a "new_val" field,
		   but that's a little silly when re-executing instr's is
		   equally easy; eh, code's here if I want it someday:

		DBG2("seeking forward from %d to %d\n", cycle, target_cycle);
		while (target_cycle > cycle) {
			if (state_head->next == NULL) {
				WARN("Request to seek to cycle %d failed\n",
						target_cycle);
				WARN("State is only known up to cycle %d\n",
						state_head->cycle);
				WARN("Simulator left at cycle %d\n", cycle);
				return 1;
			}

			state_head = state_head->next;
			*(state_head->loc) = state_head->val;
			if (
					(NULL == state_head->next) ||
					(state_head->next->cycle > state_head->cycle)
			   ) {
				cycle++;
			}
		}

		return 0;
		*/
		WARN("Request to seek into the future ignored\n");
		return 1;
	} else if (target_cycle == cycle) {
		WARN("Request to seek to current cycle ignored\n");
		return 1;
	} else {
		DBG2("seeking backward from %d to %d\n", cycle, target_cycle);
		while (state_head->cycle > target_cycle) {
			if (state_head == &state_io_barrier_state) {
				WARN("Cannot rewind past I/O access\n");
				WARN("Simulator left at cycle %08d\n", cycle);
				return 1;
			}

			assert(NULL != state_head->prev);

			DBG2("cycle: %d target: %d head: %d\n",
					cycle, target_cycle, state_head->cycle);

			DBG2("Resetting %p to %08x\n",
					state_head->loc, state_head->prev_val);
			*(state_head->loc) = state_head->prev_val;

			state_head = state_head->prev;
			free(state_head->next);
			state_head->next = NULL;

			cycle = state_head->cycle;
		}

		return 0;
	}
}

static void state_io_barrier(void) {
	// List behind this point is lost, but memory is cheap and this
	// isn't permanent anyway
	state_io_barrier_state.cycle = cycle;
	state_head->next = &state_io_barrier_state;
	state_head = state_head->next;
}

/* These are the functions called into by the student simulator project */

/* ARMv7-M implementations treat SP bits [1:0] as RAZ/WI.
 * ARM strongly recommends that software treats SP bits [1:0]
 * as SBZP for maximum portability across ARMv7 profiles.
 */

uint32_t CORE_reg_read(int r) {
	assert(r >= 0 && r < 16 && "CORE_reg_read");
	G_REG(READ, r);
	if (r == PC_REG)
		return PC & 0xfffffffe;
	else if (r == SP_REG)
		return SP & 0xfffffffc;
	else
		return reg[r];
}

void CORE_reg_write(int r, uint32_t val) {
	assert(r >= 0 && r < 16 && "CORE_reg_write");
	if (r == PC_REG)
		state_write(&PC, val & 0xfffffffe);
	else if (r == SP_REG)
		state_write(&SP, val & 0xfffffffc);
	else
		state_write(&(reg[r]), val);
	G_REG(WRITE, r);
}

uint32_t CORE_cpsr_read(void) {
	G_CPSR(READ);
	return CPSR;
}

void CORE_cpsr_write(uint32_t val) {
	if (in_ITblock(ITSTATE)) {
		DBG1("WARN update of cpsr in IT block\n");
	}
#ifdef M_PROFILE
	if (val & 0x07f0ffff) {
		DBG1("WARN update of reserved CPSR bits\n");
	}
#endif
	state_write(&CPSR, val);
	G_CPSR(WRITE);
}

#ifdef M_PROFILE
uint32_t CORE_ipsr_read(void) {
	G_IPSR(READ);
	return IPSR;
}

void CORE_ipsr_write(uint32_t val) {
	state_write(&IPSR, val);
	G_IPSR(WRITE);
}

uint32_t CORE_epsr_read(void) {
	G_EPSR(READ);
	return EPSR;
}

void CORE_epsr_write(uint32_t val) {
	state_write(&EPSR, val);
	G_EPSR(WRITE);
}
#endif

uint32_t CORE_rom_read(uint32_t addr) {
	DBG2("ROM Read request addr %x (idx: %d)\n", addr, ADDR_TO_IDX(addr, ROMBOT));
#ifdef DEBUG1
	assert((addr >= ROMBOT) && (addr < ROMTOP) && "CORE_rom_read");
#endif
	if ((addr >= ROMBOT) && (addr < ROMTOP) && (0 == (addr & 0x3))) {
		G_ROM(READ, addr);
		return rom[ADDR_TO_IDX(addr, ROMBOT)];
	} else {
		CORE_ERR_invalid_addr(false, addr);
	}
}

uint32_t CORE_ram_read(uint32_t addr) {
#ifdef DEBUG1
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_read");
#endif
	if ((addr >= RAMBOT) && (addr < RAMTOP) && (0 == (addr & 0x3))) {
		G_RAM(READ, addr);
		return ram[ADDR_TO_IDX(addr, RAMBOT)];
	} else {
		CORE_ERR_invalid_addr(false, addr);
	}
}

void CORE_ram_write(uint32_t addr, uint32_t val) {
	DBG2("RAM Write request addr %x (idx: %d)\n", addr, ADDR_TO_IDX(addr, RAMBOT));
#ifdef DEBUG1
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_write");
#endif
	if ((addr >= RAMBOT) && (addr < RAMTOP) && (0 == (addr & 0x3))) {
		state_write(&ram[ADDR_TO_IDX(addr, RAMBOT)],val);
		G_RAM(WRITE, addr);
	} else {
		CORE_ERR_invalid_addr(true, addr);
	}
}

uint32_t CORE_red_led_read(void) {
	G_PERIPH(READ, LED, RED);
	return leds[RED];
}

void CORE_red_led_write(uint32_t val) {
	G_PERIPH(WRITE, LED, RED);
	state_write(&leds[RED],val);
	if (showledwrites)
		print_leds_line();
}

uint32_t CORE_grn_led_read(void) {
	G_PERIPH(READ, LED, GRN);
	return leds[GRN];
}

void CORE_grn_led_write(uint32_t val) {
	G_PERIPH(WRITE, LED, GRN);
	state_write(&leds[GRN],val);
	if (showledwrites)
		print_leds_line();
}

uint32_t CORE_blu_led_read(void) {
	G_PERIPH(READ, LED, BLU);
	return leds[BLU];
}

void CORE_blu_led_write(uint32_t val) {
	G_PERIPH(WRITE, LED, BLU);
	state_write(&leds[BLU],val);
	if (showledwrites)
		print_leds_line();
}

uint8_t CORE_poll_uart_status_read() {
	state_io_barrier();
	return poll_uart_status_read();
}

void CORE_poll_uart_status_write(uint8_t val) {
	state_io_barrier();
	return poll_uart_status_write(val);
}

uint8_t CORE_poll_uart_rxdata_read() {
	state_io_barrier();
	return poll_uart_rxdata_read();
}

void CORE_poll_uart_txdata_write(uint8_t val) {
	state_io_barrier();
	return poll_uart_txdata_write(val);
}

void CORE_ERR_invalid_addr(uint8_t is_write, uint32_t addr) {
	WARN("CORE_ERR_invalid_addr %s address: %08x\n",
			is_write ? "writing":"reading", addr);
	WARN("Dumping Core...\n");
	print_full_state();
	ERR(E_INVALID_ADDR, "Terminating due to invalid addr\n");
}

void CORE_ERR_illegal_instr(uint32_t inst) {
	WARN("CORE_ERR_illegal_instr, inst: %04x\n", inst);
	WARN("Dumping core...\n");
	print_full_state();
	ERR(E_UNKNOWN, "Unknown inst\n");
}

void CORE_ERR_illegal_line(const char *line) {
	WARN("CORE_ERR_illegal_line: %s\n", line);
	WARN("Dumping core...\n");
	print_full_state();
	ERR(E_UNKNOWN, "This is a bug in your simulator.\n");
}

void CORE_ERR_unpredictable(const char *opt_msg) {
	ERR(E_UNPREDICTABLE, "CORE_ERR_unpredictable -- %s\n", opt_msg);
}

void CORE_ERR_not_implemented(const char *opt_msg) {
	ERR(E_NOT_IMPLEMENTED, "CORE_ERR_not_implemented -- %s\n", opt_msg);
}


////////////////////////////////////////////////////////////////////////////////
// SIMULATOR
////////////////////////////////////////////////////////////////////////////////

/*
static uint16_t read_addr16(uint32_t addr) {
	if (0x1 & (addr >> 1)) {
		DBG2("Returning upper half of %x (%x)\n",
				(addr & 0xfffc), (addr & 0xfffc) + 2);
		return (read_word(addr - 2) >> 16);
	} else {
		DBG2("Returning lower half of %x\n", addr);
		return (read_word(addr) & 0xffff);
	}
}
*/

struct op {
	struct op *prev;
	struct op *next;
	uint32_t ones_mask;
	uint32_t zeros_mask;
	int ex_cnt;
	uint32_t *ex_ones;
	uint32_t *ex_zeros;
	int (*fn) (uint32_t);
	const char *name;
};

static struct op *ops = NULL;

static struct op* _find_op(struct op* o, uint32_t inst) {
	while (NULL != o) {
		if (
				(o->ones_mask  == (o->ones_mask  & inst)) &&
				(o->zeros_mask == (o->zeros_mask & ~inst))
		   ) {
			// The mask matched, now verify there isn't an exception
			int i;
			bool exception = false;
			for (i = 0; i < o->ex_cnt; i++) {
				uint32_t ones_mask  = o->ex_ones[i];
				uint32_t zeros_mask = o->ex_zeros[i];
				if (
						(ones_mask  == (ones_mask  & inst)) &&
						(zeros_mask == (zeros_mask & ~inst))
				   ) {
					DBG2("Collision averted via exception\n");
					exception = true;
				}
			}

			if (!exception)
				break;
		}
		o = o->next;
	}

	return o;
}

static struct op* find_op(uint32_t inst, bool reorder) {
	struct op *o = ops;
	o = _find_op(o, inst);

	if (NULL != o) {
		// Check for ambiguity, and let's report all of them now
		struct op* t = _find_op(o->next, inst);
		bool ambiguous = false;

		while (NULL != t) {
			WARN("AMB\t%s:\tones %08x zeros %08x\n", t->name,
					t->ones_mask, t->zeros_mask);
			ambiguous = true;
			t = _find_op(t->next, inst);
		}

		if (ambiguous) {
			WARN("AMB\t%s:\tones %08x zeros %08x\n", o->name,
					o->ones_mask, o->zeros_mask);
			WARN("AMB\tAmbiguous instruction detected!\n");
			WARN("Instruction %08x at PC %08x matched multiple masks\n",
					inst,
					(inst & 0xffff0000) ? PC-4 : PC-2);
			CORE_ERR_illegal_instr(inst);
		}
	}

	if (o && reorder) {
		// ...
	}

	return o;
}

static int _register_opcode_mask(uint32_t ones_mask, uint32_t zeros_mask,
		int (*fn) (uint32_t), const char* fn_name, va_list va_args) {
	struct op *o = malloc(sizeof(struct op));

	o->prev = NULL;
	o->next = ops;		// ops is NULL on first pass, this is fine
	o->ones_mask = ones_mask;
	o->zeros_mask = zeros_mask;
	o->fn = fn;
	o->name = fn_name;

	o->ex_cnt = 0;
	o->ex_ones = NULL;
	o->ex_zeros = NULL;

	ones_mask  = va_arg(va_args, uint32_t);
	zeros_mask = va_arg(va_args, uint32_t);
	while (ones_mask || zeros_mask) {
		// Make the assumption that callers will have one, at most
		// two exceptions; go with the simple realloc scheme
		int idx = o->ex_cnt;

		o->ex_cnt++;
		o->ex_ones  = realloc(o->ex_ones,  o->ex_cnt * sizeof(uint32_t));
		o->ex_zeros = realloc(o->ex_zeros, o->ex_cnt * sizeof(uint32_t));

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

	va_end(va_args);

	return ++opcode_masks;
}

int register_opcode_mask_ex_real(uint32_t ones_mask, uint32_t zeros_mask,
		int (*fn) (uint32_t), const char* fn_name, ...) {

	va_list va_args;
	va_start(va_args, fn_name);

	if ((zeros_mask & 0xffff0000) == 0) {
		WARN("%s registered zeros_mask requiring none of the top 4 bytes\n",
				fn_name);
		WARN("This is __probably__ an error, you should verify this\n");
	}

	if (NULL == ops) {
		// first registration
		return _register_opcode_mask(ones_mask, zeros_mask,
				fn, fn_name, va_args);
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

	return _register_opcode_mask(ones_mask, zeros_mask, fn, fn_name, va_args);
}

int register_opcode_mask_real(uint32_t ones_mask, uint32_t zeros_mask,
		int (*fn) (uint32_t), const char* fn_name) {
	return register_opcode_mask_ex_real(ones_mask, zeros_mask,
			fn, fn_name, 0, 0);
}

static int sim_execute(void) {
	uint32_t inst;
	struct op *o;

	static uint32_t last_pc;
	static uint32_t was_16 = false; // bool, but eases state tracking

	if (slowsim) {
		static struct timespec s = {0, NSECS_PER_SEC/10};
		nanosleep(&s, NULL);
	}

	// dectect branch to self loop for termination
	if ((cycle) && (last_pc == PC)) {
		INFO("Simulator determined PC 0x%08x is branch to self, terminating.\n", PC);
		print_full_state();
		if (returnr0) {
			DBG2("Return code is r0: %08x\n", reg[0]);
			exit(reg[0]);
		}
		exit(EXIT_SUCCESS);
	}

	// Now we're committed to starting the next cycle
	cycle++;

	// Instruction Fetch
	if (T_BIT) {
		if (was_16) {
			// do NOT correct PC if we've branched here
			if (last_pc == (PC - 4)) {
				state_write(&PC, PC - 2);
			}
		}

		DBG2("Reading thumb mode instruction\n");
		inst = read_halfword(PC);

		// If inst[15:11] are any of
		// 11101, 11110, or 11111 then this is
		// a 32-bit thumb inst

		switch (inst & 0xf800) {
			case 0xe800:
			case 0xf000:
			case 0xf800:
			{
				// 32-bit thumb inst
				DBG2("inst %04x is 32-bit\n", inst);

				state_write(&last_pc, PC);
				state_write(&PC, PC + 2);

				inst <<= 16;
				inst |= read_halfword(PC);
				state_write(&PC, PC + 2);
				state_write(&was_16, false);
				break;
			}
			default:
				// 16-bit thumb inst
				state_write(&last_pc, PC);
				state_write(&PC, PC + 2);

				// yeesh... read A5.1.2 p153 *carefully*
				// use of 0b1111 as a register specifier
				// reading PC must *always* return inst addr + 4
				state_write(&PC, PC + 2);
				state_write(&was_16, true);
		}
	} else {
#ifdef M_PROFILE
		CORE_ERR_not_implemented("This CPU conforms to the ARM-7M profile, which requires the T bit to always be set\n");
#endif
		DBG2("Reading arm mode instruction\n");
		inst = read_word(PC);
	}

	// Instruction Decode
	o = find_op(inst, true);
	if (NULL == o) {
		WARN("No handler registered for inst %x\n", inst);
		CORE_ERR_illegal_instr(inst);
	}

	// Execute
	if (in_ITblock(ITSTATE)) {
		// XXX: if we ever go cycle-accurate (or even bus-accurate) this
		// check should go earlier, if we aren't going to execture this
		// instruction we shouldn't even fetch it
		int ret;

		if (printcycles) {
			DBG2("PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP\n");
			printf("    P: %08d - 0x%08x : %04x\tITSTATE\n",
					cycle, PC, inst);
		}

		if (eval_cond(CPSR, (ITSTATE & 0xf0) >> 4)) {
			ret = o->fn(inst);
		} else {
			DBG2("itstate skipped instruction\n");
			//WARN("itstate skipped instruction\n");
			ret = SUCCESS;
		}

		if (ret == SUCCESS) {
			/*
			ITAdvance()
			if ITSTATE<2:0> == ‘000’ then
				ITSTATE.IT = ‘00000000’;
			else
				ITSTATE.IT<4:0> = LSL(ITSTATE.IT<4:0>, 1);
			*/
			/*
			if ((ITSTATE & 0x7) == 0) {
				write_itstate(0);
				DBG2("Left itstate\n");
			} else {
				uint8_t temp = ITSTATE & 0x0f; // yes 0f, not 1f
				uint8_t itstate_t = ITSTATE;
				itstate_t &= 0xe0;
				itstate_t |= (temp << 1);
				write_itstate(itstate_t);
				DBG2("New itstate: %02x\n", ITSTATE);
			}
			*/
			IT_advance(ITSTATE);
		}

		return ret;
	} else {
		if (printcycles)
			printf("    P: %08d - 0x%08x : %04x (%s)\n",
					cycle, PC, inst, o->name);
		return o->fn(inst);
	}
}

static void sim_reset(void) __attribute__ ((noreturn));
static void sim_reset(void) {
	uint8_t ret;

	INFO("Asserting reset pin\n");
	cycle = 0;
	reset();
	INFO("De-asserting reset pin\n");

	INFO("Entering main loop...\n");
	do {
		if (dumpatcycle == cycle) {
			print_full_state();
			shell();
		} else
		if ((dumpatpc & 0xfffffffe) == (reg[PC_REG] & 0xfffffffe)) {
			print_full_state();
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

static void power_on(void) {
	INFO("Powering on processor...\n");
	sim_reset();
}

static void load_opcodes(void) {
	INFO("Registering all opcode masks/handlers...\n");
	register_opcodes_add();
	register_opcodes_branch();
	register_opcodes_cb();
	register_opcodes_cmp();
	register_opcodes_div();
	register_opcodes_extend();
	register_opcodes_it();
	register_opcodes_ld();
	register_opcodes_ldm();
	register_opcodes_logical();
	register_opcodes_mov();
	register_opcodes_mul();
	register_opcodes_pop();
	register_opcodes_push();
	register_opcodes_shift();
	register_opcodes_str();
	register_opcodes_strm();
	register_opcodes_sub();

	INFO("Registered %d opcode mask%s\n", opcode_masks,
			(opcode_masks == 1) ? "":"s");
}

int main(int argc, char **argv) {
	// Command line args
	const char *flash_file = NULL;
	static int polluartport = POLL_UART_PORT;
	static int usetestflash = 0;

	// Command line parsing
	while (1) {
		static struct option long_options[] = {
			{"dumpatpc",      required_argument, 0,              'c'},
			{"dumpatcycle",   required_argument, 0,              'y'},
			{"dumpallcycles", no_argument,       &dumpallcycles, 'd'},
			{"printcycles",   no_argument,       &printcycles,   'p'},
			{"slowsim",       no_argument,       &slowsim,       's'},
			{"raiseonerror",  no_argument,       &raiseonerror,  'e'},
			{"returnr0",      no_argument,       &returnr0,      'r'},
			{"flash",         required_argument, 0,              'f'},
			{"showledwrites", no_argument,       &showledwrites, 'l'},
			{"polluartport",  required_argument, 0,              'u'},
			{"usetestflash",  no_argument,       &usetestflash,  1},
			{"help",          no_argument,       0,              '?'},
			{0,0,0,0}
		};
		int option_index = 0;
		int c;

		c = getopt_long(argc, argv, "c:y:dpserf:lu:?", long_options,
				&option_index);

		if (c == -1) break;

		switch (c) {
			case 0:
				// option set a flag
				break;

			case 'c':
				dumpatpc = strtol(optarg, NULL, 16);
				INFO("Simulator will pause at PC %x\n",
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

			case'f':
				flash_file = optarg;
				INFO("Simulator will use %s as flash\n",
						flash_file);
				break;

			case 'l':
				showledwrites = true;
				break;

			case 'u':
				polluartport = atoi(optarg);
				break;

			case '?':
			default:
				usage();
				break;
		}
	}

	// Read in flash
	if (usetestflash) {
		memcpy(rom, static_rom, STATIC_ROM_NUM_BYTES);
		INFO("Loaded internal test flash\n");
	} else {
		if (NULL == flash_file) {
			ERR(E_BAD_FLASH, "--flash or --usetestflash required, see --help\n");
		}
		int flashfd = open(flash_file, O_RDONLY);
		ssize_t ret;
		if (-1 == flashfd) {
			ERR(E_BAD_FLASH, "Could not open file %s for reading\n",
					flash_file);
		}
		assert (ROMSIZE < SSIZE_MAX);
		ret = read(flashfd, rom, ROMSIZE);
		if (ret < 0) {
			WARN("%s\n", strerror(errno));
			ERR(E_BAD_FLASH, "Failed to read flash file %s\n", flash_file);
		}
		if (ret < ROMSIZE) {
			WARN("Flash file (%zd) smaller than ROMSIZE (%d)\n", ret, ROMSIZE);
			WARN("Zero-filling remainder of ROM\n");
		}
		INFO("Succesfully loaded flash ROM: %s\n", flash_file);
	}

	load_opcodes();

	// Spawn uart thread
	pthread_t poll_uart_pthread;
	pthread_mutex_lock(&poll_uart_mutex);
	pthread_create(&poll_uart_pthread, NULL, poll_uart_thread, &polluartport);
	pthread_cond_wait(&poll_uart_cond, &poll_uart_mutex);
	pthread_mutex_unlock(&poll_uart_mutex);

	// Never returns
	power_on();

	assert(false);
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// UART THREAD(S)
////////////////////////////////////////////////////////////////////////////////

void *poll_uart_thread(void *arg_v) {
	int port = *((int *) arg_v);

	int sock;
	struct sockaddr_in server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock) {
		ERR(E_UNKNOWN, "Creating UART device: %s\n", strerror(errno));
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	int on;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if (-1 == bind(sock, (struct sockaddr *) &server, sizeof(server))) {
		ERR(E_UNKNOWN, "Creating UART device: %s\n", strerror(errno));
	}

	if (-1 == listen(sock, 1)) {
		ERR(E_UNKNOWN, "Creating UART device: %s\n", strerror(errno));
	}

	INFO("UART listening on port %d (use `nc -4 localhost %d` to communicate)\n",
			port, port);

	pthread_cond_signal(&poll_uart_cond);

	while (1) {
		int client = accept(sock, NULL, 0);
		if (-1 == client) {
			ERR(E_UNKNOWN, "UART device failure: %s\n", strerror(errno));
		}

		INFO("UART connected\n");

		static const char *welcome = "\
>>MSG<< You are now connected to the UART polling device\n\
>>MSG<< This device has a "VAL2STR(POLL_UART_BUFSIZE)" byte buffer\n\
>>MSG<< This device operates at "VAL2STR(POLL_UART_BAUD)" baud\n\
>>MSG<< To send a message, simply type and press the return key\n\
>>MSG<< All characters, up to and including the \\n will be sent\n";

		if (-1 == send(client, welcome, strlen(welcome), 0)) {
			ERR(E_UNKNOWN, "%d UART: %s\n", __LINE__, strerror(errno));
		}

		pthread_mutex_lock(&poll_uart_mutex);
		poll_uart_client = client;
		pthread_mutex_unlock(&poll_uart_mutex);

		static char c;
		while (1 == recv(poll_uart_client, &c, 1, 0)) {
			pthread_mutex_lock(&poll_uart_mutex);
			*poll_uart_tail = c;
			if (NULL == poll_uart_head)
				poll_uart_head = poll_uart_tail;
			poll_uart_tail++;
			if (poll_uart_tail == (poll_uart_buffer + POLL_UART_BUFSIZE))
				poll_uart_tail = poll_uart_buffer;
			pthread_mutex_unlock(&poll_uart_mutex);

			nanosleep(&poll_uart_baud_sleep, NULL);
		}

		WARN("Lost connection to UART client\n");

		pthread_mutex_lock(&poll_uart_mutex);
		poll_uart_client = -1;
		pthread_mutex_unlock(&poll_uart_mutex);
	}
}

uint8_t poll_uart_status_read() {
	uint8_t ret = 0;

	pthread_mutex_lock(&poll_uart_mutex);
	ret |= (poll_uart_head != NULL) << POLL_UART_RXBIT; // data avail?
	ret |= (poll_uart_client == -1) << POLL_UART_TXBIT; // tx busy?
	pthread_mutex_unlock(&poll_uart_mutex);

	// For lock contention
	nanosleep(&poll_uart_baud_sleep, NULL);

	return ret;
}

void poll_uart_status_write() {
	pthread_mutex_lock(&poll_uart_mutex);
	poll_uart_head = NULL;
	poll_uart_tail = poll_uart_buffer;
	pthread_mutex_unlock(&poll_uart_mutex);

	// For lock contention
	nanosleep(&poll_uart_baud_sleep, NULL);
}

uint8_t poll_uart_rxdata_read() {
	uint8_t ret;

#ifdef DEBUG1
	int idx;
#endif

	pthread_mutex_lock(&poll_uart_mutex);
	if (NULL == poll_uart_head) {
		// XXX warn? nah.. but grade?
		ret = poll_uart_buffer[3]; // eh... rand? 3, why not?
	} else {
#ifdef DEBUG1
		idx = poll_uart_head - poll_uart_buffer;
#endif
		ret = *poll_uart_head;
		poll_uart_head++;
		if (poll_uart_head == (poll_uart_buffer + POLL_UART_BUFSIZE))
			poll_uart_head = poll_uart_buffer;
		if (poll_uart_head == poll_uart_tail)
			poll_uart_head = NULL;
	}
	pthread_mutex_unlock(&poll_uart_mutex);

	DBG1("UART read byte: %c %x\tidx: %d\n", ret, ret, idx);

	return ret;
}

void poll_uart_txdata_write(uint8_t val) {
	DBG1("UART write byte: %c %x\n", val, val);

	pthread_mutex_lock(&poll_uart_mutex);
	if (-1 == poll_uart_client) {
		// XXX warn? no, grade
		// no connected client (TX is busy...) so drop
	}
	else if (-1 == send(poll_uart_client, &val, 1, 0)) {
		ERR(E_UNKNOWN, "%d UART: %s\n", __LINE__, strerror(errno));
	}
	pthread_mutex_unlock(&poll_uart_mutex);

	DBG2("UART byte sent\n");
}
