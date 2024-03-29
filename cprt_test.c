/* cprt_test.c - Test program for the definitions in cprt.h
 * This tries to be portable between Mac, Linux, and Windows.
 * See https://github.com/fordsfords/cprt
 */
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

#include "cprt.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>


/* Options and their defaults */
int o_testnum = 0;


char usage_str[] = "Usage: cprt_test [-h] [-t testnum] [unused_arg]";

void usage(char *msg) {
  if (msg) fprintf(stderr, "%s\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}

void help() {
  fprintf(stderr, "%s\n", usage_str);
  fprintf(stderr, "where:\n"
      "  -h : print help\n"
      "  -t testnum : run specified test\n"
      "  unused_arg : optional argument printed in test 10 (if supplied)\n");
  exit(0);
}


CPRT_THREAD_ID_T main_thread_id;
CPRT_THREAD_ID_T sub_thread_id;

/* Some tests require an independent thread. */

int my_thread_arg;
CPRT_MUTEX_T my_thread_arg_mutex;
CPRT_SPIN_T my_thread_arg_spinlock;
CPRT_SEM_T my_thread_wake_sem;
CPRT_SEM_T my_test_wake_sem;
CPRT_MUTEX_T my_cond_mutex;
CPRT_COND_T my_cond_var;
int my_cond_state;

CPRT_THREAD_ENTRYPOINT thread_test_8(void *in_arg)
{
  int *int_arg = (int *)in_arg;
  int got_lock;
  struct cprt_timespec ts1, ts2;
  uint64_t ts_diff_ns, ms1, ms2;
  struct cprt_timeval tv1, tv2;
  uint64_t tv_diff_us;
  long counter;

  CPRT_ASSERT(int_arg == &my_thread_arg);
  CPRT_ASSERT(my_thread_arg == o_testnum);

  CPRT_MUTEX_TRYLOCK(got_lock, my_thread_arg_mutex);
  CPRT_ASSERT(! got_lock);
  CPRT_MUTEX_LOCK(my_thread_arg_mutex);
  CPRT_ASSERT(my_thread_arg == o_testnum+1);

  CPRT_EOK0(CPRT_TIMEOFDAY(&tv1, NULL));
  CPRT_GETTIME(&ts1);
  ms1 = cprt_get_ms_time();
  CPRT_SLEEP_MS(50);
  CPRT_EOK0(CPRT_TIMEOFDAY(&tv2, NULL));
  CPRT_GETTIME(&ts2);
  ms2 = cprt_get_ms_time();
  CPRT_DIFF_TS(ts_diff_ns, ts2, ts1);
  CPRT_DIFF_TV(tv_diff_us, tv2, tv1);
  printf("50ms = %"PRIu64"ns, %"PRIu64"us\n", ts_diff_ns, tv_diff_us);
  CPRT_ASSERT(ts_diff_ns > 40000000 && ts_diff_ns < 69000000);
  CPRT_ASSERT(tv_diff_us > 40000 && tv_diff_us < 69000);
  CPRT_ASSERT((ms2 - ms1) > 40 && (ms2 - ms1) < 69);
  cprt_ms_printf(ms1, "Test of %s: %"PRIu64"\n", "cprt_ms_printf", ts_diff_ns);
  cprt_ts_printf("Test of %s: %"PRIu64"\n", "cprt_ts_printf", ts_diff_ns);

  my_thread_arg++;  /* Becomes o_testnum+2. */
  CPRT_MUTEX_UNLOCK(my_thread_arg_mutex);

  CPRT_SLEEP_MS(10);
  CPRT_MUTEX_LOCK(my_thread_arg_mutex);
  CPRT_ASSERT(my_thread_arg == o_testnum+3);
  my_thread_arg++;  /* Becomes o_testnum+4. */
  CPRT_MUTEX_UNLOCK(my_thread_arg_mutex);
  CPRT_SLEEP_MS(50);

  counter = 0;
  CPRT_ASSERT(CPRT_ATOMIC_INC_VAL(&counter) == 1);
  CPRT_ASSERT(CPRT_ATOMIC_INC_VAL(&counter) == 2);
  CPRT_ASSERT(counter == 2);
  CPRT_ASSERT(CPRT_ATOMIC_DEC_VAL(&counter) == 1);
  CPRT_ASSERT(CPRT_ATOMIC_DEC_VAL(&counter) == 0);
  CPRT_ASSERT(counter == 0);

  sub_thread_id = CPRT_GET_THREAD_ID();
  printf("Main thread ID=%"PRIu64", Sub thread ID=%"PRIu64"\n",
      main_thread_id, sub_thread_id);
  CPRT_ASSERT(main_thread_id != sub_thread_id);

  CPRT_THREAD_EXIT;
  return 0;
}  /* thread_test_8 */

CPRT_THREAD_ENTRYPOINT thread_test_8_1(void *in_arg)
{
  int *int_arg = (int *)in_arg;
  int got_lock;
  struct cprt_timespec ts1, ts2;
  uint64_t ts_diff_ns;
  long counter;

  CPRT_ASSERT(int_arg == &my_thread_arg);
  CPRT_ASSERT(my_thread_arg == o_testnum);

  CPRT_SPIN_TRYLOCK(got_lock, my_thread_arg_spinlock);
  CPRT_ASSERT(! got_lock);
  CPRT_SPIN_LOCK(my_thread_arg_spinlock);
  CPRT_ASSERT(my_thread_arg == o_testnum+1);

  CPRT_GETTIME(&ts1);
  CPRT_SLEEP_MS(50);
  CPRT_GETTIME(&ts2);
  CPRT_DIFF_TS(ts_diff_ns, ts2, ts1);
  printf("50ms = %"PRIu64"ns\n", ts_diff_ns);
  CPRT_ASSERT(ts_diff_ns > 40000000 && ts_diff_ns < 69000000);

  my_thread_arg++;  /* Becomes o_testnum+2. */
  CPRT_SPIN_UNLOCK(my_thread_arg_spinlock);

  CPRT_SLEEP_MS(10);
  CPRT_SPIN_LOCK(my_thread_arg_spinlock);
  CPRT_ASSERT(my_thread_arg == o_testnum+3);
  my_thread_arg++;  /* Becomes o_testnum+4. */
  CPRT_SPIN_UNLOCK(my_thread_arg_spinlock);
  CPRT_SLEEP_MS(50);

  counter = 0;
  CPRT_ASSERT(CPRT_ATOMIC_INC_VAL(&counter) == 1);
  CPRT_ASSERT(CPRT_ATOMIC_INC_VAL(&counter) == 2);
  CPRT_ASSERT(counter == 2);
  CPRT_ASSERT(CPRT_ATOMIC_DEC_VAL(&counter) == 1);
  CPRT_ASSERT(CPRT_ATOMIC_DEC_VAL(&counter) == 0);
  CPRT_ASSERT(counter == 0);

  CPRT_THREAD_EXIT;
  return 0;
}  /* thread_test_8_1 */

CPRT_THREAD_ENTRYPOINT thread_test_8_2(void *in_arg)
{
  int *int_arg = (int *)in_arg;

  CPRT_ASSERT(int_arg == &my_thread_arg);
  CPRT_ASSERT(my_thread_arg == o_testnum);

  CPRT_SLEEP_MS(100);
  my_thread_arg++;
  CPRT_SEM_POST(my_test_wake_sem);

  CPRT_SEM_WAIT(my_thread_wake_sem);
  CPRT_ASSERT(my_thread_arg == (o_testnum+2));

  CPRT_SLEEP_MS(100);
  my_thread_arg++;
  CPRT_THREAD_EXIT;
  return 0;
}  /* thread_test_8_2 */

CPRT_THREAD_ENTRYPOINT thread_test_8_3(void *in_arg)
{
  int *int_arg = (int *)in_arg;

  CPRT_ASSERT(int_arg == &my_thread_arg);
  CPRT_ASSERT(my_thread_arg == o_testnum);

  CPRT_SLEEP_MS(50);
  {
    CPRT_MUTEX_LOCK(my_cond_mutex);
    my_cond_state = 1;
    CPRT_COND_SIGNAL(my_cond_var);
    CPRT_MUTEX_UNLOCK(my_cond_mutex);
  }

  CPRT_THREAD_EXIT;
  return 0;
}  /* thread_test_8_3 */


/* Affinity stuff not for Mac. */
#if ! defined(__APPLE__)
CPRT_THREAD_ENTRYPOINT thread_test_9(void *in_arg)
{
  uint64_t affinity = 0x01;
  int i;
  time_t cur_time;

  /* Wait till the next second. */
  cur_time = time(NULL);
  while (time(NULL) < (cur_time + 1)) { }

  for (i = 0; i < 4; i++) {
    fprintf(stderr, "Setting affinity to %"PRIu64"\n", affinity);
    fflush(stderr);
    cprt_set_affinity(affinity);

    /* Wait 4 seconds. */
    cur_time = time(NULL);
    while (time(NULL) < (cur_time + 4)) { }
    affinity = affinity << 1;
  }

  CPRT_THREAD_EXIT;
  return 0;
}  /* thread_test_9 */
#endif


int main(int argc, char **argv)
{
  int opt;

  CPRT_NET_START;

  main_thread_id = CPRT_GET_THREAD_ID();

  while ((opt = cprt_getopt(argc, argv, "ht:")) != EOF) {
    switch (opt) {
      case 't':
        CPRT_ATOI(cprt_optarg, o_testnum);
        break;
      case 'h':
        help();
        break;
      case '?':
        fprintf(stderr, "cprt_optopt='%c', Use '-h' for help\n", cprt_optopt);
        exit(1);
        break;
      default:
        fprintf(stderr, "Undefined option '%c'\n", opt);
        exit(1);
    }  /* switch opt */
  }  /* while cprt_getopt */

  CPRT_INITTIME();

  switch(o_testnum) {
    case 0:
      fprintf(stderr, "test %d: GETOPT_PORT, CPRT_NET_START\n", o_testnum);
      fflush(stderr);
      break;

    case 1:
    {
      FILE *fp;
      struct cprt_timeval tv;
      struct tm out_tm;
      fprintf(stderr, "test %d: CPR_NULL, CPRT_PERRNO\n", o_testnum);
      CPRT_EOK0(CPRT_TIMEOFDAY(&tv, NULL));
      CPRT_LOCALTIME_R(&(tv.tv_sec), &out_tm);
      fprintf(stderr, "%04d/%02d/%02d %02d:%02d:%02d.%06d\n",
        (int)out_tm.tm_year + 1900, (int)out_tm.tm_mon + 1, (int)out_tm.tm_mday,
        (int)out_tm.tm_hour, (int)out_tm.tm_min, (int)out_tm.tm_sec,
        (int)tv.tv_usec);
      fflush(stderr);
      CPRT_ENULL(fp = fopen("cprt.c", "r")); 
      fclose(fp); 
      CPRT_ENULL(fp = fopen("This_should_fail", "r")); 
      break;
    }

    case 2:
      cprt_ts_eprintf("test %d: CPRT_ASSERT\n", o_testnum);
      CPRT_ASSERT((o_testnum == 2) && "This should NOT fail");
      CPRT_ASSERT((o_testnum != 2) && "This should fail");
      break;

    case 3:
    {
      FILE *this_should_not_fail_fp;
      int this_should_fail = 22;
      fprintf(stderr, "test %d: CPRT_EOK0\n", o_testnum);
      fflush(stderr);
      CPRT_ENULL(this_should_not_fail_fp = fopen("cprt.c", "r"));
      CPRT_EOK0(fclose(this_should_not_fail_fp));
      CPRT_EOK0(errno = this_should_fail);
      break;
    }

    case 4:
      fprintf(stderr, "test %d: CPRT_ABORT\n", o_testnum);
      fflush(stderr);
      CPRT_ABORT("CPRT_ABORT This should fail");
      break;

    case 5:
      fprintf(stderr, "test %d: CPRT_SLEEP_SEC\n", o_testnum);
      fflush(stderr);
      CPRT_SLEEP_SEC(1);
      break;

    case 6:
      fprintf(stderr, "test %d: CPRT_SLEEP_MS 1000\n", o_testnum);
      fflush(stderr);
      CPRT_SLEEP_MS(1000);
      break;

    case 66:
      fprintf(stderr, "test %d: CPRT_SLEEP_NS 1000000000\n", o_testnum);
      fflush(stderr);
      CPRT_SLEEP_NS(1000000000);
      break;

    case 7:
    {
      char *str, *word, *context;

      fprintf(stderr, "test %d: CPRT_STRTOK\n", o_testnum);
      fflush(stderr);
      str = strdup("abc,xyz,123");
      for (word = CPRT_STRTOK(str, ",", &context);
          word != NULL;
          word = CPRT_STRTOK(NULL, ",", &context)) {
        printf("word='%s'\n", word);
      }

      break;
    }

    case 8:
    {
      CPRT_THREAD_T my_thread_id;
      int got_lock;
      fprintf(stderr, "test %d: CPRT_THREAD_CREATE, CPRT_MUTEX_INIT\n", o_testnum);
      fflush(stderr);

      CPRT_MUTEX_INIT(my_thread_arg_mutex);
      CPRT_MUTEX_LOCK(my_thread_arg_mutex);
      my_thread_arg = o_testnum;

      CPRT_THREAD_CREATE(my_thread_id, thread_test_8, &my_thread_arg);

      CPRT_SLEEP_MS(50);
      CPRT_ASSERT(my_thread_arg == o_testnum);
      my_thread_arg++;  /* Becomes o_testnum+1 */
      CPRT_MUTEX_UNLOCK(my_thread_arg_mutex);

      CPRT_SLEEP_MS(10);
      CPRT_MUTEX_TRYLOCK(got_lock, my_thread_arg_mutex);
      CPRT_ASSERT(! got_lock);
      while (! got_lock) {
        CPRT_MUTEX_TRYLOCK(got_lock, my_thread_arg_mutex);
      }
      CPRT_ASSERT(my_thread_arg == o_testnum+2);

      my_thread_arg++;  /* Becomes o_testnum+3 */
      CPRT_MUTEX_UNLOCK(my_thread_arg_mutex);
      CPRT_SLEEP_MS(50);

      CPRT_THREAD_JOIN(my_thread_id);
      CPRT_ASSERT(my_thread_arg == o_testnum+4);

      CPRT_MUTEX_DELETE(my_thread_arg_mutex);

      break;
    }

    case 81:
    {
      CPRT_THREAD_T my_thread_id;
      int got_lock;
      fprintf(stderr, "test %d: CPRT_THREAD_CREATE, CPRT_SPIN_INIT\n", o_testnum);
      fflush(stderr);

      CPRT_SPIN_INIT(my_thread_arg_spinlock);
      CPRT_SPIN_LOCK(my_thread_arg_spinlock);
      my_thread_arg = o_testnum;

      CPRT_THREAD_CREATE(my_thread_id, thread_test_8_1, &my_thread_arg);

      CPRT_SLEEP_MS(50);
      CPRT_ASSERT(my_thread_arg == o_testnum);
      my_thread_arg++;  /* Becomes o_testnum+1 */
      CPRT_SPIN_UNLOCK(my_thread_arg_spinlock);

      CPRT_SLEEP_MS(10);
      CPRT_SPIN_TRYLOCK(got_lock, my_thread_arg_spinlock);
      CPRT_ASSERT(! got_lock);
      while (! got_lock) {
        CPRT_SPIN_TRYLOCK(got_lock, my_thread_arg_spinlock);
      }
      CPRT_ASSERT(my_thread_arg == o_testnum+2);

      my_thread_arg++;  /* Becomes o_testnum+3 */
      CPRT_SPIN_UNLOCK(my_thread_arg_spinlock);
      CPRT_SLEEP_MS(50);

      CPRT_THREAD_JOIN(my_thread_id);
      CPRT_ASSERT(my_thread_arg == o_testnum+4);

      CPRT_SPIN_DELETE(my_thread_arg_spinlock);
      break;
    }

    case 82:
    {
      CPRT_THREAD_T my_thread_id;
      fprintf(stderr, "test %d: CPRT_THREAD_CREATE, CPRT_SEM_INIT\n", o_testnum);
      fflush(stderr);

      CPRT_SEM_INIT(my_thread_wake_sem, 0);
      CPRT_SEM_INIT(my_test_wake_sem, 0);
      my_thread_arg = o_testnum;

      CPRT_THREAD_CREATE(my_thread_id, thread_test_8_2, &my_thread_arg);

      CPRT_SEM_WAIT(my_test_wake_sem);
      CPRT_ASSERT(my_thread_arg == (o_testnum + 1));

      CPRT_SLEEP_MS(100);
      my_thread_arg++;
      CPRT_SEM_POST(my_thread_wake_sem);

      CPRT_THREAD_JOIN(my_thread_id);
      CPRT_ASSERT(my_thread_arg == o_testnum+3);

      CPRT_SEM_DELETE(my_thread_wake_sem);
      CPRT_SEM_DELETE(my_test_wake_sem);

      break;
    }

    case 83:
    {
      struct cprt_timespec ts1, ts2;
      uint64_t ts_diff_ns;
      CPRT_THREAD_T my_thread_id;
      fprintf(stderr, "test %d: CPRT_THREAD_CREATE, CPRT_COND_INIT\n", o_testnum);
      fflush(stderr);

      CPRT_MUTEX_INIT(my_cond_mutex);
      CPRT_COND_INIT(my_cond_var);
      my_cond_state = 0;
      my_thread_arg = o_testnum;

      CPRT_THREAD_CREATE(my_thread_id, thread_test_8_3, &my_thread_arg);

      CPRT_GETTIME(&ts1);
      {
        CPRT_MUTEX_LOCK(my_cond_mutex);
        while (my_cond_state != 1) {
          CPRT_COND_WAIT(my_cond_var, my_cond_mutex);
        }
        CPRT_MUTEX_UNLOCK(my_cond_mutex);
      }
      CPRT_GETTIME(&ts2);
      CPRT_DIFF_TS(ts_diff_ns, ts2, ts1);
      CPRT_ASSERT(ts_diff_ns > 40000000 && ts_diff_ns < 69000000);

      CPRT_THREAD_JOIN(my_thread_id);

      CPRT_MUTEX_DELETE(my_cond_mutex);
      CPRT_COND_DELETE(my_cond_var);

      break;
    }

    case 9:
    {
#if defined(__APPLE__)
      fprintf(stderr, "test %d: ERROR, threading not supported for Mac\n", o_testnum);
#else
      CPRT_THREAD_T my_thread_id;
      uint64_t cpuset = 0xdeadbeef;
      fprintf(stderr, "test %d: CPRT_SET_AFFINITY\n", o_testnum);
      fflush(stderr);

      CPRT_CPU_ZERO(&cpuset);
      CPRT_ASSERT(cpuset == 0);
      CPRT_CPU_SET(3, &cpuset);
      CPRT_ASSERT(cpuset == 0x8);
      CPRT_CPU_SET(0, &cpuset);
      CPRT_ASSERT(cpuset == 0x9);

      CPRT_THREAD_CREATE(my_thread_id, thread_test_9, NULL);
      CPRT_THREAD_JOIN(my_thread_id);
#endif

      break;
    }

    case 10:
    {
      fprintf(stderr, "test %d: cprt_getopt\n", o_testnum);
      if (cprt_optind >= argc) {
        fprintf(stderr, "Test=10\n");
      } else {
        fprintf(stderr, "Test=10, Argv[%d]='%s'\n",
            cprt_optind, argv[cprt_optind]);
      }

      break;
    }

    default: /* CPRT_ABORT */
      fprintf(stderr, "Bad test number: %d\n", o_testnum);
      fflush(stderr);
      CPRT_ABORT("Bad o_testnum");
  }

  CPRT_NET_CLEANUP;
  return 0;
}  /* main */
