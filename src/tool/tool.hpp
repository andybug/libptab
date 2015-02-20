#ifndef PTABTOOL_TOOL_HPP
#define PTABTOOL_TOOL_HPP

#include <cstdio>

#include <string>
#include <vector>
#include <iostream>

#include <ptab.h>

#include "types.hpp"
#include "column.hpp"
#include "row.hpp"


namespace ptabtool {

	class Tool {
	public:
		Tool();
		virtual ~Tool();
	
		void set_format(const std::string& format_);
		void set_alignments(const std::string& alignments_);
		void set_delimiter(const std::string& delim_);
	
		void run();
	
	
	private:
		ptab_t table;
		enum format format;
		char delim;

		bool user_align;
		std::vector<enum alignment> user_alignments;
	
		std::vector<Column> columns;
		Row header;
		std::vector<DataRow> rows;

		std::istream *in_stream;
		FILE *out_stream;


		void read_input();
		void create_columns();
		void build_table();
		void write_table();
	};
}

#endif
