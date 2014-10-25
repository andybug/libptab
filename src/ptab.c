
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ptab.h>

#include "internal.h"

/* Allocation functions */

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

static void *internal_alloc(struct ptab *p, size_t size)
{
	void *ptr;

	ptr = p->allocator.alloc_func(size, p->allocator.opaque);

	if (ptr) {
		p->allocator_stats.total += size;
		p->allocator_stats.allocations++;
	}

	return ptr;
}

static void internal_free(struct ptab *p, void *ptr)
{
	p->allocator.free_func(ptr, p->allocator.opaque);
	p->allocator_stats.frees++;
}

/* Column functions */

static void add_to_column_list(struct ptab *p, struct ptab_column *c)
{
	if (p->internal->columns_tail) {
		p->internal->columns_tail->next = c;
		p->internal->columns_tail = c;
		c->next = NULL;
	} else {
		p->internal->columns_head = c;
		p->internal->columns_tail = c;
		c->next = NULL;
	}

	p->internal->num_columns++;
}

static int add_column(struct ptab *p,
		      const char *name,
		      const char *fmt,
		      int type,
		      int align)
{
	size_t name_len;
	size_t total_alloc;
	struct ptab_column *column;

	/*
	 * first, we need to figure out how many bytes to
	 * allocate for this column:
	 * ptab_column size + name len + 1 byte null + fmt len + 1 byte null
	 * in memory:
	 * [ptab_column][name\0][fmt\0]
	 */
	total_alloc = sizeof(struct ptab_column);

	name_len = strlen(name);
	total_alloc += name_len + 1;
	total_alloc += fmt ? (strlen(fmt) + 1) : 0;

	column = internal_alloc(p, total_alloc);
	if (!column)
		return PTAB_ENOMEM;

	/* get pointer to point just behind the ptab_column */
	column->name = (char*)(column + 1);
	strcpy(column->name, name);

	/* copy format, if we have one */
	if (fmt) {
		column->fmt = (char*)(column->name + name_len + 1);
		strcpy(column->fmt, fmt);
	}

	column->name_len = name_len;
	column->type = type;
	column->align = align;
	column->width = name_len;

	add_to_column_list(p, column);

	return PTAB_OK;
}

static void free_columns(struct ptab *p)
{
	struct ptab_column *cur, *next;

	cur = p->internal->columns_head;
	while (cur) {
		next = cur->next;
		internal_free(p, cur);
		cur = next;
	}

	p->internal->columns_head = NULL;
	p->internal->columns_tail = NULL;
}

/* Row functions */

static void add_row_to_list(struct ptab *p, struct ptab_row *r)
{
	if (p->internal->rows_tail) {
		p->internal->rows_tail->next = r;
		p->internal->rows_tail = r;
		r->next = NULL;
	} else {
		p->internal->rows_head = r;
		p->internal->rows_tail = r;
		r->next = NULL;
	}

	p->internal->num_rows++;
}

static int add_row(struct ptab *p)
{
	struct ptab_row *row;
	size_t struct_size;
	size_t data_size, strings_size, lengths_size;
	size_t alloc_total;
	int num_columns;
	int i;

	/*
	 * need to know how many columns in the table to
	 * allocate enough space in each row
	 */
	num_columns = p->internal->num_columns;

	/*
	 * calculate size of each component and make one allocation
	 * for all of them
	 */
	struct_size = sizeof(struct ptab_row);
	data_size = sizeof(union ptab_row_data) * num_columns;
	strings_size = sizeof(char*) * num_columns;
	lengths_size = sizeof(size_t) * num_columns;
	alloc_total = struct_size + data_size + strings_size + lengths_size;

	row = internal_alloc(p, alloc_total);
	if (!row)
		return PTAB_ENOMEM;

	/*
	 * setup the pointers
	 * the memory layout is:
	 * [    ptab_row    ][row data][string data][string lengths]
	 */
	row->data = (union ptab_row_data*)(row + 1);
	row->strings = (char**)(row->data + p->internal->num_columns);
	row->lengths = (size_t*)(row->strings + p->internal->num_columns);

	for (i = 0; i < p->internal->num_columns; i++) {
		row->data[i].s = NULL;
		row->strings[i] = NULL;
		row->lengths[i] = 0;
	}

	add_row_to_list(p, row);

	return PTAB_OK;
}

static void free_rows(struct ptab *p)
{
	struct ptab_row *cur, *next;
	int i;

	cur = p->internal->rows_head;
	while (cur) {
		next = cur->next;

		for (i = 0; i < p->internal->num_columns; i++)
			internal_free(p, cur->strings[i]);

		internal_free(p, cur);
		cur = next;
	}

	p->internal->rows_head = NULL;
	p->internal->rows_tail = NULL;
}


/* API functions */

const char *ptab_version_string(void)
{
	static const char version_string[] = PTAB_VERSION_STRING;

	return version_string;
}

void ptab_version(int *major, int *minor, int *patch)
{
	*major = PTAB_VERSION_MAJOR;
	*minor = PTAB_VERSION_MINOR;
	*patch = PTAB_VERSION_PATCH;
}

int ptab_init(struct ptab *p, const struct ptab_allocator *a)
{
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
	p->allocator_stats.total = 0;
	p->allocator_stats.allocations = 0;
	p->allocator_stats.frees = 0;

	/* allocate the library's internal structure */
	p->internal = internal_alloc(p, sizeof(struct ptab_internal));
	if (!p->internal)
		return PTAB_ENOMEM;

	/* initialize internals */
	p->internal->state = PTAB_STATE_INITIALIZED;
	p->internal->columns_head = NULL;
	p->internal->columns_tail = NULL;
	p->internal->rows_head = NULL;
	p->internal->rows_tail = NULL;
	p->internal->num_columns = 0;
	p->internal->num_rows = 0;

	return PTAB_OK;
}

int ptab_free(struct ptab *p)
{
	if (!p)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EORDER;

	free_columns(p);
	free_rows(p);

	internal_free(p, p->internal);
	p->internal = NULL;

	return PTAB_OK;
}

int ptab_begin_columns(struct ptab *p)
{
	if (!p)
		return PTAB_ENULL;

	if (!p->internal || p->internal->state != PTAB_STATE_INITIALIZED)
		return PTAB_EORDER;

	p->internal->state = PTAB_STATE_DEFINING_COLUMNS;

	return PTAB_OK;
}

int ptab_define_column(struct ptab *p,
		       const char *name,
		       const char *fmt,
		       int flags)
{
	int type;
	int align;
	int default_align;
	int requires_fmt;

	if (!p || !name)
		return PTAB_ENULL;

	if (!p->internal || p->internal->state != PTAB_STATE_DEFINING_COLUMNS)
		return PTAB_EORDER;

	type = flags & (PTAB_INTEGER | PTAB_FLOAT | PTAB_STRING);
	switch (type) {
	case PTAB_INTEGER:
		default_align = PTAB_ALIGN_RIGHT;
		requires_fmt = 1;
		break;

	case PTAB_FLOAT:
		default_align = PTAB_ALIGN_RIGHT;
		requires_fmt = 1;
		break;

	case PTAB_STRING:
		default_align = PTAB_ALIGN_LEFT;
		requires_fmt = 0;
		break;

	default:
		return PTAB_ETYPE;
	}

	if (!fmt && requires_fmt)
		return PTAB_ENULL;

	align = flags & (PTAB_ALIGN_LEFT | PTAB_ALIGN_RIGHT);
	if ((align & PTAB_ALIGN_LEFT) && (align & PTAB_ALIGN_RIGHT))
		return PTAB_EALIGN;

	if (!align)
		align = default_align;

	return add_column(p, name, fmt, type, align);
}

int ptab_end_columns(struct ptab *p)
{
	if (!p)
		return PTAB_ENULL;

	if (!p->internal || p->internal->state != PTAB_STATE_DEFINING_COLUMNS)
		return PTAB_EORDER;

	if (p->internal->num_columns == 0)
		return PTAB_ENOCOLUMNS;

	p->internal->state = PTAB_STATE_DEFINED_COLUMNS;

	return PTAB_OK;
}

int ptab_begin_row(struct ptab *p)
{
	int err;

	if (!p)
		return PTAB_ENULL;

	if (!p->internal || p->internal->state != PTAB_STATE_DEFINED_COLUMNS)
		return PTAB_EORDER;

	err = add_row(p);
	if (err)
		return err;

	p->internal->state = PTAB_STATE_ADDING_ROW;
	p->internal->current_row = p->internal->rows_tail;
	p->internal->current_column = p->internal->columns_head;
	p->internal->current_column_num = 0;

	return PTAB_OK;
}

int ptab_add_row_data_s(struct ptab *p, const char *val)
{
	struct ptab_row *row;
	struct ptab_column *column;
	int column_num;
	char *str;
	size_t len;

	if (!p || !val)
		return PTAB_ENULL;

	if (!p->internal || p->internal->state != PTAB_STATE_ADDING_ROW)
		return PTAB_EORDER;

	row = p->internal->current_row;
	column = p->internal->current_column;
	column_num = p->internal->current_column_num;

	if (!column || column_num >= p->internal->num_columns)
		return PTAB_ENUMCOLUMNS;

	if (column->type != PTAB_STRING)
		return PTAB_ETYPE;

	len = strlen(val);
	str = internal_alloc(p, len + 1);
	if (!str)
		return PTAB_ENOMEM;

	strcpy(str, val);

	row->data[column_num].s = str;
	row->strings[column_num] = str;
	row->lengths[column_num] = len;

	p->internal->current_column = column->next;
	p->internal->current_column_num++;

	return PTAB_OK;
}

int ptab_add_row_data_i(struct ptab *p, int val)
{
	struct ptab_row *row;
	struct ptab_column *column;
	int column_num;
	static const int BUF_SIZE = 128;
	char buf[BUF_SIZE];
	char *str;
	size_t len;

	if (!p)
		return PTAB_ENULL;

	if (!p->internal || p->internal->state != PTAB_STATE_ADDING_ROW)
		return PTAB_EORDER;

	row = p->internal->current_row;
	column = p->internal->current_column;
	column_num = p->internal->current_column_num;

	if (!column || column_num >= p->internal->num_columns)
		return PTAB_ENUMCOLUMNS;

	if (column->type != PTAB_INTEGER)
		return PTAB_ETYPE;

	len = (size_t)snprintf(buf, BUF_SIZE, column->fmt, val);
	str = internal_alloc(p, len + 1);
	if (!str)
		return PTAB_ENOMEM;

	strcpy(str, buf);

	row->data[column_num].i = val;
	row->strings[column_num] = str;
	row->lengths[column_num] = len;

	p->internal->current_column = column->next;
	p->internal->current_column_num++;

	return PTAB_OK;
}

int ptab_add_row_data_f(struct ptab *p, float val)
{
	struct ptab_row *row;
	struct ptab_column *column;
	int column_num;
	static const int BUF_SIZE = 128;
	char buf[BUF_SIZE];
	char *str;
	size_t len;

	if (!p)
		return PTAB_ENULL;

	if (!p->internal || p->internal->state != PTAB_STATE_ADDING_ROW)
		return PTAB_EORDER;

	row = p->internal->current_row;
	column = p->internal->current_column;
	column_num = p->internal->current_column_num;

	if (!column || column_num >= p->internal->num_columns)
		return PTAB_ENUMCOLUMNS;

	if (column->type != PTAB_FLOAT)
		return PTAB_ETYPE;

	len = (size_t)snprintf(buf, BUF_SIZE, column->fmt, val);
	str = internal_alloc(p, len + 1);
	if (!str)
		return PTAB_ENOMEM;

	strcpy(str, buf);

	row->data[column_num].i = val;
	row->strings[column_num] = str;
	row->lengths[column_num] = len;

	p->internal->current_column = column->next;
	p->internal->current_column_num++;

	return PTAB_OK;
}
