/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef WDT_H
#define WDT_H

#include "core/common.h"

#ifndef PP_STRING
#define PP_STRING "WDT"
#include "core/pretty_print.h"
#endif

#define WDT_BASE      0x400F0C00
#define WDT_SIZE           0x400

#endif // WDT_H
