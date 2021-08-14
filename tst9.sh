#!/bin/sh
# tst9.sh - script to build and run the cprt test #9 (cpu affinity).


OPTS=""
if echo "$OSTYPE" | egrep -i linux; then :
  OPTS="-l pthread"
fi

gcc -Wall -o cprt $OPTS cprt.c
if [ $? -ne 0 ]; then exit 1; fi


# Disable core files
ulimit -c 0


echo "If on Linux, run "top -d 1" and then press "1" to get per-CPU usage"
./cprt -t 9 >tst.tmp
if [ $? -ne 0 ]; then fail; fi

