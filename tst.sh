#!/bin/sh
# tst.sh - script to build and run the cprt test program.

gcc -Wall -o cprt cprt.c
if [ $? -ne 0 ]; then exit 1; fi

./cprt -t 0 >tst.tmp 2>&1
if [ $? -eq 0 -a ! -s tst.tmp ]; then echo 1: ok
else echo 1: FAIL
fi

./cprt -t 1 >tst.tmp 2>&1
if [ $? -eq 1 -a -s tst.tmp ]; then echo 1: ok
else echo 1: FAIL
fi
