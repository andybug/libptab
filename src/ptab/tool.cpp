
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

	this->format = PTAB_ASCII;
	this->delim = '\t';
	this->user_align = false;

	this->in_stream = &std::cin;
	this->out_stream = stdout;
}

Tool::~Tool()
{
	ptab_free(this->table);
}

void Tool::set_format(const std::string &format_)
{
	if (format_ == "ascii")
		this->format = PTAB_ASCII;
	else if (format_ == "unicode")
		this->format = PTAB_UNICODE;
	else
		throw std::runtime_error("invalid format");
}

void Tool::set_alignments(const std::string &alignments_)
{
	std::string::const_iterator iter;

	this->user_align = true;

	for (iter = alignments_.begin(); iter < alignments_.end(); iter++) {
		switch (*iter) {
		case 'l':
			this->user_alignments.push_back(PTAB_LEFT);
			break;

		case 'r':
			this->user_alignments.push_back(PTAB_RIGHT);
			break;

		case 'c':
			throw std::runtime_error(
			    "center alignment not implemented yet");
			break;

		default:
			throw std::runtime_error("invalid align value");
			break;
		}
	}
}

void Tool::set_delimiter(const std::string &delim_)
{
	bool escaped = (delim_.length() == 2) && (delim_[0] == '\\');

	if (delim_.length() >= 2 && !escaped) {
		throw std::runtime_error("delimiter must be a single character "
					 "(or a backslash-escaped special "
					 "character");
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
			throw std::runtime_error(
			    "unknown escape character in delimiter");
			break;
		}
	} else {
		assert(delim_.length() == 1);
		this->delim = delim_[0];
	}
}

void Tool::run()
{
	read_header();
	read_rows();
	write_table();
}

void Tool::read_header()
{
	std::string line;

	if (std::getline(*(this->in_stream), line)) {
		std::stringstream line_stream(line);
		std::string token;
		std::vector<std::string> tokens;

		while (std::getline(line_stream, token, this->delim)) {
			if (!token.empty())
				tokens.push_back(token);
		}

		if (tokens.size() == 0)
			throw std::runtime_error("no input data");

		// create columns from header tokens
		std::vector<std::string>::const_iterator iter;
		for (iter = tokens.begin(); iter != tokens.end(); iter++) {
			this->columns.push_back(Column(*iter));

			int err = ptab_column(
			    this->table, (*iter).c_str(), PTAB_STRING);
			if (err)
				throw std::runtime_error("ptab_column error");
		}

		// make sure the number of alignments given as an argument
		// match the actual number of columns
		if (this->user_align) {
			if (this->columns.size() !=
			    this->user_alignments.size()) {
				throw std::runtime_error(
				    "number of columns does not match number "
				    "of alignments given");
			}
		}
	}
}

void Tool::read_rows()
{
	std::string line;

	while (std::getline(*(this->in_stream), line)) {
		std::stringstream line_stream(line);
		std::string token;
		std::vector<std::string> tokens;

		while (std::getline(line_stream, token, this->delim)) {
			if (!token.empty())
				tokens.push_back(token);
		}

		if (tokens.size() != this->columns.size())
			throw std::runtime_error("column count inconsistent");

		// add row to table
		std::vector<std::string>::const_iterator iter;
		int err;
		int i;

		err = ptab_begin_row(this->table);
		if (err)
			throw std::runtime_error("ptab_begin_row error");

		for (iter = tokens.begin(), i = 0; iter != tokens.end();
		     iter++, i++) {
			err = ptab_row_data_s(this->table, (*iter).c_str());
			if (err)
				throw std::runtime_error(
				    "ptab_row_data_s error");

			this->columns[i].update_align(*iter);
		}

		err = ptab_end_row(this->table);
		if (err)
			throw std::runtime_error("ptab_end_row error");
	}
}

void Tool::write_table()
{
	int err;
	enum ptab_align align;

	for (unsigned int i = 0; i < this->columns.size(); i++) {
		align = this->user_align ? this->user_alignments[i]
					 : this->columns[i].get_align();

		err = ptab_column_align(this->table, i, align);
		if (err)
			throw std::runtime_error("ptab_column_align error");
	}

	err = ptab_dumpf(this->table, out_stream, this->format);
	if (err)
		throw std::runtime_error("ptab_dumpf error");
}
