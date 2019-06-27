#include "tests.h"

#define ARGC(argv) (sizeof(argv)/sizeof(*argv))



// VERBOSITY OPTIONS

/** Verbose output.
 */
static void verbose_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-v"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_VERBOSE);
}


/** Minimal output.
 */
static void minimal_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-m"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_MINIMAL);
}


/** Silent output.
 */
static void silent_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-s"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_SILENT);
}


// COLOR OPTION

/** Enable color.
 */
static void color_on_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-c", "on"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_true(cutl, cutl_get_color(fix->cutl));
}


/** Disable color.
 */
static void color_off_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_set_color(fix->cutl, true);
	char *argv[] = {"My_tests", "-c", "off"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_color(fix->cutl));
}


/** Auto color.
 * If color autodetection is enabled then color is disabled for files; otherwise
 * color is always disabled. Either way, it's off here.
 */
static void color_auto_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-c", "auto"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_color(fix->cutl));
}


/** Bad color argument.
 */
static void color_bad_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-c", "bad"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	const char *expected = "[ERROR cutl_parse_args()] Invalid argument "
		"for option 'c': 'bad'.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_false(cutl, cutl_get_color(fix->cutl));
}


/** Missing color argument.
 */
static void color_missing_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-c"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	const char *expected = "[ERROR cutl_parse_args()] Missing argument "
		"for option 'c'.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
	cutl_assert_false(cutl, cutl_get_color(fix->cutl));
}



// OUTPUT OPTION

/** Set output file.
 */
static void output_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-o", tmpnam(NULL)};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
	cutl_assert_true(cutl, cutl_get_output(fix->cutl) != fix->output);

	// Cleanup
	fclose(cutl_get_output(fix->cutl));
}


/** Bad output file.
 */
static void output_bad_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-o", "/"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_output(fix->cutl), fix->output);
}


/** Missing output file.
 */
static void output_missing_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-o"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	const char *expected = "[ERROR cutl_parse_args()] Missing argument "
		"for option 'o'.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_output(fix->cutl), fix->output);
}



// HELP OPTION

/** Display help message.
 */
static void help_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	cutl_at_interrupt(fix->cutl, at_interrupt_longjmp, fix);
	char *argv[] = {"My_tests", "-h"};
	FILE * const old_stdout = stdout;
	stdout = fix->output; // Help message is display on stdout.

	// Function under test
	if (setjmp(fix->env) == 0) {
		cutl_parse_args(fix->cutl, ARGC(argv), argv);
		cutl_assert_canceled(cutl)
	}

	// Asserts
	const char *expected =
		"Usage: My_tests [options]\n"
		"Options:\n"
		"  -v               Verbose output.\n"
		"  -m               Minimal output.\n"
		"  -s               Silent output.\n"
		"  -c <auto|on|off> Colored output.\n"
		"  -o <file>        Output file.\n"
		"  -h               Print this message and exit.\n"
		"CUTL version: "CUTL_VERSION"\n";
	cutl_assert_content(cutl, fix->output, expected);

	// Cleanup
	stdout = old_stdout;
}



// MISCELLANEOUS

/** Unknown option.
 */
static void unknown_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-u", "-v"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	const char *expected = "[ERROR cutl_parse_args()] Invalid option: "
		"'u'.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_NORMAL);
}


/** Grouped option and argument.
 */
static void grouped_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-con"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
	cutl_assert_true(cutl, cutl_get_color(fix->cutl));
}


/** Multiple options.
 */
static void multiple_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-v", "-c", "on", "-s"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_SILENT);
	cutl_assert_true(cutl, cutl_get_color(fix->cutl));
}


/** Grouped multiple options.
 */
static void multiple_grouped_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-svcon"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_VERBOSE);
	cutl_assert_true(cutl, cutl_get_color(fix->cutl));
}


/** Stop at delimiter.
 */
static void delimiter_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-v", "--", "-s"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_VERBOSE);
}


/** Stop at first non-option.
 */
static void nonoption_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-s", "nonoption", "-v"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_SILENT);
}


/** Null option.
 */
static void null_opt_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-v", NULL, "-s"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	cutl_assert_false(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_VERBOSE);
}


/** Null argument.
 */
static void null_arg_test(Cutl *cutl, Fixture *fix)
{
	// Setup
	char *argv[] = {"My_tests", "-s", "-c", NULL, "-v"};

	// Function under test
	cutl_parse_args(fix->cutl, ARGC(argv), argv);

	// Asserts
	const char *expected = "[ERROR cutl_parse_args()] Missing argument "
		"for option 'c'.\n";
	cutl_assert_content(cutl, fix->output, expected);

	cutl_assert_true(cutl, cutl_get_error(fix->cutl));
	cutl_assert_equal(cutl, cutl_get_verbosity(fix->cutl), CUTL_NORMAL);
}





// PARSE ARGS SUITE

void cutl_parse_args_suite(Cutl *cutl)
{
	cutl_at_start(cutl, fixture_setup, NULL);
	cutl_at_end(cutl, fixture_clean, NULL);

	cutl_test(cutl, verbose_test);
	cutl_test(cutl, minimal_test);
	cutl_test(cutl, silent_test);

	cutl_test(cutl, color_on_test);
	cutl_test(cutl, color_off_test);
	cutl_test(cutl, color_auto_test);
	cutl_test(cutl, color_bad_test);
	cutl_test(cutl, color_missing_test);

	cutl_test(cutl, output_test);
	cutl_test(cutl, output_bad_test);
	cutl_test(cutl, output_missing_test);

	cutl_test(cutl, help_test);

	cutl_test(cutl, unknown_test);
	cutl_test(cutl, grouped_test);
	cutl_test(cutl, multiple_test);
	cutl_test(cutl, multiple_grouped_test);
	cutl_test(cutl, delimiter_test);
	cutl_test(cutl, nonoption_test);
	cutl_test(cutl, null_opt_test);
	cutl_test(cutl, null_arg_test);
}
