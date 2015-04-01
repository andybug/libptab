
#include <check.h>
#include <ptab.h>

START_TEST (version)
{
	const char *verstr;

	verstr = ptab_version();
	ck_assert_str_eq(verstr, PTAB_VERSION);
}
END_TEST

TCase *version_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Version");
	tcase_add_test(tc, version);

	return tc;
}
