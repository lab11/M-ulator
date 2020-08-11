/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef GPIO_H
#define GPIO_H

#include "core/common.h"

#ifndef PP_STRING
#define PP_STRING "GIO"
#include "core/pretty_print.h"
#endif

#define GPIO_BASE      0x400E1000
#define GPIO_PORT_SIZE     0x0200

// Simplified
#define GPIO_PORT_COUNT 4

#define GPIO_END (GPIO_BASE + GPIO_PORT_COUNT * GPIO_PORT_SIZE)

#endif // GPIO_H
