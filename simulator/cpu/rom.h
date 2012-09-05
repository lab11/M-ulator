#ifndef ROM_H
#define ROM_H

#include "../common.h"

#ifndef PP_STRING
#define PP_STRING "ROM"
#include "../pretty_print.h"
#endif

void flash_ROM(void *image, uint32_t nbytes);

#endif // ROM_H
