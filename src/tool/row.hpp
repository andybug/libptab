#ifndef PTABTOOL_ROW_HPP
#define PTABTOOL_ROW_HPP

#include <string>
#include <vector>

#include "types.hpp"


namespace ptabtool {

	class Row {
	public:
		Row(const std::vector<std::string>& data);
		~Row();

		const std::string& operator[](int index);


	private:
		std::vector<std::string> data;
	};


	class DataRow : public Row {
	public:
		DataRow(const std::vector<std::string>& data);
		~DataRow();

		enum type get_type(int index);


	private:
		std::vector<enum type> types;

		enum type find_type(const std::string& str);
	};
}

#endif
