
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include <check.h>
#include <ptab.h>

/* Some helper functions */

static void *test_alloc(size_t size, void *opaque)
{
	(void)opaque;

	return malloc(size);
}

static void test_free(void *ptr, void *opaque)
{
	(void)opaque;

	free(ptr);
}


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
	pa.alloc_func = test_alloc;
	pa.free_func = test_free;
	pa.opaque = NULL;

	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_OK);

	/* check allocator */
	ck_assert(p.allocator.alloc_func == pa.alloc_func);
	ck_assert(p.allocator.free_func == pa.free_func);
	ck_assert(p.allocator.opaque == pa.opaque);

	ptab_free(&p);
}
END_TEST

START_TEST (test_init_no_allocator)
{
	struct ptab p;
	struct ptab_allocator pa;
	int err;

	/* set some non-NULL pointers for the allocators */
	pa.alloc_func = test_alloc;
	pa.free_func = test_free;

	/* 
	 * assign the allocators in the ptable so we can
	 * see if they change during the init() call
	 */
	p.allocator.alloc_func = pa.alloc_func;
	p.allocator.free_func = pa.free_func;

	err = ptab_init(&p, NULL);
	ck_assert_int_eq(err, PTAB_OK);

	/* make sure the allocators changed */
	ck_assert(p.allocator.alloc_func != pa.alloc_func);
	ck_assert(p.allocator.free_func != pa.free_func);

	ptab_free(&p);
}
END_TEST

START_TEST (test_init_null)
{
	struct ptab p;
	struct ptab_allocator pa;
	int err;

	/* set some non-NULL pointers for the allocators */
	pa.alloc_func = test_alloc;
	pa.free_func = test_free;
	pa.opaque = NULL;

	/* non-NULL params and non-NULL allocators are good */
	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_OK);
	ptab_free(&p);

	/* ensure NULL parameter causes error */
	err = ptab_init(NULL, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);

	/* set allocator to NULL and check for error */
	pa.alloc_func = NULL;
	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);

	/* set free to NULL and check for error */
	pa.alloc_func = test_alloc;
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
	tcase_add_test(tc_init, test_init_no_allocator);
	tcase_add_test(tc_init, test_init_null);
	suite_add_tcase(s, tc_init);

	return s;
}


