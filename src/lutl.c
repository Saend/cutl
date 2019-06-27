/* CUTL - A simple C unit testing library.
 * AUTHOR: Baptiste "Saend" CEILLIER
 */
#include <lutl.h>


#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <lualib.h>
#include <lauxlib.h>



// INTERNAL STRUCT

typedef struct {
	Cutl *cutl;
	bool dynamic;
	int start, test, end, interrupt;
} Lutl;



static Lutl *lutl_tolutl(lua_State *L, int index)
{
	return luaL_testudata(L, index, "Lutl");
}


static Lutl *lutl_checklutl(lua_State *L, int arg)
{
	return luaL_checkudata(L, arg, "Lutl");
}


static void lutl_start_iface(Cutl *cutl, void *data);
static void lutl_end_iface(Cutl *cutl, void *data);
static void lutl_interrupt_iface(Cutl *cutl, void *data);
static Lutl *lutl_register(lua_State *L, Cutl *cutl)
{
	const int top = lua_gettop(L);

	// Create new Lutl
	Lutl *lutl = lua_newuserdata(L, sizeof(*lutl));
	lutl->cutl = cutl;
	lutl->dynamic = cutl_get_id(cutl) == 0;
	lutl->start = LUA_NOREF;
	lutl->test = LUA_NOREF;
	lutl->end = LUA_NOREF;
	lutl->interrupt = LUA_NOREF;
	luaL_setmetatable(L, "Lutl");

	cutl_at_start(cutl, lutl_start_iface, lutl);
	cutl_at_end(cutl, lutl_end_iface, lutl);
	cutl_at_interrupt(cutl, lutl_interrupt_iface, L);

	// Push into reftable
	luaL_getsubtable(L, LUA_REGISTRYINDEX, "cutl");
	lua_pushvalue(L, -2);
	lua_rawsetp(L, -2, cutl);
	lua_pop(L, 1); // Pop reftable

	assert(lua_gettop(L) == top + 1);
	return lutl;
}


static Lutl *lutl_lookup(lua_State *L, Cutl *cutl)
{
	const int top = lua_gettop(L);

	luaL_getsubtable(L, LUA_REGISTRYINDEX, "cutl");
	lua_rawgetp(L, -1, cutl);

	Lutl *lutl = lutl_tolutl(L, -1);
	lua_remove(L, -2);

	assert(lua_gettop(L) == top + 1);
	return lutl;
}


void lutl_pushcutl(lua_State *L, Cutl *cutl)
{
	const int top = lua_gettop(L);

	Lutl *lutl = lutl_lookup(L, cutl);
	if (lutl == NULL) {
		lua_pop(L, 1);
		lutl_register(L, cutl);
	}

	assert(lua_gettop(L) == top + 1);
}


Cutl *lutl_checkcutl(lua_State *L, int arg)
{
	Lutl *lutl = lutl_checklutl(L, arg);
	if (lutl->cutl == NULL) {
		luaL_error(L, "Bad test context: out of scope.");
	}
	return lutl->cutl;
}


static int lutl_gc(lua_State *L)
{
	Lutl *lutl = lutl_checklutl(L, 1);

	luaL_unref(L, LUA_REGISTRYINDEX, lutl->start);
	luaL_unref(L, LUA_REGISTRYINDEX, lutl->test);
	luaL_unref(L, LUA_REGISTRYINDEX, lutl->end);
	luaL_unref(L, LUA_REGISTRYINDEX, lutl->interrupt);

	lutl->start = LUA_NOREF;
	lutl->test = LUA_NOREF;
	lutl->end = LUA_NOREF;
	lutl->interrupt = LUA_NOREF;

	if (lutl->dynamic) {
		cutl_free(lutl->cutl);
		lutl->cutl = NULL;
	}

	return 0;
}


static void lutl_pack(lua_State *L, int len)
{
	const int top = lua_gettop(L);

	const int index = lua_gettop(L) - (len -1);

	lua_newtable(L);
	lua_insert(L, index);
	for (int i=0; i<len; i++) {
		lua_rawseti(L, index, len-i);
	}

	assert(lua_gettop(L) == index);
	assert(lua_type(L, index) == LUA_TTABLE);
	assert(lua_gettop(L) == top - (len - 1));
}


static int lutl_unpack(lua_State *L, int ref)
{
	if (ref == LUA_NOREF) return 0;

	const int top = lua_gettop(L);

	int type = lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	assert(type == LUA_TTABLE);

	const int index = lua_gettop(L);
	const int len = lua_rawlen(L, index);
	lua_checkstack(L, len);

	for (int i=0; i<len; i++) {
		lua_rawgeti(L, index, i+1);
	}
	lua_remove(L, index);

	assert(lua_gettop(L) == top + len);
	return len;
}



static void lutl_parse_error(lua_State *L, Cutl *cutl)
{
	const char *msg = "unknown Lua error";
	if (!lua_isnil(L, -1)) {
		msg = lua_tostring(L, -1);
	}

	// Find location.
	lua_Debug ar;
	int depth = 0;
	while (lua_getstack(L, depth++, &ar)) {
		lua_getinfo(L, "Sl", &ar);
		if (ar.currentline != -1) break;
	}

	const char *file = ar.short_src;
	int line = ar.currentline;

	// Remove 'file:line: ' prefix.
	size_t len = strlen(file);
	if (strncmp(msg, file, len) == 0) {
		msg += len;
		while (isdigit(*msg) || *msg == ':' || *msg == ' ') msg++;
	}

	// Remove './'
	if (strncmp(file, "./", 2) == 0) {
		file += 2;
	}

	// HACK: Assume capitalized message are not Lua errors.
	const char *fmt = islower(msg[0]) ? "Lua: %s." : "%s";
	cutl_message_at(
		cutl, CUTL_ERROR, line > 0 ? file : NULL, line, fmt, msg
	);

	lua_pop(L, 1); // pop string
}


static void lutl_call(lua_State *L, int ref, Cutl *cutl)
{
	const int top = lua_gettop(L);

	if (ref == LUA_NOREF) return;

	lua_State *T = lua_newthread(L);
	cutl_at_interrupt(cutl, lutl_interrupt_iface, T);

	int len = lutl_unpack(T, ref);
	assert(lua_isfunction(T, 1));
	lutl_pushcutl(T, cutl);
	lua_insert(T, 2);

	int retval = lua_resume(T, L, len);
	if (retval != LUA_OK && retval != LUA_YIELD) {
		lutl_parse_error(T, cutl);
	}
	lua_pop(L, 1); // pop thread

	assert(lua_gettop(L) == top);
}



// INTERFACE FUNCTIONS

static void lutl_test_iface(Cutl *cutl, void *data)
{
	lua_State *L = data;
	Lutl *lutl = lutl_lookup(L, cutl);
	assert(lutl != NULL);

	lutl_call(L, lutl->test, cutl);
}


static void lutl_interrupt_iface(Cutl *cutl, void *data)
{
	lua_State *T = data;
	Lutl *lutl = lutl_lookup(T, cutl);

	// Call `at_interrupt` function
	lutl_call(T, lutl->interrupt, cutl);
	lua_yield(T, 0);
}


static void lutl_end_iface(Cutl *cutl, void *data)
{
	lua_State *L = cutl_get_data(cutl);
	Lutl *parent = data;
	Lutl *lutl = lutl_lookup(L, cutl);
	assert(lutl != NULL);

	// Call `at_end` function
	lutl_call(L, parent->end, cutl);

	// Cleanup
	lutl->cutl = NULL;
}


static void lutl_start_iface(Cutl *cutl, void *data)
{
	lua_State *L = cutl_get_data(cutl);
	Lutl *parent = data;

	Lutl *lutl = lutl_register(L, cutl); // keep on stack!
	lua_insert(L, 1);
	lutl->test = luaL_ref(L, LUA_REGISTRYINDEX);

	// Call `at_start` function
	lutl_call(L, parent->start, cutl);

	// Cleanup in case of failure
	if (cutl_get_failed(cutl)) {
		lutl->cutl = NULL;
	}
}



// DO FUNCTIONS

static void lutl_do_iface(Cutl *cutl, void *data)
{
	lua_State *L = data;

	// Change main Lutl
	luaL_requiref(L, "lutl", luaopen_lutl, 0);
	Lutl *lutl = lutl_tolutl(L, -1);

	luaL_getsubtable(L, LUA_REGISTRYINDEX, "cutl");

	lua_pushvalue(L, -2);
	lua_rawsetp(L, -2, cutl);

	lua_pushnil(L);
	lua_rawsetp(L, -2, lutl->cutl);

	lua_pop(L, 2);

	cutl_free(lutl->cutl);
	lutl->cutl = cutl;
	lutl->dynamic = false;

	// Test setup
	cutl_at_start(cutl, lutl_start_iface, lutl);
	cutl_at_end(cutl, lutl_end_iface, lutl);

	lutl->test = luaL_ref(L, LUA_REGISTRYINDEX);
	lutl_call(L, lutl->test, cutl);
	lutl->cutl = NULL;
}


int lutl_dofile(Cutl *cutl, const char *name, const char *filename)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if (luaL_loadfile(L, filename) != LUA_OK) {
		cutl_message_at(
			cutl, CUTL_ERROR, "lutl_dofile()", 0,
			"%s.", lua_tostring(L, -1)
		);
	} else {
		lutl_pack(L, 1);
		cutl_run(cutl, name, lutl_do_iface, L);
	}

	lua_close(L);
	return cutl_get_failed(cutl);
}


int lutl_dostring(Cutl *cutl, const char *name, const char *str)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requiref(L, "lutl", luaopen_lutl, 1);
	lua_pop(L, 1);

	if (luaL_loadstring(L, str) != LUA_OK) {
		cutl_message_at(
			cutl, CUTL_ERROR, "lutl_dostring()", 0,
			"%s.", lua_tostring(L, -1)
		);
	} else {
		lutl_pack(L, 1);
		cutl_run(cutl, name, lutl_do_iface, L);
	}

	lua_close(L);
	return cutl_get_failed(cutl);
}



// BINDING FUNCTIONS

int lutl_new(lua_State *L)
{

	const char *name = luaL_opt(L, luaL_checkstring, 1, NULL);
	lutl_register(L, cutl_new(name));
	return 1;
}


static int lutl_closef(lua_State *L)
{
	lua_pushboolean(L, true);
	return 1;
}


int lutl_set_output(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	luaL_Stream *stream = luaL_checkudata(L, 2, LUA_FILEHANDLE);

	cutl_set_output(cutl, stream->f);

	return 0;
}


int lutl_get_output(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);

	luaL_Stream *stream = lua_newuserdata(L, sizeof(*stream));
	luaL_setmetatable(L, LUA_FILEHANDLE);
	stream->f = cutl_get_output(cutl);
	stream->closef = lutl_closef;

	return 1;
}


int lutl_set_verbosity(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	int verbosity = luaL_checkinteger(L, 2);

	cutl_set_verbosity(cutl, verbosity);

	return 0;
}


int lutl_get_verbosity(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	int verbosity = cutl_get_verbosity(cutl);
	lua_pushinteger(L, verbosity);
	return 1;
}


int lutl_set_color(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	int color = luaL_opt(L, lua_toboolean, 2, -1);

	cutl_set_color(cutl, color);

	return 0;
}


int lutl_get_color(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	bool color = cutl_get_color(cutl);
	lua_pushboolean(L, color);
	return 1;
}


int lutl_set_indent(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	const char *indent = luaL_checklstring(L, 2, NULL);

	cutl_set_indent(cutl, indent);

	return 0;
}


int lutl_parse_args(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);

	// Compute argc
	int argc = 0;
	while (lua_geti(L, 2, ++argc) != LUA_TNIL) {
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	// Prepare executable name.
	lua_Debug ar;
	lua_getstack(L, 0, &ar);
	lua_getinfo(L, "S", &ar);
	const char *name = ar.source;
	if (*name == '@' || *name == '=') {
		name++;
	}
	if (strncmp(name, "./", 2) == 0) {
		name += 2;
	}

	// Build argv
	const char **argv = calloc(argc, sizeof(*argv));
	assert(argv != NULL);

	for (int i=0; i<argc; i++) {
		lua_geti(L, 2, i);
		const char *str = lua_tostring(L, -1);
		if (i == 0 && str == NULL) {
			str = name;
		}
		argv[i] = str;
		lua_pop(L, 1);
	}

	// Parse
	cutl_parse_args(cutl, argc, (char**) argv);

	free(argv);
	return 0;
}


static int lutl_checkverbobj(lua_State *L, int arg)
{
	enum {LUTL_NUM_VERBOBJ = 6};
	static const char * const LUTL_VERBOBJ_NAMES[LUTL_NUM_VERBOBJ+1] = {
		"error", "fail", "warn", "info", "summary", NULL
	};
	static const int LUTL_VERBOBJ_VALUES[LUTL_NUM_VERBOBJ] = {
		CUTL_ERROR, CUTL_FAIL, CUTL_WARN, CUTL_INFO, CUTL_SUMMARY
	};

	const int option = luaL_checkoption(L, arg, NULL, LUTL_VERBOBJ_NAMES);
	assert(0 <= option && option < LUTL_NUM_VERBOBJ);
	return LUTL_VERBOBJ_VALUES[option];
}


static void lutl_get_location(
	lua_State *L, lua_Debug *ar, int level, const char **file, int *line)
{
	assert(file != NULL);
	assert(line != NULL);

	const bool found_loc = level > 0 && lua_getstack(L, level, ar)
		&& lua_getinfo(L, "Sl", ar) && ar->source[0] == '@';

	if (found_loc) {
		*file = ar->short_src;
		*line = ar->currentline;

		// Remove './'
		if (strncmp(*file, "./", 2) == 0) {
			*file += 2;
		}
	} else {
		*file = NULL;
		*line = 0;
	}
}


int lutl_message(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	int verbosity = lutl_checkverbobj(L, 2);
	const char *msg = luaL_checkstring(L, 3);
	int level = luaL_optinteger(L, 4, 1);

	lua_Debug ar;
	const char *file;
	int line;
	lutl_get_location(L, &ar, level, &file, &line);

	cutl_message_at(cutl, verbosity, file, line, msg);

	return 0;
}


int lutl_info(lua_State *L)
{
	lutl_checkcutl(L, 1);
	luaL_checkstring(L, 2);

	lua_pushstring(L, "info");
	lua_insert(L, 2);

	return lutl_message(L);
}


int lutl_warn(lua_State *L)
{
	lutl_checkcutl(L, 1);
	luaL_checkstring(L, 2);

	lua_pushstring(L, "warn");
	lua_insert(L, 2);

	return lutl_message(L);
}


int lutl_at_start(lua_State *L)
{
	Lutl *lutl = lutl_checklutl(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	lutl_pack(L, lua_gettop(L) - 1);

	luaL_unref(L, LUA_REGISTRYINDEX, lutl->start);
	lutl->start = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}


int lutl_at_end(lua_State *L)
{
	Lutl *lutl = lutl_checklutl(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	lutl_pack(L, lua_gettop(L) - 1);

	luaL_unref(L, LUA_REGISTRYINDEX, lutl->end);
	lutl->end = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}


int lutl_at_interrupt(lua_State *L)
{
	Lutl *lutl = lutl_checklutl(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	lutl_pack(L, lua_gettop(L) - 1);

	luaL_unref(L, LUA_REGISTRYINDEX, lutl->interrupt);
	lutl->interrupt = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}


int lutl_run(lua_State *L)
{
	Lutl *lutl = lutl_checklutl(L, 1);
	const char *name = lua_tostring(L, 2); // keep on stack!
	luaL_checktype(L, 3, LUA_TFUNCTION);
	lutl_pack(L, lua_gettop(L) - 2);

	lua_remove(L, 1);
	cutl_run(lutl->cutl, name, lutl_test_iface, L);
	lua_pushinteger(L, cutl_get_failed(lutl->cutl));
	return 1;
}


int lutl_test(lua_State *L)
{
	lutl_checkcutl(L, 1);
	const char *name = luaL_checkstring(L, lua_istable(L, 2) ? 3 : 2);
	if (!lua_istable(L, 2)) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		lua_insert(L, 2);
	}

	lua_getfield(L, 2, name);
	if (lua_isfunction(L, -1)) {
		lua_insert(L, 3);
	} else {
		return luaL_argerror(L, 2, "no such function");
	}

	// Build test name
	const int first_arg = 5;
	const int last_arg = lua_gettop(L);

	lua_pushstring(L, name);
	for (int i=first_arg; i<=last_arg; i++) {
		if(i == first_arg) {
			lua_pushstring(L, "('");
		}
		luaL_tolstring(L, i, 0);
		if (i == last_arg) {
			lua_pushstring(L, "')");
		} else {
			lua_pushstring(L, "', '");
		}
	}
	lua_concat(L, lua_gettop(L) - last_arg);
	lua_replace(L, 2);

	return lutl_run(L);
}


int lutl_suite(lua_State *L)
{
	return lutl_test(L);
}


int lutl_interrupt(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);

	cutl_interrupt(cutl);

	return 0;
}


int lutl_fail(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	const char *msg = luaL_checkstring(L, 2);
	int level = luaL_optinteger(L, 4, 1);

	lua_Debug ar;
	const char *file;
	int line;
	lutl_get_location(L, &ar, level, &file, &line);

	cutl_fail_at(cutl, file, line, "%s", msg);

	return 0;
}


int lutl_error(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	const char *msg = luaL_checkstring(L, 2);
	int level = luaL_optinteger(L, 4, 1);

	lua_Debug ar;
	const char *file;
	int line;
	lutl_get_location(L, &ar, level, &file, &line);

	cutl_error_at(cutl, file, line, "%s", msg);

	return 0;
}


int lutl_assert(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	luaL_checkany(L, 2);
	const char *msg = luaL_checkstring(L, 3);
	int level = luaL_optinteger(L, 4, 1);

	lua_Debug ar;
	const char *file;
	int line;
	lutl_get_location(L, &ar, level, &file, &line);

	cutl_assert_at(cutl, lua_toboolean(L, 2), file, line, "%s", msg);

	return 0;
}


int lutl_assert_true(lua_State *L)
{
	lutl_checkcutl(L, 1);
	luaL_checkany(L, 2);

	if (!lua_isstring(L, 3)) {
		lua_pushfstring(
			L, "'%s' is not true.", luaL_tolstring(L, 2, NULL)
		);
		lua_insert(L, 3);
		lua_pop(L, 1); // pop result of luaL_tolstring()
	}

	return lutl_assert(L);
}


int lutl_assert_false(lua_State *L)
{
	lutl_checkcutl(L, 1);
	luaL_checkany(L, 2);

	if (!lua_isstring(L, 3)) {
		lua_pushfstring(
			L, "'%s' is not false.", luaL_tolstring(L, 2, NULL)
		);
		lua_insert(L, 3);
		lua_pop(L, 1); // pop result of luaL_tolstring()
	}

	lua_pushboolean(L, !lua_toboolean(L, 2));
	lua_replace(L, 2);

	return lutl_assert(L);
}


int lutl_assert_equal(lua_State *L)
{
	lutl_checkcutl(L, 1);
	luaL_checkany(L, 2);
	luaL_checkany(L, 3);

	if (!lua_isstring(L, 4)) {
		lua_pushfstring(
			L, "'%s' and '%s' are not equal.",
			luaL_tolstring(L, 2, NULL), luaL_tolstring(L, 3, NULL)
		);
		lua_insert(L, 4);
		lua_pop(L, 2); // pop result of luaL_tolstring()
	}

	lua_pushboolean(L, lua_compare(L, 2, 3, LUA_OPEQ));
	lua_replace(L, 2);
	lua_remove(L, 3);

	return lutl_assert(L);
}


int lutl_assert_nil(lua_State *L)
{
	lutl_checkcutl(L, 1);
	luaL_checkany(L, 2);

	if (!lua_isstring(L, 3)) {
		lua_pushfstring(
			L, "'%s' is not nil.", luaL_tolstring(L, 2, NULL)
		);
		lua_insert(L, 3);
		lua_pop(L, 1); // pop result of luaL_tolstring()
	}

	lua_pushboolean(L, lua_isnil(L, 2));
	lua_replace(L, 2);

	return lutl_assert(L);
}


int lutl_assert_notnil(lua_State *L)
{
	lutl_checkcutl(L, 1);
	luaL_checkany(L, 2);

	if (!lua_isstring(L, 3)) {
		lua_pushfstring(
			L, "'%s' is nil.", luaL_tolstring(L, 2, NULL)
		);
		lua_insert(L, 3);
		lua_pop(L, 1); // pop result of luaL_tolstring()
	}

	lua_pushboolean(L, !lua_isnil(L, 2));
	lua_replace(L, 2);

	return lutl_assert(L);
}


int lutl_check(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	luaL_checkany(L, 2);
	const char *msg = luaL_checkstring(L, 3);
	int level = luaL_optinteger(L, 4, 1);

	lua_Debug ar;
	const char *file;
	int line;
	lutl_get_location(L, &ar, level, &file, &line);

	cutl_check_at(cutl, lua_toboolean(L, 2), file, line, "%s", msg);

	return 0;
}


int lutl_get_name(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	lua_pushstring(L, cutl_get_name(cutl));
	return 1;
}


int lutl_get_id(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	lua_pushinteger(L, cutl_get_id(cutl));
	return 1;
}


int lutl_get_depth(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	lua_pushinteger(L, cutl_get_depth(cutl));
	return 1;
}


int lutl_get_data(lua_State *L)
{
	Lutl *lutl = lutl_checklutl(L, 1);
	int len = lutl_unpack(L, lutl->test);
	return len > 0 ? len - 1 : 0;
}


int lutl_set_data(lua_State *L)
{
	Lutl *lutl = lutl_checklutl(L, 1);
	int len = lutl_unpack(L, lutl->test);
	lua_pop(L, len - 1); // Pop old data
	lua_insert(L, 2);

	luaL_unref(L, LUA_REGISTRYINDEX, lutl->test);
	lutl_pack(L, lua_gettop(L) - 1);
	lutl->test = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}


int lutl_get_children(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	lua_pushinteger(L, cutl_get_children(cutl));
	return 1;
}


int lutl_get_passed(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	lua_pushinteger(L, cutl_get_passed(cutl));
	return 1;
}


int lutl_get_failed(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	lua_pushinteger(L, cutl_get_failed(cutl));
	return 1;
}


int lutl_get_error(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	lua_pushboolean(L, cutl_get_error(cutl));
	return 1;
}


int lutl_summary(lua_State *L)
{
	Cutl *cutl = lutl_checkcutl(L, 1);
	lua_pushinteger(L, cutl_summary(cutl));
	return 1;
}


int lutl_is_standalone(lua_State *L)
{
	luaL_requiref(L, "lutl", luaopen_lutl, 0);
	Lutl *lutl = lutl_tolutl(L, -1);
	lua_pushboolean(L, cutl_get_id(lutl->cutl) == 0);
	return 1;
}



static const struct luaL_Reg functions_array[] = {
	{"__gc", lutl_gc},

	{"new", lutl_new},
	{"set_output", lutl_set_output},
	{"get_output", lutl_get_output},
	{"set_verbosity", lutl_set_verbosity},
	{"get_verbosity", lutl_get_verbosity},
	{"set_color", lutl_set_color},
	{"get_color", lutl_get_color},
	{"set_indent", lutl_set_indent},
	{"parse_args", lutl_parse_args},

	{"message", lutl_message},
	{"info", lutl_info},
	{"warn", lutl_warn},

	{"at_start", lutl_at_start},
	{"at_end", lutl_at_end},
	{"at_interrupt", lutl_at_interrupt},
	{"run", lutl_run},
	{"test", lutl_test},
	{"suite", lutl_suite},
	{"interrupt", lutl_interrupt},
	{"fail", lutl_fail},
	{"error", lutl_error},

	{"assert", lutl_assert},
	{"assert_true", lutl_assert_true},
	{"assert_false", lutl_assert_false},
	{"assert_equal", lutl_assert_equal},
	{"assert_nil", lutl_assert_nil},
	{"assert_notnil", lutl_assert_notnil},
	{"check", lutl_check},

	{"summary", lutl_summary},

	{"get_name", lutl_get_name},
	{"get_id", lutl_get_id},
	{"get_depth", lutl_get_depth},
	{"get_data", lutl_get_data},
	{"set_data", lutl_set_data},
	{"get_children", lutl_get_children},
	{"get_passed", lutl_get_passed},
	{"get_failed", lutl_get_failed},
	{"get_error", lutl_get_error},

	{"is_standalone", lutl_is_standalone},

	{NULL, NULL},
};


typedef struct {
	const char *name;
	enum {
		LUTL_CONSTANT_INT,
		LUTL_CONSTANT_STR,
		LUTL_CONSTANT_BOOL,
	} type;
	union {
		int integer;
		const char *string;
		bool boolean;
	} val;
} Lutl_Constant;

static const Lutl_Constant constants_array[] = {
	{"VERSION", LUTL_CONSTANT_STR, .val.string = CUTL_VERSION},
	{"VERSION_MAJOR", LUTL_CONSTANT_INT, .val.integer = CUTL_VERSION_MAJOR},
	{"VERSION_MINOR", LUTL_CONSTANT_INT, .val.integer = CUTL_VERSION_MINOR},
	{"VERSION_PATCH", LUTL_CONSTANT_INT, .val.integer = CUTL_VERSION_PATCH},

	{
		"AUTO_COLOR_ENABLED", LUTL_CONSTANT_BOOL,
#ifdef CUTL_AUTO_COLOR_ENABLED
		.val.boolean = true,
#endif
	},

	{"ERROR", LUTL_CONSTANT_INT, .val.integer = CUTL_ERROR},
	{"FAIL", LUTL_CONSTANT_INT, .val.integer = CUTL_FAIL},
	{"WARN", LUTL_CONSTANT_INT, .val.integer = CUTL_WARN},
	{"INFO", LUTL_CONSTANT_INT, .val.integer = CUTL_INFO},
	{"SUMMARY", LUTL_CONSTANT_INT, .val.integer = CUTL_SUMMARY},
	{"TESTS", LUTL_CONSTANT_INT, .val.integer = CUTL_TESTS},
	{"SUITES", LUTL_CONSTANT_INT, .val.integer = CUTL_SUITES},
	{"SILENT", LUTL_CONSTANT_INT, .val.integer = CUTL_SILENT},
	{"MINIMAL", LUTL_CONSTANT_INT, .val.integer = CUTL_MINIMAL},
	{"NORMAL", LUTL_CONSTANT_INT, .val.integer = CUTL_NORMAL},
	{"VERBOSE", LUTL_CONSTANT_INT, .val.integer = CUTL_VERBOSE},

	{NULL},
};


static void lutl_setconsts(lua_State *L, const Lutl_Constant *consts)
{
	while (consts->name != NULL) {
		switch (consts->type) {
		case LUTL_CONSTANT_INT:
			lua_pushinteger(L, consts->val.integer); break;
		case LUTL_CONSTANT_STR:
			lua_pushstring(L, consts->val.string); break;
		case LUTL_CONSTANT_BOOL:
			lua_pushboolean(L, consts->val.boolean); break;
		}
		lua_setfield(L, -2, consts->name);
		consts++;
	}
}


int luaopen_lutl(lua_State *L)
{
	// Metatable
	luaL_newmetatable(L, "Lutl");
	luaL_setfuncs(L, functions_array, 0);
	lutl_setconsts(L, constants_array);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1); // pop metatable

	// Reftable
	luaL_getsubtable(L, LUA_REGISTRYINDEX, "cutl");
	lua_newtable(L);
	lua_pushliteral(L, "v");
	lua_setfield(L, -2, "__mode");
	lua_setmetatable(L, -2);
	lua_pop(L, 1); // pop reftable

	// Main context
	lutl_register(L, cutl_new(NULL));
	return 1;
}

