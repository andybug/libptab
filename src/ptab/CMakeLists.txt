# --- compiler settings ---

add_definitions(-D_POSIX_SOURCE)


# --- dependency checks ---

include(CheckIncludeFile)

check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(assert.h HAVE_ASSERT_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(stdio.h HAVE_STDIO_H)


# --- build tool ---

add_executable(
	ptab-tool
	main.c
	ptab.c
)

if("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
	target_link_libraries(
		ptab-tool
		ptab-library
		gcov
	)
else()
	target_link_libraries(
		ptab-tool
		ptab-library
	)
endif()

set_target_properties(
	ptab-tool PROPERTIES
	OUTPUT_NAME ptab
)

install(
	TARGETS ptab-tool
	RUNTIME DESTINATION bin
)
