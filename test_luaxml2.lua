--[[
file:       test_libxml2.lua
author:     JP Florijn (JPF), <c.florijn@plugwise.com>
created:    11/21/2011 08:06:55 PM CET
--]]

require"luaxml2"
require"pwutil.test.luaunit"
local assertIs = require"pwutil.type.assertIs"

TestLuaXML2 = {}

function TestLuaXML2:test_xsd_file_true()
	local ok, err = luaxml2.validateXSD(io.open("test/example_xml.xml", "r"):read("*all"), "test/example_schema.xsd") 
	assertIs.True(ok, err)
end

function TestLuaXML2:test_xsd_file_false()
	local ok, err = luaxml2.validateXSD(io.open("test/fault_xml.xml", "r"):read("*all"), "test/example_schema.xsd")
	assertIs.False(ok, "This XML document should not be validated by the schema.")
end

function TestLuaXML2:test_ng_file_true()
	local ok, err = luaxml2.validateRelaxNG(io.open("test/valid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng") 
	assertIs.True(ok, err)
end

function TestLuaXML2:test_ng_file_false()
	local ok, err = luaxml2.validateRelaxNG(io.open("test/invalid_address_book.xml", "r"):read("*all"), "test/address_book_pattern.rng")
	assertIs.False(ok, "This XML document should not be validated by the pattern.")
end

function TestLuaXML2:test_xsd_string_true()
	local xsd = [[<?xml version="1.0"?>
	<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
	<xs:element name="wifi">
	  <xs:complexType>
	    <xs:sequence>
	      <xs:element name="mode" type="xs:string"/>
	      <xs:element name="ssid" type="xs:string"/>
	      <xs:element name="encryption">
		<xs:complexType>
		  <xs:sequence>
		    <xs:element name="method" type="xs:string"/>
		    <xs:element name="key" type="xs:string"/>
		  </xs:sequence>
		</xs:complexType>
	      </xs:element>
	    </xs:sequence>
	  </xs:complexType>
	</xs:element>
	</xs:schema>
	]]
	
	local xml = [[<?xml version="1.0" encoding="utf-8"?>
	<wifi>
	  <mode>sta</mode>
	  <ssid>smile</ssid>
	  <encryption>
	    <method>wpa2</method>
	    <key>somekeythatslongenough</key>
	  </encryption>
	</wifi>
	]]
	
	local ok, err = luaxml2.validateXSDString(xml, xsd) 
	assertIs.True(ok, err)
end

function TestLuaXML2:test_xsd_string_false()
	local xsd = [[<?xml version="1.0"?>
	<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
	<xs:element name="wifi">
	  <xs:complexType>
	    <xs:sequence>
	      <xs:element name="mode" type="xs:string"/>
	      <xs:element name="ssid" type="xs:string"/>
	      <xs:element name="encryption">
		<xs:complexType>
		  <xs:sequence>
		    <xs:element name="method" type="xs:string"/>
		    <xs:element name="key" type="xs:string"/>
		  </xs:sequence>
		</xs:complexType>
	      </xs:element>
	    </xs:sequence>
	  </xs:complexType>
	</xs:element>
	</xs:schema>
	]]
	
	local xml = [[<?xml version="1.0" encoding="utf-8"?>
	<wifi>
	  <ssid>smile</ssid>
	  <encryption>
	    <method>wpa2</method>
	    <key>somekeythatslongenough</key>
	  </encryption>
	</wifi>
	]]
	
	local ok, err = luaxml2.validateXSDString(xml, xsd) 
	assertIs.False(ok, "This XML document should not be validated by the schema.")
end

LuaUnit:run()

