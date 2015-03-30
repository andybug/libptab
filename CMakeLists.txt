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
set(CMKAE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions(-Wall -Wextra -pedantic)
include_directories(include)


# --- dependency checks ---

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckFunctionExists)

check_include_file(assert.h HAVE_ASSERT_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)
check_include_file(stddef.h HAVE_STDDEF_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdio.h HAVE_STDIO_H)
check_function_exists(free HAVE_FUNC_FREE)
check_function_exists(fwrite HAVE_FUNC_FWRITE)
check_function_exists(malloc HAVE_FUNC_MALLOC)
check_function_exists(snprintf HAVE_FUNC_SNPRINTF)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)
check_function_exists(memcpy HAVE_FUNC_MEMCPY)
check_function_exists(memset HAVE_FUNC_MEMSET)
check_function_exists(strcpy HAVE_FUNC_STRCPY)
check_function_exists(strlen HAVE_FUNC_STRLEN)
check_include_file_cxx(cassert HAVE_CASSERT)
check_include_file_cxx(cstdio HAVE_CSTDIO)
check_include_file_cxx(cstdlib HAVE_CSTDLIB)
check_include_file_cxx(iostream HAVE_IOSTREAM)
check_include_file_cxx(sstream HAVE_SSTREAM)
check_include_file_cxx(stdexcept HAVE_STDEXCEPT)
check_include_file_cxx(string HAVE_STRING)
check_include_file_cxx(vector HAVE_VECTOR)

check_include_file_cxx(tclap/CmdLine.h HAV_TCLAP_H)

check_include_file(check.h HAVE_CHECK_H)
check_library_exists(check suite_create "" HAVE_CHECK)

if(NOT HAVE_CHECK_H OR NOT HAVE_CHECK)
	set(PTAB_BUILD_TESTS FALSE)
endif()


# --- descend into source ---

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(src/tool)
add_subdirectory(examples)


# --- build tests if instructed ---

if (PTAB_BUILD_TESTS STREQUAL TRUE)
	add_subdirectory(tests)
endif()
