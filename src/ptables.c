
#include <stdbool.h>
#include <ptables.h>

const char *ptables_version(void)
{
	static const char version_string[] = PTABLES_VERSION_STRING;

	return version_string;
}

int ptable_init(struct ptable *p, int flags)
{
	bool use_buffer = flags & PTABLES_USE_BUFFER;
	bool use_allocator = flags & PTABLES_USE_ALLOCATOR;

	if (use_buffer && use_allocator)
		return PTABLES_ERR_ONE_ALLOCATOR;

	return PTABLES_OK;
}
