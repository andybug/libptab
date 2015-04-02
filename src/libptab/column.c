
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include <ptab.h>
#include "internal.h"

static bool check_type(enum ptab_type type)
{
	bool is_good = false;

	/*
	 * switch on type, ensure that the value is one
	 * of the valid enum values
	 */
	switch (type) {
	case PTAB_STRING:
	case PTAB_INTEGER:
	case PTAB_FLOAT:
		is_good = true;
		break;

	default:
		is_good = false;
	}

	return is_good;
}

static bool check_align(enum ptab_align align)
{
	bool is_good = false;

	/*
	 * switch on align, ensure that the value is one
	 * of the valid enum values
	 */
	switch (align) {
	case PTAB_LEFT:
	case PTAB_RIGHT:
		is_good = true;
		break;

	case PTAB_CENTER:
	/* center is not implemented yet */
	default:
		is_good = false;
	}

	return is_good;
}

static enum ptab_align get_default_align(enum ptab_type type)
{
	enum ptab_align align = PTAB_LEFT;

	/*
	 * get the default alignment for each of the
	 * valid ptab_type types
	 */
	switch (type) {
	case PTAB_STRING:
		align = PTAB_LEFT;
		break;

	case PTAB_INTEGER:
	case PTAB_FLOAT:
		align = PTAB_RIGHT;
		break;
	}

	return align;
}

static void add_to_column_list(ptab_t *p, struct ptab_col *c)
{
	if (p->columns_tail) {
		c->id = p->columns_tail->id + 1;
		p->columns_tail->next = c;
		p->columns_tail = c;
		c->next = NULL;
	} else {
		c->id = 0;
		p->columns_head = c;
		p->columns_tail = c;
		c->next = NULL;
	}

	p->num_columns++;
}

static int add_column(ptab_t *p,
		      const char *name,
		      enum ptab_type type,
		      enum ptab_align align)
{
	struct ptab_col *col;
	size_t len;

	/*
	 * find how much we need to allocate to store the name;
	 * this will also be used as the starting width of
	 * the column
	 */
	len = strlen(name);

	/*
	 * allocate the column structure and the buffer for the name
	 * string in a single allocation
	 */
	col = ptab__mem_alloc(p, sizeof(struct ptab_col) + len + 1);
	if (!col)
		return PTAB_EMEM;

	/* name immediately follows ptab_col in memory */
	col->name = (char *)(col + 1);
	strcpy(col->name, name);

	/* initialize the column structure */
	col->id = 0;
	col->type = type;
	col->align = align;
	col->name_len = len;
	col->width = len;
	col->next = NULL;

	/* finally, add it to the list */
	add_to_column_list(p, col);

	return PTAB_OK;
}

int ptab_column(ptab_t *p, const char *name, enum ptab_type type)
{
	enum ptab_align align;

	if (!p || !name)
		return PTAB_ENULL;

	if (p->num_rows > 0 || p->current_row)
		return PTAB_EORDER;

	/* ensure type is valid */
	if (!check_type(type))
		return PTAB_ETYPE;

	/* get the column alignment from the type */
	align = get_default_align(type);

	/* allocate the column and add it to the columns list */
	return add_column(p, name, type, align);
}

int ptab_column_align(ptab_t *p, unsigned int col, enum ptab_align align)
{
	if (!p)
		return PTAB_ENULL;

	if (col >= p->num_columns)
		return PTAB_ERANGE;

	if (!check_align(align))
		return PTAB_EALIGN;

	/* find the column that matches the id */
	struct ptab_col *column = p->columns_head;
	while (column) {
		if (column->id == col) {
			column->align = align;
			break;
		}

		column = column->next;
	}

	/* it should never fail to find a column */
	assert(column != NULL);

	return PTAB_OK;
}
