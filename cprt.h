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

#ifdef __cplusplus
extern "C" {
#endif

/* Macro to approximate the basename() function. */
#define CPRT_BASENAME(_p) ((strrchr(_p, '/') == NULL) ? (_p) : (strrchr(_p, '/')+1))

/* Error handling macros. These print to stderr and exit(1), or in
 * some cases abort. Most real programs would do something better.
 */

#ifdef _WIN32
  #define CPRT_PERR(s_) do { \
    char errno_ = errno; \
    char errstr_[1024]; \
    strerror_s(errno_, errstr_, sizeof(errstr_)); \
    fprintf(stderr, "ERROR (%s line %d): %s errno=%u ('%s')\n", \
      __FILE__, __LINE__, s_, errno, errstr_); \
    fflush(stderr); \
    exit(1); \
  } while (0)
#else
  #define CPRT_PERR(s_) do { \
    char errno_ = errno; \
    char errnostr_[1024]; \
    strerror_r(errno_, errnostr_, sizeof(errnostr_)); \
    fprintf(stderr, "ERROR (%s line %d): %s errno=%u ('%s')\n", \
      __FILE__, __LINE__, s_, errno, errnostr_); \
    fflush(stderr); \
    exit(1); \
  } while (0)
#endif

/* Use when non-zero means error. */
#define CPRT_EOK0(expr_) do { \
  int st_ = (expr_); \
  char errstr_[1024]; \
  if (st_ != 0) { \
    sprintf(errstr_, "'%s' is not 0", #expr_); \
    CPRT_PERR(errstr_); \
  } \
} while (0)

/* Use when NULL means error. */
#define CPRT_ENULL(expr_) do { \
  if ((expr_) == NULL) { \
    char errstr_[1024]; \
    sprintf(errstr_, "'%s' is NULL", #expr_); \
    CPRT_PERR(errstr_); \
  } \
} while (0)

#define CPRT_ABORT(s_) do { \
  fprintf(stderr, "ERROR (%s line %d): %s\n", \
    __FILE__, __LINE__, s_); \
  fflush(stderr); \
  abort(); \
} while (0)

#define CPRT_ASSERT(cond_) do { \
  if (! (cond_)) { \
    fprintf(stderr, "%s:%d, ERROR: '%s' not true\n", \
      __FILE__, __LINE__, #cond_); \
    abort(); \
  } \
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
  #include <windows.h>
  #include <winsock2.h>

#else  /******************************** Unix ********************************/
  #include <unistd.h>
  #include <stdlib.h>
  #include <pthread.h>
  #ifdef __APPLE__
    #include <dispatch/dispatch.h>
  #else  /* Non-Apple Unixes. */
    #include <semaphore.h>
  #endif
#endif


#if defined(_WIN32)
  #define CPRT_SNPRINTF _snprintf
  #define CPRT_STRDUP _strdup
  #define CPRT_SLEEP_SEC(s_) Sleep((s_)*1000)
  #define CPRT_SLEEP_MS Sleep
  #define CPRT_STRTOK_PORT strtok_s

#else  /******************************** Unix ********************************/
  #define CPRT_SNPRINTF snprintf
  #define CPRT_STRDUP strdup
  #define CPRT_SLEEP_SEC sleep
  #define CPRT_SLEEP_MS(ms_) usleep((ms_)*1000)
  #define CPRT_STRTOK_PORT strtok_r
#endif


#if defined(_WIN32)
  #define CPRT_MUTEX_T prt_mutex_t;
  #define CPRT_MUTEX_INIT(_m) InitializeCriticalSection(&(_m))
  #define CPRT_MUTEX_INIT_RECURSIVE(_m) InitializeCriticalSection(&(_m))
  #define CPRT_MUTEX_LOCK(_m) EnterCriticalSection(&(_m))
  #define CPRT_MUTEX_TRYLOCK(_m) (TryEnterCriticalSection(&((_m)))==0)
  #define CPRT_MUTEX_UNLOCK(_m) LeaveCriticalSection(&(_m))
  #define CPRT_MUTEX_DELETE(_m) DeleteCriticalSection(&(_m))
#else  /******************************** Unix ********************************/

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

#else  /******************************** Unix ********************************/
  #ifdef __APPLE__
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

#else  /******************************** Unix ********************************/
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


#ifdef __cplusplus
}
#endif

#endif  /* CPRT_H */
