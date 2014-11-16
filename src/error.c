
#include <ptab.h>

struct error_description {
	int error_code;
	const char *description;
};

static struct error_description descriptions[] = {
	{ PTAB_OK,	  "There is no error here"		   },
	{ PTAB_EOF,	  "End of the output stream"		   },
	{ PTAB_ENULL,	  "Received NULL parameter"		   },
	{ PTAB_ENOMEM,	  "Call to allocator returned NULL"	   },
	{ PTAB_EINIT,	  "ptab structure not initialized"	   },
	{ PTAB_ENUMTYPE,  "Too many type flags provided (or none)" },
	{ PTAB_ENUMALIGN, "Too many alignment flags provided"	   },
	{ PTAB_EROWS,	  "Rows already defined"		   }
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
