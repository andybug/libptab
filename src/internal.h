#ifndef INTERNAL_H
#define INTERNAL_H

#include <ptab.h>

#define PTAB_ALLOC_BLOCK_SIZE 2048

struct ptab_bst_node {
	unsigned char *buf;
	size_t used;
	size_t avail;
	struct ptab_bst_node *parent;
	struct ptab_bst_node *left;
	struct ptab_bst_node *right;
};

struct ptab_internal_s {
	struct ptab_bst_node *alloc_tree;
};

/* alloc.c */
extern void *ptab_alloc(ptab *p, size_t size);

#endif
