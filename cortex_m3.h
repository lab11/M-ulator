#ifndef CORTEX_M3_H
#define CORTEX_M3_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>

#ifndef true
#define true	1
#endif
#ifndef false
#define false	0
#endif

/////////////////
// ERROR CODES //
/////////////////

// Internal Simulator Errors
#define E_NOT_IMPLEMENTED	1
#define E_BAD_FLASH		2
#define E_UNKNOWN		3

// Runtime Errors (CORR_ERR_... codes)
#define E_INVALID_ADDR		100
#define E_BAD_OPCODE		101
#define E_UNPREDICTABLE		102

/////////////////////////////
// SIMULATOR CONFIGURATION //
/////////////////////////////

#define M_PROFILE

#define ROMBOT 0x00000000
#define ROMTOP 0x00020000

#define RAMBOT 0x20000000
#define RAMTOP 0x20040000

#define REDLED 0x40001000
#define GRNLED 0x40001004
#define BLULED 0x40001008

#define POLL_UART_STATUS 0x40004100
#define POLL_UART_RSTBIT 0
#define POLL_UART_RXBIT  1
#define POLL_UART_TXBIT  2
#define POLL_UART_RXDATA 0x40004104
#define POLL_UART_TXDATA 0x40004108

#define ROMSIZE (ROMTOP - ROMBOT) // In bytes
#define RAMSIZE (RAMTOP - RAMBOT) // In bytes

/////////////////////////
// USEFUL CORE DEFINES //
/////////////////////////

#define SUCCESS	0
#define FAILURE	1

#define SP_REG	13
#define LR_REG	14
#define PC_REG	15

/* TTTA: What's up with the 'x's here? */

#define N_IDX 31
#define Z_IDX 30
#define C_IDX 29
#define V_IDX 28
#define Q_IDX 27

#define xPSR_N (1<<N_IDX)
#define xPSR_Z (1<<Z_IDX)
#define xPSR_C (1<<C_IDX)
#define xPSR_V (1<<V_IDX)
#define xPSR_Q (1<<Q_IDX)

///////////////////////////////
// CORE INTERATION FUNCTIONS //
///////////////////////////////

// These functions you must implement in core.c 
void		reset(void);

uint32_t	read_word(uint32_t addr);
void		write_word(uint32_t addr, uint32_t val);
uint16_t	read_halfword(uint32_t addr);
void		write_halfword(uint32_t addr, uint16_t val);
uint8_t		read_byte(uint32_t addr);
void		write_byte(uint32_t addr, uint8_t val);

// These functions you must implemented in misc.c
uint8_t		eval_cond(uint32_t cpsr, uint8_t cond);
void		write_itstate(uint8_t new_state);
uint8_t		in_ITblock(uint8_t itstate);
void		IT_advance(uint8_t itstate);
uint8_t		last_in_ITblock(uint8_t itstate);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Everything after this line has been implemented for you and are available for
// your use. I'd read over these carefully...

// Use this function to register your opcode handler
// if (
//       (ones_mask  == ones_mask  &  inst) &&
//       (zeros_mask == zeros_mask & ~inst)
//    )
// the instruction is considered a match
int		register_opcode_mask(uint32_t ones_mask,
		uint32_t zeros_mask, int (*fn) (uint32_t));
int		register_opcode_mask_ex(uint32_t ones_mask,
		uint32_t zeros_mask, int (*fn) (uint32_t), ...);
/* TTTA: Why do you need to specify two masks?
 *
 *  _Hint:_ When could we consider hardware to be trinary instead of binary?
 *
 */

// Use these functions to affect hardware state as necessary
uint32_t	CORE_reg_read(int r);
void		CORE_reg_write(int r, uint32_t val);
uint32_t	CORE_cpsr_read(void);
void		CORE_cpsr_write(uint32_t val);
uint32_t	CORE_ipsr_read(void);		// Don't need this yet
void		CORE_ipsr_write(uint32_t val);	// Don't need this yet
uint32_t	CORE_epsr_read(void);
void		CORE_epsr_write(uint32_t val);
uint32_t	CORE_rom_read(uint32_t addr);
/* TTTA: Why is there no CORE_rom_write? */
uint32_t	CORE_ram_read(uint32_t addr);
void		CORE_ram_write(uint32_t addr, uint32_t val);

uint32_t	CORE_red_led_read(void);
void		CORE_red_led_write(uint32_t val);
uint32_t	CORE_grn_led_read(void);
void		CORE_grn_led_write(uint32_t val);
uint32_t	CORE_blu_led_read(void);
void		CORE_blu_led_write(uint32_t val);

uint8_t		CORE_poll_uart_status_read();
void		CORE_poll_uart_status_write(uint8_t val);
uint8_t		CORE_poll_uart_rxdata_read();
void		CORE_poll_uart_txdata_write(uint8_t val);

// Thrown when an illegal read or write attempt is made
void		CORE_ERR_invalid_addr(uint8_t is_write, uint32_t addr) __attribute__ ((noreturn));

// Thrown when an illegal instruction is encountered
void		CORE_ERR_illegal_instr(uint32_t instr) __attribute__ ((noreturn));
void		CORE_ERR_illegal_line(const char *line) __attribute__ ((noreturn));

// Thrown when an undefined or unpredictable state is encountered
void		CORE_ERR_unpredictable(const char *opt_msg) __attribute__ ((noreturn));

// Useful for incremental work, indicates that everything up until this
// point has completed successfully (pass NULL or "a message\n")
void		CORE_ERR_not_implemented(const char *opt_msg) __attribute__ ((noreturn));


///////////////
// DEBUGGING //
///////////////

/*
 * USAGE: (Just like printf)
 * if (foo < bar)
 *     DBG2("Foo was less than bar, and baz is %d\n", baz);
 *
 * To enable, compile as either 'make debug=1' or 'make debug=2'
 * (debug2 ==> debug1)
 */

#ifdef DEBUG1
// "Level 1" debug, for statements that would print not more than
// once to a dozen times during execution
#define DBG1(...) printf("111 D: %s:%d\t%s:\t", __FILE__, __LINE__, __func__); \
	printf(__VA_ARGS__)
#else
#define DBG1(...)
#endif

#ifdef DEBUG2
// "Level 2" debug, for statements that print often, but may sometimes
// be usefule for debugging
#define DBG2(...) printf("222 D: %s:%d\t%s:\t", __FILE__, __LINE__, __func__); \
	printf(__VA_ARGS__)
#else
#define DBG2(...)
#endif


/////////////
// UTILITY //
/////////////

#define QUOTE(x)	#x
#define VAL2STR(x)	QUOTE(x)

//////////////////
// MACRO TRICKS //
//////////////////

#define register_opcode_mask(_o, _z, _f)\
	register_opcode_mask_real((_o), (_z), (_f), __FILE__":"VAL2STR(_f))
int		register_opcode_mask_real(uint32_t, uint32_t,
		int (*fn) (uint32_t), const char *);
#define register_opcode_mask_ex(_o, _z, _f, ...)\
	register_opcode_mask_ex_real((_o), (_z),(_f),\
			__FILE__":"VAL2STR(_f), __VA_ARGS__)
int		register_opcode_mask_ex_real(uint32_t, uint32_t,
		int (*fn) (uint32_t), const char*, ...);

#endif // CORTEX_M3_H
