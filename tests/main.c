
#include <stdlib.h>
#include <check.h>

#include "test_cases.h"

typedef TCase *(*test_case_generator)(void);

test_case_generator test_cases[] = {
	version_test_case,
	error_test_case,
	init_test_case,
	NULL
};

static Suite *build_suite(void)
{
	TCase *tc;
	Suite *s;
	int i;

	s = suite_create("libptab Test Suite");

	for (i = 0; test_cases[i]; i++) {
		tc = test_cases[i]();
		suite_add_tcase(s, tc);
	}

	return s;
}

int main(void)
{
	Suite *s;
	SRunner *sr;
	int failed;

	s = build_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
