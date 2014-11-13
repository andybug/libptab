
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

static struct ptab_alloc_tree_s *ptab_find_block(ptab *p,
		struct ptab_alloc_tree_s *t,
		size_t size)
{
	struct ptab_alloc_tree_s *ret = NULL;

	if (t->left && (size < t->avail))
		ret = ptab_find_block(p, t->left, size);
	else if (t->right && (size >= t->avail))
		ret = ptab_find_block(p, t->right, size);

	if (!ret && (size >= t->avail))
		return t;

	return NULL;
}

static struct ptab_alloc_tree_s *ptab_find_largest_block(
		struct ptab_alloc_tree_s *t)
{
	if (t->right)
		return ptab_find_largest_block(t->right);

	return t;
}

static void ptab_insert_block(
		struct ptab_alloc_tree_s *tree,
		struct ptab_alloc_tree_s *block)
{
	if (block->avail < tree->avail) {
		if (tree->left) {
			ptab_insert_block(tree->left, block);
		} else {
			block->parent = tree;
			block->left = NULL;
			block->right = NULL;
			tree->left = block;
		}
	} else {
		if (tree->right) {
			ptab_insert_block(tree->right, block);
		} else {
			block->parent = tree;
			block->left = NULL;
			block->right = NULL;
			tree->right = block;
		}
	}
}

static void ptab_remove_block(ptab *p, struct ptab_alloc_tree_s *block)
{
	struct ptab_alloc_tree_s *largest;

	if (!block->left && !block->right) {
		if (block->parent) {
			if (block == block->parent->left)
				block->parent->left = NULL;
			else
				block->parent->right = NULL;
		} else {
			p->internal->alloc_tree = NULL;
		}
	} else if (block->left && !block->right) {
		if (block->parent) {
			if (block == block->parent->left) {
				block->parent->left = block->left;
				block->left->parent = block->parent;
			} else {
				block->parent->right = block->left;
				block->left->parent = block->parent;
			}
		} else {
			p->internal->alloc_tree = block->left;
			block->left->parent = NULL;
		}
	} else if (!block->left && block->right) {
		if (block->parent) {
			if (block == block->parent->left) {
				block->parent->left = block->right;
				block->right->parent = block->parent;
			} else {
				block->parent->right = block->right;
				block->right->parent = block->parent;
			}
		} else {
			p->internal->alloc_tree = block->right;
			block->right->parent = NULL;
		}
	} else {
		largest = ptab_find_largest_block(block->left);
		ptab_remove_block(p, largest);
		largest->parent = block->parent;
		largest->left = block->left;
		largest->right = block->right;

		if (block->parent) {
			if (block == block->parent->left)
				block->parent->left = largest;
			else
				block->parent->right = largest;
		} else {
			p->internal->alloc_tree = largest;
		}

		if (block->right)
			block->right->parent = largest;

		if (block->left)
			block->left->parent = largest;
	}
}

/*
 * Check if node satisfies BST properties
 *
 * A BST node must satisfy the following properties:
 *  1. If has parent and is left child, node < parent
 *  2. If has parent and is right child, node >= parent
 *  3. If has left child, node > left
 *  4. If has right child, node <= right
 *
 * Returns 0 if valid and 1 if invalid
 */
static int check_bst_node(struct ptab_alloc_tree_s *t)
{
	if (t->parent) {
		if (t == t->parent->right) {
			if (t->avail < t->parent->avail)
				return 1;
		} else {
			if (t->avail >= t->parent->avail)
				return 1;
		}
	}

	if (t->left && (t->left->avail >= t->avail))
		return 1;

	if (t->right && (t->right->avail < t->avail))
		return 1;

	return 0;
}

void *ptab_alloc(ptab *p, size_t size)
{
	struct ptab_alloc_tree_s *t;
	void *ptr = NULL;

	t = ptab_find_block(p, p->internal->alloc_tree, size);

	if (t) {
		ptr = ptab_alloc_from_block(p, t, size);
		assert(ptr);

		/*
		 * if this allocation causes the BST to no longer
		 * be correct, rebalance it by removing the node
		 * and reinserting it
		 */
		if (check_bst_node(t)) {
			ptab_remove_block(p, t);
			ptab_insert_block(p->internal->alloc_tree, t);
		}
	} else {
		/*
		 * TODO: modify allow_block to accept a minimum size
		 * to allocate, otherwise a single large allocation
		 * could cause it to fail
		 */
		t = ptab_alloc_block(p);
		if (!t)
			return NULL;

		ptr = ptab_alloc_from_block(p, t, size);
		assert(ptr);

		ptab_insert_block(p->internal->alloc_tree, t);
	}

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
