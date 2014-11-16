
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

static void *alloc_null(size_t size, void *opaque)
{
	(void)size;
	(void)opaque;

	return NULL;
}

START_TEST (column_default)
{
	err = ptab_column(&p, "Name", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (column_null)
{
	err = ptab_column(NULL, "Column", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_column(&p, NULL, PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (column_type_flags)
{
	err = ptab_column(&p, "Column", PTAB_STRING | PTAB_FLOAT | PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENUMTYPE);

	err = ptab_column(&p, "Column", PTAB_STRING | PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_ENUMTYPE);

	err = ptab_column(&p, "Column", PTAB_STRING | PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENUMTYPE);

	err = ptab_column(&p, "Column", PTAB_FLOAT | PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENUMTYPE);

	err = ptab_column(&p, "Column", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(&p, "Column", PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(&p, "Column", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(&p, "Column", 0);
	ck_assert_int_eq(err, PTAB_ENUMTYPE);
}
END_TEST

START_TEST (column_align_flags)
{
	err = ptab_column(&p, "Column", PTAB_STRING | PTAB_ALIGN_LEFT | PTAB_ALIGN_RIGHT);
	ck_assert_int_eq(err, PTAB_ENUMALIGN);

	err = ptab_column(&p, "Column", PTAB_STRING | PTAB_ALIGN_LEFT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(&p, "Column", PTAB_STRING | PTAB_ALIGN_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_column(&p, "Column", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (column_noinit)
{
	ptab p;

	p.internal = NULL;

	err = ptab_column(&p, "Column", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_EINIT);
}
END_TEST

START_TEST (column_many)
{
	int i;

	for (i = 0; i < 1000; i++) {
		err = ptab_column(&p, "Column", PTAB_INTEGER);
		ck_assert_int_eq(err, PTAB_OK);
	}
}
END_TEST

START_TEST (column_nomem)
{
	int i;
	int nomem = 0;

	p.allocator.alloc_func = alloc_null;

	/*
	 * need to burn through the remaining space in the already-allocated
	 * block
	 */
	for (i = 0; i < 1000; i++) {
		err = ptab_column(&p, "Column", PTAB_INTEGER);
		if (err == PTAB_ENOMEM) {
			nomem = 1;
			break;
		}
	}

	ck_assert_int_eq(nomem, 1);
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
	tcase_add_test(tc, column_type_flags);
	tcase_add_test(tc, column_align_flags);
	tcase_add_test(tc, column_noinit);
	tcase_add_test(tc, column_many);
	tcase_add_test(tc, column_nomem);
	tcase_add_test(tc, column_rowsdefined);

	return tc;
}
