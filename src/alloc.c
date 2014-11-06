
#include <assert.h>
#include <stdlib.h>

#include <ptab.h>

#include "internal.h"

/*
 * Call the user-provided allocator function
 */

static void *ptab_alloc_external(ptab *p, size_t size)
{
	void *ptr;

	ptr = p->allocator.alloc_func(size, p->allocator.opaque);

	if (ptr) {
		p->allocator_stats.allocated += size;
		p->allocator_stats.num_allocations++;
	}

	return ptr;
}

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

static struct ptab_alloc_tree_s *ptab_alloc_block(ptab *p)
{
	struct ptab_alloc_tree_s *tree;

	/* TODO: grow the allocation size; 1.5x? 2x? exponential? */
	assert(sizeof(struct ptab_alloc_tree_s) < PTAB_ALLOC_BLOCK_SIZE);

	tree = ptab_alloc_external(p, PTAB_ALLOC_BLOCK_SIZE);
	if (!tree)
		return NULL;

	tree->block = (unsigned char*)(tree + 1);
	tree->used = 0;
	tree->avail = PTAB_ALLOC_BLOCK_SIZE - sizeof(struct ptab_alloc_tree_s);
	tree->parent = NULL;
	tree->left = NULL;
	tree->right = NULL;

	p->allocator_stats.used += sizeof(struct ptab_alloc_tree_s);

	return tree;
}

/*
static struct ptab_alloc_tree_s *find_block(
		struct ptab_alloc_tree_s *t,
		size_t size)
{
	struct ptab_alloc_tree_s *block;

	if (!t)
		return NULL;

	if (size <= t->avail) {
		block = find_block(t->left, size);
		return block ? block : t;
	}

	block = find_block(t->right, size);
	return block ? block : NULL;
}
*/

int ptab_init(ptab *p, const ptab_allocator *a)
{
	struct ptab_alloc_tree_s *root;

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
	p->allocator_stats.allocated = 0;
	p->allocator_stats.used = 0;
	p->allocator_stats.num_allocations = 0;

	/* allocate block that will contain the internal structure */
	root = ptab_alloc_block(p);
	if (!root)
		return PTAB_ENOMEM;

	/*
	 * set the internal pointer to the top of the block
	 * and account for it in the used and avail totals
	 */
	p->internal = (struct ptab_internal_s*)root->block;
	root->used = sizeof(struct ptab_internal_s);
	root->avail -= sizeof(struct ptab_internal_s);
	p->allocator_stats.used += sizeof(struct ptab_internal_s);

	/*
	 * now that the internal structure has been allocated,
	 * set the top of the allocator tree to be the block
	 * we already allocated: at this point we can start
	 * using ptab_alloc()
	 */
	p->internal->alloc_tree = root;

	return PTAB_OK;
}

int ptab_free(ptab *p)
{
	return PTAB_OK;
}
