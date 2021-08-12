#!/bin/sh
# tst.sh - script to build and run the cprt test program.

gcc -Wall -o cprt cprt.c
if [ $? -ne 0 ]; then exit 1; fi

./cprt $*
