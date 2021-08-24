#!/bin/sh
# tst9.sh - script to build and run the cprt test #9 (cpu affinity).

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


echo "If on Linux, run "top -d 1" and then press "1" to get per-CPU usage"
./cprt_test -t 9 >tst.tmp
if [ $? -ne 0 ]; then fail; fi

