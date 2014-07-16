/** CUTL - A simple C unit testing library. 
 * OVERVIEW:
 * A test is a function containing calls to cutl_assert(), it is run using 
 * cutl_run(). A suite is a test containing calls to cutl_run(). When an
 * assert fails, cutl_fail() calls longjmp() and executlion continues from the
 * last call to cutl_run(). The test, and any parent suites are considered
 * failed.
 *
 * This API is thread-safe. 
 */
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <setjmp.h>


// MEMORY MANAGEMENT
typedef struct cutl_t cutl_t;

cutl_t *cutl_new(const char *name);
void cutl_free(cutl_t *cutl);


// VERBOSITY OBJECTS
#define CUTL_NOTHING 	0
#define CUTL_ERROR 	1
#define CUTL_FAILURE	2
#define CUTL_WARNING	4
#define CUTL_INFO	8
#define CUTL_SUCCESS	16
#define CUTL_TEST	CUTL_SUCCESS | CUTL_FAILURE
#define CUTL_SUITE	32
#define CUTL_STATUS	64
#define CUTL_STAT	128
#define CUTL_EVERYTHING	255

// VERBOSITY LEVELS
#define CUTL_SILENT	CUTL_NOTHING
#define CUTL_MINIMAL	CUTL_ERROR | CUTL_FAILURE | CUTL_WARNING | CUTL_STATUS
#define CUTL_NORMAL	CUTL_MINIMAL | CUTL_INFO | CUTL_SUITE
#define CUTL_VERBOSE	CUTL_EVERYTHING


// SETTINGS
void cutl_output(cutl_t *cutl, FILE *output);
void cutl_verbosity(cutl_t *cutl, int verbosity);
void cutl_reset(cutl_t *cutl);


// LOW-LEVEL
void cutl_message(cutl_t *cutl, const char *msg, int verbosity, 
                  const char *file, int line);

void cutl_fail(cutl_t *cutl, const char *msg, const char *file, int line);


// ASSERT
void cutl_assert(cutl_t *cutl, int val, const char *msg, const char *file,
                  int line);

#define cutl_assert_true(cutl, val)						\
	cutl_assert(								\
		(cutl), (val), "assert_true("#val")", __FILE__, __LINE__	\
	);
	
#define cutl_assert_false(cutl, val)						\
	cutl_assert(								\
		(cutl), !(val), "assert_false("#val")", __FILE__, __LINE__	\
	);
	
// TESTING
void cutl_before(cutl_t *cutl, void (*func)(cutl_t*, void*));

void cutl_after(cutl_t *cutl, void (*func)(cutl_t*, void*));

void cutl_run(cutl_t *cutl, const char *name, void (*func)(cutl_t*, void*),
               void *data);

#define cutl_test(cutl, func) 							\
	cutl_run((cutl), #func, (func), NULL)

#define cutl_testdata(cutl, func, data)						\
	cutl_run((cutl), #func"("#data")", (func), (data))

#define cutl_suite(cutl, func) 							\
	cutl_run((cutl), #func, (func), NULL)


// REPORTING
void cutl_summary(cutl_t *cutl);



