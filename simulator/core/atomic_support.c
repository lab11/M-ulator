/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#include "atomic_support.h"

#ifdef NO_ATOMIC
    pthread_mutex_t no_atomic_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
