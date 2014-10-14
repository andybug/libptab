#!/bin/sh

if [ -d $PWD/src -a -f $PWD/include/ptab.h ]; then
	SEARCHPATHS="$PWD/src $PWD/include"
	CONFIG="$PWD/scripts/.uncrustify.cfg"
elif [ -d $PWD/../src -a -f $PWD/../include/ptab.h ]; then
	SEARCHPATHS="$PWD/../src $PWD/../include"
	CONFIG="$PWD/.uncrustify.cfg"
else
	echo "uncrustify.sh needs to be run from the top directory"
	echo "or the scripts directory"
	exit 1
fi

find $SEARCHPATHS -regex '^.*\.[hc]$' | xargs uncrustify -c $CONFIG --no-backup

exit 0
