
#include <stdio.h>

#include <ptab.h>
#include "internal.h"

struct io_char {
	const char *c;
	unsigned int len;
};

struct format_desc {
	struct io_char horiz_div;
	struct io_char vert_div;
	struct io_char top_left_intersect;
	struct io_char top_middle_intersect;
	struct io_char top_right_intersect;
	struct io_char div_left_intersect;
	struct io_char div_middle_intersect;
	struct io_char div_right_intersect;
	struct io_char bottom_left_intersect;
	struct io_char bottom_middle_intersect;
	struct io_char bottom_right_intersect;
};

union io_stream {
	FILE *f;
	ptab_stream_t *s;
};

struct io_vtable {
	size_t (*write)(const char*, size_t, union io_stream);
	size_t (*write_char)(const struct io_char*, union io_stream);
};

/*
 * Format descriptors
 */

static const struct format_desc ascii_format = {
	.horiz_div = {"-", 1},
	.vert_div = {"|", 1},
	.top_left_intersect = {"+", 1},
	.top_middle_intersect = {"+", 1},
	.top_right_intersect = {"+", 1},
	.div_left_intersect = {"+", 1},
	.div_middle_intersect = {"+", 1},
	.div_right_intersect = {"+", 1},
	.bottom_left_intersect = {"+", 1},
	.bottom_middle_intersect = {"+", 1},
	.bottom_right_intersect = {"+", 1}
};

/*
 * Generic table writing
 */

static int write_repeat(
		const char *s,
		size_t len,
		size_t num,
		const struct io_vtable *vtable,
		union io_stream stream)
{
	size_t i;

	for (i = 0; i < num; i++)
		vtable->write(s, len, stream);
}

static int write_repeat_char(
		const struct io_char *c,
		size_t num,
		const struct io_vtable *vtable,
		union io_stream stream)
{
	size_t i;

	for (i = 0; i < num; i++)
		vtable->write_char(c, stream);
}

static int write_row_top(
		const ptab *p,
		const struct format_desc *desc,
		const struct io_vtable *vtable,
		union io_stream stream)
{
	const struct ptab_col *col = p->internal->columns_head;

	vtable->write_char(&desc->top_left_intersect, stream);
	vtable->write_char(&desc->horiz_div, stream);

	while (col) {
		write_repeat_char(
			&desc->horiz_div,
			col->width,
			vtable,
			stream);

		if (col->next) {
			vtable->write_char(&desc->horiz_div, stream);
			vtable->write_char(
					&desc->top_middle_intersect,
					stream);
			vtable->write_char(&desc->horiz_div, stream);
		}

		col = col->next;
	}

	vtable->write_char(&desc->horiz_div, stream);
	vtable->write_char(&desc->top_right_intersect, stream);
	vtable->write("\n", 1, stream);
}

static int write_row_heading(
		const ptab *p,
		const struct format_desc *desc,
		const struct io_vtable *vtable,
		union io_stream stream)
{
	const struct ptab_col *col = p->internal->columns_head;
	size_t padding;

	vtable->write_char(&desc->vert_div, stream);
	vtable->write(" ", 1, stream);

	while (col) {
		padding = col->width - col->name_len;

		vtable->write(col->name, col->name_len, stream);
		write_repeat(" ", 1, padding, vtable, stream);

		if (col->next) {
			vtable->write(" ", 1, stream);
			vtable->write_char(&desc->vert_div, stream);
			vtable->write(" ", 1, stream);
		}

		col = col->next;
	}

	vtable->write(" ", 1, stream);
	vtable->write_char(&desc->vert_div, stream);
	vtable->write("\n", 1, stream);
}

static int write_table(
		const ptab *p,
		const struct format_desc *desc,
		const struct io_vtable *vtable,
		union io_stream stream)
{
	write_row_top(p, desc, vtable, stream);
	write_row_heading(p, desc, vtable, stream);

	return PTAB_OK;
}

/*
 * File-stream specific functions
 */

static size_t file_write(
		const char *str,
		size_t len,
		union io_stream stream)
{
	return fwrite(str, 1, len, stream.f);
}

static size_t file_write_char(
		const struct io_char *c,
		union io_stream stream)
{
	return fwrite(c->c, 1, c->len, stream.f);
}

int ptab_dumpf(ptab *p, FILE *f, int flags)
{
	const struct format_desc *desc = &ascii_format;
	struct io_vtable vtable;
	union io_stream stream;

	/* setup vtable with file functions */
	vtable.write = file_write;
	vtable.write_char = file_write_char;

	/* set the stream to the file handle */
	stream.f = f;

	write_table(p, desc, &vtable, stream);

	return PTAB_OK;
}
