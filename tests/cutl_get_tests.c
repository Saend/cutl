#include "tests.h"


// CHILDREN, PASSED AND FAILED GETTERS

// MY TEST FUNCTIONS

static void My_pass_test(Cutl *cutl, void *data)
{
	cutl_assert_true(cutl, true);
}

static void My_fail_test(Cutl *cutl, void *data)
{
	cutl_assert_true(cutl, false);
}

static void My_mixed_subsuite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "test1", My_pass_test, NULL);
	cutl_run(cutl, "test2", My_fail_test, NULL);
	cutl_run(cutl, "test3", My_pass_test, NULL);
}

static void My_mixed_suite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "subsuite", My_mixed_subsuite, NULL);
	cutl_run(cutl, NULL, My_mixed_subsuite, NULL);
}



/** Mixed suite.
 */
static void count_mixed_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_run(fix->cutl, "suite", My_mixed_suite, NULL);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_children(fix->cutl), 6);
	cutl_assert_equal(cutl, cutl_get_passed(fix->cutl), 4);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 2);
}


/** Passing test.
 */
static void count_pass_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_run(fix->cutl, "test", My_pass_test, NULL);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_children(fix->cutl), 1);
	cutl_assert_equal(cutl, cutl_get_passed(fix->cutl), 1);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 0);
}


/** Top-level passing test.
 */
static void count_toplevel_pass_test(Cutl *cutl, Fixture *fix)
{
	// Setup

	// Asserts
	cutl_assert_equal(cutl, cutl_get_children(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_passed(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 0);
}


/** Anonymous passing test.
 */
static void count_anon_pass_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_run(fix->cutl, NULL, My_pass_test, NULL);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_children(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_passed(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 0);
}


/** Failing test.
 */
static void count_fail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_run(fix->cutl, "test", My_fail_test, NULL);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_children(fix->cutl), 1);
	cutl_assert_equal(cutl, cutl_get_passed(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
}


/** Top-level failing test.
 */
static void count_toplevel_fail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_message_at(fix->cutl, CUTL_FAIL, NULL, 0, "My failure");

	// Asserts
	cutl_assert_equal(cutl, cutl_get_children(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_passed(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
}


/** Anonymous failing test.
 */
static void count_anon_fail_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_run(fix->cutl, NULL, My_fail_test, NULL);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_children(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_passed(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_failed(fix->cutl), 1);
}



// DEPTH AND ID GETTERS

// MY TEST SUITE

typedef struct {
	int depth, id;
} TestInfo;

static void My_info_test(Cutl *cutl, void *data)
{
	TestInfo *info = data;
	info->depth = cutl_get_depth(cutl);
	info->id = cutl_get_id(cutl);
}

static void My_info_subsuite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "test", My_info_test, data);
}

static void My_info_suite(Cutl *cutl, void *data)
{
	cutl_run(cutl, "subsuite1", My_info_subsuite, data);
	cutl_run(cutl, "subsuite2", My_info_subsuite, data);
}



/** Suite depth.
 */
static void info_suite_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	TestInfo info = {0};
	cutl_run(fix->cutl, "suite", My_info_suite, &info);

	// Asserts
	cutl_assert_equal(cutl, info.depth, 3);
	cutl_assert_equal(cutl, info.id, 5);
}


/** Test depth.
 */
static void info_test_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	TestInfo info = {0};
	cutl_run(fix->cutl, "test", My_info_test, &info);

	// Asserts
	cutl_assert_equal(cutl, info.depth, 1);
	cutl_assert_equal(cutl, info.id, 1);
}


/** Anonymous test depth.
 */
static void info_anon_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	TestInfo info = {-1, -1};
	cutl_run(fix->cutl, NULL, My_info_test, &info);

	// Asserts
	cutl_assert_equal(cutl, info.depth, 0);
	cutl_assert_equal(cutl, info.id, 1);
}


/** Top-level depth.
 */
static void info_toplevel_test(Cutl *cutl, Fixture *fix)
{
	// Setup

	// Asserts
	cutl_assert_equal(cutl, cutl_get_depth(fix->cutl), 0);
	cutl_assert_equal(cutl, cutl_get_id(fix->cutl), 0);
}



// GET SUITE

void cutl_get_suite(Cutl *cutl)
{
	cutl_at_start(cutl, fixture_setup, NULL);
	cutl_at_end(cutl, fixture_clean, NULL);

	cutl_test(cutl, count_mixed_test);
	cutl_test(cutl, count_pass_test);
	cutl_test(cutl, count_toplevel_pass_test);
	cutl_test(cutl, count_anon_pass_test);
	cutl_test(cutl, count_fail_test);
	cutl_test(cutl, count_toplevel_fail_test);
	cutl_test(cutl, count_anon_fail_test);

	cutl_test(cutl, info_suite_test);
	cutl_test(cutl, info_test_test);
	cutl_test(cutl, info_anon_test);
	cutl_test(cutl, info_toplevel_test);
}
