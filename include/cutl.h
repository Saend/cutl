#pragma once
/** \file cutl.h
 * \brief cutl: C unit testing library.
 * \author Baptiste "Saend" CEILLIER
 * \copyright See LICENSE file.
 *
 * \mainpage
 * A test is a C function containing calls to cutl_assert_at() that is run using
 * cutl_run(). A suite is a test containing other calls to cutl_run(), but can
 * also contain asserts.
 *
 * When an assert fails, execution is resumed from the parent call to
 * cutl_run() using `longjmp()`, and the test and any parent suites are
 * considered failed.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * // This simple example can be compiled with `cc simple.c -lcutl`
 * #include <cutl.h>
 *
 * void simple_test(Cutl *cutl) {
 *	cutl_assert_true(cutl, 34 == 45); // macro wrapping cutl_assert_at()
 * }
 *
 * int main(void) {
 *	Cutl *cutl = cutl_new(NULL);
 *	cutl_test(cutl, simple_test); // macro wrapping cutl_run()
 *	return cutl_summary(cutl);
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <cutl_config.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>



/// \name API VISIBILITY

/**\def CUTL_EXPORT
 * Marks functions to be exported when building as a shared library.
 */
/** \def CUTL_IMPORT
 * Marks functions to be imported when using the shared library.
 */

#if defined(_WIN32) || defined(__CYGWIN__)
# define CUTL_IMPORT __declspec(dllimport)
# define CUTL_EXPORT __declspec(dllexport)
#elif __GNUC__ >= 4
# define CUTL_IMPORT __attribute__((visibility("default")))
# define CUTL_EXPORT __attribute__((visibility("default")))
#else
# define CUTL_IMPORT
# define CUTL_EXPORT
#endif


/**\def CUTL_API
 * Marks functions of the API.
 */
#ifdef CUTL_SHARED
# ifdef CUTL_BUILDING
#  define CUTL_API CUTL_EXPORT
# else
#  define CUTL_API CUTL_IMPORT
# endif
#else
# define CUTL_API
#endif


/**\def CUTL_NORETURN
 * Marks functions that never return.
 */
#if __GNUC__ >= 3
# define CUTL_NORETURN __attribute__((noreturn))
#else
# define CUTL_NORETURN
#endif



/// \name MEMORY MANAGEMENT

/** The test context needed by most functions of this library.
 * The hierarchy of tests and suites is represented by a tree of Cutl pointers.
 * In this tree nodes are suites and leafs are actual tests.
 * The root of this tree (called top-level test context) is created by calling
 * cutl_new(). Children test contexts are created internally when calling
 * cutl_run(), and only exist for the duration of the test.
 *
 * \warning The Cutl pointers passed to your test functions are only valid
 * inside their respective functions and must **not** be kept or freed.
 */
typedef struct Cutl Cutl;


/** Creates a new top-level test context.
 * Returns a new top-level test context that usually contains others tests.
 * Those test functions receive their own Cutl pointer as first parameter.
 *
 * If the `name` parameter is NULL, then "Unit tests" is used instead. The
 * pointer must be valid until cutl_free() is called.
 */
CUTL_API Cutl *cutl_new(const char *name);


/** Destroys the top-level test context.
 * Only pointers you received from cutl_new() should be freed.
 * Pointers received as parameter by test functions must **not** be freed.
 */
CUTL_API void cutl_free(Cutl *cutl);



/// \name SETTINGS

/** Sets the output file to use when printing messages.
 * If the `output` parameter is NULL, then the default value is used instead.
 *
 * Children tests inherit this setting.
 */
CUTL_API void cutl_set_output(Cutl *cutl, FILE *output);

/** Returns the current output file, as set by cutl_set_output().
 */
CUTL_API FILE *cutl_get_output(const Cutl *cutl);


/**The types of messages that can be displayed.
 * Used with cutl_message_at() or combined and used with cutl_set_verbosity().
 */
typedef enum {
	/** Programmer errors.
	 * Trying to print a message of this type (whether the verbosity allows
	 * it or not) marks the test and its parents as failed and cancels the
	 * rest of the test sequence.
	 */
	CUTL_ERROR = 1,

	/** Failed tests.
	 * Trying to print a message of this type (whether the verbosity allows
	 * it or not) marks the test and its parents as failed.
	 */
	CUTL_FAIL = 2,

	/** User defined warnings.
	 */
	CUTL_WARN = 4,

	/** User defined information.
	 */
	CUTL_INFO = 8,

	/** Test summary.
	 * This type is used internally by cutl_summary() but can be manually
	 * used to display custom summaries that respect the verbosity setting.
	 */
	CUTL_SUMMARY = 16,

	/** Successful tests.
	 * This type is used internally to report successful tests, it should
	 * not be used with cutl_message_at().
	 */
	CUTL_TESTS = 32,

	/** Suites of tests.
	 * This type is used internally to display suites and show the
	 * hierarchy of tests in a nested fashion, it should not be used with
	 * cutl_message_at().
	 */
	CUTL_SUITES = 64,
} Cutl_VerbObj;


/** Common sets of messages to display.
 * To be used with cutl_set_verbosity(). The default verbosity is CUTL_NORMAL.
 */
typedef enum {
	/** Display nothing */
	CUTL_SILENT = 0,

	/** Display errors, failures, warnings and summaries */
	CUTL_MINIMAL = (CUTL_ERROR | CUTL_FAIL | CUTL_WARN | CUTL_SUMMARY),

	/** Display everything but test successes */
	CUTL_NORMAL = (CUTL_MINIMAL | CUTL_INFO | CUTL_SUITES),

	/** Display everything */
	CUTL_VERBOSE = (CUTL_NORMAL | CUTL_TESTS),
} Cutl_VerbLvl;


/** Sets the verbosity filter for messages.
 * The `verb` parameter can be a bitwise-OR composition of verbosity object
 * values, or a pre-defined verbosity level. If it is negative, then the
 * default value (#CUTL_NORMAL) is used instead.
 *
 * When calling cutl_message_at(), messages with a verbosity not included in
 * the verbosity filter are discarded.
 *
 * Children tests inherit this setting.
 */
CUTL_API void cutl_set_verbosity(Cutl *cutl, int verb);

/** Return the current verbosity, as set by cutl_set_verbosity().
 */
CUTL_API int cutl_get_verbosity(const Cutl *cutl);


/** Sets whether to use color when printing messages.
 * If the `color` parameter is zero then color output is disabled; if it is
 * positive then color output is enabled. If `color` is negative and both
 * #CUTL_USE_ISATTY and #CUTL_USE_FILENO were defined at build time, then color
 * output is enabled only if the current output file is a TTY and the `TERM`
 * environment variable is set; otherwise it is disabled.
 *
 * Children tests inherit this setting.
 */
CUTL_API void cutl_set_color(Cutl *cutl, int color);

/** Returns the current *actual* color setting.
 * If cutl_set_color() was called with a negative value, then this function
 * returns whether color output was actually enabled or not.
 *
 * Returns true if color output is enabled; false otherwise.
 */
CUTL_API bool cutl_get_color(const Cutl *cutl);


/** Sets the indentation string.
 * The `indent` parameter is used to prefix nested messages. The pointer must be
 * valid for the duration of the test. If it is NULL, then the default value is
 * used instead.
 *
 * Children tests inherit this setting.
 */
CUTL_API void cutl_set_indent(Cutl *cutl, const char *indent);

/** Returns the current indentation string, as set by cutl_set_indent().
 */
CUTL_API const char *cutl_get_indent(const Cutl *cutl);


/** Reads the settings from the command-line arguments.
 * If #CUTL_PARSE_ARGS_ENABLED was defined at build time, then this function
 * uses `getopt()` to parse different command-line options.
 * Theses options include a `-h` option that describes the other available
 * options on the standard output, and immediately interrupts the test.
 * Missing or invalid arguments are reported as errors, aborting the test.
 * The `optind` variable (defined in `unistd.h`) is not reset before or after
 * parsing.
 *
 * If #CUTL_PARSE_ARGS_ENABLED was not defined at built time, then this
 * function does nothing.
 */
CUTL_API void cutl_parse_args(Cutl *cutl, int argc, char *argv[]);



/// \name MESSAGING

/** Prints a message for the current test, if the verbosity allows it.
 * The `type` parameter should be a verbosity object value. If the result of a
 * bitwise-AND between `type` and the current verbosity is different than zero,
 * then the message is displayed.
 * If `type` is #CUTL_FAIL, then the test and its parents are marked as failed
 * regardless of the verbosity setting, but it is not interrupted.
 * If `type` is #CUTL_ERROR, then the test and its parents are marked as both
 * erroneous and failed regardless of the verbosity setting. The test is not
 * immediately interrupted, but following calls to cutl_run() are canceled.
 * If `type` is a bitwise-OR combination of several verbosity objects, then it
 * is displayed as the most critical one.
 *
 * If the `file` parameter is not NULL, then the file name and line number are
 * displayed before the message.
 *
 * The `fmt` parameter is a `printf()`-style format string describing the
 * content of message. It cannot be NULL.
 */
CUTL_API void cutl_message_at(
	Cutl *cutl, int type, const char *file, int line, const char *fmt, ...);

/** Prints a message for the current test.
 * Same as cutl_message_at(), but with the `file` and `line` parameters
 * automatically generated.
 */
#define cutl_message(cutl, type, ...)					\
	cutl_message_at((cutl), (type), __FILE__, __LINE__, __VA_ARGS__)

/** Prints an information message for the current test.
 * Same as cutl_message(), but with the `type` parameter set to CUTL_INFO.
 */
#define cutl_info(cutl, ...)						\
	cutl_message((cutl), CUTL_INFO, __VA_ARGS__)

/** Prints a warning message for the current test.
 * Same as cutl_message(), but with the `type` parameter set to CUTL_WARN.
 */
#define cutl_warn(cutl, ...)						\
	cutl_message((cutl), CUTL_WARN, __VA_ARGS__)



/// \name TESTING

/** Test function type for cutl_run(),  cutl_at_start(), cutl_at_end() and
 * cutl_at_interrupt().
 * The second parameter is the data pointer passed when registered.
 */
typedef void (Cutl_Func)(Cutl*, void*);


/** Sets the function to be called before each test.
 * Inside the current test context, every following call to cutl_run() causes
 * `func` to be called before the test function.
 *
 * If the `at_start` function fails, the test and `at_end` functions are not
 * executed and the test is marked as failed. This means the `at_start` function
 * must do its own cleaning up when failing.
 *
 * The `func` function pointer replaces any previously set `at_start` function.
 * If it is NULL, then the current `at_start` function is disabled.
 *
 * The `data` pointer will be passed to `func` every time it is called. It can
 * safely be NULL.
 */
CUTL_API void cutl_at_start(Cutl *cutl, Cutl_Func *func, void *data);


/** Sets the function to be called after each test.
 * Inside the current test context, every following call to cutl_run() causes
 * `func` to be called after the test function, whether it succeeded or not.
 *
 * The function is not called if the `at_start` function failed.
 *
 * The `func` function pointer replaces any previous set `at_end` function. If
 * it is NULL, then the current `at_end` function is disabled.
 *
 * The `data` pointer will be passed to `func` every time it is called. It can
 * safely be NULL.
 */
CUTL_API void cutl_at_end(Cutl *cutl, Cutl_Func *func, void *data);


/** Sets the function to be called before the test gets interrupted.
 * Inside the current test context, the next call to cutl_interrupt() causes
 * `func` to be called. The function is not called if the test is interrupted
 * again. The function can use `longjmp()` to jump back to the test function,
 * but should not jump higher then the parent call to cutl_run().
 *
 * The `func` function pointer replaces any previously set `at_interrupt`
 * function. If it is NULL, then the current `at_interrupt` function is
 * disabled.
 *
 * The `data` pointer will be passed to `func` when it is called. It can
 * safely be NULL.
 */
CUTL_API void cutl_at_interrupt(Cutl *cutl, Cutl_Func *func, void *data);


/** Runs the test function in a new test context.
 * The calling environment is saved with `setjmp()` so that the test can be
 * interrupted at any point by calling cutl_fail() or cutl_interrupt().
 *
 * If set, the `at_start` and `at_end` functions are called accordingly.
 *
 * The `name` parameter is used to create the new test context that is passed
 * to the `test` function. The pointer must be valid for the duration of the
 * test. If it is NULL then the test is "anonymous". Anonymous tests behave as
 * a regular ones, except that they are transparent, meaning that any message
 * is displayed as if the parent displayed it, that they don't count towards
 * the parents' number of children, and that they have the same depth as their
 * parent. They are useful when splitting suites across files, allowing to
 * regroup them in an invisible suite.
 *
 * The `test` parameter is a pointer to the test function. It cannot be NULL.
 *
 * The `data` pointer is passed as parameter to the `at_start`, `test` and
 * `at_end` functions, and can be retrieved with cutl_get_data(). It can safely
 * be NULL.
 *
 * Returns the number of failed tests, by calling cutl_get_failed().
 */
CUTL_API int cutl_run(
	Cutl *cutl, const char *name, Cutl_Func *test, void *data);

/** Runs the test function in a new test context.
 * Same as cutl_run(), with the `name` parameter automatically generated and the
 * `data` parameter set to NULL.
 */
#define cutl_test(cutl, func)						\
	cutl_run((cutl), #func, (Cutl_Func*) (func), NULL)

/** Runs the test function in a new test context.
 * Same as cutl_run(), with the `name` parameter automatically generated.
 */
#define cutl_test_data(cutl, func, data)				\
	cutl_run((cutl), #func"('"#data"')", (Cutl_Func*) (func), (data))

/** Runs the suite of tests in a new test context.
 * Same as cutl_run(), with the `name` parameter automatically generated and the
 * `data` parameter set to NULL.
 *
 * This macro is exactly the same as cutl_test(). Suites are indistinguishable
 * from regular tests when run, the only difference between tests and suites is
 * that suites also call cutl_run().
 */
#define cutl_suite(cutl, func)						\
	cutl_run((cutl), #func, (Cutl_Func*) (func), NULL)

/** Runs the test/suite anonymously.
 * Same as cutl_run(), with `name` and `data` parameter set to NULL.
 */
#define cutl_anon(cutl, func)						\
	cutl_run((cutl), NULL, (Cutl_Func*) (func), NULL)


/** Interrupts the current test without marking it as failed.
 * Performs a longjmp() back to the parent call to cutl_run(). If the `cutl`
 * parameter is a top-level test context, then exit() is called instead,
 * returning the number of failed tests by calling cutl_get_failed().
 *
 * If the `at_interrupt` function is set, then it is called first and if it
 * returns then the function continues normally.
 *
 * This function never returns.
 */
CUTL_API CUTL_NORETURN void cutl_interrupt(Cutl *cutl);


/** Interrupts the current test and marks it as failed.
 * Displays a failure message (if the verbosity allows it) by calling
 * cutl_message_at(). Execution is stopped by calling cutl_interrupt(), and
 * resumed from the parent call to cutl_run().
 *
 * The `file`,`line` and `fmt` parameters are the same as cutl_message_at().
 */
CUTL_API CUTL_NORETURN void cutl_fail_at(
	Cutl *cutl, const char *file, int line, const char *fmt, ...);

/** Interrupts the current test and marks it as failed.
 * Same as cutl_fail_at()
 */
#define cutl_fail(cutl, ...)						\
	cutl_fail_at((cutl), __FILE__, __LINE__, __VA_ARGS__);


/** Interrupts the current test, marks it as failed, and cancels the rest of the
 * test sequence.
 * Displays a failure message (if the verbosity allows it) by calling
 * cutl_message_at(). Execution is stopped by recursively calling
 * cutl_interrupt() on the test's parents.
 *
 * The `file`,`line` and `fmt` parameters are the same as cutl_message_at().
 */
CUTL_API CUTL_NORETURN void cutl_error_at(
	Cutl *cutl, const char *file, int line, const char *fmt, ...);

/** Interrupts the current test, marks it as failed, and cancels the rest of the
 * test sequence.
 * Same as cutl_error_at()
 */
#define cutl_error(cutl, ...)						\
	cutl_error_at((cutl), __FILE__, __LINE__, __VA_ARGS__);



/// \name ASSERTIONS

/** Fails and interrupts the current test if the condition is false.
 * Calls cutl_fail_at() if `val` is false.
 *
 * The `file`,`line` and `fmt` parameters are the same as cutl_fail_at().
 */
CUTL_API void cutl_assert_at(
	Cutl *cutl, bool val, const char *file, int line, const char *fmt, ...);

/** Fails and interrupts the current test if the `val` is false.
 * Same as cutl_assert_at(), with the `file` and `line` parameters automatically
 * generated.
 */
#define cutl_assert(cutl, val, ...)					\
	cutl_assert_at((cutl), (val), __FILE__, __LINE__, __VA_ARGS__)

/** Fails and interrupts the current test if `val` is false.
 * Same as cutl_assert(), with the `fmt` parameter automatically generated.
 */
#define cutl_assert_true(cutl, val)					\
	cutl_assert((cutl), (val), "'"#val"' is not true.")

/** Fails and interrupts the current test if the `val` is true.
 * Same as cutl_assert(), with the `fmt` parameter automatically generated.
 */
#define cutl_assert_false(cutl, val)					\
	cutl_assert((cutl), !(val), "'"#val"' is not false.")

/** Fails and interrupts the current test if `val1` and `val2` are different.
 * Same as cutl_assert(), with the `fmt` parameter automatically generated.
 */
#define cutl_assert_equal(cutl, val1, val2)				\
	cutl_assert(							\
		(cutl), (val1) == (val2),				\
		"'"#val1"' and '"#val2"' are not equal."		\
	)

/** Fails and interrupts the current test if the memory regions pointed by
 * `val1` and `val2` are different.
 * Same as cutl_assert(), with the `fmt` parameter automatically generated.
 */
#define cutl_assert_like(cutl, val1, val2, len)				\
	cutl_assert(							\
		(cutl), memcmp((val1), (val2), (len)) == 0,		\
		"'"#val1"' and '"#val2"' are not alike."		\
	)


/** Fails and cancels the test sequence if the condition is false.
 * Calls cutl_error_at() if `val` is false.
 *
 * The `file`,`line` and `fmt` parameters are the same as cutl_fail_at().
 */
CUTL_API void cutl_check_at(
	Cutl *cutl, bool val, const char *file, int line, const char *fmt, ...);

/** Fails and cancels the test sequence if the condition is false.
 * Same as cutl_check_at().
 */
#define cutl_check(cutl, val, ...) \
	cutl_check_at((cutl), (val), __FILE__, __LINE__, __VA_ARGS__)



/// \name REPORTING

/** Reports on the overall success of the test context.
 * Prints the total number of failed and passed test if the verbosity allows it.
 * Returns the number of failed tests, exactly as cutl_get_failed() does.
 */
CUTL_API int cutl_summary(Cutl *cutl);



/// \name MISCELLANEOUS

/** Returns the name of the current test context.
 * The name is set when calling cutl_new() or cutl_run().
 */
CUTL_API const char *cutl_get_name(const Cutl *cutl);


/** Returns the unique identifier for the current test context.
 * The top-level test context has an id of zero, each following call to
 * cutl_run() increments the id by one (even for anonymous tests).
 */
CUTL_API int cutl_get_id(const Cutl *cutl);


/** Returns the depth of the current test context.
 * The top-level test context has a depth of zero, each nested call to
 * cutl_run() increments the depth by one (except for anonymous tests).
 */
CUTL_API int cutl_get_depth(const Cutl *cutl);


/** Returns the parent of the current test context.
 */
CUTL_API const Cutl *cutl_get_parent(const Cutl *cutl);


/** Returns the test data passed to the parent call to cutl_run().
 */
CUTL_API void *cutl_get_data(const Cutl *cutl);


/** Sets the test data as if it was passed to the parent call to cutl_run().
 */
CUTL_API void cutl_set_data(Cutl *cutl, void *data);


/** Returns the number of children tests inside the current test context.
 * Recursively counts tests, but not suites and anonymous tests.
 *
 * By definition only suites have a number of children greater then zero.
 */
CUTL_API int cutl_get_children(Cutl *cutl);


/** Returns the number of passed tests inside the current test context.
 * Recursively counts tests but not suites.
 *
 * Inside tests that have not (yet) called cutl_run() and that are not marked
 * as failed, this function returns `0`.
 */
CUTL_API int cutl_get_passed(Cutl *cutl);


/** Returns the number of failed tests inside the current test context.
 * Recursively counts tests but not suites.
 *
 * Inside tests that have not (yet) called cutl_run() but that are marked as
 * failed, this function returns `1`.
 */
CUTL_API int cutl_get_failed(Cutl *cutl);


/** Returns whether the current test context is marked as erroneous.
 * Returns `true` if any error occurred; `false` otherwise.
 */
CUTL_API bool cutl_get_error(Cutl *cutl);
