
#include <stdexcept>

#include "column.hpp"

using namespace ptabtool;


Column::Column(const std::string& name)
	: name(name), is_mutable(true)
{
	align = ALIGN_RIGHT;
}

Column::Column(const std::string& name, enum alignment a)
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

void Column::add_to_table(ptab_t *table) const
{
	int err;

	/* FIXME */
#if 0
	switch (this->align) {
	case ALIGN_LEFT:
		flags |= PTAB_LEFT;
		break;

	case ALIGN_RIGHT:
		flags |= PTAB_RIGHT;
		break;

	case ALIGN_CENTER:
	default:
		break;
	}
#endif

	err = ptab_column(table, this->name.c_str(), PTAB_STRING);
	if (err)
		throw std::runtime_error("ptab_column error");
}
