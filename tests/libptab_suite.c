
#include <stdint.h>
#include <limits.h>

#include <check.h>
#include <ptab.h>

/* Version test case */

START_TEST (test_version_string)
{
	const char *verstr;

	verstr = ptab_version_string();
	ck_assert_str_eq(verstr, PTAB_VERSION_STRING);
}
END_TEST

START_TEST (test_version)
{
	int major, minor, patch;

	ptab_version(&major, &minor, &patch);
	ck_assert_int_eq(major, PTAB_VERSION_MAJOR);
	ck_assert_int_eq(minor, PTAB_VERSION_MINOR);
	ck_assert_int_eq(patch, PTAB_VERSION_PATCH);
}
END_TEST


/* Suite definition */

Suite *get_libptab_suite(void)
{
	Suite *s;
	TCase *tc_version;

	s = suite_create("libptab Test Suite");

	/* create test cases */
	tc_version = tcase_create("Version");
	tcase_add_test(tc_version, test_version_string);
	tcase_add_test(tc_version, test_version);
	suite_add_tcase(s, tc_version);

	return s;
}


