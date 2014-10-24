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
	char *name;
	size_t name_len;
	char *fmt;
	int type;
	int align;
	size_t width;
	struct ptab_column *next;
};

union ptab_row_data {
	char *s;
	int i;
	double f;
};

struct ptab_row {
	union ptab_row_data *data;
	char **strings;
	size_t *lengths;
	struct ptab_row *next;
};

struct ptab_internal {
	enum ptab_state state;
	struct ptab_column *columns_head;
	struct ptab_column *columns_tail;
	struct ptab_row *rows_head;
	struct ptab_row *rows_tail;
	int num_columns;
	int num_rows;
	struct ptab_row *current_row;
	struct ptab_column *current_column;
	int current_column_num;
};

#endif
