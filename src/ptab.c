
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
	return PTAB_OK;
}
