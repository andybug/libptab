
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ptab.h>

static int add_heading(ptab_t *p, FILE *stream)
{
	char buf[1024];
	char *s;
	char *saveptr;
	int num = 0;

	s = fgets(buf, 1024, stream);

	if (s != buf)
		return 0;

	s = strtok_r(buf, "\t\n", &saveptr);
	while (s) {
		ptab_column(p, s, PTAB_STRING);
		num++;

		s = strtok_r(NULL, "\t\n", &saveptr);
	}

	return num;
}

static int add_row(ptab_t *p, FILE *stream)
{
	char buf[1024];
	char *s;
	char *saveptr;

	s = fgets(buf, 1024, stream);

	if (s != buf)
		return 0;

	ptab_begin_row(p);

	s = strtok_r(buf, "\t\n", &saveptr);
	while (s) {
		ptab_row_data_s(p, s);

		s = strtok_r(NULL, "\t\n", &saveptr);
	}

	return (ptab_end_row(p) == PTAB_OK);
}

int main(int argc, char **argv)
{
	ptab_t p;
	int num_columns;
	int num;

	ptab_init(&p, NULL);

	num_columns = add_heading(&p, stdin);

	while (add_row(&p, stdin));

	ptab_dumpf(&p, stdout, PTAB_ASCII);
	ptab_free(&p);

	return EXIT_SUCCESS;
}
