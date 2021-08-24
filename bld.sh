#!/bin/sh


OPTS=""
if echo "$OSTYPE" | egrep -i linux; then :
  OPTS="-l pthread"
fi

gcc -Wall -o cprt_test $OPTS cprt.c cprt_test.c
if [ $? -ne 0 ]; then exit 1; fi
