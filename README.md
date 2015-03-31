ptab
=======
[![Build Status](https://travis-ci.org/andybug/ptab.svg?branch=master)](https://travis-ci.org/andybug/ptab) [![Coverage Status](https://img.shields.io/coveralls/andybug/ptab.svg)](https://coveralls.io/r/andybug/ptab?branch=master)

_ptab_ (pretty table) is a tool that allows the easy creation of human-
readable data tables. The row and column data is parsed from stdin
and the beautified table is printed to stdout. The back end work is
handled by _libptab_, a very simple library and API that can be used
in external programs. The API can be viewed at
[include/ptab.h](include/ptab.h).

# Sample
Contents of _sample.csv_:

  School,City,State,Stadium,Size
  Alabama,Tuscaloosa,AL,Bryant Denny,101821
  Arkansas,Fayetteville,AR,Razorback Stadium,72000
  Auburn,Auburn,AL,Jordan Hare,87451
  LSU,Baton Rouge,LA,Tiger Stadium,102321
  Mississippi State,Starkville,MS,Davis Wade,61337
  Ole Miss,Oxford,MS,Vaught-Hemingway,60580
  Texas A&M,College Station,TX,Kyle Field,106511

Command, setting the delimiter to ',' for the CSV file:

  ptab -d ',' < sample.csv

Output:

  +-------------------+-----------------+-------+-------------------+--------+
  | School            | City            | State | Stadium           | Size   |
  +-------------------+-----------------+-------+-------------------+--------+
  | Alabama           | Tuscaloosa      | AL    | Bryant Denny      | 101821 |
  | Arkansas          | Fayetteville    | AR    | Razorback Stadium |  72000 |
  | Auburn            | Auburn          | AL    | Jordan Hare       |  87451 |
  | LSU               | Baton Rouge     | LA    | Tiger Stadium     | 102321 |
  | Mississippi State | Starkville      | MS    | Davis Wade        |  61337 |
  | Ole Miss          | Oxford          | MS    | Vaught-Hemingway  |  60580 |
  | Texas A&M         | College Station | TX    | Kyle Field        | 106511 |
  +-------------------+-----------------+-------+-------------------+--------+

# Building
## Dependencies
_ptab_ requires the following packages be installed for building:
 1. **cmake** - build tool
 2. **check** - unit test framework (optional)
 3. **tclap** - command line processing library (headers only)

## Build
_ptab_ is a typical cmake project:

  mkdir build
  cd build
  cmake ..
  make
  make test

And optionally:
  make install
