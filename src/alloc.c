
#include <assert.h>
#include <stdlib.h>

#include <ptab.h>

#include "internal.h"

/*
 * Call the user-provided allocator function
 */
static void *external_alloc(ptab *p, size_t size)
{
	void *ptr;

	ptr = p->allocator.alloc_func(size, p->allocator.opaque);

	if (ptr) {
		p->allocator_stats.allocated += size;
		p->allocator_stats.num_allocations++;
	}

	return ptr;
}

static void external_free(const ptab *p, void *v)
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

static struct ptab_bst_node *alloc_node(ptab *p)
{
	struct ptab_bst_node *node;

	/* TODO: grow the allocation size; 1.5x? 2x? exponential? */
	assert(sizeof(struct ptab_bst_node) < PTAB_ALLOC_BLOCK_SIZE);

	/* request a block of memory from the user-provided allocator */
	node = external_alloc(p, PTAB_ALLOC_BLOCK_SIZE);
	if (!node)
		return NULL;

	/*
	 * the usable buffer is positioned immediately after the
	 * ptab_bst_node structure. initialize all of
	 * the node's fields
	 */
	node->buf = (unsigned char*)(node + 1);
	node->used = 0;
	node->avail = PTAB_ALLOC_BLOCK_SIZE - sizeof(struct ptab_bst_node);
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;

	/* account for the tree struct in the used bytes statistics */
	p->allocator_stats.used += sizeof(struct ptab_bst_node);

	return node;
}

static void *alloc_from_node(ptab *p, struct ptab_bst_node *n, size_t size)
{
	void *ptr;

	/* if there's not enough space, get outta here */
	if (n->avail < size)
		return NULL;

	/* save the current location in the buffer */
	ptr = n->buf + n->used;

	/* update usage information for the block */
	n->used += size;
	n->avail -= size;

	/* update allocator stats */
	p->allocator_stats.used += size;

	return ptr;
}

static struct ptab_bst_node *find_node(struct ptab_bst_node *tree, size_t size)
{
	struct ptab_bst_node *ret = NULL;

	if (tree->left && (size < tree->avail))
		ret = find_node(tree->left, size);
	else if (tree->right && (size >= tree->avail))
		ret = find_node(tree->right, size);

	if (!ret && (size >= tree->avail))
		return tree;

	return NULL;
}

static void insert_node(
		struct ptab_bst_node *tree,
		struct ptab_bst_node *node)
{
	if (node->avail < tree->avail) {
		if (tree->left) {
			insert_node(tree->left, node);
		} else {
			node->parent = tree;
			node->left = NULL;
			node->right = NULL;
			tree->left = node;
		}
	} else {
		if (tree->right) {
			insert_node(tree->right, node);
		} else {
			node->parent = tree;
			node->left = NULL;
			node->right = NULL;
			tree->right = node;
		}
	}
}

/*
 * Find the smallest node in a subtree
 *
 * This is used to find a replacement for node that is
 * being deleted. It is assumed that this is only called
 * in the case where the deleted node has two children.
 */
static struct ptab_bst_node *find_smallest_node(struct ptab_bst_node *t)
{
	while (t->left)
		t = t->left;

	return t;
}

static void replace_in_parent(
		struct ptab_bst_node *node,
		struct ptab_bst_node *new_node)
{
	if (!node->parent)
		return;

	if (node == node->parent->left)
		node->parent->left = new_node;
	else
		node->parent->right = new_node;

	if (new_node)
		new_node->parent = node->parent;
}

static void remove_node(ptab *p, struct ptab_bst_node *node)
{
	if (!node->left && !node->right) {
		/*
		 * if no children, set parent's pointer to
		 * this node to be NULL
		 */
		if (node->parent)
			replace_in_parent(node, NULL);
		else
			p->internal->alloc_tree = NULL;

	} else if (node->left && !node->right) {
		/*
		 * if just a left child, set parent's pointer
		 * to this node to the left child
		 */
		if (node->parent) {
			replace_in_parent(node, node->left);
		} else {
			p->internal->alloc_tree = node->left;
			node->left->parent = NULL;
		}

	} else if (!node->left && node->right) {
		/*
		 * if just a right child, set parent's pointer
		 * to this node to the right child
		 */
		if (node->parent) {
			replace_in_parent(node, node->right);
		} else {
			p->internal->alloc_tree = node->right;
			node->right->parent = NULL;
		}
	} else {
		/*
		 * If this node has two children, replace it with
		 * the smallest node from the right subtree.
		 */
		struct ptab_bst_node *new_node;

		new_node = find_smallest_node(node->right);
		remove_node(p, new_node);

		if (node->parent) {
			replace_in_parent(node, new_node);
		} else {
			new_node->parent = NULL;
			p->internal->alloc_tree = new_node;
		}

		if (node->right)
			node->right->parent = new_node;

		if (node->left)
			node->left->parent = new_node;
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
static int check_bst_node(struct ptab_bst_node *n)
{
	if (n->parent) {
		if (n == n->parent->right) {
			if (n->avail < n->parent->avail)
				return 1;
		} else {
			if (n->avail >= n->parent->avail)
				return 1;
		}
	}

	if (n->left && (n->left->avail >= n->avail))
		return 1;

	if (n->right && (n->right->avail < n->avail))
		return 1;

	return 0;
}

void *ptab_alloc(ptab *p, size_t size)
{
	struct ptab_bst_node *n;
	void *ptr = NULL;

	n = find_node(p->internal->alloc_tree, size);

	if (n) {
		ptr = alloc_from_node(p, n, size);
		assert(ptr);

		/*
		 * if this allocation causes the BST to no longer
		 * be correct, rebalance it by removing the node
		 * and reinserting it
		 */
		if (check_bst_node(n)) {
			remove_node(p, n);
			insert_node(p->internal->alloc_tree, n);
		}
	} else {
		/*
		 * TODO: modify allow_block to accept a minimum size
		 * to allocate, otherwise a single large allocation
		 * could cause it to fail
		 */
		n = alloc_node(p);
		if (!n)
			return NULL;

		ptr = alloc_from_node(p, n, size);
		assert(ptr);

		insert_node(p->internal->alloc_tree, n);
	}

	return ptr;
}

int ptab_init(ptab *p, const ptab_allocator *a)
{
	struct ptab_bst_node *root;

	/*
	 * sanity check that the initial block allocation can safely
	 * store the tree structure and the internal structure
	 */
	assert(PTAB_ALLOC_BLOCK_SIZE >=
			(sizeof(struct ptab_bst_node) +
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

	/* allocate node that will contain the internal structure */
	root = alloc_node(p);
	if (!root)
		return PTAB_ENOMEM;

	/* allocate the internal structure from the root node */
	p->internal = alloc_from_node(p, root,
			sizeof(struct ptab_internal_s));

	/* this shouldn't ever be NULL, but just make sure */
	assert(p->internal);

	/*
	 * now that the internal structure has been allocated,
	 * set the top of the allocator tree to be the node
	 * we already allocated: at this point we can start
	 * using ptab_alloc()
	 */
	p->internal->alloc_tree = root;

	return PTAB_OK;
}

static void ptab_free_tree(ptab *p, struct ptab_bst_node *t)
{
	if (t->left)
		ptab_free_tree(p, t->left);

	if (t->right)
		ptab_free_tree(p, t->right);

	external_free(p, t);
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
