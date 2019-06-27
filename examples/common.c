#include <cutl.h>

#include <math.h>
#include <fenv.h>


// TRIG TESTS

void sin_test(Cutl *cutl)
{
	cutl_assert_equal(cutl, sin(0), 0);
	cutl_assert_equal(cutl, sin(M_PI/2), 1);
}

void asin_test(Cutl *cutl)
{
	cutl_assert_equal(cutl, asin(0), 0);
	cutl_assert_equal(cutl, asin(1), M_PI/2);
}

void trig_suite(Cutl *cutl)
{
	cutl_test(cutl, sin_test);
	cutl_test(cutl, asin_test);
}


// FLOAT TESTS

void float_setup(Cutl *cutl, void *_, void *__)
{
	cutl_check(
		cutl, feclearexcept(FE_ALL_EXCEPT) == 0, 
		"Could not clear floating-point exceptions."
	);
}

void div_by_zero_test(Cutl *cutl)
{
	float f = 1.0/0.0;
	cutl_assert_true(cutl, fetestexcept(FE_DIVBYZERO));
}

void invalid_test(Cutl *cutl)
{
	float f = sqrt(-1);
	cutl_assert_true(cutl, fetestexcept(FE_INVALID));
}

void float_suite(Cutl *cutl)
{
	cutl_at_start(cutl, float_setup, NULL);

	cutl_test(cutl, div_by_zero_test);
	cutl_test(cutl, invalid_test);
}


// TEST CONTEXT

int main(int argc, char *argv[]) {
	Cutl *cutl = cutl_new("Math tests");
	cutl_parse_args(cutl, argc, argv);

	cutl_suite(cutl, trig_suite);
	cutl_suite(cutl, float_suite);

	return cutl_summary(cutl);
}
