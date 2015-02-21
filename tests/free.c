
#include <check.h>
#include <ptab.h>

static ptab_t *p;
static int err;

START_TEST (free_default)
{
	p = ptab_init(NULL);

	err = ptab_free(p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (free_null)
{
	err = ptab_free(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

TCase *free_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Free");
	tcase_add_test(tc, free_default);
	tcase_add_test(tc, free_null);

	return tc;
}
