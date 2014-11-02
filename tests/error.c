
#include <check.h>
#include <ptab.h>

START_TEST (strerror_ok)
{
	const char *desc;

	desc = ptab_strerror(PTAB_OK);
	ck_assert(desc != NULL);
}
END_TEST

START_TEST (strerror_positive)
{
	const char *desc;

	desc = ptab_strerror(5);
	ck_assert(desc == NULL);
}
END_TEST

START_TEST (strerror_noterror)
{
	const char *desc;

	desc = ptab_strerror(-300);
	ck_assert(desc == NULL);
}
END_TEST

TCase *error_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Error");
	tcase_add_test(tc, strerror_ok);
	tcase_add_test(tc, strerror_positive);
	tcase_add_test(tc, strerror_noterror);

	return tc;
}
