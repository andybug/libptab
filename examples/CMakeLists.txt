PROJECT(libptab)

ADD_EXECUTABLE(
	ptab-example
	# --- sources ---
	example.c
)

TARGET_LINK_LIBRARIES(
	ptab-example
	# --- static libraries ---
	ptab-static
)

INSTALL(
	TARGETS ptab-example
	RUNTIME DESTINATION bin
)
