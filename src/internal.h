#ifndef INTERNAL_H
#define INTERNAL_H

#include <ptab.h>

struct ptab_column {
	int flags;
	size_t width;
	size_t name_len;
	const char name[];
};

struct ptab_row {
	char **column_data;
	size_t *column_len;
	char data[];
};

struct ptab_internal {
	struct ptab_column *columns;
	struct ptab_row *rows;
	int num_columns;
	int num_rows;
};

#endif
