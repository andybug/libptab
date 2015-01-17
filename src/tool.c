
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <ptab.h>

#define BUF_SIZE 1024

struct ptab_tool_state {
	bool no_heading;
	bool unicode;

	unsigned int num_columns;
	unsigned int num_rows;

	int *col_align;
};

static bool test_numeric(const char *str)
{
	char *endptr;

	/* test if it is an integer value */
	strtol(str, &endptr, 10);
	if (*endptr == '\0')
		return true;

	/* test if it is a float value */
	strtod(str, &endptr);
	if (*endptr == '\0')
		return true;

	return false;
}

static int create_columns_no_heading(
		struct ptab_tool_state *s,
		ptab_t *p,
		FILE *stream)
{
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	char *str;
	char *saveptr;
	bool numeric;
	int flags;

	/* read line into buf1 */
	str = fgets(buf1, BUF_SIZE, stream);
	if (str != buf1)
		return 1;

	/* copy to buf2 before tokenizing */
	strcpy(buf2, buf1);

	/* tokenize buf1 and add columns */
	str = strtok_r(buf1, "\t\n", &saveptr);
	while (str) {
		numeric = test_numeric(str);
		flags = PTAB_STRING | (numeric ? PTAB_ALIGN_RIGHT : 0);

		ptab_column(p, "", flags);
		s->num_columns++;

		str = strtok_r(NULL, "\t\n", &saveptr);
	}

	/* tokenize buf2 and add first row */
	ptab_begin_row(p);

	str = strtok_r(buf2, "\t\n", &saveptr);
	while (str) {
		printf("add str=%s\n", str);
		ptab_row_data_s(p, str);
		s->num_rows++;

		str = strtok_r(NULL, "\t\n", &saveptr);
	}

	return (ptab_end_row(p) != PTAB_OK);
}

static int create_columns_with_heading(
		struct ptab_tool_state *s,
		ptab_t *p,
		FILE *stream)
{
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	char buf3[BUF_SIZE];
	char *str1, *str2, *str3;
	char *saveptr1, *saveptr2, *saveptr3;
	bool numeric;
	int flags;

	/* read heading line into buf1 */
	str1 = fgets(buf1, BUF_SIZE, stream);
	if (str1 != buf1)
		return 1;

	/* read first line into buf2 */
	str2 = fgets(buf2, BUF_SIZE, stream);
	if (str2 != buf2)
		return 1;

	/* copy first line into buf3 before tokenizing */
	strcpy(buf3, buf2);

	/* tokenize buf1 and add columns */
	str1 = strtok_r(buf1, "\t\n", &saveptr1);
	str2 = strtok_r(buf2, "\t\n", &saveptr2);
	while (str1 && str2) {
		numeric = test_numeric(str2);
		flags = PTAB_STRING | (numeric ? PTAB_ALIGN_RIGHT : 0);

		ptab_column(p, str1, flags);
		s->num_columns++;

		str1 = strtok_r(NULL, "\t\n", &saveptr1);
		str2 = strtok_r(NULL, "\t\n", &saveptr2);
	}

	/* tokenize buf3 and add first row */
	ptab_begin_row(p);

	str3 = strtok_r(buf3, "\t\n", &saveptr3);
	while (str3) {
		ptab_row_data_s(p, str3);
		s->num_rows++;

		str3 = strtok_r(NULL, "\t\n", &saveptr3);
	}

	return (ptab_end_row(p) != PTAB_OK);
}

static int create_columns(struct ptab_tool_state *s, ptab_t *p, FILE *stream)
{
	if (s->no_heading)
		return create_columns_no_heading(s, p, stream);
	else
		return create_columns_with_heading(s, p, stream);

	return 1;
}

static int add_rows(struct ptab_tool_state *s, ptab_t *p, FILE *stream)
{
	char buf[BUF_SIZE];
	char *str;
	char *saveptr;
	int err;

	/* read line from stream */
	while (fgets(buf, BUF_SIZE, stream)) {
		/* tokenize buf */
		str = strtok_r(buf, "\t\n", &saveptr);

		ptab_begin_row(p);

		/* add row data from tokens */
		while (str) {
			err = ptab_row_data_s(p, str);
			if (err != PTAB_OK) {
				fprintf(stderr, "ptab: ptab_row_data_s: %s\n",
					ptab_strerror(err));
				return 1;
			}

			str = strtok_r(NULL, "\t\n", &saveptr);
		}

		err = ptab_end_row(p);
		if (err != PTAB_OK) {
			fprintf(stderr, "ptab: ptab_end_row: %s\n",
				ptab_strerror(err));
			return 1;
		}

		s->num_rows++;
	}

	return 0;
}

static void init_state(struct ptab_tool_state *s)
{
	s->no_heading = false;
	s->unicode = false;
	s->num_columns = 0;
	s->num_rows = 0;
}

static int process_short_arg(struct ptab_tool_state *s, const char *arg)
{
	const char *c;

	assert(arg[0] == '-');

	c = arg + 1;
	while (*c != '\0') {
		switch (*c) {
		case 'n':
			s->no_heading = true;
			break;

		case 'u':
			s->unicode = true;
			break;

		default:
			fprintf(stderr, "ptab: unknown option '%c'\n", *c);
			return 1;
		}

		c++;
	}

	return 0;
}

static int process_long_arg(struct ptab_tool_state *s, const char *arg)
{
	assert(arg[0] == '-' && arg[1] == '-');

	arg += 2;

	if (strcmp("noheading", arg) == 0) {
		s->no_heading = true;
	} else if (strcmp("unicode", arg) == 0) {
		s->unicode = true;
	} else {
		fprintf(stderr, "ptab: unknown option '%s'\n", arg);
		return 1;
	}

	return 0;
}

static int process_args(struct ptab_tool_state *s, int argc, char **argv)
{
	int i;
	const char *arg;
	size_t len;

	for (i = 1; i < argc; i++) {
		arg = argv[i];
		len = strlen(arg);

		if (len > 2 && arg[0] == '-' && arg[1] == '-') {
			if (process_long_arg(s, arg) != 0)
				return 1;
		} else if (len > 1 && arg[0] == '-') {
			if (process_short_arg(s, arg) != 0)
				return 1;
		} else {
			fprintf(stderr, "ptab: unknown argument: '%s'\n", arg);
			/* TODO: print usage */
			return 1;
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	struct ptab_tool_state state;
	ptab_t p;
	int flags;

	init_state(&state);

	if (process_args(&state, argc, argv) != 0)
		return EXIT_FAILURE;

	ptab_init(&p, NULL);

	if (create_columns(&state, &p, stdin) != 0)
		return EXIT_FAILURE;

	if (add_rows(&state, &p, stdin) != 0)
		return EXIT_FAILURE;

	flags = 0;
	flags |= state.unicode ? PTAB_UNICODE : PTAB_ASCII;
	flags |= state.no_heading ? PTAB_NOHEADING : 0;

	ptab_dumpf(&p, stdout, flags);
	ptab_free(&p);

	return EXIT_SUCCESS;
}
