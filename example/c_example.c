/* CUTL - A simple C unit testing library.
 * AUTHOR: Baptiste "Saend" CEILLIER
 * OVERVIEW: This is a simple example showing the basic functions.
 * Can be built with: `cc c_example.c -lcutl`
 */
#include "cutl.h"


// TEST EXAMPLE

void empty_test(cutl_t *cutl, void *data) {
	cutl_info(
		cutl, "This is the simplest test possible. It prints this message."
	);

	cutl_info(
		cutl, "And since it doesn't assert anything, it won't ever fail."
	);
}

void simple_test(cutl_t *cutl, void *data) {
	int condition = 1;
	
	cutl_assert(
		cutl, condition, 
		"Since the condition is true, the test doesn't fail."
	);

	// These macros do the same thing, but are easier to use.
	cutl_assert_true(cutl, condition); 	
	cutl_assert_equal(cutl, condition, 1);

	cutl_info(cutl, "An assert which doesn't fail is never displayed.");
}


// FAIL EXAMPLES

void failure1(cutl_t *cutl, void *data) {
	int condition = 0;
	cutl_assert(
		cutl, condition, 
		"Since the condition is false this assert and test fail."
	);

	cutl_info(cutl, "The function is stoppped after the failed assert.");
	cutl_info(cutl, "That's why these lines are never reached.");
}

void failure2(cutl_t *cutl, void *data) {
	cutl_fail(cutl, "This will always fail.");
	cutl_info(cutl, "This line will never be reached.");
}

void failure3(cutl_t *cutl, void *data) {
	cutl_message(
		cutl, CUTL_FAIL, 
		"This message causes the test to be marked as failed."
	);
	
	cutl_info(cutl, "But the execution of the test continues.");
}

void failure_suite(cutl_t *cutl, void *data) {
	cutl_test(cutl, failure1);
	cutl_test(cutl, failure2);
	cutl_test(cutl, failure3);
	
	cutl_info(cutl, "This suite of test fails because at least one test failed.");
}


// BEFORE AND AFTER FUNCTIONS AND TESTING DATA

void before(cutl_t *cutl, void *data) {
	cutl_assert(cutl, data, "This function needs data.");
	cutl_info(cutl, "Before");
}

void echo(cutl_t *cutl, void *data) {
	cutl_info(cutl, (char*) data);
}

void after(cutl_t *cutl, void *data) {
	cutl_info(cutl, "After");
}

void beforeafter_suite(cutl_t *cutl, void *data) {
	cutl_before(cutl, before);
	cutl_after(cutl, after);

	cutl_testdata(cutl, echo, "During");
	cutl_test(cutl, echo);
}


// MAIN FUNCTION

int main(void) {
	cutl_t *cutl = cutl_new("Example tests");
	
	cutl_test(cutl, empty_test);
	cutl_test(cutl, simple_test);

	cutl_suite(cutl, failure_suite);		
	cutl_suite(cutl, beforeafter_suite);

	cutl_summary(cutl);
	cutl_free(cutl);
	return 0;
}
