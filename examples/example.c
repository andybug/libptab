
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ptab.h>

int main(void)
{
	ptab table;
	int major, minor, patch;
	int err;

	memset(&table, 0, sizeof(ptab));

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

	ptab_column(&table, "Team", PTAB_STRING);
	ptab_column(&table, "Wins", PTAB_INTEGER);
	ptab_column(&table, "Losses", PTAB_INTEGER);
	ptab_column(&table, "Percent", PTAB_FLOAT);

	ptab_free(&table);

	return EXIT_SUCCESS;
}
