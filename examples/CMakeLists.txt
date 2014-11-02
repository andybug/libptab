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

IF("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
	TARGET_LINK_LIBRARIES(ptab-example gcov)
ENDIF()

INSTALL(
	TARGETS ptab-example
	RUNTIME DESTINATION bin
)
