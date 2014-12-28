#ifndef INTERNAL_H
#define INTERNAL_H

#include <ptab.h>

#define PTAB_ALLOC_BASE_SIZE  4096
#define PTAB_ALLOC_OVERHEAD     32

struct ptab_bst_node {
	unsigned char *buf;
	size_t used;
	size_t avail;
	struct ptab_bst_node *parent;
	struct ptab_bst_node *left;
	struct ptab_bst_node *right;
};

struct ptab_col {
	unsigned int id;
	char *name;
	int type;
	int align;
	size_t name_len;
	size_t width;
	struct ptab_col *next;
};

union ptab_row_data {
	char *s;
	int i;
	double f;
};

struct ptab_row {
	union ptab_row_data *data;
	char **strings;
	size_t *lengths;
	struct ptab_row *next;
};

struct ptab_internal {
	struct ptab_bst_node *alloc_tree;
	unsigned int alloc_count;

	unsigned int num_columns;
	unsigned int num_rows;

	struct ptab_col *columns_head;
	struct ptab_col *columns_tail;

	struct ptab_row *rows_head;
	struct ptab_row *rows_tail;

	struct ptab_row *current_row;
	struct ptab_col *current_column;
};

/* alloc.c */
extern void *ptab_alloc(ptab_t *p, size_t size);

#endif
