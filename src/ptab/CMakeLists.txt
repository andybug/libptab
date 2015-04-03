# --- dependency checks ---

# none right now...
include(CheckIncludeFile)
include(CheckSymbolExists)

check_include_file(stdlib.h HAVE_STDLIB_H)
check_symbol_exists(EXIT_SUCCESS stdlib.h EXIT_SUCCESS_DEFINED)


# --- build tool ---

add_executable(
	ptab-tool
	main.c
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
