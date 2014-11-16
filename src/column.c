
#include <assert.h>

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

int ptab_column(ptab *p, const char *name, int flags)
{
	int type;
	int align;

	if (!p || !name)
		return PTAB_ENULL;

	if (!p->internal)
		return PTAB_EINIT;

	/* FIXME: check if num_rows > 0 */

	/* get the column type from the flags */
	type = get_type(flags);
	if (type < 0)
		return PTAB_ENUMTYPE;

	/* get the column alignment from the flags */
	align = get_align(type, flags);
	if (align < 0)
		return PTAB_ENUMALIGN;

	return PTAB_OK;
}
