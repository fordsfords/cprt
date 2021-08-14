#!/bin/sh
# tst.sh - script to build and run the cprt test program.

fail() {
  egrep "^test" tst.tmp
  echo "FAIL at $(caller)"
  exit 1
}

ok() {
  echo "OK: $(egrep '^test' tst.tmp)"
}


OPTS=""
if echo "$OSTYPE" | egrep -i linux; then :
  OPTS="-l pthread"
fi

gcc -Wall -o cprt $OPTS cprt.c
if [ $? -ne 0 ]; then exit 1; fi


# Disable core files
ulimit -c 0


./cprt -t 0 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test " tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt -t 1 >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "This_should_fail" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "^test |This_should_fail" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt -t 2 >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "This should fail" tst.tmp >/dev/null; then :; else :
  fail; fi
egrep -v "^test |This should fail" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt -t 3 >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "this_should_fail" tst.tmp >/dev/null; then :; else :
  fail; fi
egrep -v "^test |this_should_fail" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt -t 4 >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "This should fail" tst.tmp >/dev/null; then :; else :
  fail; fi
egrep -v "^test |This should fail" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt -t 8 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test " tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

