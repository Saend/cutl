require('tests/tests')

local T = {}



-- MY PASS TEST

local function My_pass_test(lutl)
	lutl:assert_true(true)
end

local function My_pass_subsuite(lutl)
	lutl:run('test1', My_pass_test)
	lutl:run('test2', My_pass_test)
end

local function My_pass_suite(lutl)
	lutl:run('subsuite1', My_pass_subsuite)
	lutl:run('subsuite2', My_pass_subsuite)
end



-- Passed test summary.
function T.success_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SUMMARY)
	fix.lutl:run('suite', My_pass_suite)

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = "Unit tests summary: 0 failed, 4 passed.\n"
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 0)
end


-- Silent passed test summary.
function T.success_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SILENT)
	fix.lutl:run('suite', My_pass_suite)

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = ""
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 0)
end


-- Passed top-level test summary.
function T.success_toplevel_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SUMMARY)

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = "Unit tests summary: 0 failed, 0 passed.\n"
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 0)
end



-- MY FAIL TEST

local function My_fail_test(lutl)
	lutl:message('fail', 'My failure')
end

local function My_fail_subsuite(lutl)
	lutl:run('test1', My_fail_test)
	lutl:run('test2', My_pass_test)
end

local function My_fail_suite(lutl)
	lutl:run('subsuite1', My_fail_subsuite)
	lutl:run('subsuite2', My_fail_subsuite)
end



-- Failed test summary.
function T.failure_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SUMMARY)
	fix.lutl:run('suite', My_fail_suite)

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = "Unit tests summary: 2 failed, 2 passed.\n"
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 2)
end


-- Silent failed test summary.
function T.failure_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SILENT)
	fix.lutl:run('suite', My_fail_suite)

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = ""
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 2)
end


-- Failed top-level test summary.
function T.failure_toplevel_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SUMMARY)
	fix.lutl:message('fail', "Failure")

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = "Unit tests summary: 1 failed, 0 passed.\n"
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 1)
end



-- MY ERROR TEST

local function My_error_test(lutl)
	lutl:message('error', 'My error')
end

local function My_error_subsuite(lutl)
	lutl:run('test1', My_fail_test)
	lutl:run('test2', My_pass_test)
	lutl:run('test3', My_error_test)
end

local function My_error_suite(lutl)
	lutl:run('subsuite1', My_error_subsuite)
	lutl:run('subsuite2', My_error_subsuite)
end



-- Aborted test summary.
function T.canceled_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SUMMARY)
	fix.lutl:run('suite', My_error_suite)

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = "Unit tests summary: canceled.\n"
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 2)
end


-- Silent canceled test summary.
function T.canceled_silent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SILENT)
	fix.lutl:run('suite', My_error_suite)

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = ""
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 2)
end


-- Aborted top-level test summary.
function T.canceled_toplevel_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SUMMARY)
	fix.lutl:message('error', "Error")

	-- Function under test
	local failed = fix.lutl:summary()

	-- Asserts
	local expected = "Unit tests summary: canceled.\n"
	lutl:assert_content(fix.output, expected)

	lutl:assert_equal(failed, fix.lutl:get_failed())
	lutl:assert_equal(failed, 1)
end



-- SUMMARY SUITE
return function(lutl)
	lutl:at_start(fixture_setup)
	lutl:at_end(fixture_clean)

	lutl:test(T, 'success_test')
	lutl:test(T, 'success_silent_test')
	lutl:test(T, 'success_toplevel_test')

	lutl:test(T, 'failure_test')
	lutl:test(T, 'failure_silent_test')
	lutl:test(T, 'failure_toplevel_test')

	lutl:test(T, 'canceled_test')
	lutl:test(T, 'canceled_silent_test')
	lutl:test(T, 'canceled_toplevel_test')
end
