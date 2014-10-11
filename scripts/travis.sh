#!/bin/sh

# install dependencies
echo "yes" | sudo add-apt-repository ppa:kalakris/cmake
sudo apt-get update -qq
sudo apt-get install -y cmake
sudo apt-get install -y check

# build libptables
make
