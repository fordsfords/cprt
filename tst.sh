#!/bin/sh
# tst.sh - script to run the cprt test program.

fail() {
  egrep "^test" tst.tmp
  echo "FAIL at $(caller)"
  exit 1
}

ok() {
  echo "OK: $(egrep '^test' tst.tmp)"
}


# Disable core files
ulimit -c 0


./cprt_test -t 0 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test " tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t x >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
egrep -v "Error, invalid number for o_testnum|errno=22" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t 0x1 >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "This_should_fail" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "^test |This_should_fail|[0-9][0-9][0-9][0-9]/[0-9][0-9]" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
egrep "[0-9][0-9][0-9][0-9]/[0-9][0-9]" tst.tmp
date
ok

./cprt_test -t 2 >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "This should fail" tst.tmp >/dev/null; then :; else :
  fail; fi
DATE=`date +%Y-%m-%d`
egrep -v "^$DATE ..:..:..\....: test 2: |This should fail" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t 3 >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "this_should_fail" tst.tmp >/dev/null; then :; else fail; fi
egrep -iv "^test |this_should_fail.*invalid argument" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t 4 >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "This should fail" tst.tmp >/dev/null; then :; else :
  fail; fi
egrep -v "^test |This should fail" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

if which gdate >/dev/null 2>&1; then gdate +"%T.%N"
else date +"%T.%N"
fi
./cprt_test -t 5 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test " tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok
if which gdate >/dev/null 2>&1; then gdate +"%T.%N"
else date +"%T.%N"
fi

if which gdate >/dev/null 2>&1; then gdate +"%T.%N"
else date +"%T.%N"
fi
./cprt_test -t 6 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test " tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok
if which gdate >/dev/null 2>&1; then gdate +"%T.%N"
else date +"%T.%N"
fi

if which gdate >/dev/null 2>&1; then gdate +"%T.%N"
else date +"%T.%N"
fi
./cprt_test -t 66 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test " tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok
if which gdate >/dev/null 2>&1; then gdate +"%T.%N"
else date +"%T.%N"
fi

./cprt_test -t 7 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test |word='abc'|word='xyz'|word='123'" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t 8 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test |^50ms = |cprt_ms_printf" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
egrep "cprt_ms_printf" tst.tmp
ok

./cprt_test -t 81 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test |^50ms = " tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t 82 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
egrep -v "^test |^50ms = " tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

if echo "$OSTYPE" | egrep -i darwin >/dev/null; then :
else :
  echo "For non-Mac, run tst9.sh (requires watching top for affinity)."
fi

# Test getopt.

./cprt_test -t 10 >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
if egrep "^Test=10$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "^test 10|^Test=10$" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t 10 abc >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
if egrep "^Test=10, Argv\[3\]='abc'$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "^test 10|^Test=10, Argv\[3\]='abc'$" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t10 - >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
if egrep "^Test=10, Argv\[2\]='-'$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "^test 10|^Test=10, Argv\[2\]='-'$" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t10 -- -abc >tst.tmp 2>&1
if [ $? -ne 0 ]; then fail; fi
if egrep "^Test=10, Argv\[3\]='-abc'$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "^test 10|^Test=10, Argv\[3\]='-abc'$" tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "^Error, '-t' requires value\.$" tst.tmp >/dev/null; then :; else fail; fi
if egrep "^cprt_optopt='t', Use '-h' for help$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "cprt_optopt='t', Use '-h' for help|Error, '-t' requires value\." tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -: >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "^Error, ':' not a valid option character\.$" tst.tmp >/dev/null; then :; else fail; fi
if egrep "^cprt_optopt=':', Use '-h' for help$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "cprt_optopt=':', Use '-h' for help|Error, ':' not a valid option character\." tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test "- " >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "^Error, argv\[1\] has non-printable character\.$" tst.tmp >/dev/null; then :; else fail; fi
if egrep "^cprt_optopt=' ', Use '-h' for help$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "cprt_optopt=' ', Use '-h' for help|Error, argv\[1\] has non-printable character\." tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -hxyz >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "^Error, '-h' must have no value\.$" tst.tmp >/dev/null; then :; else fail; fi
if egrep "^cprt_optopt='h', Use '-h' for help$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "cprt_optopt='h', Use '-h' for help|Error, '-h' must have no value\." tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok

./cprt_test -t 10 -x >tst.tmp 2>&1
if [ $? -eq 0 ]; then fail; fi
if egrep "^Error, 'x' not a defined option.$" tst.tmp >/dev/null; then :; else fail; fi
if egrep "^cprt_optopt='x', Use '-h' for help$" tst.tmp >/dev/null; then :; else fail; fi
egrep -v "cprt_optopt='x', Use '-h' for help|Error, 'x' not a defined option." tst.tmp >tst.tmp1
if [ -s tst.tmp1 ]; then fail; fi
ok
