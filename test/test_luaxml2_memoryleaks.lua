require 'luaxml2'

local max = 2500
print(string.format("Testing luaxml2.validateRelaxNG() %i times, to check for memory leaks.\n", max))

for i=1, max do
    if 0 == i % 100 then print(string.format("%5.2f%% done, %4i to go.", 100 * (i / max), max - i)) end
    local ok, err = luaxml2.validateRelaxNG(io.open("test/valid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng")
    assert(ok == true, err)
    local ok, err = luaxml2.validateRelaxNG(io.open("test/invalid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng")
    assert(ok == false, "This XML document should not be validated by the schema.")
end

print("\tSuccess!\n")
