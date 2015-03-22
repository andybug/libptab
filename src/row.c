
#include <string.h>

#include <ptab.h>
#include "internal.h"

static void add_to_row_list(ptab_t *p, struct ptab_row *r)
{
	if (p->rows_tail) {
		p->rows_tail->next = r;
		p->rows_tail = r;
		r->next = NULL;
	} else {
		p->rows_head = r;
		p->rows_tail = r;
		r->next = NULL;
	}

	p->num_rows++;
}

int ptab_begin_row(ptab_t *p)
{
	struct ptab_row *row;
	size_t alloc_size;

	if (!p)
		return PTAB_ENULL;

	if (p->num_columns == 0)
		return PTAB_EORDER;

	if (p->current_row)
		return PTAB_EORDER;

	/*
	 * allocate the row structure and all of the variable-data
	 * arrays. in memory it looks like this:
	 * [ row ][ data][ strings][ lengths ]
	 */
	alloc_size = sizeof(struct ptab_row) + (p->num_columns *
						(sizeof(union ptab_row_data) +
						 sizeof(char*) +
						 sizeof(size_t)));

	row = mem_alloc(p, alloc_size);
	if (!row)
		return PTAB_EMEM;

	/* initialize the row structure */
	row->data = (union ptab_row_data*)(row + 1);
	row->strings = (char**)(row->data + p->num_columns);
	row->lengths = (size_t*)(row->strings + p->num_columns);
	row->next = NULL;

	p->current_row = row;
	p->current_column = p->columns_head;

	return PTAB_OK;
}

int ptab_row_data_s(ptab_t *p, const char *s)
{
	struct ptab_row *row;
	struct ptab_col *column;
	char *str;
	size_t len;

	if (!p || !s)
		return PTAB_ENULL;

	row = p->current_row;
	column = p->current_column;

	if (!column || column->id >= p->num_columns)
		return PTAB_ECOLUMNS;

	if (column->type != PTAB_STRING)
		return PTAB_ETYPE;

	len = strlen(s);
	str = mem_alloc(p, len + 1);
	if (!str)
		return PTAB_EMEM;

	strcpy(str, s);

	row->data[column->id].s = str;
	row->strings[column->id] = str;
	row->lengths[column->id] = len;

	if (len > column->width)
		column->width = len;

	p->current_column = column->next;

	return PTAB_OK;
}

int ptab_row_data_i(ptab_t *p, const char *format, int i)
{
	struct ptab_row *row;
	struct ptab_col *column;
	static const int BUF_SIZE = 128;
	char buf[BUF_SIZE];
	char *str;
	size_t len;

	if (!p || !format)
		return PTAB_ENULL;

	row = p->current_row;
	column = p->current_column;

	if (!column || column->id >= p->num_columns)
		return PTAB_ECOLUMNS;

	if (column->type != PTAB_INTEGER)
		return PTAB_ETYPE;

	len = (size_t)snprintf(buf, BUF_SIZE, format, i);
	str = mem_alloc(p, len + 1);
	if (!str)
		return PTAB_EMEM;

	strcpy(str, buf);

	row->data[column->id].i = i;
	row->strings[column->id] = str;
	row->lengths[column->id] = len;

	if (len > column->width)
		column->width = len;

	p->current_column = column->next;

	return PTAB_OK;
}

int ptab_row_data_f(ptab_t *p, const char *format, float f)
{
	struct ptab_row *row;
	struct ptab_col *column;
	static const int BUF_SIZE = 128;
	char buf[BUF_SIZE];
	char *str;
	size_t len;

	if (!p || !format)
		return PTAB_ENULL;

	row = p->current_row;
	column = p->current_column;

	if (!column || column->id >= p->num_columns)
		return PTAB_ECOLUMNS;

	if (column->type != PTAB_FLOAT)
		return PTAB_ETYPE;

	len = (size_t)snprintf(buf, BUF_SIZE, format, f);
	str = mem_alloc(p, len + 1);
	if (!str)
		return PTAB_EMEM;

	strcpy(str, buf);

	row->data[column->id].f = f;
	row->strings[column->id] = str;
	row->lengths[column->id] = len;

	if (len > column->width)
		column->width = len;

	p->current_column = column->next;

	return PTAB_OK;
}

int ptab_end_row(ptab_t *p)
{
	if (!p)
		return PTAB_ENULL;

	if (!p->current_row)
		return PTAB_EORDER;

	/*
	 * current_column should be null after the last
	 * row value is set (current_column = column->next)
	 */
	if (p->current_column)
		return PTAB_ECOLUMNS;

	add_to_row_list(p, p->current_row);

	p->current_row = NULL;
	p->current_column = NULL;

	return PTAB_OK;
}
