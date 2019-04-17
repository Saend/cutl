/* CUTL - A simple C unit testing library.
 * AUTHOR: Baptiste "Saend" CEILLIER
 * OVERVIEW: This is a simple example showing how to run a Lua test from C.
 * Can be built with: `cc c_example.c -llua -lm -llutl`
 */
#include <lutl.h>


int main(void) {
	cutl_t *cutl = cutl_new("Example tests");
	lutl_dofile(cutl, "lua_example.lua");
	cutl_summary(cutl);
	cutl_free(cutl);
	return 0;
}
