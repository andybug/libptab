
#include <check.h>
#include <ptab.h>
#include "../src/internal.h"

static ptab p;
static int err;

static void fixture_init(void)
{
	memset(&p, 0, sizeof(ptab));
	ptab_init(&p, NULL);

	ptab_column(&p, "StringColumn", PTAB_STRING);
	ptab_column(&p, "IntegerColumn", PTAB_INTEGER);
	ptab_column(&p, "FloatColumn", PTAB_FLOAT);
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

START_TEST (begin_row_default)
{
	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (begin_row_nocolumns)
{
	ptab p;

	ptab_init(&p, NULL);

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_ENOCOLUMNS);

	ptab_free(&p);
}
END_TEST

START_TEST (begin_row_nomem)
{
	size_t alloc_size;

	p.allocator.alloc_func = alloc_null;

	/* ugly hack */
	alloc_size = p.internal->alloc_tree->avail;
	ptab_alloc(&p, alloc_size);

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST

START_TEST (begin_row_notfinished)
{
	/* TODO: probably need to implement this after end_row */
}
END_TEST

START_TEST (begin_row_null)
{
	err = ptab_begin_row(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (begin_row_init)
{
	ptab p;

	p.internal = NULL;

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_EINIT);
}
END_TEST

START_TEST (begin_row_alreadybegan)
{
	ptab_begin_row(&p);

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_EROWBEGAN);
}
END_TEST

TCase *row_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Rows");
	tcase_add_checked_fixture(tc, fixture_init, fixture_free);
	tcase_add_test(tc, begin_row_default);
	tcase_add_test(tc, begin_row_nocolumns);
	tcase_add_test(tc, begin_row_nomem);
	tcase_add_test(tc, begin_row_notfinished);
	tcase_add_test(tc, begin_row_null);
	tcase_add_test(tc, begin_row_init);
	tcase_add_test(tc, begin_row_alreadybegan);

	return tc;
}
