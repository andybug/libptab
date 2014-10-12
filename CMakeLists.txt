cmake_minimum_required(VERSION 2.8.9)
project(libptables C)

# --- general project settings ---

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

ENABLE_TESTING()


# --- compiler settings ---

add_definitions(-Wall -Wextra -pedantic -std=c99)
include_directories(include)


# --- dependency checks ---

include(CheckIncludeFiles)
include(CheckSymbolExists)

check_include_files(stddef.h HAVE_STDDEF_H)
check_include_files(stdbool.h HAVE_STDBOOL_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(limits.h HAVE_LIMITS_H)


# --- descend into source ---

add_subdirectory(src)
add_subdirectory(tests)
