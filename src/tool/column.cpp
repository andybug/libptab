
#include <sstream>

#include "column.hpp"

using namespace ptabtool;


Column::Column(const std::string& name)
	: name(name)
{
	align = PTAB_RIGHT;
}

Column::~Column()
{
}

void Column::update_align(const std::string& val)
{
	std::stringstream stream(val);
	double temp;
	bool is_numeric = false;

	if (!(stream >> temp).fail())
		is_numeric = true;

	if (!is_numeric && this->align == PTAB_RIGHT)
		this->align = PTAB_LEFT;
}

enum ptab_align Column::get_align() const
{
	return this->align;
}
