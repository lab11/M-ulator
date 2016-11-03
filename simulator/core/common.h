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

#ifndef COMMON_H
#define COMMON_H

// http://stackoverflow.com/questions/777261/avoiding-unused-variables-warnings-when-using-assert-in-a-release-build
#ifdef NDEBUG
#define assert(x) do { (void)sizeof(x); } while (0)
#else
#include <assert.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#ifndef __APPLE__
#include <sys/prctl.h>
#endif

#include <pthread.h>
#include <semaphore.h>

#include "atomic_support.h"
#include "conf.h"

/////////////////////
// GENERAL DEFINES //
/////////////////////

#define SUCCESS 0
#define FAILURE 1

// Clarity macro (since to export is simply to not be static)
#define EXPORT

/////////////////////////
// USEFUL CORE DEFINES //
/////////////////////////

#define SP_REG	13
#define LR_REG	14
#define PC_REG	15

/* TTTA: What's up with the 'x's here? */

#define N_IDX 31
#define Z_IDX 30
#define C_IDX 29
#define V_IDX 28
#define Q_IDX 27

#define xPSR_N (1U<<N_IDX)
#define xPSR_Z (1U<<Z_IDX)
#define xPSR_C (1U<<C_IDX)
#define xPSR_V (1U<<V_IDX)
#define xPSR_Q (1U<<Q_IDX)

///////////////////////////////
// ERROR FUNCTIONS //
///////////////////////////////

// Runtime Errors (CORE_ERR_... return codes)
#define E_INVALID_ADDR		100
#define E_BAD_OPCODE		101
#define E_UNPREDICTABLE		102
#define E_RUNTIME		103

// Generic warning primative, will not cause exit
void	CORE_WARN(const char *);

// Thrown on invalid access to an address
void	CORE_ERR_read_only(uint32_t addr) __attribute__ ((noreturn));
void	CORE_ERR_write_only(uint32_t addr) __attribute__ ((noreturn));

// Thrown when an illegal read or write attempt is made
void	CORE_ERR_invalid_addr(uint8_t is_write, uint32_t addr) __attribute__ ((noreturn));

// Thrown when an illegal instruction is encountered
void	CORE_ERR_illegal_instr(uint32_t instr) __attribute__ ((noreturn));

// Thrown when an undefined or unpredictable state is encountered
// *** This error may be optimized out (ignored) ***
void	CORE_ERR_unpredictable(const char *opt_msg) __attribute__ ((noreturn));

// General error for an unrecoverable case
void	CORE_ERR_runtime(const char *opt_msg) __attribute__ ((noreturn));

// Useful for incremental work, indicates that everything up until this
// point has completed successfully
void	CORE_ERR_not_implemented(const char *opt_msg) __attribute__ ((noreturn));

/////////////
// UTILITY //
/////////////

#ifndef MAX
#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#endif

#define QUOTE(x)	#x
#define VAL2STR(x)	QUOTE(x)

#define NSECS_PER_SEC 1000000000

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#define export_inline  inline __attribute__ ((always_inline))

///////////////
// DEBUGGING //
///////////////

/*
 * USAGE: (Just like printf)
 * if (foo < bar)
 *     DBG2("Foo was less than bar, and baz is %d\n", baz);
 *
 * To enable, compile as either 'make debug=1' or 'make debug=2'
 * (debug2 ==> debug1)
 */


#ifdef __APPLE__
#define _DBG_T_NAME_HELPER(_t_buf) pthread_getname_np(pthread_self(), _t_buf, 16)
#else
#define _DBG_T_NAME_HELPER(_t_buf) prctl(PR_GET_NAME, _t_buf, 0, 0, 0)
#endif

#ifdef DEBUG1
// "Level 1" debug, for statements that would print not more than
// once to a dozen times during execution
#define DBG1(...)\
	do {\
		flockfile(stdout); flockfile(stderr);\
		char _t_name[16];\
		if (0 == _DBG_T_NAME_HELPER(_t_name))\
			printf("111 %.2s D: %s:%d\t%s:\t", _t_name, __FILE__, __LINE__, __func__); \
		else\
			printf("111 -- D: %s:%d\t%s:\t", __FILE__, __LINE__, __func__); \
		printf(__VA_ARGS__);\
		funlockfile(stderr); funlockfile(stdout);\
	} while (0)
#else
#define DBG1(...)
#endif

#ifdef DEBUG2
// "Level 2" debug, for statements that print often, but may sometimes
// be usefule for debugging
#define DBG2(...)\
	do {\
		flockfile(stdout); flockfile(stderr);\
		char _t_name[16];\
		if (0 == _DBG_T_NAME_HELPER(_t_name))\
			printf("222 %.2s D: %s:%d\t%s:\t", _t_name, __FILE__, __LINE__, __func__); \
		else\
			printf("222 -- D: %s:%d\t%s:\t", __FILE__, __LINE__, __func__); \
		printf(__VA_ARGS__);\
		funlockfile(stderr); funlockfile(stdout);\
	} while (0)
#else
#define DBG2(...)
#endif

/////////////
// PROFILE //
/////////////

#if defined (M_PROFILE)
#if defined (R_PROFILE)
#error "Must choose one of M or R profiles, not both"
#endif // R
#if defined (A_PROFILE)
#error "Must choose one of M or A profiles, not both"
#endif // A
#endif // M
#if defined (R_PROFILE) && defined (A_PROFILE)
#error "Must choose one of R or A profiles, not both"
#endif // R && A

//////////////////
// MACRO TRICKS //
//////////////////

#ifdef __has_feature
# if !__has_feature(cxx_static_assert)
#  define static_assert _Static_assert
# endif
#endif

#define CORE_WARN(_m)\
	CORE_WARN_real(__FILE__, __LINE__, (_m))
void	CORE_WARN_real(const char*, int, const char *);

#define CORE_ERR_read_only(_a)\
	CORE_ERR_read_only_real(__FILE__, __LINE__, (_a))
void	CORE_ERR_read_only_real(const char*, int, uint32_t)
		__attribute__ ((noreturn));

#define CORE_ERR_write_only(_a)\
	CORE_ERR_write_only_real(__FILE__, __LINE__, (_a))
void	CORE_ERR_write_only_real(const char*, int, uint32_t)
		__attribute__ ((noreturn));

#define CORE_ERR_invalid_addr(_w, _a)\
	CORE_ERR_invalid_addr_real(__FILE__, __LINE__, (_w), (_a))
void	CORE_ERR_invalid_addr_real(const char*, int, uint8_t, uint32_t)
		__attribute__ ((noreturn));

#define CORE_ERR_illegal_instr(_i)\
	CORE_ERR_illegal_instr_real(__FILE__, __LINE__, (_i))
void	CORE_ERR_illegal_instr_real(const char*, int, uint32_t)
		__attribute__ ((noreturn));

#define CORE_ERR_unpredictable(_o)\
	CORE_ERR_unpredictable_real(__FILE__, __LINE__, (_o))
void	CORE_ERR_unpredictable_real(const char*, int, const char *)
		__attribute__ ((noreturn));

#define CORE_ERR_runtime(_o)\
	CORE_ERR_runtime_real(__FILE__, __LINE__, (_o))
void	CORE_ERR_runtime_real(const char*, int, const char *)
		__attribute__ ((noreturn));

#define CORE_ERR_not_implemented(_o)\
	CORE_ERR_not_implemented_real(__FILE__, __LINE__, (_o))
void	CORE_ERR_not_implemented_real(const char*, int, const char *)
		__attribute__ ((noreturn));

#ifdef DEBUG1
#define pthread_mutex_lock(_m)\
	do {\
		int _ret = pthread_mutex_lock((_m));\
		if (_ret) {\
			errno = _ret;\
			perror("Locking "VAL2STR(_m));\
			raise(SIGTRAP);\
		}\
	} while (0)
#define pthread_mutex_unlock(_m)\
	do {\
		int _ret = pthread_mutex_unlock((_m));\
		if (_ret) {\
			errno = _ret;\
			perror("Unlocking "VAL2STR(_m));\
			raise(SIGTRAP);\
		}\
	} while (0)
#endif

#endif // COMMON_H
