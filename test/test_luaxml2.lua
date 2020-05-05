
local max = tonumber(arg[1]) or 1

for i=1, max do
    if max > 1 and 0 == i % 100 then
        print(string.format("\t\t%5.2f%% done, %4i to go.", 100 * (i / max), max - i))

        -- Tear down on every 100th go, to amplify any possible errors caused by lib loading.
        package.loaded.luaxml2 = nil
        luaxml2 = nil
    end
    require 'luaxml2'
    local ok, err = luaxml2.validateRelaxNG(io.open("test/valid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng")
    assert(ok == true, err)
    local ok, err = luaxml2.validateRelaxNG(io.open("test/invalid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng")
    assert(ok == false, "This XML document should not be validated by the schema.")
end

print("\n\tSuccess!\n")
