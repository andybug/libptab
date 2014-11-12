
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

/*
 * Red-Black tree functions
 * http://en.wikipedia.org/wiki/Red%E2%80%93black_tree
 */

static void rotate_left(struct ptab_alloc_tree_s *t);
static void rotate_right(struct ptab_alloc_tree_s *t);
static struct ptab_alloc_tree_s *grandparent(struct ptab_alloc_tree_s *t);
static struct ptab_alloc_tree_s *uncle(struct ptab_alloc_tree_s *t);
static void insert_case1(struct ptab_alloc_tree_s *t);
static void insert_case2(struct ptab_alloc_tree_s *t);
static void insert_case3(struct ptab_alloc_tree_s *t);
static void insert_case4(struct ptab_alloc_tree_s *t);
static void insert_case5(struct ptab_alloc_tree_s *t);

static void rotate_left(struct ptab_alloc_tree_s *t)
{
	struct ptab_alloc_tree_s *parent;

	parent = t->parent;
	t->parent = t->right;
	t->parent->parent = parent;
	t->right = t->right->left;

	assert(parent);
}

static void rotate_right(struct ptab_alloc_tree_s *t)
{
	struct ptab_alloc_tree_s *parent;

	parent = t->parent->parent;
	t->parent->left = t->right;
	t->right = t->parent;
	t->parent = parent;

	assert(parent);
}

static struct ptab_alloc_tree_s *grandparent(struct ptab_alloc_tree_s *t)
{
	if (t && t->parent)
		return t->parent->parent;

	return NULL;
}

static struct ptab_alloc_tree_s *uncle(struct ptab_alloc_tree_s *t)
{
	struct ptab_alloc_tree_s *gp;

	gp = grandparent(t);
	if (!gp)
		return NULL;

	if (t->parent == gp->left)
		return gp->right;
	else
		return gp->left;
}

static void insert_case1(struct ptab_alloc_tree_s *t)
{
	// this case can probably be removed?
	if (!t->parent)
		t->color = PTAB_TREE_BLACK;
	else
		insert_case2(t);
}

static void insert_case2(struct ptab_alloc_tree_s *t)
{
	if (t->parent->color == PTAB_TREE_BLACK)
		return;
	else
		insert_case3(t);
}

static void insert_case3(struct ptab_alloc_tree_s *t)
{
	struct ptab_alloc_tree_s *u, *g;

	u = uncle(t);
	if (u && (u->color == PTAB_TREE_RED)) {
		t->parent->color = PTAB_TREE_BLACK;
		u->color = PTAB_TREE_BLACK;
		g = grandparent(t);
		g->color = PTAB_TREE_RED;
		insert_case1(g);
	} else {
		insert_case4(t);
	}
}

static void insert_case4(struct ptab_alloc_tree_s *t)
{
	struct ptab_alloc_tree_s *g;

	g = grandparent(t);

	if ((t == t->parent->right) && (t->parent == g->left)) {
		rotate_left(t->parent);
		t = t->left;
	} else if ((t == t->parent->left) && (t->parent == g->right)) {
		rotate_right(t->parent);
		t = t->right;
	}

	insert_case5(t);
}

static void insert_case5(struct ptab_alloc_tree_s *t)
{
	struct ptab_alloc_tree_s *g;

	g = grandparent(t);

	t->parent->color = PTAB_TREE_BLACK;
	g->color = PTAB_TREE_RED;

	if (t == t->parent->left)
		rotate_right(g);
	else
		rotate_left(g);
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
	tree->color = PTAB_TREE_RED;

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
			insert_case1(block);
		}
	} else if (block->avail >= tree->avail) {
		if (tree->right) {
			ptab_insert_block(tree->right, block);
		} else {
			block->parent = tree;
			block->left = NULL;
			block->right = NULL;
			tree->right = block;
			insert_case1(block);
		}
	}
}

static void ptab_remove_block(struct ptab_alloc_tree_s *block)
{
}

void *ptab_alloc(ptab *p, size_t size)
{
	struct ptab_alloc_tree_s *t;
	void *ptr = NULL;

	t = ptab_find_block(p, p->internal->alloc_tree, size);

	if (t) {
		ptr = ptab_alloc_from_block(p, t, size);
		assert(ptr);
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
	root->color = PTAB_TREE_BLACK;

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
