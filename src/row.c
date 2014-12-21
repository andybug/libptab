
#include <string.h>

#include <ptab.h>
#include "internal.h"

static void add_to_row_list(ptab *p, struct ptab_row *r)
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

int ptab_begin_row(ptab *p)
{
	struct ptab_row *row;
	size_t alloc_size;

	if (!p)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	if (p->internal->num_columns == 0)
		return PTAB_ENOCOLUMNS;

	if (p->internal->current_row)
		return PTAB_EROWBEGAN;

	/*
	 * allocate the row structure and all of the variable-data
	 * arrays. in memory it looks like this:
	 * [ row ][ data][ strings][ lengths ]
	 */
	alloc_size = sizeof(struct ptab_row) + (p->internal->num_columns *
						(sizeof(union ptab_row_data) +
						 sizeof(char*) +
						 sizeof(size_t)));

	row = ptab_alloc(p, alloc_size);
	if (!row)
		return PTAB_ENOMEM;

	/* initialize the row structure */
	row->data = (union ptab_row_data*)(row + 1);
	row->strings = (char**)(row->data + p->internal->num_columns);
	row->lengths = (size_t*)(row->strings + p->internal->num_columns);
	row->next = NULL;

	p->internal->current_row = row;
	p->internal->current_column = p->internal->columns_head;

	return PTAB_OK;
}

int ptab_row_data_s(ptab *p, const char *s)
{
	struct ptab_row *row;
	struct ptab_col *column;
	char *str;
	size_t len;

	if (!p || !s)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	row = p->internal->current_row;
	column = p->internal->current_column;

	if (!column || column->id >= p->internal->num_columns)
		return PTAB_ENUMCOLUMNS;

	if (column->type != PTAB_STRING)
		return PTAB_ETYPE;

	len = strlen(s);
	str = ptab_alloc(p, len + 1);
	if (!str)
		return PTAB_ENOMEM;

	strcpy(str, s);

	row->data[column->id].s = str;
	row->strings[column->id] = str;
	row->lengths[column->id] = len;

	if (len > column->width)
		column->width = len;

	p->internal->current_column = column->next;

	return PTAB_OK;
}

int ptab_row_data_i(ptab *p, const char *format, int i)
{
	struct ptab_row *row;
	struct ptab_col *column;
	static const int BUF_SIZE = 128;
	char buf[BUF_SIZE];
	char *str;
	size_t len;

	if (!p || !format)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	row = p->internal->current_row;
	column = p->internal->current_column;

	if (!column || column->id >= p->internal->num_columns)
		return PTAB_ENUMCOLUMNS;

	if (column->type != PTAB_INTEGER)
		return PTAB_ETYPE;

	len = (size_t)snprintf(buf, BUF_SIZE, format, i);
	str = ptab_alloc(p, len + 1);
	if (!str)
		return PTAB_ENOMEM;

	strcpy(str, buf);

	row->data[column->id].i = i;
	row->strings[column->id] = str;
	row->lengths[column->id] = len;

	if (len > column->width)
		column->width = len;

	p->internal->current_column = column->next;

	return PTAB_OK;
}

int ptab_row_data_f(ptab *p, const char *format, float f)
{
	struct ptab_row *row;
	struct ptab_col *column;
	static const int BUF_SIZE = 128;
	char buf[BUF_SIZE];
	char *str;
	size_t len;

	if (!p || !format)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	row = p->internal->current_row;
	column = p->internal->current_column;

	if (!column || column->id >= p->internal->num_columns)
		return PTAB_ENUMCOLUMNS;

	if (column->type != PTAB_FLOAT)
		return PTAB_ETYPE;

	len = (size_t)snprintf(buf, BUF_SIZE, format, f);
	str = ptab_alloc(p, len + 1);
	if (!str)
		return PTAB_ENOMEM;

	strcpy(str, buf);

	row->data[column->id].f = f;
	row->strings[column->id] = str;
	row->lengths[column->id] = len;

	if (len > column->width)
		column->width = len;

	p->internal->current_column = column->next;

	return PTAB_OK;
}

int ptab_end_row(ptab *p)
{
	if (!p)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	if (!p->internal->current_row)
		return PTAB_ENOROWBEGAN;

	/*
	 * current_column should be null after the last
	 * row value is set (current_column = column->next)
	 */
	if (p->internal->current_column)
		return PTAB_ENUMCOLUMNS;

	add_to_row_list(p, p->internal->current_row);

	p->internal->current_row = NULL;
	p->internal->current_column = NULL;

	return PTAB_OK;
}
