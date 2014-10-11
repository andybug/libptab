PROJECT(libptables)


ADD_EXECUTABLE(
	ptables-test
	# --- sources ---
	test_main.c
	libptables_suite.c
)

TARGET_LINK_LIBRARIES(
	ptables-test
	# --- static libraries ---
	ptables-static
	check
)

INSTALL(
	TARGETS ptables-test
	RUNTIME DESTINATION bin
)

ADD_TEST(
	NAME libptables-unit-tests
	COMMAND ptables-test
)
