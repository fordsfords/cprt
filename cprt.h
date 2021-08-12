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
  #define CPRT_PERRNO(s_) do { \
    char errno_ = errno; \
    char errstr_[1024]; \
    strerror_s(errno_, errstr_, sizeof(errstr_)); \
    fprintf(stderr, "ERROR (%s line %d): %s errno=%u ('%s')\n", \
      __FILE__, __LINE__, s_, errno, errstr_); \
    fflush(stderr); \
    exit(1); \
  } while (0)

#else  /* Unix */
  #define CPRT_PERRNO(s_) do { \
    char errno_ = errno; \
    char errstr_[1024]; \
    strerror_r(errno_, errstr_, sizeof(errstr_)); \
    fprintf(stderr, "ERROR (%s line %d): %s errno=%u ('%s')\n", \
      __FILE__, __LINE__, s_, errno, errstr_); \
    fflush(stderr); \
    exit(1); \
  } while (0)
#endif

/* Use when non-zero means error. */
#define CPRT_EOK0(expr_) do { \
  errno = 0; \
  if ((expr_) != 0) { \
    int errno_ = errno; \
    char errstr_[1024]; \
    CPRT_SNPRINTF(errstr_, sizeof(errstr_), "'%s' is not 0", #expr_); \
    errno = errno_; \
    CPRT_PERRNO(errstr_); \
  } \
} while (0)

/* Use when NULL means error. */
#define CPRT_ENULL(expr_) do { \
  errno = 0; \
  if ((expr_) == NULL) { \
    int errno_ = errno; \
    char errstr_[1024]; \
    CPRT_SNPRINTF(errstr_, sizeof(errstr_), "'%s' is NULL", #expr_); \
    errno = errno_; \
    CPRT_PERRNO(errstr_); \
  } \
} while (0)

#define CPRT_ASSERT(cond_) do { \
  if (! (cond_)) { \
    fprintf(stderr, "%s:%d, ERROR: '%s' not true\n", \
      __FILE__, __LINE__, #cond_); \
    fflush(stderr); \
    exit(1); \
  } \
} while (0)

#define CPRT_ABORT(s_) do { \
  fprintf(stderr, "%s:%d, ABORT: %s\n", \
    __FILE__, __LINE__, s_); \
  fflush(stderr); \
  abort(); \
} while (0)


/* Macro to make it easier to use sscanf().  See:
 *   https://stackoverflow.com/questions/25410690
 */
#define CPRT_CPRT_STRDEF2(_s) #_s
#define CPRT_STRDEF(_s) CPRT_CPRT_STRDEF2(_s)

/* See http://blog.geeky-boy.com/2014/06/clangllvm-optimize-o3-understands-free.html
 *  for why the CPRT_VOL32 macro is needed.
 */
#define CPRT_VOL32(x) (*(volatile lbm_uint32_t *)&(x))


#if defined(_WIN32)
  #define CPRT_NET_START do { \
    WSADATA wsadata; \
    errno = WSAStartup(MAKEWORD(2,2), &wsadata); \
    CPRT_EOK0(errno); \
  } while (0)
  #define CPRT_NET_CLEANUP do { \
    int e_; \
    e_ = WSACleanup(); \
    if (e_ != 0) { \
      errno = GetLastError(); \
      CPRT_PERRNO("WSACleanup:); \
    } \
  } while (0)

#else  /* Unix */
  #define CPRT_START_NET do { \
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
  #define CPRT_MUTEX_TRYLOCK(_m) (TryEnterCriticalSection(&((_m)))==0)
  #define CPRT_MUTEX_UNLOCK(_m) LeaveCriticalSection(&(_m))
  #define CPRT_MUTEX_DELETE(_m) DeleteCriticalSection(&(_m))

#else  /* Unix */
  #define CPRT_MUTEX_T pthread_mutex_t
  #define CPRT_MUTEX_INIT_RECURSIVE(_m) do { \
    pthread_mutexattr_t errchk_attr; \
    pthread_mutexattr_init(&errchk_attr); \
    pthread_mutexattr_settype(&errchk_attr,PTHREAD_MUTEX_RECURSIVE); \
    pthread_mutex_init(&(_m),&errchk_attr); \
    pthread_mutexattr_destroy(&errchk_attr); \
  } while(0)
  #define CPRT_MUTEX_INIT(_m) pthread_mutex_init(&(_m),NULL)
  #define CPRT_MUTEX_LOCK(_m) pthread_mutex_lock(&(_m))
  #define CPRT_MUTEX_TRYLOCK(_m) pthread_mutex_trylock(&((_m)))
  #define CPRT_MUTEX_UNLOCK(_m) pthread_mutex_unlock(&(_m))
  #define CPRT_MUTEX_DELETE(_m) pthread_mutex_destroy(&(_m))
#endif


#if defined(_WIN32)
  #define CCPRT_SEM_T prt_sem_t;
  #define CPRT_SEM_INIT(_s, _i) _s = CreateSemaphore(NULL, _i, INT_MAX, NULL)
  #define CPRT_SEM_DELETE(_s) CloseHandle(_s)
  #define CPRT_SEM_POST(_s) ReleaseSemaphore(_s, 1, NULL)
  #define CPRT_SEM_WAIT(_s) WaitForSingleObject(_s, INFINITE)

#elif defined(__APPLE__)
  #define CCPRT_SEM_T dispatch_semaphore_t
  #define CPRT_SEM_INIT(_s, _i) _s = dispatch_semaphore_create(_i)
  #define CPRT_SEM_DELETE(_s) dispatch_release(_s)
  #define CPRT_SEM_POST(_s) dispatch_semaphore_signal(_s)
  #define CPRT_SEM_WAIT(_s) dispatch_semaphore_wait(_s, DISPATCH_TIME_FOREVER)

#else  /* Non-Apple Unixes */
  #define CCPRT_SEM_T sem_t
  #define CPRT_SEM_INIT(_s, _i) sem_init(&(_s), 0, _i)
  #define CPRT_SEM_DELETE(_s) sem_destroy(&(_s))
  #define CPRT_SEM_POST(_s) sem_post(&(_s))
  #define CPRT_SEM_WAIT(_s) sem_wait(&(_s))
#endif


#if defined(_WIN32)
  #define CPRT_THREAD_T HANDLE
  #define CPRT_THREAD_ENTRYPOINT DWORD WINAPI
  #define CPRT_THREAD_CREATE(_stat, _tid, _tstrt, _targ) do {\
    DWORD _ignore, _err;\
    _tid = CreateThread(NULL, 0, _tstrt, _targ, 0, &_ignore);\
    _err = GetLastError();\
    if (_tid == NULL) {\
      _stat = LBM_FAILURE;\
      lbm_seterrf(LBM_EINVAL, "Windows returned code %d", _err);\
    } else {\
      _stat = LBM_OK;\
    }\
  } while (0)
  #define CPRT_THREAD_EXIT do { ExitThread(0); } while (0)
  #define CPRT_THREAD_JOIN(_tid) WaitForSingleObject(_tid, INFINITE)

#else  /* Unix */
  #define CPRT_THREAD_T pthread_t
  #define CPRT_THREAD_ENTRYPOINT void *
  #define CPRT_THREAD_CREATE(_stat, _tid, _tstrt, _targ) do {\
    int _err;\
    _err = pthread_create(&_tid, NULL, _tstrt, _targ);\
    if (_err != 0) {\
      _stat = LBM_FAILURE;\
      lbm_seterrf(LBM_EINVAL, "%s", strerror(_err));\
    } else {\
      _stat = LBM_OK;\
    }\
  } while (0)
  #define CPRT_THREAD_EXIT do { pthread_exit(NULL); } while (0)
  #define CPRT_THREAD_JOIN(_tid) pthread_join(_tid, NULL)
#endif

#if defined(_WIN32)
#define CPRT_AFFINITY_MASK_T DWORD_PTR;
  #define CPRT_SET_AFFINITY(in_mask_) do { \
    DWORD_PTR rc_;
    rc_ = SetThreadAffinityMask(GetCurrentThread(), in_mask_); \
    if (rc_ == 0) { \
      errno = GetLastError(); \
      CPRT_PERRNO("SetThreadAffinityMask"); \
    } \
  } while (0)


#elif defined(__linux__)
  #define CPRT_AFFINITY_MASK_T cpu_set_t
  #define CPRT_SET_AFFINITY(in_mask_) do { \
    CPRT_AFFINITY_MASK_T *cpuset_; \
    int i_; \
    size_t cpuset_sz_; \
    uint64_t bit_ = 1; \
    uint64_t bit_mask_ = in_mask_; \
    cpuset_ = CPU_ALLOC(64); \
    cpuset_sz_ = CPU_ALLOC_SIZE(64); \
    CPU_ZERO_S(cpuset_sz_, cpuset_); \
    for (i = 0; i < 64; i++) { \
      if ((bit_mask_ & bit_) == bit_) { \
        CPU_SET_S(i_, cpuset_sz_, cpuset_); \
      } \
      bit_ = bit_ << 1; \
    } \
    errno = pthread_setaffinity_np(0, cpuset_sz_, cpuset_); CPRT_EOK0(errno); \
    CPU_FREE(cpu_set_); \
  } while (0)

#else  /* Non-Linux Unixes not supported. */
#define CPRT_AFFINITY_MASK_T int

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
     \
    static char* optcursor = NULL; \
     \
    int getopt(int argc, char* const argv[], const char* optstring) { \
      int optchar = -1; \
      const char* optdecl = NULL; \
     \
      optarg = NULL; \
      opterr = 0; \
      optopt = 0; \
     \
      if (optind >= argc) \
        goto no_more_optchars; \
     \
      if (argv[optind] == NULL) \
        goto no_more_optchars; \
     \
      if (*argv[optind] != '-') \
        goto no_more_optchars; \
     \
      if (strcmp(argv[optind], "-") == 0) \
        goto no_more_optchars; \
     \
      if (strcmp(argv[optind], "--") == 0) { \
        ++optind; \
        goto no_more_optchars; \
      } \
     \
      if (optcursor == NULL || *optcursor == '\0') \
        optcursor = argv[optind] + 1; \
     \
      optchar = *optcursor; \
     \
      optopt = optchar; \
     \
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
     \
          optcursor = NULL; \
        } \
      } else { \
        optchar = '?'; \
      } \
     \
      if (optcursor == NULL || *++optcursor == '\0') \
        ++optind; \
     \
      return optchar; \
     \
    no_more_optchars: \
      optcursor = NULL; \
      return -1; \
    } \
#endif


#if defined(__cplusplus)
}
#endif

#endif // INCLUDED_GETOPT_PORT_H



#ifdef __cplusplus
}
#endif

#endif  /* CPRT_H */
