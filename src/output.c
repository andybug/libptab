
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
	utf8_char_t bot_left_intersect;
	utf8_char_t bot_middle_intersect;
	utf8_char_t bot_right_intersect;
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
	.horiz_div = { "-", 1 },
	.vert_div = { "|", 1 },
	.top_left_intersect = { "+", 1 },
	.top_middle_intersect = { "+", 1 },
	.top_right_intersect = { "+", 1 },
	.div_left_intersect = { "+", 1 },
	.div_middle_intersect = { "+", 1 },
	.div_right_intersect = { "+", 1 },
	.bot_left_intersect = { "+", 1 },
	.bot_middle_intersect = { "+", 1 },
	.bot_right_intersect = { "+", 1 }
};

static const struct format_desc unicode_format = {
	.horiz_div = { "\u2500", 3 },
	.vert_div = { "\u2502", 3 },
	.top_left_intersect = { "\u250c", 3 },
	.top_middle_intersect = { "\u252c", 3 },
	.top_right_intersect = { "\u2510", 3 },
	.div_left_intersect = { "\u251c", 3 },
	.div_middle_intersect = { "\u253c", 3 },
	.div_right_intersect = { "\u2524", 3 },
	.bot_left_intersect = { "\u2514", 3 },
	.bot_middle_intersect = { "\u2534", 3 },
	.bot_right_intersect = { "\u2518", 3 }
};

/*
 * strbuf functions
 */

static void strbuf_init(struct strbuf *sb, char *buf, size_t size)
{
	sb->buf = buf;
	sb->size = size;
	sb->used = 0;
	sb->avail = size;
}

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

	memcpy(sb->buf + sb->used, c->c, c->len);
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

static void write_row_top(const ptab_t *p,
			  const struct format_desc *desc,
			  struct strbuf *sb)
{
	const struct ptab_col *col = p->columns_head;

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

static void write_row_heading(const ptab_t *p,
			      const struct format_desc *desc,
			      struct strbuf *sb)
{
	const struct ptab_col *col = p->columns_head;
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

static void write_row_divider(const ptab_t *p,
			      const struct format_desc *desc,
			      struct strbuf *sb)
{
	const struct ptab_col *col = p->columns_head;

	strbuf_putu(sb, &desc->div_left_intersect);
	strbuf_putu(sb, &desc->horiz_div);

	while (col) {
		strbuf_repeatu(sb, &desc->horiz_div, col->width);

		if (col->next) {
			strbuf_putu(sb, &desc->horiz_div);
			strbuf_putu(sb, &desc->div_middle_intersect);
			strbuf_putu(sb, &desc->horiz_div);
		}

		col = col->next;
	}

	strbuf_putu(sb, &desc->horiz_div);
	strbuf_putu(sb, &desc->div_right_intersect);
	strbuf_putc(sb, '\n');
}

static void write_row_data(const ptab_t *p,
			   const struct format_desc *desc,
			   const struct ptab_row *row,
			   struct strbuf *sb)
{
	const struct ptab_col *col = p->columns_head;
	size_t padding;

	strbuf_putu(sb, &desc->vert_div);
	strbuf_putc(sb, ' ');

	while (col) {
		padding = col->width - row->lengths[col->id];

		if (col->align == PTAB_RIGHT)
			strbuf_repeatc(sb, ' ', padding);

		strbuf_puts(sb, row->strings[col->id], row->lengths[col->id]);

		if (col->align == PTAB_LEFT)
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

static void write_row_bottom(const ptab_t *p,
			     const struct format_desc *desc,
			     struct strbuf *sb)
{
	const struct ptab_col *col = p->columns_head;

	strbuf_putu(sb, &desc->bot_left_intersect);
	strbuf_putu(sb, &desc->horiz_div);

	while (col) {
		strbuf_repeatu(sb, &desc->horiz_div, col->width);

		if (col->next) {
			strbuf_putu(sb, &desc->horiz_div);
			strbuf_putu(sb, &desc->bot_middle_intersect);
			strbuf_putu(sb, &desc->horiz_div);
		}

		col = col->next;
	}

	strbuf_putu(sb, &desc->horiz_div);
	strbuf_putu(sb, &desc->bot_right_intersect);
	strbuf_putc(sb, '\n');
}

static int
write_table(const ptab_t *p, const struct format_desc *desc, struct strbuf *sb)
{
	const struct ptab_row *row;

	write_row_top(p, desc, sb);
	write_row_heading(p, desc, sb);
	write_row_divider(p, desc, sb);

	row = p->rows_head;
	while (row) {
		write_row_data(p, desc, row, sb);
		row = row->next;
	}

	write_row_bottom(p, desc, sb);

	return PTAB_OK;
}

/*
 * Helper functions
 */

static size_t calculate_variable_widths(const ptab_t *p)
{
	size_t total = 0;
	const struct ptab_col *col = p->columns_head;

	while (col) {
		total += col->width;
		col = col->next;
	}

	return total;
}

static size_t calculate_top_row(const struct format_desc *desc,
				unsigned int num_columns,
				size_t variable)
{
	size_t left, middle, right, total;

	left = desc->top_left_intersect.len + desc->horiz_div.len;
	middle = desc->top_middle_intersect.len + (2 * desc->horiz_div.len);
	right = desc->top_right_intersect.len + desc->horiz_div.len + 1;

	total = left + (middle * (num_columns - 1)) + right +
		(variable * desc->horiz_div.len);

	return total;
}

static size_t calculate_div_row(const struct format_desc *desc,
				unsigned int num_columns,
				size_t variable)
{
	size_t left, middle, right, total;

	left = desc->div_left_intersect.len + desc->horiz_div.len;
	middle = desc->div_middle_intersect.len + (2 * desc->horiz_div.len);
	right = desc->div_right_intersect.len + desc->horiz_div.len + 1;

	total = left + (middle * (num_columns - 1)) + right +
		(variable * desc->horiz_div.len);

	return total;
}

static size_t calculate_bot_row(const struct format_desc *desc,
				unsigned int num_columns,
				size_t variable)
{
	size_t left, middle, right, total;

	left = desc->bot_left_intersect.len + desc->horiz_div.len;
	middle = desc->bot_middle_intersect.len + (2 * desc->horiz_div.len);
	right = desc->bot_right_intersect.len + desc->horiz_div.len + 1;

	total = left + (middle * (num_columns - 1)) + right +
		(variable * desc->horiz_div.len);

	return total;
}

static size_t calculate_row(const struct format_desc *desc,
			    unsigned int num_columns,
			    size_t variable)
{
	size_t left, middle, right, total;

	left = desc->vert_div.len + 1;
	middle = desc->vert_div.len + 2;
	right = desc->vert_div.len + 2;

	total = left + (middle * (num_columns - 1)) + right + variable;

	return total;
}

static size_t calculate_table_size(const ptab_t *p,
				   const struct format_desc *desc)
{
	unsigned int num_columns = p->num_columns;
	unsigned int num_rows = p->num_rows;
	size_t top, div, bot, row, total;
	size_t variable;

	variable = calculate_variable_widths(p);
	top = calculate_top_row(desc, num_columns, variable);
	div = calculate_div_row(desc, num_columns, variable);
	bot = calculate_bot_row(desc, num_columns, variable);
	row = calculate_row(desc, num_columns, variable);

	total = top + div + (row * (num_rows + 1)) + bot;

	return total;
}

static const struct format_desc *get_desc(enum ptab_format f)
{
	const struct format_desc *desc = NULL;

	switch (f) {
	case PTAB_ASCII:
		desc = &ascii_format;
		break;

	case PTAB_UNICODE:
		desc = &unicode_format;
		break;

	default:
		break;
	}

	return desc;
}

/*
 * API functions
 */

int ptab_dumpf(ptab_t *p, FILE *f, enum ptab_format fmt)
{
	const struct format_desc *desc;
	struct strbuf sb;
	size_t alloc_size;
	char *buf;

	if (!p || !f)
		return PTAB_ENULL;

	/* get the format descriptor from the format enum */
	desc = get_desc(fmt);
	if (!desc)
		return PTAB_EFORMAT;

	/* allocate a buffer large enough to hold the entire table */
	alloc_size = calculate_table_size(p, desc);
	buf = mem_alloc_block(p, alloc_size);

	if (!buf)
		return PTAB_EMEM;

	/* init strbuf with allocated buffer */
	strbuf_init(&sb, buf, alloc_size);

	/* write the table to the strbuf buffer */
	write_table(p, desc, &sb);

	/* write the buffer to the file */
	fwrite(sb.buf, 1, sb.used, f);

	/* free the allocated buffer */
	mem_free_block(p, buf);

	return PTAB_OK;
}

int ptab_dumps(ptab_t *p, ptab_string_t *s, enum ptab_format fmt)
{
	const struct format_desc *desc;
	struct strbuf sb;
	size_t alloc_size;
	char *buf;

	/* get the format descriptor from the flags */
	desc = get_desc(fmt);
	if (!desc)
		return PTAB_EFORMAT;

	/* allocate a buffer large enough to hold the entire table */
	alloc_size = calculate_table_size(p, desc);
	buf = mem_alloc_block(p, alloc_size);

	if (!buf)
		return PTAB_EMEM;

	/* init strbuf with allocated buffer */
	strbuf_init(&sb, buf, alloc_size);

	/* write the table to the strbuf buffer */
	write_table(p, desc, &sb);

	/* fill out the string structure */
	s->str = sb.buf;
	s->len = sb.used;

	return PTAB_OK;
}
