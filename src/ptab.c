
#include <ptab.h>

/* API functions */

const char *ptab_version_string(void)
{
	static const char version_string[] = PTAB_VERSION_STRING;

	return version_string;
}

void ptab_version(int *major, int *minor, int *patch)
{
	*major = PTAB_VERSION_MAJOR;
	*minor = PTAB_VERSION_MINOR;
	*patch = PTAB_VERSION_PATCH;
}

int ptab_init(struct ptab *p, const struct ptab_allocator *a)
{
	if (p == NULL || a == NULL)
		return PTAB_ENULL;

	if (a->alloc_func == NULL || a->free_func == NULL)
		return PTAB_ENULL;

	p->num_columns = 0;
	p->num_rows = 0;

	p->columns = NULL;
	p->rows = NULL;

	p->allocator = *a;
	p->allocator_stats.total = 0;
	p->allocator_stats.high = 0;
	p->allocator_stats.current = 0;
	p->allocator_stats.allocations = 0;
	p->allocator_stats.frees = 0;

	return PTAB_OK;
}
