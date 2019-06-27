require('tests/tests')

local T = {}



-- MESSAGE TYPE: ERROR

-- Display error message.
function T.error_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.ERROR)

	-- Function under test
	fix.lutl:message('error', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "[ERROR] Message\n")
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())
end


-- Disabled message type
function T.error_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.VERBOSE ~ lutl.ERROR)

	-- Function under test
	fix.lutl:message('error', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "")
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())
end



-- MESSAGE TYPE: FAIL

-- Display failure message.
function T.fail_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.FAIL)

	-- Function under test
	fix.lutl:message('fail', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "[FAIL] Message\n")
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())
end


-- Disabled message type.
function T.fail_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.VERBOSE ~ lutl.FAIL)

	-- Function under test
	fix.lutl:message('fail', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "")
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())
end



-- MESSAGE TYPE: WARN

-- Display warning message.
function T.warn_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.WARN)

	-- Function under test
	fix.lutl:message('warn', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "[WARN] Message\n")
	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end


-- Disabled message type.
function T.warn_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.VERBOSE ~ lutl.WARN)

	-- Function under test
	fix.lutl:message('warn', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "")
	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end



-- MESSAGE TYPE: INFO

-- Display infoing message.
function T.info_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.INFO)

	-- Function under test
	fix.lutl:message('info', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "[INFO] Message\n")
	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end


-- Disabled message type.
function T.info_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.VERBOSE ~ lutl.INFO)

	-- Function under test
	fix.lutl:message('info', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "")
	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end



-- MESSAGE TYPE: SUMMARY

-- Display summary.
function T.summary_test(lutl, fix)
	--Setup
	fix.lutl:set_verbosity(lutl.SUMMARY)

	-- Function under test
	fix.lutl:message('summary', "Message", 0)

	-- Asserts
	lutl:assert_content(fix.output, "Message\n")
	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end



-- MESSAGE NESTING

-- MY ERROR TEST

local function My_error(lutl)
	lutl:message('error', "My error", 0)
	lutl:message('fail', "My failure")
end

local function My_error_subsuite(lutl)
	lutl:run('test', My_error)
end

local function My_error_suite(lutl)
	lutl:run('subsuite', My_error_subsuite)
end


-- Nested erroneous fix.
function T.nested_error_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.ERROR | lutl.SUITES)

	-- Function under test
	fix.lutl:run('suite', My_error_suite)

	-- Asserts
	expected =
		'suite:\n'..
		'	subsuite:\n'..
		'		test:\n'..
		'			[ERROR] My error\n'..
		'		test canceled.\n'..
		'	subsuite canceled.\n'..
		'suite canceled.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())
end


-- Flattened erroneous fix.
function T.flattened_error_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.ERROR)

	-- Function under test
	fix.lutl:run('suite', My_error_suite)

	-- Asserts
	expected =
		'test:\n'..
		'	[ERROR] My error\n'..
		'test canceled.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())
end



-- MY FAIL TEST

local function My_fail(lutl)
	lutl:message('fail', "My failure", 0)
	lutl:message('info', "My message")
end

local function My_fail_subsuite(lutl)
	lutl:run('test', My_fail)
end

local function My_fail_suite(lutl)
	lutl:run('subsuite', My_fail_subsuite)
end


-- Nested failed fix.
function T.nested_fail_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.FAIL | lutl.SUITES)

	-- Function under test
	fix.lutl:run('suite', My_fail_suite)

	-- Asserts
	expected =
		'suite:\n'..
		'	subsuite:\n'..
		'		test:\n'..
		'			[FAIL] My failure\n'..
		'		test failed.\n'..
		'	subsuite failed.\n'..
		'suite failed.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())
end


-- Flattened failed fix.
function T.flattened_fail_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.FAIL)

	-- Function under test
	fix.lutl:run('suite', My_fail_suite)

	-- Asserts
	expected =
		'test:\n'..
		'	[FAIL] My failure\n'..
		'test failed.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())
end



-- MY INFO TEST

local function My_info(lutl)
	lutl:message('info', "My message", 0)
	lutl:message('warn', "My warning")
end

local function My_info_subsuite(lutl)
	lutl:run('test', My_info)
end

local function My_info_suite(lutl)
	lutl:run('subsuite', My_info_subsuite)
end



-- Nested info fix.
function T.nested_info_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.INFO | lutl.SUITES)

	-- Function under test
	fix.lutl:run('suite', My_info_suite)

	-- Asserts
	expected =
		'suite:\n'..
		'	subsuite:\n'..
		'		test:\n'..
		'			[INFO] My message\n'..
		'		test passed.\n'..
		'	subsuite passed.\n'..
		'suite passed.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end


-- Flattened info fix.
function T.flattened_info_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.INFO)

	-- Function under test
	fix.lutl:run('suite', My_info_suite)

	-- Asserts
	expected =
		'test:\n'..
		'	[INFO] My message\n'..
		'test passed.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end


-- MY SILENT TEST

local function My_silent(lutl)
	lutl:assert(true, "Never fails.")
end

local function My_silent_subsuite(lutl)
	lutl:run('test', My_silent)
end

local function My_silent_suite(lutl)
	lutl:run('subsuite', My_silent_subsuite)
end



-- Nested silent fix.
function T.nested_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.TESTS | lutl.SUITES)

	-- Function under test
	fix.lutl:run('suite', My_silent_suite)

	-- Asserts
	expected =
		'suite:\n'..
		'	subsuite:\n'..
		'		test passed.\n'..
		'	subsuite passed.\n'..
		'suite passed.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end


-- Semi-nested silent fix.
function T.seminested_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SUITES)

	-- Function under test
	fix.lutl:run('suite', My_silent_suite)

	-- Asserts
	expected =
		'suite:\n'..
		'	subsuite passed.\n'..
		'suite passed.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end


-- Flattened silent fix.
function T.flattened_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.TESTS)

	-- Function under test
	fix.lutl:run('suite', My_silent_suite)

	-- Asserts
	expected = 'test passed.\n'
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())
end





-- MESSAGE SUITE

return function(lutl)
	lutl:at_start(fixture_setup)
	lutl:at_end(fixture_clean)

	lutl:test(T, 'error_test')
	lutl:test(T, 'error_silent_test')
	lutl:test(T, 'fail_test')
	lutl:test(T, 'fail_silent_test')
	lutl:test(T, 'warn_test')
	lutl:test(T, 'warn_silent_test')
	lutl:test(T, 'info_test')
	lutl:test(T, 'info_silent_test')
	lutl:test(T, 'summary_test')

	lutl:test(T, 'nested_error_test')
	lutl:test(T, 'flattened_error_test')
	lutl:test(T, 'nested_fail_test')
	lutl:test(T, 'flattened_fail_test')
	lutl:test(T, 'nested_info_test')
	lutl:test(T, 'flattened_info_test')
	lutl:test(T, 'nested_silent_test')
	lutl:test(T, 'seminested_silent_test')
	lutl:test(T, 'flattened_silent_test')
end
