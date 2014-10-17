#!/bin/sh

set -e

# install dependencies
#echo "yes" | sudo add-apt-repository ppa:kalakris/cmake >/dev/null
#sudo apt-get update -qq >/dev/null
#sudo apt-get install -y cmake check valgrind >/dev/null

# build libptab and tests
make

# run the tests
make test

# run valgrind
echo '\033[36mRunning valgrind...\033[39;49m'
./scripts/valgrind.sh

# we're done!
echo
echo '\033[32;1mBUILD COMPLETE\033[39;49m'
exit 0
