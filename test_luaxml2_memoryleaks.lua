require 'luaxml2'

function test_xsd_string_true()
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
end

for i=1,10000 do
	test_xsd_string_true()
end

