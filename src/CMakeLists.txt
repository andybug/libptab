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
	ptab_objects OBJECT
	# --- sources ---
	alloc.c
	error.c
	version.c
)

# need to compile as position independent
SET_TARGET_PROPERTIES(
	ptab_objects PROPERTIES
	POSITION_INDEPENDENT_CODE ON
)


# --- build so ---

ADD_LIBRARY(
	ptab SHARED
	$<TARGET_OBJECTS:ptab_objects>
)

INSTALL(
	TARGETS ptab
	LIBRARY DESTINATION lib
)


# --- build static library ---

ADD_LIBRARY(
	ptab-static STATIC
	$<TARGET_OBJECTS:ptab_objects>
)

INSTALL(
	TARGETS ptab-static
	ARCHIVE DESTINATION lib
)
