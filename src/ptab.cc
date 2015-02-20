
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <vector>
#include <list>
#include <iostream>
#include <sstream>
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

	enum type {
		TYPE_STRING,
		TYPE_NUMERIC
	};

	ptab_tool();
	virtual ~ptab_tool();

	void set_format(const std::string& format_);
	void set_alignments(const std::string& alignments_);
	void set_delimiter(const std::string& delim_);

	void read_input(std::istream& stream);
	void build_table();
	void write_table(FILE *stream);


private:
	ptab_t table;
	enum format format;
	char delim;

	bool user_align;
	std::vector<enum alignment> col_alignments;
	std::vector<enum type> col_types;
	std::list<std::vector<std::string> > tokenized_rows;

	unsigned int num_columns;
	unsigned int num_rows;


	bool is_numeric(const std::string& str);
	void find_column_types();
	void create_column(std::string& name, enum alignment align);
	enum alignment get_default_alignment(enum type t);
	void build_columns();
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
			this->col_alignments.push_back(ALIGN_LEFT);
			break;

		case 'r':
			this->col_alignments.push_back(ALIGN_RIGHT);
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

	if (delim_.length() >= 2 && !escaped) {
		throw std::runtime_error("delimiter must be a single character (or a backslash-escaped special character");
	} else if (escaped) {
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
	} else {
		assert(delim_.length() == 1);
		this->delim = delim_[0];
	}
}

void ptab_tool::read_input(std::istream& stream)
{
	std::string line;

	while (std::getline(stream, line)) {
		std::stringstream line_stream(line);
		std::string token;
		std::vector<std::string> tokens;
		unsigned int columns = 0;

		while (std::getline(line_stream, token, this->delim)) {
			if (!token.empty()) {
				tokens.push_back(token);
				columns++;
			}
		}

		// num_columns not set yet, this must be the first row
		if (this->num_columns == 0)
			this->num_columns = columns;

		// if it isn't the first row, make sure it
		// has the right amount of columns
		if (columns != this->num_columns)
			throw std::runtime_error("column count inconsistent");

		this->tokenized_rows.push_back(tokens);
		this->num_rows++;
	}
}

bool ptab_tool::is_numeric(const std::string& str)
{
	std::stringstream stream(str);
	double temp;

	if (!(stream >> temp).fail())
		return true;

	return false;
}

void ptab_tool::find_column_types()
{
	// if the user has specified the alignment for
	// the columns, no need for use to do it
	if (this->user_align)
		return;

	// initialize col_type to be numeric
	// change to string the first time we encounter a string
	// in the column data
	for (unsigned int i = 0; i < this->num_columns; i++)
		this->col_types.push_back(TYPE_NUMERIC);

	std::list<std::vector<std::string> >::const_iterator iter;
	iter = this->tokenized_rows.begin();

	// skip the heading row
	iter++;

	for(; iter != this->tokenized_rows.end(); iter++) {
		for (unsigned int i = 0; i < this->num_columns; i++) {
			if (this->col_types[i] == TYPE_NUMERIC) {
				// if this row data is not numeric, change
				// the column to string type
				if (!is_numeric((*iter)[i]))
					this->col_types[i] = TYPE_STRING;
			}
		}
	}
}

void ptab_tool::create_column(std::string& name, enum alignment align)
{
	int flags = PTAB_STRING;

	switch (align) {
	case ALIGN_LEFT:
		flags |= PTAB_ALIGN_LEFT;
		break;

	case ALIGN_RIGHT:
		flags |= PTAB_ALIGN_RIGHT;
		break;

	case ALIGN_CENTER:
	default:
		flags |= PTAB_ALIGN_LEFT;
		break;
	}

	int err;
	err = ptab_column(&this->table, name.c_str(), flags);

	if (err)
		throw std::runtime_error("ptab_column error");
}

enum ptab_tool::alignment ptab_tool::get_default_alignment(enum type t)
{
	enum alignment align = ALIGN_LEFT;

	switch (t) {
	case TYPE_STRING:
		align = ALIGN_LEFT;
		break;

	case TYPE_NUMERIC:
		align = ALIGN_RIGHT;
		break;
	}

	return align;
}

void ptab_tool::build_columns()
{
	for (unsigned int i = 0; i < this->num_columns; i++) {
		std::string& name = this->tokenized_rows.front()[i];
		enum alignment align;

		if (this->user_align) {
			align = this->col_alignments[i];
		} else {
			align = get_default_alignment(this->col_types[i]);
		}

		create_column(name, align);
	}
}

void ptab_tool::build_table()
{
	this->find_column_types();
	this->build_columns();

	int err;
	std::list<std::vector<std::string> >::const_iterator iter;
	iter = this->tokenized_rows.begin();

	// skip the first row since that is the heading
	iter++;

	for (; iter != this->tokenized_rows.end(); iter++) {
		err = ptab_begin_row(&this->table);
		if (err)
			throw std::runtime_error("ptab_begin_row error");

		std::vector<std::string>::const_iterator data;
		data = (*iter).begin();

		for (; data != (*iter).end(); data++) {
			err = ptab_row_data_s(&this->table, (*data).c_str());
			if (err)
				throw std::runtime_error("ptab_row_data_s error");
		}

		err = ptab_end_row(&this->table);
		if (err)
			throw std::runtime_error("ptab_end_row error");
	}
}

void ptab_tool::write_table(FILE *stream)
{
	int err;
	int flags;

	switch (this->format) {
	case FORMAT_ASCII:
		flags = PTAB_ASCII;
		break;

	case FORMAT_UNICODE:
		flags = PTAB_UNICODE;
		break;
	}

	err = ptab_dumpf(&this->table, stream, flags);
	if (err)
		throw std::runtime_error("ptab_dumpf error");
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
		ptab_tool tool;

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
