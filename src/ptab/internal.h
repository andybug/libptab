#ifndef INTERNAL_H
#define INTERNAL_H

#include <ptab.h>

struct params {
	char delim;
	enum ptab_format format;
	enum ptab_align *aligns;
	int num_aligns;
};

#endif
