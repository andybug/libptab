
#include <ptab.h>
#include "internal.h"

int ptab_column(ptab *p, const char *name, int flags)
{
	int type;
	int align;

	if (!p || !name)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	type = (PTAB_STRING | PTAB_FLOAT | PTAB_INTEGER) & flags;
	switch (type) {
	case PTAB_STRING:
		break;

	case PTAB_FLOAT:
		break;

	case PTAB_INTEGER:
		break;

	default:
		return PTAB_ENUMTYPE;
	}

	align = (PTAB_ALIGN_LEFT | PTAB_ALIGN_RIGHT) & flags;
	switch (align) {
	case 0:
		break;

	case PTAB_ALIGN_LEFT:
		break;

	case PTAB_ALIGN_RIGHT:
		break;

	default:
		return PTAB_ENUMALIGN;
	}

	return PTAB_OK;
}
