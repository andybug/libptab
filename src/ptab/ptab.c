
#include <stdio.h>
#include <string.h>

#include <ptab.h>
#include "internal.h"

struct state {
	const struct params *params;
	ptab_t *table;
	int num_columns;
	int num_rows;
};

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

/* FIXME remove */
int temp(struct state *s, const char *token)
{
	static int count = 0;

	printf("%d: '%s'\n", ++count, token);
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
	struct state s = {
		.params = p,
		.num_columns = 0,
		.num_rows = 0
	};

	while (read_line(&s, temp) == 0);

	return 0;
}
