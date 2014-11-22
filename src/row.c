
#include <ptab.h>
#include "internal.h"

int ptab_begin_row(ptab *p)
{
	struct ptab_row *row;

	if (!p)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	if (p->internal->num_columns == 0)
		return PTAB_ENOCOLUMNS;

	if (p->internal->current_row)
		return PTAB_EROWBEGAN;

	row = ptab_alloc(p, sizeof(struct ptab_row));
	if (!row)
		return PTAB_ENOMEM;

	/* FIXME: initialize row memebers */

	p->internal->current_row = row;

	return PTAB_OK;
}
