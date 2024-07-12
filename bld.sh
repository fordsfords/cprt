#!/bin/sh

for F in *.md; do :
  if egrep "<!-- mdtoc-start -->" $F >/dev/null; then :
    # Update doc table of contents (see https://github.com/fordsfords/mdtoc).
    if which mdtoc.pl >/dev/null; then mdtoc.pl -b "" $F;
    elif [ -x ../mdtoc/mdtoc.pl ]; then ../mdtoc/mdtoc.pl -b "" $F;
    else echo "FYI: mdtoc.pl not found; Skipping doc build"; echo ""; fi
  fi
done

OPTS=""
if echo "$OSTYPE" | egrep -i linux >/dev/null; then :
  OPTS="-pthread -l pthread"
fi

echo "Building code"

gcc -Wall -o cprt_test $OPTS cprt.c cprt_test.c
if [ $? -ne 0 ]; then exit 1; fi

echo "Success"
