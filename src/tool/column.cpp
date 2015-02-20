
#include "column.hpp"

using namespace ptabtool;


Column::Column(std::string& name)
	: name(name), is_mutable(true)
{
	align = ALIGN_RIGHT;
}

Column::Column(std::string& name, enum alignment a)
	: name(name), is_mutable(false), align(a)
{
}

Column::~Column()
{
}

void Column::check_alignment(enum type t)
{
	if (!is_mutable)
		return;

	if (t == TYPE_STRING && this->align == ALIGN_RIGHT)
		this->align = ALIGN_LEFT;
}
