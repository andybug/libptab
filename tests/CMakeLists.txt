PROJECT(libptables)


ADD_EXECUTABLE(
	ptables-test
	# --- sources ---
	tests.c
)

TARGET_LINK_LIBRARIES(
	ptables-test
	# --- static libraries ---
	ptables-static
)

INSTALL(
	TARGETS ptables-test
	RUNTIME DESTINATION bin
)

ADD_TEST(
	NAME libptables-unit-tests
	COMMAND ptables-test
)
