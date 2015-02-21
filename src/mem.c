
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#define MEM_BLOCK_SIZE      4096
#define MEM_BLOCK_OVERHEAD    32

static void *default_alloc(size_t size, void *opaque)
{
	(void)opaque;
	return malloc(size);
}

static void default_free(void *p, void *opaque)
{
	(void)opaque;
	free(p);
}

static void cache_insert(struct mem_block_cache *c, struct mem_block *b)
{
}

static void cache_free(struct mem_block_cache *c)
{
}

void *mem_alloc(ptab_t *p, size_t size)
{
	return NULL;
}

ptab_t *mem_init(const ptab_allocator_t *funcs_)
{
	/*
	 * set up allocator functions on the stack since
	 * our internal object hasn't been created yet
	 */
	ptab_allocator_t funcs;

	if (!funcs_ || !funcs_->alloc_func || !funcs_->free_func)
		funcs = { default_alloc, default_free, NULL };
	else
		funcs = *funcs_;

	/*
	 * calculate initial allocation size and allocate
	 * the internal structure
	 */
	ptab_t *p;
	size_t size;

	size = MEM_BLOCK_SIZE - MEM_BLOCK_OVERHEAD;
	assert(sizeof(ptab_t) < size);

	p = funcs.alloc_func(size, funcs.opaque);
	if (!p)
		return NULL;

	/* zero the structure */
	memset(p, 0, sizeof(ptab_t));

	/*
	 * initialize root mem_block
	 * this block should never be free'd since the
	 * ptab_internal structure resides in it
	 */
	struct mem_block *block;
	block = (struct mem_block *)(p + 1);
	block->buf = (unsigned char *)p;
	block->used = sizeof(ptab_t) + sizeof(struct mem_block);
	block->avail = size - block->used;

	cache_insert(&p->mem.cache, block);
	p->mem.cache.root = block;

	return p;
}
