#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"

/////////////////////////////
// SIMULATOR CONFIGURATION //
/////////////////////////////

#include "cpu/memmap.h"

#define ROMSIZE (ROMTOP - ROMBOT) // In bytes
#define RAMSIZE (RAMTOP - RAMBOT) // In bytes

/////////////////
// ERROR CODES //
/////////////////

// Internal Simulator Errors
#define E_NOT_IMPLEMENTED	1
#define E_BAD_FLASH		2
#define E_UNKNOWN		3

#endif // SIMULATOR_H
