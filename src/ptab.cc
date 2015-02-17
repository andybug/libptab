
#include <cstdlib>
#include <cassert>
#include <vector>
#include <iostream>
#include <exception>
#include <stdexcept>

#include <tclap/CmdLine.h>

#include <ptab.h>

#define BUF_SIZE 1024

enum output_format {
	OUTPUT_ASCII,
	OUTPUT_UNICODE
};

enum align_spec {
	ALIGN_SPEC_LEFT,
	ALIGN_SPEC_RIGHT,
	ALIGN_SPEC_CENTER
};

struct ptab_state {
	enum output_format format;

	bool user_align;
	std::vector<enum align_spec> align;

	unsigned int num_columns;
	unsigned int num_rows;
};

#if 0
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

static int get_column_flags(const struct ptab_state *s, const char *str)
{
	bool numeric;
	int flags = PTAB_STRING;

	if (s->align) {
		if (s->num_columns >= s->num_alignments) {
			fprintf(stderr, "ptab: not enough alignments provided\n");
			return -1;
		}

		flags |= s->alignments[s->num_columns];

	} else {
		numeric = test_numeric(str);
		flags |= (numeric ? PTAB_ALIGN_RIGHT : 0);
	}

	return flags;
}

static int create_columns_no_heading(
		struct ptab_state *s,
		ptab_t *p,
		FILE *stream)
{
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	char *str;
	char *saveptr;
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
		flags = get_column_flags(s, str);
		if (flags < 0)
			return 1;

		ptab_column(p, "", flags);
		s->num_columns++;

		str = strtok_r(NULL, "\t\n", &saveptr);
	}

	/* tokenize buf2 and add first row */
	ptab_begin_row(p);

	str = strtok_r(buf2, "\t\n", &saveptr);
	while (str) {
		ptab_row_data_s(p, str);
		s->num_rows++;

		str = strtok_r(NULL, "\t\n", &saveptr);
	}

	return (ptab_end_row(p) != PTAB_OK);
}

static int create_columns_with_heading(
		struct ptab_state *s,
		ptab_t *p,
		FILE *stream)
{
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	char buf3[BUF_SIZE];
	char *str1, *str2, *str3;
	char *saveptr1, *saveptr2, *saveptr3;
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
		flags = get_column_flags(s, str2);
		if (flags < 0)
			return 1;

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

static int create_columns(struct ptab_state *s, ptab_t *p, FILE *stream)
{
	if (s->no_heading)
		return create_columns_no_heading(s, p, stream);
	else
		return create_columns_with_heading(s, p, stream);

	return 1;
}

static int add_rows(struct ptab_state *s, ptab_t *p, FILE *stream)
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

static int process_short_arg(struct ptab_state *s, const char *arg)
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

static int process_align_arg(struct ptab_state *s, const char *arg)
{
	const char *val;
	size_t len;
	unsigned int i;

	val = strchr(arg, '=');
	if (!val) {
		fprintf(stderr, "ptab: align option requires a value (--align=[rl]...)\n");
		return 1;
	}

	len = strlen(++val);
	s->alignments = (int*)malloc(sizeof(int) * len);
	s->num_alignments = len;

	for (i = 0; i < len; i++) {
		switch (val[i]) {
		case 'l':
			s->alignments[i] = PTAB_ALIGN_LEFT;
			break;

		case 'r':
			s->alignments[i] = PTAB_ALIGN_RIGHT;
			break;

		default:
			fprintf(stderr, "ptab: unknown alignment '%c'\n", val[i]);
			return 1;
		}
	}

	return 0;
}

static int process_long_arg(struct ptab_state *s, const char *arg)
{
	assert(arg[0] == '-' && arg[1] == '-');

	arg += 2;

	if (strcmp("noheading", arg) == 0) {
		s->no_heading = true;
	} else if (strcmp("unicode", arg) == 0) {
		s->unicode = true;
	} else if (strncmp("align", arg, 5) == 0) {
		s->align = true;
		if (process_align_arg(s, arg) != 0)
			return 1;
	} else {
		fprintf(stderr, "ptab: unknown option '%s'\n", arg);
		return 1;
	}

	return 0;
}

static int process_args(struct ptab_state *s, int argc, char **argv)
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
#endif

static void init_state(struct ptab_state &s)
{
	s.format = OUTPUT_ASCII;
	s.user_align = false;
	s.align = std::vector<enum align_spec>();
	s.num_columns = 0;
	s.num_rows = 0;
}

static void process_args(struct ptab_state &s, int argc, char **argv)
{
	static const char desc[] = "ptab (\"pretty table\") is a utility to produce human-readable tables.";
	bool unicode = false;
	std::string format_desc;
	std::string alignments;

	try {
		TCLAP::CmdLine cmd(desc, ' ', PTAB_VERSION_STRING);

		// unicode switch
		TCLAP::SwitchArg unicodeSwitch("u", "unicode",
				"Output using Unicode graph characters", cmd, false);

		// format arg
		std::vector<std::string> format_list;
		format_list.push_back("ascii");
		format_list.push_back("unicode");
		TCLAP::ValuesConstraint<std::string> formatConstraints(format_list);
		TCLAP::ValueArg<std::string> formatArg("", "format",
				"Specify the output format", false, "none", &formatConstraints, cmd);

		// align arg
		TCLAP::ValueArg<std::string> alignArg("", "align",
				"Manually set column alignment, one character per column. For example, a table with three columns could be set to align the first to columns on the left and the last column on the right like so: '--align llr'. If this is not provided, ptab will attempt to detect the data formats and align appropriately.",
				false, "none", "[l|r]+", cmd);

		cmd.parse(argc, argv);

		unicode = unicodeSwitch.getValue();
		format_desc = formatArg.getValue();
		alignments = alignArg.getValue();

	} catch (TCLAP::ArgException &e) {
		std::string msg = __func__;
		msg.append(": error processing arguments: ");
		msg.append(e.error());
		throw std::runtime_error(msg);
	}

	// check arguments for valid values and fill out ptab_state structure

	if (format_desc == "none" || format_desc == "ascii")
		s.format = OUTPUT_ASCII;
	else if (format_desc == "unicode")
		s.format = OUTPUT_UNICODE;
	else
		throw std::runtime_error("unknown format specified");

	// if the unicode flag and format arg given, let the unicode flag
	// override
	if (unicode)
		s.format = OUTPUT_UNICODE;

	if (alignments.length() > 0) {
		s.user_align = true;
		std::string::iterator iter;

		for (iter = alignments.begin(); iter < alignments.end(); iter++) {
			switch (*iter) {
			case 'l':
				s.align.push_back(ALIGN_SPEC_LEFT);
				break;

			case 'r':
				s.align.push_back(ALIGN_SPEC_RIGHT);
				break;

			case 'c':
				throw std::runtime_error("center alignment has not yet been implemented");
				break;

			default:
				throw std::runtime_error("valid align values are [lr]");
				break;
			}
		}
	}
}

int main(int argc, char **argv)
{
	struct ptab_state state;

	init_state(state);

	try {
		process_args(state, argc, argv);
	} catch (std::exception& e) {
		std::cerr << "ptab: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
#if 0
	struct ptab_state state;
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
#endif
	return EXIT_SUCCESS;
}
