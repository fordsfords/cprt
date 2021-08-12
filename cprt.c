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

#include "safe_atoi.h"
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


int main(int argc, char **argv)
{
  int opt;

#ifdef _WIN32
  /* windows-specific code */
  WSADATA wsadata;
  int wsStat = WSAStartup(MAKEWORD(2,2), &wsadata);
  if (wsStat != 0) {printf("line %d: wsStat=%d\n",__LINE__,wsStat);exit(1);}
#endif

  while ((opt = getopt(argc, argv, "ht:")) != EOF) {
    switch (opt) {
      case 't':
        SAFE_ATOI(optarg, o_testnum);
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
    case 0:  /* no test */
      break;

    case 1:
      fprintf(stderr, "ASSRT\n");
      ASSRT(o_testnum == 1 && "internal test fail");
      ASSRT(o_testnum != 1 && "should fail");
      break;

    case 2:
    {
      FILE *perr_fp;
      fprintf(stderr, "PERR\n");
      perr_fp = fopen("file_not_exist", "r");
      if (perr_fp == NULL) {
        PERR("errno should be 'file not found':");
      } else {
        ABRT("Internal test failure: 'file_not_exist' appears to exist");
      }
      break;
    }

    case 3:
    {
      FILE *perr_fp;
      fprintf(stderr, "EOK0\n");
      perr_fp = fopen("cprt.c", "r");
      if (perr_fp == NULL) {
        ABRT("Internal test failure: 'cprt.c' appears to not exist");
      } else {
        EOK0(fclose(perr_fp) && "internal test fail");
        EOK0(fclose(perr_fp) && "should fail with bad file descr");
      }
      break;
    }

    case 4:
    {
      FILE *perr_fp;
      fprintf(stderr, "ENULL\n");
      ENULL(perr_fp = fopen("cprt.c", "r")); /* should be OK. */
      ENULL(perr_fp = fopen("file_not_exist", "r")); /* should fail. */
      break;
    }

    case 5:
      fprintf(stderr, "ABRT\n");
      ABRT("ABRT test");
      break;

    case 6:
      fprintf(stderr, "SLEEP_SEC\n");
      SLEEP_SEC(1);
      fprintf(stderr, "Done\n");
      break;

    case 7:
      fprintf(stderr, "SLEEP_MS 1000\n");
      SLEEP_MS(1000);
      fprintf(stderr, "Done\n");
      break;

    case 8:
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

    default: /* ABRT */
      ABRT("unknown option, aborting.");
  }

#ifdef _WIN32
  WSACleanup();
#endif
}  /* main */
