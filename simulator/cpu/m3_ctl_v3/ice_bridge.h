/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef ICE_BRIDGE_H
#define ICE_BRIDGE_H

#ifndef PP_STRING
#define PP_STRING "ICE"
#include "core/pretty_print.h"
#endif

#include "core/common.h"

struct ice_instance;

void ice_gpio_out(struct ice_instance* ice, uint8_t idx, bool val);
void ice_gpio_dir(struct ice_instance* ice, uint8_t idx, bool val);
void ice_gpio_int(struct ice_instance* ice, uint8_t idx, bool val);
unsigned ice_i2c_send(struct ice_instance* ice, uint8_t addr, char *data, int len);

// This function *MUST* be called from a peripheral's constructor
struct ice_instance* create_ice_instance(const char *host, int baud);

#endif // ICE_BRIDGE_H
