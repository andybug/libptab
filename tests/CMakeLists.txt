PROJECT(libptab)


ADD_EXECUTABLE(
	ptab-test
	# --- sources ---
	test_main.c
	libptab_suite.c
)

TARGET_LINK_LIBRARIES(
	ptab-test
	# --- static libraries ---
	ptab-static
	check
)

INSTALL(
	TARGETS ptab-test
	RUNTIME DESTINATION bin
)

ADD_TEST(
	NAME libptab-unit-tests
	COMMAND ptab-test
)
