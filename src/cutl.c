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

typedef enum {
	CUTL_BEFORE = 1,
	CUTL_TESTING,
	CUTL_AFTER,
} cutl_stage_t;

struct cutl_t {
	const char *name;
	
	bool failed;
	unsigned int nb_passed, nb_failed;
	
	bool is_suite;
	int depth;
	
	bool is_prefixed, is_infixed;
	
	cutl_t *parent;
	void (*before)(cutl_t*, void*);
	void (*after)(cutl_t*, void*);
	cutl_stage_t stage;
	jmp_buf env;
	cutl_settings_t *settings;
};


// MEMORY MANAGEMENT

cutl_t *cutl_new(const char *name) {
	cutl_t *cutl = malloc(sizeof(*cutl));
	memset(cutl, 0, sizeof(*cutl));
	
	cutl->name = name;	
	cutl->settings = malloc(sizeof(*cutl->settings));
	cutl_reset(cutl);
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
	cutl->failed = false;
	cutl->nb_failed = 0;
	cutl->nb_passed = 0;
	
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

static void cutl_indent(cutl_t *cutl) {
	if (VERB_CHECK(cutl, CUTL_SUITE)) {
		for (int i=0; i<cutl->depth-1; ++i) {
			fprintf(cutl->settings->output, "\t");
		}
	}
}

static void cutl_infix(cutl_t *cutl, const char *str) {
	assert(cutl);
	
	if (cutl->is_infixed || !cutl->is_prefixed || cutl->depth == 0) return;
	
	fprintf(cutl->settings->output, "%s\n", str);
	cutl->is_infixed = true;
}

static void cutl_prefix(cutl_t *cutl) {
	assert(cutl);
	
	if (cutl->is_prefixed || cutl->depth == 0) return;
	
	if (VERB_CHECK(cutl, CUTL_SUITE)) {
		cutl_prefix(cutl->parent);
	}
	
	cutl_infix(cutl->parent, ":");
	cutl_indent(cutl);
	fprintf(cutl->settings->output, "%s", cutl->name);
	cutl->is_prefixed = true;
}

static void cutl_suffix(cutl_t *cutl) {
	assert(cutl);
	
	if (cutl->depth == 0) return;
	
	if (cutl->is_infixed) {
		cutl_indent(cutl);
		fprintf(cutl->settings->output, "%s", cutl->name);
	}
	
	const char *str;
	if (cutl->failed) {
		str = "failed";
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
		cutl_indent(cutl);
		if (cutl->depth > 0) {
			fprintf(cutl->settings->output, "\t");
		}
		
		if (verbosity & CUTL_ERROR) 
			fprintf(cutl->settings->output, "ERROR: ");
		if (verbosity & CUTL_WARNING) 
			fprintf(cutl->settings->output, "WARN: ");
		if (verbosity & CUTL_INFO) 
			fprintf(cutl->settings->output, "INFO: ");
		if (verbosity & CUTL_FAILURE) 
			fprintf(cutl->settings->output, "FAIL: ");
		
		if (file) {
			fprintf(
				cutl->settings->output, "%s:%d: %s\n",
				file, line, msg
			);
		} else {
			fprintf(cutl->settings->output, "%s\n", msg);
		}
	}
}

void cutl_fail(cutl_t *cutl, const char* msg, const char*file, int line) {
	assert(cutl);
	
	cutl->failed = true;	
	
	if (VERB_CHECK(cutl, CUTL_FAILURE)) {
		cutl_prefix(cutl);
		cutl_message(cutl, msg, CUTL_FAILURE, file, line);
	}
	
	if (cutl->depth > 0) {
		longjmp(cutl->env, cutl->stage);
	}
}


// TESTING

void cutl_before(cutl_t *cutl, void (*func)(cutl_t*, void*)) {
	assert(cutl);
	
	cutl->before = func;
}

void cutl_after(cutl_t *cutl, void (*func)(cutl_t*, void*)) {
	assert(cutl);
	
	cutl->after = func;
}

void cutl_run(cutl_t *parent, const char *name, void (*func)(cutl_t*, void*),
               void *data) {
	assert(parent);
	
	parent->is_suite = true;
	
	cutl_t child = {
		.name = name,
		.depth = parent->depth + 1,
		.parent = parent,
		.settings = parent->settings,
	};
	cutl_t *cutl = &child;
	
	
	// calls the before function, stops if it fails.
	// calls the test function, continues if it fails.
	// calls the after function, stops if it fails.
	switch (setjmp(cutl->env)) {
	case 0:
		cutl->stage = CUTL_BEFORE;
		if (parent->before) {
			parent->before(cutl, data);
		}
		
		cutl->stage++;
		func(cutl, data);
	
	case CUTL_TESTING:
		cutl->stage++;
		if (parent->after) {
			parent->after(cutl, data);
		}
	}
	
	
	if (!cutl->is_suite) {
		if (cutl->failed) {
			cutl->nb_failed++;
		} else {
			cutl->nb_passed++;
		}
	}
	
	if (cutl->is_prefixed
	    || (!cutl->is_suite && VERB_CHECK(cutl, CUTL_SUCCESS))
	    || ( cutl->is_suite && VERB_CHECK(cutl, CUTL_SUITE))
	) {
		cutl_prefix(cutl);
		cutl_suffix(cutl);
	}
	
	parent->nb_passed += cutl->nb_passed;
	parent->nb_failed += cutl->nb_failed;
	if (cutl->failed) parent->failed = true;
}


// ASSERT

void cutl_assert(cutl_t *cutl, int val, const char *msg, const char *file,
                 int line) {
	assert(cutl);
	
	if(!val) {
		cutl_fail(cutl, msg, file, line);
	}
}


// REPORTING

void cutl_summary(cutl_t *cutl) {
	if (VERB_CHECK(cutl, CUTL_STATUS)) {
		cutl_indent(cutl);
		fprintf(
			cutl->settings->output, 
			"Test summary: %d failed, %d passed.\n", 
			cutl->nb_failed, cutl->nb_passed
		);
	}
}