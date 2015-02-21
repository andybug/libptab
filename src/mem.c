
#include <stddef.h>

#include "internal.h"

struct mem_block {
	unsigned char *buf;
	size_t used;
	size_t avail;
	struct mem_block *next;
};

struct mem_block_cache {
	unsigned int num_blocks;
	size_t total_used;
	size_t total_avail;
	struct mem_block *head;
	struct mem_block *tail;
};

struct mem_internal {
	struct ptab_allocator funcs;
	struct mem_block_cache cache;
};

static void cache_insert(struct mem_block_cache *c, struct mem_block *b)
{
}

static void cache_free(struct mem_block_cache *c)
{
}

void *mem_alloc(struct ptab_internal *p, size_t size)
{
	return NULL;
}

struct ptab_internal *mem_init(const ptab_allocator_t *funcs)
{
	return NULL;
}
