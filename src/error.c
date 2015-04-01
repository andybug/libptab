
#include <ptab.h>

struct error_description {
	int error_code;
	const char *description;
};

static struct error_description descriptions[] = {
	{ PTAB_OK, "there is no error here" },
	{ PTAB_ENULL, "received NULL parameter" },
	{ PTAB_EMEM, "call to allocator returned NULL" },
	{ PTAB_EORDER, "called API functions in wrong order" },
	{ PTAB_ERANGE, "parameter out of valid range" },
	{ PTAB_ETYPE, "unknown type or type mismatch" },
	{ PTAB_EALIGN, "unknown alignment" },
	{ PTAB_EFORMAT, "unknown format" },
	{ PTAB_ECOLUMNS, "row data does not match column count" }
};

const char *ptab_strerror(int err)
{
	int num_descriptions;

	num_descriptions =
	    sizeof(descriptions) / sizeof(struct error_description);

	if (err > 0 || (-err) >= num_descriptions)
		return NULL;

	return descriptions[-err].description;
}
