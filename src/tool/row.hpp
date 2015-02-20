#ifndef PTABTOOL_ROW_HPP
#define PTABTOOL_ROW_HPP

#include <string>
#include <vector>

#include <ptab.h>

#include "types.hpp"


namespace ptabtool {

	class Row {
	public:
		Row();
		Row(const std::vector<std::string>& data);
		~Row();

		const std::string& operator[](int index) const;
		size_t size() const;
		void add_to_table(ptab_t *p) const;


	private:
		std::vector<std::string> data;
	};


	class DataRow : public Row {
	public:
		DataRow(const std::vector<std::string>& data);
		~DataRow();

		enum type get_type(int index) const;


	private:
		std::vector<enum type> types;

		enum type find_type(const std::string& str) const;
	};
}

#endif
