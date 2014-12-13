
#include <stdio.h>
#include <ptab.h>

#include "internal.h"
#include "output.h"

static void write_repeat(const char *s, size_t n, FILE *f)
{
	size_t i;

	for (i = 0; i < n; i++)
		fprintf(f, "%s", s);
}

void write_row_top(ptab *p, const struct format_desc *desc, FILE *f)
{
	struct ptab_col *col;

	fprintf(f, "%s%s",
		desc->top_left_intersect,
		desc->horiz_div);

	col = p->internal->columns_head;

	while (col) {
		write_repeat(desc->horiz_div, col->width, f);

		if (col->next) {
			fprintf(f, "%s%s%s",
				desc->horiz_div,
				desc->top_middle_intersect,
				desc->horiz_div);
		}

		col = col->next;
	}

	fprintf(f, "%s%s\n",
		desc->horiz_div,
		desc->top_right_intersect);
}

void write_row_heading(ptab *p, const struct format_desc *desc, FILE *f)
{
	struct ptab_col *col;
	size_t padding;

	fprintf(f, "%s ", desc->vert_div);

	col = p->internal->columns_head;

	while (col) {
		fprintf(f, "%s", col->name);

		padding = col->width - col->name_len;
		write_repeat(" ", padding, f);

		if (col->next)
			fprintf(f, " %s ", desc->vert_div);

		col = col->next;
	}

	fprintf(f, " %s\n", desc->vert_div);
}

void write_row_divider(ptab *p, const struct format_desc *desc, FILE *f)
{
	struct ptab_col *col;

	fprintf(f, "%s%s",
		desc->div_left_intersect,
		desc->horiz_div);

	col = p->internal->columns_head;

	while (col) {
		write_repeat(desc->horiz_div, col->width, f);

		if (col->next) {
			fprintf(f, "%s%s%s",
				desc->horiz_div,
				desc->div_middle_intersect,
				desc->horiz_div);
		}

		col = col->next;
	}

	fprintf(f, "%s%s\n",
		desc->horiz_div,
		desc->div_right_intersect);
}

void write_row_data(ptab *p,
		struct ptab_row *row,
		const struct format_desc *desc,
		FILE *f)
{
	struct ptab_col *col;
	size_t padding;

	fprintf(f, "%s ", desc->vert_div);

	col = p->internal->columns_head;

	while (col) {
		fprintf(f, "%s", row->strings[col->id]);

		padding = col->width - row->lengths[col->id];
		write_repeat(" ", padding, f);

		if (col->next)
			fprintf(f, " %s ", desc->vert_div);

		col = col->next;
	}

	fprintf(f, " %s\n", desc->vert_div);
}

void write_row_bottom(ptab *p, const struct format_desc *desc, FILE *f)
{
	struct ptab_col *col;

	fprintf(f, "%s%s",
		desc->bottom_left_intersect,
		desc->horiz_div);

	col = p->internal->columns_head;

	while (col) {
		write_repeat(desc->horiz_div, col->width, f);

		if (col->next) {
			fprintf(f, "%s%s%s",
				desc->horiz_div,
				desc->bottom_middle_intersect,
				desc->horiz_div);
		}

		col = col->next;
	}

	fprintf(f, "%s%s\n",
		desc->horiz_div,
		desc->bottom_right_intersect);
}

int ptab_dumpf(ptab *p, FILE *f, int flags)
{
	const struct format_desc *desc = &ascii_format;
	struct ptab_row *row = p->internal->rows_head;

	write_row_top(p, desc, f);
	write_row_heading(p, desc, f);
	write_row_divider(p, desc, f);

	while (row) {
		write_row_data(p, row, desc, f);
		row = row->next;
	}

	write_row_bottom(p, desc, f);

	return PTAB_OK;
}
