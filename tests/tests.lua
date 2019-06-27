lutl = require('lutl')

local mt = getmetatable(lutl)



-- CUSTOM ASSERTS

-- Custom assert that fails if the input file doesn't exactly contain the
-- given content.
function mt.assert_content(lutl, file, content)
	file:seek('set')
	lutl:assert(content == file:read('a'), "File content differs.", 2)
end


-- Custom assert that always fails. Insures that test was interrupted.
function mt.assert_canceled(lutl)
	lutl:fail("Test should have been canceled.")
end



-- TEST FIXTURE

-- Creates a new test context and output file. To be used with lutl:at_start().
function fixture_setup(lutl)
	local fix = {}
	fix.lutl = lutl.new()
	lutl:check(lutl ~= nil, "Could not create test context.")

	fix.output = io.tmpfile()
	lutl:check(fix.output, "Could not make tmp file.")

	lutl:set_data(fix)
	fix.lutl:set_output(fix.output)
	fix.lutl:at_interrupt(function (lutl)
		lutl:fail("Test for interrupted.")
	end)
end


-- Closes test context and output file. To be used with lutl:at_end().
function fixture_clean(lutl)
	local fix = lutl:get_data()
	lutl:check(fix.output ~= nil, "Output file was not opened.")
	if lutl:get_failed() > 0 then
		fix.output:seek('set')
		lutl:message('info', "Test output:\n"..fix.output:read('a'), 0)
	end
	fix.lutl = nil
	fix.output = nil
end
