/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef ATOMIC_SUPPORT_H
#define ATOMIC_SUPPORT_H

#if !defined(__STDC_VERSION__)
# error Compiler does not support C
#endif
#if (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_ATOMICS__)
# include <stdatomic.h>
# define HAVE_STDATOMIC
#elif defined(__has_include) && defined(__GNUC__) && __has_include(<pthread.h>)
# define NO_ATOMIC
# include <pthread.h>
   extern pthread_mutex_t no_atomic_mutex;
# define atomic_bool _Bool
# define atomic_flag _Bool
# define atomic_store(_p, _i) atomic_store_compat(_p, _i)
   static inline void atomic_store_compat(atomic_bool *p, atomic_bool val) {
	   pthread_mutex_lock(&no_atomic_mutex);
	   *p = val;
	   pthread_mutex_unlock(&no_atomic_mutex);
   }
# define atomic_load(_p) atomic_load_compat(_p)
   static inline _Bool atomic_load_compat(atomic_bool *p) {
	   _Bool temp;
	   pthread_mutex_lock(&no_atomic_mutex);
	   temp = *p;
	   pthread_mutex_unlock(&no_atomic_mutex);
	   return temp;
   }
   // but these gcc extensions seem to work
# define atomic_flag_test_and_set(_p) __sync_lock_test_and_set(_p, 1)
# define atomic_flag_clear(_p)        __sync_lock_release(_p)
# define ATOMIC_VAR_INIT(val) (val)
# define ATOMIC_FLAG_INIT ((_Bool)!1)
#else
# error No C11 atomic or GCC compatible pthread support?
#endif
#if (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_THREADS__)
# include <threads.h>
#elif defined(__GNUC__)
# define thread_local __thread
#else
# error Unknown compiler does not support C11 threads?
#endif

#endif // ATOMIC_SUPPORT_H
