
#include <cstdlib>

#include <iostream>
#include <stdexcept>

#include <tclap/CmdLine.h>

#include "tool.hpp"


static void process_args(ptabtool::Tool& t, int argc, char **argv)
{
	static const char desc[] =
		"ptab (\"pretty table\") is a utility to produce human-readable tables.";

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
			"Specify the output format",
			false, "none", &formatConstraints, cmd);

		// align arg
		TCLAP::ValueArg<std::string> alignArg("", "align",
			"Manually set column alignment, one character per column."
			"For example, a table with three columns could be set to align"
			"the first to columns on the left and the last column on the"
			"right like so: '--align llr'. If this is not provided, ptab"
			"will attempt to detect the data formats and align appropriately.",
			false, "none", "[l|r]+", cmd);

		// delim arg
		TCLAP::ValueArg<std::string> delimArg("d", "delim",
			"Sets what separates the columns in the input data",
			false, "\t", "char", cmd);

		cmd.parse(argc, argv);

		// pass the arguments to the ptab_tool
		if (formatArg.isSet())
			t.set_format(formatArg.getValue());

		if (unicodeSwitch.isSet())
			t.set_format("unicode");

		if (alignArg.isSet())
			t.set_alignments(alignArg.getValue());

		if (delimArg.isSet())
			t.set_delimiter(delimArg.getValue());

	} catch (TCLAP::ArgException &e) {
		std::string msg = __func__;
		msg.append(": error processing arguments: ");
		msg.append(e.error());
		throw std::runtime_error(msg);
	}
}

int main(int argc, char **argv)
{
	std::cin.sync_with_stdio(false);

	try {
		ptabtool::Tool tool;

		process_args(tool, argc, argv);

		tool.read_input(std::cin);
		tool.build_table();
		tool.write_table(stdout);

	} catch (std::exception& e) {
		std::cerr << "ptab: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
