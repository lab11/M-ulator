#ifndef __GDB_H__
#define __GDB_H__

// Define a variable with a hidden symbol, which will allow
// the compilation to detect the symbol and recoginize 
// we want to pull in gdb support
// NOTE:  this varable is dead code, and will be eliminated
// if LTO (Link-Time-Optimization) is enabled 
int use_gdb __attribute__ ((visibility ("hidden")));

#endif
