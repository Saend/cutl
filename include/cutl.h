/* CUTL - A simple C unit testing library. 
 * AUTHOR: Baptiste "Saend" CEILLIER
 * OVERVIEW:
 * A test is a function containing calls to cutl_assert(), it is run using 
 * cutl_run(). A suite is a test containing calls to cutl_run(). When an
 * assert fails, cutl_fail() calls longjmp() and execution continues from the
 * last call to cutl_run(). The test, and any parent suites are considered
 * failed.
 *
 * This API is thread-safe. 
 */
#pragma once

#include <stdio.h>

// MEMORY MANAGEMENT

/** cutl_t - The testing context.
 * This is the structure passed to most functions of this library. You usually
 * only need to create (and then free) one. Behind the scenes the library creates
 * more and pass them to your tests. But you don't need to worry about those.
 * Every function taking a cutl_t pointer checks it first using the standard 
 * assert() function, which violently interrupts the program if it is NULL.
 */
typedef struct cutl_t cutl_t;


/** cutl_new() - Create a new testing context.
 * This is the root test, and usually contains others tests. Those test functions
 * receive their own cutl_t pointer as first parameter.
 */
cutl_t *cutl_new(const char *name);


/** cutl_free() - Destroy the testing context.
 * Only pointers you received from cutl_new() should be freed. You must NOT 
 * free the pointers received as parameters by test functions.
 */
void cutl_free(cutl_t *cutl);


// SETTINGS

/** VERBOSITY OBJECTS
 * Types of messages to be displayed. To be combined and used with cutl_verbosity().
 */
#define CUTL_NOTHING 	0
#define CUTL_ERROR 	1	// Errors (different from test failures)
#define CUTL_FAIL	2	// Failed tests
#define CUTL_WARN	4	// User defined warnings
#define CUTL_INFO	8	// User defined information
#define CUTL_SUCCESS	16	// Successful tests
#define CUTL_SUITE	32	// Successful suites of tests
#define CUTL_SUMMARY	64	// Success or failure of the overall session
#define CUTL_EVERYTHING	255


/** VERBOSITY LEVELS
 * Common sets of messages to display. To be used with cutl_verbosity().
 * The default verbosity is CUTL_NORMAL.
 */
#define CUTL_SILENT	CUTL_NOTHING
#define CUTL_MINIMAL	CUTL_ERROR | CUTL_FAIL | CUTL_WARN | CUTL_SUMMARY
#define CUTL_NORMAL	CUTL_MINIMAL | CUTL_INFO | CUTL_SUITE
#define CUTL_VERBOSE	CUTL_EVERYTHING


/** cutl_output() - Set the output to use when printing messages.
 * The default value is _stdout_. Cannot be NULL.
 */
void cutl_output(cutl_t *cutl, FILE *output);


/** cutl_verbosity() - Set the messages to be displayed.
 * The default value is CUTL_NORMAL.
 */
void cutl_verbosity(cutl_t *cutl, int verbosity);


/** cutl_reset() - Reset the stats and settings to their default values.
 */
void cutl_reset(cutl_t *cutl);


// TESTING

/** cutl_message_at() - Print a message if the verbosity allows it.
 * Printing a message with the CUTL_FAIL verbosity marks the test as failed
 * regardless of verbosity settings, but execution is not interrupted 
 * (unlike cutl_fail()).
 * If _msg_ is NULL, returns without printing anything.
 * If _file_ is not NULL, displays the file name and line number.
 */
void cutl_message_at(cutl_t *cutl, int verbosity, const char *msg,
                     const char *file, int line);

#define cutl_message(cutl, verbosity, msg)					\
	cutl_message_at((cutl), (verbosity), (msg), __FILE__, __LINE__)

#define cutl_info(cutl, msg)							\
	cutl_message_at((cutl), CUTL_INFO, (msg), NULL, 0)

#define cutl_warn(cutl, msg)							\
	cutl_message_at((cutl), CUTL_WARN, (msg), NULL, 0)


/** cutl_fail_at() - Interrupt test and mark it as failed.
 * Displays a failure message if the verbosity setting allows it (CUTL_FAIL).
 * Execution is resumed from previous call to cutl_run().
 * Uses cutl_message() with CUTL_FAIL internally.
 */
void cutl_fail_at(cutl_t *cutl, const char *msg, const char *file, int line);

#define cutl_fail(cutl, msg)							\
	cutl_fail_at((cutl), (msg), __FILE__, __LINE__);


/** cutl_assert_at() - Fail test if condition is false.
 * Uses cutl_fail() internally.
 */
void cutl_assert_at(cutl_t *cutl, int val, const char *msg, const char *file,
                    int line);

#define cutl_assert(cutl, val, msg)						\
	cutl_assert_at(							\
		(cutl), (int)(val), (msg), __FILE__, __LINE__	\
	)

#define cutl_assert_true(cutl, val)						\
	cutl_assert_at(							\
		(cutl), (int)(val), #val" is not true.", __FILE__, __LINE__	\
	)
	
#define cutl_assert_equal(cutl, val1, val2)					\
	cutl_assert_at(								\
		(cutl), (val1) == (val2), #val1" and "#val2" are not equal.",	\
		__FILE__, __LINE__						\
	)


// TESTING

/** cutl_before() - Set the function to be called before tests.
 * Inside the current test, every following call to cutl_run() causes _func_
 * to be called before the test function. 
 * If the _before_ function fails, the test and _after_ functions are not 
 * executed and the test is marked as failed. This means the _before_ must do
 * its own cleaning up before failing.
 */
void cutl_before(cutl_t *cutl, void (*func)(cutl_t*, void*));


/** cutl_after() - Sets the function to be called after tests.
 * Inside the current test, every following call to cutl_run() causes _func_
 * to be called after the test function.
 * The function is not be called if the _before_ function failed.
 */
void cutl_after(cutl_t *cutl, void (*func)(cutl_t*, void*));

/** cutl_run() - Run the test function.
 * The _data_ pointer is passed to the _before_ function (if set), to
 * the test function, and then to the _after_ function (if set).
 */
void cutl_run(cutl_t *cutl, const char *name, void (*func)(cutl_t*, void*),
              void *data);

#define cutl_test(cutl, func) 							\
	cutl_run((cutl), #func, (void (*)(cutl_t*, void*))(func), NULL)

#define cutl_testdata(cutl, func, data) 					\
	cutl_run((cutl), #func"("#data")", (void (*)(cutl_t*, void*))(func), (data))

#define cutl_suite(cutl, func) 							\
	cutl_run((cutl), #func, (void (*)(cutl_t*, void*))(func), NULL)


/** cutl_has_failed() - Check if the current test has failed.
 * Can be used to tell if the current suite had at least one failed test.
 * The functions actually returns the number of failed tests for the current
 * test, counting recursively through its children.
 */
int cutl_has_failed(const cutl_t *cutl);


/** cutl_summary() - Report on the overall success of the session.
 * Prints the number of failed and passed test, if the verbosity allows it
 * (CUTL_SUMMARY). Only tests are counted, suites are ignored. A test calling
 * cutl_run() is automatically concidered a suite, even if it has asserts of
 * its own.
 */
int cutl_summary(cutl_t *cutl);



