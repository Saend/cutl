#include "tests.h"

#include <stdlib.h>
#include <string.h>


// HELPER FUNCTION

static char *read_file(Cutl *cutl, FILE* in)
{
	fseek(in, 0, SEEK_END);
	size_t length = ftell(in);
	rewind(in);

	char *buffer = malloc(length + 1);
	cutl_check(cutl, buffer != NULL, "Malloc failed.");
	size_t size = fread(buffer, 1, length, in);
	buffer[size] = '\0';

	return buffer;
}



// CUSTOM ASSERTS

void cutl_assert_content_at(
	Cutl *cutl, const char *file, int line, FILE *input,
	const char *content)
{
	size_t pos = 0;
	int c;

	rewind(input);
	while ((c = fgetc(input)) == content[pos++]);

	if (pos < strlen(content) || c != EOF) {
		cutl_fail_at( cutl, file, line, "File content differs.");
	}
}



// TEST FIXTURE

static void fixture_at_interrupt(Cutl *cutl_under_test, void* data)
{
	Cutl *cutl = data;
	cutl_fail(cutl, "Test got interrupted.");
}

void fixture_setup(Cutl *cutl, void *data)
{
	Fixture *fix = calloc(1, sizeof(*fix));
	cutl_check(cutl, fix != NULL, "Malloc failed.");

	fix->cutl = cutl_new(NULL);
	cutl_check(cutl, fix->cutl != NULL, "Could not create test context.");

	fix->output = tmpfile();
	cutl_check(cutl, fix->output != NULL, "Could not make tmp file.");

	cutl_set_data(cutl, fix);
	cutl_set_output(fix->cutl, fix->output);
	cutl_at_interrupt(fix->cutl, fixture_at_interrupt, cutl);
}


void fixture_clean(Cutl *cutl, void *data)
{
	Fixture *fix = cutl_get_data(cutl);

	cutl_check(cutl, fix->output != NULL, "Output file is not opened.");

	if (cutl_get_failed(cutl)) {
		char *output = read_file(cutl, fix->output);
		cutl_message_at(
			cutl, CUTL_INFO, NULL, 0, "Test output:\n%s", output
		);
		free(output);
	}

	fclose(fix->output);
	cutl_free(fix->cutl);
	free(fix);
}


void at_interrupt_longjmp(Cutl *cutl, void *data)
{
	Fixture *fix = data;

	longjmp(fix->env, 1);
}

