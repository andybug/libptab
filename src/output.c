
#include <stdio.h>
#include <string.h>

#include <ptab.h>
#include "internal.h"

typedef struct utf8_char {
	const char *c;
	unsigned int len;
} utf8_char_t;

struct format_desc {
	utf8_char_t horiz_div;
	utf8_char_t vert_div;
	utf8_char_t top_left_intersect;
	utf8_char_t top_middle_intersect;
	utf8_char_t top_right_intersect;
	utf8_char_t div_left_intersect;
	utf8_char_t div_middle_intersect;
	utf8_char_t div_right_intersect;
	utf8_char_t bottom_left_intersect;
	utf8_char_t bottom_middle_intersect;
	utf8_char_t bottom_right_intersect;
};

struct strbuf {
	char *buf;
	size_t size;
	size_t used;
	size_t avail;
};

union io_stream {
	FILE *f;
	ptab_stream_t *s;
};

struct io_vtable {
	size_t (*write)(const char*, size_t, union io_stream);
	size_t (*write_char)(const utf8_char_t*, union io_stream);
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
 * strbuf functions
 */

static int strbuf_putc(struct strbuf *sb, char c)
{
	if (sb->avail == 0)
		return EOF;

	sb->buf[sb->used] = c;
	sb->used++;
	sb->avail--;

	return 0;
}

static int strbuf_puts(struct strbuf *sb, const char *str, size_t len)
{
	if (len > sb->avail)
		return EOF;

	memcpy(sb->buf + sb->used, str, len);
	sb->used += len;
	sb->avail -= len;

	return 0;
}

static int strbuf_put_utf8c(struct strbuf *sb, const utf8_char_t *c)
{
	if (c->len > sb->avail)
		return EOF;

	memcpy(sb->buf+ sb->used, c->c, c->len);
	sb->used += c->len;
	sb->avail -= c->len;

	return 0;
}

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
		const struct utf8_char *c,
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
		const struct utf8_char *c,
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
