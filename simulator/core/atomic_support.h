/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2016  Pat Pannuto <pat.pannuto@gmail.com>
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

#ifndef ATOMIC_SUPPORT_H
#define ATOMIC_SUPPORT_H

#if defined(__has_include)
# if __has_include(<stdatomic.h>)
#  include <stdatomic.h>
#  define HAVE_STDATOMIC
# elif 0 // as of 3.4 clang BUG's at -O1+ with atomics
#  define CLANG_ATOMIC
   typedef enum memory_order {
   	  memory_order_relaxed, memory_order_consume, memory_order_acquire,
   	    memory_order_release, memory_order_acq_rel, memory_order_seq_cst
   } memory_order;
#  define atomic_store(_p, _i)         __c11_atomic_store(_p, _i, memory_order_release)
#  define atomic_load(_p)              __c11_atomic_load(_p, memory_order_acquire)
#  define atomic_flag_test_and_set(_p) __sync_lock_test_and_set(_p, 1)
#  define atomic_flag_clear(_p)        __sync_lock_release(_p)
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
