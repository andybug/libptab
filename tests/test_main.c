
#include <stdlib.h>
#include <check.h>

extern Suite * get_libptables_suite(void);

int main(void)
{
	Suite *s;
	SRunner *sr;
	int failed;

	s = get_libptables_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
