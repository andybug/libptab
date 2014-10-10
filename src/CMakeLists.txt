PROJECT(libptables)

# create object library so we don't have
# to recompile for .a and .so
ADD_LIBRARY(
	ptables_objects OBJECT
	# --- sources ---
	ptables.c
)

# need to compile as position independent
SET_TARGET_PROPERTIES(
	ptables_objects PROPERTIES
	POSITION_INDEPENDENT_CODE ON
)


# --- build so ---

ADD_LIBRARY(
	ptables SHARED
	$<TARGET_OBJECTS:ptables_objects>
)

INSTALL(
	TARGETS ptables
	LIBRARY DESTINATION lib
)


# --- build static library ---

ADD_LIBRARY(
	ptables-static STATIC
	$<TARGET_OBJECTS:ptables_objects>
)

INSTALL(
	TARGETS ptables-static
	ARCHIVE DESTINATION lib
)
