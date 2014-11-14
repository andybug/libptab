
#include <check.h>
#include <ptab.h>

static ptab p;
static int err;

START_TEST (free_default)
{
	ptab_init(&p, NULL);

	err = ptab_free(&p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (free_null)
{
	err = ptab_free(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (free_uninitialized)
{
	memset(&p, 0, sizeof(ptab));

	err = ptab_free(&p);
	ck_assert_int_eq(err, PTAB_EINIT);
}
END_TEST

TCase *free_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Free");
	tcase_add_test(tc, free_default);
	tcase_add_test(tc, free_null);
	tcase_add_test(tc, free_uninitialized);

	return tc;
}
