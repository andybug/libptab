
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <ptab.h>

struct ptab_tool_state {
	bool no_heading;
	bool unicode;
};

static int add_heading(ptab_t *p, FILE *stream)
{
	char buf[1024];
	char *s;
	char *saveptr;
	int num = 0;

	s = fgets(buf, 1024, stream);

	if (s != buf)
		return 0;

	s = strtok_r(buf, "\t\n", &saveptr);
	while (s) {
		ptab_column(p, s, PTAB_STRING);
		num++;

		s = strtok_r(NULL, "\t\n", &saveptr);
	}

	return num;
}

static int add_row(ptab_t *p, FILE *stream)
{
	char buf[1024];
	char *s;
	char *saveptr;

	s = fgets(buf, 1024, stream);

	if (s != buf)
		return 0;

	ptab_begin_row(p);

	s = strtok_r(buf, "\t\n", &saveptr);
	while (s) {
		ptab_row_data_s(p, s);

		s = strtok_r(NULL, "\t\n", &saveptr);
	}

	return (ptab_end_row(p) == PTAB_OK);
}

static void init_state(struct ptab_tool_state *s)
{
	s->no_heading = false;
	s->unicode = false;
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
	int num_columns;
	int num;

	init_state(&state);

	if (process_args(&state, argc, argv) != 0)
		return EXIT_FAILURE;

	ptab_init(&p, NULL);

	num_columns = add_heading(&p, stdin);

	while (add_row(&p, stdin));

	ptab_dumpf(&p, stdout, state.unicode ? PTAB_UNICODE : PTAB_ASCII);
	ptab_free(&p);

	return EXIT_SUCCESS;
}
