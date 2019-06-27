#pragma once

#include <cutl.h>

#include <setjmp.h>



// CUSTOM ASSERTS

/** Custom assert that fails if the input file doesn't exactly contain the
 * given content.
 */
void cutl_assert_content_at(
	Cutl *cutl, const char *file, int line, FILE *input,
	const char *content);

#define cutl_assert_content(cutl, output, content) \
	cutl_assert_content_at((cutl), __FILE__, __LINE__, (output), (content))


/** Custom assert that always fails. Insures that test was interrupted.
 */
#define cutl_assert_canceled(cutl) \
	cutl_fail((cutl), "Test should have been canceled.")



// TEST FIXTURE

/** Test fixture.
 * Most tests are going to need a fresh cutl test context and an file to check
 * the output of the test.
 *
 * Some tests might need a jump environment before to protect from interruption.
 */
typedef struct {
	Cutl *cutl;
	FILE *output;
	jmp_buf env;
} Fixture;


/** Creates a new test context and output file. To be used with cutl_at_start().
 * Dynamically allocates a new fixture and sets it as test data.
 */
void fixture_setup(Cutl *cutl, void*);


/** Closes test context and output file. To be used with cutl_at_end().
 * Frees the dynamically allocated fixture.
 */
void fixture_clean(Cutl *cutl, void*);


/** Longjumps to the saved environment. To be used with cutl_at_interrupt().
 * The jump environment needs to be manually saved using setjmp() and the test
 * fixture must be passed to cutl_at_interrupt().
 */
void at_interrupt_longjmp(Cutl *cutl, void*);
