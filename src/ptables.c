
#include <ptables.h>

int ptables_check_version(int version)
{
	/* this is a temporary function - FIXME remove */
	if (version == PTABLES_VERSION_NUMBER)
		return 0;

	return -1;
}
