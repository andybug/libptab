
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

static int strbuf_putu(struct strbuf *sb, const utf8_char_t *c)
{
	if (c->len > sb->avail)
		return EOF;

	memcpy(sb->buf+ sb->used, c->c, c->len);
	sb->used += c->len;
	sb->avail -= c->len;

	return 0;
}

static int strbuf_repeatc(struct strbuf *sb, char c, size_t num)
{
	size_t i;

	if (num > sb->avail)
		return EOF;

	for (i = 0; i < num; i++)
		sb->buf[sb->used + i] = c;

	sb->used += i;
	sb->avail -= i;

	return 0;
}

static int strbuf_repeatu(struct strbuf *sb, const utf8_char_t *c, size_t num)
{
	size_t i;

	if ((num * c->len) > sb->avail)
		return EOF;

	for (i = 0; i < num; i++)
		strbuf_putu(sb, c);

	return 0;
}

/*
 * Generic table writing
 */

static int write_row_top(
		const ptab *p,
		const struct format_desc *desc,
		struct strbuf *sb)
{
	const struct ptab_col *col = p->internal->columns_head;

	strbuf_putu(sb, &desc->top_left_intersect);
	strbuf_putu(sb, &desc->horiz_div);

	while (col) {
		strbuf_repeatu(sb, &desc->horiz_div, col->width);

		if (col->next) {
			strbuf_putu(sb, &desc->horiz_div);
			strbuf_putu(sb, &desc->top_middle_intersect);
			strbuf_putu(sb, &desc->horiz_div);
		}

		col = col->next;
	}

	strbuf_putu(sb, &desc->horiz_div);
	strbuf_putu(sb, &desc->top_right_intersect);
	strbuf_putc(sb, '\n');
}

static int write_row_heading(
		const ptab *p,
		const struct format_desc *desc,
		struct strbuf *sb)
{
	const struct ptab_col *col = p->internal->columns_head;
	size_t padding;

	strbuf_putu(sb, &desc->vert_div);
	strbuf_putc(sb, ' ');

	while (col) {
		padding = col->width - col->name_len;

		strbuf_puts(sb, col->name, col->name_len);
		strbuf_repeatc(sb, ' ', padding);

		if (col->next) {
			strbuf_putc(sb, ' ');
			strbuf_putu(sb, &desc->vert_div);
			strbuf_putc(sb, ' ');
		}

		col = col->next;
	}

	strbuf_putc(sb, ' ');
	strbuf_putu(sb, &desc->vert_div);
	strbuf_putc(sb, '\n');
}

static int write_table(
		const ptab *p,
		const struct format_desc *desc,
		struct strbuf *sb)
{
	write_row_top(p, desc, sb);
	write_row_heading(p, desc, sb);

	return PTAB_OK;
}

int ptab_dumpf(ptab *p, FILE *f, int flags)
{
	const struct format_desc *desc = &ascii_format;
	struct strbuf sb;
	size_t alloc_size = 128;

	/*
	 * allocate strbuf to be the same size as the entire
	 * output table
	 */
	sb.buf = ptab_alloc(p, alloc_size);
	sb.size = alloc_size;
	sb.used = 0;
	sb.avail = alloc_size;

	write_table(p, desc, &sb);
	fwrite(sb.buf, 1, sb.used, f);

	return PTAB_OK;
}
