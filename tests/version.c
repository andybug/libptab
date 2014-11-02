
#include <check.h>
#include <ptab.h>

START_TEST (version_string)
{
	const char *verstr;

	verstr = ptab_version_string();
	ck_assert_str_eq(verstr, PTAB_VERSION_STRING);
}
END_TEST

START_TEST (version)
{
	int major, minor, patch;

	ptab_version(&major, &minor, &patch);
	ck_assert_int_eq(major, PTAB_VERSION_MAJOR);
	ck_assert_int_eq(minor, PTAB_VERSION_MINOR);
	ck_assert_int_eq(patch, PTAB_VERSION_PATCH);
}
END_TEST

TCase *version_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Version");
	tcase_add_test(tc, version_string);
	tcase_add_test(tc, version);

	return tc;
}
