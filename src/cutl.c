/**
 * LICENCE:
 * Copyright (c) <2014> <Baptiste "Saend" CEILLIER>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */
#include "cutl.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define VERB_CHECK(cutl, ver) ((cutl)->settings->verbosity & (ver))


// INTERNAL DATA STRUCTURES

typedef struct {
	FILE *output;
	int verbosity;
} cutl_settings_t;

struct cutl_t {
	const char *name;

	bool failed;
	unsigned int nb_asserts, nb_passed, nb_failed;
	
	bool suite;
	int depth;
	
	bool prefixed, infixed;
	
	cutl_t *parent;
	jmp_buf env;
	void (*setup)(cutl_t*, void*);
	void (*teardown)(cutl_t*, void*);
	cutl_settings_t *settings;
};


// MEMORY MANAGEMENT

cutl_t *cutl_new(const char *name) {
	cutl_t *cutl = malloc(sizeof(*cutl));
	cutl->settings = malloc(sizeof(*cutl->settings));
	
	cutl_reset(cutl);
	cutl->name = name;
	
	return cutl;
}

void cutl_free(cutl_t *cutl) {
	assert(cutl);
	assert(cutl->depth == 0);

	free(cutl->settings);
	free(cutl);
}


// SETTINGS

void cutl_reset(cutl_t *cutl) {	
	cutl_settings_t *settings = cutl->settings;
	if (cutl->depth == 0) {
		memset(cutl, 0, sizeof(*cutl));
	}
	
	cutl->settings = settings;
	cutl->settings->output = stdout;
	cutl->settings->verbosity = CUTL_NORMAL;
}

void cutl_output(cutl_t *cutl, FILE *output) {
	assert(cutl);
	
	cutl->settings->output = output;
}

void cutl_verbosity(cutl_t *cutl, int verbosity) {
	assert(cutl);

	cutl->settings->verbosity = verbosity;
}


// TEST REPORTING

static void cutl_infix(cutl_t *cutl, const char *str) {
	assert(cutl);
	
	if (cutl->infixed || !cutl->prefixed || cutl->depth == 0) return;
	
	fprintf(cutl->settings->output, "%s\n", str);
	cutl->infixed = true;
}

static void cutl_prefix(cutl_t *cutl) {
	assert(cutl);
	
	if (cutl->prefixed || cutl->depth == 0) return;
	
	if (VERB_CHECK(cutl, CUTL_SUITE)) {
		cutl_prefix(cutl->parent);
	}
	
	cutl_infix(cutl->parent, ":");
	if (VERB_CHECK(cutl, CUTL_SUITE)) {
		for (int i=0; i<cutl->depth-1; ++i) {
			fprintf(cutl->settings->output, "\t");
		}
	}
	fprintf(cutl->settings->output, "%s", cutl->name);
	cutl->prefixed = true;
}

static void cutl_suffix(cutl_t *cutl) {
	assert(cutl);
	
	if (cutl->depth == 0) return;
	
	if (cutl->infixed) {
		if (VERB_CHECK(cutl, CUTL_SUITE)) {
			for (int i=0; i<cutl->depth-1; ++i) {
				fprintf(cutl->settings->output, "\t");
			}
		}
		fprintf(cutl->settings->output, "%s", cutl->name);
	}
	
	const char *str;
	if (cutl->failed) {
		str = "failed";
	} else if (cutl->nb_asserts == 0 && cutl->nb_passed == 0) {
		str = "is empty";
	} else {
		str = "passed";
	}
	
	fprintf(cutl->settings->output, " %s.\n", str);
}


// LOW-LEVEL

void cutl_message(cutl_t *cutl, const char *msg, int verbosity, 
                  const char *file, int line) {
	if (VERB_CHECK(cutl, verbosity)) {
		cutl_prefix(cutl);
		cutl_infix(cutl, ":");
		if (VERB_CHECK(cutl, CUTL_SUITE)) {
			for (int i=0; i<cutl->depth-1; ++i) {
				fprintf(cutl->settings->output, "\t");
			}
		} 
		if (cutl->depth > 0) {
			fprintf(cutl->settings->output, "\t");
		}
		
		if (verbosity & CUTL_ERROR) 
			fprintf(cutl->settings->output, "ERROR: ");
		if (verbosity & CUTL_WARNING) 
			fprintf(cutl->settings->output, "WARNING: ");
		if (verbosity & CUTL_INFO) 
			fprintf(cutl->settings->output, "INFO: ");
		
		if (file) {
			fprintf(cutl->settings->output, "%s:%d: %s\n", file, line, msg);
		} else {
			fprintf(cutl->settings->output, "%s\n", msg);
		}
	}
}

void cutl_fail(cutl_t *cutl, const char* msg, const char*file, int line) {
	assert(cutl);
	
	if (cutl->depth <= 0) {
		cutl_message(
			cutl, "failing is not allowed outside of tests.",
			CUTL_WARNING, file, line
		);
		return;
	}
	
	cutl->failed = true;
	
	if (VERB_CHECK(cutl, CUTL_FAILURE)) {
		cutl_prefix(cutl);
		cutl_message(cutl, msg, CUTL_FAILURE, file, line);
		cutl_suffix(cutl);
	}
	
	cutl->nb_failed++;
	
	longjmp(cutl->env, 1);
}


// TESTING

void cutl_setup(cutl_t *cutl, void (*func)(cutl_t*, void*)) {
	assert(cutl);
	
	cutl->setup = func;
}

void cutl_teardown(cutl_t *cutl, void (*func)(cutl_t*, void*)) {
	assert(cutl);
	
	cutl->teardown = func;
}

void cutl_run(cutl_t *parent, const char *name, void (*func)(cutl_t*, void*),
               void *data) {
	assert(parent);
	
	parent->suite = true;
	
	cutl_t child = {
		.name = name,
		.depth = parent->depth + 1,
		.parent = parent,
		.settings = parent->settings,
	};
	cutl_t *cutl = &child;
	
	if (!setjmp(cutl->env)) {
		if (parent->setup) parent->setup(cutl, data);
		func(cutl, data);
		if (parent->teardown) parent->teardown(cutl, data);
		
		if ((cutl->suite && VERB_CHECK(cutl, CUTL_SUITE)) 
		     || (!cutl->suite && VERB_CHECK(cutl, CUTL_SUCCESS))
		     || cutl->prefixed
		) {
			cutl_prefix(cutl);
			cutl_suffix(cutl);
		}
		
		if (!cutl->failed) cutl->nb_passed++;
	}
	
	parent->nb_passed += cutl->nb_passed;
	parent->nb_failed += cutl->nb_failed;
	if (cutl->failed) parent->failed = true;
}


// ASSERT

void cutl_assert(cutl_t *cutl, int val, const char *msg, const char *file,
                  int line) {
	assert(cutl);
	
	if (cutl->depth <= 0) {
		cutl_message(
			cutl, "asserts are not allowed outside of tests.",
			CUTL_WARNING, file, line
		);
		return;
	}
	
	cutl->nb_asserts++;
	if(!val) {
		cutl_fail(cutl, msg, file, line);
	}
}


// REPORTING

void cutl_report(cutl_t *cutl) {
	int failure_verbosity = cutl->failed ? CUTL_FAILURE : CUTL_NOTHING;
	
	if (VERB_CHECK(cutl, CUTL_STATUS | failure_verbosity)) {
		fprintf(
			cutl->settings->output, 
			"%s: %s.\n", 
			cutl->name ? cutl->name : "Report", 
			cutl->failed ? "failure" : "success"
		);
	}
	
	if (VERB_CHECK(cutl, CUTL_STAT)) {
		fprintf(
			cutl->settings->output, 
			"Tests passed: %d\n", cutl->nb_passed
		);
	}
	if (VERB_CHECK(cutl, CUTL_STAT | failure_verbosity)) {
		fprintf(
			cutl->settings->output, 
			"Tests failed: %d\n", cutl->nb_failed
		);
	}
	if (VERB_CHECK(cutl, CUTL_STAT)) {
		fprintf(
			cutl->settings->output, 
			"       Total: %d\n", cutl->nb_passed + cutl->nb_failed
		);
	}
}