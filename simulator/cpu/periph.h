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

#ifndef PERIPH_H
#define PERIPH_H

#include "core/common.h"

void register_periph_printer(void (*fn)(void));
struct periph_time_travel {
	int (*rewind_start_fn) (void);
	int (*rewind_end_fn) (void);
	int (*rewind) (uint32_t *addr, uint32_t val);
	int (*rewind_p) (uint32_t **addr, uint32_t *val);
	int (*replay_start_fn) (void);
	int (*replay_end_fn) (void);
	int (*replay) (uint32_t *addr, uint32_t val);
	int (*replay_p) (uint32_t **addr, uint32_t *val);
};
#define PERIPH_TIME_TRAVEL_NONE {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
void register_periph_thread(pthread_t (*fn)(void*), const char *name,
		struct periph_time_travel tt,
		volatile bool *en, int fd,
		void* arg);

#endif // PERIPH_H
