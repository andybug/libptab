
#include <check.h>
#include <ptab.h>

static ptab p;
static int err;

static void fixture_init(void)
{
	memset(&p, 0, sizeof(ptab));
	ptab_init(&p, NULL);
}

static void fixture_free(void)
{
	ptab_free(&p);
}

START_TEST (column_default)
{
}
END_TEST

START_TEST (column_null)
{
}
END_TEST

START_TEST (column_nomem)
{
}
END_TEST

START_TEST (column_flags)
{
}
END_TEST

START_TEST (column_many)
{
}
END_TEST

START_TEST (column_noinit)
{
}
END_TEST

TCase *column_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Column");
	tcase_add_checked_fixture(tc, fixture_init, fixture_free);
	tcase_add_test(tc, column_default);
	tcase_add_test(tc, column_null);
	tcase_add_test(tc, column_nomem);
	tcase_add_test(tc, column_flags);
	tcase_add_test(tc, column_many);
	tcase_add_test(tc, column_noinit);

	return tc;
}
