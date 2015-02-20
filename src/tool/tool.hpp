#ifndef PTABTOOL_TOOL_HPP
#define PTABTOOL_TOOL_HPP

#include <list> // FIXME
#include <string>
#include <vector>
#include <iostream>

#include <ptab.h>


namespace ptabtool {

	enum format {
		FORMAT_ASCII,
		FORMAT_UNICODE
	};

	enum alignment {
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};

	enum type {
		TYPE_STRING,
		TYPE_NUMERIC
	};


	class Tool {
	public:
		Tool();
		virtual ~Tool();
	
		void set_format(const std::string& format_);
		void set_alignments(const std::string& alignments_);
		void set_delimiter(const std::string& delim_);
	
		void read_input(std::istream& stream);
		void build_table();
		void write_table(FILE *stream);
	
	
	private:
		ptab_t table;
		enum format format;
		char delim;
	
		bool user_align;
		std::vector<enum alignment> col_alignments;
		std::vector<enum type> col_types;
		std::list<std::vector<std::string> > tokenized_rows;
	
		unsigned int num_columns;
		unsigned int num_rows;
	
	
		bool is_numeric(const std::string& str);
		void find_column_types();
		void create_column(std::string& name, enum alignment align);
		enum alignment get_default_alignment(enum type t);
		void build_columns();
	};
}

#endif
