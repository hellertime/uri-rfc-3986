#!/bin/sh

libtool --mode=execute valgrind --tool=memcheck -q --leak-check=full --leak-resolution=high --show-reachable=yes --malloc-fill=0xfc --free-fill=0xfa --num-callers=50 --error-limit=no --read-var-info=yes "$@" 2> .valgrind-log

result="$?"

log_contents="`cat .valgrind-log`"

if [ "x$log_contents" != "x" ]
then
	cat .valgrind-log >&2
	result=1
fi

rm -f .valgrind-log

exit $result
