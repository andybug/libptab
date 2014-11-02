
#include <ptab.h>

struct error_description {
	int error_code;
	const char *description;
};

static struct error_description descriptions[] = {
	{ PTAB_OK, "There is no error here" }
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
