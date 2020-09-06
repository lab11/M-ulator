/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
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
