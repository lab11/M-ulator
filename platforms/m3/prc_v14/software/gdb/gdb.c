/* 
 *
 * GDB Hooks for GDBoverMBUS support
 *
 * This works by co-opting the service routine call (svc)
 * as well as Interrupt #9 (handler_ext_int_9)
 *
 *
 *
 * Andrew Lukefahr
 * lukefahr@indiana.edu
 *
 * Much of this is borrowed from:
 *  - https://github.com/adamheinrich/os.h/blob/master/src/os_pendsv_handler.s
 *  - The Definitive Guide to ARM Cortex-M0 and Cortex-M0+ Processors Section 10.7.1
 */

#include <stdio.h>
#include <stdlib.h>

#include "PRCv14.h"

/* this struct is used by the assembley-level file 'gdb.s'
 * and must stay in this specific order
 */
struct svc_args 
{
    //yes this is a wired order, and yes it needs to be this way
    uint32_t isr_lr;
    uint32_t sp;  //pre-svc sp
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xPSR;
} ; //18 regs in total

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 *
 *          PROTOTYPES
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


/**
 * High-level (C) function called in response to a software-break
 * which is  implimented as 'svc 01' (0xdf01)
 * 
 * @regs:  a copy of the processor registers as the existed pre-software break
 * 
 */
void _gdb_break( struct svc_args * regs) 
    __attribute__ ((noinline,used));

/** 
 * High-level (C) function called in reponse to a "halt" interrupt 
 * (this is overriding handler_ext_int_9)
 *
 * @regs:  a copy of the processor registers as the existed pre-halt
 */
void _gdb_halt( struct svc_args * regs) 
    __attribute__ ((noinline,used));

/**
 * writes a 32-bit message over MBUS
 * 
 * This is duplicated to allow us to debug the 'regular' mbus_write32 function
 * could be inlined depending on compiler
 *
 * @addr the mbus address to write to
 * @data the 32-bit data to be written
 */
uint32_t _gdb_mbus_write_message32(uint32_t addr, uint32_t data) 
                                    __attribute__ ((used));







/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 *
 *          IMPLIMENTATIONS
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

void _gdb_halt( struct svc_args * regs)
{
    volatile uint32_t break_flag = 0x0;

    //disable watchdogs (CPU + MBUS)
    disable_timerwd();
    *MBCWD_RESET = 1;

    //broadcast the address of the flag
    _gdb_mbus_write_message32( 0xe0, (uint32_t) &break_flag);
    // and the starting address of the registers
    _gdb_mbus_write_message32( 0xe0, (uint32_t) regs);

    //loop until GDB signals
    while (break_flag != 0x1) {}
    
    // oh-right, this is actually IRQ-9
    //maybe we should clear the interrupt flag
    *NVIC_ICPR = (0x1 << 9); 

    config_timerwd( *TIMERWD_CNT);
    *MBCWD_RESET = 0;
}



void _gdb_break( struct svc_args * regs)
{
    //disable watchdogs (CPU + MBUS)
    disable_timerwd();
    *MBCWD_RESET = 1;


    //this wasn't a "real" instruction so backup one instruction
    regs->pc -= 2; 

    volatile uint32_t break_flag = 0x0;

    //broadcast the address of the flag
    _gdb_mbus_write_message32( 0xe0, (uint32_t) &break_flag);
    // and the starting address of the registers
    _gdb_mbus_write_message32( 0xe0, (uint32_t) regs);

    //loop until GDB signals
    while (break_flag != 0x1) {}

    config_timerwd( *TIMERWD_CNT);
    *MBCWD_RESET = 0;
 
}

//so we can soft-step into mbus_write_message32
uint32_t _gdb_mbus_write_message32(uint32_t addr, uint32_t data) {
    uint32_t mbus_addr = 0xA0003000 | (addr << 4);
    *((volatile uint32_t *) mbus_addr) = data;
    return 1;
}

