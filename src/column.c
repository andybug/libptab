
#include <assert.h>

#include <ptab.h>
#include "internal.h"

static int get_type(int flags)
{
	int masked;
	int type = -1;

	masked = (PTAB_STRING | PTAB_FLOAT | PTAB_INTEGER) & flags;

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

	masked = (PTAB_ALIGN_LEFT | PTAB_ALIGN_RIGHT) & flags;

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

int ptab_column(ptab *p, const char *name, int flags)
{
	int type;
	int align;

	if (!p || !name)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	/* FIXME: check if num_rows > 0 */

	type = get_type(flags);
	if (type < 0)
		return PTAB_ENUMTYPE;

	align = get_align(type, flags);
	if (align < 0)
		return PTAB_ENUMALIGN;

	return PTAB_OK;
}
