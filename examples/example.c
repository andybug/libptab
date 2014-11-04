
#include <stdio.h>
#include <stdlib.h>

#include <ptab.h>

int main(void)
{
	int major, minor, patch;
	int err;

	ptab_version(&major, &minor, &patch);
	if (major != PTAB_VERSION_MAJOR) {
		fprintf(stderr, "libptab version mismatch:\n");
		fprintf(stderr, "\tcompiled against: %s\n", PTAB_VERSION_STRING);
		fprintf(stderr, "\tlinked with:      %s\n", ptab_version_string());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
