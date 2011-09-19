#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>

#include "cortex_m3.h"
#include "operations/opcodes.h"

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

#define REG(_rw, _reg) printf("G~G E: "); \
	printf("%c %02d %x\n", _rw ? 'w':'r', _reg, reg[_reg]);
#define CPSR(_rw) printf("G~G C: "); \
	printf("%c %d %d %d %d %08x\n", _rw ? 'w':'r',\
			!!(*cpsr & 0x80000000),\
			!!(*cpsr & 0x40000000),\
			!!(*cpsr & 0x20000000),\
			!!(*cpsr & 0x10000000),\
			(*cpsr));
#define ROM(_rw, _addr) printf("G~G O: "); \
	printf("%c %08x %x\n", _rw ? 'w':'r',\
			_addr, rom[ADDR_TO_IDX(_addr, ROMBOT)]);
#define RAM(_rw, _addr) printf("G~G A: "); \
	printf("%c %08x %x\n", _rw ? 'w':'r',\
			_addr, ram[ADDR_TO_IDX(_addr, RAMBOT)]);
#define PERIPH(_rw, _type, _idx) printf("G~G P: "); \
	printf("%c %s %d\n", _rw ? 'w':'r', _type, _idx);
#else
#define REG(...)
#define CPSR(...)
#define ROM(...)
#define RAM(...)
#define PERIPH(...)
#endif

static void usage_fail(int retcode) {
	printf("USAGE: ./simulator [OPTS]\n\n");
	printf("\
\t--dumpatpc PC_IN_HEX\n\
\t\tExecute until PC reaches the given value. The simulator will\n\
\t\tpause at the given PC and prompt for a new pause PC\n\
\t--dumpatcycle N\n\
\t\tExecute until cycle N and print the current machine state.\n\
\t\tYou will be prompted for a new N at the pause\n\
\t--dumpallcycles\n\
\t\tPrints the machine state every cycle -- This is a lot of text\n\
\t--printcycles\n\
\t\tPrints cycle count and inst to execute every cycle (before exec)\n\
\t\tConceptually, this executes between the intstruction fetch and\n\
\t\tdecode stage, so the PC is already advanced\n\
\t--slowsim\n\
\t\tSlows simulation down, running an instruction every .1s\n\
\t--raiseonerror\n\
\t\tRaises a SIGTRAP for gdb on errors before dying\n\
\t\t(Useful for debugging with gdb\n\
\t--flash FILE\n\
\t\tUse FILE as flash.mem\n\
\t--showledtoggles\n\
\t\tPrints a message every time the LEDs are written to\n\
\t--polluartport "VAL2STR(POLL_UART_PORT)"\n\
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
static const struct timespec poll_uart_baud_sleep = {0, NSECS_PER_SEC/POLL_UART_BAUD};

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
static int showledtoggles = 0;
static int dumpatpc = -1;
static int dumpatcycle = -1;
static int dumpallcycles = 0;

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

#define SP	reg[SP_REG]
#define LR	reg[LR_REG]
#define PC	reg[PC_REG]

// XXX: Reset handler
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
#define	T_BIT		((*cpsr) & 0x0020)
static uint32_t *cpsr = &apsr;

#define ITSTATE			( (((*cpsr) & 0xfc00) >> 8) | (((*cpsr) & 0x06000000) >> 25) )
#define	IN_IT_BLOCK()		((ITSTATE & 0xf) != 0)
#define LAST_IN_IT_BLOCK()	((ITSTATE & 0xf) == 0x8)


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
			!!(*cpsr & xPSR_N),
			!!(*cpsr & xPSR_Z),
			!!(*cpsr & xPSR_C),
			!!(*cpsr & xPSR_V)
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

static void shell(void) {
	static char buf[100];

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
			// fall thru

		case 'h':
		default:
			printf(">> The following commands are recognized:\n");
			printf("   <none>		Advance 1 cycle\n");
			printf("   pc HEX_ADDR		Stop at pc\n");
			printf("   cycle INT		Stop at cycle\n");
			printf("   continue		Continue\n");
			printf("   terminate		Terminate Simulation\n");
			return shell();
	}
}

////////////////////////////////////////////////////////////////////////////////
// CORE
////////////////////////////////////////////////////////////////////////////////

/* These are the functions called into by the student simulator project */

uint32_t CORE_reg_read(int r) {
	assert(r >= 0 && r < 16 && "CORE_reg_read");
	REG(READ, r);
	if (r == PC_REG)
		return reg[r] & 0xfffffffe;
	else
		return reg[r];
}

void CORE_reg_write(int r, uint32_t val) {
	assert(r >= 0 && r < 16 && "CORE_reg_write");
	if (r == PC_REG)
		reg[r] = val & 0xfffffffe;
	else
		reg[r] = val;
	REG(WRITE, r);
}

uint32_t CORE_cpsr_read(void) {
	CPSR(READ);
	return *cpsr;
}

void CORE_cpsr_write(uint32_t val) {
	if (IN_IT_BLOCK()) {
		DBG1("WARN update of cpsr in IT block\n");
	}
	*cpsr = val;
	CPSR(WRITE);
}

uint32_t CORE_rom_read(uint32_t addr) {
	DBG2("ROM Read request addr %x (idx: %d)\n", addr, ADDR_TO_IDX(addr, ROMBOT));
	assert((addr >= ROMBOT) && (addr < ROMTOP) && "CORE_rom_read");
	ROM(READ, addr);
	return rom[ADDR_TO_IDX(addr, ROMBOT)];
}

uint32_t CORE_ram_read(uint32_t addr) {
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_read");
	RAM(READ, addr);
	return ram[ADDR_TO_IDX(addr, RAMBOT)];
}

void CORE_ram_write(uint32_t addr, uint32_t val) {
	DBG2("RAM Write request addr %x (idx: %d)\n", addr, ADDR_TO_IDX(addr, RAMBOT));
	assert((addr >= RAMBOT) && (addr < RAMTOP) && "CORE_ram_write");
	ram[ADDR_TO_IDX(addr, RAMBOT)] = val;
	RAM(WRITE, addr);
}

uint32_t CORE_red_led_read(void) {
	PERIPH(READ, LED, RED);
	return leds[RED];
}

void CORE_red_led_write(uint32_t val) {
	PERIPH(WRITE, LED, RED);
	leds[RED] = val;
	if (showledtoggles)
		print_leds_line();
}

uint32_t CORE_grn_led_read(void) {
	PERIPH(READ, LED, GRN);
	return leds[GRN];
}

void CORE_grn_led_write(uint32_t val) {
	PERIPH(WRITE, LED, GRN);
	leds[GRN] = val;
	if (showledtoggles)
		print_leds_line();
}

uint32_t CORE_blu_led_read(void) {
	PERIPH(READ, LED, BLU);
	return leds[BLU];
}

void CORE_blu_led_write(uint32_t val) {
	PERIPH(WRITE, LED, BLU);
	leds[BLU] = val;
	if (showledtoggles)
		print_leds_line();
}

uint8_t CORE_poll_uart_status_read() {
	return poll_uart_status_read();
}

void CORE_poll_uart_status_write(uint8_t val) {
	return poll_uart_status_write(val);
}

uint8_t CORE_poll_uart_rxdata_read() {
	return poll_uart_rxdata_read();
}

void CORE_poll_uart_txdata_write(uint8_t val) {
	return poll_uart_txdata_write(val);
}

void CORE_ERR_invalid_addr(uint8_t is_write, uint32_t addr) {
	WARN("CORE_ERR_invalid_addr %s address: %x\n",
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
	int (*fn) (uint32_t);
};

static struct op *ops = NULL;

static struct op* find_op(uint32_t inst, uint8_t reorder) {
	struct op *o = ops;
	//printf("inst:\t%08x %08x\n", inst, inst);
	while (NULL != o) {
		//printf("o:\t%08x %08x\n", o->ones_mask, o->zeros_mask);
		if (
				(o->ones_mask  == (o->ones_mask  & inst)) &&
				(o->zeros_mask == (o->zeros_mask & ~inst))
		   ) {
			break;
		}
		o = o->next;
	}

	if (o && reorder) {
		// ...
	}

	return o;
}

int register_opcode_mask(uint32_t ones_mask, uint32_t zeros_mask,
		int (*fn) (uint32_t)) {
	if (NULL == ops) {
		// first registration
		ops = malloc(sizeof(struct op));
		ops->prev = NULL;
		ops->next = NULL;
		ops->ones_mask = ones_mask;
		ops->zeros_mask = zeros_mask;
		ops->fn = fn;
		return ++opcode_masks;
	}

	struct op* o = ops;

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
				"\tExisting  registration: 1's %x, 0's %x\n"\
				"\tAttempted registration: 1's %x, 0's %x\n"\
				, o->ones_mask, ones_mask\
				, o->zeros_mask, zeros_mask);
	}

	// Add new element to list head b/c that's easy and it doesn't matter
	o = malloc(sizeof(struct op));
	o->prev = NULL;
	o->next = ops;
	o->ones_mask = ones_mask;
	o->zeros_mask = zeros_mask;
	o->fn = fn;
	ops->prev = o;
	ops = o;

	return ++opcode_masks;
}

static int sim_execute(void) {
	uint32_t inst;
	struct op *o;

	static uint32_t last_pc;

	if (slowsim) {
		static struct timespec s = {0, NSECS_PER_SEC/10};
		nanosleep(&s, NULL);
	}

	// dectect branch to self loop for termination
	if ((cycle) && (last_pc == PC)) {
		INFO("Simulator determined PC 0x%08x is branch to self\n", PC);
		print_full_state();
		INFO("Simulation completed successfully.\n");
		exit(EXIT_SUCCESS);
	}

	if (T_BIT) {
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

				last_pc = PC;
				PC += 2;

				inst <<= 16;
				inst |= read_halfword(PC);
				PC += 2;
				break;
			}
			default:
				// 16-bit thumb inst
				last_pc = PC;
				PC += 2;
		}
	} else {
		CORE_ERR_not_implemented("This CPU only supports thumb mode\n");
		DBG2("Reading arm mode instruction\n");
		inst = read_word(PC);
	}

	o = find_op(inst, true);
	if (NULL == o) {
		WARN("No handler registered for inst %x\n", inst);
		CORE_ERR_illegal_instr(inst);
	} else {
		cycle++;
		if (IN_IT_BLOCK()) {
			int ret;

			if (printcycles) {
				DBG2("PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP\n");
				printf("    P: %08d - 0x%08x : %04x\tITSTATE\n",
						cycle, PC, inst);
			}

			if (eval_cond(*cpsr, (ITSTATE & 0xf0) >> 4)) {
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
			}

			return ret;
		} else {
			if (printcycles)
				printf("    P: %08d - 0x%08x : %04x\n",
						cycle, PC, inst);
			return o->fn(inst);
		}
	}
}

static void sim_reset(void) {
	uint8_t ret;

	INFO("Asserting reset pin\n");
	cycle = 0;
	reset();
	INFO("De-asserting reset pin\n");

	INFO("Entering main loop...\n");
	while (SUCCESS == (ret = sim_execute()) ) {
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
	}

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
	register_opcodes_extend();
	register_opcodes_it();
	register_opcodes_ldm();
	register_opcodes_ld();
	register_opcodes_str();
	register_opcodes_logical();
	register_opcodes_mov();
	register_opcodes_mul();
	register_opcodes_pop();
	register_opcodes_push();
	register_opcodes_shift();
	register_opcodes_sub();

	INFO("Registered %d opcode mask%s\n", opcode_masks,
			(opcode_masks == 1) ? "":"s");
}

int main(int argc, char **argv) {
	// Command line args
	const char *flash_file = "flash.mem";
	static int polluartport = POLL_UART_PORT;
	static int usetestflash = 0;
	static int usetestcore = 0;

	// Command line parsing
	while (1) {
		static struct option long_options[] = {
			{"dumpatpc",      required_argument, 0,              'c'},
			{"dumpatcycle",   required_argument, 0,              'd'},
			{"dumpallcycles", no_argument,       &dumpallcycles, 1},
			{"printcycles",   no_argument,       &printcycles,   1},
			{"raiseonerror",  no_argument,       &raiseonerror,  1},
			{"slowsim",       no_argument,       &slowsim,       1},
			{"flash",         required_argument, 0,              'f'},
			{"showledtoggles",no_argument,       &showledtoggles,'1'},
			{"polluartport",  required_argument, 0,              'p'},
			{"usetestflash",  no_argument,       &usetestflash,  1},
			{"usetestcore",   no_argument,       &usetestcore,   1},
			{"help",          no_argument,       0,              'h'},
		};
		int option_index = 0;
		int c;

		c = getopt_long(argc, argv, "d:af:yz", long_options,
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

			case 'd':
				dumpatcycle = atoi(optarg);
				INFO("Simulator will pause at cycle %d\n",
						dumpatcycle);
				break;

			case'f':
				flash_file = optarg;
				INFO("Simulator will use %s as flash\n",
						flash_file);
				break;

			case 'p':
				polluartport = atoi(optarg);
				break;

			case 'h':
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
