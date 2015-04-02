#!/bin/sh

SCRIPT_PATH="`readlink -f "$0"`"
LIBPTAB_PATH="`dirname $SCRIPT_PATH`/../src/libptab"
PTAB_PATH="`dirname $SCRIPT_PATH`/../src/ptab"

cd $LIBPTAB_PATH
if [ $? -ne 0 ]; then
	exit 1
fi

clang-format -i *.c *.h


cd $PTAB_PATH
if [ $? -ne 0 ]; then
	exit 1
fi

clang-format -i *.cpp *.hpp
