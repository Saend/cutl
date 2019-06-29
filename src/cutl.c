/** \file cutl.h
 * \brief CUTL: A simple C unit testing library.
 * \author Baptiste "Saend" CEILLIER
 * \copyright See LICENSE file.
 */



// INCLUDES

#include <cutl.h>

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <setjmp.h>



// DEFAULT VALUES

#define CUTL_DEFAULT_NAME "Unit tests"

#define CUTL_DEFAULT_INDENT "\t"

#define CUTL_DEFAULT_OUTPUT stdout

#define CUTL_PASS_COLOR "[0;32m"

#define CUTL_FAIL_COLOR "[0;31m"



// UTILITY MACROS

#define CUTL_VERBCHECK(cutl, verb) ((cutl)->settings.verbosity & (verb))



// INTERNAL DATA STRUCTURES

typedef struct {
	FILE *output;
	int verbosity;
	int color;
	const char *indent;
} Cutl_Settings;

typedef struct {
	int last_id;
} Cutl_Globals;

struct Cutl {
	const char * const name;
	const int id;
	const int depth;
	Cutl * const parent;
	Cutl_Globals * const globals;

	Cutl_Settings settings;

	void *test_data;

	Cutl_Func *start;
	void *start_data;

	Cutl_Func *end;
	void *end_data;

	Cutl_Func *interrupt;
	void *interrupt_data;

	jmp_buf env;
	enum {
		CUTL_STAGE_BEFORE = 1,
		CUTL_STAGE_TESTING = 2,
		CUTL_STAGE_AFTER = 3,
	} stage;

	bool failed, error;
	int nb_children, nb_passed, nb_failed;
	bool is_prefixed, is_infixed;
	bool has_color;
};



// MEMORY MANAGEMENT

Cutl *cutl_new(const char *name)
{
	name = name ? name : CUTL_DEFAULT_NAME;

	Cutl *cutl = malloc(sizeof(*cutl));
	assert(cutl);

	Cutl_Globals *globals = calloc(1, sizeof(*globals));

	Cutl _cutl = {.name = name, .globals = globals};
	memcpy(cutl, &_cutl, sizeof(_cutl));

	cutl_set_output(cutl, NULL);
	cutl_set_verbosity(cutl, -1);
	cutl_set_color(cutl, -1);
	cutl_set_indent(cutl, NULL);

	return cutl;
}


void cutl_free(Cutl *cutl)
{
	assert(cutl);
	assert(cutl->depth == 0);

	free(cutl->globals);

	memset(cutl, 0, sizeof(*cutl));

	free(cutl);
}



// SETTINGS

void cutl_set_output(Cutl *cutl, FILE *output)
{
	assert(cutl);

	cutl->settings.output = output ? output : CUTL_DEFAULT_OUTPUT;
	cutl_set_color(cutl, cutl->settings.color);
}

FILE *cutl_get_output(const Cutl *cutl)
{
	assert(cutl);

	return cutl->settings.output;
}


void cutl_set_verbosity(Cutl *cutl, int verbosity)
{
	assert(cutl);

	cutl->settings.verbosity = verbosity >= 0 ? verbosity : CUTL_NORMAL;
}

int cutl_get_verbosity(const Cutl *cutl)
{
	assert(cutl);

	return cutl->settings.verbosity;
}


void cutl_set_color(Cutl *cutl, int color)
{
	assert(cutl);

	cutl->settings.color = color;
}

bool cutl_get_color(const Cutl *cutl)
{
	assert(cutl);

	return cutl->has_color;
}


void cutl_set_indent(Cutl *cutl, const char *indent)
{
	assert(cutl);

	cutl->settings.indent = indent ? indent : CUTL_DEFAULT_INDENT;
}

const char *cutl_get_indent(const Cutl *cutl)
{
	assert(cutl);

	return cutl->settings.indent;
}



// MESSAGING

static void cutl_indent(const Cutl *cutl)
{
	assert(cutl);

	if (CUTL_VERBCHECK(cutl, CUTL_SUITES)) {
		for (int i=0; i<cutl->depth-1; ++i) {
			fprintf(
				cutl->settings.output, "%s",
				cutl->settings.indent
			);
		}
	}
}


static void cutl_infix(Cutl *cutl, const char *str)
{
	assert(cutl);


	if (cutl->is_infixed || !cutl->is_prefixed || cutl->depth == 0) return;

	fprintf(cutl->settings.output, "%s\n", str);
	cutl->is_infixed = true;
}


static void cutl_prefix(Cutl *cutl)
{
	assert(cutl);

	if (cutl->is_prefixed || cutl->depth == 0) return;

	if (CUTL_VERBCHECK(cutl, CUTL_SUITES)) {
		cutl_prefix(cutl->parent);
	}

	cutl_infix(cutl->parent, ":");
	cutl_indent(cutl);
	fprintf(cutl->settings.output, "%s", cutl->name);
	cutl->is_prefixed = true;
}


static void cutl_suffix(Cutl *cutl)
{
	assert(cutl);

	if (cutl->depth == 0) return;

	if (cutl->is_infixed) {
		cutl_indent(cutl);
		fprintf(cutl->settings.output, "%s", cutl->name);
	}

	fprintf(
		cutl->settings.output, " %s.\n",
		cutl->error ? "canceled" : cutl->failed ? "failed" : "passed"
	);
}


static void cutl_vmessage_at(
	Cutl *cutl, int type, const char *file, int line, const char *fmt,
	va_list ap)
{
	assert(cutl);
	assert(fmt);

	if (type & CUTL_ERROR) {
		cutl->error = true;
		cutl->failed = true;
	} if (type & CUTL_FAIL) {
		cutl->failed = true;
	}

	if (!CUTL_VERBCHECK(cutl, type)) return;


	cutl_prefix(cutl);
	cutl_infix(cutl, ":");
	cutl_indent(cutl);
	if (cutl->depth > 0) {
		//FIXME: What is that?
		fprintf(cutl->settings.output, "%s", cutl->settings.indent);
	}

	const char *kind = NULL;
	if (type & CUTL_ERROR) {
		kind = cutl->has_color
			? "\033"CUTL_FAIL_COLOR"ERROR\033[0m" : "ERROR";
	} else if (type & CUTL_FAIL) {
		kind = cutl->has_color
			? "\033"CUTL_FAIL_COLOR"FAIL\033[0m" : "FAIL";
	} else if (type & CUTL_WARN) {
		kind = "WARN";
	} else if (type & CUTL_INFO) {
		kind = "INFO";
	}

	if (kind) {
		if (file) {
			fprintf(
				cutl->settings.output, "[%s %s:%d] ", kind,
				file, line
			);
		} else {
			fprintf(cutl->settings.output, "[%s] ", kind);
		}
	}

	vfprintf(cutl->settings.output, fmt, ap);

	fprintf(cutl->settings.output, "\n");
}

void cutl_message_at(
	Cutl *cutl, int type, const char *file, int line, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	cutl_vmessage_at(cutl, type, file, line, fmt, ap);
	va_end(ap);
}



// TESTING

void cutl_at_start(Cutl *cutl, Cutl_Func *func, void *data)
{
	assert(cutl);

	cutl->start = func;
	cutl->start_data = data;
}


void cutl_at_end(Cutl *cutl, Cutl_Func *func, void *data)
{
	assert(cutl);

	cutl->end = func;
	cutl->end_data = data;
}


void cutl_at_interrupt(Cutl *cutl, Cutl_Func *func, void *data)
{
	assert(cutl);

	cutl->interrupt = func;
	cutl->interrupt_data = data;
}


int cutl_run(Cutl *parent, const char *name, Cutl_Func *test, void *data)
{
	assert(parent);
	assert(parent->globals);
	assert(test);

	if (parent->error) return 1;

	// Protect variable from compiler optimizations, which *may* cause local
	// variables to be stored inside registers and thus restored end a
	// call to longjmp().
	volatile Cutl child = {
		.name = name,
		.id = ++parent->globals->last_id,
		.depth = parent->depth + (name ? 1 : 0),
		.parent = parent,
		.globals = parent->globals,
		.settings = parent->settings,
		.has_color = parent->has_color,
		.test_data = data,
	};
	Cutl *cutl = (Cutl*) &child;

	// Testing
	if (parent->start) {
		if (setjmp(cutl->env) == 0) {
			parent->start(cutl, parent->start_data);
		}
	}

	if (!cutl->failed) {
		if (setjmp(cutl->env) == 0) {
			test(cutl, cutl->test_data);
		}
		if (parent->end) {
			if (setjmp(cutl->env) == 0) {
				parent->end(cutl, parent->end_data);
			}
		}
	}

	// Reporting
	if (cutl->is_prefixed
		|| (cutl->nb_children == 0 && CUTL_VERBCHECK(cutl, CUTL_TESTS))
		|| (cutl->nb_children != 0 && CUTL_VERBCHECK(cutl, CUTL_SUITES))
	) {
		cutl_prefix(cutl);
		cutl_suffix(cutl);
	}

	if (cutl->nb_children == 0 && cutl->name != NULL) {
		parent->nb_children++;
		if (cutl->failed) {
			parent->nb_failed++;
		} else {
			parent->nb_passed++;
		}
	} else {
		parent->nb_children += cutl->nb_children;
		parent->nb_passed += cutl->nb_passed;
		parent->nb_failed += cutl->nb_failed;
	}

	if (cutl->failed) parent->failed = true;
	if (cutl->error) {
		parent->error = true;
	}

	return cutl_get_failed(cutl);
}


void cutl_interrupt(Cutl *cutl)
{
	assert(cutl);

	if (cutl->interrupt) {
		Cutl_Func* interrupt = cutl->interrupt;
		cutl->interrupt = NULL;
		interrupt(cutl, cutl->interrupt_data);
	}

	if (cutl->id > 0) {
		longjmp(cutl->env, cutl->stage);
	} else {
		exit(cutl_get_failed(cutl));
	}
}


void cutl_fail_at(Cutl *cutl, const char *file, int line, const char *fmt, ...)
{
	assert(cutl);

	va_list ap;
	va_start(ap, fmt);
	cutl_vmessage_at(cutl, CUTL_FAIL, file, line, fmt, ap);
	va_end(ap);

	cutl_interrupt(cutl);
}


void cutl_error_at(Cutl *cutl, const char *file, int line, const char *fmt, ...)
{
	assert(cutl);

	va_list ap;
	va_start(ap, fmt);
	cutl_vmessage_at(cutl, CUTL_ERROR, file, line, fmt, ap);
	va_end(ap);

	cutl_interrupt(cutl);
}



// ASSERT

void cutl_assert_at(
	Cutl *cutl, bool val, const char *file, int line, const char *fmt, ...)
{
	assert(cutl);

	if (!val) {
		va_list ap;
		va_start(ap, fmt);
		cutl_vmessage_at(cutl, CUTL_FAIL, file, line, fmt, ap);
		va_end(ap);

		return cutl_interrupt(cutl);
	}
}


void cutl_check_at(
	Cutl *cutl, bool val, const char *file, int line, const char *fmt, ...)
{
	assert(cutl);

	if (!val) {
		va_list ap;
		va_start(ap, fmt);
		cutl_vmessage_at(cutl, CUTL_ERROR, file, line, fmt, ap);
		va_end(ap);

		return cutl_interrupt(cutl);
	}
}



// REPORTING

int cutl_summary(Cutl *cutl)
{
	assert(cutl);

	if (!CUTL_VERBCHECK(cutl, CUTL_SUMMARY)) return cutl->nb_failed;

	const char *start_color = "", *stop_color = "";
	if (cutl->settings.color) {
		start_color = (cutl->failed || cutl->error)
			? "\033" CUTL_FAIL_COLOR
			: "\033" CUTL_PASS_COLOR;
		stop_color = "\033[0m";
	}

	cutl_indent(cutl);

	if (cutl->error) {
		fprintf(
			cutl->settings.output,
			"%s%s summary: canceled.%s\n", start_color, cutl->name,
			stop_color);
	} else {
		fprintf(
			cutl->settings.output,
			"%s%s summary: %d failed, %d passed.%s\n",
			start_color, cutl->name,
			cutl->nb_failed, cutl->nb_passed,
			stop_color
		);
	}

	return cutl->nb_failed;
}



// MISCELLANEOUS

const char *cutl_get_name(const Cutl *cutl)
{
	assert(cutl);

	return cutl->name;
}


int cutl_get_id(const Cutl *cutl)
{
	assert(cutl);

	return cutl->id;
}


int cutl_get_depth(const Cutl *cutl)
{
	assert(cutl);

	return cutl->depth;
}


const Cutl *cutl_get_parent(const Cutl *cutl)
{
	assert(cutl);

	return cutl->parent;
}


void *cutl_get_data(const Cutl *cutl)
{
	assert(cutl);

	return cutl->test_data;
}


void cutl_set_data(Cutl *cutl, void *data)
{
	cutl_check_cutl(cutl);

	cutl->test_data = data;
}


int cutl_get_children(Cutl *cutl)
{
	assert(cutl);

	return cutl->nb_children;
}


int cutl_get_passed(Cutl *cutl)
{
	assert(cutl);

	return cutl->nb_passed;
}


int cutl_get_failed(Cutl *cutl)
{
	assert(cutl);

	if (cutl->failed && cutl->nb_failed > 0) {
		return cutl->nb_failed;
	}
	return cutl->failed;
}


bool cutl_get_error(Cutl *cutl)
{
	return cutl->error;
}

