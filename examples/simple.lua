#! /usr/bin/env lua
-- This simple example can be run with ./simple.lua or with lutl_dofile().
lutl = require('lutl')

function simple_test(lutl)
	lutl:assert_true(34 == 45)
end

lutl:test('simple_test')
if lutl:is_standalone() then os.exit(lutl:summary()) end
