
#include <stdio.h>
#include <stdlib.h>

#include <ptab.h>

int main(void)
{
	ptab table;
	int major, minor, patch;
	int err;

	ptab_version(&major, &minor, &patch);
	if (major != PTAB_VERSION_MAJOR) {
		fprintf(stderr, "libptab version mismatch:\n");
		fprintf(stderr, "\tcompiled against: %s\n", PTAB_VERSION_STRING);
		fprintf(stderr, "\tlinked with:      %s\n", ptab_version_string());
		return EXIT_FAILURE;
	}

	err = ptab_init(&table, NULL);
	if (err != PTAB_OK) {
		fprintf(stderr, "error: ptab_init: %s\n", ptab_strerror(err));
		return EXIT_FAILURE;
	}

	ptab_free(&table);

	return EXIT_SUCCESS;
}
