
#include <sstream>

#include "row.hpp"

using namespace ptabtool;


Row::Row(const std::vector<std::string>& data)
	: data(data)
{
}

Row::~Row()
{
}

const std::string& Row::operator[](int index)
{
	return data[index];
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

enum type DataRow::find_type(const std::string& str)
{
	std::stringstream stream(str);
	double temp;

	if (!(stream >> temp).fail())
		return TYPE_NUMERIC;

	return TYPE_STRING;
}
