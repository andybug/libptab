
#include <ptables.h>

const char *ptables_version(void)
{
	static const char version_string[] = PTABLES_VERSION_STRING;

	return version_string;
}

int ptable_init(struct ptable *p, int flags)
{
	return PTABLES_OK;
}
