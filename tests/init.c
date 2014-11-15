
#include <stdlib.h>
#include <string.h>

#include <check.h>
#include <ptab.h>

static ptab p;
static int err;

static void fixture_zero(void)
{
	memset(&p, 0, sizeof(ptab));
}

static void fixture_free(void)
{
	ptab_free(&p);
}

static void *alloc_func(size_t size, void *opaque)
{
	(void)opaque;

	return malloc(size);
}

static void *null_alloc_func(size_t size, void *opaque)
{
	(void)size;
	(void)opaque;

	return NULL;
}

static void free_func(void *ptr, void *opaque)
{
	(void)opaque;

	free(ptr);
}

START_TEST (init_default)
{
	err = ptab_init(&p, NULL);
	ck_assert_int_eq(err, PTAB_OK);

	ck_assert(p.internal != NULL);
	ck_assert(p.allocator.alloc_func != NULL);
	ck_assert(p.allocator.free_func != NULL);
	ck_assert(p.allocator.opaque == NULL);
}
END_TEST

START_TEST (init_allocator)
{
	ptab_allocator pa;

	pa.alloc_func = alloc_func;
	pa.free_func = free_func;
	pa.opaque = (void*)(0xdeadbeef);

	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_OK);

	ck_assert(p.internal != NULL);
	ck_assert(p.allocator.alloc_func == alloc_func);
	ck_assert(p.allocator.free_func == free_func);
	ck_assert(p.allocator.opaque == (void*)(0xdeadbeef));
}
END_TEST

START_TEST (init_nomem)
{
	ptab_allocator pa;

	pa.alloc_func = null_alloc_func;
	pa.free_func = free_func;
	pa.opaque = (void*)(0xdeadbeef);

	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST

START_TEST (init_null)
{
	err = ptab_init(NULL, NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (init_null_allocator)
{
	ptab_allocator pa;

	pa.alloc_func = NULL;
	pa.free_func = free_func;
	pa.opaque = (void*)(0xdeadbeef);

	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);

	pa.alloc_func = alloc_func;
	pa.free_func = NULL;

	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

TCase *init_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Init");
	tcase_add_checked_fixture(tc, fixture_zero, fixture_free);
	tcase_add_test(tc, init_allocator);
	tcase_add_test(tc, init_default);
	tcase_add_test(tc, init_nomem);
	tcase_add_test(tc, init_null);
	tcase_add_test(tc, init_null_allocator);

	return tc;
}
