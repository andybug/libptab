#ifndef PTABTOOL_COLUMN_HPP
#define PTABTOOL_COLUMN_HPP

#include <string>

#include "types.hpp"


namespace ptabtool {

	class Column {
	public:
		Column(std::string& name);
		Column(std::string& name, enum alignment a);
		virtual ~Column();

		void check_alignment(enum type t);


	private:
		const std::string name;
		const bool is_mutable;
		enum alignment align;
	};
}

#endif
