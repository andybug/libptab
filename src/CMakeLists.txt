PROJECT(libptab)

# don't export symbols unless specified
add_definitions(-fvisibility=hidden)

# add some definitions if gcov is enabled
if("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
	add_definitions(-fprofile-arcs -ftest-coverage)
endif()

# create object library so we don't have
# to recompile for .a and .so
ADD_LIBRARY(
	ptab-objects OBJECT
	# --- sources ---
	alloc.c
	column.c
	error.c
	output.c
	row.c
	version.c
)

# need to compile as position independent
SET_TARGET_PROPERTIES(
	ptab-objects PROPERTIES
	POSITION_INDEPENDENT_CODE ON
)


# --- build so ---

ADD_LIBRARY(
	ptab-so SHARED
	$<TARGET_OBJECTS:ptab-objects>
)

SET_TARGET_PROPERTIES(
	ptab-so PROPERTIES
	OUTPUT_NAME ptab
)

INSTALL(
	TARGETS ptab-so
	LIBRARY DESTINATION lib
)


# --- build static library ---

ADD_LIBRARY(
	ptab-static STATIC
	$<TARGET_OBJECTS:ptab-objects>
)

SET_TARGET_PROPERTIES(
	ptab-static PROPERTIES
	OUTPUT_NAME ptab
)

INSTALL(
	TARGETS ptab-static
	ARCHIVE DESTINATION lib
)


# --- build tool ---

add_definitions(-D_POSIX_C_SOURCE=1)

add_executable(
	ptab-tool
	tool.c
)

if("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
	target_link_libraries(
		ptab-tool
		ptab-static
		gcov
	)
else()
	target_link_libraries(
		ptab-tool
		ptab-static
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
