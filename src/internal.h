#ifndef INTERNAL_H
#define INTERNAL_H

#include <ptab.h>

enum ptab_state {
	PTAB_STATE_INITIALIZED,
	PTAB_STATE_DEFINING_COLUMNS,
	PTAB_STATE_DEFINED_COLUMNS,
	PTAB_STATE_ADDING_ROW,
	PTAB_STATE_FINISHED_ROW
};

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
	enum ptab_state state;
	struct ptab_column *columns;
	struct ptab_row *rows;
	int num_columns;
	int num_rows;
};

#endif
