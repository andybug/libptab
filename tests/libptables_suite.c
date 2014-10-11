
#include <stdint.h>
#include <limits.h>

#include <check.h>
#include <ptables.h>

/* Version test case */

START_TEST (test_version)
{
	const char *verstr;

	verstr = ptables_version();
	ck_assert_str_eq(verstr, PTABLES_VERSION_STRING);
}
END_TEST


/* Init test case */

START_TEST (test_init)
{
	struct ptable p;
	int err;
	ptable_alloc_func alloc_func = (ptable_alloc_func) 0xdeadbeef;
	ptable_free_func free_func = (ptable_free_func) 0xdeadbeef;

	p.columns = INT_MAX;
	p.rows = INT_MAX;
	p.alloc_total = SIZE_MAX;
	p.alloc_func = alloc_func;
	p.free_func = free_func;
	p.opaque = (void*) 0xdeadbeef;

	err = ptable_init(&p, 0);
	ck_assert_int_eq(err, PTABLES_OK);

	ck_assert_int_eq(p.columns, 0);
	ck_assert_int_eq(p.rows, 0);

	ck_assert_int_eq(p.alloc_total, 0);
	ck_assert(p.alloc_func != alloc_func);
	ck_assert(p.alloc_func != NULL);
	ck_assert(p.free_func != free_func);
	ck_assert(p.free_func != NULL);
	ck_assert(p.opaque == NULL);
}
END_TEST

START_TEST (test_init_allocator)
{
	struct ptable p;
	int err;
	ptable_alloc_func alloc_func = (ptable_alloc_func) 0xdeadbeef;
	ptable_free_func free_func = (ptable_free_func) 0xdeadbeef;

	p.alloc_total = SIZE_MAX;
	p.alloc_func = alloc_func;
	p.free_func = free_func;
	p.opaque = (void*) 0xdeadbeef;

	err = ptable_init(&p, PTABLES_USE_ALLOCATOR);
	ck_assert_int_eq(err, PTABLES_OK);

	ck_assert_int_eq(p.alloc_total, 0);
	ck_assert(p.alloc_func != alloc_func);
	ck_assert(p.alloc_func != NULL);
	ck_assert(p.free_func != free_func);
	ck_assert(p.free_func != NULL);
	ck_assert(p.opaque == NULL);
}
END_TEST

START_TEST (test_init_buffer)
{
	struct ptable p;
	int err;
	ptable_alloc_func alloc_func = (ptable_alloc_func) 0xdeadbeef;
	ptable_free_func free_func = (ptable_free_func) 0xdeadbeef;

	p.buffer.buf = (void*) 0xdeadbeef;
	p.buffer.size = SIZE_MAX;
	p.buffer.used = SIZE_MAX;
	p.buffer.avail = SIZE_MAX;

	p.alloc_func = alloc_func;
	p.free_func = free_func;

	err = ptable_init(&p, PTABLES_USE_BUFFER);
	ck_assert_int_eq(err, PTABLES_OK);

	ck_assert(p.buffer.buf == NULL);
	ck_assert_int_eq(p.buffer.size, 0);
	ck_assert_int_eq(p.buffer.used, 0);
	ck_assert_int_eq(p.buffer.avail, 0);

	ck_assert(p.alloc_func != alloc_func);
	ck_assert(p.alloc_func != NULL);
	ck_assert(p.free_func != free_func);
	ck_assert(p.free_func != NULL);
}
END_TEST

START_TEST (test_init_flags)
{
	struct ptable p;
	int err;

	err = ptable_init(&p, PTABLES_USE_BUFFER | PTABLES_USE_ALLOCATOR);
	ck_assert_int_eq(err, PTABLES_ERR_ONE_ALLOCATOR);
}
END_TEST

START_TEST (test_init_diff_allocators)
{
	struct ptable p1, p2;
	int err;
	ptable_alloc_func alloc_func = (ptable_alloc_func) 0xdeadbeef;
	ptable_free_func free_func = (ptable_free_func) 0xdeadbeef;

	p1.alloc_func = alloc_func;
	p1.free_func = free_func;
	p2.alloc_func = alloc_func;
	p2.free_func = free_func;

	err = ptable_init(&p1, PTABLES_USE_ALLOCATOR);
	ck_assert_int_eq(err, PTABLES_OK);

	err = ptable_init(&p2, PTABLES_USE_BUFFER);
	ck_assert_int_eq(err, PTABLES_OK);

	ck_assert(p1.alloc_func != p2.alloc_func);
	ck_assert(p1.free_func != p2.free_func);
}
END_TEST


/* Suite definition */

Suite *get_libptables_suite(void)
{
	Suite *s;
	TCase *tc_version;
	TCase *tc_init;

	s = suite_create("libptables Test Suite");

	/* create test cases */
	tc_version = tcase_create("Version");
	tcase_add_test(tc_version, test_version);
	suite_add_tcase(s, tc_version);

	tc_init = tcase_create("Init");
	tcase_add_test(tc_init, test_init);
	tcase_add_test(tc_init, test_init_allocator);
	tcase_add_test(tc_init, test_init_buffer);
	tcase_add_test(tc_init, test_init_flags);
	tcase_add_test(tc_init, test_init_diff_allocators);
	suite_add_tcase(s, tc_init);

	return s;
}


