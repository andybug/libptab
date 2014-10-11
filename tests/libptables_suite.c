
#include <check.h>
#include <ptables.h>

/* Version test case */

START_TEST (test_version)
{
	const char *verstr;

	verstr = ptables_version();
	ck_assert_str_eq(verstr, PTABLES_VERSION_STRING);
}
END_TEST

/* Suite definition */

Suite * get_libptables_suite(void)
{
	Suite *s;
	TCase *tc_version;

	s = suite_create("libptables Test Suite");

	/* create test cases */
	tc_version = tcase_create("Check Version");
	tcase_add_test(tc_version, test_version);
	suite_add_tcase(s, tc_version);

	return s;
}


