#include "tests.h"



// MY TEST FUNCTIONS

// Test states.
enum {
	NOT_EXECUTED = 0,
	EXECUTED,
	FINISHED,
};

static void My_normal(Cutl *cutl, void *data)
{
	int *state = data;
	(*state)++;
	cutl_message_at(cutl, CUTL_INFO, NULL, 0, "Message");
	(*state)++;
}

static void My_softerror(Cutl *cutl, void *data)
{
	int *state = data;
	(*state)++;
	cutl_message_at(cutl, CUTL_ERROR, NULL, 0, "Soft Error");
	(*state)++;
}

static void My_harderror(Cutl *cutl, void *data)
{
	int *state = data;
	(*state)++;
	cutl_error_at(cutl, NULL, 0, "Hard Error");
	(*state)++;
}

static void My_softfail(Cutl *cutl, void *data)
{
	int *state = data;
	(*state)++;
	cutl_message_at(cutl, CUTL_FAIL, NULL, 0, "Soft Failure");
	(*state)++;
}

static void My_hardfail(Cutl *cutl, void *data)
{
	int *state = data;
	(*state)++;
	cutl_fail_at(cutl, NULL, 0, "Hard Failure");
	(*state)++;
}

static void My_interrupt(Cutl *cutl, void *data)
{
	int *state = data;
	(*state)++;
	cutl_interrupt(cutl);
	(*state)++;
}



// ERROR AT: START

/** Display error message inside `at_start`.
 */
static void start_softerror_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_softerror, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, NOT_EXECUTED);
	cutl_assert_equal(cutl, end_state, NOT_EXECUTED);
}


/** Error inside `at_start`.
 */
static void start_harderror_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_harderror, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, EXECUTED);
	cutl_assert_equal(cutl, test_state, NOT_EXECUTED);
	cutl_assert_equal(cutl, end_state, NOT_EXECUTED);
}



// FAIL AT: START

/** Display fail message inside `at_start`.
 */
static void start_softfail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_softfail, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, NOT_EXECUTED);
	cutl_assert_equal(cutl, end_state, NOT_EXECUTED);
}


/** Fail inside `at_start`.
 */
static void start_hardfail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_hardfail, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, EXECUTED);
	cutl_assert_equal(cutl, test_state, NOT_EXECUTED);
	cutl_assert_equal(cutl, end_state, NOT_EXECUTED);
}



// INTERRUPT AT: START

/** Error inside `at_start`.
 */
static void start_interrupt_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_interrupt, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, EXECUTED);
	cutl_assert_equal(cutl, test_state, FINISHED);
	cutl_assert_equal(cutl, end_state, FINISHED);
}



// ERROR AT: TEST

/** Display error message inside test.
 */
static void test_softerror_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	cutl_run(fix->cutl, "test", My_softerror, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, FINISHED);
	cutl_assert_equal(cutl, end_state, FINISHED);
}


/** Error inside test.
 */
static void test_harderror_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	cutl_run(fix->cutl, "test", My_harderror, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, EXECUTED);
	cutl_assert_equal(cutl, end_state, FINISHED);
}



// FAIL AT: TEST

/** Display fail message inside test.
 */
static void test_softfail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_softfail, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, FINISHED);
	cutl_assert_equal(cutl, end_state, FINISHED);
}


/** Fail inside test.
 */
static void test_hardfail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_hardfail, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, EXECUTED);
	cutl_assert_equal(cutl, end_state, FINISHED);
}



// INTERRUPT AT: TEST

/** Error inside `at_start`.
 */
static void test_interrupt_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_normal, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_interrupt, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, EXECUTED);
	cutl_assert_equal(cutl, end_state, FINISHED);
}



// ERROR AT: END

/** Display error message inside `at_end`.
 */
static void end_softerror_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_softerror, &end_state);

	// Function under test
	cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, FINISHED);
	cutl_assert_equal(cutl, end_state, FINISHED);
}


/** Error inside `at_end`.
 */
static void end_harderror_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_harderror, &end_state);

	// Function under test
	cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, FINISHED);
	cutl_assert_equal(cutl, end_state, EXECUTED);
}



// FAIL AT: END

/** Display fail message inside `at_end`.
 */
static void end_softfail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_softfail, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, FINISHED);
	cutl_assert_equal(cutl, end_state, FINISHED);
}


/** Fail inside `at_end`.
 */
static void end_hardfail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_hardfail, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, FINISHED);
	cutl_assert_equal(cutl, end_state, EXECUTED);
}



// INTERRUPT AT: END

/** Error inside `at_start`.
 */
static void end_interrupt_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	int start_state = NOT_EXECUTED;
	int test_state = NOT_EXECUTED;
	int end_state = NOT_EXECUTED;
	cutl_at_start(fix->cutl, My_normal, &start_state);
	cutl_at_end(fix->cutl, My_interrupt, &end_state);

	// Function under test
	int failed = cutl_run(fix->cutl, "test", My_normal, &test_state);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), failed);
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));

	cutl_assert_equal(cutl, start_state, FINISHED);
	cutl_assert_equal(cutl, test_state, FINISHED);
	cutl_assert_equal(cutl, end_state, EXECUTED);
}



// ERRONEOUS USES

// MY TESTS FUNCTIONS

static void My_bad_child(Cutl *cutl, void *parent)
{
	cutl_run(parent, NULL, My_normal, NULL);
}

static void My_parent(Cutl *cutl, void *data)
{
	cutl_run(cutl, "child", My_bad_child, cutl);
}



/** Use parent test.
 */
static void use_parent_test(Cutl *cutl, Fixture *fix)
{
	// Function under test
	cutl_set_verbosity(fix->cutl, CUTL_ERROR);
	cutl_run(fix->cutl, "parent", My_parent, NULL);

	// Asserts
	const char *expected =
		"child:\n"
		"	[ERROR cutl_run()] Bad test context: running child.\n"
		"child canceled.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
}



// RUN SUITE

void cutl_run_suite(Cutl *cutl)
{
	cutl_at_start(cutl, fixture_setup, NULL);
	cutl_at_end(cutl, fixture_clean, NULL);

	cutl_test(cutl, start_softerror_test);
	cutl_test(cutl, start_harderror_test);
	cutl_test(cutl, start_softfail_test);
	cutl_test(cutl, start_hardfail_test);
	cutl_test(cutl, start_interrupt_test);

	cutl_test(cutl, test_softerror_test);
	cutl_test(cutl, test_harderror_test);
	cutl_test(cutl, test_softfail_test);
	cutl_test(cutl, test_hardfail_test);
	cutl_test(cutl, test_interrupt_test);

	cutl_test(cutl, end_softerror_test);
	cutl_test(cutl, end_harderror_test);
	cutl_test(cutl, end_softfail_test);
	cutl_test(cutl, end_hardfail_test);
	cutl_test(cutl, end_interrupt_test);

	cutl_test(cutl, use_parent_test);
}
