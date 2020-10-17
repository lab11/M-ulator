/* Mulator - An extensible {e,si}mulator
 * Copyright 2011-2020 Pat Pannuto <pat.pannuto@gmail.com>
 *
 * Licensed under either of the Apache License, Version 2.0
 * or the MIT license, at your option.
 */

#ifndef ATOMIC_SUPPORT_H
#define ATOMIC_SUPPORT_H

#if defined(__has_include)
# if __has_include(<stdatomic.h>)
#  include <stdatomic.h>
#  define HAVE_STDATOMIC
# else
#  define NO_ATOMIC
   pthread_mutex_t no_atomic_mutex = PTHREAD_MUTEX_INITIALIZER;
#  define atomic_store(_p, _i) atomic_store_compat(_p, _i)
   static void atomic_store_compat(bool *p, bool val) {
	   pthread_mutex_lock(&no_atomic_mutex);
	   *p = val;
	   pthread_mutex_unlock(&no_atomic_mutex);
   }
#  define atomic_load(_p) atomic_load_compat(_p)
   static bool atomic_load_compat(bool *p) {
	   bool temp;
	   pthread_mutex_lock(&no_atomic_mutex);
	   temp = *p;
	   pthread_mutex_unlock(&no_atomic_mutex);
	   return temp;
   }
   // but these gcc extensions seem to work
#  define atomic_flag_test_and_set(_p) __sync_lock_test_and_set(_p, 1)
#  define atomic_flag_clear(_p)        __sync_lock_release(_p)
# endif
# if __has_include(<threads.h>)
#  include <threads.h>
# else
#  define thread_local __thread
# endif
#elif defined __GNUC__
# if __GNUC__ != 4
#  error GCC 4.x required
# endif
# if __GNUC_MINOR__ < 9
#  define thread_local __thread
# else
#  include <threads.h>
# endif
# if __GNUC_MINOR__ < 7
#  error GCC 4.7+ required for __atomic support (though you could work around this if motivated)
# endif
# define GCC_ATOMIC
# define atomic_store(_p, _i)         __atomic_store_n(_p, _i, __ATOMIC_RELEASE)
# define atomic_load(_p)              __atomic_load_n(_p, __ATOMIC_ACQUIRE)
# define atomic_flag_test_and_set(_p) __atomic_test_and_set(_p, __ATOMIC_SEQ_CST)
# define atomic_flag_clear(_p)        __atomic_clear(_p, __ATOMIC_SEQ_CST)
#else
# error Unknown compiler. No C11 atomic / thread support?
#endif

#endif // ATOMIC_SUPPORT_H
