#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "luaxml2.h"

#define TMP_BUF_SIZE 256

// Save error messages by adding them to a luaL_Buffer.
void save_errors(void * buffer, const char * msg, ...) {
    char string[TMP_BUF_SIZE];
    va_list arg_ptr;

    va_start(arg_ptr, msg);
    vsnprintf(string, TMP_BUF_SIZE, msg, arg_ptr);
    va_end(arg_ptr);
    luaL_addstring(buffer, string);
};

// Save warnings by adding them to a luaL_Buffer.
void save_warnings(void * buffer, const char * msg, ...) {
    char string[TMP_BUF_SIZE];
    va_list arg_ptr;

    va_start(arg_ptr, msg);
    vsnprintf(string, TMP_BUF_SIZE, msg, arg_ptr);
    va_end(arg_ptr);
    luaL_addstring(buffer, string);
};

/* Validates XML documents against W3C XML Schemas.
 *
 * Requires two parameters to be on the stack:
 * - [1]: a string containing the XML Document to be validated.
 * - [2]: the filename of the W3C XML Schema to validate against.
 */
static int l_validate (lua_State *L) {
    xmlDocPtr doc; /* the resulting document tree */
    xmlDocPtr schema_doc; /* the XML schema document tree */
    xmlSchemaParserCtxtPtr parser_ctxt; /* the XML schema parser context */
    xmlSchemaPtr schema; /* the parsed XML schema */
    xmlSchemaValidCtxtPtr valid_ctxt; /* the XML schema validation context */

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
    xmlSchemaSetParserErrors(parser_ctxt, &save_errors, &save_warnings, &buf);

    schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        return luaL_error(L, "The schema itself is not valid.");
    }

    valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        return luaL_error(L, "Unable to create a validation context for the schema.");
    }

    // Set error handler.
    xmlSchemaSetValidErrors(valid_ctxt, &save_errors, &save_warnings, &buf);

    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    xmlFreeDoc(doc);
    // Push results: (boolean), (string)
    lua_pushboolean(L, is_valid);
    luaL_pushresult(&buf);
    return 2;
}

static const struct luaL_reg luaxml2[] = {
    {"validate", l_validate},
    {NULL, NULL}    /* sentinel */
};

int luaopen_luaxml2(lua_State *L) {
    luaL_register(L, "luaxml2", luaxml2);
    return 1;
};
