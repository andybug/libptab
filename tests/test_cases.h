#ifndef TEST_CASES_H
#define TEST_CASES_H

#include <check.h>

extern TCase *version_test_case(void);
extern TCase *error_test_case(void);
extern TCase *init_test_case(void);
extern TCase *free_test_case(void);
extern TCase *column_test_case(void);
extern TCase *begin_row_test_case(void);
extern TCase *row_data_s_test_case(void);
extern TCase *row_data_i_test_case(void);
extern TCase *row_data_f_test_case(void);
extern TCase *end_row_test_case(void);
extern TCase *output_test_case(void);

#endif
