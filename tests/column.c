
#include <check.h>
#include <ptab.h>

#include "../src/internal.h"

static ptab_t *p;
static int err;

static void fixture_init(void)
{
	p = ptab_init(NULL);
}

static void fixture_free(void)
{
	ptab_free(p);
}

START_TEST (column_default)
{
	err = ptab_column(p, "Name", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (column_null)
{
	err = ptab_column(NULL, "Column", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_column(p, NULL, PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (column_type)
{
	err = ptab_column(p, "Column", PTAB_STRING | PTAB_FLOAT | PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ETYPEFLAGS);

	err = ptab_column(p, "Column", PTAB_STRING | PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_ETYPEFLAGS);

	err = ptab_column(p, "Column", PTAB_STRING | PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ETYPEFLAGS);

	err = ptab_column(p, "Column", PTAB_FLOAT | PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ETYPEFLAGS);

	err = ptab_column(p, "Column", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(p, "Column", PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(p, "Column", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(p, "Column", 0);
	ck_assert_int_eq(err, PTAB_ETYPEFLAGS);
}
END_TEST

#if 0
START_TEST (column_align_flags)
{
	err = ptab_column(p, "Column", PTAB_STRING | PTAB_ALIGN_LEFT | PTAB_ALIGN_RIGHT);
	ck_assert_int_eq(err, PTAB_EALIGNFLAGS);

	err = ptab_column(p, "Column", PTAB_STRING | PTAB_ALIGN_LEFT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(p, "Column", PTAB_STRING | PTAB_ALIGN_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(p, "Column", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST
#endif

START_TEST (column_many)
{
	int i;

	for (i = 0; i < 1000; i++) {
		err = ptab_column(p, "Column", PTAB_INTEGER);
		ck_assert_int_eq(err, PTAB_OK);
	}
}
END_TEST

START_TEST (column_nomem)
{
	mem_disable(p);

	err = ptab_column(p, "Column", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENOMEM);

	mem_enable(p);
}
END_TEST

START_TEST (column_rowsdefined)
{
	/* FIXME write test that checks for PTAB_EROWS */
}
END_TEST

TCase *column_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Column");
	tcase_add_checked_fixture(tc, fixture_init, fixture_free);
	tcase_add_test(tc, column_default);
	tcase_add_test(tc, column_null);
	tcase_add_test(tc, column_type);
	/* tcase_add_test(tc, column_align_flags); */
	tcase_add_test(tc, column_many);
	tcase_add_test(tc, column_nomem);
	tcase_add_test(tc, column_rowsdefined);

	return tc;
}
