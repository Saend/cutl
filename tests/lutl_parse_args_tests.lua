require('tests/tests')

local T = {}


-- VERBOSITY OPTIONS

-- Verbose output.
function T.verbose_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-v'})

	-- Asserts
	lutl:assert_equal(fix.lutl:get_verbosity(), lutl.VERBOSE)
end


-- Minimal output.
function T.minimal_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-m'})

	-- Asserts
	lutl:assert_equal(fix.lutl:get_verbosity(), lutl.MINIMAL)
end


-- Silent ouput.
function T.silent_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-s'})

	-- Asserts
	lutl:assert_equal(fix.lutl:get_verbosity(), lutl.SILENT)
end



-- COLOR OPTIONS

-- Enable color.
function T.color_on_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-c', 'on'})

	-- Asserts
	lutl:assert_true(fix.lutl:get_color())
end


-- Disable color.
function T.color_off_test(lutl, fix)
	-- Function under test
	fix.lutl:set_color(0)
	fix.lutl:parse_args({'-c', 'off'})

	-- Asserts
	lutl:assert_false(fix.lutl:get_color())
end


-- Auto color.
-- If color autodetection is enabled then color is disabled for files; otherwise
-- color is always disabled. Either way, it's off here.
function T.color_auto_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-c', 'auto'})

	-- Asserts
	lutl:assert_false(fix.lutl:get_color())
end


-- Bad color argument.
function T.color_bad_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-c', 'bad'})

	-- Asserts
	local expected = "[ERROR cutl_parse_args()] Invalid argument for "..
		"option 'c': 'bad'.\n";
	lutl:assert_content(fix.output, expected)

	lutl:assert_true(fix.lutl:get_error())
	lutl:assert_false(fix.lutl:get_color())
end


-- Missing color argument.
function T.color_missing_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-c'})

	-- Asserts
	local expected = "[ERROR cutl_parse_args()] Missing argument for "..
		"option 'c'.\n";
	lutl:assert_content(fix.output, expected)

	lutl:assert_true(fix.lutl:get_error())
	lutl:assert_false(fix.lutl:get_color())
end



-- OUTPUT OPTION

-- Set output file.
function T.output_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-o', os.tmpname()})
	fix.lutl:get_output():write("Message")

	-- Asserts
	lutl:assert_false(fix.lutl:get_error())
	lutl:assert_content(fix.output, "")
end


-- Bad output file.
function T.output_bad_test(lutl, fix)
	-- Setup
	fix.lutl:set_verbosity(lutl.SILENT)

	-- Function under test
	fix.lutl:parse_args({'-o', '/'})
	fix.lutl:get_output():write("Message")

	-- Asserts
	lutl:assert_true(fix.lutl:get_error())
	lutl:assert_content(fix.output, "Message")
end


-- Missing output file.
function T.output_missing_test(lutl, fix)
	-- Function under test
	fix.lutl:parse_args({'-o'})

	-- Asserts
	local expected =  "[ERROR cutl_parse_args()] Missing argument for "..
		"option 'o'.\n"
	lutl:assert_content(fix.output, expected)

	lutl:assert_true(fix.lutl:get_error())
end



-- PARSE ARGS SUITE

return function(lutl)
	lutl:at_start(fixture_setup)
	lutl:at_end(fixture_clean)

	lutl:test(T, 'verbose_test')
	lutl:test(T, 'minimal_test')
	lutl:test(T, 'silent_test')

	lutl:test(T, 'color_on_test')
	lutl:test(T, 'color_off_test')
	lutl:test(T, 'color_auto_test')
	lutl:test(T, 'color_bad_test')
	lutl:test(T, 'color_missing_test')

	lutl:test(T, 'output_test')
	lutl:test(T, 'output_bad_test')
	lutl:test(T, 'output_missing_test')
end
