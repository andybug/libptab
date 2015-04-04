
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <ptab.h>

/* need this for the callback... */
struct params;

struct option {
	char short_name;
	const char *long_name;
	bool data;
	void (*callback)(const char *data, size_t len, struct params *);
};

struct params {
	bool data_expected;
	const struct option *data_option;
	/* actual program params here */
	char delim;
	enum ptab_format format;
	enum ptab_align *aligns;
	int num_aligns;
};

/* prototypes for the option handlers */
static void option_help(const char *, size_t, struct params *);
static void option_version(const char *, size_t, struct params *);
static void option_delim(const char *, size_t, struct params *);
static void option_format(const char *, size_t, struct params *);
static void option_align(const char *, size_t, struct params *);

/* define the options for the program */
static const struct option options[] = {
	{ 'd', "delim", true, option_delim },
	{ '\0', "format", true, option_format },
	{ '\0', "align", true, option_align },
	{ 'h', "help", false, option_help },
	{ '\0', "version", false, option_version }
};

/* get the number of options at build time */
static const int num_options = sizeof(options) / sizeof(struct option);


static const struct option *lookup_short_option(char c)
{
	const struct option *o = NULL;
	int i;

	for (i = 0; i < num_options; i++) {
		if (options[i].short_name == c) {
			o = options + i;
			break;
		}
	}

	return o;
}

static const struct option *lookup_long_option(const char *name, size_t len)
{
	const struct option *o = NULL;
	int i;

	for (i = 0; i < num_options; i++) {
		if (strncmp(options[i].long_name, name, len) == 0) {
			/*
			 * make sure that long_name and name are the
			 * same length
			 */
			if (options[i].long_name[len] != '\0')
				continue;

			o = options + i;
			break;
		}
	}

	return o;
}

static void handle_option(const struct option *o,
			  const char *data,
			  size_t len,
			  struct params *p)
{
	o->callback(data, len, p);
}

static void handle_short_arg(const char *arg, size_t len, struct params *p)
{
	const struct option *o = lookup_short_option(*arg);

	if (!o) {
		/* unknown option */
		fprintf(stderr, "unknown argument '%c'\n", *arg);
		exit(1);
	} else if (!o->data && len == 1) {
		/* a simple flag: -f */
		handle_option(o, NULL, 0, p);
	} else if (!o->data && len > 1) {
		/* chained flags: -abcd */
		handle_option(o, NULL, 0, p);
		handle_short_arg(arg + 1, len - 1, p);
	} else if (o->data && len == 1) {
		/* argument with data following: -a <val> */
		p->data_expected = true;
		p->data_option = o;
	} else if (o->data && len > 1) {
		/* argument with data attached: -aVal */
		handle_option(o, arg + 1, len - 1, p);
	} else {
		/* can this even happen? */
		assert(0);
	}
}

static void handle_long_arg(const char *arg, size_t len, struct params *p)
{
	const struct option *o;
	size_t name_len, data_len;
	const char *equals_sign;

	equals_sign = strrchr(arg, '=');
	if (equals_sign) {
		/*
		 * find the length of the two substrings that
		 * contain the name of the option followed by the data:
		 * name=data
		 */
		name_len = (size_t)(equals_sign - arg);
		data_len = len - name_len - 1;
	} else {
		/*
		 * if there is no equals sign, the entire
		 * arg is the option name
		 */
		name_len = len;
		data_len = 0;
	}

	/* lookup the option using on the first name_len chars */
	o = lookup_long_option(arg, name_len);

	if (!o) {
		/* unknown option */
		fprintf(stderr, "unknown argument '%s'\n", arg);
		exit(1);
	} else if (!o->data && !equals_sign) {
		/* flag: --flag */
		handle_option(o, NULL, 0, p);
	} else if (!o->data && equals_sign) {
		/* flag, but data was given: --flag=x */
		fprintf(stderr, "'%s' does not take a value\n", o->long_name);
		exit(1);
	} else if (o->data && equals_sign) {
		/* argument with value: --arg=val */
		handle_option(o, equals_sign + 1, data_len, p);
	} else if (o->data && !equals_sign) {
		/* argument with value next: --arg val */
		p->data_expected = true;
		p->data_option = o;
	} else {
		/* shouldn't get here... */
		assert(0);
	}
}

static void process_args(int argc, char **argv, struct params *p)
{
	int i;
	size_t len;

	for (i = 0; i < argc; i++) {
		len = strlen(argv[i]);

		if (p->data_expected) {
			/*
			 * data is expected from a previous argument,
			 * call the handler
			 */
			handle_option(p->data_option, argv[i], len, p);
			p->data_expected = false;
			p->data_option = NULL;
		} else if (len > 1 && argv[i][0] == '-' && argv[i][1] == '-') {
			/* long arg: --long, drop the "--" */
			handle_long_arg(argv[i] + 2, len - 2, p);
		} else if (len > 1 && argv[i][0] == '-') {
			/* short arg: -s, drop the '-' */
			handle_short_arg(argv[i] + 1, len - 1, p);
		}
	}

	/* an argument was expecting some data */
	if (p->data_expected) {
		fprintf(stderr,
			"'%s' requires a value\n",
			p->data_option->long_name);
		exit(1);
	}
}

static void option_help(const char *data, size_t len, struct params *p)
{
	(void)data;
	(void)len;
	(void)p;

	/* clang-format off */
	static const char usage[] =
		"usage: ptab [-d <char> | --delim=<char>] [--format=<f>]\n"
		"            [--align=<col aligns>] [-h | --help]\n"
		"            [--version]\n"
		"\n"
		"arguments:\n"
		"  -d <char>, --delim=<char>        Sets the field delimiter on the input data\n\n"
		"  --format=<format specifier>      Selects the output format: (ascii|unicode)\n\n"
		"  --align <col alignments          Aligns the columns in the table; provide a string that\n"
		"                                   defines each column in the table as left or right aligned\n"
		"                                   by means of a character (l|r). Example: for a table with\n"
		"                                   three columns, align the first two to the left and the last\n"
		"                                   to the right: --align=llr\n\n"
		"  -h, --help                       Print this usage and exit\n\n"
		"  --version                        Print the version and exit\n";
	/* clang-format on */

	fputs(usage, stdout);
	exit(0);
}

static void option_version(const char *data, size_t len, struct params *p)
{
	(void)data;
	(void)len;
	(void)p;

	printf("ptab version %s\n", PTAB_VERSION);
	exit(0);
}

static void option_delim(const char *data, size_t len, struct params *p)
{
	if (len != 1) {
		fprintf(stderr, "delim can only be one character\n");
		exit(1);
	}

	p->delim = *data;
}

static void option_format(const char *data, size_t len, struct params *p)
{
	(void)len;

	if (strcmp("ascii", data) == 0) {
		p->format = PTAB_ASCII;
	} else if (strcmp("unicode", data) == 0) {
		p->format = PTAB_UNICODE;
	} else {
		fprintf(stderr, "unknown format '%s'\n", data);
		exit(1);
	}
}

static void option_align(const char *data, size_t len, struct params *p)
{
	size_t i;

	if (p->aligns) {
		fprintf(stderr, "'align' specified more than once\n");
		exit(1);
	}

	/* allocate the aligns array */
	p->aligns = malloc(len);
	p->num_aligns = len;

	for (i = 0; i < len; i++) {
		/* convert each char to the associated enum val */
		switch (data[i]) {
		case 'l':
			p->aligns[i] = PTAB_LEFT;
			break;
		case 'r':
			p->aligns[i] = PTAB_RIGHT;
			break;
		default:
			fprintf(stderr,
				"unknown alignment specifier '%c'\n",
				data[i]);
			exit(1);
		}
	}
}

int main(int argc, char **argv)
{
	struct params p = {.data_expected = false,
			   .data_option = NULL,
			   .delim = '\t',
			   .format = PTAB_ASCII,
			   .aligns = NULL,
			   .num_aligns = 0 };

	process_args(argc, argv, &p);

	return EXIT_SUCCESS;
}
