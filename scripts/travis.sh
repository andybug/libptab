#!/bin/sh

set -e

# install dependencies
echo "yes" | sudo add-apt-repository ppa:kalakris/cmake >/dev/null
sudo apt-get update -qq >/dev/null
sudo apt-get install -y cmake check valgrind >/dev/null

# setup coveralls if building with GCC
if [ $CC = "gcc" ]; then
	sudo pip install cpp-coveralls >/dev/null
	export PTAB_ENV_GCOV=1
	export PTAB_ENV_COVERALLS=1
fi

# build libptab and tests
make

# run the tests
make test

# upload coverage to coveralls
if [ "$PTAB_ENV_COVERALLS" = "1" ]; then
	echo '\033[36mUploading to coveralls.io...\033[39;49m'
	coveralls --exclude include --exclude tests --exclude build/CMakeFiles
fi

# run valgrind
echo '\033[36mRunning valgrind...\033[39;49m'
./scripts/valgrind.sh

# we're done!
echo
echo '\033[32;1mBUILD COMPLETE\033[39;49m'
exit 0
