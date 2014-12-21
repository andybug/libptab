
#include <ptab.h>

struct error_description {
	int error_code;
	const char *description;
};

static struct error_description descriptions[] = {
	{ PTAB_OK,	      "There is no error here"			},
	{ PTAB_EOF,	      "End of the output stream"		},
	{ PTAB_ENULL,	      "Received NULL parameter"			},
	{ PTAB_ENOMEM,	      "Call to allocator returned NULL"		},
	{ PTAB_EINIT,	      "ptab structure not initialized"		},
	{ PTAB_ETYPEFLAGS,    "Too many type flags provided (or none)"	},
	{ PTAB_EALIGNFLAGS,   "Too many alignment flags provided"	},
	{ PTAB_EROWS,	      "Rows already defined"			},
	{ PTAB_ENOCOLUMNS,    "No columns defined"			},
	{ PTAB_EROWBEGAN,     "Row already in process of being defined"	},
	{ PTAB_ENUMCOLUMNS,   "Column count exceeded"			},
	{ PTAB_ETYPE,         "Column is of different type"		},
	{ PTAB_ENOROWBEGAN,   "No row is currently being defined",	},
	{ PTAB_EFORMATFLAGS,  "Too many or illegal format flags",	}
};

const char *ptab_strerror(int err)
{
	int num_descriptions;

	num_descriptions = sizeof(descriptions) /
			   sizeof(struct error_description);

	if (err > 0 || (-err) >= num_descriptions)
		return NULL;

	return descriptions[-err].description;
}
