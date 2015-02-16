cmake_minimum_required(VERSION 2.8.9)
project(libptab C CXX)

# --- general project settings ---

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PTAB_BUILD_TESTS TRUE)
ENABLE_TESTING()


# --- compiler settings ---

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
add_definitions(-Wall -Wextra -pedantic)
include_directories(include)


# --- dependency checks ---

include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckLibraryExists)

check_include_files(stddef.h HAVE_STDDEF_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(limits.h HAVE_LIMITS_H)
check_include_files(check.h HAVE_CHECK_H)
check_library_exists(check suite_create "" HAVE_CHECK)

if(NOT HAVE_CHECK_H OR NOT HAVE_CHECK)
	set(PTAB_BUILD_TESTS FALSE)
endif()


# --- descend into source ---

add_subdirectory(src)
add_subdirectory(examples)


# --- build tests if instructed ---

if (PTAB_BUILD_TESTS STREQUAL TRUE)
	add_subdirectory(tests)
endif()
