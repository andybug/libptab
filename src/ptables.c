
#include <stdlib.h>
#include <stdbool.h>

#include <ptables.h>

/* allocator functions */

static void *default_alloc_func(struct ptable *p, size_t size, void *opaque)
{
	void *m;

	(void)opaque;

	m = malloc(size);
	p->alloc_total += size;

	return m;
}

static void default_free_func(struct ptable *p, void *ptr, void *opaque)
{
	(void)p;
	(void)opaque;

	free(ptr);
}

static void *buffer_alloc_func(struct ptable *p, size_t size, void *opaque)
{
	void *ptr;

	(void)opaque;

	if (size == 0)
		return NULL;

	if (size <= p->buffer.avail) {
		ptr = p->buffer.buf + p->buffer.used;
		p->buffer.used += size;
		p->buffer.avail -= size;
		return ptr;
	}

	return NULL;
}

static void buffer_free_func(struct ptable *p, void *ptr, void *opaque)
{
	(void)p;
	(void)ptr;
	(void)opaque;

	/* No freeing of buffer allocations */
}

/* API functions */

const char *ptables_version(void)
{
	static const char version_string[] = PTABLES_VERSION_STRING;

	return version_string;
}

int ptable_init(struct ptable *p, int flags)
{
	bool use_buffer = flags & PTABLES_USE_BUFFER;
	bool use_allocator = flags & PTABLES_USE_ALLOCATOR;

	if (use_buffer && use_allocator)
		return PTABLES_ERR_ONE_ALLOCATOR;

	p->flags = 0;
	p->columns = 0;
	p->rows = 0;
	p->alloc_total = 0;

	if (use_buffer) {
		p->flags |= PTABLES_USE_BUFFER;
		p->buffer.buf = NULL;
		p->buffer.size = 0;
		p->buffer.used = 0;
		p->buffer.avail = 0;
		p->alloc_func = buffer_alloc_func;
		p->free_func = buffer_free_func;
		p->opaque = NULL;
	} else {
		p->flags |= PTABLES_USE_ALLOCATOR;
		p->alloc_func = default_alloc_func;
		p->free_func = default_free_func;
		p->opaque = NULL;
	}

	return PTABLES_OK;
}

int ptable_buffer_set(struct ptable *p, char *buf, size_t size)
{
	if (buf == NULL)
		return PTABLES_ERR_NULL;

	if ((p->flags & PTABLES_USE_BUFFER) == 0)
		return PTABLES_ERR_NOT_BUFFER;

	p->buffer.buf = buf;
	p->buffer.size = size;
	p->buffer.used = 0;
	p->buffer.avail = size;

	return PTABLES_OK;
}

int ptable_allocator_set(
	struct ptable *p,
	ptable_alloc_func alloc_func,
	ptable_free_func free_func,
	void *opaque)
{
	if ((alloc_func == NULL) || (free_func == NULL))
		return PTABLES_ERR_NULL;

	if ((p->flags & PTABLES_USE_ALLOCATOR) == 0)
		return PTABLES_ERR_NOT_ALLOCATOR;

	p->alloc_func = alloc_func;
	p->free_func = free_func;
	p->opaque = opaque;

	return PTABLES_OK;
}
