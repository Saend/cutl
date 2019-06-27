#include "tests.h"


// MY PASS TEST

static void My_pass_test(Cutl *cutl, void *unused)
{
	cutl_assert_true(cutl, true);
}

static void My_pass_subsuite(Cutl *cutl, void *unused)
{
	cutl_run(cutl, "test1", My_pass_test, NULL);
	cutl_run(cutl, "test2", My_pass_test, NULL);
}

static void My_pass_suite(Cutl *cutl, void *unused)
{
	cutl_run(cutl, "subsuite1", My_pass_subsuite, NULL);
	cutl_run(cutl, "subsuite2", My_pass_subsuite, NULL);
}



/** Passing test summary.
 */
static void success_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SUMMARY);
	cutl_run(fix->cutl, "suite", My_pass_suite, NULL);

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "Unit tests summary: 0 failed, 4 passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, failed);
}


/** Silent passing test summary.
 */
static void success_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SILENT);
	cutl_run(fix->cutl, "suite", My_pass_suite, NULL);

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, failed);
}


/** Passing top-level test summary.
 */
static void success_toplevel_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SUMMARY);

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "Unit tests summary: 0 failed, 0 passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, failed);
}



// MY FAIL TEST

static void My_fail_test(Cutl *cutl, void *unused)
{
	cutl_message_at(cutl, CUTL_FAIL, NULL, 0, "My failure");
}

static void My_fail_subsuite(Cutl *cutl, void *unused)
{
	cutl_run(cutl, "test1", My_fail_test, NULL);
	cutl_run(cutl, "test2", My_pass_test, NULL);
}

static void My_fail_suite(Cutl *cutl, void *unused)
{
	cutl_run(cutl, "subsuite1", My_fail_subsuite, NULL);
	cutl_run(cutl, "subsuite2", My_fail_subsuite, NULL);
}



/** Failing test summary.
 */
static void failure_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SUMMARY);
	cutl_run(fix->cutl, "suite", My_fail_suite, NULL);

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "Unit tests summary: 2 failed, 2 passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_equal(cutl, failed, 2);
}


/** Silent failing test summary.
 */
static void failure_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SILENT);
	cutl_run(fix->cutl, "suite", My_fail_suite, NULL);

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_equal(cutl, failed, 2);
}


/** Failing top-level test summary.
 */
static void failure_toplevel_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SUMMARY);
	cutl_message_at(fix->cutl, CUTL_FAIL, NULL, 0, "Failure");

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "Unit tests summary: 1 failed, 0 passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_equal(cutl, failed, 1);
}



// MY ERROR TEST

static void My_error_test(Cutl *cutl, void *unused)
{
	cutl_message_at(cutl, CUTL_ERROR, NULL, 0, "My error");
}

static void My_error_subsuite(Cutl *cutl, void *unused)
{
	cutl_run(cutl, "test1", My_fail_test, NULL);
	cutl_run(cutl, "test2", My_pass_test, NULL);
	cutl_run(cutl, "test3", My_error_test, NULL);
}

static void My_error_suite(Cutl *cutl, void *unused)
{
	cutl_run(cutl, "subsuite1", My_error_subsuite, NULL);
	cutl_run(cutl, "subsuite2", My_error_subsuite, NULL);
}


/** Aborted test summary.
 */
static void canceled_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SUMMARY);
	cutl_run(fix->cutl, "suite", My_error_suite, NULL);

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "Unit tests summary: canceled.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_equal(cutl, failed, 2);
}


/** Aborted test summary.
 */
static void canceled_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SILENT);
	cutl_run(fix->cutl, "suite", My_error_suite, NULL);

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_equal(cutl, failed, 2);
}


/** Aborted top-level test summary.
 */
static void canceled_toplevel_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SUMMARY);
	cutl_message_at(fix->cutl, CUTL_ERROR, NULL, 0, "ERROR");

	// Function under test
	int failed = cutl_summary(fix->cutl);

	// Asserts
	const char *expected = "Unit tests summary: canceled.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, failed, cutl_get_failed(fix->cutl));
	cutl_assert_equal(cutl, failed, 1);
}



// SUMMARY SUITE

void cutl_summary_suite(Cutl *cutl)
{
	cutl_at_start(cutl, fixture_setup, NULL);
	cutl_at_end(cutl, fixture_clean, NULL);

	cutl_test(cutl, success_test);
	cutl_test(cutl, success_silent_test);
	cutl_test(cutl, success_toplevel_test);

	cutl_test(cutl, failure_test);
	cutl_test(cutl, failure_silent_test);
	cutl_test(cutl, failure_toplevel_test);

	cutl_test(cutl, canceled_test);
	cutl_test(cutl, canceled_silent_test);
	cutl_test(cutl, canceled_toplevel_test);

}
