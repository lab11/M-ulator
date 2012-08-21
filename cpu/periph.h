#ifndef PERIPH_H
#define PERIPH_H

#define STAGE PERIPH

#include "../common.h"

void register_periph_printer(void (*fn)(void));
void register_periph_thread(pthread_t (*fn)(void *), volatile bool *en);

#endif // PERIPH_H
