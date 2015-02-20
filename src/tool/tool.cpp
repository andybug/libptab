
#include <cassert>

#include <sstream>
#include <stdexcept>

#include "tool.hpp"

using namespace ptabtool;

Tool::Tool()
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

Tool::~Tool()
{
	ptab_free(&this->table);
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

void Tool::read_input(std::istream& stream)
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

bool Tool::is_numeric(const std::string& str)
{
	std::stringstream stream(str);
	double temp;

	if (!(stream >> temp).fail())
		return true;

	return false;
}

void Tool::find_column_types()
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

void Tool::create_column(std::string& name, enum alignment align)
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

enum ptabtool::alignment Tool::get_default_alignment(enum type t)
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

void Tool::build_columns()
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

void Tool::build_table()
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

void Tool::write_table(FILE *stream)
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

