
#include <stdlib.h>

#include <check.h>
#include <ptables.h>

START_TEST (check_version)
{
	const char *verstr;

	verstr = ptables_version();
	ck_assert_str_eq(verstr, PTABLES_VERSION_STRING);
}
END_TEST

Suite * version_suite(void)
{
	Suite *s;
	TCase *tc_check_version;

	s = suite_create("Version");
	tc_check_version = tcase_create("check_version");
	tcase_add_test(tc_check_version, check_version);
	suite_add_tcase(s, tc_check_version);

	return s;
}

int main(void)
{
	Suite *s;
	SRunner *sr;
	int failed;

	s = version_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
