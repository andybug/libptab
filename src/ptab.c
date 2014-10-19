
#include <stdlib.h>
#include <ptab.h>

#include "internal.h"

/* Allocation functions */

static void *default_alloc(size_t size, void *opaque)
{
	(void)opaque;

	return malloc(size);
}

static void default_free(void *ptr, void *opaque)
{
	(void)opaque;

	free(ptr);
}

static void *internal_alloc(struct ptab *p, size_t size)
{
	void *ptr;

	ptr = p->allocator.alloc_func(size, p->allocator.opaque);

	if (ptr) {
		p->allocator_stats.total += size;
		p->allocator_stats.allocations++;
	}

	return ptr;
}

static void internal_free(struct ptab *p, void *ptr)
{
	p->allocator.free_func(ptr, p->allocator.opaque);
	p->allocator_stats.frees++;
}

/* API functions */

const char *ptab_version_string(void)
{
	static const char version_string[] = PTAB_VERSION_STRING;

	return version_string;
}

void ptab_version(int *major, int *minor, int *patch)
{
	*major = PTAB_VERSION_MAJOR;
	*minor = PTAB_VERSION_MINOR;
	*patch = PTAB_VERSION_PATCH;
}

int ptab_init(struct ptab *p, const struct ptab_allocator *a)
{
	if (p == NULL)
		return PTAB_ENULL;

	/*
	 * set up the allocator functions since we're
	 * going to need to get some memory right away
	 */
	if (a != NULL) {
		if (a->alloc_func == NULL || a->free_func == NULL)
			return PTAB_ENULL;
		p->allocator = *a;
	} else {
		p->allocator.alloc_func = default_alloc;
		p->allocator.free_func = default_free;
		p->allocator.opaque = NULL;
	}

	/* initialize allocator stats before first alloc */
	p->allocator_stats.total = 0;
	p->allocator_stats.allocations = 0;
	p->allocator_stats.frees = 0;

	/* allocate the library's internal structure */
	p->internal = internal_alloc(p, sizeof(struct ptab_internal));
	if (!p->internal)
		return PTAB_ENOMEM;

	/* initialize internals */
	p->internal->state = PTAB_STATE_INITIALIZED;
	p->internal->columns = NULL;
	p->internal->rows = NULL;
	p->internal->num_columns = 0;
	p->internal->num_rows = 0;

	return PTAB_OK;
}

int ptab_begin_columns(struct ptab *p)
{
	if (!p)
		return PTAB_ENULL;

	if (!p->internal || p->internal->state != PTAB_STATE_INITIALIZED)
		return PTAB_EORDER;

	p->internal->state = PTAB_STATE_BEGIN_COLUMNS;

	return PTAB_OK;
}

int ptab_free(struct ptab *p)
{
	if (!p)
		return PTAB_ENULL;

	internal_free(p, p->internal);
	p->internal = NULL;

	return PTAB_OK;
}
