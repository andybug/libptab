
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


/* Buffer test case */

static struct ptable buffer_p;

void fixture_buffer_setup(void)
{
	ptable_init(&buffer_p, PTABLES_USE_BUFFER);
}

void fixture_buffer_teardown(void)
{
	/* do nothing */
}

START_TEST (test_buffer_set)
{
	char buf[32];
	int err;

	err = ptable_buffer_set(&buffer_p, buf, 32);
	ck_assert_int_eq(err, PTABLES_OK);

	ck_assert(buffer_p.buffer.buf == buf);
	ck_assert_int_eq(buffer_p.buffer.size, 32);
	ck_assert_int_eq(buffer_p.buffer.used, 0);
	ck_assert_int_eq(buffer_p.buffer.avail, 32);
}
END_TEST

START_TEST (test_buffer_null)
{
	int err;

	err = ptable_buffer_set(&buffer_p, NULL, 128);
	ck_assert_int_eq(err, PTABLES_ERR_NULL);
}
END_TEST

START_TEST (test_buffer_zero_allocation)
{
	char buf[32];
	void *v;
	int err;

	err = ptable_buffer_set(&buffer_p, buf, 32);
	ck_assert_int_eq(err, PTABLES_OK);

	v = buffer_p.alloc_func(&buffer_p, 0, NULL);

	ck_assert(v == NULL);
	ck_assert_int_eq(buffer_p.buffer.size, 32);
	ck_assert_int_eq(buffer_p.buffer.used, 0);
	ck_assert_int_eq(buffer_p.buffer.avail, 32);
}
END_TEST

START_TEST (test_buffer_allocations)
{
	char buf[32];
	char *c, *c_expect;
	int err;
	unsigned int i;

	err = ptable_buffer_set(&buffer_p, buf, 32);
	ck_assert_int_eq(err, PTABLES_OK);

	/*
	 * make many allocations to make sure it keeps track
	 * of them all
	 */
	c_expect = buf;
	for (i = 0; i < 7; i++) {
		c = buffer_p.alloc_func(&buffer_p, 4, NULL);
		ck_assert(c != NULL);
		ck_assert(c == c_expect);
		ck_assert_int_eq(buffer_p.buffer.used, (i+1) * 4);
		c_expect += 4;
	}

	ck_assert_int_eq(buffer_p.buffer.size, 32);
	ck_assert_int_eq(buffer_p.buffer.used, 28);
	ck_assert_int_eq(buffer_p.buffer.avail, 4);

	/* try an allocation that's too big */
	c = buffer_p.alloc_func(&buffer_p, 5, NULL);

	ck_assert(c == NULL);
	ck_assert_int_eq(buffer_p.buffer.size, 32);
	ck_assert_int_eq(buffer_p.buffer.used, 28);
	ck_assert_int_eq(buffer_p.buffer.avail, 4);

	/* try an allocation that's just right */
	c = buffer_p.alloc_func(&buffer_p, 4, NULL);

	ck_assert(c != NULL);
	ck_assert_int_eq(buffer_p.buffer.size, 32);
	ck_assert_int_eq(buffer_p.buffer.used, 32);
	ck_assert_int_eq(buffer_p.buffer.avail, 0);

	/* try another allocation just in case */
	c = buffer_p.alloc_func(&buffer_p, 1, NULL);

	ck_assert(c == NULL);
	ck_assert_int_eq(buffer_p.buffer.size, 32);
	ck_assert_int_eq(buffer_p.buffer.used, 32);
	ck_assert_int_eq(buffer_p.buffer.avail, 0);
}
END_TEST


/* Suite definition */

Suite *get_libptables_suite(void)
{
	Suite *s;
	TCase *tc_version;
	TCase *tc_init;
	TCase *tc_buffer;

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

	tc_buffer = tcase_create("Buffer");
	tcase_add_checked_fixture(tc_buffer,
		fixture_buffer_setup, fixture_buffer_teardown);
	tcase_add_test(tc_buffer, test_buffer_set);
	tcase_add_test(tc_buffer, test_buffer_null);
	tcase_add_test(tc_buffer, test_buffer_zero_allocation);
	tcase_add_test(tc_buffer, test_buffer_allocations);
	suite_add_tcase(s, tc_buffer);

	return s;
}


