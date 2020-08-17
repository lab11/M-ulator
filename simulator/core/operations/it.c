/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "cpu/misc.h"

// arm-v7-m
void it(uint8_t itstate) {
	write_itstate(itstate);
}

