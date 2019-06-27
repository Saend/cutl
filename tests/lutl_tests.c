#include "tests.h"

#include <lutl.h>


// LUTL SUITE

int main(int argc, char *argv[])
{
	Cutl *cutl = cutl_new("LUTL self-tests");
	cutl_parse_args(cutl, argc, argv);

	lutl_dofile(cutl, NULL, "tests/lutl_tests.lua");

	int failed = cutl_summary(cutl);
	cutl_free(cutl);
	return failed;
}
