#!/bin/sh

valgrind --leak-check=full --error-exitcode=1 ./build/bin/ptab-test
