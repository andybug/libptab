PROJECT(libptab)


ADD_DEFINITIONS('-D_POSIX_C_SOURCE=200809L')

ADD_EXECUTABLE(
	ptab-test
	# --- sources ---
	main.c
	version.c
)

TARGET_LINK_LIBRARIES(
	ptab-test
	# --- static libraries ---
	ptab-static
	check
)

IF("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
	TARGET_LINK_LIBRARIES(ptab-test gcov)
ENDIF()

INSTALL(
	TARGETS ptab-test
	RUNTIME DESTINATION bin
)

ADD_TEST(
	NAME libptab-unit-tests
	COMMAND ptab-test
)
