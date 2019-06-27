// This simple example can be compiled with `cc simple.c -lcutl`
#include <cutl.h>

void simple_test(Cutl *cutl) {
       cutl_assert_true(cutl, 34 == 45); // macro wrapping cutl_assert_at()
}

int main(void) {
       Cutl *cutl = cutl_new(NULL);
       cutl_test(cutl, simple_test); // macro wrapping cutl_run()
       return cutl_summary(cutl);
}
