/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef GDB_H
#define GDB_H

#include "common.h"

#ifndef PP_STRING
#define PP_STRING "GDB"
#include "pretty_print.h"
#endif

void gdb_init(int);
char* gdb_get_message(long*);
void gdb_send_message(const char*);

void wait_for_gdb(void);
void stop_and_wait_for_gdb(void);

#endif //GDB_H
