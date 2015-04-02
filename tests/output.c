
#include <check.h>
#include <ptab.h>

#include "../src/libptab/internal.h"

static ptab_t *p;
static int err;

static void fixture_init(void)
{
	p = ptab_init(NULL);

	ptab_column(p, "Name", PTAB_STRING);
	ptab_column(p, "Integer", PTAB_INTEGER);
	ptab_column(p, "Floating", PTAB_FLOAT);
	ptab_column(p, "I2", PTAB_INTEGER);

	ptab_column_align(p, 1, PTAB_LEFT);

	ptab_begin_row(p);
	ptab_row_data_s(p, "Longer");
	ptab_row_data_i(p, "%d*", 0);
	ptab_row_data_f(p, "%0.3f", 0.3211);
	ptab_row_data_i(p, "%d", 100);
	ptab_end_row(p);

	ptab_begin_row(p);
	ptab_row_data_s(p, "A");
	ptab_row_data_i(p, "test %d", 0);
	ptab_row_data_f(p, "%0.1f", 0.3211);
	ptab_row_data_i(p, "%d", 1000);
	ptab_end_row(p);
}

static void fixture_free(void)
{
	ptab_free(p);
}

START_TEST (output_file)
{
	FILE *f;

	f = fopen("/dev/null", "w");

	err = ptab_dumpf(p, f, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_dumpf(p, f, PTAB_UNICODE);
	ck_assert_int_eq(err, PTAB_OK);

	fclose(f);
}
END_TEST

START_TEST (output_file_null)
{
	FILE *f;

	f = fopen("/dev/null", "w");

	err = ptab_dumpf(NULL, f, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_dumpf(p, NULL, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_ENULL);

	fclose(f);
}
END_TEST

START_TEST (output_file_format)
{
	FILE *f;

	f = fopen("/dev/null", "w");

	err = ptab_dumpf(p, f, -1);
	ck_assert_int_eq(err, PTAB_EFORMAT);

	err = ptab_dumpf(p, f, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_dumpf(p, f, PTAB_UNICODE);
	ck_assert_int_eq(err, PTAB_OK);

	fclose(f);
}
END_TEST

START_TEST (output_file_mem)
{
	FILE *f;

	f = fopen("/dev/null", "w");

	ptab__mem_disable(p);
	err = ptab_dumpf(p, f, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_EMEM);
	ptab__mem_enable(p);

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

	err = ptab_dumps(p, &string, PTAB_ASCII);
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

	err = ptab_dumps(p, &string, PTAB_UNICODE);
	ck_assert_int_eq(err, PTAB_OK);

	diff = strncmp(string.str, expected_output, string.len);
	ck_assert_int_eq(diff, 0);
}
END_TEST

START_TEST (output_string_null)
{
	ptab_string_t string;

	err = ptab_dumps(NULL, &string, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_dumps(p, NULL, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (output_string_format)
{
	ptab_string_t string;

	err = ptab_dumps(p, &string, -1);
	ck_assert_int_eq(err, PTAB_EFORMAT);

	err = ptab_dumps(p, &string, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_dumps(p, &string, PTAB_UNICODE);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (output_string_mem)
{
	ptab_string_t s;

	ptab__mem_disable(p);
	err = ptab_dumps(p, &s, PTAB_ASCII);
	ck_assert_int_eq(err, PTAB_EMEM);
	ptab__mem_enable(p);
}
END_TEST

START_TEST (output_string_free)
{
	ptab_string_t string;

	ptab_dumps(p, &string, PTAB_ASCII);

	err = ptab_free_string(p, &string);
	ck_assert_int_eq(err, PTAB_OK);

	/* make sure that it can handle freeing it twice */
	err = ptab_free_string(p, &string);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (output_string_free_null)
{
	ptab_string_t string;
	ptab_string_t string2;

	ptab_dumps(p, &string, PTAB_ASCII);

	err = ptab_free_string(NULL, &string);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_free_string(p, NULL);
	ck_assert_int_eq(err, PTAB_ENULL);

	string2.str = NULL;
	err = ptab_free_string(p, &string2);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_free_string(p, &string);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (output_string_free_multi)
{
	ptab_string_t str1;
	ptab_string_t str2;

	ptab_dumps(p, &str1, PTAB_ASCII);
	ptab_dumps(p, &str2, PTAB_ASCII);

	err = ptab_free_string(p, &str1);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_free_string(p, &str2);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

TCase *output_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Output");
	tcase_add_checked_fixture(tc, fixture_init, fixture_free);
	tcase_add_test(tc, output_file);
	tcase_add_test(tc, output_file_null);
	tcase_add_test(tc, output_file_format);
	tcase_add_test(tc, output_file_mem);
	tcase_add_test(tc, output_string_ascii);
	tcase_add_test(tc, output_string_unicode);
	tcase_add_test(tc, output_string_null);
	tcase_add_test(tc, output_string_format);
	tcase_add_test(tc, output_string_mem);
	tcase_add_test(tc, output_string_free);
	tcase_add_test(tc, output_string_free_null);
	tcase_add_test(tc, output_string_free_multi);

	return tc;
}
