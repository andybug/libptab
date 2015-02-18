
#include <cstdlib>
#include <cassert>
#include <vector>
#include <iostream>
#include <exception>
#include <stdexcept>

#include <tclap/CmdLine.h>

#include <ptab.h>


class ptab_tool {

public:
	enum format {
		FORMAT_ASCII,
		FORMAT_UNICODE
	};

	enum alignment {
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};

	ptab_tool();
	virtual ~ptab_tool();

	void set_format(const std::string& format_);
	void set_alignments(const std::string& alignments_);
	void set_delimiter(const std::string& delim_);

	void read(std::istream& stream);


private:
	ptab_t table;
	enum format format;
	bool user_align;
	std::vector<enum alignment> alignments;
	char delim;
	unsigned int num_columns;
	unsigned int num_rows;
};

ptab_tool::ptab_tool()
{
	int err;

	err = ptab_init(&this->table, NULL);
	if (err != PTAB_OK) {
		throw std::runtime_error("not good");
	}

	this->format = FORMAT_ASCII;
	this->user_align = false;
	this->delim = '\t';
	this->num_columns = 0;
	this->num_rows = 0;
}

ptab_tool::~ptab_tool()
{
	ptab_free(&this->table);
}

void ptab_tool::set_format(const std::string& format_)
{
	if (format_ == "ascii")
		this->format = FORMAT_ASCII;
	else if (format_ == "unicode")
		this->format = FORMAT_UNICODE;
	else
		throw std::runtime_error("invalid format");
}

void ptab_tool::set_alignments(const std::string& alignments_)
{
	std::string::const_iterator iter;

	this->user_align = true;

	for (iter = alignments_.begin(); iter < alignments_.end(); iter++) {
		switch (*iter) {
		case 'l':
			this->alignments.push_back(ALIGN_LEFT);
			break;

		case 'r':
			this->alignments.push_back(ALIGN_RIGHT);
			break;

		case 'c':
			throw std::runtime_error("center alignment not implemented yet");
			break;

		default:
			throw std::runtime_error("invalid align value");
			break;
		}
	}
}

void ptab_tool::set_delimiter(const std::string& delim_)
{
	bool escaped = (delim_.length() == 2) && (delim_[0] == '\\');

	if (delim_.length() >= 2 && !escaped)
		throw std::runtime_error("delimiter must be a single character (or a backslash-escaped special character");

	else if (escaped) {
		assert(delim_.length() == 2);

		switch (delim_[1]) {
		case 's':
			this->delim = ' ';
			break;

		case 't':
			this->delim = '\t';
			break;

		default:
			throw std::runtime_error("unknown escape character in delimiter");
			break;
		}
	}

	else {
		assert(delim_.length() == 1);
		this->delim = delim_[0];
	}
}

static void process_args(ptab_tool& t, int argc, char **argv)
{
	static const char desc[] = "ptab (\"pretty table\") is a utility to produce human-readable tables.";

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
	try {
		ptab_tool tool;

		process_args(tool, argc, argv);
	} catch (std::exception& e) {
		std::cerr << "ptab: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
