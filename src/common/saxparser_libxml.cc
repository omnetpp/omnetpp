//==========================================================================
//  SAXPARSER_LIBXML.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include "stringutil.h"
#include "fileutil.h"
#include "saxparser_libxml.h"

#ifdef WITH_LIBXML

namespace omnetpp {
namespace common {

#if LIBXML_VERSION < 20600
#error At least libXML2 v2.6 is required for XML DTD validation support.
#endif


void LibxmlSaxParser::generateSAXEvents(xmlNode *node, SaxHandler *sh)
{
    nodeLine = node->line;
    switch (node->type) {
        case XML_ELEMENT_NODE: {
            // collect attributes for startElement()
            int numAttrs = 0;
            xmlAttr *attr;
            for (attr = node->properties; attr; attr = (xmlAttr *)attr->next)
                numAttrs++;
            const char **attrs = new const char *[2*(numAttrs+1)];
            int k;
            for (attr = node->properties, k = 0; attr; attr = (xmlAttr *)attr->next, k += 2) {
                // ignore namespaces: pass "prefix:name" to SAX handler
                if (attr->ns) {
                    attrs[k] = new char[strlen((const char *)attr->name)+strlen((const char *)attr->ns->prefix)+2];
                    sprintf((char *)attrs[k], "%s:%s", attr->ns->prefix, attr->name);
                }
                else {
                    attrs[k] = (const char *)attr->name;
                }
                attrs[k+1] = (const char *)attr->children->content;  // first text node within attr
            }
            attrs[k] = nullptr;
            attrs[k+1] = nullptr;

            // element name. ignore namespaces: pass "prefix:name" to SAX handler
            char *nodename;
            if (node->ns) {
                nodename = new char[strlen((const char *)node->name)+strlen((const char *)node->ns->prefix)+2];
                sprintf(nodename, "%s:%s", node->ns->prefix, node->name);
            }
            else {
                nodename = (char *)node->name;
            }

            // invoke startElement()
            sh->startElement(nodename, attrs);

            // dealloc prefixed attr names and element name
            for (attr = node->properties, k = 0; attr; attr = (xmlAttr *)attr->next, k += 2)
                if (attr->ns)
                    delete[] (char *)attrs[k];

            delete[] attrs;
            if (node->ns)
                delete[] nodename;

            // recursive processing of children
            xmlNode *child;
            for (child = node->children; child; child = child->next)
                generateSAXEvents(child, sh);

            // invoke endElement()
            sh->endElement((const char *)node->name);
            break;
        }

        case XML_TEXT_NODE: {
            const char *content = (const char *)node->content;
            if (!discardIgnorableWhiteSpace || !opp_isblank(content))
                sh->characterData(content, strlen(content));
            break;
        }

        case XML_PI_NODE:  // TODO apparently, such nodes don't occur in the DOM tree...
            sh->processingInstruction((const char *)node->name,(const char *)node->content);
            break;

        case XML_COMMENT_NODE:
            break;  // ignore

        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_ATTRIBUTE_NODE:
            // should not occur (see XML_PARSE_xxx options)
            fprintf(stderr, "ERROR: libxml wrapper: generateSAXEvents(): node type %d unexpected\n", node->type);
            assert(0);
            break;

        default:
            // DTD stuff: ignore
            break;
    }
}

void LibxmlSaxParser::parseFile(const char *filename)
{
    doParse(filename, nullptr);
}

void LibxmlSaxParser::parseContent(const char *content)
{
    doParse(nullptr, content);
}

void LibxmlSaxParser::doParse(const char *filename, const char *content)
{
    Assert((filename == nullptr) != (content == nullptr));  // exactly one of them is non-nullptr

    if (filename && !fileExists(filename))
        throw opp_runtime_error("File not found: '%s'", filename);

    //
    // When there's a DTD given, we *must* use it, and complete default attrs from it.
    //
    // Strategy: build DOM tree with validation enabled, then generate SAX events
    // from it. LibXML's SAX2-based validation code isn't robust enough yet
    // (as of 09/2004)
    //
    ctxt = xmlNewParserCtxt();
    if (!ctxt)
        throw opp_runtime_error("Failed to allocate parser context");

    // parse the file
    unsigned options =
         XML_PARSE_DTDVALID  // validate with the DTD
        |XML_PARSE_DTDATTR  // complete default attributes from DTD
        |XML_PARSE_NOENT  // substitute entities
        |XML_PARSE_NONET  // forbid network access
        |XML_PARSE_NOBLANKS  // discard ignorable white space
        |XML_PARSE_NOCDATA  // merge CDATA as text nodes
        |XML_PARSE_NOERROR  // suppress error reports -- needed in order to suppress stupid "Validation failed: no DTD found !" messages when document in fact contains no DOCTYPE
        |XML_PARSE_NOWARNING;  // suppress warning reports
        ;

    xmlDocPtr doc;
    if (filename)
        doc = xmlCtxtReadFile(ctxt, filename, nullptr, options);
    else
        doc = xmlCtxtReadMemory(ctxt, content, strlen(content), "string-content", nullptr, options);

    filename = opp_emptytodefault(filename, "string-content"); // simplify error reporting

    // check if parsing succeeded
    if (!doc) {
        const char *message = opp_emptytodefault(ctxt->lastError.message, "Document is empty"); // lastError appears to be unfilled when the document was empty
        opp_runtime_error ex("Parse error: %s at %s:%d", opp_trim(message).c_str(), filename, ctxt->lastError.line);
        xmlFreeParserCtxt(ctxt);
        throw ex;
    }

    // Note: validation error (ctxt->valid=false) is reported if the doc doesn't even have a DTD!
    bool hasDtd = doc->intSubset || doc->extSubset;
    if (hasDtd && !ctxt->valid) {
        // validate again, this time printing the error messages
        xmlValidCtxtPtr vctxt = xmlNewValidCtxt();
        xmlValidateDocument(vctxt, doc);
        xmlFreeValidCtxt(vctxt);
        xmlFreeParserCtxt(ctxt);
        xmlFreeDoc(doc);
        throw opp_runtime_error("'%s': DTD validation failed, see stderr for detailed report", filename);
    }

    // traverse tree and generate SAX events
    try {
        xmlNode *root = xmlDocGetRootElement(doc);
        generateSAXEvents(root, saxHandler);
    }
    catch (std::exception&) {
        xmlFreeParserCtxt(ctxt);
        xmlFreeDoc(doc);
        throw;
    }

    // free parser context and document tree
    xmlFreeParserCtxt(ctxt);
    xmlFreeDoc(doc);
}

int LibxmlSaxParser::getCurrentLineNumber()
{
    return nodeLine;
}

}  // namespace common
}  // namespace omnetpp

#endif //WITH_LIBXML
