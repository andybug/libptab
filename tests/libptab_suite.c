
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


#if 0
/* Init test case */

START_TEST (test_init)
{
	struct ptab p;
	int err;
	ptab_alloc_func alloc_func = (ptab_alloc_func) 0xdeadbeef;
	ptab_free_func free_func = (ptab_free_func) 0xdeadbeef;

	p.flags = INT_MAX;
	p.columns = INT_MAX;
	p.rows = INT_MAX;
	p.alloc_total = SIZE_MAX;
	p.alloc_func = alloc_func;
	p.free_func = free_func;
	p.opaque = (void*) 0xdeadbeef;

	err = ptab_init(&p, 0);
	ck_assert_int_eq(err, PTABS_OK);

	ck_assert_int_eq(p.flags, PTABS_USE_ALLOCATOR);

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
	struct ptab p;
	int err;
	ptab_alloc_func alloc_func = (ptab_alloc_func) 0xdeadbeef;
	ptab_free_func free_func = (ptab_free_func) 0xdeadbeef;

	p.flags = INT_MAX;
	p.alloc_total = SIZE_MAX;
	p.alloc_func = alloc_func;
	p.free_func = free_func;
	p.opaque = (void*) 0xdeadbeef;

	err = ptab_init(&p, PTABS_USE_ALLOCATOR);
	ck_assert_int_eq(err, PTABS_OK);

	ck_assert_int_eq(p.flags, PTABS_USE_ALLOCATOR);
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
	struct ptab p;
	int err;
	ptab_alloc_func alloc_func = (ptab_alloc_func) 0xdeadbeef;
	ptab_free_func free_func = (ptab_free_func) 0xdeadbeef;

	p.flags = INT_MAX;
	p.buffer.buf = (void*) 0xdeadbeef;
	p.buffer.size = SIZE_MAX;
	p.buffer.used = SIZE_MAX;
	p.buffer.avail = SIZE_MAX;

	p.alloc_func = alloc_func;
	p.free_func = free_func;

	err = ptab_init(&p, PTABS_USE_BUFFER);
	ck_assert_int_eq(err, PTABS_OK);

	ck_assert_int_eq(p.flags, PTABS_USE_BUFFER);

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
	struct ptab p;
	int err;

	err = ptab_init(&p, PTABS_USE_BUFFER | PTABS_USE_ALLOCATOR);
	ck_assert_int_eq(err, PTABS_ERR_ONE_ALLOCATOR);
}
END_TEST

START_TEST (test_init_diff_allocators)
{
	struct ptab p1, p2;
	int err;
	ptab_alloc_func alloc_func = (ptab_alloc_func) 0xdeadbeef;
	ptab_free_func free_func = (ptab_free_func) 0xdeadbeef;

	p1.alloc_func = alloc_func;
	p1.free_func = free_func;
	p2.alloc_func = alloc_func;
	p2.free_func = free_func;

	err = ptab_init(&p1, PTABS_USE_ALLOCATOR);
	ck_assert_int_eq(err, PTABS_OK);

	err = ptab_init(&p2, PTABS_USE_BUFFER);
	ck_assert_int_eq(err, PTABS_OK);

	ck_assert(p1.alloc_func != p2.alloc_func);
	ck_assert(p1.free_func != p2.free_func);
}
END_TEST


/* Buffer test case */

static struct ptab buffer_p;

void fixture_buffer_setup(void)
{
	ptab_init(&buffer_p, PTABS_USE_BUFFER);
}

void fixture_buffer_teardown(void)
{
	/* do nothing */
}

START_TEST (test_buffer_set)
{
	char buf[32];
	int err;

	err = ptab_buffer_set(&buffer_p, buf, 32);
	ck_assert_int_eq(err, PTABS_OK);

	ck_assert(buffer_p.buffer.buf == buf);
	ck_assert_int_eq(buffer_p.buffer.size, 32);
	ck_assert_int_eq(buffer_p.buffer.used, 0);
	ck_assert_int_eq(buffer_p.buffer.avail, 32);
}
END_TEST

START_TEST (test_buffer_set_wrong_init)
{
	struct ptab p;
	char buf[32];
	int err;

	ptab_init(&p, PTABS_USE_ALLOCATOR);

	err = ptab_buffer_set(&p, buf, 32);
	ck_assert_int_eq(err, PTABS_ERR_NOT_BUFFER);
}
END_TEST

START_TEST (test_buffer_null)
{
	int err;

	err = ptab_buffer_set(&buffer_p, NULL, 128);
	ck_assert_int_eq(err, PTABS_ERR_NULL);
}
END_TEST

START_TEST (test_buffer_zero_allocation)
{
	char buf[32];
	void *v;
	int err;

	err = ptab_buffer_set(&buffer_p, buf, 32);
	ck_assert_int_eq(err, PTABS_OK);

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

	err = ptab_buffer_set(&buffer_p, buf, 32);
	ck_assert_int_eq(err, PTABS_OK);

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


/* Allocator test case */

START_TEST (test_allocator_set_null)
{
	struct ptab p;
	int err;
	ptab_alloc_func alloc_func = (ptab_alloc_func) 0xabcd0001;
	ptab_free_func free_func = (ptab_free_func) 0xabcd0002;

	ptab_init(&p, PTABS_USE_ALLOCATOR);

	err = ptab_allocator_set(&p, NULL, free_func, NULL);
	ck_assert_int_eq(err, PTABS_ERR_NULL);

	err = ptab_allocator_set(&p, alloc_func, NULL, NULL);
	ck_assert_int_eq(err, PTABS_ERR_NULL);
}
END_TEST

START_TEST (test_allocator_set)
{
	struct ptab p;
	int err;
	ptab_alloc_func alloc_func = (ptab_alloc_func) 0xabcd0001;
	ptab_free_func free_func = (ptab_free_func) 0xabcd0002;
	void *opaque = (void*) 0xdeadbeef;

	ptab_init(&p, PTABS_USE_ALLOCATOR);

	err = ptab_allocator_set(&p, alloc_func, free_func, opaque);
	ck_assert_int_eq(err, PTABS_OK);

	ck_assert_int_eq(p.alloc_total, 0);
	ck_assert(p.alloc_func == alloc_func);
	ck_assert(p.free_func == free_func);
	ck_assert(p.opaque == opaque);
}
END_TEST

START_TEST (test_allocator_set_wrong_init)
{
	struct ptab p;
	int err;
	ptab_alloc_func alloc_func = (ptab_alloc_func) 0xabcd0001;
	ptab_free_func free_func = (ptab_free_func) 0xabcd0002;
	void *opaque = (void*) 0xdeadbeef;

	ptab_init(&p, PTABS_USE_BUFFER);

	err = ptab_allocator_set(&p, alloc_func, free_func, opaque);
	ck_assert_int_eq(err, PTABS_ERR_NOT_ALLOCATOR);
}
END_TEST
#endif


/* Suite definition */

Suite *get_libptab_suite(void)
{
	Suite *s;
	TCase *tc_version;
	//TCase *tc_init;
	//TCase *tc_buffer;
	//TCase *tc_allocator;

	s = suite_create("libptab Test Suite");

	/* create test cases */
	tc_version = tcase_create("Version");
	tcase_add_test(tc_version, test_version_string);
	tcase_add_test(tc_version, test_version);
	suite_add_tcase(s, tc_version);

#if 0
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
	tcase_add_test(tc_buffer, test_buffer_set_wrong_init);
	tcase_add_test(tc_buffer, test_buffer_null);
	tcase_add_test(tc_buffer, test_buffer_zero_allocation);
	tcase_add_test(tc_buffer, test_buffer_allocations);
	suite_add_tcase(s, tc_buffer);

	tc_allocator = tcase_create("Allocator");
	tcase_add_test(tc_allocator, test_allocator_set_null);
	tcase_add_test(tc_allocator, test_allocator_set);
	tcase_add_test(tc_allocator, test_allocator_set_wrong_init);
	suite_add_tcase(s, tc_allocator);
#endif

	return s;
}


