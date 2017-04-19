#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/relaxng.h>
#include <libxml/xmlerror.h>

#include "luaxml2.h"

#define TMP_BUF_SIZE 256
#define LINE_SIZE 16

/** global pointers to the read XML and validation schema **/
xmlDocPtr doc; /* the resulting document tree */
xmlDocPtr schema_doc; /* the XML schema document tree */

luaL_Buffer buf;

// Structured error handler.
void save_structured_errors(void *L, xmlErrorPtr err) {
    // Initialize a buffer for error messages.
    luaL_buffinit(L, &buf);
    luaL_addvalue(&buf);
    if (err->file) {
        if (strlen(err->file) > 0)
            luaL_addstring(&buf, err->file);
        else
            luaL_addstring(&buf, "(document)");
        if (err->line) {
            char string[LINE_SIZE];
            if (err->int2) {
                snprintf(string, LINE_SIZE, ":%i:%i", err->line, err->int2);
            } else {
                snprintf(string, LINE_SIZE, ":%i", err->line);
            }
            luaL_addstring(&buf, string);
        }
        luaL_addstring(&buf, ": ");
    }
    if (err->message) {
        luaL_addstring(&buf, err->message);
    }
    luaL_pushresult(&buf);
}

/* Validates XML documents against RELAX NG XML Schemas.
 *
 * Requires two parameters to be on the stack:
 * - [1]: a string containing the XML Document to be validated.
 * - [2]: the filename of the W3C XML Schema to validate against.
 */
static int l_validate_relax_ng (lua_State *L) {
    xmlDocPtr doc; /* the resulting document tree */
    xmlDocPtr schema_doc; /* the XML schema document tree */
    xmlRelaxNGParserCtxtPtr parser_ctxt; /* the XML schema parser context */
    xmlRelaxNGPtr schema; /* the parsed XML schema */
    xmlRelaxNGValidCtxtPtr valid_ctxt; /* the XML schema validation context */

    size_t len; /* length of the xml string buffer */
    const char *xml = luaL_checklstring(L, 1, &len); /* xml string buffer */
    const char *schema_filename = luaL_checkstring(L, 2); /* xml schema string buffer */

    doc = xmlReadMemory(xml, len, "", NULL, XML_PARSE_NONET);
    if (doc == NULL) {
        return luaL_error(L, "Invalid parameter #1: XML can't be loaded or is not well-formed:\n%s", xml);
    }

    schema_doc = xmlReadFile(schema_filename, NULL, XML_PARSE_NONET);
    if (schema_doc == NULL) {
        xmlFreeDoc(doc);
        return luaL_error(L, "Invalid parameter #2 (%s): XML Schema can't be loaded or is not well-formed.", schema_filename);
    }

    parser_ctxt = xmlRelaxNGNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL) {
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        return luaL_error(L, "Unable to create a parser context for the schema.");
    }

    // Set error handler.
    lua_settop(L, 0);
    lua_pushstring(L, "");
    xmlRelaxNGSetParserStructuredErrors(parser_ctxt, &save_structured_errors, L);

    schema = xmlRelaxNGParse(parser_ctxt);
    if (schema == NULL) {
        xmlRelaxNGFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        lua_pushstring(L, "The schema itself is not valid: ");
        lua_pushvalue(L, 1);
        lua_remove(L, 1);
        lua_concat(L, 2);
        return lua_error(L);
    }

    valid_ctxt = xmlRelaxNGNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
        xmlRelaxNGFree(schema);
        xmlRelaxNGFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        lua_pushstring(L, "Unable to create a validation context for the schema: ");
        lua_pushvalue(L, 1);
        lua_remove(L, 1);
        lua_concat(L, 2);
        return lua_error(L);
    }

    // Set error handler.
    xmlRelaxNGSetValidStructuredErrors(valid_ctxt, &save_structured_errors, L);

    int is_valid = (xmlRelaxNGValidateDoc(valid_ctxt, doc) == 0);
    xmlRelaxNGFreeValidCtxt(valid_ctxt);
    xmlRelaxNGFree(schema);
    xmlRelaxNGFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    xmlFreeDoc(doc);
    // Push results: (boolean), (string)
    lua_pushboolean(L, is_valid);
    lua_pushvalue(L, 1);
    lua_remove(L, 1);
    return 2;
}

/** Validate an XML string against an XML Schema string
 * @param doc pointer to the xml
 * @param schema_doc pointer to the schema
 * @param L pointer to the lua state for returning the result or errors
 */
static int validate_xsd (xmlDocPtr doc, xmlDocPtr schema_doc, lua_State *L) {
    xmlSchemaParserCtxtPtr parser_ctxt; /* the XML schema parser context */
    xmlSchemaPtr schema; /* the parsed XML schema */
    xmlSchemaValidCtxtPtr valid_ctxt; /* the XML schema validation context */

    parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL) {
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        return luaL_error(L, "Unable to create a parser context for the schema.");
    }

    // Initialize a buffer for error messages.
    luaL_Buffer buf;
    luaL_buffinit(L, &buf);
    // Set error handler.
    lua_settop(L, 0);
    lua_pushstring(L, "");
    xmlSchemaSetParserStructuredErrors(parser_ctxt, &save_structured_errors, L);

    schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        lua_pushstring(L, "The schema itself is not valid: ");
        lua_pushvalue(L, 1);
        lua_remove(L, 1);
        lua_concat(L, 2);
        return lua_error(L);
    }

    valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        lua_pushstring(L, "Unable to create a validation context for the schema: ");
        lua_pushvalue(L, 1);
        lua_remove(L, 1);
        lua_concat(L, 2);
        return lua_error(L);
    }

    // Set error handler.
    xmlSchemaSetValidStructuredErrors(valid_ctxt, &save_structured_errors, L);

    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    xmlFreeDoc(doc);
    // Push results: (boolean), (string)
    lua_pushboolean(L, is_valid);
    lua_pushvalue(L, 1);
    lua_remove(L, 1);
    return 2;
}

/** Validate an XML string against an XML Schema from a file
 *
 * Requires two parameters to be on the stack:
 * @param [1]: a string containing the XML Document to be validated.
 * @param [2]: a string containing the filename of the XML Schema to validate against.
 */
static int l_validate_xsd_file(lua_State *L){
    size_t len; /* length of the xml string buffer */
    const char *xml = luaL_checklstring(L, 1, &len); /* xml string buffer */
    const char *schema_filename = luaL_checkstring(L, 2); /* xml schema string buffer */

    doc = xmlReadMemory(xml, len, "noname.xml", NULL, XML_PARSE_NONET);
    if (doc == NULL) {
        return luaL_error(L, "Invalid parameter #1: XML can't be loaded or is not well-formed.");
    }

    schema_doc = xmlReadFile(schema_filename, NULL, XML_PARSE_NONET);
    if (schema_doc == NULL) {
        xmlFreeDoc(doc);
        return luaL_error(L, "Invalid parameter #2: XML Schema can't be loaded or is not well-formed.");
    }

    return validate_xsd(doc, schema_doc, L);

}

/** Validate an XML string against an XML Schema string
 * Requires two parameters to be on the stack:
 * @param [1]: a string containing the XML Document to be validated.
 * @param [2]: a string containing the XML Schema to validate against.
 */
static int l_validate_xsd_string(lua_State *L){
	size_t xml_len; /* length of the xml string buffer */
	size_t schema_len; /* length of the schema string buffer */
	const char *xml = luaL_checklstring(L, 1, &xml_len); /* xml string buffer */
	const char *schema = luaL_checklstring(L, 2, &schema_len); /* xml schema string buffer */

	doc = xmlReadMemory(xml, xml_len, "noname.xml", NULL, XML_PARSE_NONET);
	if (doc == NULL) {
	    return luaL_error(L, "Invalid parameter #1: XML can't be loaded or is not well-formed.");
	}

	schema_doc = xmlReadMemory(schema, schema_len, "noname.xml", NULL, XML_PARSE_NONET);
	if (schema_doc == NULL) {
	    xmlFreeDoc(doc);
	    return luaL_error(L, "Invalid parameter #2: XML Schema can't be loaded or is not well-formed.");
	}

	return validate_xsd(doc, schema_doc, L);
}

static const struct luaL_reg luaxml2[] = {
    {"validateXSD", l_validate_xsd_file},
    {"validateXSDString", l_validate_xsd_string},
    {"validateRelaxNG", l_validate_relax_ng},
    {NULL, NULL}    /* sentinel */
};

int luaopen_luaxml2(lua_State *L) {
    luaL_register(L, "luaxml2", luaxml2);
    return 1;
};
