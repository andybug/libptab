
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include <check.h>
#include <ptab.h>

/* Version test case */

START_TEST (test_version_string)
{
	const char *verstr;

	verstr = ptab_version_string();
	ck_assert_str_eq(verstr, PTAB_VERSION_STRING);
}
END_TEST

START_TEST (test_version)
{
	int major, minor, patch;

	ptab_version(&major, &minor, &patch);
	ck_assert_int_eq(major, PTAB_VERSION_MAJOR);
	ck_assert_int_eq(minor, PTAB_VERSION_MINOR);
	ck_assert_int_eq(patch, PTAB_VERSION_PATCH);
}
END_TEST


/* Init test case */

START_TEST (test_init)
{
	struct ptab p;
	struct ptab_allocator pa;
	int err;

	/* set some non-NULL pointers for the allocators */
	pa.alloc_func = (ptab_alloc_func) 0xdeadbeef;
	pa.free_func = (ptab_free_func) 0xdeadbeef;
	pa.opaque = NULL;

	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_OK);

	/* check internal state */
	ck_assert_int_eq(p.num_columns, 0);
	ck_assert_int_eq(p.num_rows, 0);
	ck_assert(p.columns == NULL);
	ck_assert(p.rows == NULL);

	ck_assert(p.allocator.alloc_func == pa.alloc_func);
	ck_assert(p.allocator.free_func == pa.free_func);
	ck_assert(p.allocator.opaque == pa.opaque);

	ck_assert_int_eq(p.allocator_stats.total, 0);
	ck_assert_int_eq(p.allocator_stats.high, 0);
	ck_assert_int_eq(p.allocator_stats.current, 0);
	ck_assert_int_eq(p.allocator_stats.allocations, 0);
	ck_assert_int_eq(p.allocator_stats.frees, 0);
}
END_TEST

START_TEST (test_init_null)
{
	struct ptab p;
	struct ptab_allocator pa;
	int err;

	/* set some non-NULL pointers for the allocators */
	pa.alloc_func = (ptab_alloc_func) 0xdeadbeef;
	pa.free_func = (ptab_free_func) 0xdeadbeef;
	pa.opaque = NULL;

	/* ensure NULL parameter causes error */
	err = ptab_init(NULL, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);

	/* ensure NULL parameter causes error */
	err = ptab_init(&p, NULL);
	ck_assert_int_eq(err, PTAB_ENULL);

	/* non-NULL params and non-NULL allocators are good */
	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_OK);

	/* set allocator to NULL and check for error */
	pa.alloc_func = NULL;
	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);

	/* set free to NULL and check for error */
	pa.alloc_func = (ptab_alloc_func) 0xdeadbeef;
	pa.free_func = NULL;
	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST


/* Suite definition */

Suite *get_libptab_suite(void)
{
	Suite *s;
	TCase *tc_version;
	TCase *tc_init;

	s = suite_create("libptab Test Suite");

	/* create test cases */
	tc_version = tcase_create("Version");
	tcase_add_test(tc_version, test_version_string);
	tcase_add_test(tc_version, test_version);
	suite_add_tcase(s, tc_version);

	tc_init = tcase_create("Init");
	tcase_add_test(tc_init, test_init);
	tcase_add_test(tc_init, test_init_null);
	suite_add_tcase(s, tc_init);

	return s;
}


