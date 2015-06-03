/* CUTL - A simple C unit testing library. 
 * AUTHOR: Baptiste "Saend" CEILLIER
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
#include "lutl.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define LUA_USE_APICHECK

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


static cutl_t *lutl_getcutl(lua_State *L) {
	lua_getglobal(L, "lutl");
	assert(lua_istable(L, -1));
	
	lua_pushstring(L, "cutl");
	lua_gettable(L, -2);
	
	cutl_t *cutl = NULL;
	if (!lua_isnil(L, -1)) {	
		assert(lua_islightuserdata(L, -1));
		cutl = lua_touserdata(L, -1);
	}
	
	lua_pop(L, 2);
	return cutl;
}

static void lutl_setcutl(lua_State *L, cutl_t *cutl) {
	lua_getglobal(L, "lutl");
	assert(lua_istable(L, -1));
	
	lua_pushstring(L, "cutl");
	lua_pushlightuserdata(L, cutl);
	lua_settable(L, -3);
	lua_pop(L, 1);
}

static int lutl_reset(lua_State *L) {
	cutl_t *cutl = lutl_getcutl(L);
	
	if (cutl) {
		cutl_free(cutl);
	}
	
	cutl = cutl_new(NULL);
	lutl_setcutl(L, cutl);
	
	return 0;
}

static int lutl_verbosity(lua_State *L) {
	cutl_t *cutl = lutl_getcutl(L);
	assert(cutl);
	
	luaL_checkinteger(L, 1);
	cutl_verbosity(
		cutl, lua_tointeger(L, 1)
	);
	
	return 0;
}

static int lutl_message(lua_State *L) {
	cutl_t *cutl = lutl_getcutl(L);
	assert(cutl);

	int verbosity = luaL_checkinteger(L, 1);	
	luaL_checkstring(L, 2);
	bool showsrc = lua_toboolean(L, 3);
	
	const char *file = NULL;
	int line = 0;

	if (showsrc || verbosity & CUTL_FAIL) {
		lua_Debug ar;
		lua_getstack(L, 1, &ar);
		lua_getinfo(L, "Sl", &ar);

		file = ar.short_src;
		line = ar.currentline;
	}
	
	cutl_message_at(cutl, verbosity, lua_tostring(L, 2), file, line);
	
	return 0;
}

static int lutl_info(lua_State *L) {
	luaL_checkstring(L, 1);
	lua_pushinteger(L, CUTL_INFO);
	lua_insert(L, 1);
	return lutl_message(L);
}

static int lutl_warn(lua_State *L) {
	luaL_checkstring(L, 1);
	lua_pushinteger(L, CUTL_WARN);
	lua_insert(L, 1);
	return lutl_message(L);
}

static int lutl_fail(lua_State *L) {
	cutl_t *cutl = lutl_getcutl(L);
	assert(cutl);
	
	luaL_checkstring(L, 1);	
	luaL_error(L, lua_tostring(L, 1));	
	return 0;
}

static int lutl_assert(lua_State *L) {
	cutl_t *cutl = lutl_getcutl(L);
	assert(cutl);

	luaL_checkany(L, 1);
	bool val = lua_toboolean(L, 1);
	
	if (!val) {
		if (lua_isstring(L, 2)) {
			lua_pushstring(L, lua_tostring(L, 2));
		} else {
			lua_pushstring(L, "lutl.assert(");
			luaL_tolstring(L, 1, NULL);
			lua_pushstring(L, ")");
			lua_concat(L, 3);
		}
	
		lua_Debug ar;
		lua_getstack(L, 1, &ar);
		lua_getinfo(L, "Sl", &ar);
		
		
		luaL_error(L, lua_tostring(L, -1));	
	}
	
	return 0;
}

static void lutl_interface(cutl_t *cutl, void *data) {
	lua_State *L = (lua_State*) data;	
	lutl_setcutl(L, cutl);
	if (lua_pcall(L, lua_gettop(L)-1, 0, 0)) {
		cutl_fail_at(cutl, lua_tostring(L, -1), NULL, 0);
	}
}

static int lutl_run(lua_State *L) {
	cutl_t *cutl = lutl_getcutl(L);
	assert(cutl);
	
	luaL_checkstring(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	int nb_param = lua_gettop(L) - 2;

	lua_pushvalue(L, 1);
	lua_remove(L, 1);
	if (nb_param > 0) {
		lua_pushstring(L, "(");
		for (int i=0; i<nb_param; ++i) {
			luaL_tolstring(L, 2+i, NULL);
			if (i<nb_param-1) {
				lua_pushstring(L, ", ");
			}
		}
		lua_pushstring(L, ")");
		lua_concat(L, 2*nb_param+2);
	}
	const char *name = lua_tostring(L, -1);
	lua_pop(L, 1);
	
	cutl_run(cutl, name, lutl_interface, L);
	lutl_setcutl(L, cutl);
	return 0;
}

static int lutl_test(lua_State *L) {
	luaL_checkstring(L, 1);
	
	lua_getglobal(L, lua_tostring(L, 1));
	lua_insert(L, 2);
	luaL_argcheck(L, !lua_isnil(L, 2), 2, "function not found");
	
	return lutl_run(L);
}

static int lutl_summary(lua_State *L) {
	cutl_t *cutl = lutl_getcutl(L);
	assert(cutl);
	
	cutl_summary(cutl);
	
	return 0;
}

static const struct luaL_Reg functions_array[] = {
	{"reset", 	lutl_reset},
	{"verbosity", 	lutl_verbosity},
	{"message", 	lutl_message},
	{"info", 	lutl_info},
	{"warn", 	lutl_warn},
	{"fail", 	lutl_fail},
	{"assert", 	lutl_assert},
	{"run", 	lutl_run},
	{"test", 	lutl_test},
	{"suite", 	lutl_test},
	{"summary", 	lutl_summary},
	{NULL, 		NULL},
};


// LIBRARY INITIALIZATION

typedef struct {
	const char *name; int value;
} lutl_constant_t;

static const lutl_constant_t constants_array[] = {
	{"NOTHING", 	CUTL_NOTHING},
	{"ERROR", 	CUTL_ERROR},
	{"FAIL", 	CUTL_FAIL},
	{"WARN", 	CUTL_WARN},
	{"INFO", 	CUTL_INFO},
	{"SUCCESS", 	CUTL_SUCCESS},
	{"SUITE", 	CUTL_SUITE},
	{"SUMMARY", 	CUTL_SUMMARY},
	{"EVERYTHING", 	CUTL_EVERYTHING},
	{"SILENT", 	CUTL_SILENT},
	{"MINIMAL", 	CUTL_MINIMAL},
	{"NORMAL", 	CUTL_NORMAL},
	{"VERBOSE", 	CUTL_VERBOSE},
	{NULL, 		0},
};

static void lutl_setloaded(lua_State *L, const char *str) {
	lua_getglobal(L, "package");
	lua_pushstring(L, "loaded");
	lua_gettable(L, -2);
	
	lua_pushstring(L, "lutl");
	lua_pushstring(L, str);
	lua_settable(L, -3);
	
	lua_pop(L, 2);
}

int luaopen_lutl(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, functions_array, 0);
	
	const lutl_constant_t *constant = constants_array;
	while (constant->name) {
		lua_pushstring(L, constant->name);
		lua_pushinteger(L, constant->value);
		lua_settable(L, -3);
		constant++;
	}
	
	lua_setglobal(L, "lutl");
	lutl_reset(L);
	
	lutl_setloaded(L, "Lua");
	
	return 0;
}


// TESTING

void lutl_dofile(cutl_t *cutl, const char *filename) {
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_lutl(L);
	lutl_setloaded(L, "C");
	lutl_setcutl(L, cutl);
	
	if (luaL_dofile(L, filename)) {
		cutl_message_at(
			cutl, CUTL_FAIL, lua_tostring(L, -1), NULL, 0
		);
	}
}

