#!/bin/sh

# install dependencies
echo "yes" | sudo add-apt-repository ppa:kalakris/cmake
sudo apt-get update -qq
sudo apt-get install -y cmake
sudo apt-get install -y check
sudo apt-get install -y valgrind

# build and test libptab
make libptab test valgrind
