//*******************************************************************
//Author: Pat Pannuto
//Description: C Startup routines
//*******************************************************************

#include <stdint.h>
#include <string.h>

extern uint32_t* _bss_start;
extern uint32_t* _bss_end;

void crt0(void) {
    uint32_t* current = _bss_start;
    while (current < _bss_end) {
        *current = 0;
        current++;
    }
}
