require('tests/tests')

local T = {}



-- CHILDREN, PASSED AND FAILED GETTERS

-- MY TEST FUNCTIONS

local function My_pass_test(lutl)
	lutl:assert_true(true)
end

local function My_fail_test(lutl)
	lutl:assert_true(false)
end

local function My_mixed_subsuite(lutl)
	lutl:run('test1', My_pass_test)
	lutl:run('test2', My_fail_test)
	lutl:run('test3', My_pass_test)
end

local function My_mixed_suite(lutl)
	lutl:run('subsuite', My_mixed_subsuite)
	lutl:run(nil, My_mixed_subsuite)
end



-- Mixed suite.
function T.count_mixed_test(lutl, fix)
	-- Setup
	fix.lutl:run('suite', My_mixed_suite)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_children(), 6)
	lutl:assert_equal(fix.lutl:get_passed(), 4)
	lutl:assert_equal(fix.lutl:get_failed(), 2)
end


-- Passing test.
function T.count_pass_test(lutl, fix)
	-- Setup
	fix.lutl:run('suite', My_pass_test)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_children(), 1)
	lutl:assert_equal(fix.lutl:get_passed(), 1)
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end


-- Top-level passing test.
function T.count_toplevel_pass_test(lutl, fix)
	-- Setup

	-- Asserts
	lutl:assert_equal(fix.lutl:get_children(), 0)
	lutl:assert_equal(fix.lutl:get_passed(), 0)
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end


-- Anonymous passing test.
function T.count_anonymous_pass_test(lutl, fix)
	-- Setup
	fix.lutl:run(nil, My_pass_test)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_children(), 0)
	lutl:assert_equal(fix.lutl:get_passed(), 0)
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end


-- Failing test.
function T.count_fail_test(lutl, fix)
	-- Setup
	fix.lutl:run('suite', My_fail_test)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_children(), 1)
	lutl:assert_equal(fix.lutl:get_passed(), 0)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end


-- Top-level failing test.
function T.count_toplevel_fail_test(lutl, fix)
	-- Setup
	fix.lutl:message('fail', "My failure")

	-- Asserts
	lutl:assert_equal(fix.lutl:get_children(), 0)
	lutl:assert_equal(fix.lutl:get_passed(), 0)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end


-- Anonymous failing test.
function T.count_anonymous_fail_test(lutl, fix)
	-- Setup
	fix.lutl:run(nil, My_fail_test)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_children(), 0)
	lutl:assert_equal(fix.lutl:get_passed(), 0)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end



-- DEPTH AND ID GETTERS

-- MY TEST SUITE

local function My_info_test(lutl, info)
	info.depth = lutl:get_depth()
	info.id = lutl:get_id()
end

local function My_info_subsuite(lutl, info)
	lutl:run('test', My_info_test, info)
end

local function My_info_suite(lutl, info)
	lutl:run('subsuite1', My_info_subsuite, info)
	lutl:run('subsuite2', My_info_subsuite, info)
end



-- Suite depth.
function T.info_suite_test(lutl, fix)
	-- Setup
	local info = {}
	fix.lutl:run('suite', My_info_suite, info)

	-- Asserts
	lutl:assert_equal(info.depth, 3)
	lutl:assert_equal(info.id, 5)
end


-- Test depth.
function T.info_test_test(lutl, fix)
	-- Setup
	local info = {}
	fix.lutl:run('test', My_info_test, info)

	-- Asserts
	lutl:assert_equal(info.depth, 1)
	lutl:assert_equal(info.id, 1)
end


-- Anonymous test depth.
function T.info_anon_test(lutl, fix)
	-- Setup
	local info = {}
	fix.lutl:run(nil, My_info_test, info)

	-- Asserts
	lutl:assert_equal(info.depth, 0)
	lutl:assert_equal(info.id, 1)
end


-- Top-level depth.
function T.info_toplevel_test(lutl, fix)
	-- Setup

	-- Asserts
	lutl:assert_equal(fix.lutl:get_depth(), 0)
	lutl:assert_equal(fix.lutl:get_id(), 0)
end



-- GET SUITE

return function(lutl)
	lutl:at_start(fixture_setup)
	lutl:at_end(fixture_clean)

	lutl:test(T, 'count_mixed_test')
	lutl:test(T, 'count_pass_test')
	lutl:test(T, 'count_toplevel_pass_test')
	lutl:test(T, 'count_anonymous_pass_test')
	lutl:test(T, 'count_fail_test')
	lutl:test(T, 'count_toplevel_fail_test')
	lutl:test(T, 'count_anonymous_fail_test')

	lutl:test(T, 'info_suite_test')
	lutl:test(T, 'info_test_test')
	lutl:test(T, 'info_anon_test')
	lutl:test(T, 'info_toplevel_test')
end
