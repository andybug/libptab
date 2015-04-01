#!/bin/sh

SCRIPT_PATH="`readlink -f "$0"`"
SRC_PATH="`dirname $SCRIPT_PATH`/../src"

cd $SRC_PATH
if [ $? -ne 0 ]; then
	exit 1
fi

clang-format -i *.c *.h
