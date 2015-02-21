
#include <stdlib.h>
#include <string.h>

#include <check.h>
#include <ptab.h>

static ptab_t *p;

static void fixture_zero(void)
{
	p = NULL;
}

static void fixture_free(void)
{
	ptab_free(p);
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
	p = ptab_init(NULL);
	ck_assert(p != NULL);
}
END_TEST

START_TEST (init_allocator)
{
	ptab_allocator_t pa;

	pa.alloc_func = alloc_func;
	pa.free_func = free_func;
	pa.opaque = (void*)(0xdeadbeef);

	p = ptab_init(&pa);
	ck_assert(p != NULL);
}
END_TEST

START_TEST (init_nomem)
{
	ptab_allocator_t pa;

	pa.alloc_func = null_alloc_func;
	pa.free_func = free_func;
	pa.opaque = (void*)(0xdeadbeef);

	p = ptab_init(&pa);
	ck_assert(p == NULL);
}
END_TEST

START_TEST (init_null_allocator)
{
	ptab_allocator_t pa;

	pa.alloc_func = NULL;
	pa.free_func = free_func;
	pa.opaque = (void*)(0xdeadbeef);

	p = ptab_init(&pa);
	ck_assert(p == NULL);

	pa.alloc_func = alloc_func;
	pa.free_func = NULL;

	p = ptab_init(&pa);
	ck_assert(p == NULL);
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
	tcase_add_test(tc, init_null_allocator);

	return tc;
}
