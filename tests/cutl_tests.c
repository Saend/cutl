#include "tests.h"


// CUTL SUITE

extern void cutl_parse_args_suite(Cutl *cutl);
extern void cutl_message_suite(Cutl *cutl);
extern void cutl_run_suite(Cutl *cutl);
extern void cutl_summary_suite(Cutl *cutl);
extern void cutl_get_suite(Cutl *cutl);

int main(int argc, char *argv[])
{
	Cutl *cutl = cutl_new("CUTL self-tests");
	cutl_parse_args(cutl, argc, argv);

	cutl_assert_equal(cutl, CUTL_VERSION_MAJOR, 2);

	cutl_suite(cutl, cutl_parse_args_suite);
	cutl_suite(cutl, cutl_message_suite);
	cutl_suite(cutl, cutl_run_suite);
	cutl_suite(cutl, cutl_summary_suite);
	cutl_suite(cutl, cutl_get_suite);

	int failed = cutl_summary(cutl);
	cutl_free(cutl);
	return failed;
}
