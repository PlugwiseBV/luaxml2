--[[
file:       test_luaxml2.lua
author:     JP Florijn (JPF), <c.florijn@plugwise.com>
created:    11/21/2011 08:06:55 PM CET
--]]

require "luaxml2"

print("Testing luaxml2.validateRelaxNG()\n")
local ok, err = luaxml2.validateRelaxNG(io.open("test/valid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng")
assert(ok == true, err)
local ok, err = luaxml2.validateRelaxNG(io.open("test/invalid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng")
assert(ok == false, "This XML document should not be validated by the schema.")

print("\tSuccess!\n")
