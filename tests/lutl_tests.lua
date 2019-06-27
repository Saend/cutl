#! /usr/bin/env lua
require('tests/tests')



-- MY TEST FUNCTIONS

function My_outofscope(lutl)
	local data = {}
	lutl:run('test', function(lutl) data.lutl = lutl end)
	data.lutl:assert_true() -- Bad test context.
end

function My_syntax(lutl)
	lutl:ruin() -- Undefined function.
end

function My_error(lutl)
	lutl:error('Error')
end



-- ERRORS

-- Out of scope context.
function outofscope_test(lutl, fix)
	-- Function under test
	fix.lutl:run('test', My_outofscope)

	-- Asserts
	expected =
		'test:\n'..
		'	[ERROR tests/lutl_tests.lua:11] Bad test context: '..
		'out of scope.\n'..
		'test canceled.\n'
	lutl:assert_content(fix.output, expected)
	lutl:assert_true(fix.lutl:get_error())
end


-- Syntax error
function syntax_test(lutl, fix)
	-- Function under test
	fix.lutl:run('test', My_syntax)

	-- Asserts
	expected =
		'test:\n'..
		'	[ERROR tests/lutl_tests.lua:15] Lua: attempt to call '..
		'a nil value (method \'ruin\').\n'..
		'test canceled.\n'
	lutl:assert_content(fix.output, expected)
	lutl:assert_true(fix.lutl:get_error())
end


-- Regular error
function error_test(lutl, fix)
	-- Function under test
	fix.lutl:run('test', My_error)

	-- Asserts
	expected =
		'test:\n'..
		'	[ERROR tests/lutl_tests.lua:19] Error\n'..
		'test canceled.\n'
	lutl:assert_content(fix.output, expected)
	lutl:assert_true(fix.lutl:get_error())
end



-- MISCELLANEOUS SUITE

function lutl_misc_suite(lutl)
	lutl:at_start(fixture_setup)
	lutl:at_end(fixture_clean)

	lutl:test('outofscope_test')
	lutl:test('error_test')
	lutl:test('syntax_test')
end



-- LUTL SUITE

lutl_parse_args_suite = require('tests/lutl_parse_args_tests')
lutl_run_suite = require('tests/lutl_run_tests')
lutl_message_suite = require('tests/lutl_message_tests')
lutl_assert_suite = require('tests/lutl_assert_tests')
lutl_summary_suite = require('tests/lutl_summary_tests')
lutl_get_suite = require('tests/lutl_get_tests')


lutl:assert_equal(lutl.VERSION_MAJOR, 2)

lutl:suite('lutl_parse_args_suite')
lutl:suite('lutl_run_suite')
lutl:suite('lutl_message_suite')
lutl:suite('lutl_assert_suite')
lutl:suite('lutl_summary_suite')
lutl:suite('lutl_get_suite')
lutl:suite('lutl_misc_suite')



if lutl:is_standalone() then os.exit(lutl:summary()) end
