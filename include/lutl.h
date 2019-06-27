#pragma once
/** \file lutl.h
 * \brief lutl: Lua unit testing library.
 * \author Baptiste "Saend" CEILLIER
 * \copyright See LICENSE file.
 *
 * The Lua API behaves similarity to the C API. Each test receives its own test
 * context as first parameter, in the form of a `Lutl` full userdata, the
 * main test context being returned by `require`. The module defines a `Lutl`
 * metatable that contains the Lua functions listed below, most of which mirror
 * their C counterpart.
 *
 * Of the functions declared here, only lutl_dofile() and lutl_dostring()
 * should commonly be used. The rest is declared mainly for documentation
 * purposes, but could be used to extend the module.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.lua}
 * #! /usr/bin/env lua
 * -- This simple example can be run as ./simple.lua or with lutl_dofile().
 * lutl = require('lutl')
 *
 * function simple_test(lutl)
 * 	lutl:assert_true(34 == 45)
 * end
 *
 * lutl:test('simple_test')
 * if lutl:is_standalone() then os.exit(lutl:summary()) end
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <cutl.h>

#include <lua.h>



/// \name Loading Lua

/** Loads and run the given Lua file.
 * The `name` parameter is used to create a new test into which the the file is
 * being run.
 *
 * Returns the number of failed tests by calling cutl_failed().
 */
CUTL_API int lutl_dofile(Cutl *cutl, const char *name, const char *filename);


/** Loads and run the given Lua string.
 * The `name` parameter is used to create a new test into which the the string
 * is being run.
 *
 * The lutl module is globally loaded beforehand, meaning the line
 * `lutl = require("lutl")` can be omitted.
 *
 * Returns the number of failed tests by calling cutl_failed().
 */
CUTL_API int lutl_dostring(Cutl *cutl, const char *name, const char *string);


/** Loads the Lua module.
 * Used by require() in Lua and luaL_requiref() in C.
 */
CUTL_API int luaopen_lutl(lua_State *L);



/// \name Lua Functions


/** Binds cutl_new() as `new([name])`.
 * Returns a new `Lutl` userdata.
 */
CUTL_API int lutl_new(lua_State *L);


/** Binds cutl_set_output() as `set_output(lutl, file)`.
 * The `file` parameter is a LUA_FILEHANDLE userdata.
 */
CUTL_API int lutl_set_output(lua_State *L);


/** Binds cutl_get_output() as `get_output(lutl)`.
 * Returns a LUA_FILEHANDLE userdata.
 */
CUTL_API int lutl_get_output(lua_State *L);


/** Binds cutl_set_verbosity() as `set_verbosity(lutl, verb)`.
 * The verb parameter can be a bitwise-OR composition of verbosity object
 * values, or a pre-defined verbosity level.
 */
CUTL_API int lutl_set_verbosity(lua_State *L);


/** Binds cutl_get_verbosity() as `get_verbosity(lutl)`.
 */
CUTL_API int lutl_get_verbosity(lua_State *L);


/** Binds cutl_set_color() as `set_color(lutl[, color])`.
 * The optional `color` parameter is a boolean. If it is nil or none then color
 * output is automatically selected (equivalent to calling the native function
 * with a negative value).
 */
CUTL_API int lutl_set_color(lua_State *L);


/** Binds cutl_get_color() as `get_color(lutl)`.
 */
CUTL_API int lutl_get_color(lua_State *L);


/** Binds cutl_set_indent() as `set_indent(lutl, indent)`.
 * The `indent` parameter is a string.
 */
CUTL_API int lutl_set_indent(lua_State *L);


/** Binds cutl_parse_args() as `parse_args(lutl, args)`.
 * The `args` parameter is a table containing the arguments in sequence,
 * starting at index 1. If index 0 exists, then its value is used as the name
 * of the source, otherwise it is automatically generated.
 */
CUTL_API int lutl_parse_args(lua_State *L);


/** Binds cutl_message_at() as `message(lutl, type, msg [, level])`.
 * The `type` parameter is a string: either 'error', 'fail', 'warn' or 'info'
 *
 * The `msg` parameter is a string.
 *
 * The optional`level` parameter is an integer defining the location (file and
 * line) of the message. If it is 0, then no location is printed. If it is 1,
 * then the location of the call to this function is printed. If it is 2, then
 * the location of the call to the function containing the call to this
 * function is printed, and so forth.
 */
CUTL_API int lutl_message(lua_State *L);


/** Defines `info(lutl, ...)` as `message(lutl, 'info', ...)`.
 */
CUTL_API int lutl_info(lua_State *L);


/** Defines `warn(lutl, ...)` as `message(lutl, 'warn', ...)`.
 */
CUTL_API int lutl_warn(lua_State *L);


/** Binds cutl_at_start() as `at_start(lutl, func, ...)`.
 * The extra parameters are passed to `func`.
 */
CUTL_API int lutl_at_start(lua_State *L);


/** Binds cutl_at_end() as `at_end(lutl, func, ...)`.
 * The extra parameters are passed to `func`.
 */
CUTL_API int lutl_at_end(lua_State *L);


/** Binds cutl_at_interrupt() as `at_interrupt(lutl, func, ...)`.
 * The extra parameters are passed to `func`.
 */
CUTL_API int lutl_at_interrupt(lua_State *L);


/** Binds cutl_run() as `run(lutl, name, func, ...)`.
 * The `name` parameter is a string or nil.
 *
 * The extra parameters are passed to `func`.
 *
 * Returns the number of failed tests.
 */
CUTL_API int lutl_run(lua_State *L);


/** Defines `test(lutl, [env, ] name, ...)` as `run(lutl, name, func, ...)`
 * The optional `env` parameter is a table containing the test function at the
 * index `name`. The default value is `_G`, the global environment.
 */
CUTL_API int lutl_test(lua_State *L);


/** Defines `suite(...)` as `test(...)`
 */
CUTL_API int lutl_suite(lua_State *L);


/** Binds cutl_interrupt() as `interrupt(lutl)`.
 */
CUTL_API int lutl_interrupt(lua_State *L);


/** Binds cutl_fail() as `fail(lutl, msg [, level])`.
 * The `level` parameter is the same as for lutl_message().
 */
CUTL_API int lutl_fail(lua_State *L);


/** Binds cutl_error() as `error(lutl, msg [, level])`.
 * The `level` parameter is the same as for lutl_message().
 */
CUTL_API int lutl_error(lua_State *L);


/** Binds cutl_assert_at() as `assert(lutl, val, msg [, level])`.
 * The `val` parameter is a boolean value following the Lua semantics.
 *
 * The optional `level` parameter is the same as for lutl_message().
 */
CUTL_API int lutl_assert(lua_State *L);


/** Defines `assert_true(lutl, val [, msg, ...])` as
 * `assert(lutl, val, msg, ...)`
 */
CUTL_API int lutl_assert_true(lua_State *L);


/** Defines `assert_false(lutl, val [, msg, ...])` as
 * `assert(lutl, not val, msg, ...)`
 */
CUTL_API int lutl_assert_false(lua_State *L);


/** Defines `assert_equal(lutl, val1, val2 [, msg, ...])` as
 * `assert(lutl, val1 == val2, msg, ...)`
 */
CUTL_API int lutl_assert_equal(lua_State *L);


/** Defines `assert_nil(lutl, val [, msg, ...])` as
 * `assert(lutl, val == nil, msg, ...)`
 */
CUTL_API int lutl_assert_nil(lua_State *L);


/** Defines `assert_notnil(lutl, val [, msg, ...])` as
 * `assert(lutl, val ~= nil, msg, ...)`
 */
CUTL_API int lutl_assert_notnil(lua_State *L);


/** Binds cutl_check() as `check(lutl, val, msg)`.
 */
CUTL_API int lutl_check(lua_State *L);


/** Binds cutl_summary() as `summary(lutl)`.
 */
CUTL_API int lutl_summary(lua_State *L);


/** Binds cutl_get_name() as `get_name(lutl)`.
 */
CUTL_API int lutl_get_name(lua_State *L);


/** Binds cutl_get_id() as `get_id(lutl)`.
 */
CUTL_API int lutl_get_id(lua_State *L);


/** Binds cutl_get_depth() as `get_depth(lutl)`.
 */
CUTL_API int lutl_get_depth(lua_State *L);


/** Binds cutl_get_data() as `get_data(lutl)`.
 */
CUTL_API int lutl_get_data(lua_State *L);


/** Binds cutl_set_data() as `set_data(lutl, ...)`.
 */
CUTL_API int lutl_set_data(lua_State *L);


/** Binds cutl_get_children() as `get_children(lutl)`.
 */
CUTL_API int lutl_get_children(lua_State *L);


/** Binds cutl_get_passed() as `get_passed(lutl)`.
 */
CUTL_API int lutl_get_passed(lua_State *L);


/** Binds cutl_get_failed() as `get_failed(lutl)`.
 */
CUTL_API int lutl_get_failed(lua_State *L);


/** Binds cutl_get_error() as `get_error(lutl)`.
 */
CUTL_API int lutl_get_error(lua_State *L);


/** Defines `is_standalone()`.
 * Returns `true` if the script is being run standalone; `false` if it is being
 * run from lutl_dofile() or lutl_dostring().
 */
CUTL_API int lutl_is_standalone(lua_State *L);



/// \name Userdata

/** Checks whether the function argument `arg` is a `Lutl` userdata and returns
 * the underlying Cutl pointer.
 * This behaves similarly to Lua's `luaL_check*()` functions, and is only
 * useful in conjunction with Lua's C API.
 */
CUTL_API Cutl *lutl_checkcutl(lua_State *L, int arg);

/** Pushes a Cutl pointer as a `Lutl` userdata onto the stack.
 * This behaves similarly to Lua's `lua_push*()` functions, and is only
 * useful in conjunction with Lua's C API.
 */
CUTL_API void lutl_pushcutl(lua_State *L, Cutl *cutl);

