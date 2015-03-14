
#include <cassert>

#include <sstream>
#include <stdexcept>

#include "tool.hpp"

using namespace ptabtool;

Tool::Tool()
{
	this->table = ptab_init(NULL);
	if (!this->table) {
		throw std::runtime_error("ptab_init error");
	}

	this->format = FORMAT_ASCII;
	this->delim = '\t';
	this->user_align = false;

	this->in_stream = &std::cin;
	this->out_stream = stdout;
}

Tool::~Tool()
{
	ptab_free(this->table);
}

void Tool::set_format(const std::string& format_)
{
	if (format_ == "ascii")
		this->format = FORMAT_ASCII;
	else if (format_ == "unicode")
		this->format = FORMAT_UNICODE;
	else
		throw std::runtime_error("invalid format");
}

void Tool::set_alignments(const std::string& alignments_)
{
	std::string::const_iterator iter;

	this->user_align = true;

	for (iter = alignments_.begin(); iter < alignments_.end(); iter++) {
		switch (*iter) {
		case 'l':
			this->user_alignments.push_back(ALIGN_LEFT);
			break;

		case 'r':
			this->user_alignments.push_back(ALIGN_RIGHT);
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

void Tool::set_delimiter(const std::string& delim_)
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

void Tool::run()
{
	read_input();
	create_columns();
	build_table();
	write_table();
}

void Tool::read_input()
{
	std::string line;

	while (std::getline(*(this->in_stream), line)) {
		std::stringstream line_stream(line);
		std::string token;
		std::vector<std::string> tokens;
		size_t columns = 0;

		while (std::getline(line_stream, token, this->delim)) {
			if (!token.empty()) {
				tokens.push_back(token);
				columns++;
			}
		}

		// header is empty, this must be the first row
		if (this->header.size() == 0)
			this->header = Row(tokens);

		// if it isn't the first row, make sure it
		// has the right amount of columns
		else if (columns != this->header.size())
			throw std::runtime_error("column count inconsistent");

		// otherwise, add it to the rows list
		else
			this->rows.push_back(DataRow(tokens));
	}

	if (this->rows.size() == 0)
		throw std::runtime_error("no rows in input");
}

void Tool::create_columns()
{
	if (this->user_align) {
		if (this->header.size() != this->user_alignments.size())
			throw std::runtime_error("column count does not match alignments given");
	}

	for (size_t i = 0; i < this->header.size(); i++) {
		const std::string& name = header[i];
		enum alignment align;

		if (this->user_align) {
			align = user_alignments[i];
			this->columns.push_back(Column(name, align));
		} else {
			this->columns.push_back(Column(name));
		}
	}

	// roll through the data and figure out what kind
	// of data is in each column
	std::vector<DataRow>::const_iterator iter;
	for (iter = this->rows.begin(); iter != this->rows.end(); iter++) {
		for (size_t i = 0; i < (*iter).size(); i++) {
			this->columns[i].check_alignment((*iter).get_type(i));
		}
	}
}

void Tool::build_table()
{
	std::vector<Column>::const_iterator citer;
	std::vector<DataRow>::const_iterator riter;

	for (citer = columns.begin(); citer != columns.end(); citer++)
		(*citer).add_to_table(this->table);

	for (riter = rows.begin(); riter != rows.end(); riter++)
		(*riter).add_to_table(this->table);
}

void Tool::write_table()
{
	int err;
	enum ptab_format fmt;

	switch (this->format) {
	case FORMAT_ASCII:
		fmt = PTAB_ASCII;
		break;

	case FORMAT_UNICODE:
		fmt = PTAB_UNICODE;
		break;
	}

	err = ptab_dumpf(this->table, out_stream, fmt);
	if (err)
		throw std::runtime_error("ptab_dumpf error");
}
