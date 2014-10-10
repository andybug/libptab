PROJECT(libptables)

# create object library so we don't have
# to recompile for .a and .so
ADD_LIBRARY(
	ptables_objects OBJECT
	# --- sources ---
	ptables.c
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
