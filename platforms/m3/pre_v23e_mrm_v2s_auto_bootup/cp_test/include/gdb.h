/* 
 *
 * GDB Hooks for GDBoverMBUS support
 *
 * Including this file will cause the compilation process
 * to link in the gdb routines and enable software GDB support
 * using the MBUS interface.  
 *
 * If used without Link-Time-Optimization (LTO), there will be one wasted byte
 * from the '_use_gdb' variable.  If LTO is used, this variable will be optimized 
 * away. 
 *
 * GDB will co-opt IRQ_REG7 (handler_ext_int_reg7), as well as the 
 * Software Interrupt (handler_scv).  
 *
 *
 * Andrew Lukefahr
 * lukefahr@indiana.edu
 *
 */
#ifndef __GDB_H__
#define __GDB_H__

/**
 * Initializes the gdb software state.  
 * 
 * Currently this turns out interrupt #9
 */
void gdb_init();

/**
 * Uninitilize the gdb software state. 
 *
 * Currently this turns off interrupt #9
 */
void gdb_uninit();

/* this will set a "hidden" variable that will be used by the
 * makefile to determine that gdb should be linked
 * This is WAY into GNU GCC only... 
 */
int _use_gdb __attribute__ ((visibility ("hidden")));

#endif
