#ifndef PTABTOOL_TYPES_HPP
#define PTABTOOL_TYPES_HPP

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
}

#endif
