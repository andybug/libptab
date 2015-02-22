
#include "internal.h"

int ptab_free(ptab_t *p)
{
	if (!p)
		return PTAB_ENULL;

	mem_free(p);

	return PTAB_OK;
}
