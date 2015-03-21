#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdbool.h>
#include <ptab.h>

struct mem_block {
	unsigned char *buf;
	size_t used;
	size_t avail;
	struct mem_block *prev;
	struct mem_block *next;
};

struct mem_block_cache {
	unsigned int num_blocks;
	size_t total_used;
	size_t total_avail;
	struct mem_block *head;
	struct mem_block *tail;
	struct mem_block *root;
};

struct mem_internal {
	bool disabled;
	struct ptab_allocator funcs;
	struct mem_block_cache cache;
};

struct ptab_col {
	unsigned int id;
	char *name;
	enum ptab_type type;
	enum ptab_align align;
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
	struct mem_internal mem;

	unsigned int num_columns;
	unsigned int num_rows;

	struct ptab_col *columns_head;
	struct ptab_col *columns_tail;

	struct ptab_row *rows_head;
	struct ptab_row *rows_tail;

	struct ptab_row *current_row;
	struct ptab_col *current_column;
};

/* mem.c */
extern ptab_t *mem_init(const ptab_allocator_t *funcs);
extern void    mem_free(ptab_t *p);
extern void   *mem_free_block(ptab_t *p, void *block);
extern void   *mem_alloc(ptab_t *p, size_t size);
extern void   *mem_alloc_block(ptab_t *p, size_t size);
extern void    mem_enable(ptab_t *p);
extern void    mem_disable(ptab_t *p);

#endif
