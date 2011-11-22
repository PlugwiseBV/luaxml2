--[[
file:       test_libxml2.lua
author:     JP Florijn (JPF), <c.florijn@plugwise.com>
created:    11/21/2011 08:06:55 PM CET
--]]

require "luaxml2"

print("\nTesting luaxml2.validateXSD().\n")

local ok, err = luaxml2.validateXSD(io.open("test/example_xml.xml", "r"):read("*all"), "test/example_schema.xsd") 
assert(ok == true, err)
local ok, err = luaxml2.validateXSD(io.open("test/fault_xml.xml", "r"):read("*all"), "test/example_schema.xsd")
assert(ok == false, "This XML document should not be validated by the schema.")

print("\nTesting luaxml2.validateRelaxNG().\n")

local ok, err = luaxml2.validateRelaxNG(io.open("test/valid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng") 
assert(ok == true, err)
local ok, err = luaxml2.validateRelaxNG(io.open("test/invalid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng")
assert(ok == false, "This XML document should not be validated by the pattern.")

--local ok, err = luaxml2.validateRelaxNG(io.open("test/valid_patronA.xml", "r"):read("*all"), "test/patron_schema.rng") 
--assert(ok == true, err)
--local ok, err = luaxml2.validateRelaxNG(io.open("test/valid_patronB.xml", "r"):read("*all"), "test/patron_schema.rng") 
--assert(ok == true, err)
--local ok, err = luaxml2.validateRelaxNG(io.open("test/invalid_patron.xml", "r"):read("*all"), "test/patron_schema.rng")
--assert(ok == false, "This XML document should not be validated by the schema.")

print("\nAll tests are successful.\n")
