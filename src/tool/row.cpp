
#include <sstream>
#include <stdexcept>

#include "row.hpp"

using namespace ptabtool;


Row::Row()
{
}

Row::Row(const std::vector<std::string>& data)
	: data(data)
{
}

Row::~Row()
{
}

const std::string& Row::operator[](int index) const
{
	return data[index];
}

size_t Row::size() const
{
	return data.size();
}

void Row::add_to_table(ptab_t *p) const
{
	int err;

	err = ptab_begin_row(p);
	if (err)
		throw std::runtime_error("ptab_begin_row error");

	std::vector<std::string>::const_iterator iter;
	for (iter = data.begin(); iter != data.end(); iter++) {
		err = ptab_row_data_s(p, (*iter).c_str());
		if (err)
			throw std::runtime_error("ptab_row_data_s error");
	}

	err = ptab_end_row(p);
	if (err)
		throw std::runtime_error("ptab_end_row error");
}


DataRow::DataRow(const std::vector<std::string>& data)
	: Row(data)
{
	std::vector<std::string>::const_iterator iter;

	for (iter = data.begin(); iter != data.end(); iter++)
		types.push_back(find_type(*iter));
}

DataRow::~DataRow()
{
}

enum type DataRow::get_type(int i) const
{
	return types[i];
}

enum type DataRow::find_type(const std::string& str) const
{
	std::stringstream stream(str);
	double temp;

	if (!(stream >> temp).fail())
		return TYPE_NUMERIC;

	return TYPE_STRING;
}
