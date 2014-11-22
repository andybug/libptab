
#include <ptab.h>
#include "internal.h"

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

	return PTAB_OK;
}
