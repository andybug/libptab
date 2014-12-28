
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include <ptab.h>

#include "internal.h"

/*
 * Call the user-provided allocator function
 */
static void *external_alloc(ptab_t *p, size_t size)
{
	void *ptr;

	ptr = p->allocator.alloc_func(size, p->allocator.opaque);

	if (ptr) {
		p->allocator_stats.allocated += size;
		p->allocator_stats.num_allocations++;
	}

	return ptr;
}

static void external_free(const ptab_t *p, void *v)
{
	p->allocator.free_func(v, p->allocator.opaque);
}

/*
 * If no allocator was provided, use default_alloc and
 * default_free, which use malloc/free
 */
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

static size_t calculate_alloc_size(const ptab_t *p, size_t min_size)
{
	size_t size;

	if (!p->internal)
		/*
		 * this is the first allocation, so p->internal
		 * doesn't exist: just allocate base size less
		 * overhead
		 */
		size = PTAB_ALLOC_BASE_SIZE - PTAB_ALLOC_OVERHEAD;
	else {
		/*
		 * we're growing allocations at a 2x rate: so we
		 * simply shift the base size by the number of allocations
		 * already made, then subtract out some bytes to help
		 * malloc with its overhead
		 */
		size = (PTAB_ALLOC_BASE_SIZE << p->internal->alloc_count) -
		       PTAB_ALLOC_OVERHEAD;
	}

	if (min_size > (size - sizeof(struct ptab_bst_node)))
		/*
		 * this allocation is even larger than the 2x growth
		 * so we will just make a new node large enough
		 * for it and the node struct
		 */
		size = min_size + sizeof(struct ptab_bst_node);

	return size;
}

static struct ptab_bst_node *alloc_node(ptab_t *p, size_t size)
{
	struct ptab_bst_node *node;
	size_t alloc_size;

	assert(sizeof(struct ptab_bst_node) < PTAB_ALLOC_BASE_SIZE);

	/* figure out how large of an allocation we need */
	alloc_size = calculate_alloc_size(p, size);

	/* request a block of memory from the user-provided allocator */
	node = external_alloc(p, alloc_size);
	if (!node)
		return NULL;

	/*
	 * the usable buffer is positioned immediately after the
	 * ptab_bst_node structure. initialize all of
	 * the node's fields
	 */
	node->buf = (unsigned char*)(node + 1);
	node->used = 0;
	node->avail = alloc_size - sizeof(struct ptab_bst_node);
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;

	/* account for the node struct in the used bytes statistics */
	p->allocator_stats.used += sizeof(struct ptab_bst_node);
	p->allocator_stats.num_allocations++;

	/* if this isn't the first allocation, update alloc_count */
	if (p->internal)
		p->internal->alloc_count++;

	return node;
}

static void *alloc_from_node(ptab_t *p, struct ptab_bst_node *n, size_t size)
{
	void *ptr;

	assert((((intptr_t)n->buf) & (sizeof(void*) - 1)) == 0);
	assert(n->used % sizeof(void*) == 0);
	assert(size % sizeof(void*) == 0);

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

	if (!ret && (size <= tree->avail))
		ret = tree;

	return ret;
}

static void insert_node(
	struct ptab_bst_node *tree,
	struct ptab_bst_node *node)
{
	if (node->avail < tree->avail) {
		if (tree->left)
			insert_node(tree->left, node);
		else {
			node->parent = tree;
			node->left = NULL;
			node->right = NULL;
			tree->left = node;
		}
	} else {
		if (tree->right)
			insert_node(tree->right, node);
		else {
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

/*
 * Replace references to node in node->parent with references
 * to new_node
 */
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

static void remove_node(ptab_t *p, struct ptab_bst_node *node)
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
		if (node->parent)
			replace_in_parent(node, node->left);
		else {
			p->internal->alloc_tree = node->left;
			node->left->parent = NULL;
		}

	} else if (!node->left && node->right) {
		/*
		 * if just a right child, set parent's pointer
		 * to this node to the right child
		 */
		if (node->parent)
			replace_in_parent(node, node->right);
		else {
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

		if (node->parent)
			replace_in_parent(node, new_node);
		else {
			new_node->parent = NULL;
			p->internal->alloc_tree = new_node;
		}

		node->right->parent = new_node;
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
		} else if (n->avail >= n->parent->avail)
			return 1;
	}

	if (n->left && (n->left->avail >= n->avail))
		return 1;

	if (n->right && (n->right->avail < n->avail))
		return 1;

	return 0;
}

/*
 * Round a size up to the next multiple of the system's
 * word size
 *
 * Examples, assuming 8-byte pointers:
 * size = 5, returns 8
 * size = 8, returns 8
 * size = 9, returns 16
 */
static size_t align_size(size_t size)
{
	static const size_t mask = sizeof(void*) - 1;
	size_t base;
	size_t rem;

	base = size & (~mask);
	rem = size & mask;

	return base + (rem ? sizeof(void*) : 0);
}

void *ptab_alloc(ptab_t *p, size_t size)
{
	struct ptab_bst_node *n;
	void *ptr = NULL;

	/* make size a multiple of the word size */
	size = align_size(size);

	/*
	 * try to find a node in the tree that can satisfy
	 * the request
	 */
	n = find_node(p->internal->alloc_tree, size);

	if (n) {
		/* a node was found, allocate from it */
		ptr = alloc_from_node(p, n, size);

		/*
		 * this should never be NULL since find_node already
		 * checked sizes, but sanity check anyway
		 */
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
		 * no current node was large enough for this allocation,
		 * so create a new node with at least enough room
		 * to hold this allocation request
		 */
		n = alloc_node(p, size);
		if (!n)
			return NULL;

		ptr = alloc_from_node(p, n, size);
		assert(ptr);

		/* this is a new node, so add it into the tree */
		insert_node(p->internal->alloc_tree, n);
	}

	return ptr;
}

int ptab_init(ptab_t *p, const ptab_allocator_t *a)
{
	struct ptab_bst_node *root;

	/*
	 * sanity check that the initial block allocation can safely
	 * store the tree structure and the internal structure
	 */
	assert((PTAB_ALLOC_BASE_SIZE + PTAB_ALLOC_OVERHEAD) >=
	       (sizeof(struct ptab_bst_node) +
		sizeof(struct ptab_internal)));

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

	/*
	 * initialize internal to NULL, this is what alloc_node
	 * keys on to determine if it is the first allocation
	 */
	p->internal = NULL;

	/* allocate node that will contain the internal structure */
	root = alloc_node(p, sizeof(struct ptab_internal));
	if (!root)
		return PTAB_ENOMEM;

	/* allocate the internal structure from the root node */
	p->internal = alloc_from_node(p, root,
				      sizeof(struct ptab_internal));

	/* this shouldn't ever be NULL, but just make sure */
	assert(p->internal);

	/*
	 * now that the internal structure has been allocated,
	 * set the top of the allocator tree to be the node
	 * we already allocated: at this point we can start
	 * using ptab_alloc()
	 */
	p->internal->alloc_tree = root;
	p->internal->alloc_count = 1;
	p->internal->num_columns = 0;
	p->internal->num_rows = 0;
	p->internal->columns_head = NULL;
	p->internal->columns_tail = NULL;
	p->internal->rows_head = NULL;
	p->internal->rows_tail = NULL;
	p->internal->current_row = NULL;

	return PTAB_OK;
}

static void ptab_free_tree(ptab_t *p, struct ptab_bst_node *t)
{
	if (t->left)
		ptab_free_tree(p, t->left);

	if (t->right)
		ptab_free_tree(p, t->right);

	external_free(p, t);
}

int ptab_free(ptab_t *p)
{
	if (!p)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	ptab_free_tree(p, p->internal->alloc_tree);
	p->internal = NULL;

	return PTAB_OK;
}
