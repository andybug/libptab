#ifndef INTERNAL_H
#define INTERNAL_H

#include <ptab.h>

#define PTAB_ALLOC_BLOCK_SIZE 2048

enum ptab_tree_color {
	PTAB_TREE_RED = 1,
	PTAB_TREE_BLACK
};

struct ptab_alloc_tree_s {
	unsigned char *block;
	size_t used;
	size_t avail;
	struct ptab_alloc_tree_s *parent;
	struct ptab_alloc_tree_s *left;
	struct ptab_alloc_tree_s *right;
	enum ptab_tree_color color;
};

struct ptab_internal_s {
	struct ptab_alloc_tree_s *alloc_tree;
};

/* alloc.c */
extern void *ptab_alloc(ptab *p, size_t size);

#endif
