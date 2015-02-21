
#include <assert.h>
#include <string.h>

#include <ptab.h>
#include "internal.h"

static int get_type(int flags)
{
	int masked;
	int type = -1;

	/* mask out other flags */
	masked = (PTAB_STRING | PTAB_FLOAT | PTAB_INTEGER) & flags;

	/*
	 * if only one type is set in the masked value, then set type to that
	 * value. Otherwise, if no type was given or more than one was,
	 * set type to -1
	 */
	switch (masked) {
	case PTAB_STRING:
	case PTAB_FLOAT:
	case PTAB_INTEGER:
		type = masked;
		break;

	default:
		type = -1;
	}

	return type;
}

static int get_align(int type, int flags)
{
	int masked;
	int align = 0;

	assert(type > 0);

	/* only want to see the alignment flags */
	masked = (PTAB_ALIGN_LEFT | PTAB_ALIGN_RIGHT) & flags;

	/*
	 * if only one alignment was specified, set align to that value
	 * if none were given, use the default alignment for that type
	 * if more than one, error
	 */
	switch (masked) {
	case PTAB_ALIGN_LEFT:
	case PTAB_ALIGN_RIGHT:
		align = masked;
		break;

	case 0:
		/*
		 * the alignment was not specified,
		 * use the default alignment for that type
		 */
		switch (type) {
		case PTAB_STRING:
			align = PTAB_ALIGN_LEFT;
			break;

		case PTAB_FLOAT:
		case PTAB_INTEGER:
			align = PTAB_ALIGN_RIGHT;
			break;
		}
		break;

	default:
		align = -1;
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

static int add_column(ptab_t *p, const char *name, int type, int align)
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
	col = ptab_alloc(p, sizeof(struct ptab_col) + len + 1);
	if (!col)
		return PTAB_ENOMEM;

	/* name immediately follows ptab_col in memory */
	col->name = (char*)(col + 1);
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

int ptab_column(ptab_t *p, const char *name, int flags)
{
	int type;
	int align;

	if (!p || !name)
		return PTAB_ENULL;

	if (p->num_rows > 0)
		return PTAB_EROWS;

	/* get the column type from the flags */
	type = get_type(flags);
	if (type < 0)
		return PTAB_ETYPEFLAGS;

	/* get the column alignment from the flags */
	align = get_align(type, flags);
	if (align < 0)
		return PTAB_EALIGNFLAGS;

	/* allocate the column and add it to the columns list */
	return add_column(p, name, type, align);
}
