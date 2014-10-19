libptab
=======
[![Build Status](https://travis-ci.org/andybug/libptab.svg?branch=master)](https://travis-ci.org/andybug/libptab) [![Coverage Status](https://img.shields.io/coveralls/andybug/libptab.svg)](https://coveralls.io/r/andybug/libptab?branch=master)

_libptab_ (lib pretty table) is a C library that allows the creation of
human-readable data tables. The user first defines the column titles
and alignment, then adds the row data. Once this is done, _libptab_
will calculate the spacing and apply the formatting to output a table
in this form:

    +---------+-------+
    | City    | Value |
    +---------+-------+
    | Atlanta |     1 |
    | Dallas  |     8 |
    | Chicago |    14 |
    +---------+-------+

# Features
Note: Since _libptab_ is still in alpha, the feature set has not yet
been finalized.

# Documentation
An overview of the API can be found at [doc/API.md](doc/API.md).

# Building
Building _libptab_ is accomplished by CMake. To help out, a Makefile
exists at the top direcory that automates the CMake setup process.
Simply typing `make` from the top directory will create the build
directory, initialize CMake for an out-of-source build, and begin
the build.

Once the build is complete, running `make test` will run the unit
tests for the library.

The _libptab_ shared object and archive libraries will be placed under
`build/lib`, while the test executable is placed under `build/bin`.
