#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include "strings.h"

typedef void (*test_function_t)(void);


int init_unit_test(const char *file_prefix)
{
	if (CU_initialize_registry())
    	return -1;
    CU_set_output_filename(file_prefix);
    CU_set_error_action(CUEA_IGNORE);
    return 0;
}

int add_unit_test_suite(const char *suite_name, void **suite)
{
	CU_Suite **pSuite = reinterpret_cast<CU_Suite **>(suite);
	*pSuite = CU_add_suite(suite_name, NULL, NULL);

	if (!(*pSuite))
		return -1;

	return 0;
}

int add_unit_test_cases(void *suite, const char *test_name, test_function_t func)
{
	CU_Suite *pSuite = static_cast<CU_Suite *>(suite);

	if (CU_add_test(pSuite, test_name, func) == NULL)
		return -1;
	CU_basic_set_mode(CU_BRM_VERBOSE);
	return 0;
}

void run_unit_test()
{
   CU_basic_run_tests();
}

void deinit_unit_test()
{
	CU_cleanup_registry();
}


void string_test(void)
{
#define STRING_BUFFER_TEST	52
	char str_buf[STRING_BUFFER_TEST];
	const char abhi[] = "ABHISHEK";
	hoi :: string a(str_buf, STRING_BUFFER_TEST);

	a.clear();

	a.copy(abhi);
	CU_ASSERT_STRING_EQUAL(a.c_str(), "ABHISHEK");
	CU_ASSERT_EQUAL(a.size(), STRING_BUFFER_TEST);
	CU_ASSERT_EQUAL(a.length(), strlen(abhi));

	a.toLowerCase();
	CU_ASSERT_STRING_EQUAL(a.c_str(), "abhishek");

	a.toUpperCase();
	CU_ASSERT_STRING_EQUAL(a.c_str(), "ABHISHEK");

	a.concat(" Arora");
	CU_ASSERT_STRING_EQUAL(a.c_str(), "ABHISHEK Arora");

	CU_ASSERT_EQUAL(a.indexOf('I'), 3);
	CU_ASSERT_EQUAL(a.indexOf('H', a.indexOf('H') + 1), 5);
	CU_ASSERT_EQUAL(a.lastIndexOf('r'), 12);

	a.replace('r', 't');
	CU_ASSERT_STRING_EQUAL(a.c_str(), "ABHISHEK Atota");

	CU_ASSERT_EQUAL(a.compareTo(abhi), ' ');
	CU_ASSERT_EQUAL(a.compareTo("ABHISHEK Atotb"), 'a' - 'b');
	CU_ASSERT_EQUAL(a.compareTo("ABHISHEK Atota"), 0);

	a.clear();
	a.copy("1234");
	int v;
	a.toInt(v);
	CU_ASSERT_EQUAL(v, 1234);

	a.clear();
	a.copy("1.55");
	float f;
	a.toFloat(f);
	CU_ASSERT_DOUBLE_EQUAL(f, 1.55, 0.000001);

	a.clear();
	a.copy("     space      ");
	a.trim();
	CU_ASSERT_STRING_EQUAL(a.c_str(), "space");

}

#define FILENAME_PREFIX			"ds_test"

int main()
{
	if (init_unit_test(FILENAME_PREFIX)) {
		std :: cerr << "Failed to init Unit Testing Module" << std :: endl;
		return -1;
	}

	void *suite;
	if (add_unit_test_suite(FILENAME_PREFIX, &suite)) {
		std :: cerr << "Failed to create Suite" << std :: endl;
		return -1;
	}

	if (add_unit_test_cases(suite, "string_test", string_test)) {
		std :: cerr << "Failed to create test case" << std :: endl;
		return -1;
	}

	run_unit_test();
	deinit_unit_test();

	return 0;
}