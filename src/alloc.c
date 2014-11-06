
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

static void ptab_free_external(ptab *p, void *v)
{
	p->allocator.free_func(v, p->allocator.opaque);
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

	/* request a block of memory from the user-provided allocator */
	tree = ptab_alloc_external(p, PTAB_ALLOC_BLOCK_SIZE);
	if (!tree)
		return NULL;

	/*
	 * the usable block is positioned immediately after the
	 * ptab_alloc_tree_s structure. initialize all of
	 * the tree's fields
	 */
	tree->block = (unsigned char*)(tree + 1);
	tree->used = 0;
	tree->avail = PTAB_ALLOC_BLOCK_SIZE - sizeof(struct ptab_alloc_tree_s);
	tree->parent = NULL;
	tree->left = NULL;
	tree->right = NULL;

	/* account for the tree struct in the used bytes statistics */
	p->allocator_stats.used += sizeof(struct ptab_alloc_tree_s);

	return tree;
}

static void *ptab_alloc_from_block(ptab *p,
		struct ptab_alloc_tree_s *t,
		size_t size)
{
	void *ptr;

	/* if there's not enough space, get outta here */
	if (t->avail < size)
		return NULL;

	/* save the current location in the block */
	ptr = t->block + t->used;

	/* update usage information for the block */
	t->used += size;
	t->avail -= size;

	/* update allocator stats */
	p->allocator_stats.used += size;

	return ptr;
}

int ptab_init(ptab *p, const ptab_allocator *a)
{
	struct ptab_alloc_tree_s *root;

	/*
	 * sanity check that the initial block allocation can safely
	 * store the tree structure and the internal structure
	 */
	assert(PTAB_ALLOC_BLOCK_SIZE >=
			(sizeof(struct ptab_alloc_tree_s) +
			 sizeof(struct ptab_internal_s)));

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

	/* allocate the internal structure from the root block */
	p->internal = ptab_alloc_from_block(p, root,
			sizeof(struct ptab_internal_s));

	/* this shouldn't ever be NULL, but just make sure */
	assert(p->internal);

	/*
	 * now that the internal structure has been allocated,
	 * set the top of the allocator tree to be the block
	 * we already allocated: at this point we can start
	 * using ptab_alloc()
	 */
	p->internal->alloc_tree = root;

	return PTAB_OK;
}

static void ptab_free_tree(ptab *p, struct ptab_alloc_tree_s *t)
{
	if (t->left)
		ptab_free_tree(p, t->left);

	if (t->right)
		ptab_free_tree(p, t->right);

	ptab_free_external(p, t);
}

int ptab_free(ptab *p)
{
	if (!p)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	ptab_free_tree(p, p->internal->alloc_tree);
	p->internal = NULL;

	return PTAB_OK;
}
