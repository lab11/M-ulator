/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
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
