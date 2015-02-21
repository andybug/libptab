#ifndef PTABTOOL_COLUMN_HPP
#define PTABTOOL_COLUMN_HPP

#include <string>
#include <ptab.h>

#include "types.hpp"


namespace ptabtool {

	class Column {
	public:
		Column(const std::string& name);
		Column(const std::string& name, enum alignment a);
		virtual ~Column();

		void check_alignment(enum type t);
		void add_to_table(ptab_t *p) const;


	private:
		std::string name;
		bool is_mutable;
		enum alignment align;
	};
}

#endif
