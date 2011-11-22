--[[
file:       test_libxml2.lua
author:     JP Florijn (JPF), <c.florijn@plugwise.com>
created:    11/21/2011 08:06:55 PM CET
--]]

require "luaxml2"

print("\nTesting luaxml2.validate().\n")

local ok, err = luaxml2.validate(io.open("test/example_xml.xml", "r"):read("*all"), "test/example_schema.xsd") 
assert(ok == true, err)
local ok, err = luaxml2.validate(io.open("test/fault_xml.xml", "r"):read("*all"), "test/example_schema.xsd")
assert(ok == false, "This XML document should not be validated by the schema.")

print("\nAll tests are successful.\n")
