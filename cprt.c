/* cprt.c - Test program for the definitions in cprt.h
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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "cprt.h"


/* Options and their defaults */
int o_testnum = 0;


char usage_str[] = "Usage: cprt [-h] [-t testnum]";

void usage(char *msg) {
  if (msg) fprintf(stderr, "%s\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}

void help() {
  fprintf(stderr, "%s\n", usage_str);
  fprintf(stderr, "where:\n"
                  "  -h : print help\n"
                  "  -t testnum : run specified test\n");
  exit(0);
}


GETOPT_PORT

int main(int argc, char **argv)
{
  int opt;

  CPRT_NET_START;

  while ((opt = getopt(argc, argv, "ht:")) != EOF) {
    switch (opt) {
      case 't':
        o_testnum = atoi(optarg);
        break;
      case 'h':
        help();
        break;
      default:
        usage(NULL);
    }  /* switch opt */
  }  /* while getopt */

  if (optind != argc) { usage("Extra parameter(s)"); }

  switch(o_testnum) {
    case 0:
      fprintf(stderr, "test %d: GETOPT_PORT, CPRT_NET_START\n", o_testnum);
      fflush(stderr);
      break;

    case 1:
    {
      FILE *fp;
      fprintf(stderr, "test %d: CPR_NULL, CPRT_PERRNO\n", o_testnum);
      fflush(stderr);
      CPRT_ENULL(fp = fopen("cprt.c", "r")); 
      fclose(fp); 
      CPRT_ENULL(fp = fopen("This_should_fail", "r")); 
      break;
    }

    case 2:
      fprintf(stderr, "test %d: CPRT_ASSERT\n", o_testnum);
      fflush(stderr);
      CPRT_ASSERT((o_testnum == 2) && "This should NOT fail");
      CPRT_ASSERT((o_testnum != 2) && "This should fail");
      break;

    case 3:
    {
      FILE *perr_fp;
      fprintf(stderr, "test %d: CPRT_EOK0\n", o_testnum);
      fflush(stderr);
      perr_fp = fopen("cprt.c", "r");
      CPRT_EOK0(fclose(perr_fp) && "This should NOT fail");
      CPRT_EOK0(fclose(perr_fp) && "This should fail with bad file descr");
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

    case 7:
    {
      char *str, *word, *context;

      fprintf(stderr, "test %d: STRTOK_PORT\n", o_testnum);
      fflush(stderr);
      str = strdup("abc,xyz,123");
      for (word = strtok_r(str, ",", &context);
          word != NULL;
          word = strtok_r(NULL, ",", &context)) {
        printf("word='%s'\n", word);
      }

      break;
    }

    default: /* CPRT_ABORT */
      fprintf(stderr, "Bad test number: %d\n", o_testnum);
      fflush(stderr);
      CPRT_ABORT("Bad o_testnum");
  }

  CPRT_NET_CLEANUP;
}  /* main */
