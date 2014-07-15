#include "cutl.h"
#include "lutl.h"

void test0(cutl_t *cutl, void *data) {
	cutl_message(cutl, "This test is empty.", CUTL_INFO, NULL, 0);
}

void test1(cutl_t *cutl, void *data) {
	cutl_assert_true(cutl, 0);
	cutl_assert_true(cutl, 0);
}

void test2(cutl_t *cutl, void *data) {
	cutl_assert_true(cutl, 1);
	cutl_assert_true(cutl, 0);
}

void test3(cutl_t *cutl, void *data) {
	cutl_assert_true(cutl, 1);
	cutl_assert_true(cutl, 1);
}

void test4(cutl_t *cutl, void *data) {
	cutl_fail(cutl, "oops", NULL, 0);
}

void suite1(cutl_t *cutl, void *data) {
	cutl_test(cutl, test1);
	cutl_test(cutl, test2);
	cutl_test(cutl, test3);
	cutl_test(cutl, test4);
}

void test_data(cutl_t *cutl, void *data) {
	cutl_assert_true(cutl, data != NULL);
}

void setup_bool(cutl_t *cutl, void *data) {
	bool *value = (bool*)data;
	*value = true;
	cutl_message(cutl, "Value set to true.", CUTL_INFO, NULL, 0);
}

void teardown_bool(cutl_t *cutl, void *data) {
	bool *value = (bool*)data;
	*value = false;
	cutl_message(cutl, "Value set to false.", CUTL_INFO, NULL, 0);
}

void test_bool(cutl_t *cutl, void *data) {
	bool *value = (bool*)data;
	cutl_assert_true(cutl, *value);
}

void suite2(cutl_t *cutl, void *data) {
	bool value = false;
	cutl_setup(cutl, setup_bool);
	cutl_teardown(cutl, teardown_bool);
	
	cutl_testdata(cutl, test_bool, &value);
	cutl_testdata(cutl, test_bool, &value);
	cutl_testdata(cutl, test_data, &value);
	
	cutl_assert_false(cutl, value);
}

void suite3(cutl_t *cutl, void *data) {
	cutl_test(cutl, test0);
	cutl_test(cutl, test3);
	cutl_test(cutl, test3);
}

void suite4(cutl_t *cutl, void *data) {
	cutl_suite(cutl, suite2);
	cutl_suite(cutl, suite3);
}


int main(void) {
	cutl_t *cutl = cutl_new("Example testing suite");
	cutl_verbosity(cutl, CUTL_VERBOSE);	
	cutl_test(cutl, test1);
	cutl_test(cutl, test3);
	cutl_suite(cutl, suite1);
	cutl_suite(cutl, suite4);

	cutl_assert_true(cutl, true);		// raises warning
	cutl_fail(cutl, "", __FILE__, __LINE__);// raises warning
	
	cutl_report(cutl);
	cutl_free(cutl);
	return 0;
}