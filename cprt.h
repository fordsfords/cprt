/* cprt.h - for portability. */
/*
# This code and its documentation is Copyright 2002-2021 Steven Ford
# and licensed "public domain" style under Creative Commons "CC0":
#   http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/cprt
*/
#ifndef CPRT_H
#define CPRT_H

#include <inttypes.h>

#if defined(_WIN32)
  #include <windows.h>
  #include <winsock2.h>

#else  /* Unix */
  #include <unistd.h>
  #include <stdlib.h>
  #include <pthread.h>
  #if defined(__APPLE__)
    #include <dispatch/dispatch.h>
  #else  /* Non-Apple Unixes. */
    #include <semaphore.h>
  #endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Macro to approximate the basename() function. */
#define CPRT_BASENAME(_p) ((strrchr(_p, '/') == NULL) ? (_p) : (strrchr(_p, '/')+1))

#if defined(_WIN32)
  #define CPRT_PERRNO(cprt_perrno_in_str) do { \
    char cprt_perrno_errno = errno; \
    char cprt_perrno_errstr[1024]; \
    strerror_s(cprt_perrno_errno, cprt_perrno_errstr, sizeof(cprt_perrno_errstr)); \
    fprintf(stderr, "ERROR (%s:%d): %s: errno=%u ('%s')\n", \
      CPRT_BASENAME(__FILE__), __LINE__, cprt_perrno_in_str, cprt_perrno_errno, cprt_perrno_errstr_); \
    fflush(stderr); \
    exit(1); \
  } while (0)

#else  /* Unix */
  #define CPRT_PERRNO(cprt_perrno_in_str) do { \
    char cprt_perrno_errno = errno; \
    char cprt_perrno_errstr[1024]; \
    strerror_r(cprt_perrno_errno, cprt_perrno_errstr, sizeof(cprt_perrno_errstr)); \
    fprintf(stderr, "ERROR (%s:%d): %s: errno=%u ('%s')\n", \
      CPRT_BASENAME(__FILE__), __LINE__, cprt_perrno_in_str, cprt_perrno_errno, cprt_perrno_errstr); \
    fflush(stderr); \
    exit(1); \
  } while (0)
#endif

/* Use when non-zero means error. */
#define CPRT_EOK0(cprt_eok0_expr) do { \
  errno = 0; \
  if ((cprt_eok0_expr) != 0) { \
    int cprt_eok0_errno = errno; \
    char cprt_eok0_errstr[1024]; \
    CPRT_SNPRINTF(cprt_eok0_errstr, sizeof(cprt_eok0_errstr), "'%s' is not 0", #cprt_eok0_expr); \
    errno = cprt_eok0_errno; \
    CPRT_PERRNO(cprt_eok0_errstr); \
  } \
} while (0)

/* Use when NULL means error. */
#define CPRT_ENULL(cprt_enull_expr) do { \
  errno = 0; \
  if ((cprt_enull_expr) == NULL) { \
    int cprt_enull_errno = errno; \
    char cprt_enull_errstr[1024]; \
    CPRT_SNPRINTF(cprt_enull_errstr, sizeof(cprt_enull_errstr), "'%s' is NULL", #cprt_enull_expr); \
    errno = cprt_enull_errno; \
    CPRT_PERRNO(cprt_enull_errstr); \
  } \
} while (0)

#define CPRT_ASSERT(cprt_assert_cond) do { \
  if (! (cprt_assert_cond)) { \
    fprintf(stderr, "ERROR (%s:%d): ERROR: '%s' not true\n", \
      CPRT_BASENAME(__FILE__), __LINE__, #cprt_assert_cond); \
    fflush(stderr); \
    exit(1); \
  } \
} while (0)

#define CPRT_ABORT(cprt_abort_in_str) do { \
  fprintf(stderr, "ERROR (%s:%d): ABORT: %s\n", \
    CPRT_BASENAME(__FILE__), __LINE__, cprt_abort_in_str); \
  fflush(stderr); \
  abort(); \
} while (0)


/* Macro to make it easier to use sscanf().  See:
 *   https://stackoverflow.com/questions/25410690
 */
#define CPRT_STRDEF2(cprt_strdef2_in_str) #cprt_strdef2_in_str
#define CPRT_STRDEF(cprt_strdef_in_str) CPRT_CPRT_STRDEF2(cprt_strdef_in_str)

/* See http://blog.geeky-boy.com/2014/06/clangllvm-optimize-o3-understands-free.html
 *  for why the CPRT_VOL32 macro is needed.
 */
#define CPRT_VOL32(cprt_vol32_ptr) (*(volatile uint32_t *)&(cprt_vol32_ptr))


#if defined(_WIN32)
  #define CPRT_NET_START do { \
    WSADATA cprt_net_start_wsadata; \
    errno = WSAStartup(MAKEWORD(2,2), &cprt_net_start_wsadata); \
    CPRT_EOK0(errno); \
  } while (0)
  #define CPRT_NET_CLEANUP do { \
    int cprt_net_start_e; \
    cprt_net_start_e = WSACleanup(); \
    if (cprt_net_start_e != 0) { \
      errno = GetLastError(); \
      CPRT_PERRNO("WSACleanup:); \
    } \
  } while (0)

#else  /* Unix */
  #define CPRT_NET_START do { \
    errno = 0; \
  } while (0)
  #define CPRT_NET_CLEANUP do { \
    errno = 0; \
  } while (0)
#endif


#if defined(_WIN32)
  #define CPRT_SNPRINTF _snprintf
  #define CPRT_STRDUP _strdup
  #define CPRT_SLEEP_SEC(s_) Sleep((s_)*1000)
  #define CPRT_SLEEP_MS Sleep
  #define CPRT_STRTOK strtok_s

#else  /* Unix */
  #define CPRT_SNPRINTF snprintf
  #define CPRT_STRDUP strdup
  #define CPRT_SLEEP_SEC sleep
  #define CPRT_SLEEP_MS(ms_) usleep((ms_)*1000)
  #define CPRT_STRTOK strtok_r
#endif


#if defined(_WIN32)
  #define CPRT_MUTEX_T prt_mutex_t;
  #define CPRT_MUTEX_INIT(_m) InitializeCriticalSection(&(_m))
  #define CPRT_MUTEX_INIT_RECURSIVE(_m) InitializeCriticalSection(&(_m))
  #define CPRT_MUTEX_LOCK(_m) EnterCriticalSection(&(_m))
  #define CPRT_MUTEX_TRYLOCK(_got_it, _m) (_got_it) = TryEnterCriticalSection(&(_m))
  #define CPRT_MUTEX_UNLOCK(_m) LeaveCriticalSection(&(_m))
  #define CPRT_MUTEX_DELETE(_m) DeleteCriticalSection(&(_m))

#else  /* Unix */
  #define CPRT_MUTEX_T pthread_mutex_t
  #define CPRT_MUTEX_INIT_RECURSIVE(_m) do { \
    pthread_mutexattr_t _mutexattr; \
    CPRT_EOK0(errno = pthread_mutexattr_init(&_mutexattr)); \
    CPRT_EOK0(errno = pthread_mutexattr_settype(&_mutexattr, \
        PTHREAD_MUTEX_RECURSIVE)); \
    CPRT_EOK0(errno = pthread_mutex_init(&(_m),&_mutexattr)); \
    CPRT_EOK0(errno = pthread_mutexattr_destroy(&_mutexattr)); \
  } while(0)
  #define CPRT_MUTEX_INIT(_m) CPRT_EOK0(errno = pthread_mutex_init(&(_m), NULL))
  #define CPRT_MUTEX_LOCK(_m) CPRT_EOK0(errno = pthread_mutex_lock(&(_m)))
  #define CPRT_MUTEX_TRYLOCK(_got_it, _m) do { \
    errno = pthread_mutex_trylock(&(_m)); \
    if (errno == 0) { \
      _got_it = 1; \
    } else if (errno == EBUSY) { \
      _got_it = 0; \
    } else { \
      CPRT_PERRNO("pthread_mutex_trylock"); \
    } \
  } while (0)
  #define CPRT_MUTEX_UNLOCK(_m) pthread_mutex_unlock(&(_m))
  #define CPRT_MUTEX_DELETE(_m) pthread_mutex_destroy(&(_m))
#endif


#if defined(_WIN32)
  #define CCPRT_SEM_T prt_sem_t;
  #define CPRT_SEM_INIT(_s, _i) do { \
    (_s) = CreateSemaphore(NULL, _i, INT_MAX, NULL); \
    if ((_s) == NULL) { \
      errno = GetLastError();\
      CPRT_PERRNO("CreateThread"); \
    } \
  } while (0)
  #define CPRT_SEM_DELETE(_s) do { \
    BOOL rc_ = CloseHandle(_s); \
    if (rc_ == 0) { \
      errno = GetLastError();\
      CPRT_PERRNO("CreateThread"); \
    } \
  } while (0)
  #define CPRT_SEM_POST(_s) do { \
    BOOL rc_ = ReleaseSemaphore(_s, 1, NULL); \
    if (rc_ == 0) { \
      errno = GetLastError();\
      CPRT_PERRNO("CreateThread"); \
    } \
  } while (0)
  #define CPRT_SEM_WAIT(_s) do { \
    WaitForSingleObject(_s, INFINITE); \
  } while (0)

#elif defined(__APPLE__)
  #define CCPRT_SEM_T dispatch_semaphore_t
  #define CPRT_SEM_INIT(_s, _i) _s = dispatch_semaphore_create(_i)
  #define CPRT_SEM_DELETE(_s) dispatch_release(_s)
  #define CPRT_SEM_POST(_s) dispatch_semaphore_signal(_s)
  #define CPRT_SEM_WAIT(_s) dispatch_semaphore_wait(_s, DISPATCH_TIME_FOREVER)

#else  /* Non-Apple Unixes */
  #define CCPRT_SEM_T sem_t
  #define CPRT_SEM_INIT(_s, _i) CPRT_EOK0(sem_init(&(_s), 0, _i))
  #define CPRT_SEM_DELETE(_s) CPRT_EOK0(sem_destroy(&(_s)))
  #define CPRT_SEM_POST(_s) CPRT_EOK0(sem_post(&(_s)))
  #define CPRT_SEM_WAIT(_s) CPRT_EOK0(sem_wait(&(_s)))
#endif


#if defined(_WIN32)
  #define CPRT_THREAD_T HANDLE
  #define CPRT_THREAD_ENTRYPOINT DWORD WINAPI
  #define CPRT_THREAD_CREATE(_tid, _tstrt, _targ) do {\
    DWORD _ignore, _err;\
    _tid = CreateThread(NULL, 0, _tstrt, _targ, 0, &_ignore);\
    if (_tid == NULL) {\
      errno = GetLastError();\
      CPRT_PERRNO("CreateThread"); \
    }\
  } while (0)
  #define CPRT_THREAD_EXIT do { ExitThread(0); } while (0)
  #define CPRT_THREAD_JOIN(_tid) WaitForSingleObject(_tid, INFINITE)

#else  /* Unix */
  #define CPRT_THREAD_T pthread_t
  #define CPRT_THREAD_ENTRYPOINT void *
  #define CPRT_THREAD_CREATE(_tid, _tstrt, _targ) \
    CPRT_EOK0(errno = pthread_create(&(_tid), NULL, _tstrt, _targ))
  #define CPRT_THREAD_EXIT \
    CPRT_EOK0(errno = pthread_exit(NULL))
  #define CPRT_THREAD_JOIN(_tid) \
    CPRT_EOK0(errno = pthread_join(_tid, NULL))
#endif

#if defined(_WIN32)
#define CPRT_AFFINITY_MASK_T DWORD_PTR;
  #define CPRT_SET_AFFINITY(_in_mask) do { \
    DWORD_PTR _rc;
    _rc = SetThreadAffinityMask(GetCurrentThread(), _in_mask); \
    if (_rc != 0) { \
      errno = GetLastError(); \
      CPRT_PERRNO("SetThreadAffinityMask"); \
    } \
  } while (0)


#elif defined(__linux__)
  #define CPRT_AFFINITY_MASK_T cpu_set_t
  #define CPRT_SET_AFFINITY(in_mask_) do { \
    CPRT_AFFINITY_MASK_T *_cpuset; \
    int _i; \
    size_t _cpuset_sz; \
    uint64_t _bit = 1; \
    uint64_t _bit_mask = _in_mask; \
    _cpuset = CPU_ALLOC(64); \
    _cpuset_sz = CPU_ALLOC_SIZE(64); \
    CPU_ZERO_S(_cpuset_sz, _cpuset); \
    for (_i = 0; _i < 64; _i++) { \
      if ((_bit_mask & _bit) == _bit) { \
        CPU_SET_S(_i, _cpuset_sz, _cpuset); \
      } \
      _bit = _bit << 1; \
    } \
    CPRT_EOK0(errno = pthread_setaffinity_np(0, _cpuset_sz, _cpuset)); \
    CPU_FREE(_cpu_set); \
  } while (0)

#else  /* Non-Linux Unixes not supported. */
  #define CPRT_AFFINITY_MASK_T int
  #define CPRT_SET_AFFINITY(in_mask_) do { \
    errno = 0; \
  } while (0)

#endif
#if defined(_WIN32)
/* The following code is from https://github.com/fordsfords/getopt_port which
 * is forked from https://github.com/kimgr/getopt_port
 */
/*******************************************************************************
 * Copyright (c) 2012-2017, Kim Grasman <kim.grasman@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Kim Grasman nor the
 *     names of contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL KIM GRASMAN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Updated by Steve Ford <fordsfords@gmail.com> to be all-inclusive in the
 * include file. No ".c" file needed. Requires adding GETOPT_PORT above
 * your "main()".
 *
 ******************************************************************************/


  #define no_argument 1
  #define required_argument 2
  #define optional_argument 3

  extern char* optarg;
  extern int optind, opterr, optopt;

  struct option {
    const char* name;
    int has_arg;
    int* flag;
    int val;
  };

  int getopt(int argc, char* const argv[], const char* optstring);

  int getopt_long(int argc, char* const argv[],
    const char* optstring, const struct option* longopts, int* longindex);


  #define GETOPT_PORT \
    char* optarg; \
    int optopt; \
    int optind = 1; \
    int opterr; \
    static char* optcursor = NULL; \
    int getopt(int argc, char* const argv[], const char* optstring) { \
      int optchar = -1; \
      const char* optdecl = NULL; \
      optarg = NULL; \
      opterr = 0; \
      optopt = 0; \
      if (optind >= argc) \
        goto no_more_optchars; \
      if (argv[optind] == NULL) \
        goto no_more_optchars; \
      if (*argv[optind] != '-') \
        goto no_more_optchars; \
      if (strcmp(argv[optind], "-") == 0) \
        goto no_more_optchars; \
      if (strcmp(argv[optind], "--") == 0) { \
        ++optind; \
        goto no_more_optchars; \
      } \
      if (optcursor == NULL || *optcursor == '\0') \
        optcursor = argv[optind] + 1; \
      optchar = *optcursor; \
      optopt = optchar; \
      optdecl = strchr(optstring, optchar); \
      if (optdecl) { \
        if (optdecl[1] == ':') { \
          optarg = ++optcursor; \
          if (*optarg == '\0') { \
            if (optdecl[2] != ':') { \
              if (++optind < argc) { \
                optarg = argv[optind]; \
              } else { \
                optarg = NULL; \
                optchar = (optstring[0] == ':') ? ':' : '?'; \
              } \
            } else { \
              optarg = NULL; \
            } \
          } \
          optcursor = NULL; \
        } \
      } else { \
        optchar = '?'; \
      } \
      if (optcursor == NULL || *++optcursor == '\0') \
        ++optind; \
      return optchar; \
    no_more_optchars: \
      optcursor = NULL; \
      return -1; \
    } \

#else  /* Unix */
  #define GETOPT_PORT
#endif


#if defined(__cplusplus)
}
#endif

#endif  /* CPRT_H */
