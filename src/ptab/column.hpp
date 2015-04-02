#ifndef PTABTOOL_COLUMN_HPP
#define PTABTOOL_COLUMN_HPP

#include <string>
#include <ptab.h>


namespace ptabtool
{

class Column
{
      public:
	Column(const std::string &name);
	virtual ~Column();

	void update_align(const std::string &val);
	enum ptab_align get_align() const;


      private:
	std::string name;
	enum ptab_align align;
};
}

#endif
