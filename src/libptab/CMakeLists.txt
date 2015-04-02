# --- dependency checks ---

include(CheckIncludeFile)
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


# don't export symbols unless specified
add_definitions(-fvisibility=hidden)

# add some definitions if gcov is enabled
if("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
	add_definitions(-fprofile-arcs -ftest-coverage)
endif()


# --- build library ---

add_library(
	ptab-library STATIC
	# --- sources ---
	internal.h
	column.c
	error.c
	output.c
	mem.c
	row.c
	version.c
)

set_target_properties(
	ptab-library PROPERTIES
	OUTPUT_NAME ptab
	POSITION_INDEPENDENT_CODE ON
)

install(
	TARGETS ptab-library
	ARCHIVE DESTINATION lib
)

install(FILES ${CMAKE_SOURCE_DIR}/include/ptab.h
	DESTINATION include
)
