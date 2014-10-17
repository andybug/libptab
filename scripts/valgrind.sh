#!/bin/sh

valgrind --leak-check=full --error-exitcode=1 ./build/bin/ptab-test >/dev/null 2>./build/valgrind.log

if [ $? -eq 1 ]; then
	egrep -A10 '[0-9]+ bytes in [0-9]+ blocks are definitely lost' ./build/valgrind.log
	echo '\033[31;1mFAILED\033[39;49m'
	exit 1
else
	echo '\033[32;1mPASSED\033[39;49m'
	exit 0
fi
