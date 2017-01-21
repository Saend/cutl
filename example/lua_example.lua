-- LUTL - A simple Lua unit testing library.
-- AUTHOR: Baptiste "Saend" CEILLIER
-- OVERVIEW: This is a simple example showing the basic functions.
-- This file can be run standalone or from C.
require('lutl')


-- TEST EXAMPLE

function empty_test()
	lutl.info("This is the simplest test possible. It prints this message.")
	lutl.info("And since it doesn't assert anything, it won't ever fail.")
end

function simple_test()
	condition = true
	lutl.assert(
		condition, 
		"Since the condition is true, the test doesn't fail."
	)
	lutl.info("An assert which doesn't fail is never displayed.")
end


-- FAIL EXAMPLES

function failure1()
	condition = false
	lutl.assert(
		condition,
		"Since the condition is false this assert and test fail."
	)

	lutl.info("The function is stoppped after the failed assert.")
	lutl.info("That's why these lines are never reached.")
end

function failure2()
	lutl.fail("This will always fail.")
	lutl.info("This line will never be reached.")
end

function failure3()
	lutl.message(lutl.FAIL, "This message causes the test to be marked as failed.")
	lutl.info("But the execution of the test continues.")
end

function failure_suite()
	lutl.test('failure1')
	lutl.test('failure2')
	lutl.test('failure3')
end


-- TESTING DATA

function echo(str)
	lutl.assert(str, "This function needs data.")
	lutl.info(str)
end

function datatesting_suite()
	lutl.test('echo', "Echo")
	lutl.test('echo')
end

-- MAIN PART

lutl.test('empty_test')
lutl.test('simple_test')
lutl.suite('failure_suite')
lutl.suite('datatesting_suite')

-- If this file was loaded from the C library, then let it show the summary
if (package.loaded.lutl == 'Lua') then lutl.summary() end
