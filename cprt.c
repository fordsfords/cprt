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
      break;

    case 1:
    {
      FILE *fp;
      fprintf(stderr, "test CPR_NULL, CPRT_PERRNO\n");
      CPRT_ENULL(fp = fopen("cprt.c", "r")); 
      fclose(fp); 
      CPRT_ENULL(fp = fopen("This_should_fail", "r")); 
      break;
    }

    case 2:
      fprintf(stderr, "test CPRT_ASSERT\n");
      CPRT_ASSERT((o_testnum == 2) && "this should NOT fail");
      CPRT_ASSERT((o_testnum != 2) && "this should fail");
      break;

    case 3:
    {
      FILE *perr_fp;
      fprintf(stderr, "CPRT_EOK0\n");
      perr_fp = fopen("cprt.c", "r");
      CPRT_EOK0(fclose(perr_fp) && "this should NOT fail");
      CPRT_EOK0(fclose(perr_fp) && "this should fail with bad file descr");
      break;
    }

    case 4:
      fprintf(stderr, "CPRT_ABORT\n");
      CPRT_ABORT("CPRT_ABORT test");
      break;

    case 5:
      fprintf(stderr, "CPRT_SLEEP_SEC\n");
      CPRT_SLEEP_SEC(1);
      fprintf(stderr, "Done\n");
      break;

    case 6:
      fprintf(stderr, "CPRT_SLEEP_MS 1000\n");
      CPRT_SLEEP_MS(1000);
      fprintf(stderr, "Done\n");
      break;

    case 7:
    {
      char *str, *word, *context;

      fprintf(stderr, "STRTOK_PORT\n");
      str = strdup("abc,xyz,123");
      for (word = strtok_r(str, ",", &context);
          word != NULL;
          word = strtok_r(NULL, ",", &context)) {
        printf("word='%s'\n", word);
      }

      break;
    }

    default: /* CPRT_ABORT */
      CPRT_ABORT("unknown option, aborting.");
  }

  CPRT_NET_CLEANUP;
}  /* main */
