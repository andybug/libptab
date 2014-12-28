
#include <check.h>
#include <ptab.h>

static ptab_t p;
static int err;

static void fixture_init(void)
{
	memset(&p, 0, sizeof(ptab_t));
	ptab_init(&p, NULL);

	ptab_column(&p, "Name", PTAB_STRING);
	ptab_column(&p, "Integer", PTAB_INTEGER | PTAB_ALIGN_LEFT);
	ptab_column(&p, "Floating", PTAB_FLOAT);
	ptab_column(&p, "I2", PTAB_INTEGER);

	ptab_begin_row(&p);
	ptab_row_data_s(&p, "Longer");
	ptab_row_data_i(&p, "%d*", 0);
	ptab_row_data_f(&p, "%0.3f", 0.3211);
	ptab_row_data_i(&p, "%d", 100);
	ptab_end_row(&p);

	ptab_begin_row(&p);
	ptab_row_data_s(&p, "A");
	ptab_row_data_i(&p, "test %d", 0);
	ptab_row_data_f(&p, "%0.1f", 0.3211);
	ptab_row_data_i(&p, "%d", 1000);
	ptab_end_row(&p);
}

static void fixture_free(void)
{
	ptab_free(&p);
}

START_TEST (output_file)
{
	FILE *f;

	f = fopen("/dev/null", "w");

	err = ptab_dumpf(&p, f, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_dumpf(&p, f, PTAB_UNICODE);
	ck_assert_int_eq(err, PTAB_OK);

	fclose(f);
}
END_TEST

START_TEST (output_string_ascii)
{
	static const char expected_output[] =
		"+--------+---------+----------+------+\n"
		"| Name   | Integer | Floating | I2   |\n"
		"+--------+---------+----------+------+\n"
		"| Longer | 0*      |    0.321 |  100 |\n"
		"| A      | test 0  |      0.3 | 1000 |\n"
		"+--------+---------+----------+------+\n";
	ptab_string_t string;
	int diff;

	err = ptab_dumps(&p, &string, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_OK);

	diff = strncmp(string.str, expected_output, string.len);
	ck_assert_int_eq(diff, 0);
}
END_TEST

START_TEST (output_string_unicode)
{
	static const char expected_output[] =
		"\u250c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500"
		"\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500\u2500\u2500"
		"\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500\u2500\u2500\u2510\n"
		"\u2502 Name   \u2502 Integer \u2502 Floating \u2502 I2   \u2502\n"
		"\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500"
		"\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500\u2500\u2500"
		"\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500\u2500\u2500\u2524\n"
		"\u2502 Longer \u2502 0*      \u2502    0.321 \u2502  100 \u2502\n"
		"\u2502 A      \u2502 test 0  \u2502      0.3 \u2502 1000 \u2502\n"
		"\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500"
		"\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500\u2500\u2500"
		"\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500\u2500\u2500\u2518\n";
	ptab_string_t string;
	int diff;

	err = ptab_dumps(&p, &string, PTAB_UNICODE);
	ck_assert_int_eq(err, PTAB_OK);

	diff = strncmp(string.str, expected_output, string.len);
	ck_assert_int_eq(diff, 0);
}
END_TEST

TCase *output_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Output");
	tcase_add_checked_fixture(tc, fixture_init, fixture_free);
	tcase_add_test(tc, output_file);
	tcase_add_test(tc, output_string_ascii);
	tcase_add_test(tc, output_string_unicode);

	return tc;
}
