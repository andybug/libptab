
#include <check.h>
#include <ptab.h>

#include "../src/libptab/internal.h"

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
	err = ptab_column(p, "Column", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(p, "Column", PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(p, "Column", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(p, "Column", 0);
	ck_assert_int_eq(err, PTAB_ETYPE);
}
END_TEST

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
	ptab__mem_disable(p);

	err = ptab_column(p, "Column", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_EMEM);

	ptab__mem_enable(p);
}
END_TEST

START_TEST (column_order)
{
	ptab_column(p, "Column", PTAB_STRING);
	ptab_begin_row(p);

	err = ptab_column(p, "Column", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_EORDER);
}
END_TEST

START_TEST (column_align_default)
{
	ptab_column(p, "Column", PTAB_STRING);

	err = ptab_column_align(p, 0, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (column_align_null)
{
	ptab_column(p, "Column", PTAB_STRING);

	err = ptab_column_align(NULL, 0, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (column_align_val)
{
	ptab_column(p, "Column", PTAB_STRING);

	err = ptab_column_align(p, 0, 633634);
	ck_assert_int_eq(err, PTAB_EALIGN);
}
END_TEST

START_TEST (column_align_range)
{
	err = ptab_column_align(p, 0, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_ERANGE);

	ptab_column(p, "Column", PTAB_STRING);

	err = ptab_column_align(p, 1, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_ERANGE);

	err = ptab_column_align(p, 0, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (column_align_multi)
{
	ptab_column(p, "Column", PTAB_STRING);
	ptab_column(p, "Column", PTAB_STRING);
	ptab_column(p, "Column", PTAB_STRING);

	err = ptab_column_align(p, 0, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column_align(p, 1, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column_align(p, 2, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column_align(p, 3, PTAB_RIGHT);
	ck_assert_int_eq(err, PTAB_ERANGE);
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
	tcase_add_test(tc, column_many);
	tcase_add_test(tc, column_nomem);
	tcase_add_test(tc, column_order);
	tcase_add_test(tc, column_align_default);
	tcase_add_test(tc, column_align_null);
	tcase_add_test(tc, column_align_val);
	tcase_add_test(tc, column_align_range);
	tcase_add_test(tc, column_align_multi);

	return tc;
}
