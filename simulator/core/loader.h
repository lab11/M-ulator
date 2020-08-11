/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef LOADER_H
#define LOADER_H

#include "common.h"

#ifndef PP_STRING
#define PP_STRING "LDR"
#include "pretty_print.h"
#endif

void flash_image(const uint8_t *image, const uint32_t num_bytes);
void load_file(const char* filename);

#endif //LOADER_H
