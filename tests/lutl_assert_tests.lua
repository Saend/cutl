require('tests/tests')

local T = {}



-- ASSERT_TRUE

function T.assert_true_true_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_true(true)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_true_number_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_true(0)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_true_string_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_true('foo')
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_true_table_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_true({})
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_true_userdata_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_true(lutl)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_true_false_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_true(false)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_true_nil_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_true(nil)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end



-- ASSERT_FALSE

function T.assert_false_false_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_false(false)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_false_nil_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_false(nil)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_false_true_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_false(true)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_false_number_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_false(0)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_false_string_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_false('foo')
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_false_table_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_false({})
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_false_userdata_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_false(lutl)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end



-- ASSERT_EQUAL

function T.assert_equal_numbers_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal(10, 6+4)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_equal_strings_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal('foobar', 'foo'..'bar')
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_equal_userdata_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal(lutl, lutl)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_equal_tables_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal({}, {})
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_equal_numbers2_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal(11, 6+4)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_equal_strings2_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal('foobar', 'Foo'..'bar')
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_equal_number_string_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal(36, '36')
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_equal_number_bool_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal(0, false)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_equal_nil_false_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_equal(nil, false)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end




-- ASSERT_NIL

function T.assert_nil_nil_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_nil(nil)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_nil_undefined_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_nil(something)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_nil_false_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_nil(false)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_nil_number_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_nil(0)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_nil_string_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_nil('foo')
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_nil_table_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_nil({})
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_nil_userdata_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_nil(lutl)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end



-- ASSERT_NOT_NIL

function T.assert_notnil_false_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_notnil(false)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_notnil_number_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_notnil(0)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_notnil_string_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_notnil('foo')
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_notnil_table_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_notnil({})
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_notnil_userdata_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_notnil(lutl)
	end)

	-- Asserts
	lutl:assert_equal(fix.lutl:get_failed(), 0)
end

function T.assert_notnil_nil_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_notnil(nil)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end

function T.assert_notnil_undefined_test(lutl, fix)
	-- Function under test
	fix.lutl:run(nil, function(lutl)
		lutl:assert_notnil(something)
	end)

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_equal(fix.lutl:get_failed(), 1)
end




-- ASSERT SUITE

return function(lutl)
	lutl:at_start(fixture_setup)
	lutl:at_end(fixture_clean)

	lutl:test(T, 'assert_true_true_test')
	lutl:test(T, 'assert_true_number_test')
	lutl:test(T, 'assert_true_string_test')
	lutl:test(T, 'assert_true_table_test')
	lutl:test(T, 'assert_true_userdata_test')
	lutl:test(T, 'assert_true_false_test')
	lutl:test(T, 'assert_true_nil_test')

	lutl:test(T, 'assert_false_false_test')
	lutl:test(T, 'assert_false_nil_test')
	lutl:test(T, 'assert_false_true_test')
	lutl:test(T, 'assert_false_number_test')
	lutl:test(T, 'assert_false_string_test')
	lutl:test(T, 'assert_false_table_test')
	lutl:test(T, 'assert_false_userdata_test')

	lutl:test(T, 'assert_equal_numbers_test')
	lutl:test(T, 'assert_equal_strings_test')
	lutl:test(T, 'assert_equal_userdata_test')
	lutl:test(T, 'assert_equal_tables_test')
	lutl:test(T, 'assert_equal_numbers2_test')
	lutl:test(T, 'assert_equal_strings2_test')
	lutl:test(T, 'assert_equal_number_string_test')
	lutl:test(T, 'assert_equal_number_bool_test')
	lutl:test(T, 'assert_equal_nil_false_test')

	lutl:test(T, 'assert_nil_nil_test')
	lutl:test(T, 'assert_nil_undefined_test')
	lutl:test(T, 'assert_nil_false_test')
	lutl:test(T, 'assert_nil_number_test')
	lutl:test(T, 'assert_nil_string_test')
	lutl:test(T, 'assert_nil_table_test')
	lutl:test(T, 'assert_nil_userdata_test')

	lutl:test(T, 'assert_notnil_nil_test')
	lutl:test(T, 'assert_notnil_undefined_test')
	lutl:test(T, 'assert_notnil_false_test')
	lutl:test(T, 'assert_notnil_number_test')
	lutl:test(T, 'assert_notnil_string_test')
	lutl:test(T, 'assert_notnil_table_test')
	lutl:test(T, 'assert_notnil_userdata_test')
end
