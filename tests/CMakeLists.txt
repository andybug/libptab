# --- dependency checks ---

include(CheckIncludeFile)
include(CheckLibraryExists)

set(PTAB_BUILD_TESTS TRUE)

check_include_file(check.h HAVE_CHECK_H)
check_library_exists(check suite_create "" HAVE_CHECK)

if(NOT HAVE_CHECK_H OR NOT HAVE_CHECK)
	set(PTAB_BUILD_TESTS FALSE)
endif()


# --- if check is installed, build the tests ---

if (PTAB_BUILD_TESTS STREQUAL TRUE)
	enable_testing()

	include_directories(${CMAKE_SOURCE_DIR}/include)

	add_executable(
		ptab-test
		# --- sources ---
		main.c
		version.c
		error.c
		init.c
		free.c
		column.c
		row.c
		output.c
	)
	
	target_link_libraries(
		ptab-test
		# --- static libraries ---
		ptab-library
		check
	)
	
	if("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
		target_link_libraries(ptab-test gcov)
	endif()
	
	add_test(
		NAME libptab-unit-tests
		COMMAND ptab-test
	)

endif(PTAB_BUILD_TESTS STREQUAL TRUE)
