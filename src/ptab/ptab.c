
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ptab.h>
#include "internal.h"

struct state {
	const struct params *params;
	ptab_t *table;
	int num_columns;
	int num_rows;
};

/*
 * read_line
 *
 * reads a line from stdin and tokenizes it based on the
 * delimiter in state. for each token, it calls the given
 * callback.
 *
 * returns:
 *  (0)  OK
 *  (1)  EOF
 * (<0)  Callback returned error
 */
static int read_line(struct state *s,
		     int (*callback)(struct state *, const char *))
{
	static const size_t BUF_SIZE = 1024;
	char buf[BUF_SIZE];
	char *result;

	/* read a line from stdin into buf */
	result = fgets(buf, BUF_SIZE, stdin);
	if (result != buf) {
		/* no characters read, EOF */
		return 1;
	}

	/*
	 * tokenize the line, calling the callback with each
	 * token in turn
	 */
	char delim[3] = { s->params->delim, '\n', '\0' };
	char *saveptr;
	char *token;

	token = strtok_r(buf, delim, &saveptr);
	while (token) {
		int err = callback(s, token);
		if (err < 0)
			return -2;

		token = strtok_r(NULL, delim, &saveptr);
	}

	return 0;
}

/*
 * heading line
 *
 * the first line in the input data specifies the headings
 * of the columns and sets the number of columns that each
 * row of data must match.
 *
 * each entry in the heading row is mapped to a ptab_column
 * call to create the column in the table. we set the column
 * type to string since we do not know what type of data will
 * be coming in
 */
static int read_heading_token(struct state *s, const char *token)
{
	int err;

	err = ptab_column(s->table, token, PTAB_STRING);
	if (err) {
		/* FIXME */
		fprintf(stderr, "ptab_column error\n");
		return -1;
	}

	s->num_columns++;

	return 0;
}

static int read_heading(struct state *s)
{
	int err;

	err = read_line(s, read_heading_token);
	if (err > 0) {
		/* reached EOF on first line */
		fprintf(stderr, "expected more than one line");
		return -1;
	} else if (err < 0) {
		return -1;
	}

	return 0;
}

/*
 * row
 *
 * each line in the input following the heading line and until EOF is
 * interpreted as a row in the table. each token in the line is
 * mapped to its associated column in the table. the number of tokens
 * in the line must match the number of columns in the table
 */
static int read_row_token(struct state *s, const char *token)
{
	int err;

	err = ptab_row_data_s(s->table, token);
	if (err) {
		/* FIXME */
		fprintf(stderr, "ptab_row_data_s error\n");
		return -1;
	}

	return 0;
}

static int read_row(struct state *s)
{
	int err;

	s->num_rows++;

	err = ptab_begin_row(s->table);
	if (err) {
		/* FIXME */
		fprintf(stderr, "ptab_begin_row error\n");
		return -1;
	}

	err = read_line(s, read_row_token);
	if (err > 0) {
		/* EOF */
		return 1;
	} else if (err < 0) {
		return -1;
	}

	err = ptab_end_row(s->table);
	if (err) {
		/* FIXME */
		fprintf(stderr, "ptab_end_row error\n");
		return -1;
	}

	return 0;
}

/*
 * this is the entry point to the program proper,
 * main calls this after reading the program arguments
 * and passes the params struct that defines how this
 * run will behave
 */
int ptab_exec(const struct params *p)
{
	int err;
	struct state s = {
		.params = p,
		.table = NULL,
		.num_columns = 0,
		.num_rows = 0
	};

	/* initialize the table */
	s.table = ptab_init(NULL);
	if (!s.table) {
		/* FIXME */
		fprintf(stderr, "ptab_init error\n");
		return -1;
	}

	/* read first line and create columns */
	err = read_heading(&s);
	if (err)
		return -1;

	/* read the remaining lines from stdin and create rows */
	while ((err = read_row(&s)) == 0);
	if (err < 0)
		return -1;

	return 0;
}
