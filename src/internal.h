#ifndef INTERNAL_H
#define INTERNAL_H

#include <ptab.h>

#define PTAB_ALLOC_BASE_SIZE  4096
#define PTAB_ALLOC_OVERHEAD     32

#define PTAB_INTERNAL_SYM __attribute__ ((visibility ("hidden")))

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
	unsigned int alloc_count;
};

/* alloc.c */
extern void * PTAB_INTERNAL_SYM ptab_alloc(ptab *p, size_t size);

#endif
