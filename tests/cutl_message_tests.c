#include "tests.h"


// MESSAGE TYPE: ERROR

/** Display error message.
 */
static void error_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_ERROR);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_ERROR, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[ERROR file:8] Message\n");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
}


/** Mixed message type.
 */
static void error_fail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_ERROR);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_ERROR|CUTL_FAIL, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[ERROR file:8] Message\n");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
}


/** Disabled message type.
 */
static void error_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_VERBOSE ^ CUTL_ERROR);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_ERROR, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
}


/** Hide line.
 */
static void error_hideline_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_ERROR);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_ERROR, "file", 0, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[ERROR file] Message\n");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
}


/** Hide file and line.
 */
static void error_hidefile_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_ERROR);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_ERROR, NULL, 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[ERROR] Message\n");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
}



// MESSAGE TYPE: FAIL

/** Display failure message.
 */
static void fail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_FAIL);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_FAIL, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[FAIL file:8] Message\n");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Mixed message type.
 */
static void fail_warn_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_FAIL);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_FAIL|CUTL_WARN, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[FAIL file:8] Message\n");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Disabled message type.
 */
static void fail_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_VERBOSE ^ CUTL_FAIL);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_FAIL, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}

/** Hide file and line.
 */
static void fail_hideline_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_FAIL);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_FAIL, "file", 0, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[FAIL file] Message\n");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Hide file and line.
 */
static void fail_hidefile_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_FAIL);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_FAIL, NULL, 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[FAIL] Message\n");
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}



// MESSAGE TYPE: WARN

/** Display warning message.
 */
static void warn_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_WARN);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_WARN, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[WARN file:8] Message\n");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Disabled message type.
 */
static void warn_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_VERBOSE ^ CUTL_WARN);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_WARN, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}

/** Hide line.
 */
static void warn_hideline_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_WARN);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_WARN, "file", 0, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[WARN file] Message\n");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Hide file and line.
 */
static void warn_hidefile_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_WARN);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_WARN, NULL, 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[WARN] Message\n");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}



// MESSAGE TYPE: INFO

/** Display information message.
 */
static void info_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_INFO);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_INFO, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[INFO file:8] Message\n");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Disabled message type.
 */
static void info_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_VERBOSE ^ CUTL_INFO);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_INFO, "file", 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}

/** Hide line.
 */
static void info_hideline_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_INFO);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_INFO, "file", 0, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[INFO file] Message\n");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Hide file and line.
 */
static void info_hidefile_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_INFO);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_INFO, NULL, 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "[INFO] Message\n");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}



// MESSAGE TYPE: SUMMARY

/** Display summary.
 */
static void summary_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SUMMARY);

	// Function under test
	cutl_message_at(fix->cutl, CUTL_SUMMARY, NULL, 8, "Message");

	// Asserts
	cutl_assert_content(cutl, fix->output, "Message\n");
	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}



// MESSAGE NESTING

// MY ERROR TEST

static void My_error_test(Cutl *cutl, void *data)
{
	cutl_message_at(cutl, CUTL_ERROR, NULL, 0, "My error");
	cutl_message_at(cutl, CUTL_FAIL, NULL, 0, "My failure");
}

static void My_error_subsuite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "test", My_error_test, NULL);
}

static void My_error_suite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "subsuite", My_error_subsuite, NULL);
}


/** Nested erroneous test.
 */
static void nested_error_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_ERROR | CUTL_SUITES);

	// Function under test
	cutl_run(fix->cutl, "suite", My_error_suite, NULL);

	// Asserts
	const char *expected =
		"suite:\n"
		"	subsuite:\n"
		"		test:\n"
		"			[ERROR] My error\n"
		"		test canceled.\n"
		"	subsuite canceled.\n"
		"suite canceled.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
}


/** Flattened erroneous test.
 */
static void flattened_error_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_ERROR);

	// Function under test
	cutl_run(fix->cutl, "suite", My_error_suite, NULL);

	// Asserts
	const char *expected =
		"test:\n"
		"	[ERROR] My error\n"
		"test canceled.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
}



// MY FAIL TEST

static void My_fail_test(Cutl *cutl, void *data)
{
	cutl_message_at(cutl, CUTL_FAIL, NULL, 0, "My failure");
	cutl_message_at(cutl, CUTL_INFO, NULL, 0, "My message");
}

static void My_fail_subsuite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "test", My_fail_test, NULL);
}

static void My_fail_suite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "subsuite", My_fail_subsuite, NULL);
}


/** Nested failed test.
 */
static void nested_fail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_FAIL | CUTL_SUITES);

	// Function under test
	cutl_run(fix->cutl, "suite", My_fail_suite, NULL);

	// Asserts
	const char *expected =
		"suite:\n"
		"	subsuite:\n"
		"		test:\n"
		"			[FAIL] My failure\n"
		"		test failed.\n"
		"	subsuite failed.\n"
		"suite failed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Flattened failed test.
 */
static void flattened_fail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_FAIL);

	// Function under test
	cutl_run(fix->cutl, "suite", My_fail_suite, NULL);

	// Asserts
	const char *expected =
		"test:\n"
		"	[FAIL] My failure\n"
		"test failed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}



// MY INFO TEST

static void My_info_test(Cutl *cutl, void *data)
{
	cutl_message_at(cutl, CUTL_INFO, NULL, 0, "My message");
	cutl_message_at(cutl, CUTL_WARN, NULL, 0, "My warning");
}

static void My_info_subsuite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "test", My_info_test, NULL);
}

static void My_info_suite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "subsuite", My_info_subsuite, NULL);
}


/** Nested info test.
 */
static void nested_info_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_INFO | CUTL_SUITES);

	// Function under test
	cutl_run(fix->cutl, "suite", My_info_suite, NULL);

	// Asserts
	const char *expected =
		"suite:\n"
		"	subsuite:\n"
		"		test:\n"
		"			[INFO] My message\n"
		"		test passed.\n"
		"	subsuite passed.\n"
		"suite passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Flattened info test.
 */
static void flattened_info_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_INFO);

	// Function under test
	cutl_run(fix->cutl, "suite", My_info_suite, NULL);

	// Asserts
	const char *expected =
		"test:\n"
		"	[INFO] My message\n"
		"test passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}



// MY SILENT TEST

static void My_silent_test(Cutl *cutl, void *data)
{
	cutl_assert(cutl, true, "Never fails.");
}

static void My_silent_subsuite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "test", My_silent_test, NULL);
}

static void My_silent_suite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "subsuite", My_silent_subsuite, NULL);
}


/** Nested info test.
 */
static void nested_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_TESTS | CUTL_SUITES);

	// Function under test
	cutl_run(fix->cutl, "suite", My_silent_suite, NULL);

	// Asserts
	const char *expected =
		"suite:\n"
		"	subsuite:\n"
		"		test passed.\n"
		"	subsuite passed.\n"
		"suite passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Semi-nested info test.
 */
static void seminested_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_SUITES);

	// Function under test
	cutl_run(fix->cutl, "suite", My_silent_suite, NULL);

	// Asserts
	const char *expected =
		"suite:\n"
		"	subsuite passed.\n"
		"suite passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}


/** Flattened info test.
 */
static void flattened_silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_verbosity(fix->cutl, CUTL_TESTS);

	// Function under test
	cutl_run(fix->cutl, "suite", My_silent_suite, NULL);

	// Asserts
	const char *expected = "test passed.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_false(cutl, cutl_get_failed(fix->cutl));
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
}



// MESSAGE SUITE

void cutl_message_suite(Cutl *cutl)
{
	cutl_at_start(cutl, fixture_setup, NULL);
	cutl_at_end(cutl, fixture_clean, NULL);

	cutl_test(cutl, error_test);
	cutl_test(cutl, error_fail_test);
	cutl_test(cutl, error_silent_test);
	cutl_test(cutl, error_hideline_test);
	cutl_test(cutl, error_hidefile_test);
	cutl_test(cutl, fail_test);
	cutl_test(cutl, fail_warn_test);
	cutl_test(cutl, fail_silent_test);
	cutl_test(cutl, fail_hideline_test);
	cutl_test(cutl, fail_hidefile_test);
	cutl_test(cutl, warn_test);
	cutl_test(cutl, warn_silent_test);
	cutl_test(cutl, warn_hideline_test);
	cutl_test(cutl, warn_hidefile_test);
	cutl_test(cutl, info_test);
	cutl_test(cutl, info_silent_test);
	cutl_test(cutl, info_hideline_test);
	cutl_test(cutl, info_hidefile_test);
	cutl_test(cutl, summary_test);

	cutl_test(cutl, nested_error_test);
	cutl_test(cutl, flattened_error_test);
	cutl_test(cutl, nested_fail_test);
	cutl_test(cutl, flattened_fail_test);
	cutl_test(cutl, nested_info_test);
	cutl_test(cutl, flattened_info_test);
	cutl_test(cutl, nested_silent_test);
	cutl_test(cutl, seminested_silent_test);
	cutl_test(cutl, flattened_silent_test);
}
