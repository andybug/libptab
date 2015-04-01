
# don't export symbols unless specified
add_definitions(-fvisibility=hidden)

# add some definitions if gcov is enabled
if("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
	add_definitions(-fprofile-arcs -ftest-coverage)
endif()


# --- build library ---

ADD_LIBRARY(
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

SET_TARGET_PROPERTIES(
	ptab-library PROPERTIES
	OUTPUT_NAME ptab
	POSITION_INDEPENDENT_CODE ON
)

INSTALL(
	TARGETS ptab-library
	ARCHIVE DESTINATION lib
)
