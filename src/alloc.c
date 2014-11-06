
#include <assert.h>

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
		p->allocator_stats.total += size;
		p->allocator_stats.allocations++;
	}

	return ptr;
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
	tree->avail = PTAB_ALLOC_BLOCK_SIZE - sizeof(struct ptab_alloc_tree_s);
	tree->parent = NULL;
	tree->left = NULL;
	tree->right = NULL;

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

int ptab_init(ptab *p, const ptab_allocator *pa)
{
	return PTAB_OK;
}

int ptab_free(ptab *p)
{
	return PTAB_OK;
}
