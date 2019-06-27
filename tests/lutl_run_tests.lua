require('tests/tests')

local T = {}



-- MY TEST FUNCTIONS

-- Test states.
local function newstate()
	return {
		['start'] = 'not_executed',
		['test'] = 'not_executed',
		['end'] = 'not_executed',
	}
end

local function My_normal(lutl, state, stage)
	state[stage] = 'finished';
end

local function My_softfail(lutl, state, stage)
	state[stage] = 'executed';
	lutl:message('fail', "Soft Failure")
	state[stage] = 'finished';
end

local function My_hardfail(lutl, state, stage)
	state[stage] = 'executed';
	lutl:fail('fail', "Hard Failure")
	state[stage] = 'finished';
end

local function My_softerror(lutl, state, stage)
	state[stage] = 'executed';
	lutl:message('error', "Soft Error")
	state[stage] = 'finished';
end

local function My_harderror(lutl, state, stage)
	state[stage] = 'executed';
	lutl:error('error', "Hard Error")
	state[stage] = 'finished';
end

local function My_interrupt(lutl, state, stage)
	state[stage] = 'executed';
	lutl:interrupt()
	state[stage] = 'finished';
end



-- ERROR AT: START

-- Display error message inside `at_start`.
function T.start_softerror_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_softerror, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'not_executed')
	lutl:assert_equal(state['end'], 'not_executed')
end



-- Error inside `at_start`.
function T.start_harderror_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_harderror, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'executed')
	lutl:assert_equal(state['test'], 'not_executed')
	lutl:assert_equal(state['end'], 'not_executed')
end



-- FAIL AT: START

-- Display fail message inside `at_start`.
function T.start_softfail_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_softfail, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'not_executed')
	lutl:assert_equal(state['end'], 'not_executed')
end


-- Fail inside `at_start`.
function T.start_hardfail_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_hardfail, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'executed')
	lutl:assert_equal(state['test'], 'not_executed')
	lutl:assert_equal(state['end'], 'not_executed')
end



-- INTERRUPT AT: START

-- Interrupt inside `at_start`.
function T.start_interrupt_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_interrupt, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'executed')
	lutl:assert_equal(state['test'], 'finished')
	lutl:assert_equal(state['end'], 'finished')
end



-- ERROR AT: TEST

-- Display error message inside fix.
function T.test_softerror_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_softerror, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'finished')
	lutl:assert_equal(state['end'], 'finished')
end


-- Error inside fix.
function T.test_harderror_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_harderror, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'executed')
	lutl:assert_equal(state['end'], 'finished')
end


-- FAIL AT: TEST

-- Display fail message inside fix.
function T.test_softfail_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_softfail, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'finished')
	lutl:assert_equal(state['end'], 'finished')
end


-- Fail inside fix.
function T.test_hardfail_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_hardfail, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'executed')
	lutl:assert_equal(state['end'], 'finished')
end



-- INTERRUPT AT: TEST

-- Interrupt inside fix.
function T.test_interrupt_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_normal, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_interrupt, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'executed')
	lutl:assert_equal(state['end'], 'finished')
end



-- ERROR AT: END

-- Display error message inside `at_end`.
function T.end_softerror_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_softerror, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'finished')
	lutl:assert_equal(state['end'], 'finished')
end


-- Error inside `at_end`.
function T.end_harderror_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_harderror, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_true(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'finished')
	lutl:assert_equal(state['end'], 'executed')
end


-- FAIL AT: END

-- Display fail message inside `at_end`.
function T.end_softfail_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_softfail, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'finished')
	lutl:assert_equal(state['end'], 'finished')
end


-- Fail inside `at_end`.
function T.end_hardfail_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_hardfail, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 1)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'finished')
	lutl:assert_equal(state['end'], 'executed')
end



-- INTERRUPT AT: END

-- Interrupt inside `at_end`.
function T.end_interrupt_test(lutl, fix)
	-- Setup
	local state = newstate()
	fix.lutl:at_start(My_normal, state, 'start')
	fix.lutl:at_end(My_interrupt, state, 'end')

	-- Function under test
	local failed = fix.lutl:run('test', My_normal, state, 'test')

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), failed)
	lutl:assert_equal(fix.lutl:get_failed(), 0)
	lutl:assert_false(fix.lutl:get_error())

	lutl:assert_equal(state['start'], 'finished')
	lutl:assert_equal(state['test'], 'finished')
	lutl:assert_equal(state['end'], 'executed')
end



-- ERRONEOUS USES

-- Use parent test.
function T.use_parent_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.ERROR)

	-- Function under test
	fix.lutl:run('parent', function(parent)
		parent:run('child', function(child)
			parent:run(nil, My_normal)
		end)
	end)

	-- Asserts
	expected =
		'child:\n'..
		'	[ERROR cutl_run()] Bad test context: running child.\n'..
		'child canceled.\n'
	lutl:assert_content(fix.output, expected)
	lutl:assert_true(fix.lutl:get_error())
end



-- RUN SUITE

return function(lutl)
	lutl:at_start(fixture_setup)
	lutl:at_end(fixture_clean)

	lutl:test(T, 'start_softerror_test')
	lutl:test(T, 'start_harderror_test')
	lutl:test(T, 'start_softfail_test')
	lutl:test(T, 'start_hardfail_test')
	lutl:test(T, 'start_interrupt_test')

	lutl:test(T, 'test_softerror_test')
	lutl:test(T, 'test_harderror_test')
	lutl:test(T, 'test_softfail_test')
	lutl:test(T, 'test_hardfail_test')
	lutl:test(T, 'test_interrupt_test')

	lutl:test(T, 'end_softerror_test')
	lutl:test(T, 'end_harderror_test')
	lutl:test(T, 'end_softfail_test')
	lutl:test(T, 'end_hardfail_test')
	lutl:test(T, 'end_interrupt_test')

	lutl:test(T, 'use_parent_test')
end
