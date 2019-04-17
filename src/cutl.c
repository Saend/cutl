/* CUTL - A simple C unit testing library. 
 * AUTHOR: Baptiste "Saend" CEILLIER
 */
#include <cutl.h>

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>


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
	assert(cutl);

	cutl->failed = false;
	cutl->nb_failed = 0;
	cutl->nb_passed = 0;
	
	cutl->settings->output = stdout;
	cutl->settings->verbosity = CUTL_NORMAL;
}

void cutl_output(cutl_t *cutl, FILE *output) {
	assert(cutl);
	assert(output);

	cutl->settings->output = output;
}

void cutl_verbosity(cutl_t *cutl, int verbosity) {
	assert(cutl);

	cutl->settings->verbosity = verbosity;
}


// TEST REPORTING

static void cutl_indent(cutl_t *cutl) {
	assert(cutl);

	int i;
	if (VERB_CHECK(cutl, CUTL_SUITE)) {
		for (i=0; i<cutl->depth-1; ++i) {
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

void cutl_message_at(cutl_t *cutl, int verbosity, const char *msg,
                  const char *file, int line) {
	assert(cutl);
	
	if (verbosity & CUTL_FAIL) 
		cutl->failed = true;	

	if (!msg) return;

	if (!VERB_CHECK(cutl, verbosity)) return;
		
	cutl_prefix(cutl);
	cutl_infix(cutl, ":");
	cutl_indent(cutl);
	if (cutl->depth > 0) {
		fprintf(cutl->settings->output, "\t");
	}
	
	if (verbosity & CUTL_FAIL) { 
		fprintf(cutl->settings->output, "FAIL: ");
	} else if (verbosity & CUTL_ERROR) {
		fprintf(cutl->settings->output, "ERROR: ");
	} else if (verbosity & CUTL_WARN) {
		fprintf(cutl->settings->output, "WARN: ");
	} else if (verbosity & CUTL_INFO) {
		fprintf(cutl->settings->output, "INFO: ");
	}

	if (file) {
		fprintf(
			cutl->settings->output, "%s:%d: %s\n",
			file, line, msg
		);
	} else {
		fprintf(cutl->settings->output, "%s\n", msg);
	}
}

void cutl_fail_at(cutl_t *cutl, const char* msg, const char*file, int line) {
	assert(cutl);
	
	cutl_message_at(cutl, CUTL_FAIL, msg, file, line);
	
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
	// reminder: setjmp() returns 0 on first call and cutl->stage on jump
	switch (setjmp(cutl->env)) {
	case 0:
		cutl->stage = CUTL_BEFORE;
		if (parent->before) {
			parent->before(cutl, data);
		}
		
		cutl->stage++;	// = CUTL_TESTING
		func(cutl, data);
	
	case CUTL_TESTING:
		cutl->stage++;	// = CUTL_AFTER
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

void cutl_assert_at(cutl_t *cutl, int val, const char *msg, const char *file,
                 int line) {
	assert(cutl);
	
	if(!val) {
		cutl_fail_at(cutl, msg, file, line);
	}
}


// REPORTING

int cutl_has_failed(const cutl_t *cutl) {
	return cutl->nb_failed;
}

int cutl_summary(cutl_t *cutl) {
	assert(cutl);

	if (!VERB_CHECK(cutl, CUTL_SUMMARY)) return cutl->nb_failed;
	
	const char *start_color = "", *stop_color = "";
	if(getenv("SHELL") != NULL) {
		start_color = cutl->failed ? "\033[22;31m" : "\033[22;32m";
		stop_color = "\033[0m";
	}
	
	cutl_indent(cutl);
	fprintf(
		cutl->settings->output, "%s%s summary: %d failed, %d passed.%s\n", 
		start_color, cutl->name ? cutl->name : "test",
		cutl->nb_failed, cutl->nb_passed, stop_color
	);
	
	return cutl->nb_failed;
}
