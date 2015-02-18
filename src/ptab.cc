
#include <cstdlib>
#include <cassert>
#include <vector>
#include <iostream>
#include <exception>
#include <stdexcept>

#include <tclap/CmdLine.h>

#include <ptab.h>

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

	return EXIT_SUCCESS;
}
